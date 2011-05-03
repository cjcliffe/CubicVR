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

#ifndef CUBICVR_PARTICLEEMITTER
#define CUBICVR_PARTICLEEMITTER


#include <CubicVR/cvr_defines.h>


#include <CubicVR/XYZ.h>
#include <CubicVR/RGB.h>
#include <CubicVR/Particle.h>
#include <CubicVR/ObjectController.h>


#include <stdlib.h>


class ParticleEmitter : public ObjectController
{
protected:
	
public:
	float particle_velocity;

	ParticleEmitter():ObjectController(),particle_velocity(10)
	{
	
	}

	virtual void emit(Particle &newParticle, float timeIndex, float lifeSpan=8.0) = 0;
	virtual void govern(Particle &p_in, Particle &p_out, float timeIndex) = 0;
	virtual void bury(Particle &dead_particle) = 0;
};


class PointParticleEmitter : public ParticleEmitter
{
protected:
	
public:
	float particle_velocity;
	float grav;

	PointParticleEmitter();
	void emit(Particle &newParticle, float timeIndex, float lifeSpan=8.0);	
	
	void govern(Particle &p_in, Particle &p_out, float timeIndex);
	void bury(Particle &dead_particle);
};



/*
		// spray emitter

		double randx = (double)rand()/(double)RAND_MAX;
		double randz = (double)rand()/(double)RAND_MAX;
		double randspeed = (double)rand()/(double)RAND_MAX;

		double clr = ((double)rand()/(double)RAND_MAX);
		
		newParticle.color.r = 0; //((double)rand()/(double)RAND_MAX);
		newParticle.color.g = clr*0.58; //((double)rand()/(double)RAND_MAX);
		newParticle.color.b = clr*1.0;
		
		newParticle.velocity.x = 40.0 * (randx - 0.5);
		newParticle.velocity.z = 40.0 * (randz - 0.5);

		newParticle.velocity.y = 10 + 60.0 * randspeed;

		newParticle.accel.y = -9.8;
		newParticle.lifespan=10;
//		newParticle.size = 0.2+6.0*((double)rand()/(double)RAND_MAX);



*/


#endif