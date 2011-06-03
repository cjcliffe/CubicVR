/*
 *  HDR.h
 *  CubicVR
 *  
 *  Based directly on http://www.prideout.net/bloom/ nice work!

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

#include "CubicVR/HDR.h"
#ifdef __linux__
#include <stdio.h>
#endif

#define STRINGIFY(A)  #A

#include "CubicVR/ph/shaders/passv.glsl"
#include "CubicVR/ph/shaders/blitf.glsl"
#include "CubicVR/ph/shaders/combine5f.glsl"
#include "CubicVR/ph/shaders/row3f.glsl"
#include "CubicVR/ph/shaders/lightv.glsl"
#include "CubicVR/ph/shaders/lightf.glsl"
#include "CubicVR/ph/shaders/hipassf.glsl"


GLfloat darkblue[4] = {7, 16, 141, 0};
GLfloat lightblue[4] = {122, 143, 248, 0};
GLfloat brightgold[4] = {5*0.8f, 5*0.498039f, 5*0.196078f, 1.0f};
GLfloat black[4] = {0, 0, 0, 1};
GLfloat white[4] = {1, 1, 1, 1};
GLfloat gray[4] = {0.5f, 0.5f, 0.5f, 1};



HDR::HDR(int width_in, int height_in, int filter_levels_in): initialized(false), blur_step(0), blur_levels(0)
{
	kernel = new float [ 5, 6, 5 ];
	view_width = buffer_width = width_in;
	view_height = buffer_height = height_in;
}


void HDR::setFilterLevel(int filter_level_in)
{
	if (filter_level_in > HDR_FILTER_MAX) filter_level_in = HDR_FILTER_MAX;
	if (filter_level_in < 1) filter_level_in = 1;
	
	if (filter_level_in == filter_levels) return;
	
	if (!initialized)
	{
		filter_levels = filter_level_in;
	}
	else
	{
		clear();
		
		filter_levels = filter_level_in;

		init();
	}
}

void HDR::setBlurLevel(int blur_level_in)
{
	int p;
	
	if (blur_level_in > HDR_BLUR_MAX) blur_level_in = HDR_BLUR_MAX;
	if (blur_level_in < 0) blur_level_in = 0;
	
	if (blur_level_in == blur_levels) return;
	
	if (!initialized)
	{
		blur_levels = blur_level_in;
	}
	else
	{
		if (blur_level_in < blur_levels)
		{			
			for (p = blur_level_in+1; p < blur_levels; p++)
			{
				phDestroySurface(&mblur[p]);
			}
		}
		else
		{
			for (p = blur_levels; p < blur_level_in; p++)
			{
				mblur[p].width = buffer_width;
				mblur[p].height = buffer_height;
				mblur[p].viewport.x = 0;
				mblur[p].viewport.y = 0;
				mblur[p].viewport.width = buffer_width;
				mblur[p].viewport.height = buffer_height;

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glGetFloatv(GL_MODELVIEW_MATRIX, mblur[p].modelview);
				glOrtho(0, window.width, window.height, 0, 0, 10);
				glGetFloatv(GL_MODELVIEW_MATRIX, mblur[p].projection);
				glLoadIdentity();
				phCreateSurface(&mblur[p], GL_TRUE, GL_TRUE, GL_FALSE);
			}
		}
		
		blur_levels = blur_level_in;
	}
}



void HDR::setViewSize(int width_in, int height_in)
{
	if (!initialized) return;
	if (view_width == width_in && view_height == height_in) return;
	
	view_width = buffer_width = width_in;
	view_height = buffer_height = height_in;
	clear();
	init();
}


void HDR::clear()
{
	int p;
	
	if (!initialized) return;
	
	phDestroySurface(&scene);

	for (p = 0; p < filter_levels; p++)
	{
		phDestroySurface(pass0 + p);
	}
	
	for (p = 0; p < filter_levels; p++)
	{
		phDestroySurface(pass1 + p);
	}	
	
	for (p = 0; p < blur_levels; p++)
	{
		phDestroySurface(&mblur[p]);
	}
	
	initialized = false;
}

HDR::~HDR()
{	
	clear();

	delete kernel;
}

void HDR::blur(PHsurface *sources, PHsurface *dests, int count, Direction dir)
{
	GLint loc;
	int p;
	
	// Set up the filter.
	glUseProgram(filterProg);
	loc = glGetUniformLocation(filterProg, "source");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(filterProg, "coefficients");
	glUniform1fv(loc, HDR_KERNEL_SIZE, kernel);
	loc = glGetUniformLocation(filterProg, "offsetx");
	glUniform1f(loc, 0);
	loc = glGetUniformLocation(filterProg, "offsety");
	glUniform1f(loc, 0);
	if (dir == HORIZONTAL)
		loc = glGetUniformLocation(filterProg, "offsetx");
	
	// Perform the blurring.
	for (p = 0; p < count; p++)
	{
		float offset = 1.2f / sources[p].width;
		glUniform1f(loc, offset);
		phBindSurface(dests + p);
		glBindTexture(GL_TEXTURE_2D, sources[p].texture);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(-1, -1);
		glTexCoord2i(1, 0); glVertex2i(1, -1);
		glTexCoord2i(1, 1); glVertex2i(1, 1);
		glTexCoord2i(0, 1); glVertex2i(-1, 1);
		glEnd();
	}
}

void HDR::init()
{
	int p, c;
	float sum;
	GLsizei width;
	GLsizei height;
	
	if (initialized) return;
		
	glMatrixMode(GL_MODELVIEW);
	
	// Compile shaders
	blitProg = phCompile(passv, blitf);
	combineProg = phCompile(passv, combine5f);
	filterProg = phCompile(passv, row3f);
	//		lightProg = phCompile(lightv, lightf);
	hiPassProg = phCompile(passv, hipassf);
	
	// Normalize kernel coefficients
	sum = 0;
	for (c = 0; c < HDR_KERNEL_SIZE; c++)
		sum += kernel[c];
	for (c = 0; c < HDR_KERNEL_SIZE; c++)
		kernel[c] /= sum;
	
	// Normalize colors
	for (c = 0; c < 4; c++)
	{
		darkblue[c] /= 255.0f;
		lightblue[c] /= 255.0f;
	}
	
	// Create Window Surface
	window.fbo = 0;
	window.depth = 0;
	window.width = view_width;
	window.height = view_height;
	window.clearColor[0] = lightblue[0];
	window.clearColor[1] = lightblue[1];
	window.clearColor[2] = lightblue[2];
	window.clearColor[3] = lightblue[3];
	window.viewport.x = 0;
	window.viewport.y = 0;
	window.viewport.width = window.width;
	window.viewport.height = window.height;
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, window.modelview);
	glOrtho(0, window.width, window.height, 0, 0, 10);
	glGetFloatv(GL_MODELVIEW_MATRIX, window.projection);
	glLoadIdentity();
	
	// Create 3D Scene Surface
	width = buffer_width;
	height = buffer_height;

	scene.width = width;
	scene.height = height;
	scene.clearColor[0] = 0;
	scene.clearColor[1] = 0;
	scene.clearColor[2] = 0;
	scene.clearColor[3] = 0;
	scene.viewport.x = 0;
	scene.viewport.y = 0;
	scene.viewport.width = width;
	scene.viewport.height = height;
	glGetFloatv(GL_MODELVIEW_MATRIX, scene.modelview);
	glGetFloatv(GL_MODELVIEW_MATRIX, scene.projection);
	phCreateSurface(&scene, GL_TRUE, GL_TRUE, GL_FALSE);
	
	for (p = 0; p < blur_levels; p++)
	{
		mblur[p].width = width;
		mblur[p].height = height;
		mblur[p].viewport.x = 0;
		mblur[p].viewport.y = 0;
		mblur[p].viewport.width = width;
		mblur[p].viewport.height = height;
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, mblur[p].modelview);
		glOrtho(0, window.width, window.height, 0, 0, 10);
		glGetFloatv(GL_MODELVIEW_MATRIX, mblur[p].projection);
		glLoadIdentity();
		phCreateSurface(&mblur[p], GL_TRUE, GL_TRUE, GL_FALSE);
	}
	
	// Create Pass Surfaces
	for (p = 0; p < HDR_FILTER_COUNT; p++)
	{
		pass0[p].width = width;
		pass0[p].height = height;
		pass0[p].viewport.x = 0;
		pass0[p].viewport.y = 0;
		pass0[p].viewport.width = width;
		pass0[p].viewport.height = height;
		glGetFloatv(GL_MODELVIEW_MATRIX, pass0[p].modelview);
		glGetFloatv(GL_MODELVIEW_MATRIX, pass0[p].projection);
		phCreateSurface(pass0 + p, GL_FALSE, GL_FALSE, GL_TRUE);
		width = width >> 1;
		height = height >> 1;
	}
	width = buffer_width;
	height = buffer_height;
	for (p = 0; p < HDR_FILTER_COUNT; p++)
	{
		pass1[p].width = width;
		pass1[p].height = height;
		pass1[p].viewport.x = 0;
		pass1[p].viewport.y = 0;
		pass1[p].viewport.width = width;
		pass1[p].viewport.height = height;
		glGetFloatv(GL_MODELVIEW_MATRIX, pass1[p].modelview);
		glGetFloatv(GL_MODELVIEW_MATRIX, pass1[p].projection);
		phCreateSurface(pass1 + p, GL_FALSE, GL_FALSE, GL_FALSE);
		width = width >> 1;
		height = height >> 1;
	}
	
	
	
	initialized = true;
}

//	static void computeLightingParams(float theta, PHvec3 *vp, PHvec3 *hhat)
//	{
//		PHvec3 position;
//		PHvec3 look = {0, 0, 0};
//		PHvec3 eye  = {0, 0, 1};
//		
//		position.x = 0;
//		position.y = sinf(theta * PH_PI / 180.0f);
//		position.z = cosf(theta * PH_PI / 180.0f);
//		
//		*vp = phSub(&look, &position);
//		*hhat = phAdd(vp, &eye);
//		phNormalize(hhat);
//	}


void HDR::drawOpen()
{
	phBindSurface(&scene);
}

void HDR::drawClear()
{
	phClearSurface();
}

void HDR::drawClose()
{
	phBindSurface(&window);
}


void HDR::drawBegin()
{
	phBindSurface(&scene);
	phClearSurface();
}



void HDR::drawEnd()
{
	int i,p;
	GLint loc;	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);	
	glPushAttrib (	GL_DEPTH_BUFFER_BIT|GL_VIEWPORT_BIT);

	
	glColor4f(1,1,1,1);
	
	// Hi-pass filter into pass0[0]
	glUseProgram(hiPassProg);
	loc = glGetUniformLocation(hiPassProg, "source");
	glUniform1i(loc, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, scene.texture);
	phBindSurface(pass0);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(-1, -1);
	glTexCoord2i(1, 0); glVertex2i(1, -1);
	glTexCoord2i(1, 1); glVertex2i(1, 1);
	glTexCoord2i(0, 1); glVertex2i(-1, 1);
	glEnd();
	glUseProgram(0);
	
	// Downsample the scene into the source surfaces.
	glBindTexture(GL_TEXTURE_2D, pass0[0].texture);
	for (p = 1; p < HDR_FILTER_COUNT; p++)
	{
		phBindSurface(pass0 + p);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(-1, -1);
		glTexCoord2i(1, 0); glVertex2i(1, -1);
		glTexCoord2i(1, 1); glVertex2i(1, 1);
		glTexCoord2i(0, 1); glVertex2i(-1, 1);
		glEnd();
	}
	
	// Perform the horizontal blurring pass.
	blur(pass0, pass1, HDR_FILTER_COUNT, HORIZONTAL);
		
	// Perform the vertical blurring pass.
	blur(pass1, pass0, HDR_FILTER_COUNT, VERTICAL);
	
	glUseProgram(blitProg);
	
	glUseProgram(combineProg);
	
	for (p = 0; p < HDR_FILTER_COUNT; p++)
	{
		char name[] = "Pass#";
		
		glActiveTexture(GL_TEXTURE0 + p);
		glBindTexture(GL_TEXTURE_2D, pass0[p].texture);
		glEnable(GL_TEXTURE_2D);
		
		sprintf(name, "Pass%d", p);
		loc = glGetUniformLocation(combineProg, name);
		glUniform1i(loc, p);
	}
	
	glActiveTexture(GL_TEXTURE0 + HDR_FILTER_COUNT);
	glBindTexture(GL_TEXTURE_2D, scene.texture);
	glEnable(GL_TEXTURE_2D);
	loc = glGetUniformLocation(combineProg, "Scene");
	glUniform1i(loc, HDR_FILTER_COUNT);
	
	
	if (!blur_levels)
	{		
		phBindSurface(&window);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex2i(0, 0);
		glTexCoord2i(1, 1); glVertex2i(view_width, 0);
		glTexCoord2i(1, 0); glVertex2i(view_width, view_height);
		glTexCoord2i(0, 0); glVertex2i(0, view_height);
		glEnd();
	}
	else
	{	
		phBindSurface(&mblur[blur_step]);
		blur_step++;
		if (blur_step >= blur_levels) blur_step = 0;
		
		glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex2i(0, 0);
		glTexCoord2i(1, 1); glVertex2i(view_width, 0);
		glTexCoord2i(1, 0); glVertex2i(view_width, view_height);
		glTexCoord2i(0, 0); glVertex2i(0, view_height);
		glEnd();
	}
		
	phBindSurface(&window);
	
	glUseProgram(0);
	for (p = 0; p < HDR_FILTER_COUNT; p++)
	{
		glActiveTexture(GL_TEXTURE0 + p);
		glDisable(GL_TEXTURE_2D);
	}
	glActiveTexture(GL_TEXTURE0 + HDR_FILTER_COUNT);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);


	/////////////////////////////////
	if (blur_levels)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR); // *
//		glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);
//		glBlendFunc(GL_DST_COLOR,GL_ONE_MINUS_SRC_COLOR);
		
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		for (i = 0; i < blur_levels; i++)
		{		
			p = ((i+blur_step-1)+blur_levels) % (blur_levels);
			
//			if (i == blur_levels-1) glBlendFunc(GL_SRC_COLOR,GL_ONE);

			
//			float blurStep = 0.25f;
//			float extValW = (((float)view_width*blurStep)/(float)i);
//			float extValH = (((float)view_height*blurStep)/(float)i);

			
//			glBindTexture(GL_TEXTURE_2D,mblur[p].texture);
//			glBegin(GL_QUADS);
//			glTexCoord2i(0, 1); glVertex2i(-extValW, -extValH);
//			glTexCoord2i(1, 1); glVertex2i(view_width+extValW, -extValW);
//			glTexCoord2i(1, 0); glVertex2i(view_width+extValW, view_height+extValW);
//			glTexCoord2i(0, 0); glVertex2i(-extValW, view_height+extValW);
//			glEnd();
			
			
			glBindTexture(GL_TEXTURE_2D,mblur[p].texture);
			glBegin(GL_QUADS);
			glTexCoord2i(0, 1); glVertex2i(0, 0);
			glTexCoord2i(1, 1); glVertex2i(view_width, 0);
			glTexCoord2i(1, 0); glVertex2i(view_width, view_height);
			glTexCoord2i(0, 0); glVertex2i(0, view_height);
			glEnd();
		}
		
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ZERO);
	}
		
//	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, scene.fbo);
//	phBindSurface(&scene);

//	for (p = 0; p < 1; p++)
//	{
//		glBindTexture(GL_TEXTURE_2D,mblur[p].texture);
//		glCopyTexSubImage2D( mblur[p].texture,
//							 0,
//							 0,
//							 0,
//							 0,
//							 0,
//							 mblur[p].width,
//							 mblur[p].height );
//		
//		// Hi-pass filter into pass0[0]
//		glUseProgram(hiPassProg);
//		loc = glGetUniformLocation(hiPassProg, "source");
//		glUniform1i(loc, 0);
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, scene.texture);
//		phBindSurface(pass0);
//		glBegin(GL_QUADS);
//		glTexCoord2i(0, 0); glVertex2i(-1, -1);
//		glTexCoord2i(1, 0); glVertex2i(1, -1);
//		glTexCoord2i(1, 1); glVertex2i(1, 1);
//		glTexCoord2i(0, 1); glVertex2i(-1, 1);
//		glEnd();
//		glUseProgram(0);	
//	}

	
//	phBindSurface(&window);

//	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);

	
	glPopAttrib();
}
