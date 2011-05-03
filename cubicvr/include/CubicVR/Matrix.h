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

#ifndef MATRIX_H
#define MATRIX_H

#include <CubicVR/cvr_defines.h>

#include <string>
#include <stdio.h>
#include <string.h>

/* nothing is checked, so they better be the right dimensions! */
class IMPEXP Matrix
{
private:
	int width, height;
	float *mData;

public:

	Matrix(int h, int w);
	~Matrix();

	void multiply(Matrix &mIn, Matrix &mOut);

	void multiply1_4by4_4(Matrix &mIn, Matrix &mOut);	// optimized
	void multiply4_4by4_4(Matrix &mIn, Matrix &mOut);
	
	void multiply(float val);
	void add(Matrix &mIn);
	void add(Matrix &mIn, int k);	/* add with multiple k of mIn */

	void set(Matrix &mIn);
	void set(float *mat);
	float *get() { return mData; };

	void loadIdentity(void);
	

	void set(int i, int j, float val)	{	*(mData+i*width+j) = val;	}
	float get(int i, int j)				{	return *(mData+i*width+j);	}

	int WIDTH()	{	return width;	}
	int HEIGHT() {	return height;	}
};






#endif

