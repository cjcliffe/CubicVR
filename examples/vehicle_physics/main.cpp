#include <CubicVR/GLExt.h>
#include <CubicVR/Mesh.h>
#include <CubicVR/Material.h>
#include <CubicVR/UVMapper.h>
#include <CubicVR/Timer.h>
#include <CubicVR/objectLWO.h>
#include <CubicVR/sceneLWS.h>
#include <CubicVR/SceneObjects/Landscape.h>
#include <CubicVR/SceneObjects/RigidSceneObject.h>
#include <CubicVR/SceneObjects/Vehicle.h>
#include <CubicVR/SceneObjects/RigidBox.h>
#include <CubicVR/SceneObjects/RigidSphere.h>
#include <CubicVR/RigidScene.h>
#include <CubicVR/ParticleSystem.h>
#include <CubicVR/ParticleEmitter.h>
#include <CubicVR/ResourceScene.h>
#include <CubicVR/ResourceManager.h>
#include <CubicVR/AreaLight.h>
#ifdef __linux__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif


#define USE_PARALLEL_DISPATCHER 1

float baselineRPM = 1300.0;
int engineRPM = 0;
int engineMaxRPM = 6000;

#include <btBulletDynamicsCommon.h>

void CreateKey();

int frame_counter = 0;
bool bFullscreen = false;
bool deferred = true;
bool record_mode = false;
#define kFullScreenRes	"1280x720:32@60"

//#define kWindowWidth	480
//#define kWindowHeight	272

//#define kWindowWidth	720
//#define kWindowHeight	480

#define kWindowWidth	1280
#define kWindowHeight	720

#define VIEWTARGET_CAR 1
#define VIEWTARGET_BRUSH 2

#define VIEW_TARGET_MAX 2

int viewTargetId = 1;
float skidTimer = 0.0;
float heightMove=0;

//RigidScene testScene(kWindowWidth,kWindowHeight);
ResourceScene *testScene;
ResourceManager *rm;
Timer myTimer;
Light myLight;
Camera myCamera(kWindowWidth,kWindowHeight, 60, 1, 1500);
Camera dummyCam(kWindowWidth,kWindowHeight, 60, 1, 1500);
Landscape *myLandscape;
Mesh myObj;
Mesh skySphere;
SceneObject skyObj;
SceneObject *targetObj;
AreaLight *aLight;
Material *particleMat;
unsigned long particleTexAlpha;
//Light dirLight;
//Light dirLight1;
//Light dirLight2;

ParticleSystem particleSys;
PointParticleEmitter pe;
Particle *p;


Mesh boxObj;

Mesh *meshObj;
Mesh *meshCollisionObj;
Mesh *wheelObj;

Vehicle *car;

SceneObject box;

Mesh sphereObj;

Scene sky(kWindowWidth,kWindowHeight);

bool trackCar = true;
bool sectorDebug = false;

int lines_enabled = 0;
int overview_enabled = 0;
float brushSize = 8.0f;
float funcScale = 1.0f;
float sizeChange = 0.0;

float meta_xmove = 0, meta_zmove = 0;
int mouseActive = 0;

int active_function = -1;
int active_tool = 0;

float dir = 1;
float dirMotion = 0;


void InitGL(void);
void DrawGLScene(void);
void ReSizeGLScene(int Width, int Height);


bool keyPressed[256]={ false };
bool normalKeyPressed[256]={ false };

//keep track of the shapes, we release memory at exit.
//make sure to re-use collision shapes among rigid bodies whenever possible!
btAlignedObjectArray<btCollisionShape*> collisionShapes;


void CreateTower(int cnt_x, int cnt_y, int cnt_z, XYZ offset, XYZ size = XYZ(1,1,1));

bool brakeButton = false;
bool revButton = false;
bool gasButton = false;
float gasValue = 0.0;
float turnValue = 0.0;
bool rocketValue = false;

void joystickFunc (unsigned int buttonMask, int x, int y, int z)
{
	float dx = (float)x/1000.0;
	float dy = (float)y/32767.0;
	float dz = ((float)z+1000.0)/2000.0;
	
	//	printf("%.2f, %.2f, %.2f\n", dx,dy,dz);
	
	float lus = myTimer.lastUpdateSeconds();
	
	gasValue = dz;

	if (fabs(dx) > 0.5)
	{
		turnValue = (-dx/10.0)*lus;
	}
	else
	{
		turnValue = 0.0;
	}
	
	//	if (fabs(dx) > 0.20) // dead zone
	//	{	
	//		camera.angleYaw+=((float)dx*80.0)*lus;
	//	}
	//	
	//	if (fabs(dy) > 0.20) // dead zone
	//	{	
	//		camera.anglePitch-=((float)dy*80.0)*lus;
	//	}
	//	
	//	if (fabs(dz) > 0.10)
	//	{
	//		camera.position.x += dz*(float)sin(camera.angleYaw*M_PI/180)*lus*5.0;
	//		camera.position.y += dz*(float)-sin(camera.anglePitch*M_PI/180)*lus*5.0;
	//		camera.position.z -= dz*(float)cos(camera.angleYaw*M_PI/180)*lus*5.0;
	//	}
	
	//	if (buttonMask & 16) printf("16\n");	// START 
	if (buttonMask & 32)	// BACK 
	{
		CreateKey();		
	}
	//	if (buttonMask & 64) printf("64\n");	// L3 (analog)
	//	if (buttonMask & 128) printf("128\n");		// R3 (analog)
	//	if (buttonMask & 256) printf("256\n");		// L1
	//	if (buttonMask & 512) printf("512\n");		// L2
	
	if (buttonMask & 1024) // XBOX BUTTON
	{
//		engine.setPitch(0.85 + (0.2*((float)rand()/(float)RAND_MAX))+(0.2*(float)jumped));
	}
	
	//	if (buttonMask & 2048) printf("2048\n");	// A 
	//	if (buttonMask & 4096) printf("4096\n");	// B 
	//	if (buttonMask & 8192) printf("8192\n");	// X 
	
		if (buttonMask & 16384) // Y 
		{
			rocketValue = true;
		}
		else	rocketValue = false;
	
	//	if (buttonMask & 32768) printf("32768\n");  
	

	if (buttonMask & 2048 && !brakeButton) 
	{
		brakeButton = true;
	}
	else if (!(buttonMask & 2048))
	{
		brakeButton = false;
	}

	if (buttonMask & 8192 && !revButton) 
	{
		revButton = true;
	}
	else if (!(buttonMask & 8192))
	{
		revButton = false;
	}
	
	
	gasButton = (buttonMask & 512);
	
//	if (buttonMask & 2048 && !gasButton) 
//	{
//		gasButton = true;
//	}
//	else if (!(buttonMask & 2048))
//	{
//		gasButton = false;
//	}
		
	// D-PAD UP
	if (buttonMask & 1)			{ gasValue = 1.0f; brakeButton = false; }
	// D-PAD DOWN
	if (buttonMask & 2)			{ gasValue = 0.0f; brakeButton = true; }
	// D-PAD LEFT
	if (buttonMask & 4)			turnValue = 1.0f;
	// D-PAD RIGHT
	else if (buttonMask & 8)	turnValue = -1.0f;	
}



