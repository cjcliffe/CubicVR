//////////////////////////////////////////////////////////////////////////////////////////
//	IMAGE.cpp
//	functions for image for a texture
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include <CubicVR/GLExt.h>
#include <CubicVR/IMAGE.h>
#include <CubicVR/Logger.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <algorithm>

#ifdef USE_LIBJPEG
    #ifndef OPENGL_ES
    #ifndef ARCH_PSP
	#ifdef ARCH_DC
		#include "jpeg/jpeglib.h"
	#else
	extern "C"
	{
		#include "jpeglib.h"
	}
	#endif
	#endif
	#endif
#else
    #include <CubicVR/tinyjpeg/tinyjpeg.h>
#endif

//Load - load a texture from a file
bool IMAGE::Load(char * filename)
{
	//Clear the data if already used
	if(data)
		delete [] data;
	data=NULL;
	bpp=0;
	width=0;
	height=0;
#ifndef ARCH_PSP
	format=0;
#endif	
	Logger::log("Loading Texture: [%s]:\n", filename);


	int filenameLength=strlen(filename);

	if(	strncmp((filename+filenameLength-3), "BMP", 3)==0 ||
		strncmp((filename+filenameLength-3), "bmp", 3)==0)
		return LoadBMP(filename);
	
	if(	strncmp((filename+filenameLength-3), "PCX", 3)==0 ||
		strncmp((filename+filenameLength-3), "pcx", 3)==0)
		return LoadPCX(filename);
	
	if(	strncmp((filename+filenameLength-3), "TGA", 3)==0 ||
		strncmp((filename+filenameLength-3), "tga", 3)==0)
	{
		Logger::log("Loading '%s' [TGA file]\n", filename);
		return LoadTGA(filename);
	}

#ifndef OPENGL_ES
#ifndef ARCH_PSP
#ifndef ARCH_DC

	if(	strncmp((filename+filenameLength-3), "JPG", 3)==0 ||
	   strncmp((filename+filenameLength-3), "jpg", 3)==0)
		return LoadJPG(filename);
#endif
#endif
#endif
	
	
	if(	strncmp((filename+filenameLength-3), "PNG", 3)==0 ||
	   strncmp((filename+filenameLength-3), "png", 3)==0)
		return LoadPNG(filename);
	
	Logger::log(LOG_ERROR,"%s does not end in \".tga\", \".bmp\", \".jpg\", \".png\" or \".pcx\"\n", filename);
	return false;
}



bool IMAGE::LoadPNG(char * filename)
{
	LodePNG::Decoder pngDecoder;
	std::vector<unsigned char> buffer;
	
	Logger::log("Loading png file %s..\n",filename);
	
	LodePNG::loadFile(buffer,filename);
	
	LodePNG_inspect(&pngDecoder, &buffer[0], buffer.size());
	
	pngDecoder.getInfoRaw().color.colorType = (pngDecoder.getInfoPng().color.colorType == 6)?6:2;
	
	Logger::log("Calling PNG decoder..\n");
	pngDecoder.decode(image, buffer);
	
	Logger::log("Checking decoder state..\n");
	if(pngDecoder.hasError())
	{
		Logger::log(LOG_ERROR,"[loadTextureFromPng] Error %d\n", pngDecoder.getError());
		Logger::log(LOG_ERROR," while trying to load \'%s\'\n",filename);
		return false;
	}
	
	int pngwidth = pngDecoder.getWidth();
	int pngheight = pngDecoder.getHeight();
	
	Logger::log("PNG Decoder Info [ width: %d, height: %d ]..\n",pngwidth,pngheight);
	
	std::vector<unsigned char>::iterator iStart, iEnd, iTarget;
	
#ifndef ARCH_PSP	
	format = pngDecoder.isAlphaType()?GL_RGBA:GL_RGB;
#endif
	bpp = pngDecoder.isAlphaType()?32:24;
	
	Logger::log("Alpha? %s\n",pngDecoder.isAlphaType()?"Yes":"No");

	Logger::log("Swapping ranges");

	unsigned short stride = bpp/8;
	
	for (int i = 0; i < pngheight / 2; i++)
	{
		iStart = image.begin() + (i *  (pngwidth * stride));
		iEnd = iStart + (pngwidth * stride);
		iTarget = image.begin() + (((pngheight - 1) - i) *  (pngwidth * stride));
		swap_ranges(iStart, iEnd, iTarget);
//		Logger::log(".");
	}
	
	Logger::log(".\n");
	
	
	
	width=pngwidth;
	height=pngheight;
	
	data = &image[0];
	//
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	return true;
}

