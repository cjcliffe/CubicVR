// Lua test program by Frank Buss (aka Shine)

#include <CubicVR/Timer.h>
#include <CubicVR/Scene.h>
#include <CubicVR/sceneLWS.h>
#include <CubicVR/objectLWO.h>

#include "main.h"
#include "Gamepad.h"

PSP_MODULE_INFO("CubicVR", 0, 1, 1);

/////////////////////////////////////////////////////////

void App_Initialize( void );
void App_Shutdown(void);
void App_Render(void);
void DrawScene( void );

Timer timerTest;
Scene myScene(SCR_WIDTH, SCR_HEIGHT);
char *sceneName = "metacube.lws";



int main(int argc, char** argv)
{
	sceRtcGetCurrentTick( &fpsTickLast );
	tickResolution = sceRtcGetTickResolution();
	
	Gamepad gamepad;
	
	App_Initialize();
	
	timerTest.start();
	
	float nextUpdate = 0;
	
	while(1)
	{
		
		timerTest.update();
		myScene.motion(sceneName).evaluate(timerTest.getSeconds());

		App_Render();

		if(gamepad.Poll())
		{
			if(gamepad.ButtonDown(PSP_CTRL_TRIANGLE))
			{
				timerTest.paused(true);
			}
			
			if(gamepad.ButtonDown(PSP_CTRL_SQUARE))
			{
				timerTest.paused(false);
			}

			if(timerTest.paused())
			{
				if(fabs(gamepad.AnalogX()) > 0.1)
				{
					if(gamepad.ButtonDown(PSP_CTRL_RTRIGGER))
					{
						if(timerTest.getSeconds() - 0.5f > 0 || gamepad.AnalogX() > 0)	timerTest.setSeconds(timerTest.getSeconds() + (gamepad.AnalogX() * 0.5));
					}
					else
					{
						if(timerTest.getSeconds() - 0.1f > 0 || gamepad.AnalogX() > 0)	timerTest.setSeconds(timerTest.getSeconds() + (gamepad.AnalogX() * 0.1));
					}
				}
			}
						
			if(gamepad.ButtonDown(PSP_CTRL_LEFT))
			{
				if(gamepad.ButtonDown(PSP_CTRL_RTRIGGER))
				{
					if(timerTest.getSeconds() - 0.1f > 0)	timerTest.setSeconds(timerTest.getSeconds() - 0.1);
				}
				else
				{
					if(timerTest.getSeconds() - 0.01f > 0)	timerTest.setSeconds(timerTest.getSeconds() - 0.01);
				}
			}
			
			if(gamepad.ButtonDown(PSP_CTRL_RIGHT))
			{
				if(gamepad.ButtonDown(PSP_CTRL_RTRIGGER))
				{
					timerTest.setSeconds(timerTest.getSeconds() + 0.1);
				}
				else
				{
					timerTest.setSeconds(timerTest.getSeconds() + 0.01);
				}
			}
		}		
	}

	App_Shutdown();

	return 0;
}

void App_Initialize( void )
{
	
	Gamepad gamepad;
	
	dList = malloc( 262144 );
	sceneList = malloc( 262144 );
	fbp0  = 0;
 
	sceKernelDcacheWritebackAll();
	pspDebugScreenInit();
	
	///////////////////////////////////////////////////
	// Load Resources
	///////////////////////////////////////////////////
	printf("Loading Lightwave Scene: %s\n", sceneName);
	loadLWS(myScene, sceneName);	
	
	printf("Caching CVR scene: %s\n", sceneName);
	//myScene.cache(true);

	
	//printf("Retrieving scene motion controller: %iu\n", myScene.motion(sceneName));
	
	///////////////////////////////////////////////////
	// END Load Resources
	///////////////////////////////////////////////////
	printf("--------------------------\n");
	printf("   Press X to start...\n");
	printf("--------------------------\n");
	
	while(!gamepad.ButtonDown(PSP_CTRL_CROSS))
	{
		gamepad.Poll();
	}
	
	InitGU();
	SetupProjection();
	SetupCallbacks();
}

