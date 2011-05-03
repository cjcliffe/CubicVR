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

#include <CubicVR/ProceduralTexture.h>
#include <CubicVR/lodepng.h>
#include <CubicVR/Logger.h>

ProceduralTexture::ProceduralTexture(ProceduralTextureFormat fmt)
{
	format=fmt;
}

ProceduralTexture::~ProceduralTexture()
{
}

void ProceduralTexture::init(int w_in,int h_in)
{
	width = w_in;
	height = h_in;
	generateBuffer();
}

void ProceduralTexture::generateBuffer()
{	
	image.resize(width*height*getBPP(),0);
}

unsigned short ProceduralTexture::getBPP()
{
	switch (format)
	{
		case FORMAT_RGB: return 3;
		case FORMAT_RGBA: return 4;
		case FORMAT_SPAT: return 4;
	}	
	
	return 0;
}

void ProceduralTexture::clear(const RGB &color)
{
	int w = width;
	int h = height;
	unsigned short bpp = getBPP();

	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++)
		{
			image[bpp * w * y + bpp * x + 0] = (unsigned char)(color.r/255.0f);
			if (bpp > 1) image[bpp * w * y + bpp * x + 1] = (unsigned char)(color.g/255.0f);
			if (bpp > 2) image[bpp * w * y + bpp * x + 2] = (unsigned char)(color.b/255.0f);			
			if (bpp > 3) image[bpp * w * y + bpp * x + 3] = 255;				
		}
}


unsigned int ProceduralTexture::multiSample(int x,int y, unsigned short bpp, unsigned short channel,bool tile_mode)
{
	int w = width;
	int h = height;			

	unsigned int r;
	unsigned int divs=1;
			
	if (tile_mode)
	{
		while (x >= w) x-=w;
		while (x < 0) x+=w;
		while (y >= h) y-=h;
		while (y < 0) y+=h;		
	}
	else
	{
		if (x < 0) return 0;
		if (x >= w) return 0;
		if (y < 0) return 0;
		if (y >= h) return 0;
	}
	
	r = image[bpp * w * y + bpp * x + channel];
				
	if (x != 0) 
	{
		r += image[bpp * w * y + bpp * (x-1) + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * y + bpp * (w-1) + channel];
		divs++;
	}
	
	if (y != 0)
	{
		r += image[bpp * w * (y-1) + bpp * x + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * (h-1) + bpp * x + channel];
		divs++;
	}
	
	if (y != 0 && x != 0) 
	{
		r += image[bpp * w * (y-1) + bpp * (x-1) + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * (((y-1)<0)?(h-1):(y-1)) + bpp * (((x-1)<0)?(w-1):(x-1)) + channel];
		divs++;
	}
	
	if (x != 0 && y != h-1) 
	{
		r += image[bpp * w * (y+1) + bpp * (x-1) + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * (((y+1)<h)?(y+1):(0)) + bpp * (((x-1)<0)?(w-1):(x-1)) + channel];
		divs++;
	}
	
	if (y != 0 && x != w-1) 
	{
		r += image[bpp * w * (y-1) + bpp * (x+1) + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * (((y-1)<0)?(h-1):(y-1)) + bpp * (((x+1)<w)?(x+1):(0)) + channel];
		divs++;
	}
	
	if (x != w-1) 
	{
		r += image[bpp * w * y + bpp * (x+1) + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * y + bpp * (0) + channel];
		divs++;
	}
	
	if (y != h-1) 
	{
		r += image[bpp * w * (y+1) + bpp * x + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * (0) + bpp * x + channel];
		divs++;
	}
	
	if (x != w-1 && y != h-1) 
	{
		r += image[bpp * w * (y+1) + bpp * (x+1) + channel];
		divs++;
	}
	else if (tile_mode)
	{
		r += image[bpp * w * (((y+1)<h)?(y+1):(0)) + bpp * (((x+1)<w)?(x+1):(0)) + channel];
		divs++;
	}
	
	r /= divs;

	return r;
}

void ProceduralTexture::blur(bool tile_mode)
{
	int w = width;
	int h = height;			
	
	std::vector<unsigned char> blur_buf;
	
	blur_buf.resize(image.size());
	
	unsigned short bpp = getBPP();
	unsigned int v;

	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++)
		{
			for (int p = 0; p < bpp; p++)
			{
				v = multiSample(x,y,bpp,p,tile_mode);
				blur_buf[bpp * w * y + bpp * x + p] = (unsigned char)v;
			}
			
		}
	
	memcpy(&image[0],&blur_buf[0],image.size());
}


