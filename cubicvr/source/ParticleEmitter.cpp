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

#include <CubicVR/ParticleEmitter.h>

/* Check ParticleEmitter.h, this class is in the experimental stage. */

PointParticleEmitter::PointParticleEmitter():ParticleEmitter(),particle_velocity(1.0f),grav(-9.8)
{
	
}

void PointParticleEmitter::emit(Particle &newParticle, float timeIndex, float lifeSpan)
{
	newParticle.birth = timeIndex;
	
	Vector vel;
	vel = rotation;
	vel *= particle_velocity;
	
	newParticle.velocity = vel;
	newParticle.position = position;
	
	newParticle.accel.y = grav;
	newParticle.lifespan = lifeSpan;
	newParticle.src_color = newParticle.color;
};


void PointParticleEmitter::govern(Particle &p_in, Particle &p_out, float timeIndex)
{	
	float tdelta;
	
	tdelta = timeIndex - p_in.birth;
	
	
	p_out.color = p_in.src_color+(p_in.colorDelta*(1.0/(p_in.lifespan/tdelta)));
	if (p_out.color.r < 0) p_out.color.r = 0;
	if (p_out.color.g < 0) p_out.color.g = 0;
	if (p_out.color.b < 0) p_out.color.b = 0;
	if (p_out.color.a < 0) p_out.color.a = 0;

	p_out.size = p_in.size+(p_in.sizeDelta*(1.0/(p_in.lifespan/tdelta)));

	
	//		if ((p_in.position.y < -10.5) || fabs(p_in.position.x) > 300 || fabs(p_in.position.z) > 300) 
	//		{
	//			p_out.lifespan = timeIndex-p_in.birth;
	//		}
};

void PointParticleEmitter::bury(Particle &dead_particle)
{
	// nothing
};
