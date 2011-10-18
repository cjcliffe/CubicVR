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

#include <CubicVR/Bone.h>
#include <CubicVR/Motion.h>
#include <CubicVR/Renderer.h>
#include <CubicVR/Transform.h>
#include <CubicVR/GLExt.h>

Bone::Bone() :  parent(NULL), restLength(1), strength(1)
{
	center = XYZ(0,0,0);
};



Bone::~Bone()
{

};


void Bone::calcMatrix(bool childBones, bool firstBone)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (parent)
	{
		glMultMatrixf(parent->matrix);
		glPushMatrix();
	}
	
	glTranslatef(position.x,position.y,position.z);

#ifdef ARCH_PSP
	sceGumRotateX(DEGTORAD(rotation.x)-DEGTORAD(restDirection.x));
	sceGumRotateY(DEGTORAD(rotation.y)-DEGTORAD(restDirection.y));
	sceGumRotateZ(DEGTORAD(rotation.z)-DEGTORAD(restDirection.z));
#else
	glRotatef(rotation.z-restDirection.z,0,0,1);
	glRotatef(rotation.y-restDirection.y,0,1,0);
	glRotatef(rotation.x-restDirection.x,1,0,0);
#endif	
//	glRotatef(-restDirection.z,0,0,1);
//	glRotatef(-restDirection.y,0,1,0);
//	glRotatef(-restDirection.x,1,0,0);

	glTranslatef(-restPosition.x,-restPosition.y,-restPosition.z);

	glGetFloatv(GL_MODELVIEW_MATRIX,matrix);

	if (parent) glPopMatrix();
	glPopMatrix();	

	trans_init = true;
	
	if (childBones) for (unsigned int i = 0; i < children.size(); i++)			
	{
		children[i]->calcMatrix(childBones);
	}
}


void Bone::calcNodes(bool firstBone)
{
	float nodeMat[16];
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	if (firstBone) 
	{
		glLoadIdentity();
		glTranslatef(restPosition.x,restPosition.y,restPosition.z);
	}
	else
	{
		glTranslatef(position.x,position.y,position.z-1);
	}
	
#ifdef ARCH_PSP
	sceGumRotateX(DEGTORAD(rotation.x));
	sceGumRotateY(DEGTORAD(rotation.y));
	sceGumRotateZ(DEGTORAD(rotation.z));
#else
	glRotatef(rotation.x,0,0,1);
	glRotatef(rotation.y,0,1,0);
	glRotatef(rotation.z,1,0,0);
#endif
	
	glGetFloatv(GL_MODELVIEW_MATRIX,nodeMat);
	
	XYZ start_pt_trans(0,0,0);
	matTransform(nodeMat,start_pt_trans,start_pt);
	
	XYZ end_pt_trans(0,0,restLength);
	matTransform(nodeMat,end_pt_trans,end_pt);
	
	glTranslatef(0,0,restLength);
	
	for (unsigned int i = 0; i < children.size(); i++)			
	{
		children[i]->calcNodes(false);
	}
	glPopMatrix();
}


#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
void Bone::debugDrawNode()
{
	glLineWidth(1);
	
	glBegin(GL_LINES);
	
	glColor3f(31.0f/255.0f,249.0f/255.0f,1.0f);

	glColor3f(0,1,0);
	glVertex3f(start_pt.x,start_pt.y,start_pt.z);
	glColor3f(1,0,0);
	glVertex3f(end_pt.x,end_pt.y,end_pt.z);
	
	glEnd();
	
	for (unsigned int i = 0; i < children.size(); i++)			
	{
		children[i]->debugDrawNode();
	}
	
}


void Bone::debugDraw(bool firstBone)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (firstBone) 
	{
		glTranslatef(restPosition.x,restPosition.y,restPosition.z);
	}
	else
	{
		glTranslatef(position.x,position.y,position.z-1);
	}
	
	
	
	
//	if (firstBone) 
//	{
//		printf("bonerot: [%f,%f,%f]\n",restDirection.x,restDirection.y,restDirection.z);
////		glRotatef(-90,0,1,0);
//
//		glRotatef(restDirection.z,0,0,1);
//		glRotatef(restDirection.y,0,1,0);
//		glRotatef(restDirection.x,1,0,0);		
//	}
//	else
	{
		glRotatef(rotation.z,0,0,1);
		glRotatef(rotation.y,0,1,0);
		glRotatef(rotation.x,1,0,0);
	}

	
//	printf("bonerest: [%f,%f,%f]\n",restPosition.x,restPosition.y,restPosition.z);
//	printf("bonepos: [%f,%f,%f]\n",objController.position.x,objController.position.y,objController.position.z);
	glLineWidth(1);

	glBegin(GL_LINES);

	glColor3f(31.0f/255.0f,249.0f/255.0f,1);
	
	float bWidth = restLength/8.0f;
	float bDiv = restLength/3.0f;

	// start point to midcut
	glVertex3f(0,0,0);
	glVertex3f(-bWidth,-bWidth,bDiv);

	glVertex3f(0,0,0);
	glVertex3f(bWidth,-bWidth,bDiv);

	glVertex3f(0,0,0);
	glVertex3f(bWidth,bWidth,bDiv);

	glVertex3f(0,0,0);
	glVertex3f(-bWidth,bWidth,bDiv);

	// midcut to endpoint
	glVertex3f(-bWidth,-bWidth,bDiv);
	glVertex3f(0,0,restLength);
	
	glVertex3f(bWidth,-bWidth,bDiv);
	glVertex3f(0,0,restLength);
	
	glVertex3f(bWidth,bWidth,bDiv);
	glVertex3f(0,0,restLength);
	
	glVertex3f(-bWidth,bWidth,bDiv);
	glVertex3f(0,0,restLength);

	glColor3f(1,1,1);

	// midpoint line
	glVertex3f(-bWidth,-bWidth,bDiv);
	glVertex3f(bWidth,-bWidth,bDiv);
	
	glVertex3f(bWidth,-bWidth,bDiv);
	glVertex3f(bWidth,bWidth,bDiv);
	
	glVertex3f(bWidth,bWidth,bDiv);
	glVertex3f(-bWidth,bWidth,bDiv);

	glVertex3f(-bWidth,bWidth,bDiv);
	glVertex3f(-bWidth,-bWidth,bDiv);

	// single line for bone
			   
