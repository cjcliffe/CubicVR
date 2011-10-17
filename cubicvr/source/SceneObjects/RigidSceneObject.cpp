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


#include <CubicVR/SceneObjects/RigidSceneObject.h>


void RigidSceneObject::QuaternionToEulerXYZ(const btQuaternion &quat,XYZ &euler)
{
   cvrFloat w=quat.getW();   cvrFloat x=quat.getX();   cvrFloat y=quat.getY();   cvrFloat z=quat.getZ();
   cvrFloat sqw = w*w; cvrFloat sqx = x*x; cvrFloat sqy = y*y; cvrFloat sqz = z*z;
   euler.z = RADTODEG((atan2(2.0f * (x*y + z*w),(sqx - sqy - sqz + sqw))));
   euler.x = RADTODEG((atan2(2.0f * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
   euler.y = RADTODEG((asin(-2.0f * (x*z - y*w))));
}


RigidSceneObject::RigidSceneObject():mass(1.0f),restitution(0.0f),friction(0.5f),colShape(NULL),mRigidBody(NULL),constraints_setup(false),rigid_parent(false),cmap_obj(NULL),rigidMargin(0) 
{

}


RigidSceneObject::RigidSceneObject(Mesh &obj_in):mass(1.0f),restitution(0.0f),friction(0.5f),colShape(NULL),mRigidBody(NULL),constraints_setup(false),rigid_parent(false),rigidMargin(0)
{
	obj = &obj_in;
	cmap_obj = &obj_in;
}


RigidSceneObject::RigidSceneObject(Mesh &obj_in,Mesh &collision_obj):mass(1.0f),restitution(0.0f),friction(0.5f),colShape(NULL),mRigidBody(NULL),constraints_setup(false),rigid_parent(false),rigidMargin(0)
{
	obj = &obj_in;
	cmap_obj = &collision_obj;
}


void RigidSceneObject::setMass(float mass_in)
{
	mass = mass_in;
}

void RigidSceneObject::setRestitution(float restitution_in)
{
	restitution = restitution_in;	
}


void RigidSceneObject::setFriction(float friction_in)
{
	friction = friction_in;	
}



void RigidSceneObject::bindCollisionMap(Mesh &cmap_in)
{
	cmap_obj = &cmap_in;
}
	
void RigidSceneObject::bindCollisionMap(CollisionMap &cmap_in)
{
	cmap = &cmap_in;
}

void RigidSceneObject::bindCollisionShape(btCollisionShape &colShape_in)
{
	/// Create Dynamic'Objects
	colShape = &colShape_in;
}


void RigidSceneObject::initialize(btDiscreteDynamicsWorld &dynamicsWorld_in)
{
	dynamicsWorld = &dynamicsWorld_in;

	startTransform.setIdentity();
	
	btVector3 localInertia(0,0,0);
	
	btQuaternion rot;
	XYZ rot_src = getRotation(); 
	
	rot.setEuler(DEGTORAD(rot_src.y),DEGTORAD(rot_src.x),DEGTORAD(rot_src.z));
	
	startTransform.setRotation(rot);

	
	if (!rigid_parent) 
	{
		startTransform.setOrigin(getPosition().cast());
	}
	else
	{
		startTransform.setOrigin(getPosition().cast());		
		startTransform = rigid_parent->getRigidBody().getWorldTransform() * startTransform;
	}
		
	mRigidBody = NULL;
	
//	movedReset();
//	startTransform.getOpenGLMatrix(w_trans);
//	startTransform.setMatrix(w_trans);
//	clearTransform();
//	calcAABB();
	
//	dynamicsWorld->addRigidBody(mRigidBody);
}


void RigidSceneObject::evaluate()
{
	if (mRigidBody && (myMotionState = mRigidBody->getMotionState()))
	{
	
		myMotionState->getWorldTransform(objTrans);

//#ifndef ARCH_PSP
		float m[16];

		objTrans.getOpenGLMatrix(m);
		setMatrixLock(true);
		setMatrix(m);

//#endif
//		controller().position = XYZ(objTrans.getOrigin().getX(),objTrans.getOrigin().getY(),objTrans.getOrigin().getZ());

		XYZ a = getPosition();
		XYZ newpos = XYZ(objTrans.getOrigin());
		a -= newpos;
		
		XYZ newrot; 
		
		QuaternionToEulerXYZ(objTrans.getRotation(),newrot);
		
		XYZ b = getRotation();
		b -= newrot;
	
		// attempt to prevent unnecessary tiny jitters from updating position/rotation to smooth out movements and the gl matrix cache
		double trans_jitter_threshold = 0.01;
		double rot_jitter_threshold = 0.1;
	
		if ((fabs(a.x) > trans_jitter_threshold || fabs(a.y) > trans_jitter_threshold || fabs(a.z) > trans_jitter_threshold)||
			(fabs(b.x) > rot_jitter_threshold || fabs(b.y) > rot_jitter_threshold || fabs(b.z) > rot_jitter_threshold))
		{
			setPosition(newpos);
			setRotation(newrot);

			XYZ bb1,bb2;

//			movedReset();
			btVector3 bbmin,bbmax;
			
			mRigidBody->getAabb(bbmin,bbmax);
			if (bbmin!=aabbMin && bbmax != aabbMax)
			{
				aabbMin_last=aabbMin;
				aabbMax_last=aabbMax;
				aabbMin=bbmin;
				aabbMax=bbmax;
				t_moved=true;
			}
			else
			{
				t_moved=false;
			}
			
//			getAABB(bb1,bb2);
			
			//l_init = w_init = false;
//			clearBounds();
//				printf("jitter = %f,%f,%f  r: %f,%f,%f \n",a.x,a.y,a.z,b.x,b.y,b.z);
		}
//		else
//		{
//			controller().trans_last = false;
//		}



		
//			printf("rot = %f,%f,%f\n",controller().rotation.x,controller().rotation.y,controller().rotation.z);
	}
//	else
//	{
//		controller().trans_last = false;
//	}


	std::set<RigidSceneObject *>::iterator children_i;
	
	for (children_i = rigid_children.begin(); children_i != rigid_children.end(); children_i++)
	{
		(*children_i)->evaluate();
	}
}


void RigidSceneObject::setMargin(float rigidMargin_in)
{
	rigidMargin = rigidMargin_in;
}


btRigidBody &RigidSceneObject::createRigidBody()
{
	btVector3 localInertia(0,0,0);

	if (!cmap && !colShape && (cmap_obj || obj) && !mRigidBody)
	{
		cmap = new CollisionMap();
		cmap->addMesh(cmap_obj?*cmap_obj:*obj);
		cmap->setMargin(rigidMargin);
	}

	if (cmap && !colShape && !mRigidBody)
	{
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
//		if (mass == 0.0)
//		{
			colShape = cmap->makeCollisionShape(mass,scale,&arrayParam,&arraySpacing);
			if (rigidMargin) colShape->setMargin(rigidMargin);
//		}
//		else
//		{
//			colShape = cmap->makeCollisionShape(mass,scale,&arrayParam,&arraySpacing);
//			if (rigidMargin) colShape->setMargin(rigidMargin);
//		}
	}


	if (mass != 0.0)
	{
		colShape->calculateLocalInertia(mass,localInertia);
	}
	
	if (!mRigidBody)
	{
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		
		btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,colShape,localInertia);		
		
//		if (restitution != 0.0)
		{
			cInfo.m_additionalDamping = true;
		}
		
//		cInfo.m_linearSleepingThreshold = btScalar(3.0f);
//		cInfo.m_angularSleepingThreshold = btScalar(3.0f);
		
		mRigidBody = new btRigidBody(cInfo);

		if (restitution != 0.0)
		{
			mRigidBody->setRestitution(restitution);
		}
		else if (mass == 0)
		{
			mRigidBody->setRestitution(1);	
		}

		mRigidBody->setFriction(friction);
	}
		
	mRigidBody->setActivationState(WANTS_DEACTIVATION); 

	return *mRigidBody;
}


btRigidBody &RigidSceneObject::getRigidBody()
{
	if (!mRigidBody) createRigidBody();
	return *mRigidBody;
}


void RigidSceneObject::setRigidBody(btRigidBody &mRigidBody_in)
{
//		if (mRigidBody)
//		{
//			//has existing, cleanup?
//		}
	mRigidBody = &mRigidBody_in;
}


void RigidSceneObject::bindChild(RigidSceneObject &rigid_sceneobj_in)
{
	rigid_children.insert(&rigid_sceneobj_in);

	RigidSceneObjectConstraint *c = new RigidSceneObjectConstraint(*this,rigid_sceneobj_in,CONSTRAINT_GENERIC);

	rigid_constraints[&rigid_sceneobj_in] = c;
	
	rigid_sceneobj_in.rigid_parent = this;
	
	rigid_sceneobj_in.clearTransform();

	constraints_setup = false;
};


void RigidSceneObject::bindParent(RigidSceneObject &rigid_sceneobj_in)
{
	rigid_sceneobj_in.bindChild(*this);
};


void RigidSceneObject::unbindChild(RigidSceneObject &rigid_sceneobj_in)
{
	if (rigid_children.find(&rigid_sceneobj_in) != rigid_children.end())
	{
		rigid_sceneobj_in.rigid_parent = NULL;
		rigid_children.erase(&rigid_sceneobj_in);
	}
	else
	{
		return;
	}
	
	if (rigid_constraints.find(&rigid_sceneobj_in) != rigid_constraints.end())
	{
		delete rigid_constraints[&rigid_sceneobj_in];
		rigid_constraints.erase(&rigid_sceneobj_in);
	}
	
};




bool RigidSceneObject::hasParent()
{
	return (rigid_parent != NULL || parent != NULL);
};

void RigidSceneObject::render(Renderer &renderer, int stage, bool initShadowMatrix, bool no_pivot, bool no_scale)
{	
	std::set<RigidSceneObject *>::iterator children_i;

	SceneObject::render(renderer,stage,initShadowMatrix,no_pivot,no_scale);

	for (children_i = rigid_children.begin(); children_i != rigid_children.end(); children_i++)
	{
		(*children_i)->render(renderer,stage,initShadowMatrix);
	}
};


void RigidSceneObject::handleCollision(RigidSceneObject *collision_obj, btPersistentManifold &manifold)
{
	// ...
}

void RigidSceneObject::setupConstraints(btDiscreteDynamicsWorld &dynamicsWorld_in)
{
	if (constraints_setup) return;

	map<RigidSceneObject *, RigidSceneObjectConstraint *>::iterator constraint_i;
	
	for (constraint_i = rigid_constraints.begin(); constraint_i != rigid_constraints.end(); constraint_i++)
	{
		constraint_i->second->setup(dynamicsWorld_in);
		constraint_i->first->setupConstraints(dynamicsWorld_in);
	}
	
	constraints_setup = true;
};

void RigidSceneObject::activate()
{
	if (!mRigidBody) createRigidBody();
	if (mRigidBody) mRigidBody->setActivationState(ACTIVE_TAG); 
}

