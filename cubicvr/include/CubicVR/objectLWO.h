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

#ifndef CUBICVR_OBJECTLWO_H
#define CUBICVR_OBJECTLWO_H

#include <CubicVR/cvr_defines.h>

#include <CubicVR/IFFReader.h>
#include <CubicVR/Object.h>


#ifdef ARCH_PSP
#include <pspdebug.h>
#define printf pspDebugScreenPrintf 
#endif

class UVMapper;


IMPEXP void loadLWO(Object &obj, const string &fn, MapString *mapStr=NULL, map<Material *, UVMapper *> *mappers_out=NULL);
IMPEXP void getResourcesLWO(vector<string> &fnList, const string &fn);

#endif

