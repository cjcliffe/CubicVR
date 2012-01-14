/* KallistiOS ##version##
   Parts (c)2000 Jeff Molofee
*/

#include <kos.h>
#include <GLX/gl.h>
#include <GLX/glu.h>

#include <CubicVR/Timer.h>
#include <CubicVR/Mesh.h>
#include <CubicVR/UVMapper.h>
#include <CubicVR/Camera.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/RigidScene.h>
#include <CubicVR/SceneObjects/RigidSceneObject.h>
#include <CubicVR/SceneObjects/RigidBox.h>
#include <CubicVR/SceneObjects/Landscape.h>

#include <CubicVR/Gamepad.h>

const char *GRASS_PCX = RES_PATH "grass.pcx";
const char *CRATE_PCX = RES_PATH "crate.pcx";


Camera myCamera(640,480, 50, 1, 300);
RigidScene myScene(640,480);
RigidBox myRigidSceneObj;
RigidBox myGroundObj;
bool physics_paused=false;
bool pause_pressed=false;

#define NUM_LIGHTS 3

Mesh boxObj;
SceneObject box;
Light myLights[NUM_LIGHTS];

Timer myTimer;
MeshRenderer shader;
SceneObject targetObj;
Gamepad *mGamepad;

void InitScene();
void UpdatePhysics();
int main(int argc, char** argv); 

btDefaultCollisionConstructionInfo *pspCollisionConstruct;



void makeBox(Mesh &boxObj, float box_size, Material *box_mat)
{
	float half_box = box_size/2.0;
	
	boxObj.addPoint(0,XYZ( half_box,-half_box, half_box));
	boxObj.addPoint(1,XYZ( half_box, half_box, half_box));
	boxObj.addPoint(2,XYZ(-half_box, half_box, half_box));
	boxObj.addPoint(3,XYZ(-half_box,-half_box, half_box));
	
	
	boxObj.addPoint(4,XYZ( half_box,-half_box, -half_box));
	boxObj.addPoint(5,XYZ( half_box, half_box, -half_box));
	boxObj.addPoint(6,XYZ(-half_box, half_box, -half_box));
	boxObj.addPoint(7,XYZ(-half_box,-half_box, -half_box));
	
	RGB clr = RGB(1.0,0,0);
	
	boxObj.addFace();
	boxObj.addFacePoint(0,clr); boxObj.addFacePoint(1,clr); boxObj.addFacePoint(2,clr); 
	boxObj.bindFaceMaterial(box_mat);
	boxObj.addFace();
	boxObj.addFacePoint(0,clr); boxObj.addFacePoint(2,clr); boxObj.addFacePoint(3,clr);
	boxObj.bindFaceMaterial(box_mat);

	clr = RGB(0.0,0.0,1.0);

	boxObj.addFace();
	boxObj.addFacePoint(7,clr); boxObj.addFacePoint(6,clr); boxObj.addFacePoint(5,clr); 
	boxObj.bindFaceMaterial(box_mat);
	boxObj.addFace();
	boxObj.addFacePoint(7,clr);  boxObj.addFacePoint(5,clr); boxObj.addFacePoint(4,clr);
	boxObj.bindFaceMaterial(box_mat);

	clr = RGB(1.0,0.0,0);

	boxObj.addFace();
	boxObj.addFacePoint(4,clr); boxObj.addFacePoint(5,clr); boxObj.addFacePoint(1,clr); 
	boxObj.bindFaceMaterial(box_mat);
	boxObj.addFace();
	boxObj.addFacePoint(4,clr);  boxObj.addFacePoint(1,clr); boxObj.addFacePoint(0,clr);
	boxObj.bindFaceMaterial(box_mat);

	clr = RGB(0.0,1.0,0);

	boxObj.addFace();
	boxObj.addFacePoint(5,clr); boxObj.addFacePoint(6,clr); boxObj.addFacePoint(2,clr); 
	boxObj.bindFaceMaterial(box_mat);
	boxObj.addFace();
	boxObj.addFacePoint(5,clr);  boxObj.addFacePoint(2,clr); boxObj.addFacePoint(1,clr);
	boxObj.bindFaceMaterial(box_mat);
	
	clr = RGB(0.0,0.0,1.0);
	boxObj.addFace();
	boxObj.addFacePoint(6,clr); boxObj.addFacePoint(7,clr); boxObj.addFacePoint(3,clr); 
	boxObj.bindFaceMaterial(box_mat);
	boxObj.addFace();
	boxObj.addFacePoint(6,clr);  boxObj.addFacePoint(3,clr); boxObj.addFacePoint(2,clr);
	boxObj.bindFaceMaterial(box_mat);

	clr = RGB(1.0,0.0,0);
	
	boxObj.addFace();
	boxObj.addFacePoint(7,clr); boxObj.addFacePoint(4,clr); boxObj.addFacePoint(0,clr); 
	boxObj.bindFaceMaterial(box_mat);
	boxObj.addFace();
	boxObj.addFacePoint(7,clr); boxObj.addFacePoint(0,clr); boxObj.addFacePoint(3,clr);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.calcNormals();
}


