//////////////////////////////////////////////////////////////////////////////////////////
//	FRUSTUM.cpp
//	Functions for frustum
//	Downloaded from: www.paulsprojects.net
//	Created:	8th August 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt
//////////////////////////////////////////////////////////////////////////////////////////	
#include <CubicVR/GLExt.h>
#include <CubicVR/math/Maths.h>
#include <CubicVR/FRUSTUM.h>


void FRUSTUM::update()
{
	MATRIX4X4 projection, view;
	MATRIX4X4 clip;

#ifndef ARCH_PSP
	//get matrices
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	glGetFloatv(GL_MODELVIEW_MATRIX, view);
#else
#warning: need matrix load for psp
#endif
	//Multiply the matrices
	clip=projection*view;

	//calculate planes
	planes[RIGHT_PLANE].normal.x=clip.entries[3]-clip.entries[0];
	planes[RIGHT_PLANE].normal.y=clip.entries[7]-clip.entries[4];
	planes[RIGHT_PLANE].normal.z=clip.entries[11]-clip.entries[8];
	planes[RIGHT_PLANE].intercept=clip.entries[15]-clip.entries[12];

	planes[LEFT_PLANE].normal.x=clip.entries[3]+clip.entries[0];
	planes[LEFT_PLANE].normal.y=clip.entries[7]+clip.entries[4];
	planes[LEFT_PLANE].normal.z=clip.entries[11]+clip.entries[8];
	planes[LEFT_PLANE].intercept=clip.entries[15]+clip.entries[12];

	planes[BOTTOM_PLANE].normal.x=clip.entries[3]+clip.entries[1];
	planes[BOTTOM_PLANE].normal.y=clip.entries[7]+clip.entries[5];
	planes[BOTTOM_PLANE].normal.z=clip.entries[11]+clip.entries[9];
	planes[BOTTOM_PLANE].intercept=clip.entries[15]+clip.entries[13];

	planes[TOP_PLANE].normal.x=clip.entries[3]-clip.entries[1];
	planes[TOP_PLANE].normal.y=clip.entries[7]-clip.entries[5];
	planes[TOP_PLANE].normal.z=clip.entries[11]-clip.entries[9];
	planes[TOP_PLANE].intercept=clip.entries[15]-clip.entries[13];

	planes[FAR_PLANE].normal.x=clip.entries[3]-clip.entries[2];
	planes[FAR_PLANE].normal.y=clip.entries[7]-clip.entries[6];
	planes[FAR_PLANE].normal.z=clip.entries[11]-clip.entries[10];
	planes[FAR_PLANE].intercept=clip.entries[15]-clip.entries[14];

	planes[NEAR_PLANE].normal.x=clip.entries[3]+clip.entries[2];
	planes[NEAR_PLANE].normal.y=clip.entries[7]+clip.entries[6];
	planes[NEAR_PLANE].normal.z=clip.entries[11]+clip.entries[10];
	planes[NEAR_PLANE].intercept=clip.entries[15]+clip.entries[14];

	//normalize planes
	for(int i=0; i<6; ++i)
		planes[i].Normalize();
}


void FRUSTUM::update(MATRIX4X4 projMatrix, MATRIX4X4 viewMatrix)
{
	MATRIX4X4 clip;
	
	//Multiply the matrices
	clip=projMatrix*viewMatrix;
	
	//calculate planes
	planes[RIGHT_PLANE].normal.x=clip.entries[3]-clip.entries[0];
	planes[RIGHT_PLANE].normal.y=clip.entries[7]-clip.entries[4];
	planes[RIGHT_PLANE].normal.z=clip.entries[11]-clip.entries[8];
	planes[RIGHT_PLANE].intercept=clip.entries[15]-clip.entries[12];
	
	planes[LEFT_PLANE].normal.x=clip.entries[3]+clip.entries[0];
	planes[LEFT_PLANE].normal.y=clip.entries[7]+clip.entries[4];
	planes[LEFT_PLANE].normal.z=clip.entries[11]+clip.entries[8];
	planes[LEFT_PLANE].intercept=clip.entries[15]+clip.entries[12];
	
	planes[BOTTOM_PLANE].normal.x=clip.entries[3]+clip.entries[1];
	planes[BOTTOM_PLANE].normal.y=clip.entries[7]+clip.entries[5];
	planes[BOTTOM_PLANE].normal.z=clip.entries[11]+clip.entries[9];
	planes[BOTTOM_PLANE].intercept=clip.entries[15]+clip.entries[13];
	
	planes[TOP_PLANE].normal.x=clip.entries[3]-clip.entries[1];
	planes[TOP_PLANE].normal.y=clip.entries[7]-clip.entries[5];
	planes[TOP_PLANE].normal.z=clip.entries[11]-clip.entries[9];
	planes[TOP_PLANE].intercept=clip.entries[15]-clip.entries[13];
	
	planes[FAR_PLANE].normal.x=clip.entries[3]-clip.entries[2];
	planes[FAR_PLANE].normal.y=clip.entries[7]-clip.entries[6];
	planes[FAR_PLANE].normal.z=clip.entries[11]-clip.entries[10];
	planes[FAR_PLANE].intercept=clip.entries[15]-clip.entries[14];
	
	planes[NEAR_PLANE].normal.x=clip.entries[3]+clip.entries[2];
	planes[NEAR_PLANE].normal.y=clip.entries[7]+clip.entries[6];
	planes[NEAR_PLANE].normal.z=clip.entries[11]+clip.entries[10];
	planes[NEAR_PLANE].intercept=clip.entries[15]+clip.entries[14];
	
	//normalize planes
	for(int i=0; i<6; ++i)
		planes[i].Normalize();
}


