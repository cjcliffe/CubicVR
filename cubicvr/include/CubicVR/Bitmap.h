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

#ifndef CUBICVR_BITMAP_H
#define CUBICVR_BITMAP_H

/*	
	bitmap.h -- bitmap and bitmapArray class

	The bitmap class is used to manage 2D bitmaps for fonts, sprites, etc

	TODO: change this to STL, perhaps drop this class for a better alternative
	TODO: doxygen commenting
  */

/* single color bitmaps */
class Bitmap
{
private:
public:

	int width, height;
	unsigned char *bitmapdata;
	
	void set(int width_in, int height_in, unsigned char *bitmapdata_in);
	static Bitmap create(int width, int height, unsigned char *bitmapdata_in);
	void draw(int xpos, int ypos);
};


class BitmapArray
{
private:
public:

	int indexsize;
	int currentindex;
	Bitmap *bitmapdata;

	BitmapArray(int indexsize_in);
	BitmapArray(void);

	void add(Bitmap bitmap_in);
	void addTo(Bitmap bitmap_in, int index);
	

	void removelast(void);
	void removeFromn(int index);

	void destroy(void);

	void draw(int index, int xpos, int ypos);
};
	

#endif
