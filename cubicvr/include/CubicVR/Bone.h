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

#pragma once

#include <CubicVR/cvr_defines.h>

#include <CubicVR/Motion.h>
#include <CubicVR/Shader.h>
#include <CubicVR/ObjectController.h>
#include <CubicVR/map_string.h>
#include <vector>

class IMPEXP Bone : public Controllable, public ObjectController
{
public:
//	ObjectController objController;
	std::vector<Bone *> children;

	Bone *parent;
	float matrix[16];
	XYZ start_pt, end_pt;
	
	XYZ center;
	
	XYZ restPosition;
	XYZ restDirection;
	float restLength;
	float strength;
	
	bool trans_init;

	Bone();
	~Bone();
	
	void calcMatrix(bool childBones=true,bool firstBone=true);
	void calcNodes(bool firstBone=true);

	void control(int controllerId, int motionId, float value);
	float read_control(int controllerId, int motionId);

	inline void setRestPosition(const XYZ &pt) { restPosition = pt; }
	inline void setRestDirection(const XYZ &pt) { restDirection = pt; }
	inline void setRestLength(float restLen) { restLength = restLen; }
	inline void setStrength(float strengthVal) { strength = strengthVal; }
	
	void bindChild(Bone &boneObj_in);
	void bindParent(Bone &boneObj_in);

	bool hasParent();
	
//	void transformBegin();
//	void transformEnd();
//	void transformReverseBegin();
//	void transformReverseEnd();	

//	ObjectController &controller();
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)	
	void debugDraw(bool firstBone=true);
	void debugDrawNode();
#endif
};

class IMPEXP BoneSet
{
public:
	std::vector<Bone *> bones;
	std::map<const char *, cvrIndex> bone_map;
	
	cvrIndex bind(Bone *newBone, char *boneName=NULL)
	{
		bones.push_back(newBone);
		if (boneName != NULL) bone_map[boneName] = bones.size()-1;

		return bones.size()-1;
	}
	
	Bone *bone(cvrIndex boneNum) { return bones[boneNum]; }
	Bone *bone(const string &boneName) { if (bone_map.find(boneName.c_str()) == bone_map.end()) return NULL; return bones[bone_map[boneName.c_str()]]; }
};