void SetupProjection( void )
{
	// setup matrices for the triangle
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective( 75.0f, 16.0f/9.0f, 0.5f, 1000.0f);
 
    sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
 	
	sceGuClearColor( GU_COLOR( 0.0f, 0.0f, 0.0f, 1.0f ) ); 
	sceGuClearDepth(0);	
}

void App_Shutdown(void)
{
	// Free Memory
	free( dList );
	free( sceneList );
	free( fbp0 );
	
	sceGuTerm();			// Terminating the Graphics System
 
	sceKernelExitGame();
}

void App_Render(void)
{
	DrawScene();
	DebugScreen();
			
	sceDisplayWaitVblankStart();	
	fbp0 = sceGuSwapBuffers();
}

void InitGU( void )
{
	// Init GU
	sceGuInit();
	sceGuStart( GU_DIRECT, dList );

	// Set Buffers
	sceGuDrawBuffer( GU_PSM_8888, fbp0, BUF_WIDTH );
	sceGuDispBuffer( SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
	sceGuDepthBuffer( (void*)0x110000, BUF_WIDTH);

 	sceGuOffset( 2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
	sceGuDepthRange( 65535, 0);

	// Set Render States
	sceGuScissor( 0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable( GU_SCISSOR_TEST );
	sceGuDepthFunc( GU_GEQUAL );
	sceGuEnable( GU_DEPTH_TEST );
	sceGuFrontFace( GU_CW );
	sceGuEnable( GU_CULL_FACE );					
	sceGuShadeModel( GU_SMOOTH );
	sceGuEnable( GU_CLIP_PLANES );
	sceGuEnable( GU_TEXTURE_2D );
	sceGuEnable( GU_LIGHTING );
	sceGuEnable( GU_LIGHT0 );						// Enable Light 1 (NEW)

	// setup texture
	// 32-bit image, if we swizzled the texture will return true, otherwise false (NEW)
	sceGuTexMode( GU_PSM_8888, 0, 0, true );	
	sceGuTexFunc( GU_TFX_MODULATE, GU_TCC_RGB );	// Modulate the color of the image
	sceGuTexScale( 1.0f, 1.0f );					// No scaling
	sceGuTexOffset( 0.0f, 0.0f );

	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
	// finish
}


void DrawScene( void )
{
#warning Try taking out this display list at some point
	sceGuStart( GU_DIRECT, sceneList );

	sceGuViewport( 2048, 2048, SCR_WIDTH, SCR_HEIGHT);	

	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	sceGumMatrixMode(GU_MODEL);		
	sceGumLoadIdentity();	// Reset the Matrix

	myScene.render();	// Render the scene
	
	sceGuFinish();
	sceGuSync(0,0);
}


void DebugScreen( void )
{
	fps++;
	sceRtcGetCurrentTick( &fpsTickNow );
	
	if( ((fpsTickNow - fpsTickLast)/((float)tickResolution)) >= 1.0f )
	{
		fpsTickLast = fpsTickNow;
		sprintf( fpsDisplay, "FPS: %i", fps );
		fps = 0;
	}
	pspDebugScreenSetOffset( (int)fbp0 );
	pspDebugScreenSetXY( 0, 0 );
	pspDebugScreenPrintf( fpsDisplay );
	
	
	pspDebugScreenSetXY( 0, 1 );
	pspDebugScreenPrintf( "%f", timerTest.getSeconds() );
	
	pspDebugScreenSetXY( 0, 2 );
	pspDebugScreenPrintf( "%f %f %f %d", myScene.cam->position.x, myScene.cam->position.y, myScene.cam->position.z, myScene.cam->type);
}

void FPS( void )
{
	fps++;
	sceRtcGetCurrentTick( &fpsTickNow );
	
	if( ((fpsTickNow - fpsTickLast)/((float)tickResolution)) >= 1.0f )
	{
		fpsTickLast = fpsTickNow;
		sprintf( fpsDisplay, "FPS: %i", fps );
		fps = 0;
	}
	pspDebugScreenSetOffset( (int)fbp0 );
	pspDebugScreenSetXY( 0, 0 );
	pspDebugScreenPrintf( fpsDisplay );
}
