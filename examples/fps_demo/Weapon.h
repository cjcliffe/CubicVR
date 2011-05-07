#pragma once 

class Weapon;
class Clip;
class Bullet;

class Bullet
{
	public:
	int SetDamage(int damage) 	{ mDamage = damage; }
	int GetDamage() 			{ return mDamage; }

	private:
	int mDamage;
};

class Clip
{
public:
	Clip()
	{
		mMaxBulletsInClip = 6;
		mNumBulletsInClip = mMaxBulletsInClip;
#ifdef USE_OPENAL		
		mFire.loadWav("audio/gun_shotgun.wav");
		mReload.loadWav("audio/gun_pump_action.wav");
		mEmpty.loadWav("audio/gun_empty.wav");
#endif
	}
	
	void Fire()
	{
		if(mNumBulletsInClip > 0)
		{
#ifdef USE_OPENAL
			mFire.setPitch(0.7 - (0.15*((float)rand()/(float)RAND_MAX)));
			mFire.play();
#endif
			mNumBulletsInClip--;
		}
		else
		{
#ifdef USE_OPENAL
			mEmpty.setPitch(1 - (0.1*((float)rand()/(float)RAND_MAX)));
			mEmpty.play();
#endif
		}
	}
	
	void Reload()
	{
#ifdef USE_OPENAL
		mReload.setPitch(1 - (0.15*((float)rand()/(float)RAND_MAX)));
		mReload.play();
#endif
		mNumBulletsInClip = mMaxBulletsInClip;
	}
	
	
	
	int GetAmmo() { return mNumBulletsInClip; }
	void SetAmmo(int ammo) { mNumBulletsInClip = ammo; }
	
private:
	int mMaxBulletsInClip;
	int mNumBulletsInClip;
#ifdef USE_OPENAL	
	SoundSample mFire;
	SoundSample mReload;
	SoundSample mEmpty;
#endif
};

class Weapon
{
public:
	Weapon()
	{
		mClip = new Clip();
	}
	
	void Fire()
	{
		mClip->Fire();
		printf("Firing...bullet count: %d\n", mClip->GetAmmo());
	}
	
	void Reload()
	{
		mClip->Reload();
	}
	
private:
	Clip *mClip;
};

class WeaponManager
{
public:
};