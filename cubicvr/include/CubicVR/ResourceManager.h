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

#include <CubicVR/Resource.h>
#include <CubicVR/ResourceManager.h>
#include <CubicVR/ResourcePool.h>
#include <CubicVR/map_string.h>

typedef std::map<std::string, Resource*, string_less> ResourceTypeSet;
typedef std::map< std::string, std::map<std::string, Resource*, string_less>, string_less> ResourceSet;

class ResourceManager;

typedef ResourceManager * ResourceManagerPtr;
typedef Resource * ResourcePtr;
typedef ResourcePtr (*ResourceCast)(ResourceManagerPtr,ResourcePtr);

class ResourceManagerInit
{
public:
	ResourceManagerInit();	
	~ResourceManagerInit();
};


class ResourceManager
{
private:
	ResourceSet *mResources;
	ResourceSet *mSubResources;

	std::map<Resource *,Resource *> outcastBin;
	std::set<Resource *> resourceBin;
	std::set<Resource *> upcastBin;
	std::map<std::string, ResourcePool *> mFilePool;	
	
	// static class init causes initilization at runtime
	static ResourceManagerInit function_initializer;
	static std::map<std::string, ResourceCast, string_less> upcastFunc;
	
	
public:
	ResourceManager();
	~ResourceManager();

	static void addCastFunc(std::string cast_type, ResourceCast cast_func);
	
	void loadResourcePool(string poolName);
	
	bool hasPool(string poolName);	
	bool hasResource(string typeId, string idName);
	bool hasSubResource(string typeId, string idName);
	bool hasCast(string typeId, string idName);	
	
	ResourcePool &pool(string poolName);
	
	void addResource(Resource &res_in, bool final=false);
	Resource *removeResource(Resource &res_in);
	
	Resource &getResource(string typeId, string idName);
	ResourceSet &getResources();
	ResourceTypeSet &getResources(string typeId);

	Resource &getSubResource(string typeId, string idName);
	ResourceSet &getSubResources();
	ResourceTypeSet &getSubResources(string typeId);
	
	Resource *upcast(Resource *res_in);			
};