bool IMAGE::LoadBMP(char * filename)
{
	FILE * file;												//the texture file
	BITMAPFILEHEADER fileHeader;								//bitmap file header
	BITMAPINFOHEADER infoHeader;								//bitmap info header

	//open file for reading
	file=fopen(filename, "rb");
	if(file==NULL)
	{
		Logger::log(LOG_ERROR,"LoadBMP: Unable to open %s\n", filename);
		return false;
	}

	//read the file header
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);

	//check it's a bitmap
	if(fileHeader.bfType != BITMAP_ID)
	{
		fclose(file);
		Logger::log(LOG_ERROR,"LoadBMP: %s is not a legal .BMP\n", filename);
		return false;
	}

	//read in the information header
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	//close the file
	fclose(file);

	//discover the bpp
	if(infoHeader.biBitCount==24)
		return Load24BitBMP(filename);
	if(infoHeader.biBitCount==8)
		return Load8BitBMP(filename);

	Logger::log(LOG_ERROR,"LoadBMP: %s has an unknown bpp\n", filename);
	return false;
}
	

//Load24BitBMP - load a 24 bit bitmap file
bool IMAGE::Load24BitBMP(char * filename)
{
	Logger::log("Loading %s in Load24bitBMP()\n", filename);
	
	//set bpp and format
	bpp=24;
#ifndef ARCH_PSP
	format=GL_RGB;
#endif
	FILE * file;												//the texture file
	BITMAPFILEHEADER fileHeader;								//bitmap file header
	BITMAPINFOHEADER infoHeader;								//bitmap info header

	//open file for reading
	file=fopen(filename, "rb");
	if(file==NULL)
	{
		Logger::log(LOG_ERROR,"Load24BitBMP: Unable to open %s\n", filename);
		return false;
	}

	//read the file header
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);

	//check it's a bitmap
	if(fileHeader.bfType != BITMAP_ID)
	{
		fclose(file);
		Logger::log(LOG_ERROR,"Load24BitBMP: %s is not a legal .BMP\n", filename);
		return false;
	}

	//read in the information header
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	//set size
	width=infoHeader.biWidth;
	height=infoHeader.biHeight;

	//calculate the stride in bytes: width*bpp/8 plus padding bytes at the end of each row
	unsigned int stride=width*bpp/8;
	if(width%4==1)
		stride+=1;
	if(width%4==2)
		stride+=2;
	if(width%4==3)
		stride+=3;

	//point file to the beginning of the data
	fseek(file, fileHeader.bfOffBits, SEEK_SET);

	//allocate space for the image data
	data=new unsigned char[stride*height];
	if(!data)
	{
		fclose(file);
		Logger::log(LOG_ERROR,"Load24BitBMP: Unable to allocate memory for %s\n", filename);
		return false;
	}

	//read in the data
	fread(data, 1, stride*height, file);

	//close the file
	fclose(file);

	//data is in BGR format
	//swap b and r
	for(unsigned int row=0; row<height; row++)
	{
		for(unsigned int i=0; i<width*3; i+=bpp/8)
		{	
			//repeated XOR to swap bytes 0 and 2
			data[(row*stride)+i] ^= data[(row*stride)+i+2] ^=
				data[(row*stride)+i] ^= data[(row*stride)+i+2];
		}
	}

	Logger::log("Loaded %s correctly.\n", filename);
	return true;
}

