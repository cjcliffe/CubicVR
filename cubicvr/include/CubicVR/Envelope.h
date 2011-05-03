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

#ifndef CUBICVR_ENVELOPE_H
#define CUBICVR_ENVELOPE_H

/*
	Portions adapted from the work of Ernie Wright from the lightwave SDK
*/

#include <CubicVR/cvr_defines.h>

#include <map>

#ifdef ARCH_PSP
#include <fastmath.h>
#else
#include <math.h>
#endif

/* map comparison function */
struct float_less
{
  bool operator()(float s1, float s2) const
  {
    return s1 < s2;
  }
};


#define MOTION_POSITION_X 0
#define MOTION_POSITION_Y 1
#define MOTION_POSITION_Z 2

#define MOTION_ROTATION_X 3
#define MOTION_ROTATION_Y 4
#define MOTION_ROTATION_Z 5

#define MOTION_SCALE_X 6
#define MOTION_SCALE_Y 7
#define MOTION_SCALE_Z 8


#define ENV_SHAPE_TCB   0
#define ENV_SHAPE_HERM  1
#define ENV_SHAPE_BEZI  2
#define ENV_SHAPE_LINE  3
#define ENV_SHAPE_STEP  4
#define ENV_SHAPE_BEZ2  5


#define ENV_BEH_RESET      0
#define ENV_BEH_CONSTANT   1
#define ENV_BEH_REPEAT     2
#define ENV_BEH_OSCILLATE  3
#define ENV_BEH_OFFSET     4
#define ENV_BEH_LINEAR     5


class IMPEXP EnvelopeKey
{
public:
	
	float value;
	float time;
	int shape;
	float tension;
	float continuity;
	float bias;
	float param[4];	
	
	EnvelopeKey *prev;
	EnvelopeKey *next;
	
	EnvelopeKey();
};


class IMPEXP Envelope
{
public:

	std::map<float, EnvelopeKey *, float_less> key_map;
	
	int nKeys;
	int in_behavior;
	int out_behavior;

	EnvelopeKey *keys;

	float evaluate(float time);
	
	Envelope();
	~Envelope();
	
	void behavior(int in_b, int out_b);	
	bool empty();
	
	EnvelopeKey *addKey(float time, float value);
	EnvelopeKey *addKey(float time);
	EnvelopeKey *firstKey()
	{
		EnvelopeKey *k = keys;
		if (k) while (k->prev) k = k->prev;
		return k;
	}
};


#endif

