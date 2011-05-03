//////////////////////////////////////////////////////////////////////////////////////////
//	BITSET.h
//	class declaration for set of bits to represent many true/falses
//	Downloaded from: www.paulsprojects.net
//	Created:	8th August 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef BITSET_H
#define BITSET_H

#include "memory.h"
#include <stdio.h>
#include <string.h>
#include <CubicVR/Logger.h>


class BITSET
{
public:
	BITSET() : numBytes(0), bits(NULL)
	{}
	~BITSET()
	{
		if(bits)
			delete [] bits;
		bits=NULL;
	}
	
	bool Init(int numberOfBits)
	{
		//Delete any memory allocated to bits
		if(bits)
			delete [] bits;
		bits=NULL;
		
		//Calculate size
		numBytes=(numberOfBits>>3)+1;
		
		//Create memory
		bits=new unsigned char[numBytes];
		if(!bits)
		{
			Logger::log(LOG_ERROR,"BITSET: Unable to allocate space for a bitset of %d bits", numberOfBits);
			return false;
		}
		
		ClearAll();
		
		return true;
	}
	
	inline void ClearAll()
	{
		memset(bits, 0, numBytes);
	}
	
	inline void SetAll()
	{
		memset(bits, 0xFF, numBytes);
	}
	
	inline void Clear(int bitNumber)
	{
		bits[bitNumber>>3] &= ~(1<<(bitNumber & 7));
	}
	
	inline void Set(int bitNumber)
	{
		bits[bitNumber>>3] |= 1<<(bitNumber&7);
	}
	
	inline unsigned char IsSet(int bitNumber)
	{
		return bits[bitNumber>>3] & 1<<(bitNumber&7);
	}
	
protected:
	int numBytes;	//size of bits array
	unsigned char * bits;
};

#endif