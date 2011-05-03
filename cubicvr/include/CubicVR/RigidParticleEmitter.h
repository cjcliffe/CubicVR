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

#ifndef CUBICVR_RIGIDPARTICLEEMITTER
#define CUBICVR_RIGIDPARTICLEEMITTER


#include <CubicVR/cvr_defines.h>


#include <CubicVR/XYZ.h>
#include <CubicVR/RGB.h>
#include <CubicVR/Particle.h>
#include <CubicVR/ParticleEmitter.h>
#include <CubicVR/ObjectController.h>
#include <btBulletDynamicsCommon.h>


#include <stdlib.h>


class IMPEXP RigidParticleEmitter : public IMPEXP ParticleEmitter
{
protected:
	btDiscreteDynamicsWorld* dynamicsWorld;
	map<float,btCollisionShape*,ltufloat> particleShapes;
	
public:
	float particle_velocity;

	RigidParticleEmitter(btDiscreteDynamicsWorld &dynamicsWorld_in):ParticleEmitter()
	{
		dynamicsWorld = &dynamicsWorld_in;
	}

	virtual void emit(Particle &newParticle, float timeIndex, float lifeSpan=8.0)
	{
		float mass = 0.01;
		
		btCollisionShape *particleShape;
		
		newParticle.birth = timeIndex;
		newParticle.lifespan = lifeSpan;
//		newParticle.size = 1.0;
		
		if (particleShapes.find(newParticle.size) == particleShapes.end())
		{
			particleShapes[newParticle.size] = particleShape = new btSphereShape(newParticle.size/2.0);
		}
		else
		{
			particleShape = particleShapes[newParticle.size];
		}

		btTransform startTransform;

		btVector3 localInertia(0,0,0);

		particleShape->calculateLocalInertia(mass,localInertia);

		startTransform.setIdentity();

		newParticle.position = position+newParticle.position;
		
		startTransform.setOrigin(btVector3(newParticle.position.x, newParticle.position.y, newParticle.position.z));			
		
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,particleShapes[newParticle.size],localInertia);

		newParticle.mRigidBody = new btRigidBody(cInfo);
		
		newParticle.mRigidBody->setAngularFactor(0);

		newParticle.mRigidBody->setFriction(0);

		dynamicsWorld->addRigidBody(newParticle.mRigidBody);

		newParticle.mRigidBody->activate(true);
	
		if (rotation.x||rotation.y||rotation.z)
		{
			btVector3 force(rotation.x,rotation.y,rotation.z);

//			printf("%.3f -- %.3f,%.3f,%.3f\n", particle_velocity, force.getX(), force.getY(), force.getZ());			
			
			force *= particle_velocity;
	
			newParticle.mRigidBody->applyImpulse(force, btVector3(0,0,0));
			
		}

		
	};
	
	
	virtual void govern(Particle &p_in, Particle &p_out, float timeIndex)
	{	
		btTransform objTrans;

		btDefaultMotionState* myMotionState = (btDefaultMotionState*)p_in.mRigidBody->getMotionState();

		myMotionState->getWorldTransform(objTrans);

		p_out.position = XYZ(objTrans.getOrigin().getX(),objTrans.getOrigin().getY(),objTrans.getOrigin().getZ());
	};
	
	virtual void bury(Particle &dead_particle)
	{
		dynamicsWorld->removeRigidBody(dead_particle.mRigidBody);
	}

};

/*


	

class RigidSphere : public RigidSceneObject
{
	static std::map<float, btCollisionShape*, ltufloat> sphereShapes;

	
public:
	RigidSphere() : RigidSceneObject()
	{
	
	}

	virtual void initialize(btDiscreteDynamicsWorld &dynamicsWorld_in)
	{
		XYZ scale = controller().scale;
		
		if (sphereShapes[scale.x])
		{
			colShape = sphereShapes[scale.x];
		}
		else
		{
			colShape = new btSphereShape(scale.x/2.0);
			sphereShapes[scale.x] = colShape;
		}
		
		RigidSceneObject::initialize(dynamicsWorld_in);		
	}
};

*/


#endif