void CreateTower(int cnt_x, int cnt_y, int cnt_z, XYZ offset, XYZ size)
{
	RigidSceneObject *myRigidBody;
	
//	XYZ size = XYZ(1.5,1.5,2.0);
				
	for(int y = 1; y <= cnt_y; y++)
	{
		for(int z = 1; z <= cnt_z; z++)
		{
			for(int x = 1; x <= cnt_x; x++)
			{
				float spacing = 0.0001;
				float heightSpacing = 0.0001;
				
				XYZ pos = XYZ(x * size.x + (spacing * (x-1.0)), y * size.y  + (heightSpacing * (y-1.0)), z * size.z + (spacing * (z-1.0)));
				
				pos += offset;
				
				myRigidBody = new RigidBox();
				myRigidBody->bind(boxObj);
				myRigidBody->setPosition(pos);
				myRigidBody->setScale(size);
				myRigidBody->setMass(20);
				
				testScene->bind(*myRigidBody);
			}
		}
	}
}


void InitBullet()
{	
}

void CreateKey()
{
	Vector vec;
	
	vec = myCamera.target;
	vec += XYZ(0,4,0);
	vec -= myCamera.position;
	vec.makeUnit();

	XYZ pos = myCamera.position;
	
	RigidBox *myRigidBody;

	myRigidBody = new RigidBox();

	myRigidBody->setPosition(pos);
	myRigidBody->setScale(XYZ(((rand()/(float)RAND_MAX)*10.0)+2.0,((rand()/(float)RAND_MAX)*10.0)+2.0,((rand()/(float)RAND_MAX)*10.0)+2.0));	
	myRigidBody->setMass(10);


	myRigidBody->bind(boxObj);

	testScene->bind(*myRigidBody);
	
	btVector3 force = btVector3(vec.x,vec.y,vec.z)*30000.0f;
	
	myRigidBody->getRigidBody().activate(true);
	myRigidBody->getRigidBody().applyForce(force, btVector3(0,0,0));
}


void updateParticles()
{
	btRigidBody *groundObj,*landscapeObj = &myLandscape->getRigidBody();
	
	if (record_mode)
	{
		skidTimer+=myTimer.lastUpdateSeconds()/2.0;
	}
	else
	{
		skidTimer+=myTimer.lastUpdateSeconds();	
	}
	
	if (!myTimer.paused() && ((skidTimer > (1.0/30.0)) || record_mode)) 
	{
		for (int ip = 0; ip < 4; ip++) // && normalKeyPressed['\t']
		{
			p = new Particle();
			
			btVector3 raySrc = car->getWheelGroundPosition(ip).cast()+btVector3(0,car->getWheel(ip)->getWheelRadius()*1.15,0);
			btVector3 rayDst = car->getWheelGroundPosition(ip).cast()-btVector3(0,car->getWheel(ip)->getWheelRadius()*1.15,0);
			
			btCollisionWorld::ClosestRayResultCallback rayCallback(raySrc,rayDst);
			testScene->getDynamicsWorld()->rayTest(raySrc,rayDst,rayCallback);
			
			//		printf("ray: %f,%f,%f  -  cam pos: %f, %f, %f\n",rayTo.getX(),rayTo.getY(),rayTo.getZ(),myCamera.position.x,myCamera.position.y,myCamera.position.z);
			
			if (rayCallback.hasHit())
			{
				groundObj = btRigidBody::upcast(rayCallback.m_collisionObject);
			}
			
			if (landscapeObj == groundObj)
			{
				p->color = RGBA(138.0/255.0,154.0/255.0,82.0/255.0,0.2);
				p->colorDelta = RGBA(-0.3,-0.4,-0.2,-0.2);
				p->sizeDelta = 2.0;
			}
			else
			{
				p->color = RGBA(186.0/255.0,186.0/255.0,176.0/255.0,0.2);
				p->colorDelta = RGBA(-0.3,-0.4,-0.2,-0.2);
				p->sizeDelta = 2.0;
			}
			
			float xo,zo;
			
			xo = ((rand()/(float)RAND_MAX))*1.0f;
			zo = ((rand()/(float)RAND_MAX)-0.5f)*1.0f;
			
			p->size = (1.0-car->getWheelSkid(ip))/3.0;
			pe.setPosition(car->getWheelGroundPosition(ip)-XYZ(0,p->size/2.0,0));
			pe.grav = -0.005;
			
			if (car->getWheelSkid(ip) < 0.26)
			{		
				//pe.setRotation(XYZ(xo,5.0f,zo));
				pe.setRotation(XYZ(0,xo,0));
				pe.emit(*p,myTimer.getSeconds(),4.0);
				particleSys.addParticle(*p);
			}
		}
		
		skidTimer=0;
	}	
}



