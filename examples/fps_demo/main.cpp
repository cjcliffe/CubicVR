#include <CubicVR/GLExt.h>
#include <CubicVR/UVMapper.h>
#include <CubicVR/RigidScene.h>
#include <CubicVR/Timer.h>
#include <CubicVR/Mesh.h>
#include <CubicVR/objectLWO.h>
#include <CubicVR/SceneObjects/RigidSceneObject.h>
#include <CubicVR/SceneObjects/RigidCapsule.h>
#include <CubicVR/SceneObjects/RigidBox.h>
#include <CubicVR/SceneObjects/BSPSceneObject.h>
#include <CubicVR/RigidParticleEmitter.h>
#ifdef __linux__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif


#ifdef USE_OPENAL
#include "SoundKit.h"
#endif
#include "Weapon.h"
#include "Grenade.h"

bool bFullscreen = false;
#define kWindowWidth	1280
#define kWindowHeight	720
Camera myCamera(kWindowWidth,kWindowHeight, 65, 0.01, 1000);

MeshRenderer myShader;
RigidScene myScene(kWindowWidth,kWindowHeight);
Light myLight;
Mesh myBoxObj;
RigidCapsule playerObj;
Timer sceneTimer;
BSPSceneObject *bsp;

#ifdef USE_OPENAL
SoundSample jumpSound;
SoundSample thumpSound;
SoundSample metalSound;
SoundSample metalRoll;
SoundSample grenadeExplosionSound;
#endif

Weapon *weapon;

int keyPress[256];
bool specialKey[256];

void InitGL(void);
void DrawGLScene(void);
void ReSizeGLScene(int Width, int Height);
void makeBox(Mesh &boxObj, float box_size, unsigned int box_mat);


Material *particleMat;
ParticleSystem particleSys;
RigidParticleEmitter *pe;
Particle *p;

void initParticles()
{
	
	unsigned long particleTex = Texture::create("241-diffuse.png","particle_tex");
	unsigned long particleTexAlpha = Texture::create("particle_alpha.png","particle_tex_a");

	particleMat = new Material();

	Material *pMat = particleMat;

	pMat->color = RGBA(200.0/255.0,200.0/255.0,200.0/255.0,0.5);
	pMat->bindTexture(0,particleTex,TEXTURE_DIFF);
	pMat->bindTexture(1,particleTexAlpha,TEXTURE_ALPHA);

	pe = new RigidParticleEmitter(*myScene.getDynamicsWorld());
	pe->setPosition(XYZ(0,0,0));
}

float lp = 0;

void drawParticles()
{
	float lus = sceneTimer.lastUpdateSeconds(); 
	particleMat->use();
	GLShader::activateLight(LIGHT_NULL,0,particleMat->shader_mask,false);
		
	particleSys.viewSetup();
	particleSys.draw(*pe,sceneTimer.getSeconds());
	
}	






class GroundTest : public btCollisionWorld::ClosestRayResultCallback
{
public:
	GroundTest (btRigidBody* src) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		mSrc = src;
	}
	
	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == mSrc)
			return 1.0;
		
		return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace
														  );
	}
protected:
	btRigidBody* mSrc;
};

GroundTest *rayCallback;


Mesh *grenadeObj;
Mesh *grenadeObj_cmap;
RigidSceneObject *grenade_template;
std::set<Grenade *> grenades;

void loadGrenadeModel()
{	
	grenadeObj = new Mesh();
	grenadeObj_cmap = new Mesh();
	
//	loadLWO(*grenadeObj,"grenade.lwo");
//	loadLWO(*grenadeObj_cmap,"grenade_cmap.lwo");

	grenadeObj->cache(true);
	grenadeObj_cmap->cache(true);
	
	grenade_template = new RigidSceneObject(*grenadeObj,*grenadeObj_cmap);
	grenade_template->createRigidBody();
}

