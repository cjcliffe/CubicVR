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

#include <CubicVR/Material.h>
#include <CubicVR/Camera.h>

//#define nullMaterial Material::nullMaterial

//std::vector<Material> Material::materials;
//std::map<std::string, cvrIndex, string_less> Material::nameref;

//IMPEXP MaterialInit Material::initializer = MaterialInit();


//MaterialInit::MaterialInit()
//{
//	Material::createNull();
//}
//
//MaterialInit::~MaterialInit()
//{
//	// nil
//}

Material Material::nullMaterial = Material();


Material::Material() : ambient(0.02f, 0.02f, 0.02f, 1.0f), diffuse(0.7f, 0.7f, 0.7f, 1.0f), specular(0.0f, 0.0f, 0.0f, 1.0f), color(1.0f,1.0f,1.0f,1.0f), shininess(0.0f), sidedness(MATERIAL_FACE_FRONT), max_smooth(0), cached(false), dList(0), shader_mask(0), color_mask(false), alpha_mask(false)
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)  && !defined(ARCH_DC)
 ,program(NULL)

#endif
{ 
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)  && !defined(ARCH_DC)
	for (int i = 0; i < 32; i++) tex_bindref[i] = -1;
	tex_inc = 0;
#endif

};

#if !defined(OPENGL_ES) && !defined(ARCH_PSP) && !defined(ARCH_DC)

short Material::tex_bindref[32];
short Material::tex_inc;
#endif


Material::~Material()
{
};


void Material::setReflective(float ref)
{
	reflectivity = ref;	
}


//cvrIndex Material::create()
//{
//	materials.resize(materials.size()+1);
//	return materials.size()-1;
//};


//cvrIndex Material::create(std::string mat_name)
//{
//	cvrIndex i = create();
//	nameref[mat_name] = i;
//	return i;
//};


//void Material::clear()
//{
//	materials.clear();
//};


//void Material::createNull()
//{
//	if (materials.empty())	/* generate the 'null' material */
//	{
//		 materials.push_back(Material());
//		 nameref["null"] = 0;
//	}
//}


//Material &Material::getMaterial(cvrIndex mat_num)
//{
//	return materials[mat_num];
//};
//
//
//Material &Material::getMaterial(std::string mat_name)
//{
//	return materials[nameref[mat_name]];
//};
//
//
//cvrIndex Material::getMaterialId(std::string mat_name)
//{
//	return nameref[mat_name];
//};


#ifndef ARCH_PSP

//void Material::use(cvrIndex matNum)
//{
//	materials[matNum].use();
//};

void Material::use()
{
	surfaceSetup();
	texSetup();
};

