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


#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


//#include <errno.h>



#include <CubicVR/map_string.h>

#ifdef WIN32
	#define RESOURCEPOOL_PATH_CHAR "\\"
#else
	#define RESOURCEPOOL_PATH_CHAR "/"
#endif

#define RESOURCEPOOL_MAX_PATH 4096

using namespace std;

class MapStringPool;

class ResourcePool
{
public:	
	map<string, vector<string>, string_less> fileMap;
	map<string, vector<string>, string_less> extMap;
	set<string> pathList;
	map<string,bool> recList;
	string tmp_str;
	MapStringPool *strMap;

	ResourcePool();	
	~ResourcePool();	
	
	const char *findByFilename(const char *fn);		
	void findByFilename(const char *fn, std::vector<std::string> &fileList);
	void findByExtension(const char *ext, std::vector<std::string> &fileList);
	int getSize(const char *dirname);	
	int scanDir(const char *dirname,char *entries);
	void refresh();
	void addPath(const char *dirname, bool recursive=false);
	void clearPaths();
	int scanString(char *string,char *buffer);
	std::string StringToLower(std::string str);
	MapString &getMap();
};

class MapStringPool : public MapString
{
private:
	const char *tmp;
	std::string tmpStr;
	
public:
	ResourcePool *pool;
	
	MapStringPool(ResourcePool &pool_in) { pool = &pool_in; };
	const char *getString(const char *str_ref) 
	{ 
		//	printf("requested: %s\n",str_ref);		
		
		tmp = pool->findByFilename(str_ref); 
		tmpStr.clear();
		tmpStr.append(tmp);
		
		//	printf("returned: %s\n",tmpStr.c_str());
		return tmpStr.c_str();
	};
};