//Load8BitBMP - load an 8 bit paletted bitmap file
bool IMAGE::Load8BitBMP(char * filename)
{
	Logger::log("Load8BitBMP: Loading %s in Load8bitBMP()\n", filename);
	
	//set bpp and format
	bpp=24; //after conversion
#ifndef ARCH_PSP
	format=GL_RGB;
#endif
	FILE * file;												//the texture file
	BITMAPFILEHEADER fileHeader;								//bitmap file header
	BITMAPINFOHEADER infoHeader;								//bitmap info header

	//open file for reading
	file=fopen(filename, "rb");
	if(file==NULL)
	{
		Logger::log(LOG_ERROR,"Load8BitBMP: Unable to open %s\n", filename);
		return false;
	}

	//read the file header
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);

	//check it's a bitmap
	if(fileHeader.bfType != BITMAP_ID)
	{
		fclose(file);
		Logger::log(LOG_ERROR,"Load8BitBMP: %s is not a legal .BMP\n", filename);
		return false;
	}

	//read in the information header
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	//set size
	width=infoHeader.biWidth;
	height=infoHeader.biHeight;

	//make space for palette
	unsigned char * palette=new unsigned char[256*4];
	if(!palette)
	{
		Logger::log(LOG_ERROR,"Load8BitBMP: Unable to alllocate memory for palette\n");
		return false;
	}
	
	//load the palette
	fread(palette, 256*4, 1, file);
	
	//point file to the beginning of the data
	fseek(file, fileHeader.bfOffBits, SEEK_SET);

	//calculate the stride in bytes between one row and the next
	unsigned int stride=width;
	if(width%4 != 0)
		stride+=4-width%4;

	//allocate space for color indices
	unsigned char * indices=new unsigned char[stride*height];
	if(!indices)
	{
		Logger::log(LOG_ERROR,"Load8BitBMP: Unable to allocate memory for indices\n");
		return false;
	}
	
	//load indices
	fread(indices, 1, stride*height, file);

	//close the file
	fclose(file);

	//allocate space for the image data
	data=new unsigned char[stride*height*bpp/8];
	if(!data)
	{
		fclose(file);
		Logger::log(LOG_ERROR,"Load8BitBMP: Unable to allocate memory for %s\n", filename);
		return false;
	}

	//calculate the color values - keeping the padding colors
	for(unsigned int currentRow=0; currentRow<height; currentRow++)
	{
		for(unsigned int i=0; i<stride; i++)
		{
			data[(currentRow*stride+i)*3+0]=palette[indices[currentRow*stride+i]*4+2];
			data[(currentRow*stride+i)*3+1]=palette[indices[currentRow*stride+i]*4+1];
			data[(currentRow*stride+i)*3+2]=palette[indices[currentRow*stride+i]*4+0];//BGR
		}
	}
	
	Logger::log(LOG_ERROR,"Load8BitBMP: Loaded %s correctly.\n", filename);
	return true;
}







//LoadPCX - load a .pcx texture - 256 color, paletted
bool IMAGE::LoadPCX(char * filename)
{
	Logger::log("Loading %s in LoadPCX()\n", filename);

	//set bpp and format
	bpp=24;
#ifndef ARCH_PSP
	format=GL_RGB;
#endif
	FILE * file;

	file=fopen(filename, "rb");
	if(!file)
	{
		Logger::log(LOG_ERROR,"Unable to open %s\n", filename);
		return false;
	}

	//retrieve header, first 4 bytes, first 2 should be 0x0A0C
	unsigned short header[2];
	fread(header, 4, 1, file);

	if(header[0]!=0x050A)
	{
		Logger::log(LOG_ERROR,"%s is not a legal .PCX file\n", filename);
		fclose(file);
		return false;
	}

	//retrieve minimum x value
	int xMin=fgetc(file);		//loword
	xMin |= fgetc(file) << 8;	//hiword

	//retrieve minimum y value
	int yMin=fgetc(file);		//loword
	yMin |= fgetc(file) << 8;	//hiword

	//retrieve maximum x value
	int xMax=fgetc(file);		//loword
	xMax |= fgetc(file) << 8;	//hiword

	//retrieve maximum y value
	int yMax=fgetc(file);		//loword
	yMax |= fgetc(file) << 8;	//hiword

	//calculate width and height
	width = xMax-xMin+1;
	height= yMax-yMin+1;

	//allocate memory for pixel data (paletted)
	unsigned char * pixelData=new unsigned char[width*height];
	if(!pixelData)
	{
		Logger::log(LOG_ERROR,"Unable to allocate %d bytes for the image data of %s\n",
								width*height, filename);
		fclose(file);
		return false;
	}

	//set file pointer to beginning of image data
	fseek(file, 128, SEEK_SET);

	//decode and store the pixel data
	unsigned int index=0;

	while(index<(width*height))
	{
		int c = getc(file);

		if(c>0xBF)
		{
			int numRepeat = 0x3F & c;
			c=getc(file);

			for(int i=0; i<numRepeat; i++)
				pixelData[index++] = c;
		}
		else
			pixelData[index++] = c;

		fflush(stdout);
	}

	//allocate memory for the image palette
	unsigned char * paletteData = new unsigned char[768];

	//the palette is the last 769 bytes of the file
	fseek(file, -769, SEEK_END);

	//retrieve first character, should be equal to 12
	int c=getc(file);
	if(c!=12)
	{
		Logger::log(LOG_ERROR,"%s is not a legal .PCX file - the palette data has an illegal header, %d\n",
								filename, c);
		fclose(file);
		return false;
	}

	//read and store the palette
	fread(paletteData, 1, 768, file);
	
	//close the file
	fclose(file);

	//allocate memory for the "unpaletted" data
	data = new unsigned char[width*height*3];
	if(!data)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for the expanded data of %s\n", filename);
		return false;
	}

	//calculate the "unpaletted" data - "flipping" the texture top-bottom
	for(unsigned int j=0; j<height; j++)
	{
		for(unsigned int i=0; i<width; i++)
		{
			data[3*(j*width+i)]		= (unsigned char) paletteData[3*pixelData[(height-1-j)*width+i]];
			data[3*(j*width+i)+1]	= (unsigned char) paletteData[3*pixelData[(height-1-j)*width+i]+1];
			data[3*(j*width+i)+2]	= (unsigned char) paletteData[3*pixelData[(height-1-j)*width+i]+2];
		}
	}

	Logger::log("Loaded %s correctly.\n", filename);
	return true;
	
}