void UpdatePhysics()
{
	float xmv,ymv;
	float lus = myTimer.lastUpdateSeconds();
	
	if (viewTargetId == VIEWTARGET_BRUSH)
	{
		float mvSpd = (brushSize)*lus;
		xmv = keyPressed[GLUT_KEY_LEFT]?-mvSpd:0 + keyPressed[GLUT_KEY_RIGHT]?mvSpd:0;
		ymv = keyPressed[GLUT_KEY_UP]?-mvSpd:0 + keyPressed[GLUT_KEY_DOWN]?mvSpd:0;
	}
	
	float fEngineForce = 1300.0;

	if (viewTargetId == VIEWTARGET_CAR)
	{
		if (keyPressed[GLUT_KEY_UP])   { gasButton = true; gasValue = 1.0f; } else { gasButton = false; }
		if (keyPressed[GLUT_KEY_DOWN]) { revButton = true; } else revButton = false;

		if (gasButton)
		{
			if (gasValue) car->setEngineForce(gasValue*fEngineForce);
		}
		else
		{
			car->setEngineForce(car->gEngineForce*0.95*lus);
		}
		if (revButton) car->setEngineForce(-fEngineForce * 1.25);
		if (turnValue) car->incSteering(turnValue*10.0);

		if (keyPressed[GLUT_KEY_LEFT]) car->incSteering(0.5f*lus);
		if (keyPressed[GLUT_KEY_RIGHT]) car->incSteering(-0.5f*lus);
		
		if(gasValue)
		{
			engineRPM += gasValue*(fEngineForce)*lus;
			if(engineRPM > engineMaxRPM)
			{
				// Switch to get RPM for gear ratios
				engineRPM = engineMaxRPM;
			}
		}

		engineRPM -= (fEngineForce*0.5)*lus;
		if(engineRPM < 0)	engineRPM = 0;
	}

	if(rocketValue)
	{
		btVector3 impulse(0,1,0);
		impulse.normalize();
		
		float impulseStrength = 200.0f;
		impulse *= impulseStrength;
		
		car->getRigidBody().setActivationState(ACTIVE_TAG);
		car->getRigidBody().applyImpulse(impulse,btVector3(0,0,0));		
	}
	
	if (normalKeyPressed['\t']) 
	{
		XYZ rot;
		rot = car->getWorldRotation();
		
		ObjectController ctmp;
		
		XYZ fwd_src = XYZ(car->gVehicleSteering*2.0,-0.2,1),fwd;
		ctmp.setRotation(rot);
		ctmp.transformBegin(false,false,true);
		matTransform(ctmp.w_trans,fwd_src,fwd);

		XYZ bak_src = XYZ(0,0,-0.5),bak;
		ctmp.setRotation(rot);
		ctmp.transformBegin(false,false,true);
		glPushMatrix();
		glLoadMatrixf(ctmp.w_trans);
		glPushMatrix();
		glTranslatef(0,0,-1);
		glGetFloatv(GL_MODELVIEW_MATRIX,ctmp.w_trans);
		glPopMatrix();
		glPopMatrix();
		
		XYZ thruster,tmpxyz;
		
		tmpxyz=XYZ(0.0,0,-1.4);
		
		matTransform(ctmp.w_trans,tmpxyz,thruster);
		matTransform(ctmp.w_trans,bak_src,bak);
		
		btVector3 impulse = fwd.cast();
		impulse.normalize();
		
		float impulseStrength = 8500.0f*lus;
		
		impulse *= impulseStrength;
		car->getRigidBody().setActivationState(ACTIVE_TAG);
		car->getRigidBody().applyImpulse(impulse,btVector3(0,0,-1.6));		
		
		p = new Particle();

		p->color = RGBA(255/255.0,115/255.0,36.0/255.0,0.9);
		p->colorDelta = RGBA(255/255.0,255/255.0,255.0/255.0,-0.2)*10.0 - p->color;
		p->sizeDelta = 4.0;
		
		float xo = ((rand()/(float)RAND_MAX)-0.5f)*1.0f;
		float yo = ((rand()/(float)RAND_MAX)-0.5f)*1.0f;
		float zo = ((rand()/(float)RAND_MAX)-0.5f)*1.0f;

		p->size = 0.75;
		pe.setPosition(car->getWorldPosition()+thruster+(xo,yo,zo));
		pe.grav = -0.005;
		
		pe.setRotation(bak);
		pe.emit(*p,myTimer.getSeconds(),8.0);
		particleSys.addParticle(*p);
	}
	
	if (!keyPressed[GLUT_KEY_RIGHT] || !keyPressed[GLUT_KEY_LEFT] || viewTargetId != VIEWTARGET_CAR) car->gVehicleSteering -= car->gVehicleSteering*0.95*lus;

	if (normalKeyPressed[' ']) car->setBrake(fEngineForce * 0.25); else car->setBrake(0);

//	if (normalKeyPressed['k']) myTimer.setSeconds(myTimer.getSeconds()-myTimer.lastUpdateSeconds());
//	if (normalKeyPressed['l']) myTimer.setSeconds(myTimer.getSeconds()+myTimer.lastUpdateSeconds());

	if (normalKeyPressed['l']) 
	{
		if (myTimer.locked()) 
		{
			myTimer.unlock(); 
			record_mode = false;
		}
		else 
		{
			myTimer.lockFramerate(30.0);
			record_mode = true;
		}
		normalKeyPressed['l'] = false;
	}
	
	myCamera.moveViewRelative(targetObj->getPosition(),xmv*lus,ymv*lus);
	
	btTransform trans;

	if (record_mode) // double time the physics at 60fps while the scene runs at 30 for better physics accuracy
	{
//		if (!myTimer.paused()) testScene->stepSimulation(myTimer.lastUpdateSeconds()/2.0, 15);
//		if (!myTimer.paused()) testScene->stepSimulation(myTimer.lastUpdateSeconds()/2.0, 15);
		if (!myTimer.paused()) testScene->stepSimulation(1.0/60.0, 15);
		updateParticles();
		if (!myTimer.paused()) testScene->stepSimulation(1.0/60.0, 15);
		updateParticles();
	
	}
	else if (!myTimer.paused()) 
	{
		updateParticles();
		testScene->stepSimulation(myTimer.lastUpdateSeconds(), 15);
	}
	
	
	switch (viewTargetId)
	{
		case VIEWTARGET_BRUSH: dummyCam.setTarget(targetObj->getPosition()); break;
		case VIEWTARGET_CAR: dummyCam.setTarget(car->getPosition() + XYZ(0,0.3,0)); break;
	}

	myCamera.setTarget(dummyCam.position);

	

}