void CreateTower(int cnt_x, int cnt_y, int cnt_z, XYZ offset, XYZ size)
{
	RigidBox *myRigidBody;

	for(int y = 1; y <= cnt_y; y++)
	{
		for(int z = 1; z <= cnt_z; z++)
		{
			for(int x = 1; x <= cnt_x; x++)
			{
				float spacing = 0.01;
				float heightSpacing = 0.01;
				
				XYZ pos = XYZ(x * size.x + (spacing * x), y * size.y  + (heightSpacing * y), z * size.z + (spacing * z));
				
				pos += offset;
				
				myRigidBody = new RigidBox(boxObj);
				myRigidBody->setPosition(pos);
				myRigidBody->setScale(size);
				myRigidBody->buildVertexLightMap();

//				myRigidBody->setRestitution(0.8);	// bouncey fun land
				myScene.bind(*myRigidBody);
			}
		}
	}
}


void FireBox()
{
	Vector vec;
	
	vec = myCamera.target;
	vec += XYZ(0,-1,0);
	vec -= myCamera.position;
	vec.makeUnit();

	XYZ pos = myCamera.position;
	
	RigidBox *myRigidBody = new RigidBox(boxObj);

	myRigidBody->setPosition(pos);	
	myRigidBody->setScale(XYZ(1,1,1));	

	float fMass = 50;
	myRigidBody->setMass(fMass);

	myRigidBody->buildVertexLightMap();

	myScene.bind(*myRigidBody);
	float fForce = 15.0f;
	
	btVector3 force = btVector3(vec.x,vec.y,vec.z)*(fForce * 1000.0f);
	
	myRigidBody->getRigidBody().activate(true);
	//myRigidBody->getRigidBody()->applyForce(force, btVector3(myCamera.target.x, myCamera.target.y, myCamera.target.z));
	myRigidBody->getRigidBody().applyForce(force, btVector3(0,0,0));

}


Landscape *myLandscape;
void mkLandscape()
{
	Material *objMat = new Material();	

	unsigned long objTex = Texture::create(GRASS_PCX,"grass");

	objMat->bindTexture(0,objTex,TEXTURE_DIFF);
	objMat->setMaxSmooth(60);
	
	myLandscape = new Landscape(60.0,20,20,100,objMat);
	myLandscape->shadowCast(false);

	//	myLandscape->function(1,10.0,25.0,100);
//	myLandscape->function(1,1.5,5.0,100);
//	myLandscape->function(1,10.0,20.0,150);
	
	
	myLandscape->function(1,1.5,4.0,3);

	// shut off landscape here
	myLandscape->setPosition(XYZ(0,0,0));
	myLandscape->buildVertexLightMap();
	myScene.bind(*myLandscape);
}

void InitScene()
{
	Logger::log("Creating Material\n\n");

	// Generate a box material
	Material *boxMaterial = new Material();

	// Load textures for this material
	Texture::create(CRATE_PCX,"crate1");	

	// Apply the textures as layer 0, 1, 2
	boxMaterial->bindTexture(0,Texture::getTextureId("crate1"),TEXTURE_DIFF);
	
	// Create the box structure and bind our material to each face
	makeBox(boxObj,1.0,boxMaterial);
	
	// Create a UV Mapper
	UVMapper myUVMapper;
	// Set the projection type to cubic
	myUVMapper.setProjection(UV_PROJECTION_CUBIC);
	// Match the scale of our box
	myUVMapper.setScale(XYZ(1.0,1.0,1.0));	
	
	// Apply the UV map to the material we bound to our box object
	myUVMapper.apply(boxObj,boxMaterial);
	
	// Now cache the object onto the card for best performance.
	boxObj.cache(true);
	MeshRenderer tmpShader;

	mkLandscape();

	//CreateTower(2,4,2,XYZ(-1.5,5,-1.5),XYZ(1,1,1));
	// Set up the global ambient factor
//	Light::setGlobalAmbient(RGB(0.1,0.1,0.1));
	
	// Set up our light
	
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		myLights[i].setType(LIGHT_POINT);
		myLights[i].diffuse = RGB((float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX);
		myLights[i].setCutoff(20);
		myScene.bind(myLights[i]);
	}

	// Set up our camera (targeted, looking at 0,0,0)
	myCamera.setType(CAMERA_TARGET);
	myCamera.position = XYZ(-8.0,8.0,-8.0);
	//myCamera.target = XYZ(0,5,0);

	targetObj.setPosition(XYZ(0,0,0));

	myCamera.bindTarget(&targetObj);
	myCamera.setClip(0.01,100);
	
	// Bind the light and camera
	myScene.bind(myCamera);
/*
	myLightR.position = XYZ(cos(10*2.0)*8.0,	6,		sin(10*2.0)*8.0);
	myLightG.position = XYZ(cos(10	)*4.0,	6,		sin(10	)*4.0);
	myLightB.position = XYZ(cos(10*-1.5)*10.0,6,		sin(10*-1.5)*10.0);
*/
}


