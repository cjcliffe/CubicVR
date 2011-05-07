#include <CubicVR/GLExt.h>
#include <CubicVR/UVMapper.h>
#include <CubicVR/RigidScene.h>
#include <CubicVR/Timer.h>
#include <CubicVR/SceneObjects/RigidBox.h>
#include <GLUT/glut.h>

#define kWindowWidth	1280
#define kWindowHeight	720
Camera myCamera(kWindowWidth,kWindowHeight, 60, 1, 300);

ObjectShader myShader;
RigidScene myScene(kWindowWidth,kWindowHeight);
Light myLight;
Object myBoxObj;
RigidBox myRigidSceneObj;
RigidBox myGroundObj;
Timer myTimer;

void InitGL(void);
void DrawGLScene(void);
void ReSizeGLScene(int Width, int Height);


void makeBox(Object &boxObj, float box_size, Material *box_mat)
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
	
	
	boxObj.addFace();
	boxObj.addFacePoint(0); boxObj.addFacePoint(1); boxObj.addFacePoint(2); boxObj.addFacePoint(3);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.addFace();
	boxObj.addFacePoint(7); boxObj.addFacePoint(6); boxObj.addFacePoint(5); boxObj.addFacePoint(4);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.addFace();
	boxObj.addFacePoint(4); boxObj.addFacePoint(5); boxObj.addFacePoint(1); boxObj.addFacePoint(0);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.addFace();
	boxObj.addFacePoint(5); boxObj.addFacePoint(6); boxObj.addFacePoint(2); boxObj.addFacePoint(1);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.addFace();
	boxObj.addFacePoint(6); boxObj.addFacePoint(7); boxObj.addFacePoint(3); boxObj.addFacePoint(2);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.addFace();
	boxObj.addFacePoint(7); boxObj.addFacePoint(4); boxObj.addFacePoint(0); boxObj.addFacePoint(3);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.addFace();
	boxObj.addFacePoint(3); 
	boxObj.addFacePoint(0); 
	boxObj.addFacePoint(4); 
	boxObj.addFacePoint(7);
	boxObj.bindFaceMaterial(box_mat);
	
	boxObj.triangulate();
	boxObj.calcNormals();
}


void InitGL(void)
{
#ifdef _WIN32
	glewInit();	// initialize GLEW extension manager for windows
#endif

	GLShader::loadDefaultShader("shaders/cubicvr_default.v","shaders/cubicvr_default.f");
	GLShader::loadLightShader(LIGHT_DIRECTIONAL,"shaders/cubicvr_light_dir.v","shaders/cubicvr_light_dir.f");
	GLShader::loadLightShader(LIGHT_POINT,"shaders/cubicvr_light_point.v","shaders/cubicvr_light_point.f");
	GLShader::loadLightShader(LIGHT_SPOT,"shaders/cubicvr_light_spot.v","shaders/cubicvr_light_spot.f");
	
	// Generate a box material
	Material *boxMaterial = new Material();
	
	// Load textures for this material
	Texture::create("panel-diff.png","panel_diff");
	Texture::create("panel-spec.png","panel_spec");
	Texture::create("panel-norm.png","panel_norm");
	
	// Apply the textures as layer 0, 1, 2
	boxMaterial->bindTexture(0,Texture::getTextureId("panel_diff"),TEXTURE_DIFF);
	boxMaterial->bindTexture(1,Texture::getTextureId("panel_spec"),TEXTURE_SPEC);
	boxMaterial->bindTexture(2,Texture::getTextureId("panel_norm"),TEXTURE_NORM);

	// Create the box structure and bind our material to each face
	makeBox(myBoxObj,1.0,boxMaterial);

	// Create a UV Mapper
	UVMapper myUVMapper;
	// Set the projection type to cubic
	myUVMapper.setProjection(UV_PROJECTION_CUBIC);
	// Match the scale of our box
	myUVMapper.setScale(XYZ(1.0,1.0,1.0));	
	
	// Apply the UV map to the material we bound to our box object
	myUVMapper.apply(myBoxObj,boxMaterial);

	// Now cache the object onto the card for best performance.
	myBoxObj.cache(true);
	
	// Bind our box to a Scene Object
	myRigidSceneObj.bind(myBoxObj);
	// Raise the box above the ground
	myRigidSceneObj.setPosition(XYZ(0,5,0));
	// Rotate the box slightly for more physical reaction
	myRigidSceneObj.setRotation(XYZ(25,10,0));
	
	// Bind our Scene Object to the Scene
	myScene.bind(myRigidSceneObj);

	// Activate our rigid object so it will start in motion
	myRigidSceneObj.getRigidBody().setActivationState(ACTIVE_TAG);


	// Create a ground SceneObject
	myGroundObj.bind(myBoxObj);
	// Set the ground area to 10x10
	myGroundObj.setScale(XYZ(10,1,10));  
	// Offset the ground surface to Y of 0 (box height 1)
	myGroundObj.setPosition(XYZ(0,-0.5,0));  
	// Set to be a static (massless) object
	myGroundObj.setMass(0);
	
	// Bind our ground to the scene
	myScene.bind(myGroundObj);
	
	
	// Set up the global ambient factor
	Light::setGlobalAmbient(RGB(0.1,0.1,0.1));
	
	// Set up our light
	myLight.setType(LIGHT_DIRECTIONAL);
	myLight.rotation = XYZ(0,25,45);
	
	// Set up our camera (targeted, looking at 0,0,0)
	myCamera.setType(CAMERA_TARGET);
	myCamera.position = XYZ(5.0,5.0,5.0);
	myCamera.target = XYZ(0,0,0);
	
	// Bind the light and camera
	myScene.bind(myLight);
	myScene.bind(myCamera);
	
	// start our timer
	myTimer.start();
	
	// Init GL clearing and depth buffering
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer

	glDepthFunc(GL_LESS);						// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
}


void DrawGLScene(void)
{   
	// Call timer update on frame entry
	myTimer.update();
	
	// Clear the display buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	
	// Step the rigid simulation
	myScene.stepSimulation(myTimer.lastUpdateSeconds());
	
	// Render our scene
	myScene.render();

	// Flush output and swap buffers
    glFlush();	
	glutSwapBuffers();
}


void ReSizeGLScene(int Width, int Height)
{
	myScene.setRenderDimensions(Width,Height);
}
	

void KeyboardFunc(unsigned char key, int x, int y) 
{
	if (key == 27) exit(0);
}


void releaseKey(unsigned char key, int x, int y) 
{
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
	glutInitWindowSize (kWindowWidth, kWindowHeight);
	glutCreateWindow ("CubicVR Basic Test");

	InitGL();

	glutDisplayFunc(DrawGLScene);
	glutReshapeFunc(ReSizeGLScene);
	glutIdleFunc(DrawGLScene);
	glutKeyboardFunc(KeyboardFunc);
	glutMouseFunc(mouseEvent);
	glutMotionFunc(mouseMotion);

	glutIgnoreKeyRepeat(1);
	
	glutKeyboardUpFunc(releaseKey);
	
	glutMainLoop();
	
	return 0;
}
