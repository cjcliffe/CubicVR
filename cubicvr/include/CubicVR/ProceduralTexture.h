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

#pragma once

#include <CubicVR/GLExt.h>
#include <CubicVR/RGBA.h>
#include <CubicVR/Texture.h>
#include <vector>

enum ProceduralTextureBlendMode
{
	BLEND_NORMAL,
	BLEND_ADD,
	BLEND_SUBTRACT,
	//	MULTIPLY,
	//	SCREEN,
	//	LIGHT
};

enum ProceduralTextureFormat
{
	FORMAT_RGB,
	FORMAT_RGBA,
	FORMAT_SPAT
};

class ProceduralTexture: public Texture
{
private:
protected:
	std::vector<unsigned char> image;
	ProceduralTextureFormat format;
	
	// noise functions adapted from http://www.dreamincode.net/forums/showtopic66480.htm
	double findnoise2(double x,double y);
	double interpolate1(double a,double b,double x);
	double noise2(double x,double y);
	
public:	
	ProceduralTexture(ProceduralTextureFormat fmt=FORMAT_RGBA);
	~ProceduralTexture();
	
	void init(int w_in,int h_in);		
	void generateBuffer();		
	void clear(const RGB &color=RGB(0,0,0));		
	void blur(bool tile_mode=false);
	unsigned short getBPP();
	unsigned int multiSample(int x,int y, unsigned short bpp, unsigned short channel,bool tile_mode);

	
	void brush(unsigned int size, unsigned int inner, int xpos, int ypos, const RGBA &color, ProceduralTextureBlendMode blend_mode=BLEND_NORMAL, bool tile_mode=false);		
	void imageTest(float t=0.0);		
	void commit();		
	
	void clouds(double zoom, double p, int r, int g, int b);
	
	bool loadPNG(char *png_map);
	bool savePNG(char *png_map);
};
