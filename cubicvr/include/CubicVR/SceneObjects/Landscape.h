/*
    This file is part of CubicVR.

    CubicVR is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    CubicVR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CubicVR; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    
    Source (c) 2003 by Charles J. Cliffe unless otherwise specified
    To contact me, e-mail or MSN to cj@cubicproductions.com or by ICQ#7188044
    http://www.cubicproductions.com
*/

#ifndef LANDSCAPE_H
#define LANDSCAPE_H


#include <CubicVR/cvr_defines.h>

#include <CubicVR/Motion.h>
#include <CubicVR/Shader.h>
#include <CubicVR/ObjectController.h>
#include <CubicVR/UVMapper.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/SceneObjects/RigidSceneObject.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
#include <CubicVR/Pencil.h>
#endif

#include <vector>
#include <CubicVR/lodepng.h>


#define LANDSCAPE_BRUSH_BUMP		0
#define LANDSCAPE_BRUSH_RAMP		1
#define LANDSCAPE_BRUSH_PLATEAU		2


#define LANDSCAPE_FUNC_SIN					0
#define LANDSCAPE_FUNC_WAVES				1
#define LANDSCAPE_FUNC_SIN_QUANTIZED		2
#define LANDSCAPE_FUNC_WAVES_QUANTIZED		3
#define LANDSCAPE_FUNC_FLATTEN				4
#define LANDSCAPE_FUNC_CLAMP				5
#define LANDSCAPE_FUNC_SCALE				6

class IMPEXP Landscape : public RigidSceneObject
{
	btHeightfieldTerrainShape *debugShape;

public:
	cvrFloat size_w, size_h, max_height;
	unsigned int divisions_w,divisions_h;
	Material *matRef;
	float* heightfieldData;
	BITSET segmentMask;

	void quickNormalCalc();
	void quickNormalCalc(cvrIndex i, cvrIndex j);
	cvrFloat getHeightValue(XYZ &pt);
	float orient(SceneObject *obj_in, cvrFloat width, cvrFloat length, cvrFloat heading, cvrFloat center=0);
	void useBrush(int brushType, XYZ &position, float value, float brushSize);
	void function(int functionType, cvrFloat funcSize, cvrFloat funcScale, cvrFloat funcIndex);
	int getFaceAt(XYZ &pt);
	void loadRAWMap(char *raw_map, float lo=0.0f, float hi=0.0f);
	void saveRAWMap(char *raw_map, float lo=0.0f, float hi=0.0f);
	bool loadPNGMap(char *png_map, float lo=0.0f, float hi=0.0f);
	bool savePNGMap(char *png_map, float lo=0.0f, float hi=0.0f);
//	void savePNGMap(char *raw_map);
	
#ifndef ARCH_PSP
#ifndef OPENGL_ES
	void drawDebugHeightField();
#endif
#endif
	virtual void initialize(btDiscreteDynamicsWorld &dynamicsWorld_in);
	void mkHeightField();
	inline void cache() { obj->cache(true,true); };
	
	
	Landscape(cvrFloat size_in, unsigned int divisions_in_w, unsigned int divisions_in_h, cvrFloat max_height_in=1000.0f, Material *matRef_in=NULL, bool cache=true);
	~Landscape();
};


#endif
