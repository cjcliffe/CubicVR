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

#ifndef CUBICVR_MOTION_H
#define CUBICVR_MOTION_H


#include <CubicVR/cvr_defines.h>
#include <CubicVR/Envelope.h>
#include <CubicVR/Resource.h>

#include <vector>
#include <map>



#define MOTION_X	0
#define MOTION_Y	1
#define MOTION_Z	2


#define CONTROLLER_POSITION 0
#define CONTROLLER_ROTATION	1
#define CONTROLLER_SCALE	2


class IMPEXP Controllable
{
private:
public:
	virtual ~Controllable() {};	
	virtual void control(int controllerId, int motionId, float value) = 0;
	virtual float read_control(int controllerId, int motionId) { return 0; };
};

struct ltu_int
{
  bool operator()(int i1, int i2) const
  {
    return i1 < i2;
  }
};


struct ltu_cptr
{
  bool operator()(Controllable *c1, Controllable *c2) const
  {
    return (unsigned long)c1 < (unsigned long)c2;
  }
};


class IMPEXP MotionController
{
private:
	std::map<int, std::map<int, Envelope, ltu_int>, ltu_int> controllers;	
	
public:
	MotionController();
	
	Envelope &envelope(int controllerId, int motionId);
	
	void apply(Controllable &objControllable, float index);
	void record(Controllable &objControllable, float index);
};


class IMPEXP Motion : public Resource
{
private:
	std::map<Controllable *, MotionController, ltu_cptr> controllers;
public:
	Motion();
	
	void bind(Controllable &objControllable);

	EnvelopeKey *setKey(Controllable &objControllable, int controllerId, int motionId, float index, float value);
	void setBehavior(Controllable &objControllable, int controllerId, int motionId, int behavior_in, int behavior_out);
	MotionController *getController(Controllable *c)
	{
		if (controllers.find(c) != controllers.end())
		{
			return &controllers[c];
		}
		
		return NULL;
	}
	
	
	void evaluate(float index);	
	void record(float index);
};


#endif

