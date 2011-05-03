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

#ifndef CUBICVR_VEHICLE_H
#define CUBICVR_VEHICLE_H

#include <CubicVR/SceneObjects/RigidSceneObject.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <CubicVR/SceneObjects/RigidSceneObject.h>

/*

 wheelRef[1].bind(*wheelObj);
 wheelRef[1].setRotation(XYZ(0,180,0));
 wheel[1].bindChild(wheelRef[1]);
 wheel[1].setPosition(XYZ(-0.486f,0.141f,-0.9125f));
 wheel[1].setMatrixLock(true);
 
 
 btVector3 connectionPointCS0(axelWidth/2.0f,connectionHeight,frontAxelZ);
 m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,m_tuning,isFrontWheel);
 
 
 for (int i=0;i<m_vehicle->getNumWheels();i++)
 {
	 btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
	 wheel.m_suspensionStiffness = suspensionStiffness;
	 wheel.m_wheelsDampingRelaxation = suspensionDamping;
	 wheel.m_wheelsDampingCompression = suspensionCompression;
	 wheel.m_frictionSlip = wheelFriction*(i<2)?2.0:1.5;
	 wheel.m_rollInfluence = rollInfluence;
 }
 
 
 gEngineForce(0.0f), 
 gBreakingForce(0.0f), 
 maxEngineForce(2000.0f), 
 maxBreakingForce(125.0f), 
 gVehicleSteering(0.0f), 
 steeringClamp(0.51f), 
 suspensionRestLength(0.04f),
 rollInfluence(2.0f),
 connectionHeight(-0.10f),
 rightIndex(0),
 upIndex(1),
 forwardIndex(2),
 frontAxelZ(1.6f),
 rearAxelZ(-1.3f),
 axelWidth(0.848f*2.0f)
 {
 mass = 800.0f;

 wheelRadius(0.39f), 
 wheelWidth(0.3f), 
 
 */


#define VEHICLE_DEFAULT_suspensionStiffness 40.0f
#define VEHICLE_DEFAULT_suspensionRest 0.05f
#define VEHICLE_DEFAULT_dampingRelaxation 2.3f
#define VEHICLE_DEFAULT_dampingCompression 2.4f
#define VEHICLE_DEFAULT_frictionSlip 0.94f
#define VEHICLE_DEFAULT_rollInfluence 1.0f

#define VEHICLE_DEFAULT_wheelRadius 0.0f
#define VEHICLE_DEFAULT_wheelWidth 0.0f

class VehicleWheel
{
public:
	Object *wheelModel;
	SceneObject wheelRef;
	SceneObject wheelObj;
//	btWheelInfo *wheelInfo;
	
	float suspensionStiffness;
	float suspensionRest;
	
	float dampingRelaxation;
	float dampingCompression;
	
	float frictionSlip;
	float rollInfluence;
	
	string wheelModelId;
	
	// These will be initialized automatically from the model if not provided
	float wheelRadius;
	float wheelWidth;

	// Relative position/rotation ( right wheels typicaly have rotation XYZ(0,180,0) );
	XYZ wheelRotation;
	XYZ wheelPosition;

	// Is this a steering, braking and / or driving wheel?
	bool steering;
	bool braking;
	bool driving;

	VehicleWheel()
	{
		setDefaults();
	};
	
	
	void setDefaults()
	{
		suspensionStiffness = VEHICLE_DEFAULT_suspensionStiffness;
		suspensionRest = VEHICLE_DEFAULT_suspensionRest;
		dampingRelaxation = VEHICLE_DEFAULT_dampingRelaxation;
		dampingCompression = VEHICLE_DEFAULT_dampingCompression;
		frictionSlip = VEHICLE_DEFAULT_frictionSlip;
		rollInfluence = VEHICLE_DEFAULT_rollInfluence;	
		wheelRadius = VEHICLE_DEFAULT_wheelRadius;	
		wheelWidth = VEHICLE_DEFAULT_wheelWidth;	
		wheelPosition = XYZ(0,0,0);
		steering = false;
		braking = true;
		driving = true;
		wheelModel = NULL;
	}
		

