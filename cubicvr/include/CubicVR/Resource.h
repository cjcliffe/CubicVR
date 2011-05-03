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
#pragma once


#include <CubicVR/datatree/DataTree.h>
#include <string>
#include <fstream>


class Resource
{
protected:
	DataTree *properties;
	DataTree *header;
	
	std::string idName;
	std::string typeName;
	std::string bundleName;
	std::string sourceFile;
	const std::string tmpString;
	bool loaded;
	bool initialized;
	class ResourceManager *rm;
	
public:
	
	Resource();
	~Resource();
	DataTree &getHeader();
	DataTree &getProperties();
	void Dump();
	
	// read file type and id name without actually opening it
	static bool GetFileHeader(const std::string& filename, DataTree &fileHeader_out);
	// load file header without opening properties data
	bool LoadFileHeader(const std::string& filename);
	bool LoadFileProperties();
	bool LoadFromFile(const std::string& filename);
	bool LoadFromFileXML(const std::string& filename);
	bool SaveToFileXML(const std::string& filename);	
	bool SaveToFile(const std::string& filename);	
	bool Import(Resource &res_import);
	void UnLink();
	
	std::string &getSourceFile();
	
	bool headerLoaded();
	bool propertiesLoaded();
	
	virtual void setId(const std::string &id_in);
	virtual void setType(const std::string &typeName_in);
	virtual void setBundle(const std::string &bundleName_in);
	
	virtual std::string getId();
	virtual std::string getType();
	virtual std::string getSubType();
	virtual std::string getBundle();
	
	virtual bool isLoaded();
	virtual bool isInitialized();
	
	virtual bool init();
	virtual bool destroy();
	virtual bool reload();
	
	virtual bool onLoad();
	virtual bool onSave();
	
	void setManager(class ResourceManager *rm_in);
	class ResourceManager *getManager();
	
	inline void setInit(bool init_in) { initialized = init_in; };
};

