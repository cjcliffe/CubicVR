/* KallistiOS ##version##
   Parts (c)2000 Jeff Molofee
*/

#include <kos.h>
#include <CubicVR/Timer.h>
#include <CubicVR/Camera.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/Scene.h>
#include <CubicVR/Gamepad.h>
#include <CubicVR/OBJLoader.h>

const char *ORANGE_PCX = RES_PATH "orange.pcx";
const char *MODEL_OBJ = RES_PATH "dclogo.obj";

Camera myCamera(640,480, 50, 1, 300);
Scene myScene(640,480);

Mesh *boxObj;
SceneObject *box;
Gamepad *mGamepad;

bool pause_pressed=false;

#define NUM_LIGHTS 3

Light myLights[NUM_LIGHTS];

Timer myTimer;

void InitScene();
void UpdatePhysics();
int main(int argc, char** argv); 


void InitScene()
{
	OBJLoader meshLoader;

	boxObj = meshLoader.load(MODEL_OBJ);
	
	boxObj->cache(true);
	boxObj->clean();		// remove unused data

	box = new SceneObject(*boxObj);
	myScene.bind(*box);
	box->buildVertexLightMap();

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		myLights[i].setType(LIGHT_POINT);
		myLights[i].diffuse = RGB((float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX);
		myLights[i].setCutoff(10);
		myScene.bind(myLights[i]);
	}

	myCamera.setType(CAMERA_TARGET);
	myCamera.position = XYZ(-4.0,4.0,-4.0);
	myCamera.target = XYZ(0,0,0);
	myCamera.setClip(0.01,100);
	
	myScene.bind(myCamera);
}



void draw_gl(void) 
{
	myTimer.update();
//	float lus = myTimer.lastUpdateSeconds();


	float ymv = mGamepad->Y_Axis()*8.0;
	float xmv = mGamepad->X_Axis()*8.0;
	
	box->setRotation(box->getRotation()+XYZ(xmv,ymv,0));


	bool toggle = false;
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		myLights[i].position = XYZ(
			cos(myTimer.getSeconds()*(toggle?-1.0:1.0)*0.2*(float)i)*8.0,	
			cos(myTimer.getSeconds()*(toggle?-1.0:1.0)*0.1*(float)i)*8.0,
			sin(myTimer.getSeconds()*(toggle?-1.0:1.0)*0.4*(float)i)*8.0
		);
		toggle = !toggle;
	}

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myScene.render();
}



pvr_init_params_t params = {
        /* Enable opaque and translucent polygons with size 16 */
        { PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_0 },

        /* Vertex buffer size 512K */
        (512)*1024
};


#ifdef ROMDISK
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);
#endif


int main(int argc, char **argv) 
{
//#ifdef SDCARD_BUILD
//	Logger::setOutputFile("/sd/basic_test.log");
//#endif

	/* Initialize KOS */
 	pvr_init(&params);

	/* Get basic stuff initialized */
	glKosInit();

	mGamepad = new Gamepad();

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

	while(1) 
	{
		mGamepad->Update();
		if(mGamepad->Button(GAMEPAD_BTN_START))		break;

		/* Begin frame */
		glKosBeginFrame();


/*
		if (myTimer.getNumUpdates()%120==0)
		{
		 	printf("fps: %.4f\n",myTimer.getNumUpdates()/myTimer.totalSeconds());
		 	printf("fps (immediate): %.4f\n",1.0/myTimer.lastUpdateSeconds());
		}
*/
		/* Draw the "scene" */
		draw_gl();

		/* Finish the frame */
		glKosFinishFrame();
	}

	return 0;
}


