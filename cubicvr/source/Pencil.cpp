/*
 *  Pencil.cpp
 *  Builder
 *
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
 *
 */
#if !defined(ARCH_PSP)
#include <CubicVR/Pencil.h>
#include <CubicVR/cvr_defines.h>

std::vector<XYZ>::iterator Pencil::point_i;

void Pencil::drawCircles(std::vector<XYZ> &pointList, float radius)
{
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		drawCircle((*point_i),radius);
	}
}


void Pencil::drawLine(std::vector<XYZ> &pointList)
{
	glBegin(GL_LINE_STRIP);
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		glVertex3f((*point_i).x,(*point_i).y,(*point_i).z);
	}
	
	glEnd();
}

void Pencil::drawLine(std::vector<XYZ> &pointList, Vector up, Vector right)
{
	glBegin(GL_LINE_STRIP);
	
	XYZ pos;
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		pos = right * (*point_i).x + up * (*point_i).y;
		
		glVertex3f(pos.x,pos.y,pos.z);
	}
	
	glEnd();
}


void Pencil::drawCurve(std::vector<XYZ> &pointList, int subdivisions)
{
	Envelope env_x,env_y;
	float step_rate;
	
	step_rate = 1.0f/(float)subdivisions;
	
	int i = 0;
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		env_x.addKey(i,(*point_i).x);
		env_y.addKey(i,(*point_i).y);
		i++;
	}
	
	float j;
	
	glBegin(GL_LINE_STRIP);
	
	for (j = 0; j < i; j+=step_rate)
	{
		glVertex3f(env_x.evaluate(j),env_y.evaluate(j),0);
	}
	
	glEnd();
	
}


void Pencil::drawExtruded(std::vector<XYZ> &pointList)
{
	glBegin(GL_LINE_STRIP);
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		glVertex3f((*point_i).x,(*point_i).y,-3);
	}
	
	glEnd();
	
	glBegin(GL_LINE_STRIP);
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		glVertex3f((*point_i).x,(*point_i).y,3);
	}
	
	glEnd();		
	
	glBegin(GL_LINES);
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		glVertex3f((*point_i).x,(*point_i).y,-3);
		glVertex3f((*point_i).x,(*point_i).y,3);
	}
	
	glEnd();		
	
}


void Pencil::drawExtrudedCurve(std::vector<XYZ> &pointList, int subdivisions)
{
	Envelope env_x,env_y;
	float step_rate;
	
	step_rate = 1.0f/(float)subdivisions;
	
	int i = 0;
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		env_x.addKey(i,(*point_i).x);
		env_y.addKey(i,(*point_i).y);
		i++;
	}
	
	float j;
	
	glBegin(GL_LINE_STRIP);
	
	for (j = 0; j < i; j+=step_rate)
	{
		glVertex3f(env_x.evaluate(j),env_y.evaluate(j),-3);
	}
	
	glEnd();
	
	glBegin(GL_LINE_STRIP);
	
	for (j = 0; j < i; j+=step_rate)
	{
		glVertex3f(env_x.evaluate(j),env_y.evaluate(j),3);
	}
	
	glEnd();		
	
	glBegin(GL_LINES);
	
	for (j = 0; j < i; j+=step_rate)
	{
		glVertex3f(env_x.evaluate(j),env_y.evaluate(j),-3);
		glVertex3f(env_x.evaluate(j),env_y.evaluate(j),3);
	}
	
	glEnd();		
	
}

void Pencil::drawLine(std::vector<XYZ> &pointList,int subdivions)
{
	glBegin(GL_LINE_STRIP);
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		XYZ pt = (*point_i);
		
		glVertex3f(pt.x,pt.y,pt.z);
	}
	
	glEnd();
}


void Pencil::drawCircle(const XYZ &pt, float radius, int segments)
{
	float i;
	
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= (M_PI*2.0)+(M_PI/10.0); i+=M_PI/((float)segments))
	{
		glVertex3f(pt.x+(radius*cos(i)),pt.y+(radius*sin(i)),pt.z);
	}
	glEnd();
}

