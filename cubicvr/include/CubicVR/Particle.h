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

#ifndef CUBICVR_PARTICLE
#define CUBICVR_PARTICLE

#include <CubicVR/cvr_defines.h>

#include <CubicVR/XYZ.h>
#include <CubicVR/RGBA.h>
#include <btBulletDynamicsCommon.h>

class IMPEXP Particle
{
	public:
		XYZ position, velocity, accel;
		RGBA color,src_color,colorDelta;
		float sizeDelta;
		
		float size,birth,lifespan;
		
		int sequence_id;
		btRigidBody *mRigidBody;

		
		Particle() : birth(0), lifespan(0), mRigidBody(NULL)
		{
			position = XYZ(0,0,0);
			velocity = XYZ(0,0,0);
			accel = XYZ(0,0,0);
			color = RGBA(1,1,1,1);
			colorDelta = RGBA(0,0,0,0);
			size = 1;
		};
		
		Particle &operator=(const Particle &eq_point)
		{
			position = eq_point.position;
			velocity = eq_point.velocity;
			accel = eq_point.accel;
			color = eq_point.color;
			size = eq_point.size;
			birth = eq_point.birth;
			lifespan = eq_point.lifespan;
			sequence_id = eq_point.sequence_id;
			mRigidBody = eq_point.mRigidBody;
			
			return *this;
		};

		
};


#endif