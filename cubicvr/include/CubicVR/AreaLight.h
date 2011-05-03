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
#include <CubicVR/Light.h>

class Camera;

class AreaLight: public Light
{
public:
	Light oLight[3];
	Camera *cam;
	XYZ shadow_vec;
	
	unsigned int near_res, far_res, near_range, far_range;
	float ceiling, floor;
	
	
	
	AreaLight(Camera *cam_in, unsigned int near_map_res, unsigned int far_map_res, float near_map_range, float far_map_range, float ceiling_height, float floor_height);	
	void update();
	void setShadowVector(XYZ vec);
	
};
