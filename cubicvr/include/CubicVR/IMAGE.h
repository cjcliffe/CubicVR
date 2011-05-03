//////////////////////////////////////////////////////////////////////////////////////////
//	IMAGE.h
//	functions for images to be turned to textures
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt
//////////////////////////////////////////////////////////////////////////////////////////	

/*

 Charles J. Cliffe ChangeLog:
 
	- Upgraded to include PNG and JPG support
*/

#pragma once

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef long LONG;

#ifndef _WIN32
typedef struct tagBITMAPFILEHEADER { 
	WORD    bfType; 
	DWORD   bfSize; 
	WORD    bfReserved1; 
	WORD    bfReserved2; 
	DWORD   bfOffBits; 
} BITMAPFILEHEADER, *PBITMAPFILEHEADER; 

typedef struct tagBITMAPINFOHEADER{
	DWORD  biSize; 
	LONG   biWidth; 
	LONG   biHeight; 
	WORD   biPlanes; 
	WORD   biBitCount; 
	DWORD  biCompression; 
	DWORD  biSizeImage; 
	LONG   biXPelsPerMeter; 
	LONG   biYPelsPerMeter; 
	DWORD  biClrUsed; 
	DWORD  biClrImportant; 
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#endif

#include <stdio.h>
#include <CubicVR/lodepng.h>

class IMAGE
{
public:
	IMAGE() : data(NULL), bpp(0), width(0), height(0)
	{}

	~IMAGE()
	{
		//if(data != NULL) delete [] data;
	}
	
	bool Load(char * filename);					//load a texture from a file

	bool LoadBMP(char * filename);				//Load BMP
#ifndef OPENGL_ES
#ifndef ARCH_PSP
	bool LoadJPG(char * filename);
#endif
#endif
	bool Load24BitBMP(char * filename);
	bool Load8BitBMP(char * filename);
	bool LoadPCX(char * filename);				//Load PCX
	bool LoadTGA(char * filename);				//Load TGA
	bool LoadPNG(char * filename);				//Load PNG
	bool LoadUncompressed8BitTGA(char * filename);
	bool LoadUncompressedTrueColorTGA(char * filename);
	bool LoadCompressedTrueColorTGA(char * filename);
	
	//load an uncompressed greyscale TGA for alpha channel
	bool LoadAlphaTGA(char * filename);

	//Alter the image
	void FlipVertically();

	unsigned char * data;				//Image data
	unsigned int	bpp;				//Image color depth in bits per pixel
	unsigned int	width;				//Image width
	unsigned int	height;			//Image height
	std::vector<unsigned char> image;
	
	unsigned int	 format;				//Data format, eg GL_RGBA
};

//Universal bitmap identifier
const DWORD BITMAP_ID=0x4D42;
