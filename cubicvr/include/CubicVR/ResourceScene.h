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

#include <CubicVR/RigidScene.h>

class ResourceScene: public RigidScene
{
private:
	std::set<SceneObject *> resourceSceneObjects;
	std::set<SceneObject *> garbageBin;
	ResourceManager *rm;
	string scenePath;
	
public:
	
	ResourceScene(ResourceManager *rm_in=NULL, int width_in=512, int height_in=512);
	void setResourceManager(ResourceManager *rm_in);
	
	void bind(SceneObject &res_in);	
	virtual void unbind(SceneObject &res_in);
	
	using RigidScene::bind;
	using RigidScene::unbind;
	
	void reloadScenePath();
	inline void addGarbage(SceneObject *junk)
	{
		garbageBin.insert(junk);
	};
	
	virtual bool onLoad();	
	virtual bool onSave(); 
};