void ProceduralTexture::brush(unsigned int size, unsigned int inner, int xpos, int ypos, const RGBA &color, ProceduralTextureBlendMode blend_mode, bool tile_mode)
{
	int w = width;
	int h = height;
	
	int dx, dy;
	float d,f;
	
	int xstart, ystart, xend, yend;
	
	int r,g,b,a;
	int j,k,l,m;

	xstart = xpos-size;
	ystart = ypos-size;
	xend = xpos+size;
	yend = ypos+size;
	
	if (!tile_mode)
	{
		if (xstart < 0) xstart = 0;
		if (xend > w-1) xend = w-1;
		if (ystart < 0) ystart = 0;
		if (yend > h-1) yend = h-1;
	}
	
	if (inner > size) inner=size;
	
	
	int x,y;
	
//	printf("brush color: %d, %d, %d\n",(int)(color.r*255.0),(int)(color.g*255.0),(int)(color.b*255.0));		   
	
	for(int ym = ystart; ym <= yend; ym++)
		for(int xm = xstart; xm <= xend; xm++)
		{
			if (tile_mode)
			{
				x = xm;
				y = ym;
				
				while (y > h-1) y-=h;
				while (x > w-1) x-=w;
				while (y < 0) y+=h;
				while (x < 0) x+=w;
			}
			else
			{
				x = xm;
				y = ym;
			}
			
			dx = xm-xpos;
			dy = ym-ypos;
			d = sqrt(dx*dx+dy*dy);
			
			if (d<=size)
			{
				if (d <= (float)inner)
				{
					f = 1.0f;
				}
				else
				{
					f = 1.0f-(d-(float)inner)/((float)size-(float)inner);
				}
				
				if (f>1.0f) f = 1.0f;
				if (f<0.0f) f = 0.0f;
				
				
				j = image[4 * w * y + 4 * x + 0];
				k = image[4 * w * y + 4 * x + 1];
				l = image[4 * w * y + 4 * x + 2];
				m = image[4 * w * y + 4 * x + 3];
				
				switch (blend_mode)
				{
					case BLEND_NORMAL:
						r = (int)((color.r*f*255.0)+((float)j)*(1.0f-f));
						g = (int)((color.g*f*255.0)+((float)k)*(1.0f-f));
						b = (int)((color.b*f*255.0)+((float)l)*(1.0f-f));
						a = (int)((color.a*f*255.0)+((float)m)*(1.0f-f));						
						break;
					case BLEND_ADD:
						r = (int)((color.r*f*255.0f)+j);
						g = (int)((color.g*f*255.0f)+k);
						b = (int)((color.b*f*255.0f)+l);
						a = (int)((color.a*f*255.0f)+m);
						break;
					case BLEND_SUBTRACT:
						r = (int)(j-(color.r*f*255.0f));
						g = (int)(k-(color.g*f*255.0f));
						b = (int)(l-(color.b*f*255.0f));
						a = (int)(m-(color.a*f*255.0f));
						break;
				}

				if (r>255) r=255;
				if (g>255) g=255;
				if (b>255) b=255;
				if (a>255) a=255;

				if (r<0) r=0;
				if (g<0) g=0;
				if (b<0) b=0;
				if (a<0) a=0;
				
				image[4 * w * y + 4 * x + 0] = (unsigned char)r;
				image[4 * w * y + 4 * x + 1] = (unsigned char)g;
				image[4 * w * y + 4 * x + 2] = (unsigned char)b;				
				image[4 * w * y + 4 * x + 3] = (unsigned char)a;
			}
		}		
	
}

void ProceduralTexture::imageTest(float t)
{
	int w = width;
	int h = height;
	
	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++)
		{
			//pattern 1
			image[4 * w * y + 4 * x + 0] = (unsigned char)(127 * (1 + sin(t+(                    x * x +                     y * y) / (w * h / 8.0f))));
			image[4 * w * y + 4 * x + 1] = (unsigned char)(127 * (1 + sin(t+((w - x - 1) * (w - x - 1) +                     y * y) / (w * h / 8.0f))));
			image[4 * w * y + 4 * x + 2] = (unsigned char)(127 * (1 + sin(t+(                    x * x + (h - y - 1) * (h - y - 1)) / (w * h / 8.0f))));
			image[4 * w * y + 4 * x + 3] = 255; //(unsigned char)(127 * (1 + sin(((w - x - 1) * (w - x - 1) + (h - y - 1) * (h - y - 1)) / (w * h / 8.0))));
			
			//pattern 2
			//image[4 * w * y + 4 * x + 0] = 255 * !(x & y);
			//image[4 * w * y + 4 * x + 1] = x ^ y;
			//image[4 * w * y + 4 * x + 2] = x | y;
			//image[4 * w * y + 4 * x + 3] = 255;
		}		
}

void ProceduralTexture::commit()
{
	if (!glTexId) generate();
	
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, glTexId);
	
	glEnable(GL_TEXTURE_2D);
	
#ifdef OPENGL_ES
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);	
	
	//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
}