//Load a TGA texture
bool IMAGE::LoadTGA(char * filename)
{
	unsigned char	UncompressedTGAHeader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char	CompressedTGAHeader[12]={0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char	Uncompressed8BitTGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};

	unsigned char	TGAcompare[12];						//Used to compare TGA header
	
	FILE * file = fopen(filename, "rb");		//Open the TGA file
	
	if(	file==NULL )							//Does the file exist?
	{
		Logger::log(LOG_ERROR,"%s does not exist\n", filename);
		return false;
	}
	
	//read the header
	fread(TGAcompare, 1, sizeof(TGAcompare), file);
	fclose(file);

	if(memcmp(UncompressedTGAHeader, TGAcompare, sizeof(UncompressedTGAHeader))==0)
	{
		return LoadUncompressedTrueColorTGA(filename);
	}
	else if(memcmp(CompressedTGAHeader, TGAcompare, sizeof(CompressedTGAHeader))==0)
	{
		return LoadCompressedTrueColorTGA(filename);
	}
	else if(memcmp(Uncompressed8BitTGAHeader, TGAcompare, sizeof(Uncompressed8BitTGAHeader))==0)
	{
		return LoadUncompressed8BitTGA(filename);
	}
	else
	{
		Logger::log(LOG_ERROR,"%s is not a recognised type of TGA\n", filename);
		return false;
	}
	
	return false;
}

//load an 8 bit uncompressed paletted TGA
bool IMAGE::LoadUncompressed8BitTGA(char * filename)
{
	unsigned char		TGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
	unsigned char		TGAcompare[12];						//Used to compare TGA header
	unsigned char		header[6];							//First 6 useful bytes of the header
		
	Logger::log("Loading %s in LoadUncompressed8BitTGA()\n", filename);

	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{
		Logger::log(LOG_ERROR,"%s does not exist.\n", filename);
		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAHeader, TGAcompare, sizeof(TGAHeader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file
		Logger::log(LOG_ERROR,"Could not load %s correctly, general failure.\n", filename);
		return false;
	}
	
	//save data into class member variables
	width=	header[1]*256+header[0];						//determine the image width
	height=	header[3]*256+header[2];						//determine image height
	bpp=	header[4];

	if(	width<=0	||											//if width <=0
		height<=0	||											//or height<=0
		bpp!=8)													//bpp not 8
	{
		fclose(file);											//close the file
		Logger::log(LOG_ERROR,"%s's height or width is less than zero, or the TGA is not 8 bpp.\n", filename);
		return false;
	}

#ifndef ARCH_PSP
	//set format
	format=GL_RGB;
#endif
	
	//make space for palette
	unsigned char * palette=new unsigned char[256*3];
	if(!palette)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for palette");
		return false;
	}
	
	//load the palette
	fread(palette, 256*3, 1, file);
	
	//allocate space for color indices
	unsigned char * indices=new unsigned char[width*height];
	if(!indices)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for indices");
		return false;
	}
	
	//load indices
	fread(indices, 1, width*height, file);

	//close the file
	fclose(file);

	//allocate space for the image data
	data=new unsigned char[width*height*3];
	if(!data)
	{
		fclose(file);
		Logger::log(LOG_ERROR,"Unable to allocate memory for %s\n", filename);
		return false;
	}

	//calculate the color values
	for(unsigned int currentRow=0; currentRow<height; currentRow++)
	{
		for(unsigned int i=0; i<width; i++)
		{
			data[(currentRow*width+i)*3+0]=palette[indices[currentRow*width+i]*3+2];
			data[(currentRow*width+i)*3+1]=palette[indices[currentRow*width+i]*3+1];
			data[(currentRow*width+i)*3+2]=palette[indices[currentRow*width+i]*3+0];//BGR
		}
	}
	
	Logger::log("Loaded %s correctly.\n", filename);
	return true;
}





