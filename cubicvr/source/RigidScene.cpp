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

#include <CubicVR/RigidScene.h>
#include <CubicVR/Object.h>


RigidScene::RigidScene(int scr_width_in, int scr_height_in, XYZ aabbMin, XYZ aabbMax) : Scene(scr_width_in,scr_height_in), maxProxies(16384) , worldAabbMin(-10000,-10000,-10000) , worldAabbMax(10000,10000,10000), dynamicsWorld(NULL), constraints_setup(false), rigidUpdate(true), collisionEvents(false)
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
, debugDrawer(NULL)
#endif
{


#ifdef ARCH_PSP	
	btDefaultCollisionConstructionInfo *pspCollisionConstruct;

	pspCollisionConstruct = new btDefaultCollisionConstructionInfo();
	pspCollisionConstruct->m_defaultMaxPersistentManifoldPoolSize = 8192;
	pspCollisionConstruct->m_defaultMaxCollisionAlgorithmPoolSize = 8192;
	pspCollisionConstruct->m_defaultStackAllocatorSize = (1*512*1024);
	collisionConfiguration = new btDefaultCollisionConfiguration(*pspCollisionConstruct);
	maxProxies = 256;
#else
#ifdef OPENGL_ES
	btDefaultCollisionConstructionInfo *esCollisionConstruct;
	
	esCollisionConstruct = new btDefaultCollisionConstructionInfo();
	esCollisionConstruct->m_defaultMaxPersistentManifoldPoolSize = 8192;
	esCollisionConstruct->m_defaultMaxCollisionAlgorithmPoolSize = 8192;
	esCollisionConstruct->m_defaultStackAllocatorSize = (1*1024*1024);
	collisionConfiguration = new btDefaultCollisionConfiguration(*esCollisionConstruct);
	maxProxies = 384;	
#else
#ifdef ARCH_DC
	maxProxies = 256;	
	
	btDefaultCollisionConstructionInfo *esCollisionConstruct;
	
	esCollisionConstruct = new btDefaultCollisionConstructionInfo();
	esCollisionConstruct->m_defaultMaxPersistentManifoldPoolSize = 1024;
	esCollisionConstruct->m_defaultMaxCollisionAlgorithmPoolSize = 1024;
	esCollisionConstruct->m_defaultStackAllocatorSize = (1*1024*1024);
	
	collisionConfiguration = new btDefaultCollisionConfiguration(*esCollisionConstruct); 
#else	

	collisionConfiguration = new btDefaultCollisionConfiguration();

#endif
#endif
#endif

	dispatcher = new btCollisionDispatcher(collisionConfiguration);

//	dispatcher->registerCollisionCreateFunc(SPHERE_SHAPE_PROXYTYPE,SPHERE_SHAPE_PROXYTYPE, new btSphereSphereCollisionAlgorithm::CreateFunc);
//	dispatcher->registerCollisionCreateFunc(TRIANGLE_MESH_SHAPE_PROXYTYPE,TRIANGLE_MESH_SHAPE_PROXYTYPE, new btCompoundCollisionAlgorithm::CreateFunc);
//	dispatcher->registerCollisionCreateFunc(FAST_CONCAVE_MESH_PROXYTYPE,FAST_CONCAVE_MESH_PROXYTYPE, new btSphereSphereCollisionAlgorithm::CreateFunc);
//	dispatcher->registerCollisionCreateFunc(CONVEX_SHAPE_PROXYTYPE,CONVEX_SHAPE_PROXYTYPE, new btSphereSphereCollisionAlgorithm::CreateFunc);

	///the maximum size of the collision world. Make sure objects stay within these boundaries
	///Don't make the world AABB size too large, it will harm simulation quality and performance
	overlappingPairCache = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

	solver = new btSequentialImpulseConstraintSolver();

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-10,0));
//	dynamicsWorld->getSolverInfo().m_splitImpulse = 0;
//	dynamicsWorld->getDispatchInfo().m_enableSPU = true;

}



