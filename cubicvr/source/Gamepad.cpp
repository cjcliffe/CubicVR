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

#include <CubicVR/Gamepad.h>
#include <CubicVR/Logger.h>

#ifdef __APPLE__
typedef struct cookie_struct
{
	IOHIDElementCookie gXAxisCookie;
	IOHIDElementCookie gYAxisCookie;
	IOHIDElementCookie gButton1Cookie;
} *cookie_struct_t;
#endif


Gamepad::Gamepad()
{
	rumbleLeft = 0;
	rumbleRight = 0;
	
	#ifdef __APPLE__
		Logger::log("Checking for Mac Controller...%d\n", setMacDevice());
	#endif
	
	#ifdef ARCH_PSP
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	#endif
	
	#ifdef ARCH_DC	
		#if defined(maple_first_controller)
			c = maple_first_controller();
		#else
			c = NULL;
			pad = NULL;
		#endif
	#endif
}

Gamepad::~Gamepad()
{
	#ifdef _WIN32
		Vibrate(0,0);
	#endif
	
	#ifdef ARCH_PSP
	#endif
	
	#ifdef ARCH_DC	
	#endif
}

void Gamepad::Vibrate(float leftMotor, float rightMotor)
{
	#ifdef _WIN32
		XINPUT_VIBRATION vibration;				// Vibration State

		// Validate Left Motor Rumble Amount and then set
		if(leftMotor >= 0 && leftMotor <= 1)
			vibration.wLeftMotorSpeed  = leftMotor * 65535;     // use any value between 0-65535

		// Validate Right Motor Rumble Amount and then set
		if(rightMotor >= 0 && rightMotor <= 1)
			vibration.wRightMotorSpeed = rightMotor * 65535;	// use any value between 0-65535

		// Set the state
		XInputSetState( 0, &vibration );
	#endif
}

bool Gamepad::Button(int btn)
{
	#ifdef __APPLE__
		if ( HIDIsValidDevice( mDevice ) )
		{
			pRecElement elRec = NULL;
			elRec = HIDGetFirstDeviceElement( mDevice, kHIDElementTypeInput );
			if ( elRec )
			{
				if(elRec->usagePage == kHIDPage_GenericDesktop)
				{
					
					if ( HIDIsValidElement( mDevice, elRec ) )
					{
						//printf("%s\n", HIDGetElementValue(mDevice, elRec));
					}
					
					// switch ( elRec->usage )
					// 					{
					// 						case kHIDUsage_GD_X:
					// 						printf("GD_X\n");
					// 						break;
					// 						case kHIDUsage_GD_Y:
					// 						printf("GD_Y\n");
					// 						break;
					// 						case kHIDUsage_GD_GamePad:
					// 						printf("GAMEPAD\n");
					// 						break;
					// 						//etc
					// 					}
				}
			}
		}
		return 0;
	#endif
	
	#ifdef _WIN32
		if(mState.Gamepad.wButtons & btn)			return 1;
	#endif
	
	#ifdef ARCH_PSP
		if (pad.Buttons & btn)						return 1;
	#endif

	#ifdef ARCH_DC
		#if defined(maple_first_controller)
			if(!(pad.buttons & btn))				return 1;
		#else
			if (!pad) return 0;
			if (pad->buttons & btn)					return 1;
		#endif
	#endif
	
	return 0;
}

float Gamepad::Left_Trigger()
{
	#ifdef _WIN32
		if(mState.Gamepad.bLeftTrigger && mState.Gamepad.bLeftTrigger)
		{
			if(mState.Gamepad.bLeftTrigger > 0)		return (float)((float)mState.Gamepad.bLeftTrigger / 255);				
		}
	#endif
	
	#ifdef ARCH_DC
		if (!pad) return 0;
		#if defined(maple_first_controller)
			return (float)((float)pad.ltrig / 255);			
		#else
			return (float)((float)pad->ltrig / 255);
		#endif
	#endif
	
	return 0;
}

float Gamepad::Right_Trigger()
{
	#ifdef _WIN32
		if(mState.Gamepad.bRightTrigger && mState.Gamepad.bRightTrigger)
		{
			if(mState.Gamepad.bRightTrigger > 0)		return (float)((float)mState.Gamepad.bRightTrigger / 255);				
		}
	#endif
	
	#ifdef ARCH_DC
		if (!pad) return 0;
		#if defined(maple_first_controller)
			return (float)((float)pad.rtrig / 255);			
		#else
			return (float)((float)pad->rtrig / 255);
		#endif
	#endif
	
	return 0;
}

void Gamepad::Update()
{
	#ifdef _WIN32
	    DWORD dwResult;         // Used to store if a controller is connected
		XINPUT_STATE state;     // Data type that stores all the current states
								// of a controller.
		
		ZeroMemory( &state, sizeof(XINPUT_STATE) );		// Zeros out the states of
														// the controller.
		// Get all the current states of controller 1
		dwResult = XInputGetState( 0, &state );
		mConnected = dwResult;
		
		// Controller is connected.
		if( mConnected == ERROR_SUCCESS )
		{
			mState = state;
		}
		// Controller is disconnected.
		else
		{
		}

		Tune();
	#endif
	
	#ifdef ARCH_PSP
		sceCtrlReadBufferPositive(&pad, 1);
	#endif
	
	#ifdef ARCH_DC
		#if defined(maple_first_controller)
			cont_get_cond(c, &pad);
		#else
			c = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
		
			if (c) {
				pad = (cont_state_t *)maple_dev_status(c);
			} else {
				pad = NULL;
			}
		#endif

		// else if(buttons == CONTROLLER_L_TRIG)			return cond.ltrig;
		// else if(buttons == CONTROLLER_R_TRIG)			return cond.rtrig;
		// else if(!(cond.buttons & buttons))				return 1;
	#endif
}

