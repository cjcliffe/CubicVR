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

#include <CubicVR/ParticleSystem.h>

/* Check ParticleSystem.h, this class is in the experimental stage */

ParticleSystem::ParticleSystem()
{
	
}


unsigned long ParticleSystem::numParticles()
{
	return particles.size();
}


void ParticleSystem::addParticle(Particle &p_in)
{
	particles.push_front(new Particle(p_in));
};


void ParticleSystem::viewSetup(float defaultSize)
{
#ifndef ARCH_PSP
	float modelview[16];
	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	
	right.x = modelview[0]*defaultSize;
	right.y = modelview[4]*defaultSize;
	right.z = modelview[8]*defaultSize;
	
	up.x = modelview[1]*defaultSize;
	up.y = modelview[5]*defaultSize;
	up.z = modelview[9]*defaultSize;
	
	/*
	 a = center - (right + up) * size;
	 b = center + (right - up) * size;
	 c = center + (right + up) * size;
	 d = center - (right - up) * size;
	 */
#endif
}

void ParticleSystem::drawBegin()
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	glPushAttrib(GL_ENABLE_BIT);
#endif
#endif
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(0);
	glEnable(GL_COLOR_MATERIAL);
#endif
}

void ParticleSystem::drawEnd()
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	glPopAttrib();
#endif
#endif
#endif
}

void ParticleSystem::draw(ParticleEmitter &pe, float timeIndex)
{
	viewSetup(1.0f);
	
	Particle pTemp;
	
	std::list<Particle *>::iterator part_i;
	std::vector<std::list<Particle *>::iterator> deletions;
	
	drawBegin();
	
	for (part_i = particles.begin(); part_i != particles.end(); part_i++)
	{
		Particle *p = (*part_i);
		
		if (timeIndex >= p->birth && timeIndex < (p->lifespan + p->birth))
		{
			float tdelta;
			
			tdelta = timeIndex - p->birth;
			
			pTemp = (*p);
			
			pTemp.position.x = pTemp.position.x + (tdelta * pTemp.velocity.x) + (tdelta*tdelta*pTemp.accel.x);
			pTemp.position.y = pTemp.position.y + (tdelta * pTemp.velocity.y) + (tdelta*tdelta*pTemp.accel.y);
			pTemp.position.z = pTemp.position.z + (tdelta * pTemp.velocity.z) + (tdelta*tdelta*pTemp.accel.z);
			
			pe.govern(*p,pTemp,timeIndex);
			
//			drawParticle(pTemp);
			
			glColor4f(pTemp.color.r,pTemp.color.g,pTemp.color.b,pTemp.color.a);
			
			float rpux,rmux,rpuy,rmuy,rpuz,rmuz;
			
			if (pTemp.size != 1)
			{
				rpux = pTemp.size*(right.x+up.x);
				rmux = pTemp.size*(right.x-up.x);
				rpuy = pTemp.size*(right.y+up.y);
				rmuy = pTemp.size*(right.y-up.y);
				rpuz = pTemp.size*(right.z+up.z);
				rmuz = pTemp.size*(right.z-up.z);
			}
			else
			{
				rpux = right.x+up.x;
				rmux = right.x-up.x;
				rpuy = right.y+up.y;
				rmuy = right.y-up.y;
				rpuz = right.z+up.z;
				rmuz = right.z-up.z;
			}
			
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) 
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(pTemp.position.x-(rpux),pTemp.position.y-(rpuy),pTemp.position.z-(rpuz));
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(pTemp.position.x+(rmux),pTemp.position.y+(rmuy),pTemp.position.z+(rmuz));
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(pTemp.position.x+(rpux),pTemp.position.y+(rpuy),pTemp.position.z+(rpuz));
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(pTemp.position.x-(rmux),pTemp.position.y-(rmuy),pTemp.position.z-(rmuz));
			glEnd();
#endif
			
			
		}
		else if (timeIndex < p->birth)
		{
			// nothing, particle hasn't been born yet :)
		}
		else
		{
			pe.bury(*p);
			delete p;
			deletions.push_back(part_i);
		}
	}	
	
	drawEnd();
	
	std::vector<std::list<Particle *>::iterator>::iterator deletions_i;
	
	for (deletions_i = deletions.begin(); deletions_i != deletions.end(); deletions_i++)
	{
		particles.erase(*deletions_i);
	}	
	
};


void ParticleSystem::drawParticle(Particle &p)
{
#ifndef ARCH_PSP
	//		glColor3f(1,1,1);
	//		glColor4f(p.color.r,p.color.g,p.color.b,p.color.a);
	
	//		float gldiffuse[] = {p.color.r, p.color.g, p.color.b, p.color.a};
#ifndef ARCH_DC
	//		glMaterialfv(GL_FRONT, GL_DIFFUSE, gldiffuse);
#endif
	
	glColor4f(p.color.r,p.color.g,p.color.b,p.color.a);
	
	float rpux,rmux,rpuy,rmuy,rpuz,rmuz;
	
	if (p.size != 1)
	{
		rpux = p.size*(right.x+up.x);
		rmux = p.size*(right.x-up.x);
		rpuy = p.size*(right.y+up.y);
		rmuy = p.size*(right.y-up.y);
		rpuz = p.size*(right.z+up.z);
		rmuz = p.size*(right.z-up.z);
	}
	else
	{
		rpux = right.x+up.x;
		rmux = right.x-up.x;
		rpuy = right.y+up.y;
		rmuy = right.y-up.y;
		rpuz = right.z+up.z;
		rmuz = right.z-up.z;
	}
	
#ifdef OPENGL_ES
#warning OpenGL ES needs particle rendering routine
#else
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(p.position.x-(rpux),p.position.y-(rpuy),p.position.z-(rpuz));
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(p.position.x+(rmux),p.position.y+(rmuy),p.position.z+(rmuz));
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(p.position.x+(rpux),p.position.y+(rpuy),p.position.z+(rpuz));
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(p.position.x-(rmux),p.position.y-(rmuy),p.position.z-(rmuz));
	glEnd();
#endif
#endif
	//		glEnable(GL_COLOR_MATERIAL);
};

