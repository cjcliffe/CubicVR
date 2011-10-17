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

#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H


#include <CubicVR/cvr_defines.h>

#include <CubicVR/Motion.h>
#include <CubicVR/Shader.h>
#include <CubicVR/ObjectController.h>
#include <CubicVR/CollisionMap.h>
#ifndef ARCH_DC
#include <CubicVR/ParticleSystem.h>
#endif
#include <CubicVR/SectorRef.h>
#include <CubicVR/Bone.h>
#include <CubicVR/FRUSTUM.h>
#include <CubicVR/Resource.h>
#include <vector>


enum SceneObjectArrayParam
{
	OBJ_ARRAY_RIGHT = 0,
	OBJ_ARRAY_UP,
	OBJ_ARRAY_BACK,
	OBJ_ARRAY_LEFT,
	OBJ_ARRAY_DOWN,
	OBJ_ARRAY_FRONT
};



class SceneObjectCore
{
	virtual void calcVisibility(const XYZ &camPosition, FRUSTUM &frustum) {} ;
};


class IMPEXP SceneObject : public SceneObjectCore, public Controllable, public ObjectController, public Resource
{
private:
	std::set<SceneObject *>::iterator children_i;
	
public:
//	ObjectController objController;
	std::set<SceneObject *> children;
	std::vector<SectorLoc> sectorRef;
	std::vector<int> arrayParam;
	XYZ arraySpacing;
	
	SceneObject *parent;
	CollisionMap *cmap;
	Mesh *obj;
	BoneSet *bones;
	BITSET *segmentMask;
	
	std::string modelType;
	std::string modelId;
	std::string parentId;
	
	// bone_map = bone_map [ bone pointer ] [ bone # ] [ point List # ] = "Point Index"
	vector<va_xyz> bone_vertex_index;
	vector<va_xyz> bone_vertex_weight;
	
	bool shadow_self, shadow_cast, shadow_receive, isvisible, active;
	bool dynamic_object, bounds_only;
	float mass, restitution, friction;
	
	bool hasVisibility;
//	bool worldspaceChildren;
	
	XYZ aabbMin, aabbMax;
	XYZ aabbMin_last, aabbMax_last;

#ifndef ARCH_DC
	ParticleSystem *psys;
	std::vector<ParticleEmitter *> psys_emitters;
#endif
	
	SceneObject();
	SceneObject(Mesh &obj_in);	
	~SceneObject();

	// Resource Management
	static Resource *upcast(ResourceManager *rm_in, Resource *res_in);

	virtual bool onLoad();
	virtual bool onSave();

	string &getModelId();
	void setModelId(const string &m_id);

	string &getModelType();
	void setModelType(const string &m_type);
	
	string &getParentId();
	void setParentId(const string &p_id);

	virtual void setId(const std::string &id_in);
	
#ifndef ARCH_PSP
	void calculateBoneMap(BoneSet *bones);
	inline void calculateBoneMap() { calculateBoneMap(bones); } ;
#endif
	
	void setDynamicObject(bool active);
	void setBoundsOnly(bool active);
	void setMass(float m);
	void setFriction(float f);

	bool getDynamicObject();
	bool getBoundsOnly();
	float getMass();
	float getFriction();
	
	void setArrayParam(vector<int> &arrayParam_in);
	vector<int> &getArrayParam();	
	void setArrayParam(int right = 0, int top = 0, int back = 0, int left = 0, int bottom = 0, int front = 0);
	void setArrayParam(SceneObjectArrayParam param, int val);
	XYZ &getArraySpacing();
	
	// ObjectController interface
	void control(int controllerId, int motionId, float value);
	float read_control(int controllerId, int motionId);

	
	std::vector<SectorLoc> &getSectorRef();

	void bind(Mesh &obj_in);
	void bindChild(SceneObject &sceneobj_in);
	void unbindChild(SceneObject &sceneobj_in);
	void bindParent(SceneObject &sceneObj_in);

	bool hasParent();
	
	virtual void render(Shader &renderer, int stage, bool initShadowMatrix = false, bool no_pivot = false, bool no_scale = false);
	
	virtual void calcVisibility(const XYZ & camPosition, FRUSTUM &frustum);
	
//	void transformBegin();
//	void transformEnd();
//	void transformReverseBegin();
//	void transformReverseEnd();	

//	ObjectController &controller();
	
	void shadowSelf(bool state) { shadow_self = state; };
	bool shadowSelf() { return shadow_self; };

	void shadowCast(bool state) { shadow_cast = state; };
	bool shadowCast() { return shadow_cast; };

	void shadowReceive(bool state) { shadow_receive = state; };
	bool shadowReceive() { return shadow_receive; };
	
	void visible(bool state) { isvisible = state; };
	bool visible() { return isvisible; };
	
	void setCollisionMap(Mesh *objMap = NULL);
	void compileSphereMap(float nominal_rad);

#ifndef ARCH_PSP
	va_rgb *dynamic_colors;

	void buildVertexLightMap();
	void calcVertexLightMap(Shader &renderer);
#endif

	bool aabbIntersect(XYZ &aabbMin1_in, XYZ &aabbMax1_in, XYZ &aabbMin2_in, XYZ &aabbMax2_in, XYZ &aabbMinI_out, XYZ &aabbMaxI_out);
	bool calcAABB();
	inline void getLastAABB(XYZ &aabbMin_out, XYZ &aabbMax_out) { aabbMin_out = aabbMin_last; aabbMax_out = aabbMax_last; };
	
	bool getAllChildren(vector<SceneObject *> &child_list);
	
};


#endif
