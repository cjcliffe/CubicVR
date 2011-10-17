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

#include <CubicVR/Shader.h>

#ifndef ARCH_PSP
Mesh *CacheShader::lockObj = NULL;
#endif

//void Shader::bind(Light &lightObj)
//{
//	lights.push_back(&lightObj);
//};

#ifndef ARCH_PSP
#ifndef OPENGL_ES

//void Shader::setShadowModelMatrix(float m[16])
//{
//	memcpy(shadowModelMatrix, m, sizeof(float)*16);
//}
										  

void VertexColorShader::render(Mesh &obj, int stage)
{
	glDisable(GL_LIGHTING);

	std::vector<Face * >::iterator i;
	std::vector<XYZ * >::iterator j;
	
	glBegin(GL_TRIANGLES);
	for (i = obj.faces.begin(); i != obj.faces.end(); i++)
	{
		for (j = (*i)->points.begin(); j != (*i)->points.end(); j++)
		{
//			glColor3f((*j)->r,(*j)->g,(*j)->b);
			glVertex3f((*j)->x,(*j)->y,(*j)->z);
		}
	}
	glEnd();
};


/*
void LineShader::render(Object &obj, int stage)
{
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
	
	// mat_reflist_i -- used to iterate through the material mat_reflist of the object
	std::map<cvrIndex, map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint >, ltindex>::iterator obj_matref_i;
	// segment iterator
	map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint >::iterator mat_segment_type_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map
	std::map<cvrIndex, va_ref, ltindex>::iterator matref_type_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map
	std::map<cvrIndex, va_ref, ltindex>::reverse_iterator matref_last_i;
	
	unsigned int element_index = 0;
	//	bool skip_segment = false;
	if (!obj.buffer_state) element_index = (unsigned int)obj.cache_data.cache_element;
		
	// Reset mutitexture state
	Texture::clearAllTextures();
	
	// special lights which have been bound that contribute to texturing 
	unsigned int tex_offset = Texture::tex_use+1;
	
#if !defined(OPENGL_ES) && !defined(ARCH_DC)
	// store attribs
	glPushAttrib(GL_ENABLE_BIT);
#endif
	
	
#if !defined(OPENGL_ES) && !defined(ARCH_DC)	
	if (obj.buffer_state && CacheShader::lockObj != &obj)
	{
		// Bind the vertex buffers from the cache data 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, obj.cache_data.elementBufferNum);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, obj.cache_data.dataBufferNum);
		
		// set the vertex pointer 
		glVertexPointer(3,GL_FLOAT,0,BUFFER_OFFSET(0));
		glEnableClientState(GL_VERTEX_ARRAY);
		
		// set the normal pointer 
		glNormalPointer(GL_FLOAT,0,BUFFER_OFFSET(obj.normalCacheOffset));
		glEnableClientState(GL_NORMAL_ARRAY);
		
		// activate uv map pointers for multitexture layers 
		if (stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE) 
		{
			for (unsigned int m = 0; m < obj.cache_data.max_uvs; m++)
			{
				Texture::setTexture(tex_offset+m);
				glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(obj.uvCacheOffset+(sizeof(va_uv)*obj.cache_data.vertex_count*m)));
			}
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	else 
#endif
		if (CacheShader::lockObj != &obj)
		{
#if !defined(OPENGL_ES) && !defined(ARCH_DC)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
			glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB);
			glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
#endif
			
			if (obj.hasColorMap)
			{
				glColorPointer(3, GL_FLOAT, 0, (const GLvoid *)(obj.cache_data.data+(obj.colorMapOffset)));
			}
			
			glVertexPointer(3, GL_FLOAT, 0,  (const GLvoid *)obj.cache_data.data);
			glEnableClientState(GL_VERTEX_ARRAY);
			
#ifndef ARCH_DC	// DC doesn't process normals
			glNormalPointer(GL_FLOAT, 0,  (const GLvoid *)(obj.cache_data.data+(obj.normalCacheOffset)));
			glEnableClientState(GL_NORMAL_ARRAY);
#endif		
			
			if (stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE) 
			{
				if (stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE) for (unsigned int m = 0; m < obj.cache_data.max_uvs; m++)
				{
					Texture::setTexture(tex_offset+m);
					glTexCoordPointer(2, GL_FLOAT, 0,  (const GLvoid *)(obj.cache_data.data+obj.uvCacheOffset+(sizeof(va_uv)*obj.cache_data.vertex_count*m)));
				}
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			}
			
		}
	
	if (obj.hasColorMap && obj.dynamic_colors)	// Mostly just for Dreamcast lighting at the moment
	{
		glColorPointer(3, GL_FLOAT, 0, (const GLvoid *)obj.dynamic_colors);				
		glEnableClientState(GL_COLOR_ARRAY);	
	}
	
	
	CacheShader::lockObj = &obj;
	
	// iterate through the list of materials with face reference sets 
	for (obj_matref_i = obj.mat_cache_data.begin(); obj_matref_i != obj.mat_cache_data.end(); obj_matref_i++)
	{
		// (*obj_matref_i).first // material reference id
		
		unsigned int segLimit = (*obj_matref_i).second.size();
		unsigned int segIndex = 0;
		unsigned int segRun = 0;
		unsigned int segRangeIndex = 0;
		unsigned int segRangeType = 0;
		unsigned long segElementIndex = 0;
		
		bool fault = false;
		bool usefault = (obj.numSegments != 1);			// assume objects with one segment don't need segment fault tests
		
		//true; //((*mat_segment_type_i).second.size()==1 && (*obj_matref_i).second.size()!=1);
		
		va_ref segRange;
		
		segRange.range_min = obj.cache_data.vertex_count;
		segRange.range_max = 0;
		segRange.element_count = 0;	
		segRangeIndex = element_index;		
		
		for (mat_segment_type_i = (*obj_matref_i).second.begin(); mat_segment_type_i != (*obj_matref_i).second.end(); mat_segment_type_i++)
		{
			segIndex++;
						
			if (obj.numSegments >= 1 && obj.segmentMask)
			{
				if (obj.segmentMask->IsSet((*mat_segment_type_i).first) == false)
				{
					fault = true;
				}
			}
			
			segRangeType = (*(*mat_segment_type_i).second.begin()).first;
			
			if (fault) 
			{
				element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);				
				
				if (!usefault) 
				{
					fault=false;
					continue;
				}
			}
			else
			{				
				segRun++;
				
				segRange.element_count += (*(*mat_segment_type_i).second.begin()).second.group_size;
				
				if ((*(*mat_segment_type_i).second.begin()).second.range_min < segRange.range_min)
				{
					segRange.range_min = (*(*mat_segment_type_i).second.begin()).second.range_min;
				}
				
				if ((*(*mat_segment_type_i).second.begin()).second.range_max > segRange.range_max)
				{
					segRange.range_max = (*(*mat_segment_type_i).second.begin()).second.range_max;
				}				
				
				
				if (usefault) if (segIndex < segLimit) 
				{
					element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
					continue;
				}
				
			}
						
			glDepthMask(true);
			
			if (stage == SHADER_STAGE_TRANSPARENT) 
			{
				glEnable(GL_BLEND);
				glEnable(GL_ALPHA_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glAlphaFunc(GL_GEQUAL, 0.2f);
			}
				
				
				if (!usefault)	// no fault segments needed, just render all primitives
				{				
					segElementIndex = element_index;
					
					for (matref_type_i = (*mat_segment_type_i).second.begin(); matref_type_i != (*mat_segment_type_i).second.end(); matref_type_i++)
					{				
						if (!(*matref_type_i).second.element_count) continue;
						
						switch((*matref_type_i).first)
						{
							case 1: glDrawRangeElements(GL_POINTS,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
								break;
							case 2:	glDrawRangeElements(GL_LINES,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
								break;
							case 3:	
								for (unsigned long i = 0; i < (*matref_type_i).second.element_count-3; i+=3)
									glDrawRangeElements(GL_LINES,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,3,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex+i));
								break;
#ifndef OPENGL_ES
							case 4:	
								for (unsigned long i = 0; i < (*matref_type_i).second.element_count-4; i+=4)
									glDrawRangeElements(GL_POINTS,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,4,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex+i));
								break;
#endif
						}
						segElementIndex += (*matref_type_i).second.element_count*sizeof(cvrElement);
					}
				}
				else	// use accumulated cache fault runs
				{
					switch (segRangeType)
					{
						case 1:
								glDrawRangeElements(GL_POINTS,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
							break;
						case 2:
								glDrawRangeElements(GL_LINES,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
							break;
						case 3:
							for (unsigned int i = 0; i <= segRange.element_count-3; i+=3)
								glDrawRangeElements(GL_LINE_STRIP,segRange.range_min,segRange.range_max,3,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex+i));
							break;
#ifndef OPENGL_ES
						case 4:
							for (unsigned int i = 0; i <= segRange.element_count-4; i+=4)
								glDrawRangeElements(GL_LINE_STRIP,segRange.range_min,segRange.range_max,4,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex+i));
							break;
#endif					
					}
					
				}	

			if (!usefault) element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
			
			if (usefault)	// reset fault line
			{
				segRange.range_min = obj.cache_data.vertex_count;
				segRange.range_max = 0;
				segRange.element_count = 0;	
				
				segRun = 0;
				if (!fault) element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
				fault = false;
				segRangeIndex = element_index;
				
			}
			
		}
		
		
	}
	
	
#if !defined(ARCH_DC) && !defined(OPENGL_ES)	
	// reset attribs
	glPopAttrib();
#endif
};
*/


