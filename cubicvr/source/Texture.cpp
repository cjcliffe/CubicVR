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

#include <CubicVR/Texture.h>
#include <CubicVR/Logger.h>
#include <algorithm> 

IMPEXP std::vector<Texture *> Texture::textures;
IMPEXP std::map<std::string, cvrIndex, string_less> Texture::nameref;

IMPEXP TextureInit Texture::initializer = TextureInit();

#ifdef OPENGL_ES
	#define TEXTURE_MAX 32
#else
#ifdef ARCH_PSP
	#define TEXTURE_MAX 8
#else
	#define TEXTURE_MAX 32
#endif
#endif


IMPEXP int Texture::tex_use = -1;
IMPEXP int Texture::tex_lock = -1;


#ifdef ARCH_DC
#include <pcx/pcx.h>
/* Load a texture using pcx_load_texture and glKosTex2D */
void loadtxr(const char *fn, GLuint *txr) {
	kos_img_t img;
	pvr_ptr_t txaddr;
	
	if (pcx_to_img(fn, &img) < 0) {
		Logger::log(LOG_ERROR,"can't load %s\n", fn);
		return;
	}
	
	txaddr = pvr_mem_malloc(img.w * img.h * 2);
	pvr_txr_load_kimg(&img, txaddr, PVR_TXRLOAD_INVERT_Y);
	kos_img_free(&img, 0);
	
	glKosTex2D(GL_RGB565_TWID, img.w, img.h, txaddr);
}
#endif



Texture::Texture()
{
#ifndef ARCH_PSP
	glTexId = 0;
#endif
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
	render_srf = false;
#endif
};



Texture::~Texture()
{
#ifndef ARCH_PSP
	if (glTexId) 
	{
		glDeleteTextures(1, (GLuint *)&glTexId);
	}
#endif
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (render_srf)
	{
		destroyRenderTarget();
	}
#endif
};

cvrIndex Texture::create()
{
	if (textures.empty())	/* generate the 'null' material */
	{
		 textures.push_back(new Texture());

		 nameref["null"] = 0;  // this crashes MSVC++, wtf?
	}
	textures.push_back(new Texture());
	return textures.size()-1;
};


cvrIndex Texture::create(std::string file_name)
{
	create();
	textures.back()->load(file_name);
	return textures.size()-1;
};


cvrIndex Texture::create(std::string file_name, std::string tex_name)
{
	cvrIndex i = create(file_name);
	nameref[tex_name] = i;
	return i;
};


cvrIndex Texture::bindTexture(Texture *tex_in, std::string tex_name) 
{ 
	if (textures.empty())	/* generate the 'null' material */
	{
		textures.push_back(new Texture());
		
		nameref["null"] = 0;  // this crashes MSVC++, wtf?
	}
	
	textures.push_back(tex_in);

	cvrIndex i = textures.size()-1;
	
	if (tex_name != "")
	{
		nameref[tex_name] = i;
	}
	
	return i;
};


void Texture::destroy(cvrIndex texNum)
{
	if (textures[texNum]) delete textures[texNum];
	textures[texNum] = NULL;
};


cvrIndex Texture::getTextureId(std::string tex_name)
{
	return nameref[tex_name];
};
	
	
void Texture::use(cvrIndex texNum)
{
	if (textures[texNum]) textures[texNum]->use();
};


void Texture::generate()
{
#ifndef ARCH_PSP
	if (!glTexId)	glGenTextures(1, &glTexId);
	glBindTexture(GL_TEXTURE_2D, glTexId);
#endif
}


bool Texture::load(std::string strFn)
{
	setId(strFn);
	//Create texture
	//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);	
#ifdef ARCH_PSP
	const char *filename = strFn.c_str();
	
	int filenameLength=strlen(filename);
   
	if(	strncmp((filename+filenameLength-3), "TGA", 3)==0 ||
	   strncmp((filename+filenameLength-3), "tga", 3)==0)
		{
   			Logger::log("[Texture:%s] Loading....",strFn.c_str());
   
   			if( !psp_texture.LoadTGA((char *)strFn.c_str()) )
   			{
   				Logger::log(LOG_ERROR,"failed!\n");
   				//sceKernelExitGame();
				return false;
   			}
   			else
   			{
   				Logger::log("ok.");
				return true;
   			}
   			psp_texture.Swizzle();
   			Logger::log("\n");
	}

#else
	bool isJpgTexture = false;
	
	if(	strncmp((strFn.c_str()+strFn.length()-3), "JPG", 3)==0 ||
	   strncmp((strFn.c_str()+strFn.length()-3), "jpg", 3)==0)
		isJpgTexture = true;

	bool imgLoaded = false;
		
#ifdef ARCH_DC

	const char *filename = strFn.c_str();
	
	int filenameLength=strlen(filename);
	
	if(	strncmp((filename+filenameLength-3), "PCX", 3)==0 ||
	   strncmp((filename+filenameLength-3), "pcx", 3)==0)
		{
			Logger::log("Loading PCX using dreamcast loader");
			generate();
			
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_FILTER, GL_FILTER_BILINEAR);
			loadtxr(filename, &glTexId);
			return true;
		}
