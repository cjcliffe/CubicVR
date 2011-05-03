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

#include "CubicVR/RigidSceneDebug.h"
#include <CubicVR/Logger.h>

RigidSceneDebug::RigidSceneDebug()
{

}

void RigidSceneDebug::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	glBegin(GL_LINES);
	glColor3f(color.getX(), color.getY(), color.getZ());
	glVertex3d(from.getX(), from.getY(), from.getZ());
	glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();
}

void RigidSceneDebug::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{
//	const btVector3	n=cross(b-a,c-a).normalized();
	glBegin(GL_TRIANGLES);		
	glColor4f(color.getX(), color.getY(), color.getZ(),alpha);
//	glNormal3d(n.getX(),n.getY(),n.getZ());
	glVertex3d(a.getX(),a.getY(),a.getZ());
	glVertex3d(b.getX(),b.getY(),b.getZ());
	glVertex3d(c.getX(),c.getY(),c.getZ());
	glEnd();
}


void RigidSceneDebug::draw3dText(const btVector3& location,const char* textString)
{
//	glRasterPos3f(location.x(),  location.y(),  location.z());
//	BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void RigidSceneDebug::reportErrorWarning(const char* warningString)
{
	Logger::log(warningString);
}

void RigidSceneDebug::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
	btVector3 to=pointOnB+normalOnB*distance;
	const btVector3&from = pointOnB;
	glBegin(GL_LINES);
	glColor3f(color.getX(), color.getY(), color.getZ());
	glVertex3d(from.getX(), from.getY(), from.getZ());
	glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();


//	glRasterPos3f(from.x(),  from.y(),  from.z());
//	char buf[12];
//	sprintf(buf," %d",lifeTime);
//	BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
}

void RigidSceneDebug::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;

}