void LineShader::render(Mesh &obj, int stage)
{
	glDisable(GL_LIGHTING);

	std::vector<Face * >::iterator i;
	std::vector<XYZ * >::iterator j;

	for (i = obj.faces.begin(); i != obj.faces.end(); i++)
	{
	glBegin(GL_LINE_STRIP);
		for (j = (*i)->points.begin(); j != (*i)->points.end(); j++)
		{
//			glColor3f((*j)->r,(*j)->g,(*j)->b);
			glVertex3f((*j)->x,(*j)->y,(*j)->z);
		}
		
		j = (*i)->points.begin();
//		glColor3f((*j)->r,(*j)->g,(*j)->b);
		glVertex3f((*j)->x,(*j)->y,(*j)->z);
	glEnd();
	}
};


#endif
#endif
/*
#ifdef ARCH_PSP

void PSPShader::render(Object &obj, int stage)
{
	// mat_reflist_i -- used to iterate through the material mat_reflist of the object 
	map<Material *, map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint > >::iterator obj_matref_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map 
	map<unsigned short, std::set<cvrIndex>, ltushort>::iterator matref_type_i;
	// may_facelist_i -- used to iterate through the set of faces which belong to the mat_facetypelist_i map 
	set<cvrIndex>::iterator type_face_i;
	// faces_point_i -- used to step through the points of the face which is referenced by mat_facelist_i 
	vector<cvrIndex>::iterator face_point_i;
	
	bool has_trans = false;


	// iterate through the list of materials with face reference sets 
	for (obj_matref_i = obj.mat_reflist.begin(); obj_matref_i != obj.mat_reflist.end(); obj_matref_i++)
	{
		// (*obj_matref_i).first // material reference
		// if it has a transparency mask, textures will be needed for depth calc on shadow
//		bool has_mask = Material::materials[(*obj_matref_i).first].hasMask();	
		Material *mat = (*obj_matref_i).first;
			
		int matLayerCount = mat->getLayerSize();
		
		if (matLayerCount == 0) matLayerCount = 1;

		for (unsigned int matLayer = 0; matLayer < matLayerCount; matLayer++)
		{
			 // if we're in the opaque shader stage then we need to discard the transparent materials and vice versa 
			if (stage == SHADER_STAGE_NULL || stage == SHADER_STAGE_OPAQUE || stage == SHADER_STAGE_NOTEXTURE)
			{
				 // alpha = 1.0, opaque 
				if (!mat->hasMask()) 
				{
					mat->use(matLayer);
				}
				else  // we're in the transparency stage and opaque isn't needed, we have to skip the element count though 
				{
					has_trans = true;
					continue;
				}
			}
		
			 // transparent shader stage 
			if (stage == SHADER_STAGE_TRANSPARENT)
			{
				 // Alpha != 1.0, transparent 
				if (mat->hasMask()) 
				{
					mat->use(matLayer);
				}
				else  // opaque, need transparent, skip it 
				{
					continue;	
				}
			}

			for (matref_type_i = (*(*obj_matref_i).second.begin()).second.begin(); matref_type_i != (*(*obj_matref_i).second.begin()).second.end(); matref_type_i++)
			{
				// (*matref_type_i).first // primitive type
				unsigned int count = (*matref_type_i).first * obj.psp_elemcount[(*obj_matref_i).first][(*matref_type_i).first];
				unsigned int vtype = GU_TEXTURE_32BITF|GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D;
				char *vertexData = obj.psp_reflist[(*obj_matref_i).first][(*matref_type_i).first];
				unsigned int ptype = 0;
			
				switch ((*matref_type_i).first)
				{
					case 1: ptype = GU_POINTS; break;
					case 2: ptype = GU_LINES; break;
					case 3: ptype = GU_TRIANGLES; break;
	//				case 4: ptype = GU_QUADS; break;
				}
			
				sceGumDrawArray( ptype, vtype, count, 0, vertexData);
			}							
		
#ifndef ARCH_PSP
			 // clear the textures used in this material 
			if (stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE)
			{
				GLExt::clearActiveTextures();
			}
#endif
		}
	}
		
	 // call the transparency stage if necessary 
	if (stage == SHADER_STAGE_NULL && has_trans) render(obj,SHADER_STAGE_TRANSPARENT);
};

//#else
*/

