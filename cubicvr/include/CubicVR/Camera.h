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

#ifndef CUBICVR_CAMERA_H
#define CUBICVR_CAMERA_H


#include <CubicVR/SceneObject.h>


#define CAMERA_FREE 0
#define CAMERA_TARGET 1

#define CAMERA_CONTROLLER_FOV 100
#define CAMERA_MOTION_FOV 100


class IMPEXP Camera : public Resource, public Controllable
{
private:
public:
	
	unsigned short type;
	float fov, aspect, nearclip, farclip;
	int width, height;	
	
	static Camera *lastActive;
	
	float projectionMatrix[16];
	float viewMatrix[16];
	
#ifdef ARCH_PSP
	float camAngle;	// stored and used for env mapping on psp
#endif

#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	float dof_near, dof_far;
	float dof_near_linear, dof_far_linear;
	inline void setNearDOF(float dof_near_in) { dof_near=dof_near_in; dof_near_linear = dof_near/(farclip-nearclip); };
	inline void setFarDOF(float dof_far_in) { dof_far=dof_far_in; dof_far_linear = dof_far/(farclip-nearclip); };
	inline float getNearDOF() { return dof_near; };
	inline float getFarDOF() { return dof_far; };
#endif
	
	XYZ position, rotation, target;
	Vector up,right;
	SceneObject *sceneObjTarget;
	
	Camera(int width_in, int height_in, float fov_in = 60, float nearclip_in=0.1, float farclip_in=2000.0, int type_in = CAMERA_FREE);
	Camera();
	~Camera();

	void control(int controllerId, int motionId, float value);
	
	void setSize(int width_in, int height_in);
	void setFOV(float fov_in);
	void setAspect(float aspect_in);
	void setAspect(int screen_w, int screen_h);
	void setPosition(const XYZ &pos);
	void setRotation(const XYZ &pos);
	void setTarget(const XYZ &pos);
	void bindTarget(SceneObject *sceneObj);
	void trackTarget(float trackingSpeed, float safeDistance);
	void moveViewRelative(XYZ &pt, float xdelta, float zdelta);

	btVector3 getRayTo(int x,int y);
	Vector getRay(int x,int y);
	void setClip(float near_in, float far_in);
	void setType(int type_in);
	
	void setup();
	
	float distSlopeX(float z);
	float distSlopeY(float z);
	static void orthoNearBounds(XYZ position, float ortho_width, float ortho_height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, XYZ &aabb1, XYZ &aabb2);
	static void orthoFarBounds(XYZ position, float ortho_width, float ortho_height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float farClip, XYZ &aabb1, XYZ &aabb2);

};


#endif
