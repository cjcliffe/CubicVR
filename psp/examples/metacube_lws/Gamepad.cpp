#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>			// PSP Gamepad Controller

#include "Gamepad.h"

Gamepad::Gamepad()
{
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG );
}

Gamepad::~Gamepad()
{
}

unsigned int Gamepad::Buttons()
{
	return pad.Buttons;
}
		
float Gamepad::AnalogX()
{
	return ((float)pad.Lx - 128.0f) / 128.0f;
}

float Gamepad::AnalogY()
{
	return ((float)pad.Ly - 128.0f) / 128.0f;
}

bool Gamepad::ButtonDown(unsigned int buttons)
{
	if(pad.Buttons & buttons)		return true;
	return false;
}

bool Gamepad::ButtonUp(unsigned int buttons)
{
	if(pad.Buttons & buttons)		return false;
	return true;
}

bool Gamepad::Poll()
{
	sceCtrlReadBufferPositive(&pad, 1);
	/*
	if(pad.Buttons != lastpad.Buttons)
	{
		lastpad.Buttons = pad.Buttons;
		return true;
	}
	
	return false;
	*/
	
	return true;
}