void tossGrenade()
{
	Grenade *tmpGrenade;
	
	tmpGrenade = new Grenade(*grenade_template,&sceneTimer,&particleSys,pe);
#ifdef USE_OPENAL
	tmpGrenade->setBounceSound(metalSound);
	tmpGrenade->setRollSound(metalRoll);
	tmpGrenade->setExplosionSound(grenadeExplosionSound);
#endif
	grenades.insert(tmpGrenade);
	
	btVector3 impulse;
	
	impulse.setX((float)sin(-myCamera.rotation.y*M_PI/180));
	impulse.setZ(-(float)cos(-myCamera.rotation.y*M_PI/180));
	impulse.setY((float)-sin(-(myCamera.rotation.x+15.0)*M_PI/180));

	impulse *= 10.0;
	Vector ofs;
	
	ofs = XYZ(impulse.getX(),impulse.getY(),impulse.getZ());
	ofs.makeUnit();
	ofs *=2.0;
	
	tmpGrenade->setPosition(playerObj.getPosition()+ofs);
	tmpGrenade->setPosition(XYZ(((float)rand()/(float)RAND_MAX)*360.0,((float)rand()/(float)RAND_MAX)*360.0,0));
	
	myScene.bind(*tmpGrenade);
	
	btRigidBody *impulsebody = &tmpGrenade->getRigidBody();
	
	impulsebody->applyImpulse(impulse,btVector3(0,0,-0.025));				
	impulsebody->setActivationState(ACTIVE_TAG);
}



void InitGL(void)
{
#if defined(_WIN32) || defined(__linux__)
	glewInit();	// initialize GLEW extension manager for windows
#endif


	GLShader::loadDefaultShader("shaders/cubicvr_default.v","shaders/cubicvr_default.f");
	GLShader::loadLightShader(LIGHT_DIRECTIONAL,"shaders/cubicvr_light_dir.v","shaders/cubicvr_light_dir.f");
	GLShader::loadLightShader(LIGHT_POINT,"shaders/cubicvr_light_point.v","shaders/cubicvr_light_point.f");
	GLShader::loadLightShader(LIGHT_SPOT,"shaders/cubicvr_light_spot.v","shaders/cubicvr_light_spot.f");

	// Set up the global ambient factor
	Light::setGlobalAmbient(RGB(0.0,0.0,0.0));
	
	// Set up our light
	myLight.setType(LIGHT_DIRECTIONAL);
	myLight.rotation = XYZ(0,25,45);
	
	// Set up our camera (targeted, looking at 0,0,0)
	myCamera.setType(CAMERA_FREE);
	myCamera.position = XYZ(1,3,1);
//	myCamera.rotation = XYZ(0,0,0);
//	myCamera.target = XYZ(2,0,-2);
	
	// Bind the light and camera
//	myScene.bind(myLight);
	myScene.bind(myCamera);


	char *fn = "akumaq3dm8/akumaq3dm8.bsp";
	
	bsp = new BSPSceneObject(fn);

	myScene.bind(*bsp);	

	
	loadGrenadeModel();

	
	/// PLAYER INIT
	playerObj.setScale(XYZ(0.5,0.6,1.0));
	playerObj.setPosition(XYZ(1.0,2.0,-1.0));
	
	myScene.bind(playerObj);

	btRigidBody &playerBody = playerObj.getRigidBody();
	
	playerBody.setAngularFactor(0);
	playerBody.setActivationState(DISABLE_DEACTIVATION);
//	playerBody.setCollisionFlags( playerBody.getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	playerBody.setFriction(0.01);
	playerBody.setDamping(0,100);
	playerObj.setMass(250);
//	playerBody.getRigidBody().setSleepingThresholds (0.0, 0.0);

	rayCallback = new GroundTest(&playerObj.getRigidBody());

	
	// Init GL clearing and depth buffering
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer

	glDepthFunc(GL_LESS);						// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
	sceneTimer.start();
	
	initParticles();
}


void DrawGLScene(void)
{   	
	glDepthMask(true);
	// Clear the display buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Render our scene
	myScene.render();
	
	drawParticles();
	
	// Flush output and swap buffers
    glFlush();	
	glutSwapBuffers();
#if __linux__
	usleep(6500);
#endif
}


float groundDist = 0.0;
int jumped = 0;
bool onGround;

float lastThump;
bool debugMode = false;