//is a point in the frustum?
bool FRUSTUM::isPointInside(const VECTOR3D & point)
{
	for(int i=0; i<6; ++i)
	{
		if(planes[i].ClassifyPoint(point)==POINT_BEHIND_PLANE)
			return false;
	}

	return true;
}

//is a bounding box in the frustum?
bool FRUSTUM::isBoundingBoxInside(const VECTOR3D * vertices)
{
	//loop through planes
	for(int i=0; i<6; ++i)
	{
		//if a point is not behind this plane, try next plane
		if(planes[i].ClassifyPoint(vertices[0])!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(vertices[1])!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(vertices[2])!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(vertices[3])!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(vertices[4])!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(vertices[5])!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(vertices[6])!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(vertices[7])!=POINT_BEHIND_PLANE)
			continue;

		//All vertices of the box are behind this plane
		return false;
	}

	return true;
}


//is a point in the frustum?
bool FRUSTUM::isPointInside(const XYZ & point)
{
	for(int i=0; i<6; ++i)
	{
		if(planes[i].ClassifyPoint(point)==POINT_BEHIND_PLANE)
			return false;
	}
	
	return true;
}

//is a bounding box in the frustum?
bool FRUSTUM::isBoundingBoxInside(const XYZ &bb1, const XYZ &bb2)
{
	//loop through planes
	for(int i=0; i<6; ++i)
	{
		//if a point is not behind this plane, try next plane
		if(planes[i].ClassifyPoint(bb1)!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(bb2)!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(XYZ(bb2.x,bb1.y,bb1.z))!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(XYZ(bb1.x,bb2.y,bb1.z))!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(XYZ(bb1.x,bb1.y,bb2.z))!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(XYZ(bb1.x,bb2.y,bb2.z))!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(XYZ(bb2.x,bb1.y,bb2.z))!=POINT_BEHIND_PLANE)
			continue;
		if(planes[i].ClassifyPoint(XYZ(bb2.x,bb2.y,bb1.z))!=POINT_BEHIND_PLANE)
			continue;
		
		//All vertices of the box are behind this plane
		return false;
	}
	
	return true;
}



/*
 
 gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
 const GLdouble modelMatrix[16], 
 const GLdouble projMatrix[16],
 const GLint viewport[4],
 GLdouble *objx, GLdouble *objy, GLdouble *objz)
 {
 double finalMatrix[16];
 double in[4];
 double out[4];
 
 __gluMultMatricesd(modelMatrix, projMatrix, finalMatrix);
 if (!__gluInvertMatrixd(finalMatrix, finalMatrix)) return(GL_FALSE);
 
 in[0]=winx;
 in[1]=winy;
 in[2]=winz;
 in[3]=1.0;
 
 // Map x and y from window coordinates 
in[0] = (in[0] - viewport[0]) / viewport[2];
in[1] = (in[1] - viewport[1]) / viewport[3];

/// Map to range -1 to 1 
in[0] = in[0] * 2 - 1;
in[1] = in[1] * 2 - 1;
in[2] = in[2] * 2 - 1;

__gluMultMatrixVecd(finalMatrix, in, out);
if (out[3] == 0.0) return(GL_FALSE);
out[0] /= out[3];
out[1] /= out[3];
out[2] /= out[3];
*objx = out[0];
*objy = out[1];
*objz = out[2];
return(GL_TRUE);
}

 
 */

XYZ FRUSTUM::unProject(XYZ view_pos,
			 MATRIX4X4 projMatrix, 
			 MATRIX4X4 modelMatrix,
			 const int viewport[4])
{
	XYZ ret;
	
	VECTOR4D in;
	VECTOR4D out;

	MATRIX4X4 finalMatrix;
	
	//Multiply the matrices
	finalMatrix=projMatrix*modelMatrix;
	finalMatrix.Invert();
	
    in.x = view_pos.x;
    in.y = view_pos.y;
    in.z = view_pos.z;
    in.w = 1.0f;
	
    /* Map x and y from window coordinates */
    in.x = (in.x - viewport[0]) / viewport[2];
    in.y = (in.y - viewport[1]) / viewport[3];
	
    /* Map to range -1 to 1 */
    in.x = in.x * 2.0f - 1.0f;
    in.y = in.y * 2.0f - 1.0f;
    in.z = in.z * 2.0f - 1.0f;

	out = finalMatrix*in;
	
    if (out.w == 0.0) return(XYZ(0,0,0));

    ret.x=out.x / out.w;
	ret.y=out.y / out.w;
	ret.z=out.z / out.w;
	
	return ret;
}
