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

#ifndef CUBICVR_LIGHT_H
#define CUBICVR_LIGHT_H

#include <CubicVR/cvr_defines.h>

#include <CubicVR/GLExt.h>
#include <CubicVR/Motion.h>
#include <CubicVR/Transform.h>
#include <CubicVR/XYZ.h>
#include <CubicVR/RGB.h>
#include <CubicVR/Vector.h>
#include <CubicVR/Texture.h>
#include <CubicVR/Resource.h>

//#include <CubicVR/ObjectController.h>

class ObjectController;
class SceneObject;

#include <string>

#define LIGHT_POINT 1
#define LIGHT_SPOT 2
#define LIGHT_DIRECTIONAL 3
#define LIGHT_PROJECTOR 4
#define LIGHT_NULL 5
#define LIGHT_AREA 6


#define LIGHT_DEFERRED 9
#define LIGHT_POINT_DEFERRED 10
#define LIGHT_SPOT_DEFERRED 11
#define LIGHT_DIRECTIONAL_DEFERRED 12
#define LIGHT_PROJECTOR_DEFERRED 13
#define LIGHT_NULL_DEFERRED 14
#define LIGHT_AREA_DEFERRED 15



#define MAX_ATTRIBS     256
#define MAX_PFORMATS    256
#define TEX_SIZE        1024


#ifdef ARCH_PSP
#define MAX_LIGHTS		4
#else
#define MAX_LIGHTS		8
#endif


class IMPEXP Light : public Resource, public Controllable
{
protected:
	Transform directionTransform;
	
public:
#ifdef ARCH_PSP
	static GLint gl_lights[MAX_LIGHTS];
#else
//	static GLint gl_lights[MAX_LIGHTS];
	static GLint active_lights[MAX_LIGHTS];
	static GLint active_light_types[MAX_LIGHTS];
	static GLint active_shadow;
	static GLint active_projector;
#endif
	Vector direction;

	static RGB global_ambient;
	
	unsigned int shadowmapTex, coneTex, shadow_mtex, cone_mtex;
	unsigned short type;

	unsigned int shadowBuffer; 

	GLint glLightId;

	XYZ position, rotation, target;
#ifdef ARCH_DC
	XYZ position_transformed;
#endif
	XYZ rotation_last;
	
	RGB ambient,diffuse,specular;
	
	float cutoff, exponent, constant_attenuation, linear_attenuation, quadratic_attenuation;
	float nearclip, farclip;
	float ortho_size;

	int map_res;

	float projectionMatrix[16];
	float viewMatrix[16];
	float transMatrix[16];
	float modelViewMatrix[16];
	int viewport[4];
	

	bool has_target;
	bool has_shadow;
	bool has_projector;
	bool dynamic;

	SceneObject *parent;
	SceneObject *sceneObjTarget;

	Light(bool dynamic_in=true);
	~Light();

	static void setGlobalAmbient(const RGB &global_ambient_rgb);

	void setLightId(GLint glLightId_in);

	virtual void setup(GLint glLightId = -1);

	void enable();
	void disable(GLint glLightId = -1);
	
	void setTargeted(bool has_target_in);

	void control(int controllerId, int motionId, float value);
	void bindParent(SceneObject &parent_in);

	void setType(unsigned short type_in);	
	void setAmbient(const RGB &rgb_in);
	void setDiffuse(const RGB &rgb_in);
	void setSpecular(const RGB &rgb_in);
	void setPosition(const XYZ &xyz_in);
	void setRotation(const XYZ &xyz_in);

	void setTarget(const XYZ &xyz_in);
	void bindTarget(SceneObject *sceneObj);
	void setCutoff(float cutoff_in);	
	void setExponent(float exponent_in);
	void setConstantAttenuation(float constant_attenuation_in);
	void setLinearAttenuation(float linear_attenuation_in);
	void setQuadraticAttenuation(float quadratic_attenuation_in);

	void setClipping(float near_in, float far_in);
	inline void setDynamic(bool dynamic_in) { dynamic=dynamic_in; };
	inline bool isDynamic() { return dynamic; };
	
	void setShadow(unsigned int map_res_in, const std::string cone_tex);
	bool hasShadow();
	void shadowInit();
	void shadowBegin();
	void shadowEnd();
	void setupTexgen(double angle, int unit);
	void shadowRenderStart(int stage, int unit);
	void shadowRenderFinish(int stage, int unit);
};





#endif
