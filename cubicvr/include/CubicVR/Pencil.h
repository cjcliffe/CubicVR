#pragma once
/*
 *  Pencil.h
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


#include <vector>
#include <CubicVR/XYZ.h>
#include <CubicVR/Vector.h>
#include <CubicVR/Envelope.h>
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
#include <CubicVR/GLShader.h>
#endif
#include <CubicVR/Envelope.h>

using namespace std;

class Pencil
{
private:
	static std::vector<XYZ>::iterator point_i;
	
public:
	static void drawCircles(std::vector<XYZ> &pointList, float radius);	
	static void drawLine(std::vector<XYZ> &pointList);	
	static void drawLine(std::vector<XYZ> &pointList, Vector up, Vector right);	
	static void drawCurve(std::vector<XYZ> &pointList, int subdivisions);
	static void drawExtruded(std::vector<XYZ> &pointList);
	static void drawExtrudedCurve(std::vector<XYZ> &pointList, int subdivisions);
	static void drawLine(std::vector<XYZ> &pointList,int subdivions);
	static void drawCircle(const XYZ &pt, float radius, int divisions=10);	
	static void drawAxis(const XYZ &pt, float size);	
	static void drawGrid(const XYZ &center, const XYZ &dimensions, float gridsize);
	static void drawGrid(const XYZ &center, const XYZ &dimensions, float gridsize, Vector &up, Vector &right, int divs=3);
	static void drawFrustum(XYZ &position, float modelview[16], float fov, float near, float far, float aspect);
	static void drawBB(const XYZ &bb1, const XYZ &bb2);
};