void RigidScene::solidifyScene()
{
	std::set<SceneObject *> staticObjs = sceneobjs_bound;
	std::set<SceneObject *>::iterator i;
	std::map<Object *,std::set<RigidSceneObject *> > sceneMap;
	std::set<RigidSceneObject *>::iterator j;
	
	RigidSceneObject *tmp;
	
	bool found;
	
	for (i = staticObjs.begin(); i != staticObjs.end(); i++)
	{
		if (!(*i)->obj) 
		{
			continue;
		}
		
		if ((*i)->obj)
		{
			tmp = new RigidSceneObject(*(*i)->obj);
			
			if (sceneMap.find((*i)->obj) != sceneMap.end())
			{
				found = false;
				
				for (j = sceneMap[(*i)->obj].begin(); j != sceneMap[(*i)->obj].end(); j++)
				{
					if ((*j)->getScale() == (*i)->getScale() && (*j)->arraySpacing == (*i)->arraySpacing && (*j)->arrayParam == (*i)->arrayParam)
					{
						found = true;
						tmp->copyCollision(**j);
					}
				}
				
				if (!found)
				{
					sceneMap[(*i)->obj].insert(tmp);	
				}
			}
			else
			{
				sceneMap[(*i)->obj].insert(tmp);	
			}

//			tmp->setMass(0);
			tmp->setPosition((*i)->getWorldPosition());
			tmp->setRotation((*i)->getWorldRotation());
			tmp->setScale((*i)->getScale());
			tmp->mass = (*i)->getMass();
//			printf("[  [  mass!: %f ]  ]\n",tmp->mass);
			tmp->arrayParam = (*i)->arrayParam;
			tmp->arraySpacing = (*i)->arraySpacing;
			tmp->setId((*i)->getId()+"_rigid");
			
			bind(*tmp);
		}
	}
	
	for (i = staticObjs.begin(); i != staticObjs.end(); i++)
	{
		unbind(*(*i));
	}
		
	
	sceneobjs_bound.clear();
}



void RigidScene::bind(RigidSceneObject &rigid_sceneObj_in)
{
	string idName = rigid_sceneObj_in.getId();
	
	XYZ bb1,bb2;

	rigid_sceneObj_in.initialize(*dynamicsWorld);
	if (!(&rigid_sceneObj_in.getRigidBody())) rigid_sceneObj_in.createRigidBody();
	if (&rigid_sceneObj_in.getRigidBody()) 
	{
		dynamicsWorld->addRigidBody(&rigid_sceneObj_in.getRigidBody());
	}
	
	
//	rigid_sceneObj_in.evaluate();
//	rigid_sceneObj_in.transformBegin(false,false,true);
//	rigid_sceneObj_in.setMatrixLock(false);
	rigid_sceneObj_in.movedReset();
	rigid_sceneObj_in.clearTransform();
//	rigid_sceneObj_in.aabbMin=rigid_sceneObj_in.aabbMax=XYZ(0,0,0);
	rigid_sceneObj_in.calcAABB();
	rigid_sceneObj_in.aabbMin_last = rigid_sceneObj_in.aabbMax_last = XYZ(0,0,0);
//	rigid_sceneObj_in.movedForce();
	
	bb1 = rigid_sceneObj_in.aabbMin;
	bb2 = rigid_sceneObj_in.aabbMax;
//	rigid_sceneObj_in.setMatrixLock(true);
//	rigid_sceneObj_in.clearTransform();
	
	printf("RIGID AABB: %f,%f,%f - %f,%f,%f\n",bb1.x,bb1.y,bb1.z,bb2.x,bb2.y,bb2.z);

//	rigid_sceneObj_in.transformBegin(false,false,true);

	if (rigid_sceneObj_in.hasVisibility)
	{
		sceneobjs_visibility.insert(&rigid_sceneObj_in);
//		printf("visibility id %d: \n\n",&rigid_sceneObj_in);

		constraints_setup = false;
	}
	else
	{
		active_sceneobjs.insert(&rigid_sceneObj_in);
		
//		rigid_objects.push_back(&rigid_sceneObj_in);
		
		rigid_sceneobjs_bound.insert(&rigid_sceneObj_in);
		
		if (idName != "" && idName != "null")
		{
			rigid_sceneobjs_bound_ref[idName] = &rigid_sceneObj_in;
		}
		
		constraints_setup = false;
		
		if (sectorMap) sectorMap->bind(&rigid_sceneObj_in);
	}
	
	
	if (rigid_sceneObj_in.collisionEvents)
	{
		collisionEvents = true;
	}
	
	contactMap[&rigid_sceneObj_in.getRigidBody()] = &rigid_sceneObj_in;
	
	rigid_sceneObj_in.clearTransform();
}


void RigidScene::unbind(RigidSceneObject &rigid_sceneObj_in)
{
	string idName = rigid_sceneObj_in.getId();
	
	
	if (rigid_sceneObj_in.hasVisibility)
	{
		sceneobjs_visibility.erase(&rigid_sceneObj_in);
		
		if (&rigid_sceneObj_in.getRigidBody()) 
		{
			dynamicsWorld->removeRigidBody(&rigid_sceneObj_in.getRigidBody());
		}
	}
	else
	{
		active_sceneobjs.erase(&rigid_sceneObj_in);
		
		if (&rigid_sceneObj_in.getRigidBody()) 
		{
			dynamicsWorld->removeRigidBody(&rigid_sceneObj_in.getRigidBody());
		}
		
		rigid_sceneobjs_bound.erase(&rigid_sceneObj_in);
		
		if (idName != "" && idName != "null")
		{
			rigid_sceneobjs_bound_ref.erase(idName);
		}
		
//		if (sectorMap) sectorMap->unbind(&rigid_sceneObj_in);
	}
	
	contactMap.erase(&rigid_sceneObj_in.getRigidBody());
	
	Scene::unbind(rigid_sceneObj_in);
}