#endif
	
	imgLoaded = img.Load((char *)strFn.c_str());
	
	if (imgLoaded)
	{
		generate();
		glEnable(GL_TEXTURE_2D);

#ifdef OPENGL_ES
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif
#ifdef ARCH_DC
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
		
#if !defined(ARCH_DC) && !defined(OPENGL_ES) && !defined(ARCH_PSP)
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#endif

#ifndef ARCH_DC
		if(isJpgTexture)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif

		format = img.format;
		alpha = (format==GL_RGBA)?true:false;
		
		glTexImage2D(GL_TEXTURE_2D, 0, img.format, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);	

#ifndef ARCH_DC
		if(isJpgTexture)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#endif


		return true;
	}
	else
	{
		return false;
	}
#endif
//	char c = toupper(strFn[strFn.length()-3]);
//			
//	if (c == 'T')
//	{
//		load(strFn,FORMAT_TGA);
//	}
//	else 
//	if (c == 'P')
//	{
//		load(strFn,FORMAT_PNG);
//	}
}

#ifndef ARCH_PSP
#ifndef ARCH_DC
// todo: lightmap support for PSP and ES
bool Texture::loadMipmap(std::string strFn)
{
	bool isJpgTexture = false;

	if(	strncmp((strFn.c_str()+strFn.length()-3), "JPG", 3)==0 ||
	   strncmp((strFn.c_str()+strFn.length()-3), "jpg", 3)==0)
		isJpgTexture = true;
	
	//if a jpg texture, need to set UNPACK_ALIGNMENT to 1
		
	if (img.Load((char *)strFn.c_str()))
	{
		generate();
		glEnable(GL_TEXTURE_2D);
		
#ifdef OPENGL_ES
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
#else
		//Set Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif

#ifndef ARCH_DC
		if(isJpgTexture)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif

#ifdef OPENGL_ES
		//Create texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);	
#else
		gluBuild2DMipmaps(	GL_TEXTURE_2D, GL_RGBA8, img.width, img.height,
						  img.format, GL_UNSIGNED_BYTE, img.data);
#endif
		
#ifndef ARCH_DC
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#endif
		return true;
	}
	else
	{
		return false;
	}
	//	char c = toupper(strFn[strFn.length()-3]);
	//			
	//	if (c == 'T')
	//	{
	//		load(strFn,FORMAT_TGA);
	//	}
	//	else 
	//	if (c == 'P')
	//	{
	//		load(strFn,FORMAT_PNG);
	//	}
}



void Texture::loadLightMap(GLubyte *data, int width, int height)
{
		generate();
		//Create texture
	
#ifdef OPENGL_ES
	//Create texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	
#else
	gluBuild2DMipmaps(	GL_TEXTURE_2D, GL_RGBA8, width, height,
					  GL_RGB, GL_UNSIGNED_BYTE, data);
#endif

		//Set Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
}

#endif
#endif

void Texture::load(std::string strFn,int imgType)
{
#ifndef ARCH_PSP
//	bool success = false;

	if (!glTexId)	glGenTextures(1, (GLuint *)&glTexId);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glTexId);
