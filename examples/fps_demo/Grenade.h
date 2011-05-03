/*
 *  Grenade.h
 *  fps_demo
 *
 *  Created by Charles J. Cliffe on 26/02/09.
 *  Copyright 2009 Cubic Productions. All rights reserved.
 *
 */

#pragma once

#include <CubicVR/SceneObjects/RigidSceneObject.h>
#include <CubicVR/Timer.h>
#include <CubicVR/RigidParticleEmitter.h>


class Grenade : public RigidSceneObject
{
protected:
	SoundSample *bounceSound;
	SoundSample *rollSound;
	SoundSample *explodeSound;
	
	Timer *mBirthTime;
	
	ParticleSystem *particleSys;
	RigidParticleEmitter *pe;
	Timer *mTimer;
	
	float explosion_radius;
	float explosion_strength;
	float explosion_time;

	float bExplodeOnImpact;	
	
public:

	bool bExploded;
	
	Grenade(RigidSceneObject &rigid_template, Timer *mTimer_in, ParticleSystem *psys_in, RigidParticleEmitter *pe_in) : RigidSceneObject(), bounceSound(NULL), rollSound(NULL), explodeSound(NULL)
	{
		bExploded = false;
		
		bind(*rigid_template.obj);
		copyCollision(rigid_template);
		collisionEvents = true;
		
		mTimer = mTimer_in;
		
		mBirthTime = new Timer();
		mBirthTime->start();
	
		setExplodeOnImpact(false);
		setExplosionTime(3);
		setExplosionRadius(5);
		setExplosionStrength(25);

		particleSys = psys_in;
		pe = pe_in;

	}

	~Grenade()
	{
	}
	
	void setExplodeOnImpact(bool impactExplode) { bExplodeOnImpact = impactExplode; }
	void setExplosionRadius(float size) { explosion_radius = size; }
	void setExplosionStrength(float strength) { explosion_strength = strength; }
	void setExplosionTime(float seconds)
	{ 
		mBirthTime->update();
		explosion_time = mBirthTime->getSeconds() + seconds;
	}
	
	bool  getExplodeOnImpact() { return bExplodeOnImpact; }
	float getExplosionTime() { return explosion_time; }
	float getExplosionStrength() { return explosion_strength; }
	float getExplosionRadius() { return explosion_radius; }

	void explode()
	{
		if(!bExploded)
		{
			int i; 
			Particle *p;
			RGB clr[3];
			
			clr[0] = RGB(255.0/255.0, 110.0/255.0, 2.0/255.0);
			clr[1] = RGB(255.0/255.0, 255.0/255.0, 0.0/255.0);
			clr[2] = RGB(255.0/255.0, 0.0/255.0, 0.0/255.0);
			
			for (i = 0; i < 100; i++)
			{
				p = new Particle();
				
				pe->particle_velocity = 0.1;

				pe->setRotation(XYZ(sin(((float)rand()/(float)RAND_MAX)*M_PI*2.0),cos(((float)rand()/(float)RAND_MAX)*M_PI*2.0),-sin(((float)rand()/(float)RAND_MAX)*M_PI*2.0)));

				p->size = 0.1;
				p->position = getPosition()+(pe->getRotation()*0.1)+XYZ(0,0.2,0);
				
				
				p->color = clr[(int)(((float)rand()/(float)RAND_MAX)*3.0)];
				
				pe->emit(*p,mTimer->getSeconds(),1.5);
				
				particleSys->addParticle(*p);
			}

			bExploded = true;
//			printf("explode");
			float fGain = (getExplosionStrength() / 25);
			if(fGain > 3)		fGain = 3;
			
			explodeSound->setGain(fGain);
			explodeSound->play();
		}
	}
	
	virtual void handleCollision(RigidSceneObject *collision_obj, btPersistentManifold &contactManifold)
	{
		int numContacts = contactManifold.getNumContacts();
		
		for (int j=0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold.getContactPoint(j);
			
			if (pt.m_appliedImpulse > 0 && getExplodeOnImpact())		explode();
			
			if (pt.m_appliedImpulse > 2.0) 
			{
				if (bounceSound) if (!bounceSound->isPlaying())
				{
					bounceSound->setGain(pt.m_appliedImpulse/20.0);
					bounceSound->setPitch(0.85 + (0.2*((float)rand()/(float)RAND_MAX)));
					
					bounceSound->play();
				}
			}
			else if (pt.m_appliedImpulse > 0.7)
			{
				if (rollSound) if (!rollSound->isPlaying()) 
				{
					rollSound->setGain(pt.m_appliedImpulse/30.0);
					rollSound->setPitch(0.95 + (0.05*((float)rand()/(float)RAND_MAX)));
					
					rollSound->play();
				}
			}
		}		
	}
	
	void setBounceSound(SoundSample &bounceSound_in)
	{
		bounceSound = new SoundSample(bounceSound_in);
	}
	
	void setRollSound(SoundSample &rollSound_in)
	{
		rollSound = new SoundSample(rollSound_in);
	}

	void setExplosionSound(SoundSample &explodeSound_in)
	{
		explodeSound = new SoundSample(explodeSound_in);
	}
	
	void update(int lus)
	{
		mBirthTime->update();
		
//		printf("Grenade Update\n");
//		printf("%f %f\n", mBirthTime->getSeconds(), explosion_time);
		
		if(mBirthTime->getSeconds() >= explosion_time)
		{
			explode();
		}
	}
		
};