//**************************************************************************
//		PSPGU Tutorial: 'Lesson5' - TGALoader.cpp
//**************************************************************************

#include <stdio.h>
#include <string.h>
#include <memory>
#include <CubicVR/psp/TGALoader.h>

CTGATexture::CTGATexture( void )
{
	type = 0;
	image = 0;
	swizzled = false;
	imageWidth = 0;
	imageHeight = 0;
}

CTGATexture::~CTGATexture( void )
{
	FreeImage();
}

bool CTGATexture::LoadTGA( char *filename )
{
	
	FILE *pFile = 0;

	// Variables to hold image info
	unsigned char tempColor;
	unsigned char bitCount;
	int colorMode;
	long tgaSize;
	unsigned char unCompressHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char tgaHeader[12];
	unsigned char header[6];

	// Open file
	pFile = fopen( filename, "rb" );
	
	if( !pFile ) {
		// if couldn't open, exit
		return false;
	}
	
	// Read in BitmapHeader info into the structure
	fread( &tgaHeader, 1, sizeof(tgaHeader), pFile );
	
	// We only want to read uncompressed TGA's. Chech the 
	// header if it is of an uncompressed one.
	if( memcmp( unCompressHeader, tgaHeader, sizeof(unCompressHeader)) != 0 ) {
		fclose( pFile );
		return false;
	}
	
	// Read Image info
	fread( header, 1, sizeof(header), pFile );

	// Calculate and save the Width & Height of Image
	imageWidth  = header[1] * 256 + header[0];
	imageHeight = header[3] * 256 + header[2];
	bitCount	= header[4];

	// Calculate color mode & image size
	colorMode	= bitCount / 8;
	tgaSize		= imageWidth * imageHeight * colorMode;

	// Allocate memory for the image and load it
	image		= (unsigned char*) malloc( tgaSize*sizeof(unsigned char) );

	fread( image, sizeof(unsigned char), tgaSize, pFile );

	// Convert from BGR to RGB format.
	for(long index = 0; index < tgaSize; index += colorMode) {
		   tempColor = image[index];
		   image[index] = image[index + 2];
		   image[index + 2] = tempColor;
	}
	
	type = colorMode;

	// Close the file
	fclose( pFile );

	return true;
}

void CTGATexture::Swizzle( void )
{
	unsigned int i,j;
	unsigned int rowblocks = (imageWidth*sizeof(u32) / 16);
	long size = imageWidth * imageHeight * type;
	 
	unsigned char* out = (unsigned char*)malloc( size*sizeof(unsigned char) );

	for (j = 0; j < imageHeight; ++j)
	{
		for (i = 0; i < imageWidth*sizeof(u32); ++i)
		{
			unsigned int blockx = i / 16;
			unsigned int blocky = j / 8;
	 
			unsigned int x = (i - blockx*16);
			unsigned int y = (j - blocky*8);
			unsigned int block_index = blockx + ((blocky) * rowblocks);
			unsigned int block_address = block_index * 16 * 8;
	 
			out[block_address + x + y * 16] = image[i+j*imageWidth*sizeof(u32)];
		}
	}
	memcpy(image, out, size );
	swizzled = !swizzled;
	free( out );
}

void CTGATexture::FreeImage( void )
{
	if( image ) {
		free( image );
		image = 0;
	}
}