	void setWheelModel(Object *wheelModel_in, float wheelRadius_in = 0.0f, float wheelWidth_in = 0.0f)
	{
		wheelModel = wheelModel_in;
		
		if (wheelRadius_in == 0.0f)
		{
			wheelModel->calcBB();
			
			// average front to back & top to bottom to hopefully even out any tesselation misalignment
			wheelRadius = (wheelModel->bbMax.y - wheelModel->bbMin.y)/2.0;
			wheelRadius += (wheelModel->bbMax.z - wheelModel->bbMin.z)/2.0;
			wheelRadius /= 2.0f;			
		}
		else 
		{
			wheelRadius = wheelRadius_in;
		}
		
		if (wheelWidth_in == 0.0f)
		{
			wheelModel->calcBB();
			
			wheelWidth = wheelModel->bbMax.x-wheelModel->bbMin.x;
		}
		else
		{
			wheelWidth = wheelWidth_in;
		}

		wheelRef.bind(*wheelModel);
		wheelObj.bindChild(wheelRef);
		wheelObj.setMatrixLock(true);
	}
	
	
	void loadFrom(DataNode *elem)
	{
		setDefaults();
		
		if (elem->hasAnother("suspension_stiffness")) elem->child("suspension_stiffness").element().get(suspensionStiffness);
		if (elem->hasAnother("suspension_rest")) elem->child("suspension_rest").element().get(suspensionRest);
		if (elem->hasAnother("damping_relaxation")) elem->child("damping_relaxation").element().get(dampingRelaxation);
		if (elem->hasAnother("damping_compression")) elem->child("damping_compression").element().get(dampingCompression);
		if (elem->hasAnother("friction_slip")) elem->child("friction_slip").element().get(frictionSlip);
		if (elem->hasAnother("roll_influence")) elem->child("roll_influence").element().get(rollInfluence);
		if (elem->hasAnother("wheel_radius")) elem->child("wheel_radius").element().get(wheelRadius);
		if (elem->hasAnother("wheel_width")) elem->child("wheel_width").element().get(wheelWidth);
		if (elem->hasAnother("wheel_rotation")) elem->child("wheel_rotation").element().get(wheelRotation);
		if (elem->hasAnother("wheel_model")) elem->child("wheel_model").element().get(wheelModelId);
		if (elem->hasAnother("wheel_position")) elem->child("wheel_position").element().get(wheelPosition);
		if (elem->hasAnother("steering")) elem->child("steering").element().get((int &)steering);		
		if (elem->hasAnother("braking")) elem->child("braking").element().get((int &)braking);		
		if (elem->hasAnother("driving")) elem->child("driving").element().get((int &)driving);		
		
		setWheelPosition(wheelPosition);
		setWheelRotation(wheelRotation);
	};
	
	void saveTo(DataNode *elem)
	{
		if (suspensionStiffness != VEHICLE_DEFAULT_suspensionStiffness) elem->newChild("suspension_stiffness").element().set(suspensionStiffness);
		if (suspensionRest != VEHICLE_DEFAULT_suspensionRest) elem->newChild("suspension_rest").element().set(suspensionRest);
		if (dampingRelaxation != VEHICLE_DEFAULT_dampingRelaxation) elem->newChild("damping_relaxation").element().set(dampingRelaxation);
		if (dampingCompression != VEHICLE_DEFAULT_dampingCompression) elem->newChild("damping_compression").element().set(dampingCompression);
		if (frictionSlip != VEHICLE_DEFAULT_frictionSlip) elem->newChild("friction_slip").element().set(frictionSlip);
		if (rollInfluence != VEHICLE_DEFAULT_rollInfluence) elem->newChild("roll_influence").element().set(rollInfluence);
		if (wheelRadius !=  VEHICLE_DEFAULT_wheelRadius) elem->newChild("wheel_radius").element().set(wheelRadius);
		if (wheelWidth !=  VEHICLE_DEFAULT_wheelWidth) elem->newChild("wheel_width").element().set(wheelWidth);
		if (wheelRotation != XYZ(0,0,0)) elem->newChild("wheel_rotation").element().set(wheelRotation);
		if (wheelPosition != XYZ(0,0,0)) elem->newChild("wheel_position").element().set(wheelPosition);
		if (wheelModelId != "") elem->newChild("wheel_model").element().set(wheelModelId);
		if (steering) elem->newChild("steering").element().set(1);		
		if (braking) elem->newChild("braking").element().set(1);		
		if (driving) elem->newChild("driving").element().set(1);		
	};	
	
	inline void setSuspensionStiffness(float suspensionStiffness_in) { suspensionStiffness = suspensionStiffness_in; };
	inline float getSuspensionStiffness() { return suspensionStiffness; };

	inline void setSuspensionRest(float suspensionRest_in) { suspensionRest = suspensionRest_in; };
	inline float getSuspensionRest() { return suspensionRest; };

	inline void setDampingRelaxation(float dampingRelaxation_in) { dampingRelaxation = dampingRelaxation_in; };
	inline float getDampingRelaxation() { return dampingRelaxation; };

	inline void setDampingCompression(float dampingCompression_in) { dampingCompression = dampingCompression_in; };
	inline float getDampingCompression() { return dampingCompression; };

	inline void setFrictionSlip(float frictionSlip_in) { frictionSlip = frictionSlip_in; };
	inline float getFrictionSlip() { return frictionSlip; };

