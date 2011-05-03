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

#ifndef CUBICVR_SOFTSCENE_H
#define CUBICVR_SOFTSCENE_H

#include <CubicVR/RigidScene.h>
#include <CubicVR/SceneObjects/SoftSceneObject.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#ifndef ARCH_PSP
#include <CubicVR/RigidSceneDebug.h>
#endif

class SoftScene : public RigidScene
{
protected:
	std::set<SoftSceneObject *> soft_sceneobjs_bound;
	
	std::map<std::string, SoftSceneObject *, string_less> soft_sceneobjs_bound_ref;

	btSoftBodyWorldInfo	softBodyWorldInfo;
	btSoftRigidDynamicsWorld *softDynamicsWorld;
public:

	SoftScene(int scr_width_in = 512, int scr_height_in = 512);
	
	void bind(SoftSceneObject &rigid_sceneObj_in);
	using Scene::bind;

	SoftSceneObject &softSceneObject(const std::string name_in);
		
	void stepSimulation(btScalar timeStep, int maxSubSteps=1, btScalar fixedTimeStep=btScalar(1.0)/btScalar(60.0));
	virtual void firstInit();
	virtual void process();

	virtual void render();
	btSoftRigidDynamicsWorld *getSoftDynamicsWorld();

	inline std::set<SoftSceneObject *> &softobjs() { return soft_sceneobjs_bound; };
	void setGravity(const XYZ &grav_in);
	
};

#endif