//load an uncompressed TGA texture (24 or 32 bpp)
bool IMAGE::LoadUncompressedTrueColorTGA(char * filename)
{
	unsigned char	TGAheader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//Uncompressed TGA header
	unsigned char	TGAcompare[12];						//Used to compare TGA header
	unsigned char	header[6];							//First 6 useful bytes of the header
	unsigned int	bytesPerPixel;						//bytes per pixel
	unsigned int	imageSize;							//Stores Image size when in RAM
		
	Logger::log("Loading %s in LoadUncompressedTGA()\n", filename);

	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{
		Logger::log(LOG_ERROR,"%s does not exist.\n", filename);
		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file
		Logger::log(LOG_ERROR,"Could not load %s correctly, general failure.\n", filename);
		return false;
	}
	
	//save data into class member variables
	width=	header[1]*256+header[0];						//determine the image width
	height=	header[3]*256+header[2];						//determine image height
	bpp=	header[4];

	if(	width<=0	||											//if width <=0
		height<=0	||											//or height<=0
		bpp!=24 && bpp!=32)										//bpp not 24 or 32
	{
		fclose(file);											//close the file
		Logger::log(LOG_ERROR,"%s's height or width is less than zero, or the TGA is not 24 or 32 bpp.\n", filename);
		return false;
	}

#ifndef ARCH_PSP
	//set format
	if(bpp == 24)
		format=GL_RGB;
	else
		format=GL_RGBA;
#endif
	
	bytesPerPixel=bpp/8;										//calc bytes per pixel
	imageSize=width*height*bytesPerPixel;						//calc memory required

	data=new unsigned char[imageSize];							//reserve the memory for the data

	if(	data==NULL)											//Does the storage memory exist?
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for %s image\n", filename);
		fclose(file);
		return false;
	}
	
	//read in the image data
	if(fread(data, 1, imageSize, file)!=imageSize)				//Does the image size match the required?
	{															//If not
		if(data)												//If data loaded
			delete [] data;										//free memory
		data = NULL;
		Logger::log(LOG_ERROR,"Could not read %s image data\n", filename);
		fclose(file);											//close file
		return false;
	}

	fclose(file);

	//data is in BGR format
	//swap b and r
	for(int i=0; i<(int)imageSize; i+=bytesPerPixel)
	{	
		//repeated XOR to swap bytes 0 and 2
		data[i] ^= data[i+2] ^= data[i] ^= data[i+2];
	}
	
	Logger::log("Loaded %s correctly.\n", filename);
	return true;
}





