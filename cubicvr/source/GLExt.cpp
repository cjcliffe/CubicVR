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

#include <CubicVR/GLExt.h>

profiler_ent cubicvr_profiler;


#ifdef ARCH_PSP
ScePspFVector3 psp_vec_xyz;
ScePspFVector3 psp_vec_cam_eye;
ScePspFVector3 psp_vec_cam_center;
ScePspFVector3 psp_vec_cam_up;
#endif


#if defined(OPENGL_ES) || defined(ARCH_DC)
#include <CubicVR/Vector.h>

// glu replacement functions, maybe use instead of glu in gl2?

void gluPerspective(double fovy, double aspect, double zNear, double zFar)
{
	double xmin, xmax, ymin, ymax;
	
	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	
	glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void gluLookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ)
{
	Vector view_vec;
	Vector up_vec;
	
	view_vec = XYZ(lookAtX - eyeX, lookAtY - eyeY,lookAtZ - eyeZ);
	up_vec = XYZ(upX,upY,upZ);
	
	view_vec.makeUnit();
	up_vec.makeUnit();
	
	Vector s, u;
	
	s = view_vec;
	s *= up_vec;
	u = s;
	u *= view_vec;
	
	float mat[]=
	{
		s.x, u.x, -view_vec.x, 0,
		s.y, u.y, -view_vec.y, 0,
		s.z, u.z, -view_vec.z, 0,
		0, 0, 0, 1
	};
	
	glMultMatrixf(mat);
	glTranslatef (-eyeX, -eyeY, -eyeZ);
}
#endif

#ifdef ARCH_PSP

static unsigned int staticOffset = 0;

static unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm)
{
	switch (psm)
	{
		case GU_PSM_T4:
			return (width * height) >> 1;
			
		case GU_PSM_T8:
			return width * height;
			
		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return 2 * width * height;
			
		case GU_PSM_8888:
		case GU_PSM_T32:
			return 4 * width * height;
			
		default:
			return 0;
	}
}

void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
{
	unsigned int memSize = getMemorySize(width,height,psm);
	void* result = (void*)staticOffset;
	staticOffset += memSize;
	
	return result;
}

void* getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm)
{
	void* result = getStaticVramBuffer(width,height,psm);
	return (void*)(((unsigned int)result) + ((unsigned int)sceGeEdramGetAddr()));
}


#endif