void SpecialKeyFunc(int key,int x,int y)
{
    keyPressed[key]=true;
}

void SpecialKeyUpFunc(int key,int x,int y) // tagged to glutSpecialUpFunc()
{
    keyPressed[key]=false;
}

void mkLandscape()
{
	Material *objMat = new Material();

	unsigned long objTex = Texture::create("grass.png","diff");
	
	objMat->bindTexture(0,objTex,TEXTURE_DIFF);
	objMat->setMaxSmooth(60);
	objMat->setShininess(1);

	
	myLandscape = new Landscape(1400.0,80,80,1000,objMat);
	myLandscape->shadowCast(false);
//	myLandscape->function(1,10.0,25.0,100);
//	myLandscape->function(3,10.0,15.0,100);
//	myLandscape->function(1,10.0,20.0,150);


	// shut off landscape here
	myLandscape->setPosition(XYZ(0,0,0));
	testScene->bind(*myLandscape);
}

Vehicle *MakeDefaultVehicle(ResourceManager *rmPointer, std::string xmlFile = "stunt-car1.xml")
{
	Vehicle *pCar = new Vehicle();

	meshObj = new Mesh();
	wheelObj = new Mesh();
	meshCollisionObj = new Mesh();
	
	loadLWO(*meshObj,"car_default.lwo");
	meshObj->setId("car_default");
	meshObj->triangulate();
	meshObj->cache(true);
	
	loadLWO(*wheelObj,"wheel.lwo");
	wheelObj->setId("wheel");
	wheelObj->triangulate();
	wheelObj->cache(true);
	
	loadLWO(*meshCollisionObj,"car_default_cmap.lwo");
	meshCollisionObj->setId("car_default_cmap");
	meshCollisionObj->triangulate();		

	rmPointer->addResource(*meshObj,true);
	rmPointer->addResource(*wheelObj,true);
	rmPointer->addResource(*meshCollisionObj,true);
	
	pCar->setMeshModelId("car_default");
	pCar->setCollisionModelId("car_default_cmap");
	pCar->setId("stuntcar1");
	
	VehicleWheel front_left, front_right, back_left, back_right;
	
	front_left.setWheelModelId("wheel");
	front_right.setWheelModelId("wheel");
	back_left.setWheelModelId("wheel");
	back_right.setWheelModelId("wheel");
	
	float front_axel = 1.653589;
	float back_axel = -1.243587;
	float axel_width = 1.02631;
	float axel_height = -0.043902;
	float connection_height = 0.15;
	float suspension_rest = 0.15;
	
	front_left.setWheelPosition(XYZ(axel_width, axel_height-connection_height, front_axel));
	front_left.setSteering(true);
	front_left.setBraking(false);
	front_left.setDriving(true);
	front_left.setSuspensionRest(suspension_rest);
	front_left.setFrictionSlip(1.0);
	//	front_left.setDriving(true);
	//	front_left.setBraking(true);
	
	front_right.setWheelPosition(XYZ(-axel_width, axel_height-connection_height, front_axel));
	front_right.setWheelRotation(XYZ(0,180,0));
	front_right.setSteering(true);
	front_right.setBraking(false);
	front_right.setDriving(true);
	front_right.setSuspensionRest(suspension_rest);
	front_right.setFrictionSlip(1.0);
	//	front_right.setDriving(true);
	//	front_right.setBraking(true);
	
	back_left.setWheelPosition(XYZ(axel_width, axel_height-connection_height, back_axel));
	back_left.setDriving(false);
	back_left.setBraking(true);
	back_left.setSuspensionRest(suspension_rest);
	back_left.setFrictionSlip(0.94);
	
	back_right.setWheelPosition(XYZ(-axel_width, axel_height-connection_height, back_axel));
	back_right.setWheelRotation(XYZ(0,180,0));
	back_right.setDriving(false);
	back_right.setBraking(true);
	back_right.setSuspensionRest(suspension_rest);
	back_right.setFrictionSlip(0.94);
	
	pCar->addWheel(0,&front_left);
	pCar->addWheel(1,&front_right);
	pCar->addWheel(2,&back_left);
	pCar->addWheel(3,&back_right);
	
	pCar->onSave();
	pCar->SaveToFileXML(xmlFile);
	pCar->LoadFromFileXML(xmlFile);
	
	rmPointer->addResource(*pCar);
	pCar = (Vehicle *)rmPointer->upcast(pCar);
	
	return pCar;
}

//Vehicle *LoadVehicleXML(ResourceManager *rmPointer, std::string xmlFile)
//{
//	Vehicle *pCar = new Vehicle();
//	pCar->LoadFromFileXML(xmlFile);
//	
//	rmPointer->addResource(*pCar);
//	pCar = (Vehicle *)rmPointer->upcast(pCar);
//	
//	return pCar;
//}

