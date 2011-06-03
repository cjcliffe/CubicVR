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

#include <CubicVR/Font.h>
#include <cstring>

Font::Font(int width_in, int height_in, unsigned char *font_in)
{
	int i;
	
	fontlist = BitmapArray(255);
		
	draw_width = width = width_in;
	draw_height = height = height_in;

	for (i = 0; i < 255; i++)
	{
		fontlist.add(Bitmap::create(width_in, height_in, font_in+i*width*height));
	}
}


Font::Font()
{
	Font(8,8, (unsigned char *)systemfont);
}

void Font::string(int xpos, int ypos, char *str_in)
{
	int len = strlen(str_in);

	for (int i = 0; i < len; i++)
	{
		fontlist.draw(str_in[i], xpos + (i * draw_width), ypos);
	}
}


void Font::string(int xpos, int ypos, const std::string &str_in)
{
	string(xpos,ypos,str_in.c_str());
}

void Font::stringSpecialShadow(int xpos, int ypos, char *str_in)
{
	glColor3f(0.5f,0.5f,0.5f);

	for(int y = -2; y <= 2; y++)
		for(int x = -2; x <= 2; x++)
		{
			Font::string(xpos+x, ypos+y, str_in);
		}

	glColor3f(1.0f,1.0f,1.0f);
	Font::string(xpos, ypos, str_in);
}

void Font::stringSpecialShadow(int xpos, int ypos, const std::string &str_in)
{
	Font::stringSpecialShadow(xpos,ypos,str_in.c_str());	
}

void Font::stringShadow(int xpos, int ypos, char *str_in)
{
	glColor3f(0.0f,0.0f,0.0f);
	Font::string(xpos+2, ypos-2, str_in);
	glColor3f(1.0f,1.0f,1.0f);
	Font::string(xpos, ypos, str_in);
}


void Font::stringShadow(int xpos, int ypos, const std::string &str_in)
{
	Font::stringShadow(xpos,ypos,str_in.c_str());	
}


int Font::getHeight()
{
	return draw_height;
}


int Font::getWidth()
{
	return draw_width;
}


int Font::hPixelToChar(int pixelh)
{
	return pixelh/height;
}


int Font::wPixelToChar(int pixelw)
{
	return pixelw/width;
}
