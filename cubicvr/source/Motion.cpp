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

#include <CubicVR/Motion.h>


MotionController::MotionController()
{

};


Envelope &MotionController::envelope(int controllerId, int motionId)
{
	return controllers[controllerId][motionId];
};


void MotionController::apply(Controllable &objControllable, float index)
{
	std::map<int, std::map<int, Envelope, ltu_int>, ltu_int>::iterator controllers_i;
	std::map<int, Envelope, ltu_int>::iterator motions_i;
	
	for (controllers_i = controllers.begin(); controllers_i != controllers.end(); controllers_i++)
	{
		for (motions_i = (*controllers_i).second.begin(); motions_i != (*controllers_i).second.end(); motions_i++)
		{
			objControllable.control((*controllers_i).first, (*motions_i).first, (*motions_i).second.evaluate(index));
		}
	}
};

void MotionController::record(Controllable &objControllable, float index)
{
	std::map<int, std::map<int, Envelope, ltu_int>, ltu_int>::iterator controllers_i;
	std::map<int, Envelope, ltu_int>::iterator motions_i;
	
	for (controllers_i = controllers.begin(); controllers_i != controllers.end(); controllers_i++)
	{
		for (motions_i = (*controllers_i).second.begin(); motions_i != (*controllers_i).second.end(); motions_i++)
		{
			EnvelopeKey *e;
			e = envelope((*controllers_i).first, (*motions_i).first).addKey(index,objControllable.read_control((*controllers_i).first, (*motions_i).first));
			e->shape = ENV_SHAPE_LINE;
		}
	}
};



Motion::Motion()
{

};

void Motion::bind(Controllable &objControllable)
{
	controllers[&objControllable] = MotionController();
};

EnvelopeKey *Motion::setKey(Controllable &objControllable, int controllerId, int motionId, float index, float value)
{
	EnvelopeKey *tempKey;
	
	tempKey = controllers[&objControllable].envelope(controllerId,motionId).addKey(index,value);
	
	return tempKey;
};

void Motion::setBehavior(Controllable &objControllable, int controllerId, int motionId, int behavior_in, int behavior_out)
{
	controllers[&objControllable].envelope(controllerId,motionId).behavior(behavior_in, behavior_out);
};
		
void Motion::evaluate(float index)
{
	std::map<Controllable *, MotionController, ltu_cptr>::iterator controllers_i;
	
	for (controllers_i = controllers.begin(); controllers_i != controllers.end(); controllers_i++)
	{
		((*controllers_i).second).apply(*(*controllers_i).first,index);
	};
};	

void Motion::record(float index)
{
	std::map<Controllable *, MotionController, ltu_cptr>::iterator controllers_i;
	
	for (controllers_i = controllers.begin(); controllers_i != controllers.end(); controllers_i++)
	{
		((*controllers_i).second).record(*(*controllers_i).first,index);
	};
};	

