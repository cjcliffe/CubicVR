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

#include <CubicVR/Camera.h>

#ifdef ARCH_PSP
#include <fastmath.h>
#else
#include <math.h>
#endif

Camera *Camera::lastActive = NULL;

Camera::Camera() : type(CAMERA_FREE), fov(60), nearclip(1.0f), farclip(2000.0f), width(512), height(512), sceneObjTarget(NULL)
{
	position = XYZ(0,0,0);
	rotation = XYZ(0,0,0);
	target = XYZ(0,0,0);
	
	setAspect(width,height);
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	setNearDOF(50.0f);
	setFarDOF(200.0f);
#endif
}


Camera::Camera(int width_in, int height_in, float fov_in, float nearclip_in, float farclip_in, int type_in)
{
	position = XYZ(0,0,0);
	rotation = XYZ(0,0,0);
	target = XYZ(0,0,0);

	sceneObjTarget = NULL;
	width = width_in;
	height = height_in;
	fov = fov_in;
	nearclip = nearclip_in;
	farclip = farclip_in;
	type = type_in;

	setAspect(width,height);
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	setNearDOF(50.0f);
	setFarDOF(200.0f);
#endif
};


Camera::~Camera()
{

};


void Camera::setPosition(const XYZ &pos)
{
	position = pos;
}

void Camera::setRotation(const XYZ &pos)
{
	rotation = pos;
}


void Camera::setTarget(const XYZ &pos)
{
	type = CAMERA_TARGET;
	sceneObjTarget = NULL;
	target = pos;
}


void Camera::control(int controllerId, int motionId, float value)
{
	switch (controllerId)
	{
		case CONTROLLER_POSITION:
			switch (motionId)
			{
				case MOTION_X: position.x = value; break;
				case MOTION_Y: position.y = value; break;
				case MOTION_Z: position.z = value; break;
			}
		break;
		
		case CONTROLLER_ROTATION:
			switch (motionId)
			{
				case MOTION_X: rotation.x = value; break;
				case MOTION_Y: rotation.y = value; break;
				case MOTION_Z: rotation.z = value; break;
			}
		break;
		case CAMERA_CONTROLLER_FOV:
			switch (motionId)
			{
				case CAMERA_MOTION_FOV: fov = value; break;
			}
		break;
	}
};


void Camera::setSize(int width_in, int height_in)
{
	width = width_in;
	height = height_in;
};


void Camera::setFOV(float fov_in)
{
	fov = fov_in;
};


void Camera::setAspect(float aspect_in)
{
	aspect = aspect_in;
};


void Camera::setAspect(int screen_w, int screen_h)
{
	aspect = (float)screen_w/(float)screen_h;
};


void Camera::setClip(float near_in, float far_in)
{
	nearclip = near_in;
	farclip = far_in;
};


void Camera::setType(int type_in)
{
	type = type_in;
	
};


void Camera::moveViewRelative(XYZ &pt, float xdelta, float zdelta)
{

	float ang = atan2(zdelta,xdelta);
	float cam_ang = atan2(target.z-position.z,target.x-position.x);
	float mag = sqrt(xdelta*xdelta+zdelta*zdelta);
	
	float move_ang = cam_ang+ang+M_PI/2.0f;	

	pt.x+=mag*cos(move_ang);
	pt.z+=mag*sin(move_ang);
}


void Camera::trackTarget(float trackingSpeed, float safeDistance)
{
	Vector camv;
	
	camv = target;
	camv -= position;	

	Vector dist  = camv;

//	dist.y = 0;
	float fdist = dist.magnitude();

	Vector motionv = camv;

	motionv.makeUnit();
	motionv *= trackingSpeed*(1.0f / (1.0f/(fdist-safeDistance)));
	
	if (fdist > safeDistance)
	{
		position += motionv;
	}
	else if (fdist < safeDistance) 
	{
		motionv = camv;
		motionv.makeUnit();
		motionv *= trackingSpeed*(1.0f / (1.0f/(fabs(fdist-safeDistance))));
		position -= motionv;
	}
	else 
	{
		position.y += motionv.y;
	}
}

void Camera::bindTarget(SceneObject *sceneObj)
{
	type = CAMERA_TARGET;

	sceneObjTarget = sceneObj;
};

