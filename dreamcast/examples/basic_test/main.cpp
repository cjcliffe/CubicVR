/* KallistiOS ##version##
   Parts (c)2000 Jeff Molofee
*/

#include <kos.h>

#include <CubicVR/Timer.h>
#include <CubicVR/Mesh.h>
#include <CubicVR/UVMapper.h>
#include <CubicVR/Camera.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/Scene.h>
#include <CubicVR/Gamepad.h>


const char *CRATE_PCX = RES_PATH "crate.pcx";


Camera myCamera(640,480, 50, 1, 300);
Scene myScene(640,480);

Mesh boxObj;
SceneObject *box;
Light testLight;

bool pause_pressed=false;

#define NUM_LIGHTS 3

Timer myTimer;

void InitScene();
void UpdatePhysics();
int main(int argc, char** argv); 


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

	box = new SceneObject(boxObj);

	myScene.bind(*box);

	myCamera.setType(CAMERA_TARGET);
	myCamera.position = XYZ(-2.0,2.0,-2.0);
	myCamera.target = XYZ(0,0,0);
	myCamera.setClip(0.01,100);
	
	myScene.bind(myCamera);

	testLight.setType(LIGHT_POINT);
	testLight.position = XYZ(-6,5,-5);
	testLight.setCutoff(5);
	myScene.bind(testLight);
}



void draw_gl(void) 
{
	glClearColor(0.0,0.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myScene.render();
}

pvr_init_params_t params = {
       /* Enable opaque and punchthru polygons with size 16 */
       { PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_0 },

        /* Vertex buffer size 512K */
        512*1024
};


#ifdef ROMDISK
extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);
#endif

Gamepad *mGamepad;

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

		myTimer.update();
		float lus = myTimer.lastUpdateSeconds();

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