//bool PreloadAssets(ResourceManager *rmPointer)
//{
//	rmPointer->pool("resources").addPath(string("./Vehicles/resources").c_str(),true);
//	rmPointer->pool("resources").addPath(string("./Stunts/resources").c_str(),true);
//	rmPointer->loadResourcePool("resources");
//	return true;
//}

bool InitResources(bool preload)
{
	rm = new ResourceManager();
//	if(preload)		return PreloadAssets(rm);
	return true;
}

ResourceScene *LoadTrack(ResourceManager *rmPointer, std::string trackName="")
{	
	ResourceScene *tempScene = new ResourceScene(rmPointer,kWindowWidth,kWindowHeight);
//	tempScene->LoadFromFileXML("./"+trackName+"/scenes/default/scene.xml");
	
	loadLWS(*tempScene,"track1.lws");
	tempScene->unbind(tempScene->light("Light"));
	tempScene->motion("track1.lws").evaluate(0);
	
	tempScene->initSectorMap(XYZ(-5000,-5000,-5000),XYZ(5000,5000,5000),40.0);
	tempScene->solidifyScene();
	//	tempScene->setHDR(true,6);
	//	tempScene->setBlur(4);
	return tempScene;
}

void InitGL(void)
{
#if defined(_WIN32) || defined(__linux__)  
        glewInit();     // initialize GLEW extension manager for windows
#endif


	GLShader::loadDefaultShader("shaders/cubicvr_default.v","shaders/cubicvr_default.f");
	GLShader::loadLightShader(LIGHT_DIRECTIONAL,"shaders/cubicvr_light_dir.v","shaders/cubicvr_light_dir.f");
	GLShader::loadLightShader(LIGHT_POINT,"shaders/cubicvr_light_point.v","shaders/cubicvr_light_point.f");
	GLShader::loadLightShader(LIGHT_AREA,"shaders/cubicvr_light_area.v","shaders/cubicvr_light_area.f");
	
	GLShader::loadLightShader(LIGHT_DEFERRED,"shaders/cubicvr_deferred.v","shaders/cubicvr_deferred.f");
	GLShader::loadLightShader(LIGHT_DIRECTIONAL_DEFERRED,"shaders/cubicvr_light_dir_deferred_multi.v","shaders/cubicvr_light_dir_deferred_multi.f");
	GLShader::loadLightShader(LIGHT_POINT_DEFERRED,"shaders/cubicvr_light_point_deferred_multi.v","shaders/cubicvr_light_point_deferred_multi.f");
	GLShader::loadLightShader(LIGHT_SPOT_DEFERRED,"shaders/cubicvr_light_spot_deferred_multi.v","shaders/cubicvr_light_spot_deferred_multi.f");
	GLShader::loadLightShader(LIGHT_AREA_DEFERRED,"shaders/cubicvr_light_area_deferred_multi.v","shaders/cubicvr_light_area_deferred_multi.f");
	
	particleTexAlpha = Texture::create("particle_alpha.png","particle_tex_a");	
	particleMat = new Material();
	particleMat->color = RGBA(200.0/255.0,200.0/255.0,200.0/255.0,0.5);
	particleMat->bindTexture(0,particleTexAlpha,TEXTURE_DIFF);
//	particleMat->bindTexture(1,particleTexAlpha,TEXTURE_ALPHA);

	InitResources(true);
	
	// Load Standard Box
	loadLWO(boxObj,"box2.lwo");
	boxObj.triangulate();
	boxObj.cache(true);
	
	testScene = LoadTrack(rm);
	mkLandscape();
	
	targetObj = new SceneObject();
	targetObj->setPosition(XYZ(0,3,0));
	
	Light::setGlobalAmbient(RGB(0.2,0.2,0.2));
	
	myCamera.setFOV(55);
	myCamera.setNearDOF(3.0);

	
	// Make Skysphere
	loadLWO(skySphere,"skysphere.lwo");
	skySphere.triangulate();
	skySphere.cache(true);
	skyObj.bind(skySphere);
	skyObj.setScale(XYZ(100,100,100));
	sky.bind(skyObj);
	sky.bind(myCamera);
	
	// Create Car
	car = MakeDefaultVehicle(rm, "stunt-car1.xml");		//LoadVehicle(rm, "stunt-car1.xml");
	testScene->bind(*car);
	
	testScene->bind(myCamera);

	myCamera.position = XYZ(10000,5000,10000);
	
	bool bAreaLit = true;
	if(bAreaLit)
	{
		aLight = new AreaLight(&myCamera, 2048, 256, 200, 350, 100, -80);
//		aLight = new AreaLight(&myCamera, 0, 0, 200, 350, 100, -80);
		aLight->diffuse = RGB(1,1,1);
		aLight->specular = RGB(0.9,0.9,0.9);
		aLight->rotation = XYZ(0,25,45);
	//	aLight->ambient = RGB(0.2,0.2,0.2);
		testScene->bind(*aLight);
	}	
    else
    {
        myLight.setType(LIGHT_DIRECTIONAL);
        myLight.diffuse = RGB(1,1,1);
		myLight.specular = RGB(0.9,0.9,0.9);
		myLight.rotation = XYZ(0,25,45);
        myLight.ambient = RGB(0.2,0.2,0.2);
		testScene->bind(myLight);
    }
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);						// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
	glShadeModel(GL_SMOOTH);					// Enables Smooth Color Shading
	glEnable(GL_LIGHTING);

	if (deferred)
	{
		testScene->setDeferredShading(true);
//		testScene->getPostProcessInfo().dof = true;
//		testScene->getPostProcessInfo().ssao = true;
//		testScene->getPostProcessInfo().hdr = true;
	}

