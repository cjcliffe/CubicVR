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

#include <CubicVR/ObjectController.h>
#include <CubicVR/mat4.h>

using namespace cvr;

ObjectController::ObjectController() : scale(1,1,1), rotation(0,0,0), position(0,0,0), l_init(false), w_init(false), matrix_lock(false), t_moved(false), t_bounds(true), is_targeted(false), p_trans(NULL)
{
};

ObjectController::~ObjectController()
{

};

void ObjectController::targeted(bool targeted_in)
{
	is_targeted = targeted_in;
};


void ObjectController::setMatrix(float t_trans_in[16])
{
	//for (int i = 0; i < 16; i++) t_trans[i] = t_trans_in[i];
	memcpy(w_trans,t_trans_in,16*sizeof(float));
//	t_bounds = true;
//	t_moved = false;
//	matrix_lock = true;
//	l_init = false;
//	w_init = false;
//	clearTransform();
	
	XYZ min,max;

	glMatrixMode(GL_MODELVIEW);
//	if (scale.x != 1 || scale.y != 1 || scale.z != 1)
//	{
//		glPushMatrix();
//		glLoadIdentity();
//		glMultMatrixf(w_trans);
//		glScalef(scale.x,scale.y,scale.z);
//		glGetFloatv(GL_MODELVIEW_MATRIX, w_trans);
//		glPopMatrix();
//	}
	
	
//	clearTransform();
//	t_moved = false;
}

void ObjectController::setMatrixLock(bool matrix_lock_state)
{
	matrix_lock = matrix_lock_state;
}


void ObjectController::transformBegin(bool no_pivot, bool no_scale, bool update_only)
{
	//glMatrixMode(GL_MODELVIEW);

	if ((!l_init || !w_init) && !matrix_lock)
	{
	//	glPushMatrix();

		if (!l_init)
		{
			//glLoadIdentity();
			mat4::set_identity(l_trans);
			

			if (position.x || position.y || position.z)
			{
				//glTranslatef(position.x,position.y,position.z);
				mat4::translate(l_trans,position.x,position.y,position.z);
			}

			
			if (is_targeted)
			{
				float dx, dy, dz, dxz;
				
				dx = target.x-position.x;
				dy = target.y-position.y;
				dz = target.z-position.z;
				
				
				dxz = sqrt(dx*dx+dz*dz);
				
				/* !mac atan2l ? */
				rotation.y = atan2(dx,dz)*(180.0f/M_PI);
				rotation.x = -atan2(dy,dxz)*(180.0f/M_PI);
			}


/*	#ifdef ARCH_PSP
			if (rotation.x || rotation.y || rotation.z)
			{
				if (rotation.x)
				{
					sceGumRotateX(DEGTORAD(rotation.x));
				}
				if (rotation.y)
				{
					sceGumRotateY(DEGTORAD(rotation.y));
				}
				if (rotation.z)
				{
					sceGumRotateZ(DEGTORAD(rotation.z));
				}
			}			
	#else */
			if (rotation.x || rotation.y || rotation.z)
			{
	/*			if (rotation.x)
				{
					glRotatef(rotation.x,1,0,0);
				}
				if (rotation.y)
				{
					glRotatef(rotation.y,0,1,0);
				}
				if (rotation.z)
				{
					glRotatef(rotation.z,0,0,1);
				}*/
				mat4::rotate(l_trans,rotation.x,rotation.y,rotation.z);
			}			
//		#endif
		
			if (!no_pivot)
			{
				if (pivot.x||pivot.y||pivot.z) 
				{
					//glTranslatef(-pivot.x,-pivot.y,-pivot.z);
					mat4::translate(l_trans,-pivot.x,-pivot.y,-pivot.z);
				}
			}
			
//			if (scale.x != 1 || scale.y != 1 || scale.z != 1 && !no_scale)
//			{
//		#ifdef ARCH_PSP
//				ScePspFVector3 scaleVec = { scale.x, scale.y, scale.z };
//				sceGumScale(&scaleVec);
//		#else
//				glScalef(scale.x,scale.y,scale.z);
//		#endif
//			}
		


//#ifdef ARCH_PSP
//				sceGumStoreMatrix((ScePspFMatrix4 *) l_trans);
//#else			
//				glGetFloatv(GL_MODELVIEW_MATRIX, l_trans);
//#endif
//				glPushMatrix();
//					glMultMatrixf(l_trans);
//				glPopMatrix();
			if (!p_trans) {
        mat4::copy(w_trans,l_trans);
        w_init=true;
			}
		}

		if (!w_init)
		{
			if (p_trans)
			{
//					glLoadIdentity();
          mat4::set_identity(w_trans);
          mat4::multiply(w_trans,p_trans->w_trans,l_trans);
          
//					glMultMatrixf(p_trans->w_trans);
//					glMultMatrixf(l_trans);
#ifdef ARCH_PSP
//				sceGumStoreMatrix((ScePspFMatrix4 *) l_trans);
#else			
//				glGetFloatv(GL_MODELVIEW_MATRIX, w_trans);
#endif
			}
			else
			{
//				glGetFloatv(GL_MODELVIEW_MATRIX, w_trans);
			}
		}

		//glPopMatrix();
		
		
		if (!update_only)
		{
			glPushMatrix();
			glMultMatrixf(w_trans);
		}
		
		l_init = true;
		w_init = true;
		t_bounds = true;
	}
	else 
	{
		if (!update_only)
		{
			glPushMatrix();
				glMultMatrixf(w_trans);
				if (!no_scale && (scale.x != 1 || scale.y != 1 || scale.z != 1))
				{
			#ifdef ARCH_PSP
					ScePspFVector3 scaleVec = { scale.x, scale.y, scale.z };
					sceGumScale(&scaleVec);
			#else
					glScalef(scale.x,scale.y,scale.z);
			#endif
				}
			
		}
	}	// end gl

};

