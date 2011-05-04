/*

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

#ifndef CVR_DEFINES_H
#define CVR_DEFINES_H

#include <math.h>
#ifdef ARCH_DC
#include <fastmath.h>
#endif

#ifdef _WIN32
	
	#ifndef IMPEXP
//	#ifndef CUBICVR_EXPORTS
//		#define IMPEXP __declspec(dllimport)
//	#else
		#define IMPEXP
//	#endif
	#endif
	
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	
	#include <windows.h>
	#pragma warning(disable:4786)
	#pragma warning(disable:4251)
	#pragma warning(disable:4273)
	#pragma warning(disable:4099)
	#pragma warning(disable:4305)
	#pragma warning(disable:4996)
	#pragma warning (disable: 4820)
	
#else
	#define IMPEXP
#endif

/* PI values */


#define CVR_FLOAT_PRECISION


#ifdef CVR_FLOAT_PRECISION
	#define cvrFloat float

	#ifdef M_PI
	#undef M_PI
	#endif
	#define M_PI 3.14159265358979323846f

	#define DEGTORAD(x) (x*(3.14159265358979323846f/180.0f))
	#define RADTODEG(x) (x*(180.0f/3.14159265358979323846f))

	#define M_HALF_PI 1.57079632679489661923f
	#define M_TWO_PI 6.28318530717958647692f

	#define cos cosf
	#define sin sinf
	#define sqrt sqrtf
	#define tan tanf
	#define acos acosf
	#define asin asinf
	#define atan atanf
	#define atan2 atan2f
	#define fabs fabsf
	#define floor floorf
	#define ceil ceilf
	#define round roundf
	#define pow powf
#else
	#define cvrFloat double

	#ifndef M_PI
	#define M_PI 3.14159265358979323846
	#endif

	#define DEGTORAD(x) (x*(3.14159265358979323846/180.0))
	#define RADTODEG(x) (x*(180.0/3.14159265358979323846))

	#define M_HALF_PI 1.57079632679489661923
	#define M_TWO_PI 6.28318530717958647692
#endif



#if defined(ARCH_DC) || defined(ARCH_PSP) || defined(OPENGL_ES)
	#define cvrIndex unsigned int
#else
	#define cvrIndex unsigned int
//	#define cvrIndex unsigned long
#endif

#if defined(OPENGL_ES) || defined(ARCH_PSP)
	#define cvrElement unsigned short
#else
	#define cvrElement unsigned int
#endif


/* map/set numeric less-than functions */
struct ltulong
{
  bool operator()(unsigned long s1, unsigned long s2) const
  {
    return s1 < s2;
  }
};

struct ltuint
{
	bool operator()(unsigned int s1, unsigned int s2) const
	{
		return s1 < s2;
	}
};

struct ltindex
{
	bool operator()(cvrIndex s1, cvrIndex s2) const
	{
		return s1 < s2;
	}
};

struct ltushort
{
	bool operator()(unsigned short s1, unsigned short s2) const
	{
		return s1 < s2;
	}
};

struct ltufloat
{
  bool operator()(float s1, float s2) const
  {
    return s1 < s2;
  }
};

#endif


#ifdef ARCH_DC
#include <dc/matrix.h>
#include <kos/vector.h>

/*
#define mat_sqrtipr(x, y, z) ({ \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __result __asm__("fr3"); \
	__asm__ __volatile__( \
		"fldi0	fr3\n" \
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3\n" \
		: "=f" (__result) \
		: "f" (__x), "f" (__y), "f" (__z) \
		); \
	__result; \
})

	#define mat_trans_single(x, y, z) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"ftrv	xmtrx,fv0\n" \
		"fldi1	fr2\n" \
		"fdiv	fr3,fr2\n" \
		"fmul	fr2,fr0\n" \
		"fmul	fr2,fr1\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z) \
		: "0" (__x), "1" (__y), "2" (__z) \
		: "fr3" ); \
	x = __x; y = __y; z = __z; \
	}

#define mat_rot_single3(x, y, z) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"ftrv	xmtrx,fv0\n" \
		"fmov	fr3,fr2" \
		: "=f" (__x), "=f" (__y), "=f" (__z) \
		: "0" (__x), "1" (__y), "2" (__z) \
		: "fr3" ); \
	x = __x; y = __y; z = __z;  \
}

*/

#define sh4_dot_single3(x, y, z, a, b, c, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __a __asm__("fr4") = (a); \
	register float __b __asm__("fr5") = (b); \
	register float __c __asm__("fr6") = (c); \
	register float __d __asm__("fr7") = (d); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fldi1	fr7\n" \
		"fipr	fv0,fv4\n" \
		"fmov	fr7,fr0\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__a), "=f" (__b), "=f" (__c), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__a), "4" (__b), "5" (__c), "6" (__d) \
		: "fr3" ); \
	d = __d;  \
}



/*
float sh4_dot_single3(va_xyz *v1_xyz, va_xyz *v2_xyz)
{
  float v1[4], v2[4];
  float retval[1];

  memcpy(v1,v1_xyz,sizeof(float)*3);
  memcpy(v2,v2_xyz,sizeof(float)*3); 
  v1[3] = 0;
  v2[3] = 0;

  __asm(
    "fmov.s @r4+, fr0    ; Load v1 vector into fr0..fr3\n"
    "fmov.s @r4+, fr1\n"
    "fmov.s @r4+, fr2\n"
    "fmov.s @r4+, fr3\n"
    "fmov.s @r5+, fr4    ; Load v2 vector into fr4..fr7\n"
    "fmov.s @r5+, fr5\n"
    "fmov.s @r5+, fr6\n"
    "fmov.s @r5+, fr7\n"
    "fipr  fv0, fv4  ; Do the operation\n"
    "fmov.s  fr7, @r6      ; Store the return value\n", 
    v1,            // passed in R4 
    v2,            // passed in R5
    retval);          // passed in R6

    return retval[0];
}
*/

#endif