//load a compressed TGA texture (24 or 32 bpp)
bool IMAGE::LoadCompressedTrueColorTGA(char * filename)
{
	unsigned char	TGAheader[12]={0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//Compressed TGA header
	unsigned char	TGAcompare[12];						//Used to compare TGA header
	unsigned char	header[6];							//First 6 useful bytes of the header
	unsigned int	bytesPerPixel;						//bytes per pixel
	unsigned int	imageSize;							//Stores Image size when in RAM
		
	Logger::log("Loading %s in LoadCompressedTGA()\n", filename);

	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{
		Logger::log(LOG_ERROR,"%s does not exist.\n", filename);
		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file
		Logger::log(LOG_ERROR,"Could not load %s correctly, general failure.\n", filename);
		return false;
	}
	
	//save data into class member variables
	width=	header[1]*256+header[0];						//determine the image width
	height=	header[3]*256+header[2];						//determine image height
	bpp=	header[4];

	if(	width<=0	||											//if width <=0
		height<=0	||											//or height<=0
		bpp!=24 && bpp!=32)										//bpp not 24 or 32
	{
		fclose(file);											//close the file
		Logger::log(LOG_ERROR,"%s's height or width is less than zero, or the TGA is not 24 or 32 bpp.\n", filename);
		return false;
	}

#ifndef ARCH_PSP
	//set format
	if(bpp == 24)
		format=GL_RGB;
	else
		format=GL_RGBA;
#endif
	bytesPerPixel=bpp/8;										//calc bytes per pixel
	imageSize=width*height*bytesPerPixel;						//calc memory required

	data=new unsigned char[imageSize];							//reserve the memory for the data
	if(!data)													//Does the storage memory exist?
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for %s image\n", filename);
		fclose(file);
		return false;
	}
	
	//read in the image data
	int pixelCount	= height*width;
	int currentPixel= 0;
	int currentByte	= 0;
	unsigned char * colorBuffer=new unsigned char[bytesPerPixel];

	do
	{
		unsigned char chunkHeader=0;

		if(fread(&chunkHeader, sizeof(unsigned char), 1, file) == 0)
		{
			Logger::log(LOG_ERROR,"Could not read RLE chunk header");
			if(file)
				fclose(file);
			if(data)
			{
				delete [] data;
				data = NULL;
			}	
			return false;
		}

		if(chunkHeader<128)	//Read raw color values
		{
			chunkHeader++;

			for(short counter=0; counter<chunkHeader; counter++)
			{
				if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
				{
					Logger::log(LOG_ERROR,"Unable to read %s image data\n", filename);
					
					if(file)
						fclose(file);

					if(colorBuffer)
						delete [] colorBuffer;

					if(data)
					{
						delete [] data;
						data = NULL;
					}

					return false;
				}

				//transfer pixel color to data (swapping r and b values)
				data[currentByte] = colorBuffer[2];
				data[currentByte+1] = colorBuffer[1];
				data[currentByte+2] = colorBuffer[0];

				if(bytesPerPixel==4)
					data[currentByte+3]=colorBuffer[3];

				currentByte+=bytesPerPixel;
				currentPixel++;

				if(currentPixel > pixelCount)
				{
					Logger::log(LOG_ERROR,"Too many pixels read");
					if(file)
						fclose(file);
					if(colorBuffer)
						delete [] colorBuffer;
					if(data)
					{
						delete [] data;
						data = NULL;
					}
					return false;
				}
			}
		}
		else	//chunkHeader>=128
		{
			chunkHeader-=127;

			if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
			{
				Logger::log(LOG_ERROR,"Unable to read %s image data\n", filename);
					
				if(file)
					fclose(file);
				if(colorBuffer)
					delete [] colorBuffer;
				if(data)
				{
					delete [] data;
					data = NULL;
				}
				return false;
			}

			for(short counter=0; counter<chunkHeader; counter++)
			{
				//transfer pixel color to data (swapping r and b values)
				data[currentByte] = colorBuffer[2];
				data[currentByte+1] = colorBuffer[1];
				data[currentByte+2] = colorBuffer[0];

				if(bytesPerPixel==4)
					data[currentByte+3]=colorBuffer[3];

				currentByte+=bytesPerPixel;
				currentPixel++;

				if(currentPixel > pixelCount)
				{
					Logger::log(LOG_ERROR,"Too many pixels read");
					if(file)
						fclose(file);
					if(colorBuffer)
						delete [] colorBuffer;
					if(data)
					{
						delete [] data;
						data = NULL;
					}
					return false;
				}
			}
		}
	}while(currentPixel<pixelCount);

	fclose(file);

	Logger::log("Loaded %s correctly.\n", filename);
	return true;
}






