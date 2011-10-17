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

#include <CubicVR/ResourceManager.h>
#include <CubicVR/Resource.h>
#include <CubicVR/Mesh.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/SceneObjects/Vehicle.h>

std::map<std::string, ResourceCast, string_less> ResourceManager::upcastFunc;

ResourceManagerInit ResourceManager::function_initializer = ResourceManagerInit();


ResourceManagerInit::ResourceManagerInit()
{
	// Initialize known casting functions
	ResourceManager::addCastFunc("model",Mesh::upcast);
	ResourceManager::addCastFunc("sceneobject",SceneObject::upcast);
	ResourceManager::addCastFunc("sceneobject.vehicle",Vehicle::upcast);
}

ResourceManagerInit::~ResourceManagerInit()
{
	// nil
}

void ResourceManager::loadResourcePool(string poolName)
{
	std::vector<std::string> projectFiles;
	std::vector<std::string>::iterator projectFiles_i;
	
	pool(poolName).findByExtension("xml", projectFiles);
	
	for (projectFiles_i = projectFiles.begin(); projectFiles_i != projectFiles.end(); projectFiles_i++)
	{
		Resource *tmpResource;
		
		tmpResource = new Resource();
		tmpResource->LoadFromFileXML((*projectFiles_i).c_str());
		addResource(*tmpResource);
		
		Logger::log("Project File: %s\n",(*projectFiles_i).c_str());
	}		
};


ResourceManager::ResourceManager()
{
	mResources = new ResourceSet;	
	mSubResources = new ResourceSet;
}

ResourceManager::~ResourceManager()
{
	ResourceSet::iterator type_i;
	ResourceTypeSet::iterator set_i;
	
	for (type_i = getResources().begin(); type_i != getResources().end(); type_i++)
	{
		for (set_i = (*type_i).second.begin(); set_i != (*type_i).second.end(); set_i++)
		{
			delete (*set_i).second;
		}
	}
	
	delete mResources;
	delete mSubResources;
}



Resource *ResourceManager::upcast(Resource *res_in) 
{
	Resource *res_out;

	if (!res_in)
	{
		return NULL;
	}
	
	if (outcastBin.find(res_in) != outcastBin.end()) return outcastBin[res_in];
	
	if (upcastBin.find(res_in) != upcastBin.end())
	{
		return res_in;
	}
	
	
	if (!hasResource(res_in->getType(),res_in->getId()))
	{
		return NULL;
	}			
	
	if (upcastFunc.find(res_in->getType()) == upcastFunc.end())
	{
		if (upcastFunc.find(res_in->getSubType()) == upcastFunc.end())
		{
			return NULL;			
		}
	}
	
	(*mResources)[res_in->getType()].erase(res_in->getId());
	(*mSubResources)[res_in->getSubType()].erase(res_in->getId());
	
	ResourceCast castFunc;
	
	if (upcastFunc.find(res_in->getSubType()) != upcastFunc.end())
	{
		castFunc = upcastFunc[res_in->getSubType()];
	}
	else
	{
		castFunc = upcastFunc[res_in->getType()];
	}
	
	res_out = castFunc(this,res_in);
	
	if (!res_out) return NULL;
	
	res_in->UnLink();
	
	outcastBin[res_in] = res_out;

	addResource(*res_out);
	
	upcastBin.insert(res_out);
	
	return res_out;
}



ResourcePool &ResourceManager::pool(string poolName) 
{
	if (mFilePool.find(poolName) == mFilePool.end())
	{
		mFilePool[poolName] = new ResourcePool();
	}
	
	return *mFilePool[poolName];
}


bool ResourceManager::hasPool(string poolName)
{
	if (mFilePool.find(poolName) == mFilePool.end())
	{
		return false;
	}
	
	return true;
}

bool ResourceManager::hasResource(string typeId, string idName)
{
	if ((*mResources).find(typeId) == (*mResources).end()) return false;
	if ((*mResources)[typeId].find(idName) == (*mResources)[typeId].end()) return false;
	
	return true;
}

bool ResourceManager::hasSubResource(string typeId, string idName)
{
	if ((*mSubResources).find(typeId) == (*mSubResources).end()) return false;
	if ((*mSubResources)[typeId].find(idName) == (*mSubResources)[typeId].end()) return false;
	
	return true;
}


bool ResourceManager::hasCast(string typeId, string idName)
{
	if (!hasResource(typeId,idName)) return false;
	
	if (upcastBin.find((*mResources)[typeId][idName]) == upcastBin.end())
	{
		return false;
	}
	
	return true;
}


void ResourceManager::addCastFunc(std::string cast_type, ResourceCast cast_func)
{
	ResourceManager::upcastFunc[cast_type] = cast_func;
}


Resource &ResourceManager::getResource(string typeId, string idName)
{
	return *(*mResources)[typeId][idName];
}

Resource &ResourceManager::getSubResource(string typeId, string idName)
{
	return *(*mSubResources)[typeId][idName];
}


ResourceTypeSet &ResourceManager::getResources(string typeId)
{
	return (*mResources)[typeId];
}

ResourceTypeSet &ResourceManager::getSubResources(string typeId)
{
	return (*mSubResources)[typeId];
}


ResourceSet &ResourceManager::getResources()
{
	return *mResources;
}

ResourceSet &ResourceManager::getSubResources()
{
	return *mSubResources;
}


void ResourceManager::addResource(Resource &res_in, bool final)
{
	std::string id_in;
	std::string type_in;
	std::string subtype_in;
	
	id_in = res_in.getId();
	type_in = res_in.getType();
	subtype_in = res_in.getType();
	
	(*mResources)[type_in][id_in] = &res_in;
	(*mSubResources)[subtype_in][id_in] = &res_in;

	resourceBin.insert(&res_in);
	if (final) 
	{
		res_in.setInit(true);
		upcastBin.insert(&res_in);
	}
}


Resource *ResourceManager::removeResource(Resource &res_in)
{
	resourceBin.erase(&res_in);
	upcastBin.erase(&res_in);
	outcastBin.erase(&res_in);

	std::string id_in;
	std::string type_in;
	std::string subtype_in;

	id_in = res_in.getId();
	type_in = res_in.getType();
	subtype_in = res_in.getType();

//	if (!hasResource(id_in,type_in)) return NULL;
	
	(*mResources)[type_in].erase(id_in);
	if (!(*mResources)[type_in].size()) (*mResources).erase(type_in);

	(*mSubResources)[subtype_in].erase(id_in);
	if (!(*mResources)[subtype_in].size()) (*mResources).erase(subtype_in);
	
	return &res_in;
}