void Update(void)
{   
	float lus = sceneTimer.lastUpdateSeconds();
	sceneTimer.update();
	
	if (sceneTimer.getNumUpdates()%120 == 0)
	{
		printf("fps: %f\n",1.0/lus);
	}	
	
	std::set<Grenade *> ::iterator grenade_it;	
	std::set<Grenade *> removals;	
	
	for(grenade_it = grenades.begin(); grenade_it != grenades.end(); ++grenade_it)
	{
		(*grenade_it)->update(lus);

		if ((*grenade_it)->bExploded)
		{
			removals.insert(*grenade_it);
		}
	}
	
	for(grenade_it = removals.begin(); grenade_it != removals.end(); ++grenade_it)
	{
		myScene.unbind(**grenade_it);
		grenades.erase(*grenade_it);
		delete(*grenade_it);
	}
	
	float mLookSpeed = 80.0f;
	float mMoveSpeed = 6.5f;
	float mRotateSpeed = 150.0f;
	float mJumpHeight = 6.0f;
	float mCameraHeight = 0.5f;
	
	btRigidBody *impulsebody = &playerObj.getRigidBody();
	btVector3 impulse = impulsebody->getLinearVelocity();
	
	myCamera.setPosition(XYZ(playerObj.getPosition().x,playerObj.getPosition().y+mCameraHeight,playerObj.getPosition().z));

	rayCallback->m_closestHitFraction = 1.0;
	
	btVector3 m_raySource = playerObj.getPosition().cast();
	btVector3 m_rayTarget = playerObj.getPosition().cast()+btVector3(0,-1.0,0);
	
	myScene.getDynamicsWorld()->getCollisionWorld()->rayTest (m_raySource, m_rayTarget, *rayCallback);
	if (rayCallback->hasHit())
	{
		groundDist = rayCallback->m_closestHitFraction;
	} else {
		groundDist = 1.0;
	}
	
	if (specialKey[GLUT_KEY_RIGHT])
	{
		myCamera.rotation.y -= lus*mRotateSpeed;
	}
	
	if (specialKey[GLUT_KEY_LEFT])
	{
		myCamera.rotation.y += lus*mRotateSpeed;
	}

	bool isOnGround = (groundDist < 0.9);
	
	if (isOnGround && !onGround) 
	{
		float gain = -impulse.getY()/15.0;
		
		if (gain < 0.0) gain = 0; 
		if (gain > 0.9) gain = 0.9;

#ifdef USE_OPENAL
//		alSourcef (sndSourceThump, AL_GAIN,  gain     );
		thumpSound.setGain(gain);

		if (sceneTimer.getSeconds()-lastThump > 0.2 && impulse.getY()<=0.0) thumpSound.play();
		lastThump = sceneTimer.getSeconds();
#endif
	}
	
	onGround = isOnGround;
	
	
	if (specialKey[GLUT_KEY_DOWN] || specialKey[GLUT_KEY_UP] || keyPress[' '])
	{
		
		if (onGround) jumped = 0;
		
		
		impulsebody->setActivationState(DISABLE_DEACTIVATION);
		
		if(specialKey[GLUT_KEY_UP] && onGround)
		{
			impulse.setX(+(float)sin(-myCamera.rotation.y*M_PI/180)*mMoveSpeed);
			impulse.setZ(-(float)cos(-myCamera.rotation.y*M_PI/180)*mMoveSpeed);
		}
		
		if(specialKey[GLUT_KEY_DOWN] && onGround)
		{
			impulse.setX(-(float)sin(-myCamera.rotation.y*M_PI/180)*mMoveSpeed);
			impulse.setZ(+(float)cos(-myCamera.rotation.y*M_PI/180)*mMoveSpeed);
		}			
		
		//		impulse.normalize();
		//		impulse *= distance;
		
		//		impulse.setY(impulsebody->getLinearVelocity().getY());
		
		impulsebody->setLinearVelocity(impulse);//,impulsebody->getCenterOfMassPosition());
		
		
		
		if (keyPress[' '] && (onGround || ( impulse.getY() < 0.4 && impulse.getY() > 0.0 )) && jumped < 2)
		{
#ifdef USE_OPENAL
			jumpSound.setPitch(0.85 + (0.2*((float)rand()/(float)RAND_MAX))+(0.2*(float)jumped));
			jumpSound.play();
#endif
			impulse = btVector3(0,0,0);
			impulse.setY(mJumpHeight);
			keyPress[' '] = 0;
			impulsebody->applyImpulse(impulse,impulsebody->getCenterOfMassPosition());				
			jumped++;
		}
		
		
	}
	else
	{
		btRigidBody *impulsebody = &playerObj.getRigidBody();
		btVector3 impulse = btVector3(impulsebody->getLinearVelocity().getX()-(impulsebody->getLinearVelocity().getX()*lus*10.0),impulsebody->getLinearVelocity().getY(),impulsebody->getLinearVelocity().getZ()-(impulsebody->getLinearVelocity().getZ()*lus*10.0));	
		impulsebody->setLinearVelocity(impulse);
	}
	
	if(keyPress['q'])
	{
		float pitchLimit = 55;
		myCamera.rotation.x += lus * mLookSpeed;
		if(myCamera.rotation.x > pitchLimit) myCamera.rotation.x = pitchLimit;
	}
	if(keyPress['z'])
	{
		float pitchLimit = -75;
		myCamera.rotation.x += lus * -mLookSpeed;
		if(myCamera.rotation.x < pitchLimit) myCamera.rotation.x = pitchLimit;
	}
	
	if(keyPress['p'])
	{
		bsp->bspObject->disableVIS(true);
		bsp->bspObject->showAll(true);
		bsp->segmentMask->SetAll();
	}
	
	if(keyPress['i'])
	{
		bsp->bspObject->disableVIS(true);
	}
	
	if(keyPress['u'])
	{
		bsp->bspObject->disableVIS(false);
		bsp->bspObject->showAll(false);
	}
		
	
	if (keyPress['g'])
	{
		tossGrenade();		
		keyPress['g'] = 0;
	}

	if (keyPress[';'])
	{
		if (grenades.size()<30) tossGrenade();		
//		keyPress[';'] = 0;
	}
	
	
	if (keyPress['b'])
	{
		myScene.debug.details=!myScene.debug.details;
		keyPress['b'] = 0;
	}
	
	
	myScene.stepSimulation(sceneTimer.lastUpdateSeconds(),20);	
	
	DrawGLScene();
}



