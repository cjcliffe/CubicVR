/*
 *  Logger.h
 *  CubicVR
 *
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
 *
 */

#pragma once

#include <string>
#include <set>
#include <stdio.h>
#include <stdarg.h>

typedef enum logLevel
{
		LOG_DEBUG  = 0,
		LOG_WARNING = 1,
		LOG_ERROR = 2,
		LOG_NOTICE = 3
};

class LogHook
{
public:
	LogHook();
	~LogHook();
	
	virtual void doLog(logLevel lev, const char *log_str);
};


//class LogFileHook : public LogHook
//{
//public:
//	LogFileHook(char *fn);
//	doLog(char *log_str);
//};


class Logger
{
private:
	static bool std_out_val;
	static std::set<LogHook *> logSet;
	static void doLog(logLevel lev, const char *log_str);
	
public:
	static void addHook(LogHook *logger);
	static void removeHook(LogHook *logger);
	
	static void log(const char *format, ...);
	static void log(std::string &log_str);

	static void log(logLevel lev, const char *format, ...);
	static void log(logLevel lev, std::string &log_str);
	
	static void setStandardOutput(bool val);
};