void ObjectController::transformEnd()
{
	//	if (pivot.x||pivot.y||pivot.z&&trans_last) 
//	{
//		glPopMatrix();
//	}
#ifdef ARCH_PSP
	sceGumPopMatrix();
#else
		glPopMatrix();
#endif
};

void ObjectController::transformReverseBegin()
{
#ifdef ARCH_PSP
	sceGumPushMatrix();
#else
	glPushMatrix();
#endif
	
	if (rotation.x || rotation.y || rotation.z)
	{
		if (rotation.x)
		{
#ifdef ARCH_PSP
			sceGumRotateX(DEGTORAD(rotation.x));
#else
			glRotatef(-rotation.x,1,0,0);
#endif
		}
		if (rotation.y)
		{
#ifdef ARCH_PSP
			sceGumRotateY(DEGTORAD(rotation.y));
#else
			glRotatef(-rotation.y,0,1,0);
#endif
		}
		if (rotation.z)
		{
#ifdef ARCH_PSP
			sceGumRotateZ(DEGTORAD(rotation.z));
#else
			glRotatef(-rotation.z,0,0,1);
#endif
		}
	}

	if (position.x || position.y || position.z)
	{
#ifdef ARCH_PSP
		ScePspFVector3 transVec = { -position.x, -position.y, -position.z };

		sceGumTranslate(&transVec);
#else
		glTranslatef(-position.x,-position.y,-position.z);
#endif
	}

//	if (scale.x != 1 || scale.y != 1 || scale.z != 1)
//	{
//#ifdef ARCH_PSP
//		ScePspFVector3 scaleVec = { scale.x, scale.y, scale.z };
//		sceGumScale(&scaleVec);
//#else
//		glScalef(scale.x,scale.y,scale.z);
//#endif
//	}
	
	if (pivot.x||pivot.y||pivot.z) 
	{
#ifdef ARCH_PSP
		ScePspFVector3 transVec = { pivot.x,pivot.y,pivot.z };

		sceGumTranslate(&transVec);
#else
		glTranslatef(pivot.x,pivot.y,pivot.z);
#endif
	}

};

void ObjectController::transformReverseEnd()
{
#ifdef ARCH_PSP
	sceGumPopMatrix();
#else
	glPopMatrix();
#endif
};



