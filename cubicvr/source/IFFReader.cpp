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

#include <CubicVR/IFFReader.h>
#include <CubicVR/Logger.h>

// IFF Reader, currently only designed to read lightwave IFF files


//#ifdef _M_IX86
	#define IFF_FLIP_ENDIAN
//#endif

bool IFFReader::isIFF(const string &filename)
{
	char buffer[4];
	cvrIndex size;

	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);
	size = (cvrIndex)file.tellg();

	if (size < 8) return false; // file is less than 4 byte header + 32 bit length, can't be real..
	file.seekg(0, ios::beg);
	file.read(buffer,4);
	
	file.close();

	// 3 possible 4 byte headers to be an IFF file:
	if (strncmp(buffer,"FORM",4) == 0) return true;

	// 2 not yet supported
	// if (strncmp(buffer,"LIST",4) == 0) return true;
	// if (strncmp(buffer,"CAT ",4) == 0) return true;

	return false;
}


bool IFFReader::load(const string &filename)
{
	if (!isIFF(filename)) return false;
	
	char long_buffer[4];
	char temp[4];
	char *iffbuffer;
	long form_size;

	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	file.seekg(0, ios::beg);
	file.read(temp,4);
	
	file.read(long_buffer,4);	
	
	form_size = IFFBlock::longbuf2long(long_buffer);
	
	iffbuffer = new char[form_size];

	file.read(iffbuffer,form_size);
	file.close();
	
	processIFFBlockWithHeader(iffbuffer,form_size);

	delete[] iffbuffer;

	return true;
}


IFFReader::IFFReader(char *in_data, long data_size, int ptr_size) 
{
	processIFFBlock(in_data, data_size, ptr_size);
}


void IFFReader::processIFFBlockWithHeader(char *in_data, long data_size)
{
	long block_ptr = 0;

	memcpy(header,in_data,4);
	header[4] = 0;
	block_ptr += 4;

	processIFFBlock(in_data+block_ptr,data_size-block_ptr,4);
}


void IFFReader::processIFFBlock(char *in_data, long data_size, int ptr_size)
{
	long block_ptr = 0;
	
	Logger::log("Block_Header: [");

	while (block_ptr < data_size)
	{
		char block_header[4];
		char block_size_buf[4];
		long block_size;
		char *block_data;

		memcpy(block_header,in_data+block_ptr,4);
		block_ptr+=4;

		Logger::log("%c%c%c%c,",block_header[0],block_header[1],block_header[2],block_header[3]);

		switch (ptr_size)
		{
			case 4:
				memcpy(block_size_buf,in_data+block_ptr,4);
				block_size = IFFBlock::longbuf2long(block_size_buf);
				block_ptr+=4;
			break;
			
			case 2:
				memcpy(block_size_buf,in_data+block_ptr,2);
				block_size = IFFBlock::shortbuf2short(block_size_buf);
				block_ptr+=2;
			break;
		}

		//Logger::log("Block_Size: %d of %d\n",block_size,data_size);

		block_data = new char[block_size];
		memcpy(block_data,in_data+block_ptr,block_size);
		block_ptr+=block_size;

		iffData.insert(iffData.end(), new IFFBlock(block_header,block_size,block_data));

		delete[] block_data;
	}

	Logger::log("]\n");
}


void IFFBlock::vectbuf2vect(XYZ &xyz_in, char *in_buf)
{
	xyz_in.x = IFFBlock::floatbuf2float(in_buf);
	xyz_in.y = IFFBlock::floatbuf2float(in_buf+4);
	xyz_in.z = IFFBlock::floatbuf2float(in_buf+8);
}

unsigned long IFFBlock::vxbuf2vx(char *in_buf, int &vxsize)
{
	long return_val;

	if ((unsigned char) *in_buf == 0xFF)
	{
		return_val = (unsigned long)longbuf2long(in_buf) & 0x00FFFFFF;
		vxsize = 4;
	}
	else
	{
		return_val = (unsigned short)shortbuf2short(in_buf);
		vxsize = 2;
	}
	
	return return_val;
}


long IFFBlock::longbuf2long(char *in_buf)
{
#ifdef __LP64__   
	int return_val;
#else /*__LP64__*/  
	long return_val;
#endif
	
	

	// flip it
#ifdef IFF_FLIP_ENDIAN
		char temp_buf[4];
		for (int x = 0; x < 4; x++) temp_buf[x] = in_buf[3-x];
		memcpy(&return_val,&temp_buf,4);
#else
		return_val = *((long *) in_buf);
#endif
		
	return return_val;
}


float IFFBlock::floatbuf2float(char *in_buf)
{
	float return_val;

#ifdef IFF_FLIP_ENDIAN
	char temp_buf[4];
	for (int x = 0; x < 4; x++) temp_buf[x] = in_buf[3-x];
	memcpy(&return_val,&temp_buf,4);
#else
	return_val = *((float *) in_buf);
#endif
	
	return return_val;
}


short IFFBlock::shortbuf2short(char *in_buf)
{
	short return_val;

#ifdef IFF_FLIP_ENDIAN
	char temp_buf[2];
	temp_buf[0] = in_buf[1]; temp_buf[1] = in_buf[0];
	memcpy(&return_val,&temp_buf,2);
#else
	return_val = *((short *) in_buf);
#endif
	return return_val;
}


IFFBlock *IFFReader::getBlock(char *header_name)
{
	for (unsigned int x = 0; x < iffData.size(); x++)
	{
		if (strncmp(iffData[x]->header,header_name,4) == 0)
		{
			return iffData[x];
		}
	}

	return NULL;
}


IFFBlock *IFFReader::getBlock(char *header_name, int blockCount)
{
	int blockCounter = 0;

	for (unsigned int x = 0; x < iffData.size(); x++)
	{
		if (strncmp(iffData[x]->header,header_name,4) == 0)
		{
			if (blockCounter == blockCount)
			{
				return iffData[x];
			}
			else
			{
				blockCounter++;
			}
		}
	}

	return NULL;
}

void IFFReader::getBlockNames(std::vector<string> &blockList)
{
	for (unsigned int x = 0; x < iffData.size(); x++)
	{
		blockList.push_back(string(iffData[x]->header));
	}
}


IFFBlock::IFFBlock(char *header_in, unsigned long size_in, char *data_in)
{
	block_ptr = 0; 
	memcpy(header, header_in, 4);
	header[4] = 0;
	size = size_in;
	data = new char[size];
	memcpy(data,data_in,size);
}
