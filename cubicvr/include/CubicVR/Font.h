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

#ifndef CUBICVR_FONT_H
#define CUBICVR_FONT_H

/*

	font.h -- Font class

	The font class is for providing a quick way to use an array of bitmaps.

	TODO: convert this to use STL, perhaps drop this class for a better alternative

  */

#include <CubicVR/cvr_defines.h>
#include <CubicVR/Bitmap.h>
#include <CubicVR/systemfont.h>

#include <string>

class IMPEXP Font
{
private:
public:

	int width, height, draw_width, draw_height;
	BitmapArray fontlist;

	Font(int width_in, int height_in, unsigned char *font_in);
	Font();

	void string(int xpos, int ypos, char *str_in);
	void stringShadow(int xpos, int ypos, char *str_in);
	void stringSpecialShadow(int xpos, int ypos, char *str_in);
	void string(int xpos, int ypos, const std::string &str_in);
	void stringShadow(int xpos, int ypos, const std::string &str_in);
	void stringSpecialShadow(int xpos, int ypos, const std::string &str_in);

	int hPixelToChar(int pixelh);
	int wPixelToChar(int pixelw);
	
	int getHeight();
	int getWidth();
};



#endif
