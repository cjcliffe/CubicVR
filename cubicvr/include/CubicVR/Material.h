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

#ifndef CUBICVR_MATERIAL
#define CUBICVR_MATERIAL

#include <vector>
#include <map>
#include <string>

#ifdef ARCH_PSP
#include <fastmath.h>
#else
#include <math.h>
#ifdef OPENGL_ES
#warning: opengl_es: gl shaders disabled
#else
#ifndef ARCH_DC
#include <CubicVR/GLShader.h>
#endif
#endif
#endif

#include <CubicVR/Texture.h>
#include <CubicVR/Resource.h>
#include <CubicVR/RGBA.h>
#include <CubicVR/map_string.h>

#define MATERIAL_MAP_DISABLE 0
#define MATERIAL_MAP_SPHERICAL 1

#define MATERIAL_FACE_FRONT	0
#define MATERIAL_FACE_BACK	1
#define MATERIAL_FACE_BOTH	2

#define MATERIAL_BLEND_NORMAL 		0
#define MATERIAL_BLEND_SUBTRACTIVE 	1
#define MATERIAL_BLEND_DIFFERENCE 	2
#define MATERIAL_BLEND_MULTIPLY 	3
#define MATERIAL_BLEND_DIVIDE 		4
#define MATERIAL_BLEND_ALPHA 		5
#define MATERIAL_BLEND_DISPLACE 	6
#define MATERIAL_BLEND_ADDITIVE 	7

class IMPEXP MaterialLayer
{
	public:
		unsigned short map_mode;
		unsigned short blend_mode;

		GLuint texture_ref;
		unsigned short tex_type;
		
		MaterialLayer() : map_mode(MATERIAL_MAP_DISABLE), blend_mode(MATERIAL_BLEND_NORMAL), texture_ref(0) {};
		~MaterialLayer()  {};
		
		void setBlending(unsigned short blend_mode_in) { blend_mode = blend_mode_in; 	} ;
		unsigned short getBlending()	{ return blend_mode;	};
		
};


class Material: public Resource
{
	private:
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
		GLShader *program;
		static short tex_bindref[32];
		static short tex_inc;
#endif
#endif
#endif
	public:
	
		static Material nullMaterial;
	
		RGBA ambient, diffuse, specular, color;
		unsigned long shader_mask;
		float reflectivity;
		bool color_mask;
		bool alpha_mask;

		float shininess;
		unsigned short sidedness;
		float max_smooth;	// maximum smoothing angle for this material
		bool cached;
		unsigned int dList;

		// texture_ref, keeps track of a global texture ref belonging to this material, to be replaced with texture layers
		std::vector<MaterialLayer> layer;
			
		Material();
		~Material();
									
								
		/// set up the textures for this material
#ifdef ARCH_PSP
		void texSetup(unsigned int layerNum);
#else
		void texSetup(unsigned int layerType=0);		
#endif
		/// set up the surface parameters for this material
		void surfaceSetup();
		/// get the number of layers used
		unsigned int getLayerSize();
		
		/// use this material, called before a draw to setup this material
#ifdef ARCH_PSP
		void use(unsigned int layerNum=0);
#else
		void use();		
#endif
		/// bind texture to a specific layer of this material
		void bindTexture(unsigned short texLayer, cvrIndex texRefId, short tex_type);
		/// bind texture to last layer
		void bindTexture(cvrIndex texRefId);

		/// add a texture layer (specific)
		void addLayer(unsigned short texLayer);
		/// add a texture layer
		unsigned short addLayer();
		
		/// set ambient color
		void setAmbient(const RGBA &ambient_in);
		/// set diffuse color
		void setDiffuse(const RGBA &diffuse_in);
		/// set specular color
		void setSpecular(const RGBA &specular_in);
		/// set color
		void setColor(const RGBA &color_in);
		/// set shininess, 0.0 .. 128.0
		void setShininess(float shininess_in);		
		/// set all the above values at once
		void set(const RGBA &ambient_in, const RGBA &diffuse_in, const RGBA &specular_in, const RGBA &color_in, float shininess_in);
		///	set whether to show front, back or both sides of faces with this material
		void setSidedness(unsigned short sidedness_in);
		/// set the maximum smoothing angle for the calculation of normals
		void setMaxSmooth(float max_smooth_in);
		/// has a transparency mask
		inline bool hasMask()
		{
			return (color_mask || color.a != 1.0);			
		}
		inline void setupAlpha()
		{
			if (color.a < 1.0)
			{
#ifdef ARCH_PSP
				sceGuEnable( GU_BLEND );
				sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
				sceGuDepthMask(false);
#else
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDepthMask(false);
#endif
			}
			else if (alpha_mask)
			{
#ifdef ARCH_PSP
				sceGuEnable( GU_ALPHA_TEST );
				sceGuEnable( GU_BLEND );
				sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
				sceGuAlphaFunc(GU_GREATER, 0, 0xff);
				sceGuDepthMask(true);
#else
#if !defined(ARCH_DC)
				glEnable(GL_ALPHA_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glAlphaFunc(GL_GEQUAL,0.15f);
				glDepthMask(true);
#endif
#endif
			}	
			else
			{
#ifdef ARCH_PSP
				sceGuDisable(GU_ALPHA_TEST);
				sceGuDepthMask(true);
#else
#if !defined(ARCH_DC) 
				glDisable(GL_ALPHA_TEST);
				glDepthMask(true);
#endif
#endif
			}
		}
	
		void setReflective(float ref);
		/// set Material as transparent with black color mask
		void colorMask(bool cmask_in);
//		/// hasParallax = needs binormal/tangent for parallax
//		bool hasParallax();
	
#ifndef ARCH_PSP
#ifndef OPENGL_ES
		// pixel shader related functions
		bool loadProgram(const char *vp, const char *fp);
		bool loadProgramFile(const char *vf, const char *ff);
		
		bool hasProgram();
		void useProgram();
		void setupProgram();
		void clearProgram();
#endif
#endif

};



#endif

