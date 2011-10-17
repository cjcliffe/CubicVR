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

#ifndef CUBICVR_SHADER
#define CUBICVR_SHADER

#include <CubicVR/cvr_defines.h>

#include <CubicVR/GLExt.h>

#include <CubicVR/Light.h>
#include <CubicVR/AreaLight.h>

#include <CubicVR/Mesh.h>

#define SHADER_STAGE_NULL	0
#define SHADER_STAGE_OPAQUE	1
#define SHADER_STAGE_TRANSPARENT 2
#define SHADER_STAGE_NOMATERIAL 3
#define SHADER_STAGE_NOTEXTURE 4
#define SHADER_STAGE_SHADOW 5

class IMPEXP Renderer
{
	public:
		std::set<Light *> *lights;
		float *shadowModelMatrix;
		float shadow_alpha;
		XYZ lod_src;
		bool lod;
		
		Renderer();
		virtual ~Renderer() {};
		virtual void render(Mesh &obj, int stage = SHADER_STAGE_NULL) = 0;

//		void bind(Light &lightObj);
		inline void bind(std::set<Light *> *lights_in) { lights = lights_in; };
		inline void setLODSource(const XYZ &lod_src_in) { lod_src = lod_src_in; };
		inline void clearLOD() { lod = false; };
		void shadowAlpha(float shadow_alpha_in);
		float shadowAlpha();
};


#ifdef ARCH_PSP

#include <pspdebug.h>
#define printf pspDebugScreenPrintf 

//class IMPEXP PSPShader : public Renderer	/* todo: commenting */
//{
//	public:
//		void render(Object &obj, int stage = SHADER_STAGE_NULL);
//};
//
#endif
//#else

#if !defined(OPENGL_ES) && !defined(ARCH_PSP)

class IMPEXP LineRenderer : public Renderer
{
	public:
		void render(Mesh &obj, int stage = SHADER_STAGE_NULL);
};

class IMPEXP VertexColorRenderer : public Renderer
{
	public:
		void render(Mesh &obj, int stage = SHADER_STAGE_NULL);
};

class IMPEXP ShadowRenderer : public Renderer
{
public:
	void render(Mesh &obj, int stage = SHADER_STAGE_NULL);
};

class IMPEXP ImmediateRenderer : public Renderer
{
public:
	void render(Mesh &obj, int stage = SHADER_STAGE_NULL);
};

#endif

class IMPEXP VertexBufferRenderer : public Renderer	/* todo: commenting */
{
	public:
	
#ifndef ARCH_PSP
		static Mesh *lockObj;	// if we draw the same object more than once consecutively this will avoid some context switching
		static void setup(Mesh &obj, bool init_texcoord);
#endif

		void render(Mesh &obj, int stage = SHADER_STAGE_NULL);
};

//#endif



class IMPEXP MeshRenderer : public Renderer	// determines whehter the object is cached or not
{
	private:
		VertexBufferRenderer cShader;
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)
		ImmediateRenderer rShader;
#endif
	
	public:
		void bind(std::set<Light *> *lights_in);
		void render(Mesh &obj, int stage = SHADER_STAGE_NULL);
};



#endif