#endif

	switch (imgType)
	{
//#ifdef ARCH_PSP
//		case FORMAT_TGA:
//
//			Logger::log("[Texture:%s] Loading....",strFn.c_str());
//						
//			if( !psp_texture.LoadTGA((char *)strFn.c_str()) )
//			{
//				Logger::log("failed!\n");
//				//sceKernelExitGame();
//			}
//			else
//			{
//				Logger::log("ok.");
//			}
//			psp_texture.Swizzle();
//			Logger::log("\n");
//		break;
//		case FORMAT_PNG:
//			Logger::log("PNG Loading disabled, texture load failed.\n");
//		break;
//#else
//		case FORMAT_TGA:
//				
//				TextureImage tex;
//								
//				success = loadTGA(&tex, strFn.c_str());
//
//				if (success)
//				{
//#ifdef OPENGL_ES
//					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//#else
//					glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
//					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//#endif
//
//					glTexImage2D(GL_TEXTURE_2D, 0, tex.type, tex.width, tex.height, 0, tex.type, GL_UNSIGNED_BYTE, tex.imageData);
//
//					width = tex.width;
//					height = tex.height;
//					std::free(tex.imageData);	
//				}
//				else
//				{
//					std::cout << "Texture::error, could not find " << strFn << std::endl;
//				}
//		break;
//		
//		
//		case FORMAT_PNG:
//			LodePNG::Decoder pngDecoder;
//			std::vector<unsigned char> buffer;
//
//			Logger::log("Loading png file %s..\n",strFn.c_str());
//			
//			LodePNG::loadFile(buffer,strFn);
//			
//			Logger::log("Calling PNG decoder..\n");
//			pngDecoder.decode(image, buffer);
//
//			Logger::log("Checking decoder state..\n");
//			if(pngDecoder.hasError())
//			{
//				Logger::log("[loadTextureFromPng] Error %d", pngDecoder.getError());
//				Logger::log(" while trying to load \'%s\'",strFn.c_str());
//				return;
//			}
//
//			int pngwidth = pngDecoder.getWidth();
//			int pngheight = pngDecoder.getHeight();
//			
//			Logger::log("PNG Decoder Info [ width: %d, height: %d ]..\n",pngwidth,pngheight);
//			
//			std::vector<unsigned char>::iterator iStart, iEnd, iTarget;
//
//			Logger::log("Swapping ranges");
//			
//			for (int i = 0; i < pngheight / 2; i++)
//			{
//				iStart = image.begin() + (i *  (pngwidth * 4));
//				iEnd = iStart + (pngwidth * 4);
//				iTarget = image.begin() + (((pngheight - 1) - i) *  (pngwidth * 4));
//				swap_ranges(iStart, iEnd, iTarget);
//				Logger::log(".");
//			}
//			
//			Logger::log("\n");
//			
//			width=pngwidth;
//			height=pngheight;
//#ifdef OPENGL_ES
//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//#else
//			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//#endif
//	
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
//
//		break;
//#endif
	}
};


void Texture::use()
{
#ifdef ARCH_PSP
	sceGuEnable( GU_TEXTURE_2D );
	sceGuTexFilter( GU_LINEAR, GU_LINEAR );	
	sceGuTexMode( GU_PSM_8888, 0, 0, psp_texture.Swizzled() );	
 	sceGuTexImage( 0, psp_texture.Width(), psp_texture.Height(), psp_texture.Width(), psp_texture.Image() );
	sceGuTexFunc( GU_TFX_MODULATE, GU_TCC_RGB );
	sceGuTexScale( 1.0f, 1.0f );                    // No scaling
	sceGuTexOffset( 0.0f, 0.0f );		
#else
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,glTexId);
#endif
};


void Texture::clear()
{
	std::vector<Texture *>::iterator i;
	
	for (i = Texture::textures.begin(); i != Texture::textures.end(); i++)
	{
		if ((*i)) delete (*i);
	}
	
	textures.clear();
		
	tex_use = 0;
	tex_lock = 0;
}

TextureInit::TextureInit() {};

TextureInit::~TextureInit()
{
	Texture::clear();
};



void Texture::setTexture(int tex_index)
{
#ifdef ARCH_PSP
#warning ARCH_PSP: need glActiveTexture replacement
#endif
	
#ifndef ARCH_PSP
#ifndef ARCH_DC
	glActiveTexture(GL_TEXTURE0+tex_index);		
	glClientActiveTexture(GL_TEXTURE0+tex_index);
	glEnable(GL_TEXTURE_2D);
#endif
#endif
	
	if (tex_index > tex_use) tex_use = tex_index;
}


int Texture::nextTexture()
{
	tex_use++;
	
	setTexture(tex_use);
	
	return tex_use;
}


void Texture::clearAllTextures()
{
#ifndef ARCH_PSP
#ifndef ARCH_DC
	for (int i = tex_lock+1; i <= tex_use; i++)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glDisable(GL_TEXTURE_2D);
		glClientActiveTexture(GL_TEXTURE0+i);		
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//		glMatrixMode(GL_TEXTURE);
//		glLoadIdentity();
#ifndef OPENGL_ES
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
#endif
	}
#endif
#endif
	tex_use = tex_lock;
}



void Texture::lock()
{
	tex_lock = tex_use;
}

void Texture::clearLocks()
{
	tex_lock = -1;
}



#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
void Texture::destroyRenderTarget()
{
	phDestroySurface(&renderSurface);
	glTexId = 0;
}

bool Texture::createRenderTarget(unsigned int width, unsigned int height, GLboolean depth, int fp, GLboolean linear, GLboolean stencil, int attachments)
{
	render_srf = true;
	
	renderSurface.width = width;
	renderSurface.height = height;
	
	phCreateSurface(&renderSurface,depth,fp,linear,stencil,attachments);
	phCheckFBO();

	glTexId = renderSurface.texture;

	use();
	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	

	
	return true;
}

void Texture::bindRenderTarget()
{
	if (!render_srf) return;
	
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderSurface.fbo);
	
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
	glDrawBuffers(renderSurface.numAttachments, buffers);
	
}

void Texture::resetRenderTarget()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
#endif