//	testScene->getPostProcessInfo().hdr = true;

}



void camSetup()
{
	if (overview_enabled)
	{
		myCamera.position = XYZ(20,40,10);
	}
	else
	{
		if (viewTargetId == VIEWTARGET_BRUSH)
		{
			dummyCam.trackTarget(myTimer.lastUpdateSeconds()*0.75,0);
			myCamera.trackTarget(myTimer.lastUpdateSeconds(),brushSize*1.4);
		}
		else
		{
			dummyCam.trackTarget(8.0*myTimer.lastUpdateSeconds(),0);
			if (dummyCam.position.y < dummyCam.target.y+1.0) dummyCam.position.y = dummyCam.target.y+1.0;
			myCamera.trackTarget(6.0*myTimer.lastUpdateSeconds(),brushSize);
			if (myCamera.position.y < myCamera.target.y) myCamera.position.y = myCamera.target.y;
		}
	}
	
	
	XYZ camTest;
	camTest = myCamera.position;
	
	float camLandscapeHeight = myLandscape->getHeightValue(camTest);
	float camTestVal;


		
	camTest.x += 1;
	camTest.z += 1;	
	camTestVal = myLandscape->getHeightValue(camTest); if (camTestVal > camLandscapeHeight) camLandscapeHeight = camTestVal;

	camTest.x -= 2;
	camTestVal = myLandscape->getHeightValue(camTest); if (camTestVal > camLandscapeHeight) camLandscapeHeight = camTestVal;

	camTest.z -= 2;
	camTestVal = myLandscape->getHeightValue(camTest); if (camTestVal > camLandscapeHeight) camLandscapeHeight = camTestVal;

	camTest.x += 2;
	camTestVal = myLandscape->getHeightValue(camTest); if (camTestVal > camLandscapeHeight) camLandscapeHeight = camTestVal;
	
//	if (camLandscapeHeight < 2.0) camLandscapeHeight = 2.0;
	camLandscapeHeight+=1.8;

	if (viewTargetId == VIEWTARGET_BRUSH)
	{
		myCamera.position.y = brushSize*1.4;
	}
	
	if(myCamera.position.y < camLandscapeHeight) myCamera.position.y = camLandscapeHeight;
	if(myCamera.position.y < myCamera.target.y+1.0) myCamera.position.y = myCamera.target.y+1.0;

	if (heightMove) myCamera.position.y += heightMove;

//	carObj->controller().position.x=meta_xmove;
//	carObj->controller().position.z=meta_zmove;

	
//	if (myCamera.position.y != brushSize) myCamera.position.y = brushSize;
	
//	spotlight.position = pCar->controller().position;
//	spotlight.position.y+=50;
//	spotlight.position.x+=5;
//	spotlight.position.z+=5;
	
	skyObj.setPosition(myCamera.position-(XYZ(0,6,0)));
//	skyObj.clearTransform();
	
	
	//myCamera.setNearDOF(40.0+sin(myTimer.getSeconds()*2.0)*40.0);
}


void processLandscapeFunction()
{
	myLandscape->function(active_function-1,funcScale,brushSize/4.0,myTimer.getSeconds());
}

void renderBrush()
{
		float cx = targetObj->getPosition().x;
		float cz = targetObj->getPosition().z;

//		glEnable(GL_LINE_SMOOTH);
		glLineWidth(4.0);
		glDisable(GL_LIGHTING);
		glDepthFunc(GL_LEQUAL);
		
//		spotlight.shadowRenderStart(0);
//		spotlight.shadowRenderStart(1);
		
		glBegin(GL_LINE_STRIP);
		
			glColor3f(0.0,1.0,0.0);
			double q; 
			
			for (q = 0; q <= M_PI*2.0; q+=(M_PI*2.0)/200.0)
			{
				XYZ heightPt;
				
				heightPt.x = cx + brushSize*sin(q);
				heightPt.z = cz + brushSize*cos(q);
				heightPt.y = myLandscape->getHeightValue(heightPt);
				glVertex3f(heightPt.x,heightPt.y,heightPt.z);
			}
			
			
		glEnd();
//		spotlight.shadowRenderFinish(0);
//		spotlight.shadowRenderFinish(1);
}


int targetLock = 0;
int mouse_last_x, mouse_last_y;

void UpdateLogic()
{
	UpdatePhysics();
	
	if (targetLock)
	{
		btVector3 rayTo = myCamera.getRayTo(mouse_last_x, mouse_last_y);
		btCollisionWorld::ClosestRayResultCallback rayCallback(myCamera.position.cast(),rayTo);
		testScene->getDynamicsWorld()->rayTest(myCamera.position.cast(),rayTo,rayCallback);
		if (rayCallback.hasHit())
		{
			targetObj->setPosition(XYZ(rayCallback.m_hitPointWorld));
		}
	}

	DrawGLScene();
	
//	printf("%f,%f,%f - %f,%f,%f\n",car->aabbMin.x,car->aabbMin.y,car->aabbMin.z,car->aabbMax.x,car->aabbMax.y,car->aabbMax.z);
}

