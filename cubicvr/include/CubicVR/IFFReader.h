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

#ifndef IFF_H
#define IFF_H

#include <CubicVR/cvr_defines.h>

#include "CubicVR/XYZ.h"
#include "CubicVR/Vector.h"

#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>


using namespace std;

class IMPEXP IFFBlock
{

public:

	char header[5];
	unsigned long size;
	unsigned long block_ptr;
	char *data;

	IFFBlock() { size = 0; block_ptr = 0; data = NULL; }
	IFFBlock(char *header_in, unsigned long size_in, char *data_in);
	~IFFBlock() { if (data) delete data; }
	
	
	long getLong() 
	{ 
		long long_out = longbuf2long(data+block_ptr); 
		block_ptr+=4; 
		return long_out;
	}
	
	float getFloat() 
	{  
		float float_out = floatbuf2float(data+block_ptr);
		block_ptr+=4; 
		return float_out;
	}

	short getShort() 
	{  
		short short_out = shortbuf2short(data+block_ptr);
		block_ptr+=2;
		return short_out;
	}
	
	void getVector(XYZ &xyz_out) 
	{  
		vectbuf2vect(xyz_out,data+block_ptr); block_ptr+=12;	
	}
	
	unsigned long getVx(int &vxsize) 
	{  
		unsigned long vx_out;
		vx_out = vxbuf2vx(data+block_ptr,vxsize); 
		block_ptr+=vxsize;	
		return vx_out;
	}
	
	unsigned long getVx() 
	{  
		unsigned long vx_out;
		int vxsize;
		vx_out = vxbuf2vx(data+block_ptr,vxsize); 
		block_ptr+=vxsize; 
		return vx_out;
	}
	
	void getHeader(char *header) 
	{ 
		memcpy(header,data+block_ptr,4); 
		header[4] = 0; 
		block_ptr+=4; 
	}

	void getString(string &str_out) 
	{ 
		int len; 
		char *temp = data+block_ptr; 
		
		len = strlen(temp)+1; if (len % 2) len++; 
		block_ptr+=len;
		
		str_out = string(temp);
	}
	
	bool finished() { return block_ptr >= size; }

	char *data_ptr() { return data+block_ptr; }
	long remainder() { return size-block_ptr; }

	static long longbuf2long(char *in_buf);	
	static float floatbuf2float(char *in_buf);
	static short shortbuf2short(char *in_buf);
	static void vectbuf2vect(XYZ &xyz_out, char *in_buf);
	static unsigned long vxbuf2vx(char *in_buf, int &vxsize);
};


class IMPEXP IFFReader
{

private:

	vector<IFFBlock *> iffData;
	

public:

	char header[5];

	IFFReader() { }
	IFFReader(char *in_data, long data_size, int ptr_size = 4);

	static bool isIFF(const string &filename);

	bool load(const string &filename);
	
	void processIFFBlock(char *in_data, long data_size, int ptr_size);
	void processIFFBlockWithHeader(char *in_data, long data_size, int ptr_size);
	void processIFFBlockWithHeader(char *in_data, long data_size);

	IFFBlock *getBlock(char *header_name);
	IFFBlock *getBlock(char *header_name, int blockCount);
	void getBlockNames(std::vector<string> &blockList);
};


#endif