void Camera::setup()
{
	lastActive = this;
	
#ifdef ARCH_PSP
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(fov, aspect, nearclip, farclip);
	
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
#else
	// viewport setup
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, nearclip, farclip);
#ifndef ARCH_PSP
	glGetFloatv(GL_PROJECTION_MATRIX,projectionMatrix);
#endif
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer

#ifndef OPENGL_ES
#ifndef ARCH_DC
	GLShader::defaultShader.setShaderValue("camPos",position); 
#endif
#endif

#endif
 
	switch (type)
	{
		case CAMERA_FREE:
			#ifdef ARCH_PSP
				camAngle = rotation.y;
			#endif
			if (rotation.x || rotation.y || rotation.z)
			{
				if (rotation.z)
				{
					#ifdef ARCH_PSP
						sceGumRotateZ(DEGTORAD(-rotation.z));
					#else
						glRotatef(-rotation.z,0,0,1);
					#endif
				}
				if (rotation.x)
				{
					#ifdef ARCH_PSP
						sceGumRotateX(DEGTORAD(-rotation.x));
					#else
						glRotatef(-rotation.x,1,0,0);
					#endif
				}
				if (rotation.y)
				{
					#ifdef ARCH_PSP
						sceGumRotateY(DEGTORAD(-rotation.y));
					#else
						glRotatef(-rotation.y,0,1,0);
					#endif
				}
			}

			if (position.x || position.y || position.z)
			{
					glTranslatef(-position.x,-position.y,-position.z);
			}

			break;
		case CAMERA_TARGET:
#ifdef ARCH_PSP
	ScePspFVector3 cam_eye;
 	ScePspFVector3 cam_center;
 	ScePspFVector3 cam_up;


	cam_eye.x = position.x; cam_eye.y = position.y; cam_eye.z = position.z;
	cam_up.x = 0; cam_up.y = 1; cam_up.z = 0;

	if (sceneObjTarget)
	{
		target = sceneObjTarget->getPosition();
		cam_center.x = target.x; cam_center.y = target.y; cam_center.z = target.z;
	}
	else
	{
		cam_center.x = target.x; cam_center.y = target.y; cam_center.z = target.z;
	}

	Vector xAxisVector(1, 0,0);
	Vector camPos(cam_eye.x,cam_eye.y,cam_eye.z);
	Vector camTarget(cam_center.x,cam_center.y,cam_center.z);
	Vector camDir = camTarget-camPos;
	camDir.makeUnit();
	
	camAngle = camDir.angle(xAxisVector);	// used for env mapping

	sceGumLookAt(&cam_eye,&cam_center,&cam_up);
#else
	if (sceneObjTarget)
	{
		target = sceneObjTarget->getPosition();
		
		gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, 0, 1, 0);
	}
	else
	{
		gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, 0, 1, 0);
	}
	
#endif
			break;
	}
	
#ifndef ARCH_PSP
	glGetFloatv(GL_MODELVIEW_MATRIX,viewMatrix);
	
	right.x = viewMatrix[0];
	right.y = viewMatrix[4];
	right.z = viewMatrix[8];
	
	up.x = viewMatrix[1];
	up.y = viewMatrix[5];
	up.z = viewMatrix[9];
#endif
};


btVector3 Camera::getRayTo(int x,int y)
{
//	Vector rayTo = getRay(x,y);
//	
//	return btVector3(rayTo.x,rayTo.y,rayTo.z);
	btVector3 rayFrom = position.cast();
	btVector3 rayForward = (target.cast()-position.cast());
	
	rayForward.normalize();
	
	rayForward*= farclip;
	
	btVector3 rightOffset;
	btVector3 vertical = btVector3(0,1,0);
	
	btVector3 hor;
	hor = rayForward.cross(vertical);
	hor.normalize();
	
	vertical = hor.cross(rayForward);
	vertical.normalize();
	
	float tanfov = tanf(0.5f*(DEGTORAD(fov)));
	
	btScalar aspect = (btScalar)width / (btScalar)height;
	
	hor *= 2.0f * farclip * tanfov;
	vertical *= 2.0f * farclip * tanfov;
	
	if (hor<vertical)
	{
		hor*=aspect;
	} 
	else
	{
		vertical/=aspect;
	}
	
	btVector3 rayToCenter = rayFrom + rayForward;
	btVector3 dHor = hor * 1.f/float(width);
	btVector3 dVert = vertical * 1.f/float(height);
	
	
	btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
	rayTo += x * dHor;
	rayTo -= y * dVert;
	
	return rayTo;	
}


