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

#ifndef CUBICVR_RIGIDSCENE_H
#define CUBICVR_RIGIDSCENE_H

#include <CubicVR/Scene.h>
#include <CubicVR/SceneObjects/RigidSceneObject.h>

#ifndef ARCH_PSP
#include <CubicVR/RigidSceneDebug.h>
#endif

class RigidScene : public Scene
{
protected:
//	std::vector<RigidSceneObject *> rigid_objects;
	std::set<RigidSceneObject *> rigid_sceneobjs_bound;

	std::map<std::string, RigidSceneObject *, string_less> rigid_sceneobjs_bound_ref;
	
	btDiscreteDynamicsWorld* dynamicsWorld;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	
	btVector3 worldAabbMin;
	btVector3 worldAabbMax;
	
//	btAxisSweep3* overlappingPairCache;
	btDbvtBroadphase* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	
	int	maxProxies;
	bool constraints_setup;
	
//	bool debugMode;
	bool collisionEvents;

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
	RigidSceneDebug *debugDrawer;
#endif

	bool rigidUpdate;

public:
	std::map<btRigidBody *, RigidSceneObject *> contactMap;

	RigidScene(int scr_width_in = 512, int scr_height_in = 512,  XYZ aabbMin = XYZ(-10000,-10000,-10000), XYZ aabbMax = XYZ(10000,10000,10000));
	
	void solidifyScene();
	
	void bind(RigidSceneObject &rigid_sceneObj_in);
	using Scene::bind;
	void unbind(RigidSceneObject &rigid_sceneObj_in);
	using Scene::unbind;
	
	RigidSceneObject &rigidSceneObject(const std::string name_in);
	
	void stepSimulation( btScalar timeStep,int maxSubSteps=1, btScalar fixedTimeStep=btScalar(1.0)/btScalar(60.0));
	void setupConstraints();
	
	btDiscreteDynamicsWorld *getDynamicsWorld();
	
	virtual void firstInit();
	virtual void process();
	virtual void render();
//	void debug(bool debug_mode);
//	bool debug();
	
	inline void setRigidUpdate(bool rigidUpdateFlag) { rigidUpdate = rigidUpdateFlag; };
	void setGravity(const XYZ &grav_in);

	inline std::set<RigidSceneObject *> &rigidobjs() { return rigid_sceneobjs_bound; };
};

#endif