void ObjectController::getAxis(XYZ &right, XYZ &up, XYZ &forward)
{
	float m_abs[16];
	
	std::vector<ObjectController *> objStack;
	
	ObjectController *tempSceneObj = this;
	
	while (tempSceneObj->p_trans)
	{
		objStack.push_back(tempSceneObj->p_trans);
		tempSceneObj = tempSceneObj->p_trans;
	}
	
	ObjectController axisControl;
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	for (int i = objStack.size()-1; i >= 0; i--)
	{
		axisControl.rotation = objStack[i]->rotation;
		axisControl.transformReverseBegin();
	}		
	
	glGetFloatv(GL_MODELVIEW_MATRIX, m_abs);
	
	for (int i = objStack.size()-1; i >= 0; i--)
	{
		axisControl.transformReverseEnd();
	}		
	glPopMatrix();
	
	XYZ ws_right(1,0,0), ws_up(0,1,0), ws_forward(0,0,1);	// worldspace axes
	Vector ca_right, ca_up, ca_forward; // child space axes
	
	matTransform(m_abs,ws_right,ca_right);
	matTransform(m_abs,ws_up,ca_up);
	matTransform(m_abs,ws_forward,ca_forward);
	
	ca_right.makeUnit();
	ca_up.makeUnit();
	ca_forward.makeUnit();
	
	right = ca_right;
	up = ca_up;
	forward = ca_forward;
}


void ObjectController::getWorldAxis(XYZ &right, XYZ &up, XYZ &forward)
{
	float m_abs[16];
	
	std::vector<ObjectController *> objStack;
	
	ObjectController *tempSceneObj = this;
	
	while (tempSceneObj)
	{
		objStack.push_back(tempSceneObj);
		tempSceneObj = tempSceneObj->p_trans;
	}
	
	ObjectController axisControl;
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	for (int i = objStack.size()-1; i >= 0; i--)
	{
		axisControl.rotation = objStack[i]->rotation;
		axisControl.transformReverseBegin();
	}		
	
	glGetFloatv(GL_MODELVIEW_MATRIX, m_abs);
	
	for (int i = objStack.size()-1; i >= 0; i--)
	{
		axisControl.transformReverseEnd();
	}		
	glPopMatrix();
	
	XYZ ws_right(1,0,0), ws_up(0,1,0), ws_forward(0,0,1);	// worldspace axes
	Vector ca_right, ca_up, ca_forward; // child space axes
	
	matTransform(m_abs,ws_right,ca_right);
	matTransform(m_abs,ws_up,ca_up);
	matTransform(m_abs,ws_forward,ca_forward);
	
	ca_right.makeUnit();
	ca_up.makeUnit();
	ca_forward.makeUnit();
	
	right = ca_right;
	up = ca_up;
	forward = ca_forward;
}


XYZ ObjectController::getWorldPosition()
{
	if (!w_init) transformBegin(false,false,true);
	return XYZ(w_trans[12],w_trans[13],w_trans[14]);
}


//XYZ ObjectController::getWorldScale()
//{
//	if (!w_init) transformBegin(false,false,true);
//	return XYZ(w_trans[0],w_trans[5],w_trans[10]);
//}


XYZ ObjectController::getWorldRotation()
{
	if (!w_init) transformBegin(false,false,true);
	XYZ rot_out;

	btTransform basis;
	basis.setFromOpenGLMatrix(w_trans);
	
	btQuaternion rot = basis.getRotation();
	
	float w=rot.getW();   float x=rot.getX();   float y=rot.getY();   float z=rot.getZ();
	float sqw = w*w; float sqx = x*x; float sqy = y*y; float sqz = z*z;
	rot_out.z = RADTODEG((atan2(2.0f * (x*y + z*w),(sqx - sqy - sqz + sqw))));
	rot_out.x = RADTODEG((atan2(2.0f * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
	rot_out.y = RADTODEG((asin(-2.0f * (x*z - y*w)))); 
	
	return rot_out;
}