bool fire_pressed;

void UpdatePhysics()
{
	myTimer.update();
	float lus = myTimer.lastUpdateSeconds();

	float xmv = 0.0f;
	float ymv = 0.0f;

	bool toggle = false;
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		myLights[i].position = XYZ(cos(myTimer.getSeconds()*(toggle?-1.0:1.0)*0.2*(float)i)*(float)i*6.0,	6,		sin(myTimer.getSeconds()*(toggle?-1.0:1.0)*0.4*(float)i)*(float)i*6.0);
		toggle = !toggle;
	}

	if (mGamepad->Button(GAMEPAD_BTN_A)) 
	{
		if (!fire_pressed)
		{
			fire_pressed = true;

			FireBox();
		}
	}
	else fire_pressed = false;


	if (mGamepad->Button(GAMEPAD_BTN_B)) 
	{
		if (!pause_pressed)
		{
			pause_pressed = true;
			physics_paused = !physics_paused;
		}
	}
	else pause_pressed = false;

	ymv = mGamepad->Y_Axis()*8.0;
	xmv = mGamepad->X_Axis()*8.0;
	
	if (ymv||xmv)
	{
		myCamera.moveViewRelative(targetObj.getPosition(),xmv*lus,ymv*lus);
		if (ymv > 0)
		{
			myCamera.moveViewRelative(myCamera.position,0,ymv*lus);
		}
	}

	targetObj.getPosition().y = myLandscape->getHeightValue(targetObj.getPosition());

	float tmv = 0;
	
	tmv = mGamepad->Left_Trigger() * -4.0;
	tmv += mGamepad->Right_Trigger() * 4.0;

	if (tmv)
	{
		myCamera.moveViewRelative(myCamera.position,tmv*lus,0);
	}

	
	myCamera.trackTarget(2.0*lus,8.0);
	
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
	
	camLandscapeHeight+=3.0;
	
	if(myCamera.position.y < camLandscapeHeight) myCamera.position.y = camLandscapeHeight;


	//if (myCamera.position.y < 3) myCamera.position.y = 3;
	
	if (!physics_paused) myScene.stepSimulation(lus,3);

//	if (!myTimer.paused()) dynamicsWorld->stepSimulation(myTimer.lastUpdateSeconds(), 10);

//	if (!physics_paused) myLandscape->function(1,1.5,4.0,myTimer.getSeconds()/3.0);

}



void draw_gl(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myScene.render();

//	myLandscape->drawDebugHeightField();
//	myScene.sectorDebugDraw();
}

pvr_init_params_t params = {
        /* Enable opaque and translucent polygons with size 16 */
        { PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },

        /* Vertex buffer size 512K */
        (1024+512)*1024
};


#ifdef ROMDISK
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);
#endif

int main(int argc, char **argv) 
{
	/* Initialize KOS */
	pvr_init(&params);

	printf("bullet test beginning\n");

	/* Get basic stuff initialized */
	glKosInit();


	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	Logger::log("init..\n");
	InitScene();
	myTimer.start();
	Logger::log("go\n");

	mGamepad = new Gamepad();
	
	while(1) 
	{
		mGamepad->Update();
		if(mGamepad->Button(GAMEPAD_BTN_START))		break;

		
		/* Begin frame */
		glKosBeginFrame();


		UpdatePhysics();

		if (myTimer.getNumUpdates()%120==0)
		{
		 	printf("fps: %.4f\n",myTimer.getNumUpdates()/myTimer.totalSeconds());
		 	printf("fps (immediate): %.4f\n",1.0/myTimer.lastUpdateSeconds());
		}

		// printf("cam: %f,%f,%f\n",myCamera.position.x,myCamera.position.y,myCamera.position.z);
		// printf("target: %f,%f,%f\n",targetObj.controller().position.x,targetObj.controller().position.y,targetObj.controller().position.z);

		//printf("obj: %f,%f,%f\n",myRigidSceneObj.controller().position.x,myRigidSceneObj.controller().position.y,myRigidSceneObj.controller().position.z);

		/* Draw the "scene" */
		draw_gl();

		/* Finish the frame */
		glKosFinishFrame();
	}

	return 0;
}


