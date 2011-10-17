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

#include <CubicVR/SceneObjects/Vehicle.h>

/*
 float	gEngineForce = 0.f;
 float	gBreakingForce = 0.f;
 
 float	maxEngineForce = 1000.f;//this should be engine/velocity dependent
 float	maxBreakingForce = 100.f;
 
 float	gVehicleSteering = 0.f;
 float	steeringIncrement = 0.04f;
 float	steeringClamp = 0.3f;
 float	wheelRadius = 0.5f;
 float	wheelWidth = 0.4f;
 float	wheelFriction = 1000;//1e30f;
 float	suspensionStiffness = 20.f;
 float	suspensionDamping = 2.3f;
 float	suspensionCompression = 4.4f;
 float	rollInfluence = 0.1f;//1.0f;
 
 
 btScalar suspensionRestLength(0.6); 
 
 */

Vehicle::Vehicle () : 
gEngineForce(0.0f), 
gBreakingForce(0.0f), 
maxEngineForce(2000.0f), 
maxBreakingForce(125.0f), 
gVehicleSteering(0.0f), 
steeringClamp(0.51f), 
//wheelRadius(0.39f), 
//wheelWidth(0.3f), 
//wheelFriction(0.94f), 
//suspensionStiffness(40.0f), 
//suspensionDamping(2.3f), 
//suspensionCompression(2.4f), 
//suspensionRestLength(0.04f),
//rollInfluence(2.0f),
//connectionHeight(-0.10f),
rightIndex(0),
upIndex(1),
forwardIndex(2)
//frontAxelZ(1.6f),
//rearAxelZ(-1.3f),
//axelWidth(0.848f*2.0f)
{
	mass = 800.0f;
	hasVisibility=false;
	typeName = "sceneobject.vehicle";
}


Vehicle::Vehicle (Mesh &carObj_in, Mesh &carCollisionObj_in) : 
gEngineForce(0.0f), 
gBreakingForce(0.0f), 
maxEngineForce(2000.0f), 
maxBreakingForce(125.0f), 
gVehicleSteering(0.0f), 
steeringClamp(0.51f), 
//wheelRadius(0.39f), 
//wheelWidth(0.3f), 
//wheelFriction(0.94f), 
//suspensionStiffness(40.0f), 
//suspensionDamping(2.3f), 
//suspensionCompression(2.4f), 
//suspensionRestLength(0.04f),
////rollInfluence(2.0f),
//connectionHeight(-0.10f),
rightIndex(0),
upIndex(1),
forwardIndex(2)
//frontAxelZ(1.6f),
//rearAxelZ(-1.3f),
//axelWidth(0.848f*2.0f)
{
	mass = 800.0f;
	hasVisibility=false;
	bind(carObj_in);
	bindCollisionMap(carCollisionObj_in);
	typeName = "sceneobject.vehicle";
}



Resource *Vehicle::upcast(ResourceManager *rm_in, Resource *res_in)
{
	Vehicle *new_vehicle = new Vehicle();
	new_vehicle->Import(*res_in);
	new_vehicle->onLoad();
	
	string meshModelId, collisionModelId; // wheelModelId, 
	
	meshModelId = new_vehicle->getMeshModelId();
//	wheelModelId = new_vehicle->getWheelModelId();
	collisionModelId = new_vehicle->getCollisionModelId();
	
	if (meshModelId == string(""))
	{
		Logger::log(LOG_ERROR,"Mesh for vehicle not specified, upcast failed.\n");
		return NULL;
	}

//	if (wheelModelId == string(""))
//	{
//		Logger::log(LOG_ERROR,"Wheel model for vehicle not specified, upcast failed.\n");
//		return NULL;
//	}

	if (collisionModelId == string(""))
	{
		Logger::log(LOG_ERROR,"Collision model for vehicle not specified, upcast failed.\n");
		return NULL;
	}
	
	Mesh *meshObj, *wheelObj, *meshCollisionObj;
	
	meshObj = (Mesh *)rm_in->upcast(&rm_in->getResource("model",meshModelId));
//	wheelObj = (Object *)rm_in->upcast(&rm_in->getResource("model",wheelModelId));
	meshCollisionObj = (Mesh *)rm_in->upcast(&rm_in->getResource("model",collisionModelId));
	
	meshObj->init();
//	wheelObj->init();
	meshCollisionObj->init();
	meshCollisionObj->cache(false); // collision doesn't need cache, drop it
	
	new_vehicle->bind(*meshObj);
	new_vehicle->bindCollisionMap(*meshCollisionObj);
	
	for (unsigned int i = 0; i < new_vehicle->wheels.size(); i++)
	{
		wheelObj = (Mesh *)rm_in->upcast(&rm_in->getResource("model",new_vehicle->wheels[i]->getWheelModelId()));
		wheelObj->init();
		new_vehicle->wheels[i]->setWheelModel(wheelObj);
	}
	
//	new_vehicle->wheelObj = wheelObj;	
	
	
	return new_vehicle;
}


