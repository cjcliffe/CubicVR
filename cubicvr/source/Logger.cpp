/*
 *  Logger.cpp
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


#include <CubicVR/Logger.h>


LogHook::LogHook()
{
	Logger::addHook(this);
}

LogHook::~LogHook()
{
	Logger::removeHook(this);
}

void LogHook::doLog(logLevel lev, const char *log_str)
{

}



std::set<LogHook *> Logger::logSet;
bool Logger::std_out_val = true;

void Logger::log(const char *format, ...)
{
	va_list ap;
	static char strtmp[4096];
	
	va_start(ap, format);
	vsnprintf(strtmp, sizeof(strtmp), format, ap);
	va_end(ap);
	
	doLog(LOG_DEBUG,strtmp);
}


void Logger::log(std::string &log_str)
{
	
	doLog(LOG_DEBUG, log_str.c_str());
	
}


void Logger::log(logLevel lev, const char *format, ...)
{
	va_list ap;
	static char strtmp[4096];
	
	va_start(ap, format);
	vsnprintf(strtmp, sizeof(strtmp), format, ap);
	va_end(ap);
	
	doLog(lev, strtmp);
}



void Logger::log(logLevel lev, std::string &log_str)
{
	
	doLog(lev, log_str.c_str());
	
}


void Logger::doLog(logLevel lev, const char *log_str)
{
	if (!logSet.empty())
	{
		std::set<LogHook *>::iterator logSet_i;	
		
		for (logSet_i = logSet.begin(); logSet_i != logSet.end(); logSet_i++)
		{
			(*logSet_i)->doLog(lev, log_str);
		}
	}
		
	if (std_out_val)
	{
		printf("%s",log_str);
	}
		
}


void Logger::setStandardOutput(bool val)
{
	std_out_val = val;
}


void Logger::addHook(LogHook *logger)
{
	logSet.insert(logger);
}

void Logger::removeHook(LogHook *logger)
{
	logSet.erase(logger);
}