//load in an 8 bit greyscale TGA as an alpha channel
bool IMAGE::LoadAlphaTGA(char * filename)
{
	unsigned char	TGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
	unsigned char	TGAcompare[12];						//Used to compare TGA header
	unsigned char	header[6];							//First 6 useful bytes of the header
		
	Logger::log("Loading %s in LoadAlphaTGA()\n", filename);
#ifndef ARCH_PSP
	if(!(format==GL_RGB || format==GL_RGBA))
	{
		Logger::log(LOG_ERROR,"Can only load an alpha channel to RGB / RGBA format images. %s caused error\n", filename);
		return false;
	}
#endif
	FILE * file = fopen(filename, "rb");				//Open the TGA file
	
	if(file == NULL)								//Does the file exist?
	{
		Logger::log(LOG_ERROR,"%s does not exist.\n", filename);
		return false;
	}

	if(	fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||	//Are there 12 bytes to read?
		memcmp(TGAHeader, TGAcompare, sizeof(TGAHeader))!=0	||					//Is the header correct?
		fread(header, 1, sizeof(header), file)!=sizeof(header))		//Read next 6 bytes
	{
		fclose(file);								//If anything else failed, close the file
		Logger::log(LOG_ERROR,"Could not load %s correctly, general failure.\n", filename);
		return false;
	}
	
	//save data into class member variables
	unsigned int alphaWidth=	header[1]*256+header[0];						//determine the image width
	unsigned int alphaHeight=	header[3]*256+header[2];						//determine image height
	int alphaBpp=				header[4];

	if(	alphaWidth<=0	||											//if width <=0
		alphaHeight<=0	||											//or height<=0
		alphaBpp!=8)												//bpp not 8
	{
		fclose(file);											//close the file
		Logger::log(LOG_ERROR,"%s's height or width is less than zero, or the TGA is not 8 bpp.\n", filename);
		return false;
	}

	//check it is the same size as the image
	if(alphaWidth!=width || alphaHeight!=height)
	{
		Logger::log(LOG_ERROR,"%s is not the same size as the color texture\n", filename);
		return false;
	}

	//make space for palette
	unsigned char * palette=new unsigned char[256*3];
	if(!palette)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for palette");
		return false;
	}
	
	//load the palette
	fread(palette, 256*3, 1, file);
	
	//we dont use the palette
	delete [] palette;
	palette=NULL;

	//allocate space for alpha values
	unsigned char * values=new unsigned char[width*height];
	if(!values)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for alpha values");
		return false;
	}
	
	//load indices
	fread(values, 1, alphaWidth*alphaHeight, file);

	//close the file
	fclose(file);

	//now put in the alpha data
#ifndef ARCH_PSP
	if(format==GL_RGBA)
	{
		for(unsigned int i=0; i<width*height; i++)
		{
			data[i*4+3]=values[i];
		}
	}
	else if(format==GL_RGB)
	{
		unsigned char * tempData=new unsigned char[width*height*4];
		if(!tempData)
		{
			Logger::log(LOG_ERROR,"Unable to allocate memory for Temporary Data");
			return false;
		}

		for(unsigned int i=0; i<width*height; i++)
		{
			tempData[i*4+0]=data[i*3+0];
			tempData[i*4+1]=data[i*3+1];
			tempData[i*4+2]=data[i*3+2];
			tempData[i*4+3]=values[i];
		}

		//update member variables
		bpp=32;
		format=GL_RGBA;

		if(data)
		{
			delete [] data;
			data = NULL;
		}

		if (tempData)
		{
			delete [] tempData;
		}
			//data=tempData;
	}
#endif
	Logger::log("Loaded %s correctly.\n", filename);
	return true;
}


void IMAGE::FlipVertically()
{
	//dont flip zero or 1 height images
	if(height==0 || height==1)
		return;

	int rowsToSwap=0;
	//see how many rows to swap
	if(height%2==1)
		rowsToSwap=(height-1)/2;
	else
		rowsToSwap=height/2;

	//create space for a temporary row
	GLubyte * tempRow=new GLubyte[width*bpp/8];
	if(!tempRow)
	{
		Logger::log(LOG_ERROR,"Unable to flip image, unable to create space for temporary row");
		return;
	}

	//loop through rows to swap
	for(int i=0; i<rowsToSwap; ++i)
	{
		//copy row i into temp
		memcpy(tempRow, &data[i*width*bpp/8], width*bpp/8);
		//copy row height-i-1 to row i
		memcpy(&data[i*width*bpp/8], &data[(height-i-1)*width*bpp/8], width*bpp/8);
		//copy temp into row height-i-1
		memcpy(&data[(height-i-1)*width*bpp/8], tempRow, width*bpp/8);
	}

	//free tempRow
	if(tempRow)
		delete [] tempRow;
	tempRow=NULL;
}
		

#ifndef OPENGL_ES
#ifndef ARCH_PSP
#ifndef ARCH_DC

