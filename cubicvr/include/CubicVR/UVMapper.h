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

#ifndef CUBICVR_UVMAPPER
#define CUBICVR_UVMAPPER

#include <CubicVR/cvr_defines.h>

#include <CubicVR/XYZ.h>
#include <CubicVR/Vector.h>
#include <CubicVR/Mesh.h>


/* Projection enums */
#define UV_PROJECTION_UV 0
#define UV_PROJECTION_PLANAR	1
#define UV_PROJECTION_CYLINDRICAL	2
#define UV_PROJECTION_SPHERICAL	3
#define UV_PROJECTION_CUBIC	4

/* Axis enums */
#define UV_AXIS_X 0
#define UV_AXIS_Y 1
#define UV_AXIS_Z 2


class IMPEXP UVMapper
{
private:
	/* UVMapper tools */
	void rotatexyz(XYZ &rot_in, XYZ &point_in, XYZ &point_out);
	void xyz_to_h(float x,float y,float z,float *h);
	void xyz_to_hp(float x,float y,float z,float *h,float *p);
	float fract (float x);

public:
	/* UV rotation, scale and center */
	XYZ rotation;
	XYZ scale;
	XYZ center;
	
	/* Projection mode and Projection axis */
	unsigned short projection_mode;
	unsigned short projection_axis;
	
	/* wrap count, number of times WxH to wrap the texture */
	float wrap_w_count, wrap_h_count;
		
	UVMapper();
	~UVMapper();
	
	/// Apply this UVMapper to object obj on material materialRef to texture layer layerRef
	void apply(Mesh &obj, Material *materialRef, unsigned short layerRef);
	/// Apply this UVMapper to object obj on material materialRef to texture layer 0
	void apply(Mesh &obj, Material *materialRef);
	
	/// Get a U & V value for a point (all except cubic which requires surface normals 
	void getUV(XYZ &point_in, UV &uv_out);
	
	void setProjection(unsigned short proj_in);
	void setAxis(unsigned short axis_in);
	void setWrap(float w_in, float h_in);
	void setScale(const XYZ &scale_in);
	void setRotation(const XYZ &rotation_in);
	void setCenter(const XYZ &center_in);
};


#endif