//	glColor3f(0.0,1,0.0);
//
//	glVertex3f(0,0,0);
//	glColor3f(1.0,0.0,0);
//
//	glVertex3f(0,0,restLength);
	glEnd();
	
	glPointSize(8);
	
	glBegin(GL_POINTS);

	glColor3f(1,1,1);

	glVertex3f(0,0,0);	
	glVertex3f(0,0,restLength);
	
	glEnd();

	glTranslatef(0,0,restLength);
	

	for (unsigned int i = 0; i < children.size(); i++)			
	{
		children[i]->debugDraw(false);
	}
	glPopMatrix();

}		
#endif


void Bone::control(int controllerId, int motionId, float value)
{
	switch (controllerId)
	{
		case CONTROLLER_POSITION:
			switch (motionId)
			{
				case MOTION_X: position.x = value; break;
				case MOTION_Y: position.y = value; break;
				case MOTION_Z: position.z = value; break;
			}
			clearTransform();
		break;
		
		case CONTROLLER_ROTATION:
			switch (motionId)
			{
				case MOTION_X: rotation.x = value; break;
				case MOTION_Y: rotation.y = value; break;
				case MOTION_Z: rotation.z = value; break;
			}
			clearTransform();
		break;
		
		case CONTROLLER_SCALE:
			switch (motionId)
			{
				case MOTION_X: scale.x = value; break;
				case MOTION_Y: scale.y = value; break;
				case MOTION_Z: scale.z = value; break;
			}
			clearTransform();
		break;
	}
};


float Bone::read_control(int controllerId, int motionId)
{
	switch (controllerId)
	{
		case CONTROLLER_POSITION:
			switch (motionId)
			{
				case MOTION_X: return position.x;
				case MOTION_Y: return position.y;
				case MOTION_Z: return position.z;
			}
		break;
		
		case CONTROLLER_ROTATION:
			switch (motionId)
			{
				case MOTION_X: return rotation.x;
				case MOTION_Y: return rotation.y;
				case MOTION_Z: return rotation.z;
			}
		break;
		
		case CONTROLLER_SCALE:
			switch (motionId)
			{
				case MOTION_X: return scale.x;
				case MOTION_Y: return scale.y;
				case MOTION_Z: return scale.z;
			}
		break;
	}
	
	return 0;
};



void Bone::bindChild(Bone &boneObj_in)
{
	children.push_back(&boneObj_in);
	boneObj_in.parent = this;
};


void Bone::bindParent(Bone &boneObj_in)
{
	boneObj_in.bindChild(*this);
};


bool Bone::hasParent()
{
	return (parent != NULL);
};


//void Bone::transformBegin()
//{	
//	std::vector<Bone *> objStack;
//	
//	Bone *tempSceneObj = this;
//	
//	while (tempSceneObj)
//	{
//		objStack.push_back(tempSceneObj);
//		tempSceneObj = tempSceneObj->parent;
//	}
//	
//	for (int i = objStack.size()-1; i >= 0; i--)
//	{
//		objStack[i]->controller().transformBegin();
//	}		
//};


//void Bone::transformEnd()
//{	
//	std::vector<Bone *> objStack;
//	
//	Bone *tempSceneObj = this;
//	
//	while (tempSceneObj)
//	{
//		objStack.push_back(tempSceneObj);
//		tempSceneObj = tempSceneObj->parent;
//	}
//	
//	for (int i = objStack.size()-1; i >= 0; i--)
//	{
//		objStack[i]->controller().transformEnd();
//	}		
//};


//void Bone::transformReverseBegin()
//{	
//	std::vector<Bone *> objStack;
//	
//	Bone *tempSceneObj = this;
//	
//	while (tempSceneObj)
//	{
//		objStack.push_back(tempSceneObj);
//		tempSceneObj = tempSceneObj->parent;
//	}
//	
//	for (int i = objStack.size()-1; i >= 0; i--)
//	{
//		objStack[i]->controller().transformReverseBegin();
//	}		
//};
//
//
//void Bone::transformReverseEnd()
//{	
//	std::vector<Bone *> objStack;
//	
//	Bone *tempSceneObj = this;
//	
//	while (tempSceneObj)
//	{
//		objStack.push_back(tempSceneObj);
//		tempSceneObj = tempSceneObj->parent;
//	}
//	
//	for (int i = objStack.size()-1; i >= 0; i--)
//	{
//		objStack[i]->controller().transformReverseEnd();
//	}		
//};

//
//ObjectController &Bone::controller()
//{
//	return objController;
//};