//Load a jpg texture into image
bool IMAGE::LoadJPG(char * filename)
{
	Logger::log("Loading %s in LoadJPG()\n", filename);
	
	//Clear the data in image
	if(data)
		delete [] data;
	data=NULL;
	bpp=0;
	format=0;
	height=0;
	width=0;
	
    FILE * file = fopen(filename, "rb");				//Open the JPG file
	
	if(file == NULL)								//Does the file exist?
	{
		Logger::log(LOG_ERROR,"%s does not exist.\n", filename);
		return false;
	}
    
#ifndef USE_LIBJPEG
    // use tiny jpeg here
    int length_of_file;
    unsigned char *buf;
    struct jdec_private *jdec;
    unsigned char *components[4];
    
    jdec = tinyjpeg_init();
    
    /* Load the Jpeg into memory */
    struct stat sbuf;
    fstat(fileno(file), &sbuf);
    length_of_file = sbuf.st_size;

    buf = (unsigned char *)malloc(length_of_file + 4);
    fread(buf, length_of_file, 1, file);
    fclose(file);
    
    if (tinyjpeg_parse_header(jdec, buf, length_of_file)<0) {
        Logger::log(LOG_ERROR,"TinyJpeg Parse Header failed: \n");
        Logger::log(LOG_ERROR,tinyjpeg_get_errorstring(jdec));
        Logger::log(LOG_ERROR,"\n");
        return false;
    }
    
    tinyjpeg_decode(jdec, TINYJPEG_FMT_RGB24);
    
    /* 
     * Get address for each plane (not only max 3 planes is supported), and
     * depending of the output mode, only some components will be filled 
     * RGB: 1 plane, YUV420P: 3 planes, GREY: 1 plane
     */
    tinyjpeg_get_components(jdec, components);
    tinyjpeg_get_size(jdec, &width, &height);

    Logger::log(LOG_DEBUG,"JPEG Dimensions: [%d,%d] \n",width,height);

    
    format=GL_RGB;
    
    bpp=3*8;
		
	//Allocate memory for image
	data=new GLubyte[width*height*3];
    
	if(!data)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for temporary texture data");
		return false;
	}

    memcpy(data,components[0],width*height*3);

    
  
    
    /* Save it */
//    switch (output_format)
//    {
//        case TINYJPEG_FMT_RGB24:
//        case TINYJPEG_FMT_BGR24:
//            write_tga(outfilename, output_format, width, height, components);
//            break;
//        case TINYJPEG_FMT_YUV420P:
//            write_yuv(outfilename, width, height, components);
//            break;
//        case TINYJPEG_FMT_GREY:
//            write_pgm(outfilename, width, height, components);
//            break;
//    }
    
    free(buf);
    tinyjpeg_free(jdec);
    return true;

    
#else
	
	struct jpeg_decompress_struct cinfo;
	
	//Create an error handler
	jpeg_error_mgr jerr;
	
	//point the compression object to the error handler
	cinfo.err=jpeg_std_error(&jerr);
	
	//Initialize the decompression object
	jpeg_create_decompress(&cinfo);
	
	//Specify the data source
	jpeg_stdio_src(&cinfo, file);
	
	
	
	//Decode the jpeg data into the image
	//Read in the header
	jpeg_read_header(&cinfo, true);
	
	//start to decompress the data
	jpeg_start_decompress(&cinfo);
	
	//get the number of color channels
	int channels=cinfo.num_components;
	
	//Fill in class variables
	bpp=channels*8;
	width=cinfo.image_width;
	height=cinfo.image_height;
	
	if(bpp==24)
		format=GL_RGB;
	if(bpp==32)
		format=GL_RGBA;
	
	//Allocate memory for image
	data=new GLubyte[width*height*channels];
	if(!data)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for temporary texture data");
		return false;
	}
	
	//Create an array of row pointers
	unsigned char ** rowPtr = new unsigned char * [height];
	if(!rowPtr)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for row pointers");
		return false;
	}
	
	for(unsigned int i=0; i<height; ++i)
		rowPtr[i]=&(data[i*width*channels]);
	
	//Extract the pixel data
	int rowsRead=0;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		//read in this row
		rowsRead+=jpeg_read_scanlines(&cinfo, &rowPtr[rowsRead], cinfo.output_height - rowsRead);
	}
	
	//release memory used by jpeg
	jpeg_destroy_decompress(&cinfo);
	
	fclose(file);
	
	//delete row pointers
	if(rowPtr)
		delete [] rowPtr;
	rowPtr=NULL;

    //Flip the texture vertically
	FlipVertically();

#endif
    
    
    
    
	
	Logger::log("%s loaded successfully\n", filename);
	return true;
}
#endif
#endif
#endif