bool Vehicle::onLoad() 
{
	if (!properties) return false;
	
	
	if (properties->rootNode().hasAnother("position")) properties->rootNode().child("position").element().get(position);
	if (properties->rootNode().hasAnother("rotation")) properties->rootNode().child("rotation").element().get(rotation);

	if (properties->rootNode().hasAnother("mesh")) properties->rootNode().child("mesh").element().get(meshModelId);
//	if (properties->rootNode().hasAnother("wheel")) properties->rootNode().child("wheel").element().get(wheelModelId);
	if (properties->rootNode().hasAnother("collision")) properties->rootNode().child("collision").element().get(collisionModelId);
	
	if (properties->rootNode().hasAnother("mass")) properties->rootNode().child("mass").element().get(mass);
	
	unsigned int wheelCount = 0;
	
	properties->rootNode().rewind("wheel");
	while (properties->rootNode().hasAnother("wheel")) 
	{
//		properties->rootNode().child("wheel").element().get(wheelModelId);
		
		VehicleWheel *newWheel = new VehicleWheel();
		newWheel->loadFrom(&properties->rootNode().getNext("wheel"));
		addWheel(wheelCount,newWheel);
		wheelCount++;
	}
	
	w_init = l_init = false;
	
	return true;
};

bool Vehicle::onSave() 
{
	if (properties) delete properties;
	
	properties = new DataTree();
	
	properties->rootNode().newChild("position").element().set(position);
	properties->rootNode().newChild("rotation").element().set(rotation);

	properties->rootNode().newChild("mesh").element().set(meshModelId);
//	properties->rootNode().newChild("wheel").element().set(wheelModelId);
	properties->rootNode().newChild("collision").element().set(collisionModelId);	
	
	properties->rootNode().newChild("mass").element().set(mass);
		
	for (unsigned int i = 0; i < wheels.size(); i++)
	{
		wheels[i]->saveTo(&properties->rootNode().newChild("wheel"));
	}
	
	clearTransform();
	
	return true;
};