float Gamepad::X_Axis()
{
	return Axis(0, GAMEPAD_LEFT);
}

float Gamepad::X_Axis(int stick)
{
	return Axis(0, stick);
}

float Gamepad::Y_Axis()
{
	return Axis(1, GAMEPAD_LEFT);
}

float Gamepad::Y_Axis(int stick)
{
	return Axis(1, stick);
}

float Gamepad::Axis(int axi, int which)
{
	float value = 0;
	
	Update();
	

	// X axis
	if(axi == 0)
	{
		#ifdef __APPLE__
			//Get x axis
			//float result = mDevice->getDeviceInterface()->getElementValue(hidDeviceInterface, cookies->gXAxisCookie, &hidEvent);
			//if (result)		printf("getElementValue error = %lx", result);
			//xAxis = hidEvent.value;
			
		#endif
		
		#ifdef WIN32
			if(which == GAMEPAD_RIGHT)		value = float((float)mState.Gamepad.sThumbRX / 32768);
			else							value = float((float)mState.Gamepad.sThumbLX / 32768);
		#endif
		
		#ifdef ARCH_PSP
			value = float(float(pad.Lx - 128) / 128);
		#endif
			
		#ifdef ARCH_DC
			if (!pad) return 0;
			#if defined(maple_first_controller)
				value = float(float(pad.joyx - 128) / 128);
			#else
				value = float(float(pad->joyx) / 128);
			#endif
		#endif
	}
	
	// Y Axis
	else
	{
		#ifdef _WIN32
			if(which == GAMEPAD_RIGHT)		value = float((float)mState.Gamepad.sThumbRY / 32768);
			else							value = float((float)mState.Gamepad.sThumbLY / 32768);
		#endif
		
		#ifdef ARCH_PSP
			value = float(float(pad.Ly - 128) / 128);
		#endif
		
		#ifdef ARCH_DC
			if (!pad) return 0;
			#if defined(maple_first_controller)
				value = float(float(pad.joyy - 128) / 128);
			#else
				value = float(float(pad->joyy) / 128);
			#endif
		#endif
	}
	
	return value;
}

void Gamepad::Tune()
{
	#ifdef _WIN32
		if((mState.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			mState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
			(mState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			mState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			mState.Gamepad.sThumbLX = 0;
			mState.Gamepad.sThumbLY = 0;
		}

		if((mState.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			mState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
			(mState.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			mState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			mState.Gamepad.sThumbRX = 0;
			mState.Gamepad.sThumbRY = 0;
		}
	#endif
}

void Gamepad::Set_Y_Axis_Inverted(int v)
{
	bRightStickInverted = v ? 1 : 0;
}

int Gamepad::Y_Axis_Inverted()
{
	if(bRightStickInverted)		return 1;
	return 0;
}

void Gamepad::control(int controllerId, int motionId, float value)
{
	int rumbleLeftOriginal = rumbleLeft;
	int rumbleRightOriginal = rumbleRight;

	switch (controllerId)
	{
		case GAMEPAD_RUMBLE:
			switch (motionId)
			{
				case RUMBLE_LEFT: rumbleLeft = value; break;
				case RUMBLE_RIGHT: rumbleRight = value; break;
			}
		break;
	}

	if(rumbleLeftOriginal != rumbleLeft || rumbleRightOriginal != rumbleRight)
	{
		Vibrate(rumbleLeft, rumbleRight);
	}
}

#ifdef __APPLE__
bool Gamepad::setMacDevice()
{
	int num_devices = 0;
	
	HIDBuildDeviceList( NULL, NULL );
	
	if (! HIDHaveDeviceList() )		return NULL;
	
	num_devices = HIDCountDevices();
	
	pRecDevice deviceCandidate = NULL, *stored_device[100 ];
	int i, foundDevices = 0;
	
	deviceCandidate = HIDGetFirstDevice();
	
	i = 0;
	while ( i < num_devices )
	{
		if ( deviceCandidate )
		{
			//here I filter out keyboards, mice and totally unrelated devices
			// kHIDUsage_GD_GamePad
			
			if ( (deviceCandidate->usage == kHIDUsage_GD_Keyboard) || (deviceCandidate->usagePage != kHIDPage_GenericDesktop) || (deviceCandidate->usage == kHIDUsage_GD_Mouse) )
			{
				if ( HIDIsValidDevice( deviceCandidate ) )
				{
					//skip this one and just go on to the next one
					deviceCandidate = HIDGetNextDevice( deviceCandidate );
				}
				else
				{
					//handle error
				}
			}
			else
			{
				if ( HIDIsValidDevice( deviceCandidate ) )
				{
					//store the device in a list of devices we're interested in
					
					if(deviceCandidate->productID == 654)			// 360 Controller
					{
						Logger::log(LOG_NOTICE,"Found 360 Controller on Mac.\n");
						mDevice = deviceCandidate;
						stored_device[foundDevices] = &deviceCandidate;
						deviceCandidate = HIDGetNextDevice( deviceCandidate );
						foundDevices++;
						return true;
					}
					else
					{
						deviceCandidate = HIDGetNextDevice( deviceCandidate );
					}
				}
				else
				{
					//handle error
				}
				
			}
			
			i++;
		}
	}
	
	return false;
}
#endif
