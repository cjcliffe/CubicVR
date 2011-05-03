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

#include <CubicVR/XYZRGB.h>


XYZRGB::XYZRGB() : XYZ(), RGB()
{

};


XYZRGB::XYZRGB(float x_in, float y_in, float z_in, float r_in, float g_in, float b_in) : XYZ(x_in, y_in, z_in), RGB(r_in, g_in, b_in)
{

};


XYZRGB::~XYZRGB()
{

};


XYZRGB &XYZRGB::operator=(const XYZ &eq_xyz)
{
	x = eq_xyz.x;
	y = eq_xyz.y;
	z = eq_xyz.z;
	
	return *this;
};


XYZRGB &XYZRGB::operator+(const XYZ &add_xyz)
{
	(XYZ)*this += add_xyz; 
	
	return *this;
};


XYZRGB &XYZRGB::operator-(const XYZ &add_xyz)
{
	(XYZ)*this -= add_xyz; 
	
	return *this;
};


XYZRGB &XYZRGB::operator=(const RGB &eq_rgb)
{
	r = eq_rgb.r;
	g = eq_rgb.g;
	b = eq_rgb.b;
	
	return *this;
};


XYZRGB &XYZRGB::operator=(const XYZRGB &eq_xyzrgb)
{
	x = eq_xyzrgb.x;
	y = eq_xyzrgb.y;
	z = eq_xyzrgb.z;

	r = eq_xyzrgb.r;
	g = eq_xyzrgb.g;
	b = eq_xyzrgb.b;
	
	return *this;
};