bool ProceduralTexture::loadPNG(char *png_map)
{
	LodePNG::Decoder pngDecoder;
	std::vector<unsigned char> buffer;
	
	Logger::log("Loading png file %s..\n",png_map);
	
	LodePNG::loadFile(buffer,png_map);
	
	Logger::log("Calling PNG decoder..\n");
	pngDecoder.decode(image, buffer);
	
	Logger::log("Checking decoder state..\n");
	if(pngDecoder.hasError())
	{
		Logger::log("[loadTextureFromPng] Error %d\n", pngDecoder.getError());
		Logger::log(" while trying to load \'%s\'\n",png_map);
		return false;
	}
	
	width = pngDecoder.getWidth();
	height = pngDecoder.getHeight();
	
	Logger::log("PNG Decoder Info [ width: %d, height: %d ]..\n",width,height);
	
	Logger::log("image size: %d\n",image.size());
	
	Logger::log("isGreyScale: %s\n",pngDecoder.isGreyscaleType()?"true":"false");
	Logger::log("isAlphaType: %s\n",pngDecoder.isAlphaType()?"true":"false");
	Logger::log("Channels: %d\n",pngDecoder.getChannels());
	Logger::log("BPP: %d\n",pngDecoder.getBpp());
		
	return true;
}

bool ProceduralTexture::savePNG(char *png_map)
{
	std::vector<unsigned char> buffer;
		
	//create encoder and set settings and info (optional)
	LodePNG::Encoder encoder;
	encoder.addText("Comment", "CubicVR Procedural Texture");
	encoder.getSettings().zlibsettings.windowSize = 2048;
	
	encoder.encode(buffer, image.empty() ? 0 : &image[0], width, height);
	
	if(encoder.hasError())
	{
		Logger::log(LOG_ERROR,"[savePng] Error %d\n", encoder.getError());
		return false;
	}
	
	LodePNG::saveFile(buffer, png_map);
	
	return true;
}



double ProceduralTexture::findnoise2(double x,double y)
{
	int n=(int)x+(int)y*57;
	n=(n<<13)^n;
	int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
	return 1.0-((double)nn/1073741824.0);
}


double ProceduralTexture::interpolate1(double a,double b,double x)
{
	double ft=x * 3.1415927;
	double f=(1.0f-cos(ft))* 0.5f;
	return a*(1.0f-f)+b*f;
}

double ProceduralTexture::noise2(double x,double y)
{
	double floorx=(double)((int)x);//This is kinda a cheap way to floor a double integer.
	double floory=(double)((int)y);
	double s,t,u,v;//Integer declaration
	s=findnoise2(floorx,floory);
	t=findnoise2(floorx+1,floory);
	u=findnoise2(floorx,floory+1);//Get the surrounding pixels to calculate the transition.
	v=findnoise2(floorx+1,floory+1);
	double int1=interpolate1(s,t,x-floorx);//Interpolate between the values.
	double int2=interpolate1(u,v,x-floorx);//Here we use x-floorx, to get 1st dimension. Don't mind the x-floorx thingie, it's part of the cosine formula.
	return interpolate1(int1,int2,y-floory);//Here we use y-floory, to get the 2nd dimension.
}

//w and h speak for themselves, zoom wel zoom in and out on it, I usually
//use 75. P stands for persistence, this controls the roughness of the picture, i use 1/2
void ProceduralTexture::clouds(double zoom, double p, int r, int g, int b)
{
	int octaves=3;
	int w = width;
	int h = height;
	unsigned short bpp = getBPP();
	
	for(int y=0;y<h;y++)
	{//Loops to loop trough all the pixels
		for(int x=0;x<w;x++)
		{
			double getnoise =0;
			for(int a=0;a<octaves-1;a++)//This loops trough the octaves.
			{
				double frequency = pow(2,a);//This increases the frequency with every loop of the octave.
				double amplitude = pow(p,a);//This decreases the amplitude with every loop of the octave.
				getnoise += noise2(((double)x)*frequency/zoom,((double)y)/zoom*frequency)*amplitude;//This uses our perlin noise functions. It calculates all our zoom and frequency and amplitude
			}//                                            It gives a decimal value, you know, between the pixels. Like 4.2 or 5.1
		
			int color= (int)((getnoise*128.0)+128.0);//Convert to 0-256 values.
			if(color>255)
				color=255;
			if(color<0)
				color=0;

			image[bpp * w * y + bpp * x + 0] = (int)((r/255.0)*(double)color);
			if (bpp>1) image[bpp * w * y + bpp * x + 1] = (int)((g/255.0)*(double)color);
			if (bpp>2) image[bpp * w * y + bpp * x + 2] = (int)((b/255.0)*(double)color);
			if (bpp>3) image[bpp * w * y + bpp * x + 3] = 255;
		}
	}
}