void Material::texSetup(unsigned int layerType)
{
	unsigned int i;

	Texture::clearAllTextures();
	
	if (!layer.empty())
	{
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
		glDisable(GL_ALPHA_TEST);
		glDepthMask(true);

		shader_mask = 0;
		
		for	(i = 0; i < layer.size(); i++)
		{
			if (layer[i].texture_ref) 
			{
				int tex_num;
				//tex_num = Texture::nextTexture();

#ifndef OPENGL_ES
#ifndef ARCH_DC
//				glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FAIL_VALUE_ARB,0.5);
//#define TEXTURE_DIFF 1	// color
//#define TEXTURE_SPEC 2	// specular strength
//#define TEXTURE_NORM 3	// normal
//#define TEXTURE_LUMI 4	// luminosity
//#define TEXTURE_ALPHA 5	// alpha transparency
//#define TEXTURE_REFL 6	// reflection strength
//#define TEXTURE_ENVSPHERE 7	// environment (spherical)
//#define TEXTURE_LIGHTMAP 8
//#define TEXTURE_BUMP 9	// parallax bump (height map)
//				
				
				const char *texNames[10] = {NULL,"colorMap","specMap","bumpMap","lumiMap",NULL,"reflectionMap","envSphereMap","lightMap","parallaxMap"};

				if (tex_bindref[layer[i].tex_type] == -1 && texNames[layer[i].tex_type] != NULL) 
				{
					GLShader::defaultShader.setShaderValue(texNames[layer[i].tex_type], (unsigned int)tex_inc);
					tex_bindref[layer[i].tex_type] = tex_inc;
					tex_inc++;
				}
				
				if ((layerType && layer[i].tex_type==layerType) || !layerType)
				switch (layer[i].tex_type)
				{
					case TEXTURE_DIFF: 

							shader_mask |= SHADER_VARIANT_COLOR;
						
							if (Texture::textures[layer[i].texture_ref]->alpha)
							{
								shader_mask |= SHADER_VARIANT_ALPHA;	
							}
					break;
					case TEXTURE_SPEC:
//							GLShader::defaultShader.setShaderValue("specMap", (unsigned int)tex_num);
							shader_mask |= SHADER_VARIANT_SPECULAR;
					break;
					case TEXTURE_NORM:
//							GLShader::defaultShader.setShaderValue("bumpMap", (unsigned int)tex_num);
							shader_mask |= SHADER_VARIANT_BUMP;
					break;
					case TEXTURE_LUMI:
//							GLShader::defaultShader.setShaderValue("lumiMap", (unsigned int)tex_num);
							shader_mask |= SHADER_VARIANT_LUMINOSITY;
					break;
//					case TEXTURE_ALPHA: 
//							GLShader::defaultShader.setShaderValue("alphaMap", (unsigned int)tex_num); 
//							shader_mask |= SHADER_VARIANT_ALPHA;
//					break;
					case TEXTURE_REFL: 
//						GLShader::defaultShader.setShaderValue("reflectionMap", (unsigned int)tex_num); 
						shader_mask |= SHADER_VARIANT_REFLECTION;
					break;
					case TEXTURE_ENVSPHERE:
//							GLShader::defaultShader.setShaderValue("envSphereMap", (unsigned int)tex_num); 
							GLShader::defaultShader.setShaderValue("envAmount", (float)reflectivity); 
							shader_mask |= SHADER_VARIANT_ENVSPHEREMAP;
						
//						Texture::setTexture(0);
//						
//						glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//						glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//						glEnable(GL_TEXTURE_GEN_S);
//						glEnable(GL_TEXTURE_GEN_T);
//						glDisable(GL_TEXTURE_GEN_R);
//						glDisable(GL_TEXTURE_GEN_Q);
					break;
					case TEXTURE_LIGHTMAP: 
//						GLShader::defaultShader.setShaderValue("lightMap", (unsigned int)tex_num); 
						shader_mask |= SHADER_VARIANT_LIGHTMAP;
					break;
					case TEXTURE_BUMP:
//						GLShader::defaultShader.setShaderValue("parallaxMap", (unsigned int)tex_num);
						GLShader::defaultShader.setShaderValue("parallaxScale", (float)0.05);
						GLShader::defaultShader.setShaderValue("parallaxBias", (float)0.04);
						shader_mask |= SHADER_VARIANT_PARALLAX;
					default:
//							GLShader::defaultShader.setShaderValue("colorMap", GLExt::tex_usage_max); 
//							shader_mask |= SHADER_VARIANT_COLOR;
//							GLShader::defaultShader.setShaderValue("colorMap", GLExt::tex_usage_max+1); 
					break;
				}
				
				Texture::setTexture(tex_bindref[layer[i].tex_type]);
				Texture::use(layer[i].texture_ref);

#endif
#else
#ifndef ARCH_PSP			
//				Texture::setTexture(tex_bindref[layer[i].tex_type]);
				if (layer.size()) Texture::use(layer[0].texture_ref);
#endif
#endif
				
				
				if (color.a < 1.0) glEnable(GL_BLEND);

/*
				switch (layer[i].blend_mode)
				{
					case MATERIAL_BLEND_ALPHA:
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
					 	glEnable(GL_ALPHA_TEST);
						glAlphaFunc(GL_GEQUAL, 0.3f);
					break;
					case MATERIAL_BLEND_NORMAL:
						if (i == 0)
						{
							glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
							glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
						}
						else
						{
							glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
							glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
						}
					break;
					case MATERIAL_BLEND_ADDITIVE:
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
						glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);
					break;
					case MATERIAL_BLEND_SUBTRACTIVE:
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
						glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_SUBTRACT);
					break;
					case MATERIAL_BLEND_DIFFERENCE:
						//NA
					break;
					case MATERIAL_BLEND_MULTIPLY:
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
						glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MULT);
					break;
					case MATERIAL_BLEND_DIVIDE:
						//NA
					break;
					case MATERIAL_BLEND_DISPLACE:
						//NA
					break;
				}


				if (layer[i].blend_mode == MATERIAL_BLEND_ALPHA)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				 	glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GEQUAL, 0.3f);
				}
*/				
//#ifndef OPENGL_ES
//				switch(layer[i].map_mode)
//				{
//					Texture::setTexture(0);
//
//					case MATERIAL_MAP_SPHERICAL:
//						glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//						glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//						glEnable(GL_TEXTURE_GEN_S);
//						glEnable(GL_TEXTURE_GEN_T);
//						glDisable(GL_TEXTURE_GEN_R);
//						glDisable(GL_TEXTURE_GEN_Q);
//						shader_mask |= SHADER_VARIANT_ENVSPHEREMAP;
//					break;
//					default:
////						glDisable(GL_TEXTURE_GEN_S);
////						glDisable(GL_TEXTURE_GEN_T);
////						glDisable(GL_TEXTURE_GEN_R);
////						glDisable(GL_TEXTURE_GEN_Q);
//					break;
//				}
//#endif
			}
		}
	}
	else
	{
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_TEXTURE_2D);
	}
}



