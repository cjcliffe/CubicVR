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

#ifndef CUBICVR_RGBA_H
#define CUBICVR_RGBA_H

#include <CubicVR/cvr_defines.h>

#include <CubicVR/RGB.h>

#ifdef ARCH_PSP
struct va_rgba
{
	unsigned char r, g, b, a;
};
#else
struct va_rgba
{
	float r, g, b, a;
};
#endif

class IMPEXP RGBA : public RGB
{
public:
	float a;
	
	RGBA();
	RGBA(float r_in, float g_in, float b_in, float a_in);
	~RGBA();
	
	friend inline bool operator==(const RGBA &c1, const RGBA &c2)
	{
		return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
	};	
	
	friend inline bool operator!=(const RGBA &c1, const RGBA &c2)
	{
		return c1.r != c2.r || c1.g != c2.g || c1.b != c2.b || c1.a != c2.a;
	};	
	
	inline RGBA &operator=(const RGB &pt)
	{
		r = pt.r;
		g = pt.g;
		b = pt.b;
		
		return *this;
	};
	
	inline RGBA &operator+=(const RGB &pt)
	{
		r += pt.r;
		g += pt.g;
		b += pt.b;
		
		return *this;
	};
	
	
	inline RGBA &operator-=(const RGB &pt)
	{
		r -= pt.r;
		g -= pt.g;
		b -= pt.b;
		
		return *this;
	};
	
	
	inline RGBA &operator+=(const double val)
	{
		r += (cvrFloat)val;
		g += (cvrFloat)val;
		b += (cvrFloat)val;
		a += (cvrFloat)val;
		
		return *this;
	};
	
	inline RGBA &operator+=(const float val)
	{
		r += val;
		g += val;
		b += val;
		a += val;
		
		return *this;
	};
	
	inline RGBA &operator+=(const long val)
	{
		r += val;
		g += val;
		b += val;
		a += val;
		
		return *this;
	};
	
	inline RGBA &operator+=(const int val)
	{
		r += val;
		g += val;
		b += val;
		a += val;
		
		return *this;
	};
	
	
	
	inline RGBA &operator-=(const double val)
	{
		r -= (cvrFloat)val;
		g -= (cvrFloat)val;
		b -= (cvrFloat)val;
		a -= (cvrFloat)val;
		
		return *this;
	};
	
	
	inline RGBA &operator*=(const float val)
	{
		r *= val;
		g *= val;
		b *= val;
		a *= val;
		
		return *this;
	};
	
	inline RGBA &operator-=(const float val)
	{
		r -= val;
		g -= val;
		b -= val;
		a -= val;
		
		return *this;
	};
	
	inline RGBA &operator-=(const long val)
	{
		r -= val;
		g -= val;
		b -= val;
		a -= val;
		
		return *this;
	};
	
	inline RGBA &operator-=(const int val)
	{
		r -= val;
		g -= val;
		b -= val;
		a -= val;
		
		return *this;
	};
	
	
	////
	
	inline RGBA &operator=(const RGBA &pt)
	{
		r = pt.r;
		g = pt.g;
		b = pt.b;
		a = pt.a;
		
		return *this;
	};
	
	inline RGBA &operator+=(const RGBA &pt)
	{
		r += pt.r;
		g += pt.g;
		b += pt.b;
		a += pt.a;
		
		return *this;
	};
	
	
	inline RGBA &operator-=(const RGBA &pt)
	{
		r -= pt.r;
		g -= pt.g;
		b -= pt.b;
		a += pt.a;
		
		return *this;
	};
	
	
	inline RGBA operator-(const RGBA &pt)
	{
		RGBA diff(*this);
		
		diff -= pt;
		
		return diff;
	};
	
	
	inline RGBA operator+(const RGBA &pt)
	{
		RGBA sum(*this);
		
		sum += pt;
		
		return sum;
	};
	
	
	inline RGBA operator-(const double val)
	{
		RGBA sum(*this);
		
		sum -= val;
		
		return sum;
	};
	
	inline RGBA operator+(const double val)
	{
		RGBA sum(*this);
		
		sum += val;
		
		return sum;
	};
	
	
	inline RGBA operator*(const float val)
	{
		RGBA mul(*this);
		
		mul *= val;
		
		return mul;
	};
	
	
};


#endif

