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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#define NUM_TEMPS 4


#include <CubicVR/cvr_defines.h>

#include <CubicVR/Matrix.h>
#include <CubicVR/XYZ.h>

#include <deque>


class IMPEXP Transform 
{
private:
	std::deque<Matrix*> matrixStack;				// transformation stack
	std::deque<Matrix*>::iterator matrixIterator;	// transformation stack iterator

	Matrix *transMatrix;		// current transformation matrix

	Matrix *tMatrix1_4[NUM_TEMPS];	// temporary matricies (1x4)
	Matrix *tMatrix4_4[NUM_TEMPS];	// temporary matricies (4x4)

	bool invalidated;
	void invalidate(void) { invalidated = true; }

public:
	Transform();
	~Transform();

	void pushMatrix();
	void popMatrix();

	void trans(float x, float y, float z);
	void scale(float x, float y, float z);
	void rotate(float ang, float x, float y, float z);
	void apply(XYZ &v_in, XYZ &v_out);
	void set(float *mat);
};


#endif

