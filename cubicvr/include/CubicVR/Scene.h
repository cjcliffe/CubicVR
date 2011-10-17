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

#ifndef CUBICVR_SCENE_H
#define CUBICVR_SCENE_H

#include <CubicVR/cvr_defines.h>

#include <CubicVR/GLExt.h>

#include <CubicVR/Mesh.h>
#include <CubicVR/ObjectController.h>
#include <CubicVR/Light.h>
#include <CubicVR/AreaLight.h>
#include <CubicVR/Camera.h>
// #include "Action!.h"
#include <CubicVR/SceneObject.h>
#include <CubicVR/map_string.h>
#include <CubicVR/Renderer.h>
#include <CubicVR/SectorMap.h>
#include <CubicVR/FRUSTUM.h>
#include <CubicVR/ProceduralObject.h>

#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)

#include <CubicVR/HDR.h>

typedef struct _PostProcessInfo
{
	bool dof;
	bool msaa;
	bool ssao;
	bool hdr;
} PostProcessInfo;

#endif

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
#include <CubicVR/Pencil.h>
#endif


#include <vector>
#include <deque>

#define SCENE_NAMED_ITERATOR std::map<std::string, unsigned int, string_less>::iterator
#define SCENE_NAMED std::map<std::string, unsigned int, string_less>

typedef struct _SceneDebugInfo
{
	bool frustum;
	bool lights;
	bool aabb;
	bool details;
	bool wireframe;
} SceneDebugInfo;

typedef std::set<SceneObject *> SceneObjectSet;
typedef std::map<std::string, SceneObject*, string_less> SceneObjectNamedSet;
typedef std::map<float,SceneObject *,ltufloat> SceneRayResult;

class IMPEXP Scene: public Resource
{
protected:

	SceneObjectSet sceneobjs_bound;
	std::set<Camera *> cams_bound;
	std::set<Light *> lights_bound;
	std::set<Motion *> motions_bound;

	std::set<SceneObject *> sceneobjs_visibility;
	
	SceneObjectNamedSet sceneobjs_bound_ref;
	std::map<std::string, Camera*, string_less> cams_bound_ref;
	std::map<std::string, Light*, string_less> lights_bound_ref;
	std::map<std::string, Motion*, string_less> motions_bound_ref;

	int screen_w, screen_h;
	bool initialized;

	SectorMap *sectorMap;
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	
	bool hdr_enabled, hdr_buffer_open, deferred_shading;
	bool render_buffer_open;
	ProceduralObject *deferred_sphere;
	GLShader postProcessCopy;
	GLShader postProcessDOF;
	GLShader postProcessMSAA;
	GLShader postProcessSSAO;
	GLShader postProcessHDR;
	GLShader postProcessBlur;
	Material *deferredSphereMat;
	PostProcessInfo postProcessConfig;

#define HALFBUFFER_DIV 6
	
#ifndef STRINGIFY
#define STRINGIFY(A)  #A
#endif
	
#endif
	
public:

	FRUSTUM frustum;

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
#if !defined(ARCH_DC)
	FRUSTUM spot_frustum;
	HDR hdrProcess;
#endif

	Light dummyLight;
	set<Light *> dummySet;
#endif
	
	SceneDebugInfo debug;
	
	Camera *cam;
	std::set<SceneObject *> active_sceneobjs;
	std::set<SceneObject *> shadow_sceneobjs;
	std::set<Light *> active_lights;

	set<Light *>::iterator active_light_i;

	MeshRenderer oShader;
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
#if !defined(ARCH_DC)
	ShadowRenderer sShader;
#endif
	LineRenderer lShader;
#endif

//	bool frustum_debug;
	
	Scene();
	Scene(int scr_width, int scr_height);
	~Scene();
	
	void initSectorMap(const XYZ &aabb1, const XYZ &aabb2, double sector_size = 10.0);

	void clear();
	void setRenderDimensions(int scr_width, int scr_height);
	int getWidth();
	int getHeight();
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	void setHDR(bool hdr_mode, int filter_levels=4);
	void setBlur(float amt);
	void openRenderBuffer();
	void closeRenderBuffer();
	void clearRenderBuffer();
	inline PostProcessInfo &getPostProcessInfo() { return postProcessConfig; };

	float blurAmt;
	Texture *renderBuffer;
	Texture *renderBufferTarget[2];
	Texture *halfRenderBufferTarget[2];
	Texture *blendBuffer;
	
	void setDeferredShading(bool deferred_mode);
	bool renderShadowVolumes();
	void renderPostProcess();
	void renderDeferredLighting();
	
#endif	

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
	void renderDebug();
#endif	

	inline void clearLights() { active_lights.clear(); };
	
	void bind(SceneObject &sceneObj_in);	
	void bind(Light &light_in);
	void bind(Camera &cam_in);	
	void bind(Motion &motion_in);	

	void unbind(SceneObject &sceneObj_in);	
	void unbind(Light &light_in);
	void unbind(Camera &cam_in);	
	void unbind(Motion &motion_in);	
	
	void render();
	virtual void firstInit() { };
	virtual void process();
	virtual void renderStage(Renderer &renderer, int stage);

	/* managed objects return by name */
	virtual SceneObject &sceneObject(const std::string name_in);
	virtual Camera &camera(const std::string name_in);
	virtual Light &light(const std::string name_in);
	virtual Motion &motion(const std::string name_in);
	
	SceneObjectSet &sceneObjects();
	SceneObjectNamedSet &sceneObjectsNamed();
	
	string nextSceneObjectName(string baseName);
	
	// convenience functions for triangulating and caching all objects in scene
//	virtual void setCollisionMap(float nominal_rad = CMAP_SPHERE_NOMINAL);
//	virtual void triangulate();
//	virtual void cache(bool cache_state = true);
	void sectorDebugDraw();
	
#ifndef ARCH_DC
#ifdef ARCH_PSP
	void saveScreenshot(char *path="", void *fb=NULL, int seq=-1);
#else
	void saveScreenshot(char *path="", int seq=-1);
#endif	
#endif
	void bbRayTest(XYZ pos, Vector ray, SceneRayResult &selList, int axisMatch=2);
};


#endif