void ReSizeGLScene(int Width, int Height)
{
	myScene.setRenderDimensions(Width,Height);
}
	

void KeyboardFunc(unsigned char key, int x, int y) 
{
	if (key == 27) exit(0);
	keyPress[key] = true;
}


void releaseKey(unsigned char key, int x, int y) 
{
	if(keyPress['f'])
	{
		weapon->Fire();
	}
	
	if(keyPress['r'])
	{
		weapon->Reload();
	}
	
	keyPress[key] = false;
}



void SpecialKeyFunc(int key,int x,int y)
{
    specialKey[key]=true;
}

void SpecialKeyUpFunc(int key,int x,int y) 
{
    specialKey[key]=false;
}


int mouse_last_x, mouse_last_y;

void mouseEvent(int button, int state, int x, int y)
{
}


void mouseMotion (int x,int y)
{
	mouse_last_x = x;
	mouse_last_y = y;
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	if(bFullscreen)
	{
//		glutGameModeString( "1920x1080:32@60" );
		glutGameModeString( "1440x900:32@60" );
		glutEnterGameMode();
	}
	else
	{
		glutInitWindowSize (kWindowWidth, kWindowHeight);
		glutCreateWindow ("CubicVR - First Person Shooter Demo");
	}
	srand(1000);
	
	InitGL();

#ifdef USE_OPENAL
	if (SoundKit::init())
	{
		printf("Audio Init OK.\n");
	}
	else
	{
		printf("Audio Init Error.\n");
	}

	jumpSound.loadWav("audio/Grunt01.wav");
	thumpSound.loadWav("audio/drop.wav");
	metalSound.loadWav("audio/Metallic_Tumble.wav");
	metalRoll.loadWav("audio/metalriffzz__can.wav");
	grenadeExplosionSound.loadWav("audio/EXPLODE-grenade.wav");
#endif

	weapon = new Weapon();
	
	glutReshapeFunc(ReSizeGLScene);
	glutDisplayFunc(DrawGLScene);
	glutIdleFunc(Update);
	glutKeyboardFunc(KeyboardFunc);
	glutMouseFunc(mouseEvent);
	glutMotionFunc(mouseMotion);

	glutSpecialFunc(SpecialKeyFunc);
	glutSpecialUpFunc(SpecialKeyUpFunc);

	glutIgnoreKeyRepeat(1);
	
	glutKeyboardUpFunc(releaseKey);
	
	glutMainLoop();

#ifdef USE_OPENAL
	SoundKit::shutdown();
#endif

	return 0;
}
