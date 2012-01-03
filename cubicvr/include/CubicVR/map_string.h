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

#ifndef CUBICVR_MAPSTRING
#define CUBICVR_MAPSTRING

#include <string>
#include <algorithm>
#include <map>
#include <functional> 
#include <CubicVR/StringUtil.h>

/* map comparison function */
struct string_less : public std::binary_function<std::string,std::string,bool> 
{
 bool operator()(const std::string& a,const std::string& b) const
 {
  return a.compare(b) < 0;
 }
};

/* int comparison function */
struct int_less : public std::binary_function<int,int,bool> 
{
 bool operator()(int a,int b) const
 {
 	return a < b;
 }
};


class MapString
{
public:
	virtual const char *getString(const char *str_ref) = 0;
};


class ExtensionMapString
{
private:
	std::map<string,string> extMap;
public:
	const char *getString(const char *str_ref) {
		string path_in(str_ref),path_str,file_str,file_base,file_ext;
		str_file_extract(path_in, path_str, file_str, file_base, file_ext);
		std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

		printf("string expanded to: path: %s, file: %s, base: %s, ext: %s\n",path_str.c_str(),file_str.c_str(),file_base.c_str(),file_ext.c_str());

		if (extMap.find(file_ext) != extMap.end()) {
			file_ext = extMap[file_ext];
			if (path_str.length()) {
				return (path_str+string(PATH_SEP)+file_base+string(".")+file_ext).c_str();
			} else {
				return (file_base+string(".")+file_ext).c_str();
			}
		}

		return str_ref;
	}

	void addTranslation(string from, string to) {
		string fromExt(from);
		std::transform(from.begin(), from.end(), from.begin(), ::tolower);
		extMap[fromExt] = to;
	}

};

#endif