void Vehicle::initialize(btDiscreteDynamicsWorld &dynamicsWorld_in)
{		
	
//	SceneObject *nullObj;
	if (!cmap_obj)
	{
		cmap_obj = new Mesh;
		cmap_obj->cloneStructure(*obj);
	}



#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	cmap_obj->triangulate();
#endif
#endif
#endif
	
	
	cmap = new CollisionMap;
	cmap->addMesh(*cmap_obj);
	
	colShape = cmap->makeCollisionShape(mass);

//	if (wheelObj)
//	{
//		// front left
//		wheelRef[0].bind(*wheelObj);
//		wheel[0].bindChild(wheelRef[0]);
//		wheel[0].setPosition(XYZ(0.486f,0.141f,-0.9125f));
//		wheel[0].setMatrixLock(true);
//		
//		// front right
//		wheelRef[1].bind(*wheelObj);
//		wheelRef[1].setRotation(XYZ(0,180,0));
//		wheel[1].bindChild(wheelRef[1]);
//		wheel[1].setPosition(XYZ(-0.486f,0.141f,-0.9125f));
//		wheel[1].setMatrixLock(true);
//
//		// front left
//		wheelRef[2].bind(*wheelObj);
//		wheelRef[2].setRotation(XYZ(0,180,0));
//		wheel[2].bindChild(wheelRef[2]);
//		wheel[2].setPosition(XYZ(0.486f,0.141f,0.843f));
//		wheel[2].setMatrixLock(true);
//		
//		// front right
//		wheelRef[3].bind(*wheelObj);
//		wheel[3].bindChild(wheelRef[3]);
//		wheel[3].setPosition(XYZ(-0.486f,0.141f,0.843f));
//		wheel[3].setMatrixLock(true);
//	}

	btVector3 wheelAxleCSR,wheelAxleCSL;

	wheelDirectionCS0 = btVector3(0,-1,0);
	wheelAxleCS = btVector3(-1,0,0);
	
	RigidSceneObject::initialize(dynamicsWorld_in);

	btTransform tr;
	tr.setIdentity();

	setPivot(XYZ(0,0,0));

	btCompoundShape* compound = new btCompoundShape();

	btTransform localTrans;
	localTrans.setIdentity();

	localTrans.setOrigin(btVector3(0,0,0));

	compound->addChildShape(localTrans,colShape);

	tr.setOrigin(btVector3(0,0.f,0));

	setMass(mass);

	mRigidBody = &createRigidBody(*compound);
	
	// reset scene
	gVehicleSteering = 0.f;
	mRigidBody->setCenterOfMassTransform(btTransform::getIdentity());
	mRigidBody->setLinearVelocity(btVector3(0,0,0));
	mRigidBody->setAngularVelocity(btVector3(0,0,0));

	//	dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(),dynamicsWorld->getDispatcher());
		
	/// create vehicle
	m_vehicleRayCaster = new btDefaultVehicleRaycaster(dynamicsWorld);
	m_vehicle = new btRaycastVehicle(m_tuning,mRigidBody,m_vehicleRayCaster);
			
	///never deactivate the vehicle
	mRigidBody->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addVehicle(m_vehicle);
	
	//choose coordinate system
	m_vehicle->setCoordinateSystem(rightIndex,upIndex,forwardIndex);

//	btVector3 connectionPointCS0(axelWidth/2.0f,connectionHeight,frontAxelZ);
	
	for (unsigned int i = 0; i < wheels.size(); i++)
	{
		btVector3 wpos = wheels[i]->getWheelPosition().cast();
		
//		printf("Wheel Radius: %f\n",wheels[i]->getWheelRadius());
//		printf("Wheel Width: %f\n",wheels[i]->getWheelWidth());
		
		m_vehicle->addWheel(wpos,wheelDirectionCS0,wheelAxleCS,wheels[i]->getSuspensionRest(),wheels[i]->getWheelRadius(),m_tuning,wheels[i]->getSteering());
	}

//	connectionPointCS0 = btVector3(axelWidth/2.0f,connectionHeight,rearAxelZ);
//	m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,m_tuning,isFrontWheel);
	
	updateSuspension();

	/// end car

	//worldspaceChildren = true;
}


void Vehicle::updateSuspension()
{
	for (int i=0;i<m_vehicle->getNumWheels();i++)
	{
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
		
		wheel.m_suspensionStiffness = wheels[i]->getSuspensionStiffness();
		wheel.m_wheelsDampingRelaxation = wheels[i]->getDampingRelaxation();
		wheel.m_wheelsDampingCompression = wheels[i]->getDampingCompression();
		wheel.m_frictionSlip = wheels[i]->getFrictionSlip();
		wheel.m_rollInfluence = wheels[i]->getRollInfluence();
	}
	
	if (m_vehicle)
	{
		m_vehicle->resetSuspension();
		for (int i=0;i<m_vehicle->getNumWheels();i++)
		{
			//synchronize the wheels with the (interpolated) chassis worldtransform
			m_vehicle->updateWheelTransform(i,true);
		}
	}
}

