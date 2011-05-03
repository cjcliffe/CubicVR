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

#include <CubicVR/Transform.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Transform::Transform()
{
	transMatrix = new Matrix(4,4);

	for (int x = 0; x < NUM_TEMPS; x++)
	{
		Transform::tMatrix1_4[x] = new Matrix(1,4);
		Transform::tMatrix4_4[x] = new Matrix(4,4);
	}

	invalidated = true;
}


Transform::~Transform()
{
	delete transMatrix;

	for (int x = 0; x < NUM_TEMPS; x++)
	{
		delete tMatrix1_4[x];
		delete tMatrix4_4[x];
	}
}


void Transform::pushMatrix()
{
	matrixStack.push_front(new Matrix(4,4));
	matrixStack.front()->loadIdentity();
	invalidate();
}


void Transform::popMatrix()
{
	Matrix *mOut;

	if (!matrixStack.size()) return;
	
	mOut = matrixStack.front();
	
	matrixStack.pop_front();

	delete mOut;
	invalidate();
}


void Transform::trans(float x, float y, float z)
{
	Matrix *mCurrent = matrixStack.front();

	if (!matrixStack.size()) return;
	
	tMatrix4_4[0]->loadIdentity();
	tMatrix4_4[0]->set(3,0,x);
	tMatrix4_4[0]->set(3,1,y);
	tMatrix4_4[0]->set(3,2,z);

	mCurrent->multiply4_4by4_4(*(tMatrix4_4[0]),*(tMatrix4_4[1]));
	mCurrent->set(*tMatrix4_4[1]);
}


void Transform::scale(float x, float y, float z)
{
	Matrix *mCurrent = matrixStack.front();

	if (!matrixStack.size()) return;
	tMatrix4_4[0]->loadIdentity();
	tMatrix4_4[0]->set(0,0,x);
	tMatrix4_4[0]->set(1,1,y);
	tMatrix4_4[0]->set(2,2,z);

	mCurrent->multiply4_4by4_4(*tMatrix4_4[0],*tMatrix4_4[1]);
	mCurrent->set(*tMatrix4_4[1]);
}


void Transform::rotate(float ang, float x, float y, float z)
{
	if (!matrixStack.size()) return;

	Matrix *mCurrent = matrixStack.front();

	#define X_ROT (*tMatrix4_4[0])
	#define Y_ROT (*tMatrix4_4[1])
	#define Z_ROT (*tMatrix4_4[2])
	#define T_ROT (*tMatrix4_4[3])
	
	float sAng,cAng;

	sAng = sinf(ang*(M_PI/180.0f));
	cAng = cosf(ang*(M_PI/180.0f));

	if (x) 
	{
		X_ROT.loadIdentity();
		X_ROT.set(1,1,cAng*x);  X_ROT.set(1,2,sAng*x);	
		X_ROT.set(2,1,-sAng*x);	X_ROT.set(2,2,cAng*x);

		mCurrent->multiply4_4by4_4(X_ROT,T_ROT);
		mCurrent->set(T_ROT);
	}
	if (y)
	{
		Y_ROT.loadIdentity();
		Y_ROT.set(0,0,cAng*y);	Y_ROT.set(0,2,-sAng*y);	
		Y_ROT.set(2,0,sAng*y);	Y_ROT.set(2,2,cAng*y);

		mCurrent->multiply4_4by4_4(Y_ROT,T_ROT);
		mCurrent->set(T_ROT);
	}
	if (z)
	{
		Z_ROT.loadIdentity();
		Z_ROT.set(0,0,cAng*z);	Z_ROT.set(0,1,sAng*z);
		Z_ROT.set(1,0,-sAng*z);	Z_ROT.set(1,1,cAng*z);
	
		mCurrent->multiply4_4by4_4(Z_ROT,T_ROT);
		mCurrent->set(T_ROT);
	}
	
	invalidated = true;
}


void Transform::apply(XYZ &v_in, XYZ &v_out)
{
	if (!matrixStack.size()) return;

	tMatrix1_4[0]->set(0,0,v_in.x);
	tMatrix1_4[0]->set(0,1,v_in.y);
	tMatrix1_4[0]->set(0,2,v_in.z);
	tMatrix1_4[0]->set(0,3,1);
	
	if (invalidated)
	{	/* need to update transformation matrix */
		transMatrix->loadIdentity();

		for (matrixIterator = matrixStack.begin(); matrixIterator < matrixStack.end(); matrixIterator++)
		{
			transMatrix->multiply(**matrixIterator,*tMatrix4_4[0]);
			transMatrix->set(*tMatrix4_4[0]);
		}

		invalidated = false;
	}

	tMatrix1_4[0]->multiply1_4by4_4(*transMatrix,*tMatrix1_4[1]);

	v_out.x = tMatrix1_4[1]->get(0,0);
	v_out.y = tMatrix1_4[1]->get(0,1);
	v_out.z = tMatrix1_4[1]->get(0,2);
}

void Transform::set(float *mat) 
{
	Matrix *mCurrent = matrixStack.front();
	
	mCurrent->set(mat);
};
