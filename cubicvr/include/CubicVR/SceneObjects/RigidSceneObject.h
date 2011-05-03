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

#ifndef RIGIDSCENEOBJECT_H
#define RIGIDSCENEOBJECT_H

#include <btBulletDynamicsCommon.h>


#include <CubicVR/cvr_defines.h>
#include <CubicVR/XYZ.h>
#include <CubicVR/Shader.h>
#include <CubicVR/SceneObject.h>

#define CONSTRAINT_NULL 0
#define CONSTRAINT_GENERIC 1

class RigidSceneObjectConstraint;
class RigidSceneObject;

class RigidSceneObject : public SceneObject
{
protected:
	btRigidBody* mRigidBody;
	btCollisionShape *colShape;
	btTransform startTransform;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btMotionState* myMotionState;
	btTransform objTrans;
	float rigidMargin;
		
	Object *cmap_obj;

	bool constraints_setup;
	std::set<RigidSceneObject *> rigid_children;
	std::map<RigidSceneObject *, RigidSceneObjectConstraint *> rigid_constraints;
	
	
public:
	float mass;
	float restitution;
	float friction;	
	
	bool collisionEvents;
	
	RigidSceneObject *rigid_parent;

	static void QuaternionToEulerXYZ(const btQuaternion &quat,XYZ &euler);
	RigidSceneObject();
	RigidSceneObject(Object &obj_in);
	RigidSceneObject(Object &obj_in,Object &collision_obj);
	
	void setMass(float mass_in);
	void setRestitution(float restitution_in);
	void setFriction(float friction_in);
	void setMargin(float rigidMargin_in);
	
	void bindCollisionMap(Object &cmap_in);
	void bindCollisionMap(CollisionMap &cmap_in);
	void bindCollisionShape(btCollisionShape &colShape_in);
	
	virtual void handleCollision(RigidSceneObject *collision_obj, btPersistentManifold &manifold);
	
	virtual void initialize(btDiscreteDynamicsWorld &dynamicsWorld_in);
	virtual void evaluate();
	
	virtual void render(Shader &renderer, int stage, bool initShadowMatrix = false, bool no_pivot = false, bool no_scale = false);

	
	inline void copyCollision(RigidSceneObject &rigid_obj_in)
	{
		cmap = rigid_obj_in.cmap;
		colShape = rigid_obj_in.colShape;
	}
	
	btRigidBody &createRigidBody();
	inline btRigidBody &createRigidBody(btCollisionShape &colShape_in) { bindCollisionShape(colShape_in); return createRigidBody(); };
	virtual btRigidBody &getRigidBody();
	void setRigidBody(btRigidBody &mRigidBody_in);
	
	
	void bindChild(RigidSceneObject &rigid_sceneobj_in);
	void bindParent(RigidSceneObject &rigid_sceneobj_in);
	virtual bool hasParent();
	void unbindChild(RigidSceneObject &rigid_sceneobj_in);

	inline void bindChild(SceneObject &sceneobj_in) { SceneObject::bindChild(sceneobj_in); };
	inline void bindParent(SceneObject &sceneobj_in) { SceneObject::bindParent(sceneobj_in); };
	void setupConstraints(btDiscreteDynamicsWorld &dynamicsWorld_in);
	void activate();
};


class RigidSceneObjectConstraint
{
	public:
		int constraint_type;
		RigidSceneObject *parent;
		RigidSceneObject *child;
		btGeneric6DofConstraint* constraint;

	RigidSceneObjectConstraint(RigidSceneObject &parentObj, int type_in = CONSTRAINT_GENERIC) : child(NULL)
	{
		constraint_type = type_in;
		parent = &parentObj;
	}
		
	RigidSceneObjectConstraint(RigidSceneObject &parentObj, RigidSceneObject &childObj, int type_in = CONSTRAINT_GENERIC)
	{
		constraint_type = type_in;
		parent = &parentObj;
		child = &childObj;
	}
	
	void setup(btDiscreteDynamicsWorld &dynamicsWorld)
	{
		btTransform frameInA;
		btTransform frameInB;


			
		switch (constraint_type)
		{
			case CONSTRAINT_NULL: break;
			case CONSTRAINT_GENERIC:
				frameInA.setIdentity();
				frameInB.setIdentity();
				
				XYZ pos;
				
				pos -= child->getPosition();
				
				frameInB.setOrigin(pos.cast());
				frameInA.setOrigin(-(child->getPivot().cast()));
				
				constraint = new btGeneric6DofConstraint(parent->getRigidBody(),child->getRigidBody(),frameInA,frameInB,false);
				
//	btVector3 lowerSliderLimit = btVector3(0,-5,0); 
//	btVector3 hiSliderLimit = btVector3(0,5,0); 

//						constraint->setAngularLowerLimit(btVector3(10,0,0));
//		constraint->setAngularUpperLimit(btVector3(0,0,0));
//	constraint->setLinearLowerLimit(lowerSliderLimit); 
//	constraint->setLinearUpperLimit(hiSliderLimit); 

				dynamicsWorld.addConstraint(constraint);
			break;
		}

	}
	
};

#endif