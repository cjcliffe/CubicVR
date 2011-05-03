#include "Script.h"
#include <CubicVR/Logger.h>


Binder::Binder(lua_State *lState)
{
	L = lState;
}

void Binder::init (const char *tname, const luaL_reg* flist)
{
	luaL_register(L,tname,flist);
}

void Binder::pushnumber(double v)
{
	lua_pushnumber(L, v);
}

double Binder::checknumber(int index)
{
	return luaL_checknumber(L, index);
}

void Binder::pushstring(const char *s)
{
	//lua_pushstring(L, s);	
}

const char* Binder::checkstring(int index)
{
	//return lua_checkstring(L, index);
	return "";
}

void Binder::pushusertype(void*udata, const char* tname)
{
	lua_pushlightuserdata(L,udata);
}

void * Binder::checkusertype(int index, const char* tname)
{
	void *udata = lua_touserdata(L, index);
	if(udata == 0)
	{
		//luaL_typeerror(L, index, tname);
	}
	return udata;
}

//////////////////////////////////////////////////////////////////////////////////

Script::Script()
{	
	Resource::typeName = std::string("script");
	
	// init Lua and load all libraries
	L = lua_open();	
	LoadLibraries();
}

Script::Script(string scriptName)
{
	Resource::typeName = std::string("script");
	
	// init Lua and load all libraries
	L = lua_open();
	LoadLibraries();
	
	LoadScript(scriptName);	
}

Script::~Script()
{
	CloseScript();
}

Resource *Script::upcast(ResourceManager *rm_in, Resource *res_in)
{
	Script *new_script = new Script();
	new_script->Import(*res_in);
	new_script->onLoad();
	return new_script;
}

bool Script::onLoad() 
{
	if (properties->rootNode().hasAnother("script_filename")) properties->rootNode().child("script_filename").element().get(mScriptFilename);
	
	LoadScript(mScriptFilename);
	
	return true;
};

bool Script::onSave() 
{
	if (properties) delete properties;
	
	properties = new DataTree();
	properties->rootNode().newChild("script_filename").element().set(mScriptFilename);
	//properties->rootNode().newChild("script_id").element().set(scriptId);
	
	return true;
};

void Script::CloseScript()
{
	if(L)		lua_close(L);
}

void Script::LoadLibraries()
{
	luaL_openlibs(L);
	
	// register our own functions
	//lua_register(L, "print", 					lua_print);
	//lua_register(L, "printf", 					lua_print);
}

bool Script::AddUserFunction(std::string luaName, int (*foo)(lua_State *))
{
	lua_register(L, luaName.c_str(), foo);
	return true;
}

int Script::LoadUserFunctions()
{
	
}

int Script::LoadScript(string scriptName)
{
	printf("Loading script filename: %s\n", scriptName.c_str());
	FILE * pFile;
		
	//pFile = fopen (ResourceFilename(scriptName).c_str() , "rb");
	pFile = fopen (scriptName.c_str() , "rb");
	
	if(pFile != NULL)
	{
		fseek (pFile , 0 , SEEK_END);
		mScriptFile.size = ftell (pFile);
		rewind (pFile);
		
		mScriptFile.buffer = (char*) malloc (mScriptFile.size);
		if(mScriptFile.buffer != NULL)
		{
			fread(mScriptFile.buffer, 1, mScriptFile.size, pFile);
		}
	}
	fclose(pFile);
	
	printf("Script Data: %s\n", mScriptFile.buffer);
	
	mScriptFilename = scriptName;
	
	return 1;
}

int Script::Execute()
{
	if(L)
	{
		// show error, if any
		if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
		{
			//errors.insert(TMapErrorsPair(0, lua_tostring(L, -1)));
			queueErrors.push_back(lua_tostring(L, -1));
			
			Logger::log(LOG_ERROR,"Lua error: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);  // remove error message
			
			return 0;
		}
		
		return 1;
	}
	
	return 0;
}

vector<const char *> Script::getErrors()
{
	return queueErrors;
}

lua_State * Script::GetVM()
{
	return L;
}
//void Script::BindFunction(char * luaFuncExportName, void * funcName )
//{
//	lua_register(L, luaFuncExportName, funcName);
//}

int Script::CallScriptFunc(char *command)
{
	//printf("Attempting script call: %s\n", command);
	
	lua_getglobal(L, command);  
	if(!lua_isfunction(L,-1))  
	{  
	   lua_pop(L,1);  
	   return -1;  
	}  
	
//	lua_pushnumber(L, 21);   /* push 1st argument */  
//	lua_pushnumber(L, 31);   /* push 2nd argument */  

	/* do the call (2 arguments, 1 result) */  
	//int numVars = 2;
	//int numResults = 1;
	//if (lua_pcall(L, numVars, numResults, 0) != 0) {  
	if (lua_pcall(L, 0, 0, 0) != 0)
	{  
		Logger::log(LOG_ERROR,"Lua Error running function %s: %s\n", command, lua_tostring(L, -1));  
		return -1;  
	}  
   
	//printf("Call ran: %s\n", command);
	
	return 1;
//// retrieve result
//if (!lua_isnumber(L, -1)) {  
//		printf("function `f' must return a number\n");  
//		return -1;  
//	}  
//	z = lua_tonumber(L, -1);  
//	printf("Result: %f\n",z);  
//	lua_pop(L, 1);  
}

int Script::RunCommand(const char *command)
{
	int status = luaL_loadstring(L, command);
	
	if(status == 0 && L) return Execute();
		
	return 0;
}

int Script::Run()
{			
	return RunCommand(mScriptFile.buffer);
}

