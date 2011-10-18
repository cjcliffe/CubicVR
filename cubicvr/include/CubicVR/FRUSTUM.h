//////////////////////////////////////////////////////////////////////////////////////////
//	FRUSTUM.h
//	class declaration for frustum for frustum culling
//	Downloaded from: www.paulsprojects.net
//	Created:	8th August 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <CubicVR/math/Maths.h>
#include <CubicVR/XYZ.h>

//planes of frustum
enum FRUSTUM_PLANES
{
	LEFT_PLANE=0,
	RIGHT_PLANE,
	TOP_PLANE,
	BOTTOM_PLANE,
	NEAR_PLANE,
	FAR_PLANE
};

enum FRUSTUM_CLASSIFICATION
{
	OUTSIDE_FRUSTUM=0,
	IN_FRUSTUM
};

class FRUSTUM
{
public:
	void update();
	void update(MATRIX4X4 projMatrix,MATRIX4X4 viewMatrix);
	bool isPointInside(const VECTOR3D & point);
	bool isBoundingBoxInside(const VECTOR3D * vertices);
	bool isPointInside(const XYZ & point);
	bool isBoundingBoxInside(const XYZ &bb1, const XYZ &bb2);
	
	XYZ unProject(XYZ view_pos,
						   MATRIX4X4 projMatrix, 
						   MATRIX4X4 modelMatrix,
						   const int viewport[4]);
	
	inline PLANE &plane(int plane_id) { return planes[plane_id]; }
protected:
	PLANE planes[6];
	
};

#endif	//FRUSTUM_H