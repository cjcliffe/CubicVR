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

#ifndef CUBICVR_GAMEPAD_360_H
#define CUBICVR_GAMEPAD_360_H


#ifdef WIN32

	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"
	#include <XInput.h>

	namespace CubicVR
	{
			enum GamepadButtons		{	A				= XINPUT_GAMEPAD_A, 
										B				= XINPUT_GAMEPAD_B,
										X				= XINPUT_GAMEPAD_X,
										Y				= XINPUT_GAMEPAD_Y,
										LeftShoulder	= XINPUT_GAMEPAD_LEFT_SHOULDER,
										RightShoulder	= XINPUT_GAMEPAD_RIGHT_SHOULDER,
										LeftThumb		= XINPUT_GAMEPAD_LEFT_THUMB,
										RightThumb		= XINPUT_GAMEPAD_RIGHT_THUMB,
										Back			= XINPUT_GAMEPAD_BACK,
										Start			= XINPUT_GAMEPAD_START,
										DPad_Up			= XINPUT_GAMEPAD_DPAD_UP,
										DPad_Down		= XINPUT_GAMEPAD_DPAD_DOWN,
										DPad_Left		= XINPUT_GAMEPAD_DPAD_LEFT,
										DPad_Right		= XINPUT_GAMEPAD_DPAD_RIGHT };
			
			enum GamepadAxis		{	AxisHorizontal	= 0,
										AxisVertical	= 1 };
			enum GamepadSticks		{	StickLeft		= 0,
										StickRight		= 1 };
			enum GamepadTriggers	{	TriggerLeft		= 0,
										TriggerRight	= 1 };
			enum GamepadMotors		{	MotorLeft		= 0,
										MotorRight		= 1 };

			/**
			* The Gamepad class represents a mapping to an Xbox 360 controller.
			*
			* @author Derek Doucett
			*/
			class IMPEXP Gamepad
			{ 
				public:
					Gamepad();
					~Gamepad();
					
					void			Poll();
					
					int				Button_Pressed			(GamepadButtons buttons);
					int				Button_Held				(GamepadButtons buttons);

					float			Axis					(GamepadSticks  sticks, GamepadAxis direction);

					int				Trigger					(GamepadTriggers trigger);
					bool			Trigger_Pressed			(GamepadTriggers trigger);				

					bool			Inverted_Y				();

					//void			Rumble					(int leftMotor, int rightMotor);
					//int			Get_Rumble_Motor		(GamepadMotors motor);								

				private:
					bool			bRightStickInverted;	// Y-Axis Inverted?

					DWORD			dwResult;				// Used to store if a controller is connected
					XINPUT_STATE	state;					// Data type that stores all the current states
															// of a controller.
					XINPUT_VIBRATION vibration;				// Vibration State
			};
	}
#endif

#endif