#ifdef OPENGL_ES
#define GL_UNSIGNED_INT GL_UNSIGNED_SHORT
#define glDrawRangeElements(a,b,c,d,e,f) glDrawElements(a,d,e,f)
#endif

#ifdef ARCH_DC
#define glDrawRangeElements(a,b,c,d,e,f) glDrawElements(a,d,e,f)
#endif

void CacheShader::render(Mesh &obj, int stage)
{
	#define BUFFER_OFFSET(i) ((char *)NULL + (i))
	
	// mat_reflist_i -- used to iterate through the material mat_reflist of the object
	std::map<Material *, map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint > >::iterator obj_matref_i;
	// segment iterator
	map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint >::iterator mat_segment_type_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map
	std::map<cvrIndex, va_ref, ltindex>::iterator matref_type_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map
	std::map<cvrIndex, va_ref, ltindex>::reverse_iterator matref_last_i;
	
	bool has_trans = false;
	unsigned int element_index = 0;
	unsigned int shadow_count = 0;
	Material *mat;
	set<Light *>::iterator light_i;

	if (!obj.buffer_state) element_index = (unsigned long)obj.cache_data.cache_element;
	
	// Reset mutitexture state
	Texture::clearAllTextures();
	
	// special lights which have been bound that contribute to texturing 
	unsigned int tex_offset = Texture::tex_use+1;

	
//#if !defined(OPENGL_ES) && !defined(ARCH_DC)
	// store attribs
//	glPushAttrib(GL_ENABLE_BIT);
//#endif
#ifndef ARCH_PSP	
	CacheShader::setup(obj,stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE);
#else
	sceGuSignal(GU_BEHAVIOR_SUSPEND, 1);
#endif
	
	// iterate through the list of materials with face reference sets 
	for (obj_matref_i = obj.mat_cache_data.begin(); obj_matref_i != obj.mat_cache_data.end(); obj_matref_i++)
	{
		// (*obj_matref_i).first // material reference

		bool mat_use = false;
		
		mat = (*obj_matref_i).first;
		
		// store material transparency state
		bool has_mask = mat->hasMask();	
		
		unsigned int segLimit = (*obj_matref_i).second.size();
		unsigned int segIndex = 0;
		unsigned int segRun = 0;
		unsigned int segRangeIndex = 0;
		unsigned int segRangeType = 0;
		unsigned int segElementIndex = 0;
		
		bool fault = false;
		bool usefault = (obj.numSegments != 1);			// assume objects with one segment don't need segment fault tests
		
		//true; //((*mat_segment_type_i).second.size()==1 && (*obj_matref_i).second.size()!=1);
		
		va_ref segRange;
		
		segRange.range_min = obj.cache_data.vertex_count;
		segRange.range_max = 0;
		segRange.element_count = 0;	
		segRangeIndex = element_index;		
		
		for (mat_segment_type_i = (*obj_matref_i).second.begin(); mat_segment_type_i != (*obj_matref_i).second.end(); mat_segment_type_i++)
		{
			segIndex++;

			if (stage == SHADER_STAGE_TRANSPARENT) // we're in the transparency stage and opaque isn't needed, we have to increase the element count though 
			{
				if (!has_mask)
				{
					element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
					segIndex++;
					continue;
				}
			}
			else if (stage == SHADER_STAGE_NULL || stage == SHADER_STAGE_OPAQUE || stage == SHADER_STAGE_NOTEXTURE)
			{
				if (has_mask) // vice-versa
				{
					element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
					has_trans = true;
					segIndex++;
					continue;
				}				
			}
						
			if (obj.numSegments >= 1 && obj.segmentMask)
			{
				if (obj.segmentMask->IsSet((*mat_segment_type_i).first) == false)
				{
					fault = true;
				}
			}
			
			segRangeType = (*(*mat_segment_type_i).second.begin()).first;
			
			if (fault) 
			{
				element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);				
				
				if (!usefault) 
				{
					fault=false;
					continue;
				}
			}
			else
			{				
				segRun++;
							
				segRange.element_count += (*(*mat_segment_type_i).second.begin()).second.group_size;
				
				if ((*(*mat_segment_type_i).second.begin()).second.range_min < segRange.range_min)
				{
					segRange.range_min = (*(*mat_segment_type_i).second.begin()).second.range_min;
				}
				
				if ((*(*mat_segment_type_i).second.begin()).second.range_max > segRange.range_max)
				{
					segRange.range_max = (*(*mat_segment_type_i).second.begin()).second.range_max;
				}				
				
				
				if (usefault) if (segIndex < segLimit) 
				{
					element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
					continue;
				}
				
			}
			
			
		//**** Material Initialization *****/
		
		if (!mat_use)
		{
			mat_use = true;
			
			// if we're in the opaque shader stage then we need to discard the transparent materials and vice versa 
			// transparent shader stage 
			if (stage == SHADER_STAGE_TRANSPARENT)
			{
				// alpha mask
				if (has_mask)
				{
					if (stage == SHADER_STAGE_NOTEXTURE)	// no texture 
					{
						mat->surfaceSetup();
					}
					else // full material 
					{
						mat->use();
					}
				}
			}
			else if (stage == SHADER_STAGE_NULL || stage == SHADER_STAGE_OPAQUE || stage == SHADER_STAGE_NOTEXTURE)
			{
				if (!has_mask) // we're in the transparency stage and opaque isn't needed, we have to increase the element count though 
				{
					// no texturing, just use the surface setup for colors and transparencies 
					if (stage == SHADER_STAGE_NOTEXTURE)
					{
						mat->surfaceSetup();
					}
					else // else full material 
					{
						mat->use();
					}
					
				}
			}

#ifndef ARCH_PSP
			// the active texcoord states change on a per material basis 
			if (stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE) for (unsigned int m = 0; m < obj.cache_data.max_uvs; m++)
			{
				Texture::setTexture(tex_offset+m);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			}
#endif			
			
#ifndef ARCH_PSP
			if (obj.cache_data.max_uvs == 0)
			{
				Texture::setTexture(tex_offset);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
#endif			

			if (stage == SHADER_STAGE_TRANSPARENT) 
			{
				mat->setupAlpha();
			}
			else if (mat->alpha_mask)
			{
				mat->setupAlpha();
			}
		}			
			
			
		shadow_count = 0;
		
		int numLights = 0;

#ifdef ARCH_PSP
//			sceGuDepthFunc( GU_GEQUAL );
#else
			glDepthFunc(GL_LEQUAL);
#endif
			
		numLights = lights?(*lights).size():0;
			
		if (numLights)
		{
			light_i = lights->begin();
		}
		
		//				if (obj.tangent_binormal_state)
		//				{
		//					GLShader::defaultShader.setShaderAttrib("binormal",(unsigned int)BUFFER_OFFSET(obj.binormalCacheOffset));
		//					GLShader::defaultShader.setShaderAttrib("tangent",(unsigned int)BUFFER_OFFSET(obj.binormalCacheOffset));
		//				}
		
		//				printf("vb numLights %d\n",(*lights).size());
			
#if !defined(OPENGL_ES) && !defined(ARCH_DC)  && !defined(ARCH_PSP)// shader light loop header
			
		RGB global_amb_temp = Light::global_ambient;
			
		for (int shaderCount = 0; shaderCount <= numLights; shaderCount++)
		{
			unsigned long shadowMask = 0;

			if (shaderCount < numLights) 
			{
				if ((mat->shader_mask & SHADER_VARIANT_LIGHTMAP) && shaderCount > 0) mat->shader_mask -= SHADER_VARIANT_LIGHTMAP;
				
				if (shaderCount > 0)
				{
					glEnable(GL_BLEND);
					
					glDepthMask(false);
					Light::setGlobalAmbient(RGB(0,0,0));
					
					if (stage == SHADER_STAGE_TRANSPARENT) 
					{
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glDepthMask(false);
					}
					else 
					{
						glBlendFunc(GL_ONE, GL_ONE);
					}
				}
				else
				{
					glDepthMask(true);
					
					if (stage == SHADER_STAGE_TRANSPARENT) 
					{
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glDepthMask(false);
					}
					else 
					{
						glDisable(GL_BLEND);
					}
				}
				
				if ((*light_i)->has_shadow)
				{
					switch ((*light_i)->type)
					{
						case LIGHT_SPOT:
							glActiveTexture(GL_TEXTURE0+(*light_i)->shadow_mtex);
							glClientActiveTexture(GL_TEXTURE0+(*light_i)->shadow_mtex);
							GLShader::defaultShader.setShaderValue("shadowMap0", (*light_i)->shadow_mtex); 
							
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, (*light_i)->shadowmapTex);
							
							switch (shadow_count)
							{
								case 0: shadowMask |= SHADER_VARIANT_SHADOW0; break;
								case 1: shadowMask |= SHADER_VARIANT_SHADOW1; break;
								case 2: shadowMask |= SHADER_VARIANT_SHADOW2; break;
								case 3: shadowMask |= SHADER_VARIANT_SHADOW3; break;
								case 4: shadowMask |= SHADER_VARIANT_SHADOW4; break;
								case 5: shadowMask |= SHADER_VARIANT_SHADOW5; break;
								case 6: shadowMask |= SHADER_VARIANT_SHADOW6; break;
								case 7: shadowMask |= SHADER_VARIANT_SHADOW7; break;
							}
							
							shadow_count++;
						break;
						case LIGHT_AREA:
							AreaLight *aLight = (AreaLight*)(*light_i);
							
							glActiveTexture(GL_TEXTURE0+aLight->oLight[0].shadow_mtex);
							glClientActiveTexture(GL_TEXTURE0+aLight->oLight[0].shadow_mtex);
							GLShader::defaultShader.setShaderValue("shadowMap0", aLight->oLight[0].shadow_mtex); 

							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, aLight->oLight[0].shadowmapTex);

							
							glActiveTexture(GL_TEXTURE0+aLight->oLight[1].shadow_mtex);
							glClientActiveTexture(GL_TEXTURE0+aLight->oLight[1].shadow_mtex);
							GLShader::defaultShader.setShaderValue("shadowMap1", aLight->oLight[1].shadow_mtex); 

							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, aLight->oLight[1].shadowmapTex);

							
							glActiveTexture(GL_TEXTURE0+aLight->oLight[2].shadow_mtex);
							glClientActiveTexture(GL_TEXTURE0+aLight->oLight[2].shadow_mtex);
							GLShader::defaultShader.setShaderValue("shadowMap2", aLight->oLight[2].shadow_mtex); 
							
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, aLight->oLight[2].shadowmapTex);

							
							for (int n = 0; n < 3; n++)
							{
								switch (shadow_count)
								{
									case 0: shadowMask |= SHADER_VARIANT_SHADOW0; break;
									case 1: shadowMask |= SHADER_VARIANT_SHADOW1; break;
									case 2: shadowMask |= SHADER_VARIANT_SHADOW2; break;
									case 3: shadowMask |= SHADER_VARIANT_SHADOW3; break;
									case 4: shadowMask |= SHADER_VARIANT_SHADOW4; break;
									case 5: shadowMask |= SHADER_VARIANT_SHADOW5; break;
									case 6: shadowMask |= SHADER_VARIANT_SHADOW6; break;
									case 7: shadowMask |= SHADER_VARIANT_SHADOW7; break;
								}
								
								shadow_count++;							
							}
							
						break;
					}
				}
				
				
				if (!GLShader::defaultShader.activateLight((*light_i)->type,shaderCount,(*light_i)->hasShadow()?(mat->shader_mask|shadowMask):mat->shader_mask,false)) 
				{
					light_i++;
					continue;
				}
				
				light_i++;
			}
			else if (!numLights)
			{
				if (shaderCount == 0) 
				{
					glDisable(GL_BLEND);
				}
				else 
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_ZERO, GL_SRC_COLOR);
				}
				
				glDepthMask(true);
				
				if (!GLShader::defaultShader.activateLight(LIGHT_NULL,0,mat->shader_mask,false)) continue;
			}
			else
			{
				glEnable(GL_BLEND);
				
				if (mat->shader_mask & SHADER_VARIANT_LIGHTMAP) 
				{
					glBlendFunc(GL_ONE, GL_ONE);
					
					if (!GLShader::defaultShader.activateLight(LIGHT_NULL,0,mat->shader_mask,false)) break;
				}
				else
				{
					Light::setGlobalAmbient(global_amb_temp);

					break;
				}
			}
			
