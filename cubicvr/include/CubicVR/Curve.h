/*
 *  Curve.h
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

#pragma once

#include <vector>
#include <CubicVR/XYZ.h>
#include <CubicVR/Envelope.h>
#include <CubicVR/Vector.h>
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)
#include <CubicVR/Pencil.h>
#endif


// Curve types

#define CURVE_TCB   0
#define CURVE_HERM  1
#define CURVE_BEZI  2
#define CURVE_LINE  3
#define CURVE_STEP  4
#define CURVE_BEZ2  5


class CurveNode
{
public:	
	int shape;
	float tension;
	float continuity;
	float bias;
	float param[4];
	
	CurveNode();
	CurveNode &operator=(const CurveNode &node_in);
};


class Curve
{
private:
	std::vector<XYZ> pointList;
	std::vector<CurveNode> tcbList;
	std::vector<XYZ>::iterator point_i;
	
	std::vector<XYZ> pointListGen;
	std::vector<XYZ>::iterator pointGen_i;

	bool needs_regen;
	int divisions;
	float threshold;
	
	void regenerate();
	
public:
	Curve();
	
	int numPoints();
	int addPoint(const XYZ &pt);	
	bool empty();
	void clear();
	
	XYZ &getPoint(unsigned int ptNum);
	void setPoint(unsigned int ptNum, const XYZ &pt);
	void deletePoint(unsigned int ptNum);
	
	CurveNode &getNode(unsigned int ptNum);
	void setNode(unsigned int ptNum, CurveNode &node_in);
	
	void setDivisions(unsigned int numDivisions);
	void getCurve(std::vector<XYZ>&curve_out);
	
	void draw();
	void drawExtruded();
	void drawLathe(unsigned int lathe_divisions);
	void setAdaptive(float angle_threshold);
	
	
	XYZ &getCurvePoint(unsigned int ptNum);
	int addCurvePoint(unsigned int ptNum);
	
	int closestPointTo(XYZ &pt);	
	int closestCurvePointTo(XYZ &pt);	
	
//	Curve &operator=(const Curve &vect);
};