void RigidScene::stepSimulation( btScalar timeStep,int maxSubSteps, btScalar fixedTimeStep)
{
	dynamicsWorld->stepSimulation(timeStep,maxSubSteps,fixedTimeStep);
}


void RigidScene::firstInit()
{
	if (!constraints_setup && rigid_sceneobjs_bound.size()) setupConstraints();
}

void RigidScene::process()
{
	if (!rigidUpdate) 
	{
		Scene::process();
		
		return;
	}
	
	set<RigidSceneObject *>::iterator rigid_sceneobjs_bound_i;
	
	for (rigid_sceneobjs_bound_i = rigid_sceneobjs_bound.begin(); rigid_sceneobjs_bound_i != rigid_sceneobjs_bound.end(); rigid_sceneobjs_bound_i++)
	{
		if ((*rigid_sceneobjs_bound_i)->active && (*rigid_sceneobjs_bound_i)->mass)
		{
//			(*rigid_sceneobjs_bound_i)->movedReset();
			(*rigid_sceneobjs_bound_i)->evaluate();
//			(*rigid_sceneobjs_bound_i)->transformBegin();
//			(*rigid_sceneobjs_bound_i)->transformEnd();
		}
	}
	
	if (collisionEvents)
	{
		btCollisionWorld* world = getDynamicsWorld()->getCollisionWorld();
		int numManifolds = world->getDispatcher()->getNumManifolds();
		
		for (int i=0;i<numManifolds;i++)
		{
			btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
			
			btRigidBody *obj0 = (btRigidBody*)contactManifold->getBody0();
			btRigidBody *obj1 = (btRigidBody*)contactManifold->getBody1();
						
			if (contactMap.find(obj0) == contactMap.end())
			{
				continue;
			}
			
			RigidSceneObject *sceneObj0 = contactMap[obj0];
			
			if (!sceneObj0->collisionEvents) continue;

			RigidSceneObject *sceneObj1;
			
			if (contactMap.find(obj1) == contactMap.end())
			{
				sceneObj1 = NULL;
			}
			else
			{
				sceneObj1 = contactMap[obj1];	
			}
			
			
			sceneObj0->handleCollision(sceneObj1,*contactManifold);
		}
	}

	Scene::process();
}


void RigidScene::render()
{	
	Scene::render();
	
	
#ifndef ARCH_PSP
#ifndef OPENGL_ES	
#ifndef ARCH_DC
	if (debug.details)
	{
		if (!debugDrawer)
		{
			debugDrawer = new RigidSceneDebug();
			dynamicsWorld->setDebugDrawer(debugDrawer);
		}
		
		GLShader::clear();
		glDepthMask(false);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(2);
		glDisable(GL_LIGHTING);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_TEXTURE_2D);
		
		cam->setup();
		
		dynamicsWorld->debugDrawWorld();
		
		glDisable(GL_LINE_SMOOTH);
		glLineWidth(1);
		
	}
#endif
#endif
#endif
}


RigidSceneObject &RigidScene::rigidSceneObject(const std::string name_in)
{
	return *rigid_sceneobjs_bound_ref[name_in];
};

btDiscreteDynamicsWorld *RigidScene::getDynamicsWorld()
{
	return dynamicsWorld;
}


//void RigidScene::debug(bool debug_mode)
//{
//	debugMode = debug_mode;
//}
//
//bool RigidScene::debug()
//{
//	return debugMode;
//}

void RigidScene::setGravity(const XYZ &grav_in)
{
	if (!dynamicsWorld) return;
	dynamicsWorld->setGravity(btVector3(grav_in.x,grav_in.y,grav_in.z));
}

void RigidScene::setupConstraints()
{
	set<RigidSceneObject *>::iterator rigid_sceneobjs_bound_i;
	
	for (rigid_sceneobjs_bound_i = rigid_sceneobjs_bound.begin(); rigid_sceneobjs_bound_i != rigid_sceneobjs_bound.end(); rigid_sceneobjs_bound_i++)
	{
		if (!(*rigid_sceneobjs_bound_i)->hasParent())
		{
			(*rigid_sceneobjs_bound_i)->setupConstraints(*dynamicsWorld);
		}
	}
	
	constraints_setup = true;
}