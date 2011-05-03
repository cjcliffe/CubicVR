/*
    This file is part of CubicVR.

    Copyright (C) 2003 by Charles J. Cliffe

		Permission is hereby granted, free of charge, to any person obtaining a copy
		of this software and associated documentation files (the "Software"), to deal
		in the Software without restriction, including without limitation the rights
		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		copies of the Software, and to permit persons to whom the Software is
		furnished to do so, subject to the following conditions:

		The above copyright notice and this permission notice shall be included in
		all copies or substantial portions of the Software.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
		THE SOFTWARE.
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <queue>
#include <vector>

#include <CubicVR/cvr_defines.h>
#include <CubicVR/map_string.h>
#include <CubicVR/Motion.h>
#include <CubicVR/ResourceManager.h>
#include <CubicVR/Resource.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

using namespace std;

#include <lua.hpp>

//#include "resourceManager.h"
//#include "scriptingExports.h"

#ifdef check
#undef check
#endif

template <typename T> class Luna {
	typedef struct { T *pT; } userdataType;
public:
	typedef int (T::*mfp)(lua_State *L);
	typedef struct { const char *name; mfp mfunc; } RegType;
	
	static void Register(lua_State *L) {
        lua_newtable(L);
        int methods = lua_gettop(L);
		
        luaL_newmetatable(L, T::className);
        int metatable = lua_gettop(L);
		
        // store method table in globals so that
        // scripts can add functions written in Lua.
        lua_pushstring(L, T::className);
        lua_pushvalue(L, methods);
        lua_settable(L, LUA_GLOBALSINDEX);
		
        lua_pushliteral(L, "__metatable");
        lua_pushvalue(L, methods);
        lua_settable(L, metatable);  // hide metatable from Lua getmetatable()
		
        lua_pushliteral(L, "__index");
        lua_pushvalue(L, methods);
        lua_settable(L, metatable);
		
        lua_pushliteral(L, "__tostring");
        lua_pushcfunction(L, tostring_T);
        lua_settable(L, metatable);
		
        lua_pushliteral(L, "__gc");
        lua_pushcfunction(L, gc_T);
        lua_settable(L, metatable);
		
        lua_newtable(L);                // mt for method table
        int mt = lua_gettop(L);
        lua_pushliteral(L, "__call");
        lua_pushcfunction(L, new_T);
        lua_pushliteral(L, "new");
        lua_pushvalue(L, -2);           // dup new_T function
        lua_settable(L, methods);       // add new_T to method table
        lua_settable(L, mt);            // mt.__call = new_T
        lua_setmetatable(L, methods);
		
        // fill method table with methods from class T
        for (RegType *l = T::methods; l->name; l++) {
			/* edited by Snaily: shouldn't it be const RegType *l ... ? */
			lua_pushstring(L, l->name);
			lua_pushlightuserdata(L, (void*)l);
			lua_pushcclosure(L, thunk, 1);
			lua_settable(L, methods);
        }
		
        lua_pop(L, 2);  // drop metatable and method table
	}
	
	// get userdata from Lua stack and return pointer to T object
	static T *check(lua_State *L, int narg) {
        userdataType *ud =
		static_cast<userdataType*>(luaL_checkudata(L, narg, T::className));
        if(!ud) luaL_typerror(L, narg, T::className);
        return ud->pT;  // pointer to T object
	}
	
private:
	Luna();  // hide default constructor
	
	// member function dispatcher
	static int thunk(lua_State *L) {
        // stack has userdata, followed by method args
        T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
        lua_remove(L, 1);  // remove self so member function args start at index 1
        // get member function from upvalue
        RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
        return (obj->*(l->mfunc))(L);  // call member function
	}
	
	// create a new T object and
	// push onto the Lua stack a userdata containing a pointer to T object
	static int new_T(lua_State *L) {
        lua_remove(L, 1);   // use classname:new(), instead of classname.new()
        T *obj = new T(L);  // call constructor for T objects
        userdataType *ud =
		static_cast<userdataType*>(lua_newuserdata(L, sizeof(userdataType)));
        ud->pT = obj;  // store pointer to object in userdata
        luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry
        lua_setmetatable(L, -2);
        return 1;  // userdata containing pointer to T object
	}
	
	// garbage collection metamethod
	static int gc_T(lua_State *L) {
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
        T *obj = ud->pT;
        delete obj;  // call destructor for T objects
        return 0;
	}
	
	static int tostring_T (lua_State *L) {
        char buff[32];
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
        T *obj = ud->pT;
        sprintf(buff, "%p", obj);
        lua_pushfstring(L, "%s (%s)", T::className, buff);
        return 1;
	}
};


#define lua_method(class, name) {#name, &class::name}


typedef struct luaMemFile
{
  char *buffer;
  size_t size;
} luaMemFile;

class Binder
{
public:
	Binder(lua_State *L);
	void init (const char *tname, const luaL_reg* flist);
	
	void pushnumber(double v);
	double checknumber(int index);
	
	void pushstring(const char *s);
	const char* checkstring(int index);
	
	void pushusertype(void*udata, const char* tname);	
	void * checkusertype(int index, const char* tname);
	
private: 
	lua_State* L;
};


class Script : public Resource
{
public:
	Script();
	Script(string scriptName);
	
	~Script();
	
	// Resource Management
	static Resource *upcast(ResourceManager *rm_in, Resource *res_in);
	
	virtual bool onLoad();
	virtual bool onSave();

	string &getScriptId();
	void setScriptId(const string &m_id);
	
	string &getScriptType();
	void setScriptType(const string &m_type);
	
	vector<const char *> getErrors();
	
	void LoadLibraries();
	int LoadUserFunctions();
	bool AddUserFunction(std::string luaName, int (*foo)(lua_State *));

	int LoadScript(string scriptName);
	void CloseScript();

// TODO: Finish the two functions below
//	void BindFunction(char * luaFuncExportName, int * funcName );
	int CallScriptFunc(char *command);
	
	lua_State *GetVM();
	
	int RunCommand(const char *command);
	int Run();
	
private:
	int Execute();
	std::string mScriptFilename;
	
	luaMemFile 	mScriptFile;	
	lua_State 	*L;
	
	vector<const char *> queueErrors;
};

typedef std::map<std::string, Script> ScriptMap;

class ScriptingSystem
{
public:
	ScriptingSystem()
	{
		mImmediateMode = new Script();
	}
	
	Script *mImmediateMode;
	ScriptMap *mScripts;
};