void DrawGLScene(void)
{   
	double lus = myTimer.lastUpdateSeconds();

	myTimer.update();

	car->getRigidBody().setActivationState(DISABLE_DEACTIVATION);


	brushSize += sizeChange;
	if (brushSize < 0.4) brushSize = 0.4;
	
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();	

	if (active_function != -1)
	{
		processLandscapeFunction();
	}
	
	if (dirMotion && active_function==-1)
	{
		myLandscape->useBrush(active_tool,targetObj->getPosition(),dirMotion*dir*lus,brushSize);
	}
	else if (dirMotion)
	{
		funcScale += (dir*dirMotion*lus)*0.25;
		if (funcScale < 0) funcScale = 0;
	}

	camSetup();
	

//	myCamera.target = car->getWorldPosition()-XYZ(10,10,10);
//	myCamera.position = car->getWorldPosition()+XYZ(10,10,10);

	


	testScene->openRenderBuffer();
//	spotlight.position = XYZ(3,15,3);
	GLShader::clear();
	glDisable(GL_LIGHTING);
	glDepthMask(false);
	sky.render();
	glDepthMask(true);
	glEnable(GL_LIGHTING);

//	processCar();
	glClear(GL_DEPTH_BUFFER_BIT);

//	testScene->debug.details = true;
    
  	testScene->render();

	if (myTimer.getNumUpdates()%120 == 0)
	{
		printf("fps: %f\n",1.0/lus);
		printf("objs in view: %d\n",testScene->active_sceneobjs.size());
	}
	
	
	GLShader::clear();

	if (sectorDebug) 
	{
		glDepthMask(true);
		Texture::clearAllTextures();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		testScene->sectorDebugDraw();
	}

	if (lines_enabled)
	{
		renderBrush();
		//		myLandscape->drawDebugHeightField();
	}
	
	
	glDisable(GL_LIGHTING);
	particleMat->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);
	glEnable(GL_DEPTH_TEST);
	
	GLShader::clear();
	Texture::clearAllTextures();
	Texture::textures[particleTexAlpha]->use();
	
//	GLShader::defaultShader.setShaderValue("hasLighting", (unsigned int)0);
//	GLShader::defaultShader.setupShader();
//	GLShader::defaultShader.use();
	
//	GLShader::defaultShader.setShaderValue("hasLighting", (unsigned int)1);
	
//	particleSys.draw(pe,myTimer.getSeconds());

//	for (int ip = 0; ip < 4; ip++) printf("[(%f)]",car->getWheelSkid(ip));
	

	
	myCamera.setup();
	
	particleSys.viewSetup();
	particleSys.draw(pe,myTimer.getSeconds());
	
	glEnable(GL_LIGHTING);
	
	testScene->closeRenderBuffer();
	

//		printf("cam pos: %f, %f, %f\n",myCamera.position.x,myCamera.position.y,myCamera.position.z);

	
    glFlush();
	
	glutSwapBuffers();
	
	if (record_mode) testScene->saveScreenshot("/video_project/rec_02/",frame_counter++);

#ifdef __linux__
	usleep(3000);
#endif
//	if (mouseActive) glutWarpPointer(kWindowWidth/2,kWindowHeight/2);
}

void ReSizeGLScene(int Width, int Height)
{
	testScene->setRenderDimensions(Width,Height);
}
	
void KeyboardFunc(unsigned char key, int x, int y) 
{
	normalKeyPressed[key] = true;
	
	if (key == '.') CreateKey();
//	if (key == 'c') CreateClusterBomb();
	
	if (key == 'p') myTimer.paused(!myTimer.paused());
	
//	if (key == '1') active_function=1;
//	if (key == '2') active_function=2;
//	if (key == '3') active_function=3;
//	if (key == '4') active_function=4;
//	if (key == ' ') active_function=-1; 
	if (key == ';') lines_enabled=!lines_enabled;
	if (key == 'o') overview_enabled=!overview_enabled;

	/// todo: landscape controls off!!
	if (key == 'r') { dir=1; /*if (active_function==-1)  carObj->bind(toolObjsR[active_tool]);*/ dirMotion=4;}
	if (key == 'f') { dir=-1; /*if (active_function==-1) carObj->bind(toolObjsL[active_tool]);*/ dirMotion=4;}
	if (key == 't') { dir=1; /*if (active_function==-1) carObj->bind(toolObjsR[active_tool]);*/ dirMotion=15;}
	if (key == 'g') { dir=-1; /*if (active_function==-1) carObj->bind(toolObjsL[active_tool]);*/ dirMotion=15;}
	
	if (key=='q') testScene->debug.details= !testScene->debug.details;
	if (key=='e') testScene->debug.aabb= !testScene->debug.aabb;
	if (key=='w') sectorDebug = !sectorDebug;
	if (key=='a') testScene->debug.lights= !testScene->debug.lights;
	if (key=='s') testScene->debug.frustum= !testScene->debug.frustum;

	if (key == '=') sizeChange=myTimer.lastUpdateSeconds()*10.0; 
	if (key == '-') sizeChange=-myTimer.lastUpdateSeconds()*10.0;	
	if (key == 'z') { viewTargetId++; if (viewTargetId > VIEW_TARGET_MAX) viewTargetId = 1; dummyCam.position = XYZ(0,0,0); };	
	if (key == 'x') { viewTargetId--; if (viewTargetId < 1) viewTargetId = VIEW_TARGET_MAX; dummyCam.position = XYZ(0,0,0); };	
	if (key == '1') testScene->getPostProcessInfo().ssao = !testScene->getPostProcessInfo().ssao;
	if (key == '2') testScene->getPostProcessInfo().dof = !testScene->getPostProcessInfo().dof;
	if (key == '3') testScene->getPostProcessInfo().hdr = !testScene->getPostProcessInfo().hdr;
	if (key == 'v') heightMove=myTimer.lastUpdateSeconds()*10.0; 
	if (key == 'c') heightMove=-myTimer.lastUpdateSeconds()*10.0;	

	if (key == 27) 
		exit(0);
}


void releaseKey(unsigned char key, int x, int y) 
{
	normalKeyPressed[key] = false;

	switch (key) 
	{
		case '=': sizeChange=0; break;
		case '-': sizeChange=0; break;
		case 'r': dirMotion=0;break;
		case 'f': dirMotion=0;break;
		case 't': dirMotion=0;break;
		case 'g': dirMotion=0;break;
		case 'v': heightMove = 0; break;
		case 'c': heightMove = 0; break;

	}
}