void Pencil::drawAxis(const XYZ &pt, float size)
{
//	float i;
	
	glBegin(GL_LINES);
	glColor3f(1,0,0);
	glVertex3f(pt.x-size,pt.y,pt.z);
	glVertex3f(pt.x+size,pt.y,pt.z);
	glColor3f(0,1,0);
	glVertex3f(pt.x,pt.y-size,pt.z);
	glVertex3f(pt.x,pt.y+size,pt.z);
	glColor3f(0,0,1);
	glVertex3f(pt.x,pt.y,pt.z-size);
	glVertex3f(pt.x,pt.y,pt.z+size);
	glEnd();
}

void Pencil::drawGrid(const XYZ &center, const XYZ &dimensions, float gridsize)
{
	Vector up, right;
	
	float modelview[16];
	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	
	right.x = modelview[0];
	right.y = modelview[4];
	right.z = modelview[8];
	
	up.x = modelview[1];
	up.y = modelview[5];
	up.z = modelview[9];
	
	up.makeUnit();
	right.makeUnit();
	
	drawGrid(center, dimensions, gridsize, up, right);
}


void Pencil::drawGrid(const XYZ &center, const XYZ &dimensions, float gridsize, Vector &up, Vector &right, int divs)
{
	float i,j;
	
	glBegin(GL_LINES);
	
	XYZ ofs;
	XYZ ctr = center;
	
	float xwidth = ((long)(dimensions.x/gridsize)+1)*gridsize;
	float ywidth = ((long)(dimensions.y/gridsize)+1)*gridsize;
	
	ctr.x = ((long)(ctr.x/gridsize))*gridsize;
	ctr.y = ((long)(ctr.y/gridsize))*gridsize;
	ctr.z = ((long)(ctr.z/gridsize))*gridsize;
	
	ofs = ctr;
	ofs -= up * (ywidth/2.0f);
	ofs -= right * (xwidth/2.0f);
	
	XYZ start_pos, end_pos;
	
	float div_amt = 1.0f;
	float div_clr = 0.7f;
	
	for (j = 0; j < divs; j++)
	{
		div_amt /= 2.0f;
	}
	
	for (j = 0; j < divs; j++)
	{
		glColor3f(div_clr,div_clr,div_clr);
		//			div_clr *= 0.2;
		
		for (i = 0; i <= xwidth; i+= gridsize*div_amt)
		{
			start_pos = ofs + (right * i);
			end_pos = ofs + (right * i) + (up * ywidth);
			
			glVertex3f(start_pos.x,start_pos.y,start_pos.z);
			glVertex3f(end_pos.x,end_pos.y,end_pos.z);
		}
		
		
		for (i = 0; i <= ywidth; i+= gridsize*div_amt)
		{
			start_pos = ofs + (up * i);
			end_pos = ofs + (up * i) + (right * xwidth);
			
			glVertex3f(start_pos.x,start_pos.y,start_pos.z);
			glVertex3f(end_pos.x,end_pos.y,end_pos.z);
		}
		
		div_amt *= 2.0f;
	}
	
	glEnd();
	
}


