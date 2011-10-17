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

#ifndef SOFTSCENEOBJECT_H
#define SOFTSCENEOBJECT_H

#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletCollision/NarrowPhaseCollision/btGjkEpa2.h>

#include <CubicVR/cvr_defines.h>
#include <CubicVR/XYZ.h>
#include <CubicVR/Shader.h>
#include <CubicVR/SceneObjects/RigidSceneObject.h>

class SoftSceneObject : public RigidSceneObject
{
protected:
	btSoftBody *mSoftBody;
	btSoftBodyWorldInfo	*softBodyWorldInfo;
	int *indicies;
	btScalar *verticies;
	Mesh soft_obj;
	Mesh *bind_obj;

public:
	SoftSceneObject();
	SoftSceneObject(Mesh &obj_in);

	virtual void bind(Mesh &obj_in);
	
	void initialize(btSoftRigidDynamicsWorld &dynamicsWorld_in, btSoftBodyWorldInfo &softBodyWorldInfo_in);
	virtual void evaluate();
	
	virtual void render(Shader &renderer, int stage, bool initShadowMatrix = false, bool no_pivot = false, bool no_scale = false);
	
	btSoftBody &createSoftBody();
	inline btSoftBody &createSoftBody(btCollisionShape &colShape_in) { bindCollisionShape(colShape_in); return createSoftBody(); };
	btSoftBody &getSoftBody();
	void setSoftBody(btSoftBody &mSoftBody_in);
};

#endif