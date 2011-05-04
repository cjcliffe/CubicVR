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

#include <vector>
#include <string>
#include <iostream>

#ifdef ARCH_PSP
	#include <pspgu.h>
	#include <pspgum.h>

	// PSP GU <-> GL Translation macros

	// types
	#define GLint int
	#define GLuint unsigned int
	#define GLubyte	unsigned char
	#define GLfloat	float

	// constants
	#define GL_UNPACK_ALIGNMENT               0x0CF5
	#define GL_PROJECTION	GU_PROJECTION
	#define GL_MODELVIEW	GU_MODEL
	#define GL_PROJECTION	GU_PROJECTION

	// functions
	#define glViewport sceGuViewport
	#define glMatrixMode sceGumMatrixMode
	#define glViewport sceGuViewport
	#define glLoadIdentity sceGumLoadIdentity
	#define gluPerspective sceGumPerspective

	extern ScePspFVector3 psp_vec_xyz;
	extern ScePspFVector3 psp_vec_cam_eye;
	extern ScePspFVector3 psp_vec_cam_center;
	extern ScePspFVector3 psp_vec_cam_up;

	#define glTranslatef(MX,MY,MZ) psp_vec_xyz.x=MX;psp_vec_xyz.y=MY;psp_vec_xyz.z=MZ; sceGumTranslate(&psp_vec_xyz);
	#define gluLookAt(EYEX,EYEY,EYEZ,CENTERX,CENTERY,CENTERZ,UPX,UPY,UPZ) psp_vec_cam_eye.x=EYEX;psp_vec_cam_eye.y=EYEY;psp_vec_cam_eye.z=EYEZ; psp_vec_cam_center.x=CENTERX;psp_vec_cam_center.y=CENTERY;psp_vec_cam_center.z=CENTERZ; psp_vec_cam_up.x=UPX;psp_vec_cam_up.y=UPY;psp_vec_cam_up.z=UPZ; sceGumLookAt(&psp_vec_cam_eye, &psp_vec_cam_center, &psp_vec_cam_up);
	#define glPushMatrix sceGumPushMatrix
	#define glPopMatrix sceGumPopMatrix
	#define glLoadMatrixf(m) sceGumLoadMatrix((ScePspFMatrix4 *) m);
	#define glMultMatrixf(m) sceGumMultMatrix((ScePspFMatrix4 *) m);
	#define glGetFloatv(x,m) sceGumStoreMatrix((ScePspFMatrix4 *) m);


	#define GL_AMBIENT	GU_AMBIENT
	#define GL_DIFFUSE	GU_DIFFUSE
	#define GL_SPECULAR	GU_SPECULAR
	#define GL_POSITION	GU_POSITION
	#define GL_SPOT_DIRECTION	GU_SPOT_DIRECTION
#endif

#ifdef OPENGL_ES
	#include <OpenGLES/ES1/glext.h>
	#include <math.h>
	#define glColor3f(r,g,b) glColor4f(r,g,b,1.0)
	#define glFrustum glFrustumf
#endif

#ifdef _WIN32
	#include <CubicVR/glew/glew.h>
	#include <CubicVR/glew/wglew.h>
//	#include <GL/glut.h>
	#include <gl/GL.h>
//	#include <gl/GLAux.h>
	#include <gl/GLU.h>
#endif

#if defined(__APPLE__) && !defined(OPENGL_ES)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
#endif

#ifdef ARCH_DC
	//#include <kos.h>
//	#include <GL/gl.h>
//	#include <GL/glu.h>
	#include <GLX/gl.h>
	#include <GLX/glu.h>
#endif

#ifdef ARCH_DC
#define gluLookAt dc_gluLookAt
#define gluPerspective dc_gluPerspective
#endif
#if defined(OPENGL_ES) || defined(ARCH_DC)
extern IMPEXP void gluPerspective(double fovy, double aspect, double zNear, double zFar);
extern IMPEXP void gluLookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ);
#endif


#ifdef ARCH_PSP
#ifdef __cplusplus
extern "C" {
#endif
	/* make a static allocation of vram memory and return pointer relative to vram start */
	void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm);
	/* make a static allocation of vram memory and return absolute pointer */
	void* getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm);
#ifdef __cplusplus
}
#endif

#endif
	
typedef struct profiler_ent
{
	int shaderval_count;
	int object_count;
	int sector_hit;
};

extern profiler_ent cubicvr_profiler;