void Pencil::drawFrustum(XYZ &position, float modelview[16], float fov, float znear, float zfar, float aspect)
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
	Vector forward,nearpt,farpt,up,right;
	
	right.x = modelview[0];
	right.y = modelview[4];
	right.z = modelview[8];
	
	up.x = modelview[1];
	up.y = modelview[5];
	up.z = modelview[9];
	
	forward = up;
	forward *= right;
	forward.makeUnit();
	
	nearpt = forward;
	nearpt *= znear;
	
	farpt = forward;
	farpt *= zfar;
	
	float wNear,hNear,wFar,hFar;
	
	hNear = 2.0f * tan(DEGTORAD(fov) / 2.0f) * znear;
	wNear = hNear * aspect;
	hFar = 2.0f * tan(DEGTORAD(fov) / 2.0f) * zfar;
	wFar = hFar * aspect;
	
	
	Vector far_right = right * (wFar/2.0f);
	Vector far_up = up * (hFar/2.0f);
	
	Vector near_right = right * (wNear/2.0f);
	Vector near_up = up * (hNear/2.0f);
	
	Vector far_top_left = far_up - far_right + farpt;
	Vector far_top_right = far_up + far_right + farpt;
	Vector far_bottom_left = Vector(0,0,0) - far_up - far_right + farpt;
	Vector far_bottom_right = Vector(0,0,0) - far_up + far_right + farpt;
	
	Vector near_top_left = near_up - near_right + nearpt;
	Vector near_top_right = near_up + near_right + nearpt;
	Vector near_bottom_left = Vector(0,0,0) - near_up - near_right + nearpt;
	Vector near_bottom_right = Vector(0,0,0) - near_up + near_right + nearpt;		
	
	far_top_left += position;
	far_top_right += position;
	far_bottom_left += position;
	far_bottom_right += position;
	
	near_top_left += position;
	near_top_right += position;
	near_bottom_left += position;
	near_bottom_right += position;
	
	
	XYZ aabb1, aabb2;
	XYZ f_bounds[8];
	
	f_bounds[0] = far_top_left;
	f_bounds[1] = far_top_right;
	f_bounds[2] = far_bottom_left;
	f_bounds[3] = far_bottom_right;
	f_bounds[4] = near_top_left;
	f_bounds[5] = near_top_right;
	f_bounds[6] = near_bottom_left;
	f_bounds[7] = near_bottom_right;
	
	aabb1 = f_bounds[0];
	aabb2 = f_bounds[0];
	
	for (int i = 1; i < 8; i++)
	{
		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
		
		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
	}
	
#ifndef ARCH_DC	
	GLShader::clear();
#endif
	glDisable(GL_LIGHTING);
	
//	drawBB(aabb1,aabb2);
	
	glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,1.0f);
	//				glVertex3f(cam->position.x+nearpt.x,cam->position.y+nearpt.y,cam->position.z+nearpt.z);
	//				glVertex3f(cam->position.x+farpt.x,cam->position.y+farpt.y,cam->position.z+farpt.z);
	//		glVertex3f(cam->position.x+nearpt.x,cam->position.y+nearpt.y,cam->position.z+nearpt.z);
	//		glVertex3f(cam->position.x+farpt.x,cam->position.y+farpt.y,cam->position.z+farpt.z);
	
	farpt += position;
	far_right += position;
	far_up += position;
	
	nearpt += position;
	near_right += position;
	near_up += position;
	
	
	glColor3f(0.0f,0.0f,1.0f);
	
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
	
	glEnd();
	
	
	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	
	glEnd();
	
	
	glColor3f(0.0f,1.0f,0.0f);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glEnd();
#endif
#endif
}	



void Pencil::drawBB(const XYZ &bb1, const XYZ &bb2)
{
	XYZ bounds;
	
	bounds = bb2;
	bounds -= bb1;
	
	glBegin(GL_LINES);
	glVertex3f(bb1.x,bb1.y,bb1.z);
	glVertex3f(bb1.x,bb1.y,bb1.z+bounds.z);
	
	glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z);
	glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z+bounds.z);
	
	glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z);
	glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z+bounds.z);
	
	glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z);
	glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z+bounds.z);
	
	
	
	glVertex3f(bb1.x,bb1.y,bb1.z);
	glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z);
	
	glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z);
	glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z);
	
	glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z+bounds.z);
	glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z+bounds.z);
	
	glVertex3f(bb1.x,bb1.y,bb1.z+bounds.z);
	glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z+bounds.z);
	
	
	
	glVertex3f(bb1.x,bb1.y,bb1.z);
	glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z);
	
	glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z);
	glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z);
	
	glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z+bounds.z);
	glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z+bounds.z);
	
	glVertex3f(bb1.x,bb1.y,bb1.z+bounds.z);
	glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z+bounds.z);
	
	glEnd();
}
#endif