void Vehicle::render(Shader &renderer, int stage, bool initShadowMatrix, bool no_pivot, bool no_scale)
{	
	SceneObject::render(renderer,stage,initShadowMatrix,no_pivot,no_scale);

	for (unsigned int i = 0; i < wheels.size(); i++)
	{
		wheels[i]->wheelObj.render(renderer,stage,initShadowMatrix,no_pivot,no_scale);
	}
	
//	wheels[1].render(renderer,stage,initShadowMatrix,no_pivot,no_scale);
//	wheels[2].render(renderer,stage,initShadowMatrix,no_pivot,no_scale);
//	wheels[3].render(renderer,stage,initShadowMatrix,no_pivot,no_scale);
	
};


void Vehicle::evaluate()
{
	float m[16];
	
	for (int i=0;i<m_vehicle->getNumWheels();i++)
	{
		if (wheels[i]->getSteering())
		{
			m_vehicle->setSteeringValue(gVehicleSteering,i);
		}
		
		if (wheels[i]->getBraking())
		{
			m_vehicle->setBrake(gBreakingForce,i);
		}

		if (wheels[i]->getDriving())
		{
			m_vehicle->applyEngineForce(gEngineForce,i);	
		}
		
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i,true);
		m_vehicle->getWheelTransformWS(i).getOpenGLMatrix(m);
		wheels[i]->wheelObj.setMatrix(m);
		wheels[i]->wheelObj.clearTransform();
		wheels[i]->wheelRef.clearTransform();
		wheels[i]->wheelObj.calcAABB();
	}

	RigidSceneObject::evaluate();
	
	calcAABB();	
}


XYZ Vehicle::getWheelGroundPosition(int wheelNum)
{
	if (wheelNum > 3) return XYZ(0,0,0);

	return wheels[wheelNum]->wheelObj.getWorldPosition()-XYZ(0,wheels[wheelNum]->getWheelRadius(),0);
}

float Vehicle::getWheelSkid(int wheelNum)
{
	if (wheelNum > 3) return 0.0f;
	
	btWheelInfo& wheelInfo = m_vehicle->getWheelInfo(wheelNum);
	
	return wheelInfo.m_skidInfo;
}

btRigidBody *Vehicle::getRigidGround(int wheelNum)
{
	if (wheelNum > 3) return NULL;

	btWheelInfo& wheelInfo = m_vehicle->getWheelInfo(wheelNum);

	return (class btRigidBody*) wheelInfo.m_raycastInfo.m_groundObject;
}

void Vehicle::setEngineForce(float engineForce)
{
	gEngineForce = engineForce;
}

void Vehicle::setSteering(float steering)
{
	gVehicleSteering = steering;
}

void Vehicle::incSteering(float steeringVal)
{
	gVehicleSteering += steeringVal;
	
	if (gVehicleSteering > steeringClamp) gVehicleSteering = steeringClamp;
	if (gVehicleSteering < -steeringClamp) gVehicleSteering = -steeringClamp;
}

void Vehicle::setBrake(float brake_val)
{
	gBreakingForce = brake_val;
}

/*

if (normalKeyPressed['w']) gEngineForce = 5000.0;
else if (normalKeyPressed['s']) gEngineForce = -1000.0; else gEngineForce = 0.0;

if (normalKeyPressed['a']) gVehicleSteering += 0.5f*lus;
if (normalKeyPressed['d']) gVehicleSteering -= 0.5f*lus;

if (normalKeyPressed[' ']) gBreakingForce = 200.0; else gBreakingForce = 0.0;

if (!normalKeyPressed['a'] || !normalKeyPressed['d']) gVehicleSteering -= gVehicleSteering*0.95*lus;

float	steeringClamp = 0.5f;

if (gVehicleSteering > steeringClamp) gVehicleSteering = steeringClamp;
if (gVehicleSteering < -steeringClamp) gVehicleSteering = -steeringClamp;

*/

/*
void Vehicle::setSuspension(float restLength, float stiffness, float damping, float compression)
{
	if(restLength != 0)			suspensionRestLength = restLength;
	if(stiffness != 0)			suspensionStiffness = stiffness;
	if(damping != 0)			suspensionDamping = damping;
	if(compression != 0)		suspensionCompression = compression;
}

void Vehicle::setAxels(float width, float front, float rear)
{
	if(width != 0)		axelWidth = width;
	if(front != 0)		frontAxelZ = front;
	if(rear != 0)		rearAxelZ = rear;
}
*/