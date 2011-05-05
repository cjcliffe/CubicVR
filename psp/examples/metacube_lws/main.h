#ifndef MAIN_H
#define MAIN_H

#include <psptypes.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>				// PSP GU
#include <pspgum.h>				// PSP GU Matrix

#include <psprtc.h>				// for the timer/fps functions

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////////////

#define printf pspDebugScreenPrintf 
#define RGB(r, g, b) ((r)|((g)<<8)|((b)<<16))

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

void *dList;							// display List, used by sceGUStart
void *sceneList;							// display List, used by sceGUStart
void *fbp0;								// frame buffer
 
int fps = 0;							// for calculating the frames per second
char fpsDisplay[100];
u32 tickResolution;
u64 fpsTickNow;
u64 fpsTickLast;

unsigned int color = GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );

//////////////////////////////////////////////////////////////////////////////////
// PROTOTYPES
//////////////////////////////////////////////////////////////////////////////////

void DebugScreen( void );						// Display Frames Per Second 
void InitGU( void );			        // Initialize the Graphics Subsystem
void SetupProjection( void );	        // Setups the Projection Matrix


//////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;
	
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	
	sceKernelSleepThreadCB();
	
	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;
	
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}
	
	return thid;
}

#endif
