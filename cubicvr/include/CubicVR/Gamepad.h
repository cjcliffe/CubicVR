#pragma once
/*
    This file is part of CubicVR.

    Copyright (C) 2003 by Charles J. Cliffe

		Permission is hereby granted, free of charge, to any person obtaining a copy
		of this software and associated documentation files (the "Software"), to deal
		in the Software without restriction, including without limitation the rights
		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		copies of the Software, and to permit persons to whom the Software is
		furnished to do so, subject to the following conditions:

		The above copyright notice and this permission notice shall be included in
		all copies or substantial portions of the Software.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
		THE SOFTWARE.
*/


#include <stdio.h>

#include <CubicVR/cvr_defines.h>
#include <CubicVR/Motion.h>

#define GAMEPAD_RUMBLE			100
#define RUMBLE_LEFT				101
#define RUMBLE_RIGHT			102

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"
	#include <XInput.h>                       // Defines XBOX controller API
	#pragma comment(lib, "XInput.lib")        // Library containing necessary 360
											  // functions 
#endif

#ifdef __APPLE__
	#include <IOKit/hid/IOHIDUsageTables.h>
	#include "HID_Utilities_External.h"
#endif

#ifdef ARCH_PSP
	#include <psptypes.h>
	#include <pspctrl.h>
#endif

#ifdef ARCH_DC
	#include <kos.h>
#endif

#include "Gamepad_Mapping.h"

#define GAMEPAD_LEFT		0
#define GAMEPAD_RIGHT		1

class IMPEXP Gamepad : public Controllable
{
public:
	Gamepad();
	~Gamepad();
	
	bool Button(int btn);
	void Update();
	
	float X_Axis();
	float X_Axis(int stick);
	float Y_Axis();	
	float Y_Axis(int stick);	
		
	float Left_Trigger();
	float Right_Trigger();

	void Vibrate(float leftMotor, float rightMotor);
	
	void control(int controllerId, int motionId, float value);
	
//////////////////////////////////////////////////////////////////////////////////////
// Private Functions
//////////////////////////////////////////////////////////////////////////////////////
private:
	float Axis(int axi, int which);	

	void Tune();	
	void Set_Y_Axis_Inverted(int v);	
	int Y_Axis_Inverted();
		
//////////////////////////////////////////////////////////////////////////////////////
// Private Variables
//////////////////////////////////////////////////////////////////////////////////////
private:
	bool				bRightStickInverted;	// Y-Axis Inverted?
	
//////////////////////////////////////////////////////////////////////////////////////
// Private Variables Gamepad Setup for platform
//////////////////////////////////////////////////////////////////////////////////////
private:
	int rumbleLeft;									// Rumble Left
	int rumbleRight;								// Rumble Right

	#ifdef __APPLE__
		bool 				setMacDevice();
		pRecDevice 			mDevice;				// Darwin Device
	#endif
	
	#ifdef _WIN32
		DWORD				mConnected;				// Used to store if a controller is connected
		XINPUT_STATE		mState;					// Data type that stores all the current states of a controller.
		XINPUT_VIBRATION	mVibration;				// Vibration State
	#endif
	
	#ifdef ARCH_PSP
		SceCtrlData 		pad;					// PSP Datastruct for controller info
	#endif
	
	#ifdef ARCH_DC
		maple_device_t*		c;					// Dremacast controller
		cont_state_t* 		pad;					// DC Datastruct for controller info
	#endif
};
