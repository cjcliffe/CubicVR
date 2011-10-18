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

/* 2D Bitmap class for Font class*/
#include <CubicVR/Bitmap.h>
#include <CubicVR/cvr_defines.h>
#include <CubicVR/GLExt.h>

#if defined(__linux__) || defined(_PSP)
#include <cstdlib>
#include <string.h>
#endif

/* Bitmap class */
void Bitmap::set(int width_in, int height_in, unsigned char *bitmapdata_in)
{
	width = width_in;
	height = height_in;
	bitmapdata = bitmapdata_in;

}


/* width and height must be divisible by 8 */
Bitmap Bitmap::create(int width, int height, unsigned char *bitmapdata_in)
{
	Bitmap t_bitmap;
	int i, j, k;
	unsigned long q;

	//	char bit_list[8] = {128, 64, 32, 16, 8, 4, 2, 1};
	
	unsigned long *bitmapdata_t;


	t_bitmap.width = width;
	t_bitmap.height = height;

	t_bitmap.bitmapdata = (unsigned char *)std::malloc(sizeof(unsigned long)*height);
	bitmapdata_t = (unsigned long *)std::malloc(sizeof(unsigned long)*height);
	

	memset(bitmapdata_t,0,sizeof(unsigned long)*height);

	for (j = 0; j < height; j++)
		for (i = 0; i < (width/8); i++)
		{
			q = 1;
			for (k = 0; k < width; k++)
			{
				if (*(bitmapdata_in+(j*width+width-k))) 
					*(bitmapdata_t+((height-1)-j)) += q;
				q *= 2;
			}
		}
	

		memcpy(t_bitmap.bitmapdata,bitmapdata_t,sizeof(unsigned long)*height);
		
		return t_bitmap;
}


void Bitmap::draw(int xpos, int ypos)
{
#ifndef OPENGL_ES
#ifndef ARCH_PSP
	glPixelTransferi(GL_UNPACK_ALIGNMENT, 4);
	glRasterPos2i(xpos, ypos);
	glBitmap(width, height, 0, 0, 0, 0, bitmapdata);
#else
#warning PSP Architecture -- need pixel transfer / texture load equivalent
#endif
#endif
#ifdef OPENGL_ES
#warning no pixel transfer for drawing bitmaps
#endif
}




/* BitmapArray class */

BitmapArray::BitmapArray(int indexsize_in)
{

	indexsize = indexsize_in;
	bitmapdata = (Bitmap *)std::malloc(sizeof(Bitmap)*indexsize);
	currentindex = 0;

}


BitmapArray::BitmapArray(void)
{
	BitmapArray(255);
}

void BitmapArray::add(Bitmap bitmap_in)
{
	if (currentindex < indexsize)
	{
		bitmapdata[currentindex] = bitmap_in;
		currentindex++;
	}
	

}



void BitmapArray::addTo(Bitmap bitmap_in, int index)
{

	if ((index < indexsize) && (index >= 0))
	{
		bitmapdata[currentindex] = bitmap_in;
		currentindex++;
	}

}



void BitmapArray::removelast(void)
{
	if(currentindex > 0)
	{
		std::free(bitmapdata[currentindex].bitmapdata);
		currentindex--;
	}
}



void BitmapArray::removeFromn(int index)
{
	if((index >= 0) && (index < indexsize))
	{
		std::free(bitmapdata[currentindex].bitmapdata);
		currentindex--;
	}
}


void BitmapArray::destroy(void)
{
	std::free(bitmapdata);
	indexsize = 0;

}


void BitmapArray::draw(int index, int xpos, int ypos)
{

	bitmapdata[index].draw(xpos,ypos);
}