/// hasParallax = needs binormal/tangent for parallax
//bool Material::hasParallax()
//{
//	int i;
//	
//	for	(i = 0; i < layer.size(); i++)
//	{
//		if (layer[i].texture_ref) 
//		{
//			//				glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FAIL_VALUE_ARB,0.5);
//			
//			if (layer[i].tex_type == TEXTURE_BUMP)
//			{
//				return true;
//			}
//		}
//	}
//	
//	return false;
//}

void Material::surfaceSetup()
{
#ifndef ARCH_DC
	GLfloat 
	glambient[] =	{ambient.r, ambient.g, ambient.b, ambient.a},
	gldiffuse[] = {diffuse.r, diffuse.g, diffuse.b, diffuse.a}, 
	glspecular[] = {specular.r, specular.g, specular.b, specular.a};

	static float b_twoside[] =	{GL_TRUE};
	static float b_oneside[] =	{GL_FALSE};

	// pixel shader init
//	GLShader::defaultShader.setShaderValue("specMap", (unsigned int)0);
//	GLShader::defaultShader.setShaderValue("colorMap", (unsigned int)0);
//	GLShader::defaultShader.setShaderValue("bumpMap", (unsigned int)0);
//	GLShader::defaultShader.setShaderValue("lumiMap", (unsigned int)0);
//	GLShader::defaultShader.setShaderValue("alphaMap", (unsigned int)0);
	switch (sidedness)
	{
		case MATERIAL_FACE_FRONT:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, b_oneside);
		  
			glMaterialfv(GL_FRONT, GL_AMBIENT, glambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, gldiffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, glspecular);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
			break;
		
		case MATERIAL_FACE_BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, b_twoside);

			glMaterialfv(GL_BACK, GL_AMBIENT, glambient);
			glMaterialfv(GL_BACK, GL_DIFFUSE, gldiffuse);
			glMaterialfv(GL_BACK, GL_SPECULAR, glspecular);
			glMaterialf(GL_BACK, GL_SHININESS, shininess);
			break;			
		
		case MATERIAL_FACE_BOTH:
			glDisable(GL_CULL_FACE);
			glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, b_twoside);

			glMaterialfv(GL_FRONT, GL_AMBIENT, glambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, gldiffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, glspecular);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);

			glMaterialfv(GL_BACK, GL_AMBIENT, glambient);
			glMaterialfv(GL_BACK, GL_DIFFUSE, gldiffuse);
			glMaterialfv(GL_BACK, GL_SPECULAR, glspecular);
			glMaterialf(GL_BACK, GL_SHININESS, shininess);
			break;				
		}
#endif

	glEnable(GL_COLOR_MATERIAL);

	glColor4f(color.r,color.g,color.b,color.a);
	
//	if (color.a != 1.0)
//	{
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
//		glDepthMask(0);
//	}
	
};


#else


//psp
void Material::texSetup(unsigned int layerNum)
{
	if (!(layerNum < layer.size()) || (!layer.size())) 
	{
		sceGuDisable( GU_TEXTURE_2D );
//		printf("Invalid layer specified! %d\n",layerNum);
		return;
	}
		
	if (layer[layerNum].texture_ref) 
	{
		sceGuEnable( GU_TEXTURE_2D );
		Texture::use(layer[layerNum].texture_ref);
	}
	else
	{
		sceGuDisable( GU_TEXTURE_2D );	
	}
	
	if(layer[layerNum].tex_type == TEXTURE_ENVSPHERE)
	{
		float angle = -2.0f * (GU_PI/180.0f);

		if (Camera::lastActive)
		{
			angle = Camera::lastActive->camAngle;
		}
		
		float cs = cos(angle);
		float sn = sin(angle);
		sceGuDisable( GU_LIGHTING );
		sceGuEnable( GU_BLEND );
		sceGuBlendFunc(GU_ADD, GU_SRC_COLOR, GU_DST_COLOR, 0, 0);


		ScePspFVector3 envmapMatrixColumns[2] = {
			{ cs,  sn, 0.0f },
			{ -sn, cs, 0.0f }
		};
		sceGuLight( 2, GU_DIRECTIONAL, GU_DIFFUSE, &envmapMatrixColumns[0] );
		sceGuLight( 3, GU_DIRECTIONAL, GU_DIFFUSE, &envmapMatrixColumns[1] );

		sceGuTexMapMode
		(
			GU_ENVIRONMENT_MAP,	// envmap mode on
			2,	// use 2nd light position as an envmap matrix 1st column
			3	// use 3rd light position as an envmap matrix 2nd column
		);
	}
	else
	{
		sceGuTexMapMode(GU_TEXTURE_COORDS,0,0);
		sceGuEnable( GU_LIGHTING );
		sceGuDisable( GU_BLEND );
	}

}


