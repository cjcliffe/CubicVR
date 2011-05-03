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

#include "CubicVR/Gamepad_360.h"

namespace CubicVR
{
		Gamepad::Gamepad()
		{
			bRightStickInverted = true;
		}

		Gamepad::~Gamepad()
		{
		}
		
		/*
		int Gamepad::Get_Rumble_Motor(int iWhich)
		{
			if(iWhich == GAMEPAD_MOTOR_LEFT)			return vibration.wLeftMotorSpeed;
			else										return vibration.wRightMotorSpeed;
		}

		void Gamepad::Rumble(int leftMotor, int rightMotor)
		{
			// Validate Left Motor Rumble Amount and then set
			if(leftMotor >= 0 && leftMotor <= 65535)
				vibration.wLeftMotorSpeed  = leftMotor;     // use any value between 0-65535

			// Validate Right Motor Rumble Amount and then set
			if(rightMotor >= 0 && rightMotor <= 65535)
				vibration.wRightMotorSpeed = rightMotor;	// use any value between 0-65535

			// Set the state
			XInputSetState( 0, &vibration ); 
		}
		*/

		int Gamepad::Button_Pressed(GamepadButtons buttons)
		{
			if(state.Gamepad.wButtons & (int)buttons)			return 1;
			
			return 0;
		}

		int Gamepad::Button_Held(GamepadButtons buttons)
		{
			// To-do:	Fix this to properly read if a button is held.
			if(state.Gamepad.wButtons & (int)buttons)			return 1;

			return 0;
		}

		int Gamepad::Trigger(GamepadTriggers trigger)
		{
			// LEFT TRIGGER
			if(trigger == GamepadTriggers::TriggerLeft)
			{
				if(state.Gamepad.bLeftTrigger && state.Gamepad.bLeftTrigger)
				{
					if(state.Gamepad.bLeftTrigger > 0)		return state.Gamepad.bLeftTrigger / 255;				
				}
			}
			// RIGHT TRIGGER
			else
			{
				if(state.Gamepad.bRightTrigger && state.Gamepad.bRightTrigger)
				{
					if(state.Gamepad.bRightTrigger > 0)		return state.Gamepad.bRightTrigger / 255;					
				}
			}

			return 0;
		}

		bool Gamepad::Trigger_Pressed(GamepadTriggers trigger)
		{
			if(Trigger(trigger))		return 1;

			return 0;
		}

		/**
		* Find out whether or not the right stick is inverted.
		* @return A boolean state of the right stick being inverted.
		*/
		bool Gamepad::Inverted_Y()
		{
			return bRightStickInverted;
		}
		
		float Gamepad::Axis(GamepadSticks sticks, GamepadAxis direction)
		{	
			// Left Stick
			if(sticks == GamepadSticks::StickLeft)
			{
				float fThumb = 0;
							
				if(direction == GamepadAxis::AxisHorizontal)		fThumb = state.Gamepad.sThumbLX;
				else												fThumb = state.Gamepad.sThumbLY;

				if(fThumb != 0)		fThumb = fThumb / 32768;

				return fThumb;
			}
			// Right Stick
			else
			{
				float fThumb = 0;
				
				if(direction == GamepadAxis::AxisHorizontal)		fThumb = state.Gamepad.sThumbRX;
				else												fThumb = state.Gamepad.sThumbRY;

				if(fThumb != 0)		fThumb = fThumb / 32768;

				return fThumb;
			}

			return 0;
		}

		void Gamepad::Poll()
		{
			// Poll hooks into the DirectX XInput library and updates the state of the gamepad.
			
			// Get all the current states of controller 1
			dwResult = XInputGetState( 0, &state );

			if((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
				(state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
			{
				state.Gamepad.sThumbLX = 0;
				state.Gamepad.sThumbLY = 0;
			}

			if((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
				(state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
			{
				state.Gamepad.sThumbRX = 0;
				state.Gamepad.sThumbRY = 0;
			}
		}
}