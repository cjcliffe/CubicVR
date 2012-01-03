/*

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

#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <vector>

#include <sstream>
#include <stdexcept>

using namespace std;

#if defined(_WIN32) || defined(_WIN64) || defined(WINDOWS)
	#define PATH_SEP "\\"
#else
	#define PATH_SEP "/"
#endif

void str_replace(const string &needle, const string &replacement, string &haystack);
void str_explode(vector<string> &vect_out, const string &seperator, const string &in_str);
void str_implode(string &str_out, const string &seperator, vector<string> &str_vect);
void str_file_extract(string path_in, string &path_str,string &file_str,string &file_base,string &file_ext);

class BadConversion : public std::runtime_error {
public:
	BadConversion(const std::string& s) : std::runtime_error(s)
	{
	}
};

/* convert double to std::string */
inline std::string stringify(double x)
{
	std::ostringstream o;

	if (!(o << x))
		throw BadConversion("stringify(double)");

	return o.str();
}

/* convert unsigned long to std::string */
inline std::string stringify(unsigned long x)
{
	std::ostringstream o;

	if (!(o << x))
		throw BadConversion("stringify(unsigned long)");

	return o.str();
}

/* convert double to std::string */
inline double convertToDouble(const std::string& s)
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		throw BadConversion("convertToDouble(\"" + s + "\")");
	return x;
}

/* convert unsigned long to std::string */
inline unsigned long convertToUnsignedLong(const std::string& s)
{
	std::istringstream i(s);
	unsigned long x;
	if (!(i >> x))
		throw BadConversion("convertToUnsignedLong(\"" + s + "\")");
	return x;
}

inline char *va(char *format, ...)
{
	va_list ap;
	static char strtmp[4096];
	
	va_start(ap, format);
	vsnprintf(strtmp, sizeof(strtmp), format, ap);
	va_end(ap);
	
	return strtmp;
}

#endif
