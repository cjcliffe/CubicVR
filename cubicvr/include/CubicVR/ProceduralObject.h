#pragma once
/*
 *  ProceduralObject.h
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


#include <CubicVR/Mesh.h>
#include <CubicVR/Vector.h>
#include <CubicVR/UVMapper.h>
#include <CubicVR/Shader.h>
#include <CubicVR/Curve.h>

#define PROCEDURAL_LATHE 10
#define PROCEDURAL_EXTRUDE 10

class ProceduralObject : public Mesh
{
private:
	bool generated;
	bool needs_regen;
	Material *mat;
	
	std::vector<XYZ> lastPoints;
	unsigned int lastDivisions;
	
public:
	ProceduralObject();	

	void regenerate();

	Material &material();
	
	void generateSphere(float radius, unsigned int hor_div, unsigned int vert_div, Material *materialId, bool dynamicUpdate=false);
	void generateLathe(std::vector<XYZ> &pointList, unsigned int lathe_divisions, Material *materialId, bool dynamicUpdate=false);
	void generateExtruded(Curve &curve_in);
	
	void staticRotate(const XYZ &rot);
};