int gPickingConstraintId = 0;
btVector3 gOldPickingPos;
float gOldPickingDist  = 0.f;
btRigidBody* pickedBody = 0;//for deactivation state
btTypedConstraint*		m_pickConstraint;


void mouseEvent(int button, int state, int x, int y)
{
	btVector3 rayTo = myCamera.getRayTo(x,y);


/*	if (button == GLUT_LEFT_BUTTON)
	{
	 mouseActive = (state == GLUT_DOWN)?1:0;
	 mouse_last_x = x;
	 mouse_last_y = y;
	} */
	
	 mouse_last_x = x;
	 mouse_last_y = y;

	
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//apply an impulse
	
		btCollisionWorld::ClosestRayResultCallback rayCallback(myCamera.position.cast(),rayTo);
		testScene->getDynamicsWorld()->rayTest(myCamera.position.cast(),rayTo,rayCallback);

//		printf("ray: %f,%f,%f  -  cam pos: %f, %f, %f\n",rayTo.getX(),rayTo.getY(),rayTo.getZ(),myCamera.position.x,myCamera.position.y,myCamera.position.z);
						
		if (rayCallback.hasHit())
		{
			btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
			
			if (body)
			{
//				printf("hit!\n");

				body->setActivationState(ACTIVE_TAG);
				btVector3 impulse = rayTo;
				impulse.normalize();
				
				float impulseStrength = 4000.f;
				
				impulse *= impulseStrength;
				btVector3 relPos = rayCallback.m_hitPointWorld - body->getCenterOfMassPosition();
				body->applyImpulse(impulse,relPos);
			}
		}
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
			//add a point to point constraint for picking
			btCollisionWorld::ClosestRayResultCallback rayCallback(myCamera.position.cast(),rayTo);
			testScene->getDynamicsWorld()->rayTest(myCamera.position.cast(),rayTo,rayCallback);

			if (rayCallback.hasHit())
			{
				btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
				if (body)
				{
					//other exclusions?
					if (!(body->isStaticObject() || body->isKinematicObject()))
					{
						pickedBody = body;
						pickedBody->setActivationState(DISABLE_DEACTIVATION);
						
						btVector3 pickPos = rayCallback.m_hitPointWorld;

						btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
						
						// btHingeConstraint

						btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body,localPivot);
//						btPoint2PointConstraint* p2p = new btHingeConstraint(*body,localPivot);

						testScene->getDynamicsWorld()->addConstraint(p2p);
						m_pickConstraint = p2p;
						
						//save mouse position for dragging
						gOldPickingPos = rayTo;

						btVector3 eyePos = myCamera.position.cast();
						gOldPickingDist  = (pickPos-eyePos).length();

						//very weak constraint for picking
						p2p->m_setting.m_tau = 0.1f;
					}
					else if (body->isStaticObject() && (viewTargetId == VIEWTARGET_BRUSH))
					{
						targetObj->setPosition(XYZ(rayCallback.m_hitPointWorld));

						if (viewTargetId == VIEWTARGET_BRUSH)
						{
							targetLock = true;
						}
					}
				}
			}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (m_pickConstraint)
		{
			testScene->getDynamicsWorld()->removeConstraint(m_pickConstraint);
			
			delete m_pickConstraint;
			
			//printf("removed constraint %i",gPickingConstraintId);
			
			m_pickConstraint = 0;
			pickedBody->forceActivationState(ACTIVE_TAG);
			pickedBody->setDeactivationTime( 0.f );
			pickedBody = 0;
		}
		
		targetLock = false;
	}
}



void mouseMotion (int x,int y)
{
//	if (!mouseActive) return;

//	float xdelta = x- mouse_last_x;
//	float ydelta = y-mouse_last_y;



if (m_pickConstraint)
	{
		//move the constraint pivot
		btPoint2PointConstraint* p2p = static_cast<btPoint2PointConstraint*>(m_pickConstraint);
		if (p2p)
		{
			//keep it at the same picking distance

			btVector3 newRayTo = myCamera.getRayTo(x,y);
			btVector3 eyePos = myCamera.position.cast();
			btVector3 dir = newRayTo-eyePos;
			dir.normalize();
			dir *= gOldPickingDist;

			btVector3 newPos = eyePos + dir;
			p2p->setPivotB(newPos);
		}

	}

	

	
	mouse_last_x = x;
	mouse_last_y = y;

//	myCamera.moveViewRelative(targetObj->controller().position,2.0*xdelta*myTimer.lastUpdateSeconds(),2.0*ydelta*myTimer.lastUpdateSeconds());
}

int main(int argc, char** argv)
{
//	char CurrentPath[1024];
//	getcwd(CurrentPath,1024);
//	
//
//	printf("%s\n",CurrentPath);

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition (0, 0);

	if(bFullscreen)
	{
		glutGameModeString( kFullScreenRes );
		glutEnterGameMode();
	}
	else
	{
		glutInitWindowSize (kWindowWidth, kWindowHeight);
		glutCreateWindow ("CubicVR :: Vehicle Physics");
	}

	InitGL();
	
	glutDisplayFunc(DrawGLScene);
	glutReshapeFunc(ReSizeGLScene);
	glutIdleFunc(UpdateLogic);
	glutKeyboardFunc(KeyboardFunc);
//	glutJoystickFunc(gamepad,0);
	glutJoystickFunc(&joystickFunc, 1);
	glutMouseFunc(mouseEvent);
	glutMotionFunc(mouseMotion);

	glutIgnoreKeyRepeat(1);
	
	glutKeyboardUpFunc(releaseKey);
	
	glutSpecialFunc(SpecialKeyFunc);
	glutSpecialUpFunc(SpecialKeyUpFunc);
//	glutKeyboardDownFunc(pressKey);

	myTimer.start();

	srand(10000);
	
//	InitBullet();
	
	glutMainLoop();
	
	return 0;
}