float Camera::distSlopeX(float z)
{
	return z*tan(DEGTORAD(fov/2.0));
}

float Camera::distSlopeY(float z)
{
	FRUSTUM frust;
	int viewport[4] = {0,0,width,height};
	
	XYZ ypos = frust.unProject(XYZ(width/2.0,0,z),projectionMatrix,viewMatrix,viewport)-frust.unProject(XYZ(width/2.0,0,0),projectionMatrix,viewMatrix,viewport);
	
	return ypos.y/z;
}



void Camera::orthoNearBounds(XYZ position, float ortho_width, float ortho_height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, XYZ &aabb1, XYZ &aabb2)
{
	Vector vright, vup, vforward;
	
	vright.x = modelMatrix.entries[0];
	vright.y = modelMatrix.entries[4];
	vright.z = modelMatrix.entries[8];
	
	vup.x = modelMatrix.entries[1];
	vup.y = modelMatrix.entries[5];
	vup.z = modelMatrix.entries[9];
	
	vup.makeUnit();
	vright.makeUnit();
	
	vforward = vup*vright;
	vforward.makeUnit();
	
	float hw, hh;
	XYZ right=vright,up=vup,forward=vforward;
	
	hw = ortho_width/2.0;
	hh = ortho_height/2.0;
	
	XYZ f_bounds[4];
	
	f_bounds[0] = position - right*hw + up*hh + forward*nearClip;
	f_bounds[1] = position + right*hw + up*hh + forward*nearClip;
	f_bounds[2] = position - right*hw - up*hh + forward*nearClip;
	f_bounds[3] = position + right*hw - up*hh + forward*nearClip;	
	
	aabb1 = f_bounds[0];
	aabb2 = f_bounds[0];
	
	for (int i = 1; i < 4; i++)
	{
		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
		
		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
	}
	
	
}


void Camera::orthoFarBounds(XYZ position, float ortho_width, float ortho_height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float farClip, XYZ &aabb1, XYZ &aabb2)
{
	Vector vright, vup, vforward;
	
	vright.x = modelMatrix.entries[0];
	vright.y = modelMatrix.entries[4];
	vright.z = modelMatrix.entries[8];
	
	vup.x = modelMatrix.entries[1];
	vup.y = modelMatrix.entries[5];
	vup.z = modelMatrix.entries[9];
	
	vup.makeUnit();
	vright.makeUnit();
	
	vforward = vup*vright;
	vforward.makeUnit();
	
	float hw, hh;
	XYZ right=vright,up=vup,forward=vforward;
	
	hw = ortho_width/2.0;
	hh = ortho_height/2.0;
	
	XYZ f_bounds[4];
	
	f_bounds[0] = position - right*hw + up*hh + forward*farClip;
	f_bounds[1] = position + right*hw + up*hh + forward*farClip;
	f_bounds[2] = position - right*hw - up*hh + forward*farClip;
	f_bounds[3] = position + right*hw - up*hh + forward*farClip;	
	
	aabb1 = f_bounds[0];
	aabb2 = f_bounds[0];
	
	for (int i = 1; i < 4; i++)
	{
		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
		
		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
	}
	
	
}



Vector Camera::getRay(int x,int y)
{
	Vector forward(position,target);
	Vector vertical = Vector(0,1,0);
	
	forward.makeUnit();
	
	Vector nearpt,farpt;
			
	nearpt = forward;
	nearpt *= nearclip;
	
	farpt = forward;
	farpt *= farclip;
	
	float wNear,hNear,wFar,hFar;
	
	hNear = 2.0f * tan(DEGTORAD(fov) / 2.0f) * nearclip;
	wNear = hNear * aspect;
	hFar = 2.0f * tan(DEGTORAD(fov) / 2.0f) * farclip;
	wFar = hFar * aspect;
	
	Vector far_right = right * (wFar/2.0f);
	Vector far_up = up * (hFar/2.0f);
	
	Vector far_top_left = far_up - far_right + farpt;
	
	float bx = (float)x/(float)width;
	float by = (float)y/(float)height;
	
	Vector outpt;
	
	outpt = far_top_left;
	outpt += right * (wFar * bx); 
	outpt -= up * (hFar * by); 
	
	return outpt;
}

