/*
 *  Resource.cpp
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


#include <CubicVR/Resource.h>
#include <CubicVR/Logger.h>
#include <CubicVR/ResourceManager.h>


Resource::Resource() : loaded(false), initialized(false), idName(""), typeName("resource"), properties(NULL), header(NULL)
{
//	idName = std::string("unnamed");
//	loaded = false;
//	initialized = false;
//	typeName = std::string("null");
//	properties = NULL;
//	header = NULL;
}

Resource::~Resource()
{
//	if (header) delete header;
//	if (properties) delete properties;
}


DataTree &Resource::getHeader()
{
	if (!header) header = new DataTree;
	
	return *header;
}

DataTree &Resource::getProperties()
{
	if (!properties) properties = new DataTree;
	
	return *properties;
}


void Resource::Dump()
{
	Logger::log(LOG_NOTICE,"Resource[%x] Type: %s, Name: %s\n",this,typeName.c_str(),idName.c_str());
}


// read file type and id name without actually opening it
bool Resource::GetFileHeader(const std::string& filename, DataTree &fileHeader_out)
{
	char *hdr_serialized;
	long dataSize,headerSize;
	
	ifstream fin(filename.c_str(), ios::binary);
	
	fin.read((char *)&headerSize, sizeof(long));
	fin.read((char *)&dataSize, sizeof(long));
	
	hdr_serialized = new char[headerSize];
	fin.read(hdr_serialized,headerSize);
	
	fileHeader_out.setSerialized(hdr_serialized);
	
	fin.close();
	
	delete hdr_serialized;

	return true;
}

// load file header without opening properties data
bool Resource::LoadFileHeader(const std::string& filename)
{
	char *hdr_serialized;
	long dataSize,headerSize;
	
	sourceFile = filename;
	
	ifstream fin(filename.c_str(), ios::binary);
	
	fin.read((char *)&headerSize, sizeof(long));
	fin.read((char *)&dataSize, sizeof(long));
	
	hdr_serialized = new char[headerSize];
	fin.read(hdr_serialized,headerSize);
	
	if (header) delete header;
	header = new DataTree();
	
	header->setSerialized(hdr_serialized);
	
	fin.close();
	
	delete hdr_serialized;

	return true;
}


bool Resource::LoadFileProperties()
{
	if (!sourceFile.length())
	{
		return false;
	}
	
	return LoadFromFile(sourceFile);
}

bool Resource::LoadFromFile(const std::string& filename)
{
	char *serialized,*hdr_serialized;
	long dataSize,headerSize;
	
	sourceFile = filename;
	
	ifstream fin(filename.c_str(), ios::binary);
	
	fin.read((char *)&headerSize, sizeof(long));
	fin.read((char *)&dataSize, sizeof(long));
	
	hdr_serialized = new char[headerSize];
	fin.read(hdr_serialized,headerSize);
	
	serialized = new char[dataSize];
	fin.read(serialized,dataSize);
	
	fin.close();
	
	if (properties) delete properties;
	if (header) delete header;
	
	header = new DataTree();
	header->setSerialized(hdr_serialized);
	
	header->rootNode().child("type").element().get(typeName);
	header->rootNode().child("id").element().get(idName);
	header->rootNode().child("bundle").element().get(bundleName);
	//		header->rootNode().child("version").element().get(1.0);
	
	properties = new DataTree();
	properties->setSerialized(serialized);
	
	delete serialized;
	delete hdr_serialized;
	
	loaded = onLoad();

	return true;
}

bool Resource::LoadFromFileXML(const std::string& filename)
{
	sourceFile = filename;
	
	TiXmlDocument doc(filename.c_str());
	bool loadOkay = doc.LoadFile();
	
	if (!loadOkay)
	{
		Logger::log(LOG_ERROR,"LoadFromFileXML[error loading]: \"%s\"\n",filename.c_str());
		return false;
	}
	
	TiXmlNode *xml_root_node = doc.FirstChild("root");
	
	if (!xml_root_node)
	{
		Logger::log(LOG_ERROR,"LoadFromFileXML[error no root]: \"%s\"\n",filename.c_str());
		return false;
	}
	
	TiXmlNode *xml_header_node = xml_root_node->FirstChild( "header" );
	TiXmlNode *xml_properties_node = xml_root_node->FirstChild( "properties" );
	
	if (!xml_header_node)
	{
		Logger::log(LOG_ERROR,"LoadFromFileXML[error no header]: \"%s\"\n",filename.c_str());
		return false;
	}
	
	if (!xml_properties_node)
	{
		Logger::log(LOG_ERROR,"LoadFromFileXML[error no properties]: \"%s\"\n",filename.c_str());
		return false;
	}
	
	
	header = new DataTree();
	header->setFromXML(&header->rootNode(),xml_header_node);
	//	header->setSerialized(hdr_serialized);
	
	if (header->rootNode().hasAnother("type")) header->rootNode().child("type").element().get(typeName);
	if (header->rootNode().hasAnother("id")) header->rootNode().child("id").element().get(idName);
	if (header->rootNode().hasAnother("bundle")) header->rootNode().child("bundle").element().get(bundleName);

	//	header->rootNode().child("version").element().get(1.0);
	
	properties = new DataTree();
	properties->setFromXML(&properties->rootNode(),xml_properties_node);
	
	loaded = onLoad();
	return true;
}


bool Resource::SaveToFileXML(const std::string& filename)
{
	sourceFile = filename;

	onSave();
	
	
	if (!header)
	{
		header = new DataTree;
		header->rootNode().newChild("type").element().set(typeName);
		header->rootNode().newChild("id").element().set(idName);
		header->rootNode().newChild("bundle").element().set(bundleName);
//		header->rootNode().newChild("version").element().set(1.0);
	}
	
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	
	TiXmlElement *element = new TiXmlElement("root");
	doc.LinkEndChild( element );
	
	//		if (!root->numChildren()) doc.Print();
	TiXmlElement *header_node = new TiXmlElement("header");
	element->LinkEndChild(header_node);
	
	header->nodeToXML(&header->rootNode(),header_node);
	
	TiXmlElement *properties_node = new TiXmlElement("properties");
	element->LinkEndChild(properties_node);
	
	properties->nodeToXML(&properties->rootNode(),properties_node);
	
	doc.SaveFile(filename.c_str());

	return true;
}

bool Resource::SaveToFile(const std::string& filename)
{
	sourceFile = filename;

	onSave();
	
	char *serialized,*hdr_serialized;
	long dataSize,headerSize;
	
	
	if (header) delete header;
	
	header = new DataTree;
	header->rootNode().newChild("type").element().set(typeName);
	header->rootNode().newChild("id").element().set(idName);
	header->rootNode().newChild("version").element().set(1.0);
	
	headerSize = header->getSerialized(&hdr_serialized);
	dataSize = properties->getSerialized(&serialized);
	
	std::ofstream fout(filename.c_str(), ios::binary);
	
	fout.write((char *) &headerSize, sizeof(long));
	fout.write((char *) &dataSize, sizeof(long));
	
	fout.write(hdr_serialized, headerSize);
	fout.write(serialized, dataSize);
	
	fout << flush;
	fout.close();
	
	delete serialized;
	delete hdr_serialized;
	
	return true;
}

bool Resource::headerLoaded()
{
	return header?true:false;
}

bool Resource::propertiesLoaded()
{
	return properties?true:false;
}

void Resource::setId(const std::string &id_in)
{
	idName.clear();
	idName.append(id_in);
}

void Resource::setType(const std::string &typeName_in)
{
	typeName.clear();
	typeName.append(typeName_in);
}

std::string Resource::getId()
{
	return idName;
};

std::string Resource::getBundle()
{
	return bundleName;
}

void Resource::setBundle(const std::string &bundleName_in)
{
	bundleName = bundleName_in;
}

std::string Resource::getType()
{
	if (int i = typeName.find_first_of(".") != string::npos)
	{

		return typeName.substr(0, i);
	}
	else
	{
		return typeName;
	}
};

std::string Resource::getSubType()
{
	return typeName;
};


bool Resource::onLoad() 
{
	return false;
};

bool Resource::onSave() 
{ 
	return false;
};


void Resource::UnLink() 
{ 
	header = NULL;
	properties = NULL;
};


bool Resource::isLoaded()
{
	return loaded;
};


bool Resource::isInitialized()
{
	return initialized;
};


bool Resource::reload()
{
	return true;
}


bool Resource::init()
{
	return true;
}

bool Resource::destroy()
{
	return true;
}


bool Resource::Import(Resource &res_import)
{
	header = res_import.header;
	properties = res_import.properties;
	sourceFile = res_import.sourceFile;
	typeName = res_import.typeName;
	idName = res_import.idName;
	bundleName = res_import.bundleName;
	
	return true;
};

std::string &Resource::getSourceFile()
{
	return sourceFile;
}


void Resource::setManager(ResourceManager *rm_in)
{
	rm = rm_in;
}

ResourceManager *Resource::getManager()
{
	return rm;
}