	inline void setRollInfluence(float rollInfluence_in) { rollInfluence = rollInfluence_in; };
	inline float getRollInfluence() { return rollInfluence; };
	
	inline void setWheelRadius(float wheelRadius_in) { wheelRadius = wheelRadius_in; };
	inline float getWheelRadius() { return wheelRadius; };
	
	inline void setWheelWidth(float wheelWidth_in) { wheelWidth = wheelWidth_in; };
	inline float getWheelWidth() { return wheelWidth; };
	
	inline void setWheelRotation(XYZ wheelRotation_in) 
	{ 
		wheelRotation = wheelRotation_in;
		wheelRef.setRotation(wheelRotation);
	};
	inline XYZ getWheelRotation() { return wheelRotation; };

	inline void setWheelModelId(string wheelModelId_in) { wheelModelId = wheelModelId_in; };
	inline string getWheelModelId() { return wheelModelId; };

	inline void setWheelPosition(XYZ wheelPosition_in) 
	{ 
		wheelPosition = wheelPosition_in; 
		wheelObj.setPosition(wheelPosition);
	};
	inline XYZ getWheelPosition() { return wheelPosition; };

	inline void setSteering(bool steering_in) { steering = steering_in; };
	inline bool getSteering() { return steering; };

	inline void setBraking(bool braking_in) { braking = braking_in; };
	inline bool getBraking() { return braking; };
	
	inline void setDriving(bool driving_in) { driving = driving_in; };
	inline bool getDriving() { return driving; };

};

class Vehicle : public RigidSceneObject
{
public:
	btVehicleRaycaster* m_vehicleRayCaster;
	btRaycastVehicle* m_vehicle;
	btRaycastVehicle::btVehicleTuning	m_tuning;

	int rightIndex;
	int upIndex;
	int forwardIndex;
	
	btVector3 wheelDirectionCS0;
	btVector3 wheelAxleCS;

	vector<VehicleWheel *> wheels;
	
//	Object *wheelObj;
	
	/* Vehicle variables */
	float	gEngineForce;
	float	gBreakingForce;

	float	maxEngineForce;//this should be engine/velocity dependent
	float	maxBreakingForce;

	float	gVehicleSteering;
//	float	steeringIncrement = 0.04f;
	float	steeringClamp;
	
	
//	float	wheelRadius;
//	float	wheelWidth;
//	float	wheelFriction;	//1e30f;
//	float	suspensionStiffness;
//	float	suspensionDamping;
//	float	suspensionCompression;
//	float	rollInfluence;	//1.0f;
//	float	suspensionRestLength;
//	float	connectionHeight;
	
//	float frontAxelZ;
//	float rearAxelZ;
//	float axelWidth;
	/*  */
	
	
	
//	SceneObject wheel[4];
//	SceneObject wheelRef[4];

	static Resource *upcast(ResourceManager *rm_in, Resource *res_in);

	Vehicle ();
	Vehicle (Object &carObj_in, Object &carCollisionObj_in);
	virtual void initialize(btDiscreteDynamicsWorld &dynamicsWorld_in);
	virtual void render(Shader &renderer, int stage, bool initShadowMatrix, bool no_pivot, bool no_scale);
	virtual void evaluate();
	void setEngineForce(float engineForce);
	void setSteering(float steering);
	void incSteering(float steeringVal);
	void setBrake(float brake_val);
	
	XYZ getWheelGroundPosition(int wheelNum);
	float getWheelSkid(int wheelNum);
	btRigidBody *getRigidGround(int wheelNum);
	
	void addWheel(unsigned int wheelNum, VehicleWheel *wheel_in)
	{
		if (wheels.size()<=wheelNum) wheels.resize(wheelNum+1);
		
		wheels[wheelNum] = wheel_in;
	};
	
	VehicleWheel *getWheel(unsigned int wheelNum)
	{
		return wheels[wheelNum];
	};
	
	// updates any wheel suspension changes
	void updateSuspension();
	
//	void setSuspension(float restLength = 0, float stiffness = 0, float damping = 0, float compression = 0);	
//	void setAxels(float width = 0, float front = 0, float rear = 0);
	

protected:
	string meshModelId,collisionModelId; // wheelModelId,

	
public:	// Resource params
	inline void setMeshModelId(string meshModelId_in) { meshModelId = meshModelId_in; };
	inline string getMeshModelId() { return meshModelId; };
//	inline void setWheelModelId(string wheelModelId_in) { wheelModelId = wheelModelId_in; };
//	inline string getWheelModelId() { return wheelModelId; };
	inline void setCollisionModelId(string collisionModelId_in) { collisionModelId = collisionModelId_in; };
	inline string getCollisionModelId() { return collisionModelId; };

	virtual bool onLoad();
	virtual bool onSave();
	
};

	

#endif

