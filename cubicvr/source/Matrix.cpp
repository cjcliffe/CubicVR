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

#include <CubicVR/Matrix.h>


Matrix::Matrix(int h, int w)
{
	width = w;
	height = h;

	mData = new float[width*height];
}


Matrix::~Matrix()
{
	delete[] mData;
}


void Matrix::multiply(Matrix &mIn, Matrix &mOut)
{
	float tval;
	int i,j,k;

	int mInW = mIn.WIDTH();

	for (i = 0; i < height; i++)
		for (j = 0; j < mInW; j++)
		{	
			tval = 0;
			for (k = 0; k < width; k++)
			{
				tval += (get(i,k))*(mIn.get(k,j));
			}
			mOut.set(i,j,tval);
		}
}


void Matrix::multiply1_4by4_4(Matrix &mIn, Matrix &mOut)
{
	mOut.mData[0] = 
	mIn.mData[0]*mData[0]+ 
	mIn.mData[4]*mData[1]+ 
	mIn.mData[8]*mData[2]+
	mIn.mData[12]*mData[3];

	mOut.mData[1] = 
	mIn.mData[1]*mData[0]+
	mIn.mData[5]*mData[1]+
	mIn.mData[9]*mData[2]+
	mIn.mData[13]*mData[3];

	mOut.mData[2] = 
	mIn.mData[2]*mData[0]+
	mIn.mData[6]*mData[1]+
	mIn.mData[10]*mData[2]+
	mIn.mData[14]*mData[3];
	
	mOut.mData[3] = 
	mIn.mData[3]*mData[0]+
	mIn.mData[7]*mData[1]+
	mIn.mData[11]*mData[2]+
	mIn.mData[15]*mData[3];
}

void Matrix::multiply4_4by4_4(Matrix &mIn, Matrix &mOut)	// aww yea
{
	mOut.mData[0] = mIn.mData[0]*mData[0]+mIn.mData[4]*mData[1]+mIn.mData[8]*mData[2]+mIn.mData[12]*mData[3];
	mOut.mData[1] = mIn.mData[1]*mData[0]+mIn.mData[5]*mData[1]+mIn.mData[9]*mData[2]+mIn.mData[13]*mData[3];
	mOut.mData[2] = mIn.mData[2]*mData[0]+mIn.mData[6]*mData[1]+mIn.mData[10]*mData[2]+mIn.mData[14]*mData[3];
	mOut.mData[3] = mIn.mData[3]*mData[0]+mIn.mData[7]*mData[1]+mIn.mData[11]*mData[2]+mIn.mData[15]*mData[3];
	mOut.mData[4] = mIn.mData[0]*mData[4]+mIn.mData[4]*mData[5]+ mIn.mData[8]*mData[6]+mIn.mData[12]*mData[7];
	mOut.mData[5] = mIn.mData[1]*mData[4]+mIn.mData[5]*mData[5]+mIn.mData[9]*mData[6]+mIn.mData[13]*mData[7];
	mOut.mData[6] = mIn.mData[2]*mData[4]+mIn.mData[6]*mData[5]+mIn.mData[10]*mData[6]+mIn.mData[14]*mData[7];	
	mOut.mData[7] = mIn.mData[3]*mData[4]+mIn.mData[7]*mData[5]+mIn.mData[11]*mData[6]+mIn.mData[15]*mData[7];
	mOut.mData[8] = mIn.mData[0]*mData[8]+mIn.mData[4]*mData[9]+mIn.mData[8]*mData[10]+mIn.mData[12]*mData[11];
	mOut.mData[9] = mIn.mData[1]*mData[8]+mIn.mData[5]*mData[9]+mIn.mData[9]*mData[10]+mIn.mData[13]*mData[11];
	mOut.mData[10] = mIn.mData[2]*mData[8]+mIn.mData[6]*mData[9]+mIn.mData[10]*mData[10]+mIn.mData[14]*mData[11];
	mOut.mData[11] = mIn.mData[3]*mData[8]+mIn.mData[7]*mData[9]+mIn.mData[11]*mData[10]+mIn.mData[15]*mData[11];
	mOut.mData[12] = mIn.mData[0]*mData[12]+mIn.mData[4]*mData[13]+mIn.mData[8]*mData[14]+mIn.mData[12]*mData[15];
	mOut.mData[13] = mIn.mData[1]*mData[12]+mIn.mData[5]*mData[13]+mIn.mData[9]*mData[14]+mIn.mData[13]*mData[15];
	mOut.mData[14] = mIn.mData[2]*mData[12]+mIn.mData[6]*mData[13]+mIn.mData[10]*mData[14]+mIn.mData[14]*mData[15];
	mOut.mData[15] = mIn.mData[3]*mData[12]+mIn.mData[7]*mData[13]+mIn.mData[11]*mData[14]+mIn.mData[15]*mData[15];
}


void Matrix::multiply(float val)
{
	int i,j;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{	
			set(i,j,get(i,j)*val);
		}
}


void Matrix::add(Matrix &mIn)
{
	int i,j;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{	
			set(i,j,get(i,j)+mIn.get(i,j));
		}
}


void Matrix::add(Matrix &mIn, int k)
{
	int i,j;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{	
			set(i,j,get(i,j)+(k*mIn.get(i,j)));
		}
}


void Matrix::set(Matrix &mIn)
{
	int i,j;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{	
			set(i,j,mIn.get(i,j));
		}
}


void Matrix::set(float *mat)
{
	memcpy(mData,mat,width*height);
}


void Matrix::loadIdentity(void)
{
	set(0,0,1);
	set(0,1,0);
	set(0,2,0);
	set(0,3,0);

	set(1,0,0);
	set(1,1,1);
	set(1,2,0);
	set(1,3,0);

	set(2,0,0);
	set(2,1,0);
	set(2,2,1);
	set(2,3,0);

	set(3,0,0);
	set(3,1,0);
	set(3,2,0);
	set(3,3,1);
}