//psp
void Material::use(unsigned int layerNum)
{
	surfaceSetup();
	texSetup(layerNum);
};


//psp
//void Material::use(cvrIndex matNum, unsigned int layerNum)
//{
//	materials[matNum].use(layerNum);
//};

//psp
void Material::surfaceSetup()
{
	sceGuEnable(GU_CULL_FACE);
	sceGuFrontFace(GU_CCW);
	sceGuColorMaterial(GU_DIFFUSE_AND_SPECULAR);
	sceGuEnable(GU_SEPARATE_SPECULAR_COLOR);

	sceGuModelColor(
		GU_COLOR(0,0,0,0),
		GU_COLOR(ambient.r, ambient.g, ambient.b, ambient.a),
		GU_COLOR(diffuse.r, diffuse.g, diffuse.b, diffuse.a),
		GU_COLOR(specular.r, specular.g, specular.b, specular.a));

	sceGuColor(GU_COLOR(color.r,color.g,color.b,color.a));
		
	sceGuSpecular(shininess);
};

#endif


unsigned int Material::getLayerSize()
{
	return layer.size();
}


void Material::bindTexture(unsigned short texLayer, cvrIndex texRefId, short tex_type)	
{
	if ((unsigned short)layer.size()<texLayer+1) 
	{
		addLayer(texLayer);
	}
	
	layer[texLayer].texture_ref = texRefId;
	layer[texLayer].tex_type = tex_type;
	
	if (Texture::textures[texRefId]->alpha && tex_type == TEXTURE_DIFF) alpha_mask = true;
	
//	if (tex_type == TEXTURE_ALPHA) 
//	{
//		layer[texLayer].blend_mode = MATERIAL_BLEND_ALPHA;
//		alpha_mask = true;
//	}
};


void Material::bindTexture(cvrIndex texRefId)
{
	if (layer.empty()) 
	{
		addLayer();
	}

	if (Texture::textures[texRefId]->alpha) alpha_mask = true;

	layer.back().texture_ref = texRefId;
	layer.back().tex_type = TEXTURE_DIFF;
};


void Material::addLayer(unsigned short texLayer)
{
	if ((unsigned short)layer.size() < texLayer+1) layer.resize(texLayer+1, MaterialLayer());
};


unsigned short Material::addLayer()
{
	layer.push_back(MaterialLayer());
	return layer.size()-1;
};


/// set ambient color
void Material::setAmbient(const RGBA &ambient_in)
{
	ambient = ambient_in;
};


void Material::setDiffuse(const RGBA &diffuse_in)
{
	diffuse = diffuse_in;
};


void Material::setSpecular(const RGBA &specular_in)
{
	specular = specular_in;
};


void Material::setShininess(float shininess_in)
{
	shininess = shininess_in;
};


void Material::setColor(const RGBA &color_in)
{
	color = color_in;
};


void Material::set(const RGBA &ambient_in, const RGBA &diffuse_in,const  RGBA &specular_in, const RGBA &color_in, float shininess_in)
{
	setAmbient(ambient_in);
	setDiffuse(diffuse_in);
	setSpecular(specular_in);
	setColor(color_in);
	setShininess(shininess_in);
};


void Material::setSidedness(unsigned short sidedness_in)
{
	sidedness = sidedness_in;
};
		
		
void Material::setMaxSmooth(float max_smooth_in)
{
	max_smooth = max_smooth_in;
};
		

void Material::colorMask(bool cmask_in)
{
	color_mask = cmask_in;
};


//bool Material::hasMask()
//{
//	if (color_mask) return true;
//	if (color.a != 1.0) return true;
//	if (alpha_mask) return true;
//	
////	for	(unsigned int i = 0; i < layer.size(); i++)
////	{
////		if (layer[i].tex_type == TEXTURE_ALPHA) return true;
////	}	
//
//	return false;
//};


#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC

bool Material::hasProgram()
{
	return (program != NULL || GLShader::defaultShaderLoaded);
}


void Material::useProgram()
{
	if (program) 
	{
		program->setupShader();
	}
	else if (GLShader::defaultShaderLoaded)
	{
		GLShader::defaultShader.setupShader();
	}
}


void Material::setupProgram()
{
	if (program) 
	{
		program->setupShader();
	}
	else
	{
		GLShader::defaultShader.setupShader();
	}
}

void Material::clearProgram()
{
	if (program) delete program;
};


bool Material::loadProgram(const char *vp, const char *fp)
{
	if (program) delete program;
	
	program = new GLShader();
	
	return program->setup(vp,fp);
};


bool Material::loadProgramFile(const char *vf, const char *ff)
{
	if (program) delete program;
	
	program = new GLShader();
	
	return program->load(vf,ff);
};


#endif
#endif
#endif
