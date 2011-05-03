/*
 This file is part of CubicVR.
 
 CubicVR is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 CubicVR is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with CubicVR; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
 Source (c) 2003 by Charles J. Cliffe unless otherwise specified
 To contact me, e-mail or MSN to cj@cubicproductions.com or by ICQ#7188044
 http://www.cubicproductions.com
 */

#include <CubicVR/SoftScene.h>
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"




SoftScene::SoftScene(int scr_width_in, int scr_height_in)
{
	Scene(scr_width_in,scr_height_in);

	maxProxies = 10240;
	worldAabbMin = btVector3(-1000,-1000,-1000);
	worldAabbMax = btVector3(1000,1000,1000);
	softDynamicsWorld = NULL;
	dynamicsWorld = NULL;
	constraints_setup = false;
//	debugMode = false;
	rigidUpdate = true;


#ifdef ARCH_PSP	
	btDefaultCollisionConstructionInfo *pspCollisionConstruct;
	
	pspCollisionConstruct = new btDefaultCollisionConstructionInfo();
	pspCollisionConstruct->m_defaultMaxPersistentManifoldPoolSize = 8192;
	pspCollisionConstruct->m_defaultMaxCollisionAlgorithmPoolSize = 8192;
	pspCollisionConstruct->m_defaultStackAllocatorSize = (1*512*1024);
	collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration(*pspCollisionConstruct);
	maxProxies = 256;
#else
#ifdef OPENGL_ES
	btDefaultCollisionConstructionInfo *esCollisionConstruct;
	
	esCollisionConstruct = new btDefaultCollisionConstructionInfo();
	esCollisionConstruct->m_defaultMaxPersistentManifoldPoolSize = 8192;
	esCollisionConstruct->m_defaultMaxCollisionAlgorithmPoolSize = 8192;
	esCollisionConstruct->m_defaultStackAllocatorSize = (1*1024*1024);
	collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration(*esCollisionConstruct);

	maxProxies = 384;	
#else
#ifdef ARCH_DC
	btDefaultCollisionConstructionInfo *esCollisionConstruct;
	
	esCollisionConstruct = new btDefaultCollisionConstructionInfo();
	esCollisionConstruct->m_defaultMaxPersistentManifoldPoolSize = 8192;
	esCollisionConstruct->m_defaultMaxCollisionAlgorithmPoolSize = 8192;
	esCollisionConstruct->m_defaultStackAllocatorSize = (1*1024*1024);
	collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration(*esCollisionConstruct);
	
	maxProxies = 384;		
#else
	collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
#endif
#endif
#endif
	
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

	solver = new btSequentialImpulseConstraintSolver();

	softDynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	softDynamicsWorld->setGravity(btVector3(0,-10,0));

	softDynamicsWorld->getDispatchInfo().m_enableSPU = true;
	
	dynamicsWorld = softDynamicsWorld;

	softBodyWorldInfo.m_dispatcher = dispatcher;
	softBodyWorldInfo.m_broadphase = overlappingPairCache;
	softBodyWorldInfo.m_gravity.setValue(0,-10,0);
	softBodyWorldInfo.m_sparsesdf.Initialize();

	softBodyWorldInfo.air_density		=	(btScalar)1.2;
	softBodyWorldInfo.water_density	=	0;
	softBodyWorldInfo.water_offset		=	0;
	softBodyWorldInfo.water_normal		=	btVector3(0,0,0);

}




void SoftScene::bind(SoftSceneObject &soft_sceneObj_in)
{
	string idName = soft_sceneObj_in.getId();
	
	active_sceneobjs.insert(&soft_sceneObj_in);

	soft_sceneObj_in.initialize(*softDynamicsWorld,softBodyWorldInfo);
	if (!(&soft_sceneObj_in.getSoftBody())) soft_sceneObj_in.createSoftBody();
	if (&soft_sceneObj_in.getSoftBody()) softDynamicsWorld->addSoftBody(&soft_sceneObj_in.getSoftBody());

	soft_sceneobjs_bound.insert(&soft_sceneObj_in);
	
	if (idName != "" && idName != "null")
	{
		soft_sceneobjs_bound_ref[idName] = &soft_sceneObj_in;
	}

	constraints_setup = false;
	
	if (sectorMap) sectorMap->bind(&soft_sceneObj_in);
}


void SoftScene::stepSimulation( btScalar timeStep,int maxSubSteps, btScalar fixedTimeStep)
{
	softDynamicsWorld->stepSimulation(timeStep,maxSubSteps,fixedTimeStep);
}


btSoftRigidDynamicsWorld *SoftScene::getSoftDynamicsWorld()
{
	return softDynamicsWorld;
}


void SoftScene::process()
{
	RigidScene::process();

	if (!rigidUpdate) return;
	
	set<SoftSceneObject *>::iterator soft_objects_i;
	
	
	for (soft_objects_i = soft_sceneobjs_bound.begin(); soft_objects_i != soft_sceneobjs_bound.end(); soft_objects_i++)
	{
		if ((*soft_objects_i)->active)
		{
			if (rigidUpdate) (*soft_objects_i)->evaluate();
		}
	}
	
}

void SoftScene::render()
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
	if (debug.details)
	{
		if (!debugDrawer)
		{
			debugDrawer = new RigidSceneDebug();
			softDynamicsWorld->setDebugDrawer(debugDrawer);
		}
			
#ifndef ARCH_DC
		GLShader::clear();
#endif
		//		glEnable(GL_LINE_SMOOTH);
		glLineWidth(1);
		glDisable(GL_LIGHTING);
		glDepthFunc(GL_LEQUAL);
		
		dynamicsWorld->debugDrawWorld();
	}
#endif
#endif
	
	Scene::render();
}


void SoftScene::firstInit()
{
	RigidScene::firstInit();
}


SoftSceneObject &SoftScene::softSceneObject(const std::string name_in)
{
	return *soft_sceneobjs_bound_ref[name_in];
};

void SoftScene::setGravity(const XYZ &grav_in)
{
	softBodyWorldInfo.m_gravity.setValue(grav_in.x,grav_in.y,grav_in.z);
};

