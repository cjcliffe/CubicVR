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




#include <CubicVR/ResourceScene.h>
#include <CubicVR/ResourceManager.h>
#include <CubicVR/objectLWO.h>
#include <CubicVR/StringUtil.h>
#include <sys/types.h>
#include <sys/stat.h>

ResourceScene::ResourceScene(ResourceManager *rm_in, int width_in, int height_in): RigidScene(width_in,height_in)
{
	setResourceManager(rm_in);
	typeName = "scene";
}


bool ResourceScene::onSave() 
{
	if (properties) delete properties;
	
	properties = new DataTree();
	
	reloadScenePath();
	
	struct stat oBuf;
	
		
	
	std::set<SceneObject *>::iterator res_i;
	for (res_i = garbageBin.begin(); res_i != garbageBin.end(); res_i++)
	{
		if ((*res_i)->getSourceFile() != "")
		{
			if (!stat((*res_i)->getSourceFile().c_str(),&oBuf))
			{
				remove((*res_i)->getSourceFile().c_str());
			}
			
			delete (*res_i);
		}
	}
	
	garbageBin.clear();
	
	
	for (res_i = resourceSceneObjects.begin(); res_i != resourceSceneObjects.end(); res_i++)
	{
		if ((*res_i)->getSourceFile().empty())
		{
			string newFileName = scenePath + "/" + (*res_i)->getId() + ".xml";
			(*res_i)->SaveToFileXML(newFileName);
		}
		else
		{
			(*res_i)->SaveToFileXML((*res_i)->getSourceFile());
		}
	}
	
	return true;
	//		if (properties->rootNode().hasAnother("scene_name")) properties->rootNode().child("scene_name").element().set(sceneName);
};


void ResourceScene::unbind(SceneObject &res_in)
{
	resourceSceneObjects.erase(&res_in);
	RigidScene::unbind(res_in);
};

void ResourceScene::setResourceManager(ResourceManager *rm_in)
{
	rm = rm_in;
}


void ResourceScene::bind(SceneObject &res_in)
{
	if (resourceSceneObjects.find(&res_in) == resourceSceneObjects.end())
	{
		resourceSceneObjects.insert(&res_in);
		RigidScene::bind(res_in);
	}
}


bool ResourceScene::onLoad() 
{
	reloadScenePath();
	
	std::vector<std::string> projectFiles;
	std::vector<std::string>::iterator projectFiles_i;
	
	rm->pool("scene").findByExtension("xml", projectFiles);
	
	for (projectFiles_i = projectFiles.begin(); projectFiles_i != projectFiles.end(); projectFiles_i++)
	{
		string file_path, file_name, file_base, file_ext;
		
		str_file_extract((*projectFiles_i), file_path, file_name, file_base, file_ext);
		
//		printf("Parsing project file: [%s : %s]\n",(*projectFiles_i).c_str(), file_name.c_str());
		printf("Parsing project file: %s\n",file_name.c_str());
		
		if (file_name != "scene.xml")
		{
//			SceneObject *tmpResource;
			
			Resource *newRes = new Resource();
			newRes->LoadFromFileXML((*projectFiles_i).c_str());
			
			rm->addResource(*newRes);
			
			printf("Found Resource: %s: %s\n",newRes->getId().c_str(), newRes->getSubType().c_str());

		}
	}		
	
	
	ResourceTypeSet sceneObjs = rm->getResources("sceneobject");
	ResourceTypeSet::iterator sceneObjs_i;
	
	//	initSectorMap(XYZ(-5000,-5000,-5000),XYZ(5000,5000,5000));
	
	
	for (sceneObjs_i = sceneObjs.begin(); sceneObjs_i != sceneObjs.end(); sceneObjs_i++)
	{
		Resource *tmpResource = sceneObjs_i->second;
		//SceneObject *newSceneObj = (SceneObject *)
		rm->upcast(tmpResource);
	}

	sceneObjs = rm->getResources("sceneobject");

	for (sceneObjs_i = sceneObjs.begin(); sceneObjs_i != sceneObjs.end(); sceneObjs_i++)
	{
		Resource *tmpResource = sceneObjs_i->second;
		SceneObject *newSceneObj = (SceneObject *)rm->upcast(tmpResource);

//		// warning:temp
//		newSceneObj->setScale(XYZ(1,1,1));
		
		if (!newSceneObj)
		{
			Logger::log("Error upcasting element!");
		}
		else
		{
			printf("Adding scene object: %s\n",newSceneObj->getId().c_str());
			bind(*newSceneObj);
		}
	}
	
	//	for (sceneObjs_i = sceneObjs.begin(); sceneObjs_i != sceneObjs.end(); sceneObjs_i++)
	//	{
	//		Resource *tmpResource = sceneObjs_i->second;
	//		SceneObject *newSceneObj = (SceneObject *)rm->upcast(tmpResource);
	//		
	//		newSceneObj->clearTransform();
	//	}
	//
//	for (sceneObjs_i = sceneObjs.begin(); sceneObjs_i != sceneObjs.end(); sceneObjs_i++)
//	{
//		Resource *tmpResource = sceneObjs_i->second;
//		SceneObject *newSceneObj = (SceneObject *)rm->upcast(tmpResource);
//		
//		if (!newSceneObj->hasParent()) 
//		{
//			newSceneObj->movedReset();
//			newSceneObj->clearTransform();	
//			newSceneObj->calcAABB();
//		}
//	}

	return true;
};


void ResourceScene::reloadScenePath()
{
	if (getSourceFile().empty()) return;
	
	string file_path, file_name, file_base, file_ext;
	
	str_file_extract(getSourceFile(), file_path, file_name, file_base, file_ext);
	
	scenePath = file_path;	
	
	rm->pool("scene").addPath(scenePath.c_str(),true);
	rm->pool("scene").refresh();
}