#endif // end shader light loop header
			
			

			if (!usefault)	// no fault segments needed, just render all primitives
			{			
				segElementIndex = element_index;

				for (matref_type_i = (*mat_segment_type_i).second.begin(); matref_type_i != (*mat_segment_type_i).second.end(); matref_type_i++)
				{				
					if (!(*matref_type_i).second.element_count) continue;

#ifndef ARCH_PSP					
					switch((*matref_type_i).first)
					{
						case 1: glDrawRangeElements(GL_POINTS,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
						break;
						case 2:	glDrawRangeElements(GL_LINES,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
						break;
						case 3:	glDrawRangeElements(GL_TRIANGLES,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
						break;
#if !defined(OPENGL_ES)
						case 4:	glDrawRangeElements(GL_QUADS,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
						break;
#endif
					}
#else
//					sceGuDisable(GU_TEXTURE_2D);
//					sceGuDisable(GU_LIGHTING);
//					sceGuColor(0xff66ccff);
					
					unsigned int vtype = GU_INDEX_16BIT|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D;
					
					if (obj.cache_data.max_uvs) vtype |= GU_TEXTURE_32BITF;
					if (obj.hasColorMap) vtype |= GU_COLOR_8888;
					
					unsigned int ptype = 0;				
					
					switch ((*matref_type_i).first)
					{
						case 1: ptype = GU_POINTS; break;
						case 2: ptype = GU_LINES; break;
						case 3: ptype = GU_TRIANGLES; break;
					}

					
					sceGumDrawArray( ptype, vtype, (*matref_type_i).second.element_count, BUFFER_OFFSET(segElementIndex), obj.cache_data.data);

					int matLayerCount = mat->getLayerSize();
					for (unsigned int matLayer = 1; matLayer < matLayerCount; matLayer++)
					{
						mat->texSetup(matLayer);
						sceGumDrawArray( ptype, vtype, (*matref_type_i).second.element_count, BUFFER_OFFSET(segElementIndex), obj.cache_data.data);
					}
						
					
#endif					
					segElementIndex += (*matref_type_i).second.element_count*sizeof(cvrElement);
				}
			}
			else	// use accumulated cache fault runs
			{
#ifndef ARCH_PSP
				switch (segRangeType)
				{
					case 1:
						glDrawRangeElements(GL_POINTS,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
					case 2:
						glDrawRangeElements(GL_LINES,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
					case 3:
						glDrawRangeElements(GL_TRIANGLES,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
#if !defined(OPENGL_ES)
					case 4:
						glDrawRangeElements(GL_QUADS,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
#endif					
				}
#else
				unsigned int vtype = GU_INDEX_16BIT|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D;
				
				if (obj.cache_data.max_uvs) vtype |= GU_TEXTURE_32BITF;
				if (obj.hasColorMap) vtype |= GU_COLOR_8888;

				unsigned int ptype = 0;				
				switch (segRangeType)
				{
					case 1: ptype = GU_POINTS; break;
					case 2: ptype = GU_LINES; break;
					case 3: ptype = GU_TRIANGLES; break;
				}
				
				if (ptype) sceGumDrawArray( ptype, vtype, segRange.element_count, BUFFER_OFFSET(segRangeIndex), obj.cache_data.data);
#endif
			}	

#if !defined(OPENGL_ES) && !defined(ARCH_DC) && !defined(ARCH_PSP)
			// end shader light loop
		}
#endif			
			if (!usefault) element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
			
			if (usefault)	// reset fault line
			{
				segRange.range_min = obj.cache_data.vertex_count;
				segRange.range_max = 0;
				segRange.element_count = 0;	
				
				segRun = 0;
				if (!fault) element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
				fault = false;
				segRangeIndex = element_index;
				
			}

		}

			
	}

	
	// clear the textures used in this material 
	if (stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE)
	{
		Texture::clearAllTextures();
	}
		
	// call the transparency stage if necessary 
	if (stage == SHADER_STAGE_NULL && has_trans) render(obj,SHADER_STAGE_TRANSPARENT);	
};

#ifndef ARCH_PSP

void CacheShader::setup(Mesh &obj, bool init_texcoord)
{
	// special lights which have been bound that contribute to texturing 
//	unsigned int tex_offset = Texture::tex_use+1;

#if !defined(OPENGL_ES) && !defined(ARCH_DC)	
	if (obj.buffer_state && lockObj != &obj)
	{
		// Bind the vertex buffers from the cache data 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, obj.cache_data.elementBufferNum);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, obj.cache_data.dataBufferNum);
		
		// set the vertex pointer 
		glVertexPointer(3,GL_FLOAT,0,BUFFER_OFFSET(0));
		glEnableClientState(GL_VERTEX_ARRAY);
		
		// set the normal pointer 
		glNormalPointer(GL_FLOAT,0,BUFFER_OFFSET(obj.normalCacheOffset));
		glEnableClientState(GL_NORMAL_ARRAY);
		
		// activate uv map pointers for multitexture layers 
		if (init_texcoord) 
		{
			for (unsigned int m = 0; m < obj.cache_data.max_uvs; m++)
			{
				Texture::setTexture(m);
				glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(obj.uvCacheOffset+(sizeof(va_uv)*obj.cache_data.vertex_count*m)));
			}
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	else 
#endif
		if (lockObj != &obj)
		{
#if !defined(OPENGL_ES) && !defined(ARCH_DC)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
			glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB);
			glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
#endif
			
			if (obj.hasColorMap)
			{
				glColorPointer(3, GL_FLOAT, 0, (const GLvoid *)(obj.cache_data.data+(obj.colorMapOffset)));
			}
			
			glVertexPointer(3, GL_FLOAT, 0,  (const GLvoid *)obj.cache_data.data);
			glEnableClientState(GL_VERTEX_ARRAY);
			
#ifndef ARCH_DC	// DC doesn't process normals
			glNormalPointer(GL_FLOAT, 0,  (const GLvoid *)(obj.cache_data.data+(obj.normalCacheOffset)));
			glEnableClientState(GL_NORMAL_ARRAY);
#endif		
			
			if (init_texcoord) 
			{
				for (unsigned int m = 0; m < obj.cache_data.max_uvs; m++)
				{
					Texture::setTexture(m);
					glTexCoordPointer(2, GL_FLOAT, 0,  (const GLvoid *)(obj.cache_data.data+obj.uvCacheOffset+(sizeof(va_uv)*obj.cache_data.vertex_count*m)));
				}
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			}
			
		}
	
	if (obj.hasColorMap && obj.dynamic_colors)	// Mostly just for Dreamcast lighting at the moment
	{
		glColorPointer(3, GL_FLOAT, 0, (const GLvoid *)obj.dynamic_colors);				
		glEnableClientState(GL_COLOR_ARRAY);	
	}
	
#ifdef ARCH_PSP
	sceGuSignal(GU_BEHAVIOR_SUSPEND, 2);
#endif
	
	lockObj = &obj;
}
#endif


#if  !defined(OPENGL_ES) && !defined(ARCH_PSP)
void ShadowShader::render(Mesh &obj, int stage)
{
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
	
	// mat_reflist_i -- used to iterate through the material mat_reflist of the object
	std::map<Material *, map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint > >::iterator obj_matref_i;
	// segment iterator
	map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint >::iterator mat_segment_type_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map
	std::map<cvrIndex, va_ref, ltindex>::iterator matref_type_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map
	std::map<cvrIndex, va_ref, ltindex>::reverse_iterator matref_last_i;
	
	bool has_trans = false;
	unsigned int element_index = 0;
	//	bool skip_segment = false;
	//	Material *matNum;
	Material *mat;
	
	if (!obj.buffer_state) element_index = (unsigned long)obj.cache_data.cache_element;
		
	// Reset mutitexture state
	Texture::clearAllTextures();
	
	unsigned int tex_offset = Texture::tex_use+1;
		
//#if !defined(OPENGL_ES) && !defined(ARCH_DC)
	// store attribs
	//glPushAttrib(GL_ENABLE_BIT);
//#endif
	
	CacheShader::setup(obj,true);
		
	bool maskState = false;
	
	// iterate through the list of materials with face reference sets 
	for (obj_matref_i = obj.mat_cache_data.begin(); obj_matref_i != obj.mat_cache_data.end(); obj_matref_i++)
	{
		// (*obj_matref_i).first // material reference id
		
		bool mat_use = false;
		
		//		matNum = (*obj_matref_i).first;
		mat = (*obj_matref_i).first; //&Material::materials[matNum];
		
		// store material transparency state
		bool has_mask = mat->hasMask();	
		
		unsigned int segLimit = (*obj_matref_i).second.size();
		unsigned int segIndex = 0;
		unsigned int segRun = 0;
		unsigned int segRangeIndex = 0;
		unsigned int segRangeType = 0;
		unsigned long segElementIndex = 0;
		
		bool fault = false;
		bool usefault = (obj.numSegments != 1);			// assume objects with one segment don't need segment fault tests
		
		//true; //((*mat_segment_type_i).second.size()==1 && (*obj_matref_i).second.size()!=1);
		
		va_ref segRange;
		
		segRange.range_min = obj.cache_data.vertex_count;
		segRange.range_max = 0;
		segRange.element_count = 0;	
		segRangeIndex = element_index;		
		
		for (mat_segment_type_i = (*obj_matref_i).second.begin(); mat_segment_type_i != (*obj_matref_i).second.end(); mat_segment_type_i++)
		{
			segIndex++;
			
			if (has_mask) // vice-versa
			{
				element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
				has_trans = true;
				segIndex++;
				continue;
			}				
			
			if (obj.numSegments >= 1 && obj.segmentMask)
			{
				if (obj.segmentMask->IsSet((*mat_segment_type_i).first) == false)
				{
					fault = true;
				}
			}
			
			segRangeType = (*(*mat_segment_type_i).second.begin()).first;
			
			if (fault) 
			{
				element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);				
				
				if (!usefault) 
				{
					fault=false;
					continue;
				}
			}
			else
			{				
				segRun++;
				
				segRange.element_count += (*(*mat_segment_type_i).second.begin()).second.group_size;
				
				if ((*(*mat_segment_type_i).second.begin()).second.range_min < segRange.range_min)
				{
					segRange.range_min = (*(*mat_segment_type_i).second.begin()).second.range_min;
				}
				
				if ((*(*mat_segment_type_i).second.begin()).second.range_max > segRange.range_max)
				{
					segRange.range_max = (*(*mat_segment_type_i).second.begin()).second.range_max;
				}				
				
				
				if (usefault) if (segIndex < segLimit) 
				{
					element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
					continue;
				}
				
			}
			
			
			//**** Material Initialization *****/
			
			if (!mat_use)
			{
				mat_use = true;
				
				glDepthFunc(GL_LEQUAL);
//				glDepthMask(true);	
				
				if (mat->alpha_mask)
				{					
					mat->texSetup(TEXTURE_DIFF);
					mat->surfaceSetup();
					
					Texture::setTexture(tex_offset);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					mat->setupAlpha();
					
					maskState = true;
				}
				else
				{
					glDisable(GL_ALPHA_TEST);

					if (maskState)
					{
						Texture::setTexture(tex_offset);
						glDisable(GL_TEXTURE_2D);
						glEnable(GL_CULL_FACE);
						glCullFace(GL_FRONT);
					}
//					mat->setupAlpha();
				}
			}			
			
			
			
				
			if (!usefault)	// no fault segments needed, just render all primitives
			{				
				segElementIndex = element_index;
				
				for (matref_type_i = (*mat_segment_type_i).second.begin(); matref_type_i != (*mat_segment_type_i).second.end(); matref_type_i++)
				{				
					if (!(*matref_type_i).second.element_count) continue;
					
					switch((*matref_type_i).first)
					{
						case 1: glDrawRangeElements(GL_POINTS,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
							break;
						case 2:	glDrawRangeElements(GL_LINES,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
							break;
						case 3:	glDrawRangeElements(GL_TRIANGLES,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
							break;
#ifndef OPENGL_ES
						case 4:	glDrawRangeElements(GL_QUADS,(*matref_type_i).second.range_min,(*matref_type_i).second.range_max,(*matref_type_i).second.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segElementIndex));
							break;
#endif
					}
					segElementIndex += (*matref_type_i).second.element_count*sizeof(cvrElement);
				}
			}
			else	// use accumulated cache fault runs
			{
				switch (segRangeType)
				{
					case 1:
						glDrawRangeElements(GL_POINTS,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
					case 2:
						glDrawRangeElements(GL_LINES,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
					case 3:
						glDrawRangeElements(GL_TRIANGLES,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
#ifndef OPENGL_ES
					case 4:
						glDrawRangeElements(GL_QUADS,segRange.range_min,segRange.range_max,segRange.element_count,GL_UNSIGNED_INT,BUFFER_OFFSET(segRangeIndex));
						break;
#endif					
				}
				
			}	
				
			if (!usefault) element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
			
			if (usefault)	// reset fault line
			{
				segRange.range_min = obj.cache_data.vertex_count;
				segRange.range_max = 0;
				segRange.element_count = 0;	
				
				segRun = 0;
				if (!fault) element_index += (*(*mat_segment_type_i).second.begin()).second.group_size*sizeof(cvrElement);
				fault = false;
				segRangeIndex = element_index;
				
			}
			
		}
		
		
	}
	
//#ifndef ARCH_DC
	//glPopAttrib();
//#endif
	
};
#endif	


#if !defined(OPENGL_ES) && !defined(ARCH_PSP)

void RegularShader::render(Mesh &obj, int stage)
{
	//	map<cvrIndex, map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint >, ltindex> mat_reflist;

	/* mat_reflist_i -- used to iterate through the material mat_reflist of the object */
	map<Material *, map<unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint > >::iterator obj_matref_i;
	map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint >::iterator matref_segment_i;
	/* mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map */
	map<unsigned short, std::set<cvrIndex>, ltushort>::iterator matref_type_i;
	/* may_facelist_i -- used to iterate through the set of faces which belong to the mat_facetypelist_i map */
	std::set<cvrIndex>::iterator type_face_i;
	/* faces_point_i -- used to step through the points of the face which is referenced by mat_facelist_i */
	std::vector<cvrIndex>::iterator face_point_i;
	
	bool has_trans = false;

	if (obj.mat_reflist.size()==1) obj.buildRefList();
	
#ifndef ARCH_DC
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif
	/* iterate through the list of materials with face reference sets */
	for (obj_matref_i = obj.mat_reflist.begin(); obj_matref_i != obj.mat_reflist.end(); obj_matref_i++)
	{
		// (*obj_matref_i).first // material reference
		Material *mat = (*obj_matref_i).first;

		unsigned int tex_offset = Texture::tex_use+1;

		// if it has a transparency mask, textures will be needed for depth calc on shadow
		bool has_mask = mat->hasMask();	

		/* if we're in the opaque shader stage then we need to discard the transparent materials and vice versa */
		if (stage == SHADER_STAGE_NULL || stage == SHADER_STAGE_OPAQUE || stage == SHADER_STAGE_NOTEXTURE)
		{
			/* alpha mask? */
			if (!mat->hasMask()) 
			{				
				/* no texturing, just use the surface setup for colors and transparencies */
				if (stage == SHADER_STAGE_NOTEXTURE)
				{
#ifndef ARCH_DC
					GLShader::clear();
#endif
					mat->surfaceSetup();
				}
				else /* else full material */
				{
					mat->use();
				}
			}
			else /* we're in the transparency stage and opaque isn't needed, we have to skip the element count though */
			{
				has_trans = true;
				continue;
			}
		}
		
		/* transparent shader stage */
		if (stage == SHADER_STAGE_TRANSPARENT)
		{
			/* alpha mask */
			if (mat->hasMask()) 
			{
				if (stage == SHADER_STAGE_NOTEXTURE)	/* no texture */
				{
					mat->surfaceSetup();
				}
				else /* full material */
				{
					mat->use();
				}
			}
			else /* opaque, need transparent, skip it */
			{
				continue;	
			}
		}

				

		for (matref_segment_i = (*obj_matref_i).second.begin(); matref_segment_i != (*obj_matref_i).second.end(); matref_segment_i++)
		{
			// (*obj_matref_i).first // material reference
			for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
			{
					// (*matref_type_i).first // primitive type
				
				if ((*matref_type_i).first == 1) glBegin(GL_POINTS);
				else if ((*matref_type_i).first == 2) glBegin(GL_LINES);
				else if ((*matref_type_i).first == 3) glBegin(GL_TRIANGLES);
				else if ((*matref_type_i).first == 4) glBegin(GL_QUADS);
			
				for (type_face_i = (*matref_type_i).second.begin(); type_face_i != (*matref_type_i).second.end(); type_face_i++)
				{
					cvrIndex pt_count = 0;

					if ((*matref_type_i).first > 4)	glBegin(GL_POLYGON);
					
					for (face_point_i = obj.faces[(*type_face_i)]->pointref.begin(); face_point_i != obj.faces[(*type_face_i)]->pointref.end(); face_point_i++)
					{
	#ifndef ARCH_DC
						for (unsigned int m = 0; m < obj.faces[(*type_face_i)]->uv.size(); m++)
						{
							if (!obj.faces[(*type_face_i)]->uv[m].empty())
							{
								int id = GL_TEXTURE0_ARB+tex_offset+m;
								cvrFloat u = obj.faces[(*type_face_i)]->uv[m][pt_count].u;
								cvrFloat v = obj.faces[(*type_face_i)]->uv[m][pt_count].v;

								glMultiTexCoord2fARB(id,u,v);
							}
						}
	#else
						if (obj.faces[(*type_face_i)]->uv.size())
						{
							glTexCoord2f(obj.faces[(*type_face_i)]->uv[0][pt_count].u,obj.faces[(*type_face_i)]->uv[0][pt_count].v);
						}
	#endif
						
						glNormal3f(obj.faces[(*type_face_i)]->point_normals[pt_count].x,obj.faces[(*type_face_i)]->point_normals[pt_count].y,obj.faces[(*type_face_i)]->point_normals[pt_count].z);
						glVertex3f(obj.points[(*face_point_i)]->x,obj.points[(*face_point_i)]->y,obj.points[(*face_point_i)]->z);
						
						pt_count++;
					}					

					if ((*matref_type_i).first > 4) glEnd();
				}					
				
				if ((*matref_type_i).first <= 4) glEnd();
			}	
		}
		/* clear the textures used in this material */
		if (stage != SHADER_STAGE_NOMATERIAL && stage != SHADER_STAGE_NOTEXTURE)
		{
			Texture::clearAllTextures();
		}
	}


//#ifndef ARCH_DC
	//glPopAttrib();
//#endif
	
	/* call the transparency stage if necessary */
	if (stage == SHADER_STAGE_NULL && has_trans) render(obj,SHADER_STAGE_TRANSPARENT);
};
#endif



void ObjectShader::bind(std::set<Light *> *lights_in)
{
	lights = lights_in;

	cShader.bind(lights_in);
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)
	rShader.bind(lights_in);
#endif
};

void ObjectShader::render(Mesh &obj, int stage)
{
	if (obj.cache_state)
	{
		cShader.shadowModelMatrix = shadowModelMatrix;
		cShader.render(obj,stage);
	}
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)
	else
	{
		cShader.lockObj = NULL;
		rShader.render(obj,stage);
	}	
#endif
};


Shader::Shader() : shadow_alpha(0.6f), lights(NULL), shadowModelMatrix(NULL)
{
	
};


void Shader::shadowAlpha(float shadow_alpha_in)
{
	shadow_alpha = shadow_alpha_in;
};

float Shader::shadowAlpha()
{
	return shadow_alpha;
};



