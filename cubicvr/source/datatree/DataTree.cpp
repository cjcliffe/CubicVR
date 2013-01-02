/*
 * DataElement/DataNode/DataTree -- structured serialization/unserialization system 
 * designed for the CoolMule project :)
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
 */

#include <CubicVR/datatree/DataTree.h>
#include <CubicVR/StringUtil.h>
#include <CubicVR/Logger.h>
#include <fstream>

/* DataElement class */


DataElement::DataElement()
{
	data_type = DATA_NULL;
	data_val = NULL;
	data_size = 0;
}


DataElement::~DataElement()
{
	if (data_val) delete data_val;
}


void DataElement::data_init(long data_size_in)
{
	if (data_val) delete data_val;
	data_size = data_size_in;
	data_val = new char[data_size];
}


int DataElement::getDataType()
{
	return data_type;
}


long DataElement::getDataSize()
{
	return data_size;
}

//void DataElement::set(const bool &bool_in)
//{
//	data_type = DATA_BOOL;
//	data_init(sizeof(bool));
//	memcpy(data_val,&bool_in,data_size);
//}

void DataElement::set(const int &int_in)
{
	data_type = DATA_INT;
	data_init(sizeof(int));
	memcpy(data_val,&int_in,data_size);
}


void DataElement::set(const long &long_in)
{
	data_type = DATA_LONG;
	data_init(sizeof(long));
	memcpy(data_val,&long_in,data_size);
}


void DataElement::set(const float &float_in)
{
	data_type = DATA_FLOAT;
	data_init(sizeof(float));
	memcpy(data_val,&float_in,data_size);
}


void DataElement::set(const double &double_in)
{
	data_type = DATA_DOUBLE;
	data_init(sizeof(double));
	memcpy(data_val,&double_in,data_size);
}


void DataElement::set(const char *data_in, long size_in)
{
	data_type = DATA_FLOAT;
	data_init(size_in);
	memcpy(data_val,data_in,data_size);
}


void DataElement::set(const char *data_in)
{
	data_type = DATA_CHAR;
	data_init(strlen(data_in)+1);
	memcpy(data_val,data_in,data_size);
}


void DataElement::set(const string &str_in)
{
	data_type = DATA_STRING;
	data_init(str_in.length()+1);
	memcpy(data_val,str_in.c_str(),data_size);
}

void DataElement::set(const XYZ &xyz_in)
{
	data_type = DATA_XYZ;
	data_init(sizeof(cvrFloat)*3);
	
	cvrFloat xyz_dbl[3];
	xyz_dbl[0] = xyz_in.x;
	xyz_dbl[1] = xyz_in.y;
	xyz_dbl[2] = xyz_in.z;
	
	memcpy(data_val,xyz_dbl,data_size);
}


void DataElement::set(vector<XYZ> &xyzvect_in)
{
	vector<XYZ>::iterator i;
	long vectsize;
	long ptr;
	
	data_type = DATA_XYZVECTOR;
	
	vectsize = xyzvect_in.size()*sizeof(cvrFloat)*3;
	
	data_init(vectsize);
	
	ptr = 0;
	
	cvrFloat xyz_dbl[3];
	
	for (i = xyzvect_in.begin(); i != xyzvect_in.end(); i++)
	{		
		xyz_dbl[0] = (*i).x;
		xyz_dbl[1] = (*i).y;
		xyz_dbl[2] = (*i).z;
		
		memcpy(data_val+ptr, xyz_dbl, sizeof(cvrFloat)*3);
		ptr += sizeof(cvrFloat)*3;		
	}	
}


void DataElement::set(vector<string> &strvect_in)
{
	vector<string>::iterator i;
	long vectsize;
	long ptr;
	
	data_type = DATA_STRVECTOR;
    
	vectsize = 0;
	
	for (i = strvect_in.begin(); i != strvect_in.end(); i++)
	{
		vectsize += (*i).length()+1;
	}
	
	data_init(vectsize);
    
	ptr = 0;
    
	for (i = strvect_in.begin(); i != strvect_in.end(); i++)
	{		
		int str_length;
		
		str_length = (*i).length()+1;
		
		memcpy(data_val+ptr, (*i).c_str(), str_length);
		ptr += str_length;		
	}	
}


void DataElement::set(std::set<string> &strset_in)
{
	std::set<string>::iterator i;
	vector<string> tmp_vect;
	
	for (i = strset_in.begin(); i != strset_in.end(); i++)
	{
		tmp_vect.push_back(*i);
	}
    
	set(tmp_vect);
}



void DataElement::set(vector<int> &intvect_in)
{
	long ptr;
	int temp_int;
    
	data_type = DATA_INTVECTOR;
	
	data_init(sizeof(int)*intvect_in.size());
	
	vector<int>::iterator i;
	
	ptr = 0;
	
	for (i = intvect_in.begin(); i != intvect_in.end(); i++)
	{
		temp_int = *i;
		memcpy(data_val+ptr, &temp_int, sizeof(int));
		ptr += sizeof(int);
	}
}


void DataElement::set(vector<long> &longvect_in)
{
	long ptr;
	long temp_long;
    
	data_type = DATA_LONGVECTOR;
	
	data_init(sizeof(long)*longvect_in.size());
	
	vector<long>::iterator i;
	
	ptr = 0;
	
	for (i = longvect_in.begin(); i != longvect_in.end(); i++)
	{
		temp_long = *i;
		memcpy(data_val+ptr, &temp_long, sizeof(long));
		ptr += sizeof(long);
	}
}


void DataElement::set(vector<float> &floatvect_in)
{
	long ptr;
	float temp_float;
    
	data_type = DATA_FLOATVECTOR;
	
	data_init(sizeof(float)*floatvect_in.size());
	
    //	vector<float>::iterator i;
    //	
    //	ptr = 0;
    //	
    //	for (i = floatvect_in.begin(); i != floatvect_in.end(); i++)
    //	{
    //		temp_float = *i;
    //		memcpy(data_val+ptr, &temp_float, sizeof(float));
    //		ptr += sizeof(float);
    //	}
	
	memcpy(data_val,&floatvect_in[0],sizeof(float)*floatvect_in.size());
}


void DataElement::set(vector<double> &doublevect_in)
{
	long ptr;
	double temp_double;
    
	data_type = DATA_DOUBLEVECTOR;
	
	data_init(sizeof(double)*doublevect_in.size());
	
	vector<double>::iterator i;
	
	ptr = 0;
	
	for (i = doublevect_in.begin(); i != doublevect_in.end(); i++)
	{
		temp_double = *i;
		memcpy(data_val+ptr, &temp_double, sizeof(double));
		ptr += sizeof(double);
	}
}

//void DataElement::get(bool &bool_in) throw (DataTypeMismatchException)
//{
//	if (!data_type) return;
//	if (data_type != DATA_BOOL) throw(new DataTypeMismatchException("Type mismatch, not a BOOL"));
//
//	memcpy(&bool_in, data_val, data_size);
//}

void DataElement::get(int &int_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
	if (data_type != DATA_INT && data_type != DATA_LONG) throw(new DataTypeMismatchException("Type mismatch, not a INT"));
	
	if (data_type == DATA_LONG)
	{
		long tmp_long;
		memcpy(&tmp_long, data_val, data_size);
		int_in = (int)tmp_long;
	}
	else
	{
		memcpy(&int_in, data_val, data_size);
	}
}


void DataElement::get(long &long_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
	if (data_type != DATA_LONG && data_type != DATA_INT) throw(new DataTypeMismatchException("Type mismatch, not a LONG"));
	
	if (data_type == DATA_INT)
	{	
		int tmp_int;
		memcpy(&tmp_int, data_val, data_size);
		long_in = (long)tmp_int;
	}
	else
	{
		memcpy(&long_in, data_val, data_size);
	}
}


void DataElement::get(float &float_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
	if (data_type != DATA_FLOAT && data_type != DATA_DOUBLE && data_type != DATA_INT  && data_type != DATA_LONG) throw(new DataTypeMismatchException("Type mismatch, not a FLOAT"));
    
	if (data_type == DATA_DOUBLE)
	{
		float tmp_double;
		memcpy(&tmp_double, data_val, data_size);
		float_in = (float)tmp_double;
	}
	else if (data_type == DATA_LONG)
	{
		long tmpInt;
		get(tmpInt);
		float_in = (float)tmpInt;
	}
	else if (data_type == DATA_INT)
	{
		int tmpInt;
		get(tmpInt);
		float_in = (float)tmpInt;
	}
	else
	{
		memcpy(&float_in, data_val, data_size);
	}
}


void DataElement::get(double &double_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
	if (data_type != DATA_DOUBLE && data_type != DATA_FLOAT) throw(new DataTypeMismatchException("Type mismatch, not a DOUBLE"));
    
	if (data_type == DATA_FLOAT)
	{
		float tmp_float;
		memcpy(&tmp_float, data_val, data_size);
		double_in = (double)tmp_float;
	}
	else
	{
		memcpy(&double_in, data_val, data_size);
	}
}


void DataElement::get(char **data_in) throw (DataTypeMismatchException)
{
	if (data_type != DATA_CHAR) throw(new DataTypeMismatchException("Type mismatch, not a CHAR"));
	*data_in = new char[data_size];
	memcpy(*data_in, data_val, data_size);
}


void DataElement::get(string &str_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
    
	if (data_type != DATA_STRING && data_type != DATA_CHAR) throw(new DataTypeMismatchException("Type mismatch, not a STRING"));
    
	if (!str_in.empty())	// flush the string
	{
		str_in.erase(str_in.begin(),str_in.end());
	}
    
	str_in.append(data_val);
}


void DataElement::get(vector<string> &strvect_in) throw (DataTypeMismatchException)
{
	long ptr;
	if (!data_type) return;
	
	if (data_type != DATA_STRVECTOR) throw(new DataTypeMismatchException("Type mismatch, not a STRING VECTOR"));
    
	ptr = 0;
    
	while (ptr != data_size)
	{
		strvect_in.push_back(string(data_val+ptr));
		ptr += strlen(data_val+ptr)+1;
	}
	
}

void DataElement::get(std::set<string> &strset_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
	
	if (data_type != DATA_STRVECTOR) throw(new DataTypeMismatchException("Type mismatch, not a STRING VECTOR/SET"));
    
	std::vector<string> tmp_vect;
	std::vector<string>::iterator i;
    
	get(tmp_vect);
	
	for (i = tmp_vect.begin(); i != tmp_vect.end(); i++)
	{
		strset_in.insert(*i);
	}
}



void DataElement::get(vector<int> &intvect_in) throw (DataTypeMismatchException)
{
	long ptr;
	if (!data_type) return;
	
	if (data_type != DATA_INTVECTOR && data_type != DATA_LONGVECTOR) throw(new DataTypeMismatchException("Type mismatch, not a INT VECTOR"));
	
	ptr = 0;
    
	if (data_type == DATA_LONGVECTOR)
	{
		long temp_long;
		while  (ptr < data_size)
		{
			temp_long = 0;
			memcpy(&temp_long,data_val+ptr,sizeof(long));
			intvect_in.push_back((int)temp_long);
			ptr += sizeof(long);
		}
	}
	else
	{
		int temp_int;
		while  (ptr < data_size)
		{
			temp_int = 0;
			memcpy(&temp_int,data_val+ptr,sizeof(int));
			intvect_in.push_back(temp_int);
			ptr += sizeof(int);
		}
	}
}


void DataElement::get(vector<long> &longvect_in) throw (DataTypeMismatchException)
{
	long ptr;
	if (!data_type) return;
	
	if (data_type != DATA_LONGVECTOR && data_type != DATA_INTVECTOR) throw(new DataTypeMismatchException("Type mismatch, not a LONG VECTOR"));
	
	ptr = 0;
	
	if (data_type == DATA_LONGVECTOR)
	{
		long temp_long;
		while  (ptr < data_size)
		{
			temp_long = 0;
			memcpy(&temp_long,data_val+ptr,sizeof(long));
			longvect_in.push_back(temp_long);
			ptr += sizeof(long);
		}
	}
	else
	{
		int temp_int;
		while  (ptr < data_size)
		{
			temp_int = 0;
			memcpy(&temp_int,data_val+ptr,sizeof(int));
			longvect_in.push_back((long)temp_int);
			ptr += sizeof(int);
		}
	}
}


void DataElement::get(vector<float> &floatvect_in) throw (DataTypeMismatchException)
{
	long ptr;
	if (!data_type) return;
    
	if (data_type != DATA_FLOATVECTOR && data_type != DATA_DOUBLEVECTOR  && data_type != DATA_INTVECTOR && data_type != DATA_LONGVECTOR) throw(new DataTypeMismatchException("Type mismatch, not a FLOAT VECTOR"));
    
	ptr = 0;
    
	if (data_type == DATA_DOUBLEVECTOR)
	{
		float temp_double;
		while  (ptr < data_size)
		{
			temp_double = 0;
			memcpy(&temp_double,data_val+ptr,sizeof(double));
			floatvect_in.push_back((float)temp_double);
			ptr += sizeof(double);
		}
	}
    if (data_type == DATA_INTVECTOR)
	{
		int temp_int;
		while  (ptr < data_size)
		{
			temp_int = 0;
			memcpy(&temp_int,data_val+ptr,sizeof(int));
			floatvect_in.push_back((float)temp_int);
			ptr += sizeof(int);
		}
	}
	else
	{		
		float temp_float;
		while  (ptr < data_size)
		{
			temp_float = 0;
			memcpy(&temp_float,data_val+ptr,sizeof(float));
			floatvect_in.push_back(temp_float);
			ptr += sizeof(float);
		}
	}
}

void DataElement::get(vector<double> &doublevect_in) throw (DataTypeMismatchException)
{
	long ptr;
	if (!data_type) return;
	
	if (data_type != DATA_DOUBLEVECTOR && data_type != DATA_FLOATVECTOR) throw(new DataTypeMismatchException("Type mismatch, not a DOUBLE VECTOR"));
	
	ptr = 0;
	
	if (data_type == DATA_FLOATVECTOR)
	{
		float temp_float;
		while  (ptr < data_size)
		{
			temp_float = 0;
			memcpy(&temp_float,data_val+ptr,sizeof(float));
			doublevect_in.push_back((double)temp_float);
			ptr += sizeof(float);
		}
	}
	else
	{
		double temp_double;
        
		while  (ptr < data_size)
		{
			temp_double = 0;
			memcpy(&temp_double,data_val+ptr,sizeof(double));
			doublevect_in.push_back(temp_double);
			ptr += sizeof(double);
		}
	}
}



void DataElement::get(XYZ &xyz_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
	cvrFloat xyz_dbl[3];
	
	if (data_type != DATA_XYZ) throw(new DataTypeMismatchException("Type mismatch, not a XYZ"));
	
	memcpy(&xyz_dbl, data_val, data_size);
	
	xyz_in = XYZ(xyz_dbl[0],xyz_dbl[1],xyz_dbl[2]);
}


void DataElement::get(vector<XYZ> &xyzvect_in) throw (DataTypeMismatchException)
{
	if (!data_type) return;
	XYZ temp_xyz;
	cvrFloat dbl_xyz[3];
	
	long ptr;
    
	if (data_type != DATA_XYZVECTOR) throw(new DataTypeMismatchException("Type mismatch, not a XYZ VECTOR"));
    
	ptr = 0;
    
	while (ptr < data_size)
	{
		memcpy(&dbl_xyz,data_val+ptr,sizeof(cvrFloat)*3);
		xyzvect_in.push_back(XYZ(dbl_xyz[0],dbl_xyz[1],dbl_xyz[2]));
		ptr += sizeof(cvrFloat)*3;
	}
}


long DataElement::getSerializedSize()
{
	return sizeof(unsigned char)+sizeof(unsigned int)+data_size;	
}


long DataElement::getSerialized(char **ser_str)
{
	long ser_size = getSerializedSize();
	
	*ser_str = new char[ser_size];
	
	char *ser_pointer;
	
    ser_pointer = *ser_str;
	
	memcpy(ser_pointer, &data_type, sizeof(unsigned char));
    ser_pointer+=sizeof(unsigned char);
	memcpy(ser_pointer, &data_size, sizeof(unsigned int));
    ser_pointer+=sizeof(unsigned int);
	memcpy(ser_pointer, data_val, data_size);
    
	return ser_size;	
}


void DataElement::setSerialized(char *ser_str)
{
	char *ser_pointer = ser_str;
    
	memcpy(&data_type,ser_pointer,sizeof(unsigned char));
    ser_pointer+=sizeof(unsigned char);
	memcpy(&data_size,ser_pointer,sizeof(unsigned int));
    ser_pointer+=sizeof(unsigned int);
    
	data_init(data_size);
	memcpy(data_val,ser_pointer,data_size);
}


/* DataNode class */

DataNode::DataNode()
{
	ptr = 0;
    parentNode = NULL;
}


DataNode::DataNode(const char *name_in)
{
	ptr = 0;
	node_name = name_in;
    parentNode = NULL;
}


DataNode::~DataNode()
{ 
	for (vector<DataNode *>::iterator i = children.begin(); i != children.end(); i++)
	{
		delete *i;
	}
}


void DataNode::setName(const char *name_in)
{
	node_name = name_in;
}


DataElement &DataNode::element() 
{ 
	return data_elem; 
}


DataNode &DataNode::newChild(const char *name_in)
{
	children.push_back(new DataNode(name_in));
	childmap[name_in].push_back(children.back());
    
    children.back()->setParentNode(*this);
    
	return *children.back();
}


DataNode &DataNode::child(const char *name_in, int index) throw (DataInvalidChildException)
{
	DataNode *child_ret;
    
	child_ret = childmap[name_in][index];
	
	if (!child_ret) 
	{
		stringstream error_str;
		error_str << "no child '" << index << "' in DataNode '" << node_name << "'";
		throw (DataInvalidChildException(error_str.str().c_str()));
	}
	
	return *child_ret;
}


DataNode &DataNode::child(int index) throw (DataInvalidChildException)
{
    
	DataNode *child_ret;
	
	child_ret = children[index];
	
	if (!child_ret) 
	{
		stringstream error_str;
		error_str << "no child '" << index << "' in DataNode '" << node_name << "'";
		throw (DataInvalidChildException(error_str.str().c_str()));
	}
	
	return *child_ret;
}


int DataNode::numChildren()
{
	return children.size();
}


int DataNode::numChildren(const char *name_in)
{
	return childmap[name_in].size();
}


bool DataNode::hasAnother()
{
	return children.size() != ptr;
}


bool DataNode::hasAnother(const char *name_in)
{
	return childmap[name_in].size() != childmap_ptr[name_in];
}


DataNode &DataNode::getNext() throw (DataInvalidChildException)
{
	return child(ptr++);
}


DataNode &DataNode::getNext(const char *name_in) throw (DataInvalidChildException)
{
	return child(name_in,childmap_ptr[name_in]++);
}


void DataNode::rewind()
{
	ptr = 0;
}

void DataNode::rewind(const char *name_in)
{
	childmap_ptr[name_in] = 0;
}


/* DataTree class */

DataTree::DataTree(const char *name_in)
{
	dn_root.setName(name_in);
}

DataTree::DataTree()
{
	
}

DataTree::~DataTree() { };		


DataNode &DataTree::rootNode() 
{ 
	return dn_root;
}


/*
 
 
 
 GameObject *newGO = createGO(filename);
 if(newGO)
 {
 printf("Loading GameObject from XML\n");
 TiXmlDocument doc( filename.c_str() );
 if(doc.LoadFile())
 {
 printf("Loaded XML\n");
 TiXmlNode *root = doc.FirstChild( "gameobject" );
 if(root)
 {
 printf("Found root\n");
 TiXmlNode *goc = root->FirstChild( "goc" );
 
 while(goc)
 {
 printf("Game object\n");
 
 TiXmlElement *gocEl = goc->ToElement();
 std::string type = gocEl->Attribute("type");
 
 if(type == "gocMesh")
 {
 printf("MESH COMPONENT!\n");
 
 GameObjectComponent *gocComponent = new gocMesh();
 newGO->addGOC(gocComponent);
 
 gocMesh *mesh = static_cast<gocMesh *>(gocComponent);
 mesh->load_xml(goc);
 mesh->bind(scene);
 }
 else if(type == "gocRigid")
 {
 printf("RIGID COMPONENT!\n");
 
 GameObjectComponent *mRigid = new gocRigid();
 newGO->addGOC(mRigid);
 
 gocRigid *rigid = static_cast<gocRigid *>(mRigid);
 rigid->load_xml(goc);
 rigid->bind((RigidScene *)scene);
 }
 else if(type == "gocLight")
 {
 printf("LIGHT COMPONENT!\n");
 
 GameObjectComponent *mLight = new gocLight();
 newGO->addGOC(mLight);
 
 gocLight *light = static_cast<gocLight *>(mLight);
 light->load_xml(goc);
 light->bind(scene);
 }
 else if(type == "gocCamera")
 {
 printf("CAMERA COMPONENT!\n");
 
 GameObjectComponent *mCamera = new gocCamera();
 newGO->addGOC(mCamera);
 
 gocCamera *camera = static_cast<gocCamera *>(mCamera);
 camera->load_xml(goc);
 camera->bind(scene);
 }
 else if(type == "gocTerrain")
 {
 printf("TERRAIN COMPONENT!\n");
 
 GameObjectComponent *mTerrain = new gocTerrain();
 newGO->addGOC(mTerrain);
 
 gocTerrain *terrain = static_cast<gocTerrain *>(mTerrain);
 terrain->load_xml(goc);
 terrain->bind((RigidScene *)scene);
 }
 else if(type == "gocScript")
 {
 printf("SCRIPT COMPONENT!\n");
 
 GameObjectComponent *mScript = new gocScript();
 newGO->addGOC(mScript);
 
 gocScript *script = static_cast<gocScript *>(mScript);
 script->load_xml(goc);
 //script->bind(scene);
 }
 
 
 printf("Type: %s\n", type.c_str());
 goc = goc->NextSibling();
 }
 }
 else
 {
 printf("No root\n");
 }
 //printf("%s\n", root->Value());
 }
 else
 {
 printf("Couldn't load: %s\n", filename.c_str());
 }
 
 
 void write_simple_doc2( )
 {
 // same as write_simple_doc1 but add each node
 // as early as possible into the tree.
 
 TiXmlDocument doc;
 TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
 doc.LinkEndChild( decl );
 
 TiXmlElement * element = new TiXmlElement( "Hello" );
 doc.LinkEndChild( element );
 
 TiXmlText * text = new TiXmlText( "World" );
 element->LinkEndChild( text );
 
 dump_to_stdout( &doc );
 doc.SaveFile( "madeByHand2.xml" );
 }
 
 
 */

std::string trim(std::string& s,const std::string& drop = " ")
{
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}


void DataTree::decodeXMLText(DataNode *elem, const char *src_text, DT_FloatingPointPolicy fpp) {
    
	long tmp_long;
    int tmp_int;
   	double tmp_double;
    float tmp_float;
	string tmp_str;
	string tmp_str2;
	std::stringstream tmp_stream;
	std::stringstream tmp_stream2;
	XYZ tmp_xyz;
	
	vector<long> tmp_longvect;
	vector<long>::iterator tmp_longvect_i;
	vector<int> tmp_intvect;
	vector<double> tmp_doublevect;
	vector<double>::iterator tmp_doublevect_i;
	vector<float> tmp_floatvect;
	vector<XYZ> tmp_xyzvect;
	
    bool vInts = false;

    string in_text = src_text;
    
    trim(in_text);
    trim(in_text,"\r\n");
    tmp_stream.clear();
    tmp_stream2.clear();
    
    if (in_text.find_first_not_of("0123456789-") == string::npos)
    {
        tmp_stream << in_text;
        tmp_stream >> tmp_long;
        
        tmp_int = tmp_long;
        
        if (tmp_int == tmp_long) {
            elem->element().set((int)tmp_int);                        
        } else {
            elem->element().set((long)tmp_long);                        
        }                    
    }
    else if (in_text.find_first_not_of("0123456789.e+-") == string::npos)
    {
        tmp_stream << in_text;
        
        if (fpp == USE_FLOAT) {
            tmp_stream >> tmp_float;
            
            elem->element().set((float)tmp_float);
        } else {
            tmp_stream >> tmp_double;
            
            elem->element().set((double)tmp_double);
        }
    }
    else if (in_text.find_first_not_of("0123456789.,e+-") == string::npos)
    {
        str_replace(","," ",in_text);
        tmp_stream << in_text;
        tmp_stream >> tmp_xyz.x;
        tmp_stream >> tmp_xyz.y;
        tmp_stream >> tmp_xyz.z;
        
        elem->element().set(tmp_xyz);
    }
    else if (in_text.find_first_not_of("0123456789- ") == string::npos)
    {
        tmp_stream << in_text;
        
        vInts = true;
        
        while (!tmp_stream.eof())
        {
            tmp_stream >> tmp_long;
            tmp_int = tmp_long;
            if (tmp_int != tmp_long) {
                vInts = false; 
                //                        printf("Failed int: [%d] != [%ld]\n",tmp_int,tmp_long);
            }
            tmp_longvect.push_back((long)tmp_long);
        }
        
        if (vInts) {
            tmp_intvect.clear();
            for (tmp_longvect_i = tmp_longvect.begin(); tmp_longvect_i != tmp_longvect.end(); tmp_longvect_i++)
            {
                tmp_intvect.push_back(*tmp_longvect_i);
            }
            tmp_longvect.clear();
            elem->element().set(tmp_intvect);
        } else {
            elem->element().set(tmp_longvect);
        }
    }
    else if (in_text.find_first_not_of("0123456789.e-+ ") == string::npos)
    {
        tmp_stream << in_text;
        
        if (fpp == USE_FLOAT) {
            tmp_floatvect.clear();
        } else {
            tmp_doublevect.clear();
        }
        
        while (!tmp_stream.eof())
        {
            
            if (fpp == USE_FLOAT) {
                tmp_stream >> tmp_float;
                tmp_floatvect.push_back(tmp_float);
            } else {
                tmp_stream >> tmp_double;
                tmp_doublevect.push_back(tmp_double);
            }
        }
        
        if (fpp == USE_FLOAT) {
            elem->element().set(tmp_floatvect);
        } else {
            elem->element().set(tmp_doublevect);
        }
    }
    else if (in_text.find_first_not_of("0123456789.,e-+ ") == string::npos)
    {
        tmp_stream << in_text;
        
        while (!tmp_stream.eof())
        {
            tmp_stream >> tmp_str2;
            
            str_replace(","," ",tmp_str2);
            
            tmp_stream2 << tmp_str2;
            
            tmp_stream2 >> tmp_xyz.x;
            tmp_stream2 >> tmp_xyz.y;
            tmp_stream2 >> tmp_xyz.z;
            
            tmp_stream2.clear();
            
            tmp_xyzvect.push_back(tmp_xyz);
        }
        
        elem->element().set(tmp_xyzvect);
    }
    else
    {
        elem->element().set(src_text);
        //					printf( "Unhandled DataTree XML Field: [%s]", tmp_str.c_str() );	
    }
    
}


void DataTree::setFromXML(DataNode *elem, TiXmlNode *elxml, bool root_node, DT_FloatingPointPolicy fpp)
{
	TiXmlText *pText;
	int t = elxml->Type();
 	string tmp_str;
    
	switch ( t )
	{
		case TiXmlNode::DOCUMENT:
            //				printf( "Document" );
			break;
			
		case TiXmlNode::ELEMENT:
            if (!root_node) 
                elem = &elem->newChild(elxml->Value());
            
            const TiXmlAttribute *attribs;
            attribs = elxml->ToElement()->FirstAttribute();
            
            while (attribs) {
                //Logger::log(LOG_DEBUG,"attrib value: @%s = %s\n\n",attribs->Name(), attribs->Value());  
                
                // following badgerfish xml->json and xml->ruby convention for attributes..
                string attrName("@");
                attrName.append(attribs->Name());
 
                decodeXMLText(&elem->newChild(attrName.c_str()), attribs->Value(), fpp);

                attribs = attribs->Next();
            }
            
            //				printf( "Element \"%s\"", elxml->Value());
			break;
			
		case TiXmlNode::COMMENT:
            //				printf( "Comment: \"%s\"", elxml->Value());
			break;
			
		case TiXmlNode::UNKNOWN:
            //				printf( "Unknown" );
			break;
			
		case TiXmlNode::TEXT:
            pText = elxml->ToText();
 			
            decodeXMLText(elem, pText->Value(), fpp);
			
            //				pText = elxml->ToText();
            //				printf( "Text: [%s]", pText->Value() );
 			break;
			
		case TiXmlNode::DECLARATION:
            //				printf( "Declaration" );
            break;
		default:
			break;
	}
    
    //	printf( "\n" );
	
	
	TiXmlNode * pChild;
	
	if (!elxml->NoChildren())
	{
		if (elxml->FirstChild()->Type() == TiXmlNode::ELEMENT)
		{
			if (elxml->FirstChild()->Value() == TIXML_STRING("str"))
			{
				std::vector<std::string> tmp_strvect;
				
				for ( pChild = elxml->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
				{
					if (pChild->Value() == TIXML_STRING("str"))
					{
						if (!pChild->FirstChild()) 
						{
							tmp_strvect.push_back("");
							continue;
						}
						
						pText = pChild->FirstChild()->ToText();						
                        
						if (pText)
						{
							tmp_str = pText->Value();
							tmp_strvect.push_back(tmp_str);
						}
					}
				}
				
				elem->element().set(tmp_strvect);
                
				return;
			}
		}
	}
	
	for ( pChild = elxml->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		setFromXML( elem, pChild, false, fpp );
	}
	
}


void DataTree::nodeToXML(DataNode *elem, TiXmlElement *elxml)
{
	DataNode *child;
    
	elem->rewind();
	
	while (elem->hasAnother()) 
	{
		child = &elem->getNext();
		
		std::string nodeName = child->getName();
		
		TiXmlElement *element;
        
		element = new TiXmlElement(nodeName.length()?nodeName.c_str():"node");
		std::string tmp;
		std::stringstream tmp_stream;
		TiXmlText *text;
		XYZ tmp_xyz;
		std::vector<XYZ> tmp_xyzvect;
		std::vector<XYZ>::iterator tmp_xyzvect_i;
		std::vector<float> tmp_floatvect;
		std::vector<float>::iterator tmp_floatvect_i;
		std::vector<double> tmp_doublevect;
		std::vector<double>::iterator tmp_doublevect_i;
		std::vector<int> tmp_intvect;
		std::vector<int>::iterator tmp_intvect_i;
		std::vector<long> tmp_longvect;
		std::vector<long>::iterator tmp_longvect_i;
		std::vector<string> tmp_stringvect;
		std::vector<string>::iterator tmp_stringvect_i;
		TiXmlElement *tmp_node;
		char *tmp_pstr;
		double tmp_double;
		float tmp_float;
		long tmp_long;
		int tmp_int;
        
        
        
		switch (child->element().getDataType())
		{
			case DATA_NULL:
				break;
			case DATA_INT:
				child->element().get(tmp_int);
				
				tmp_stream.clear();
				
				tmp_stream << tmp_int;
				
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				break;
			case DATA_LONG:
				child->element().get(tmp_long);
				
				tmp_stream.clear();
				
				tmp_stream << tmp_long;
				
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				break;
			case DATA_FLOAT:
				child->element().get(tmp_float);
				
				tmp_stream.clear();
				
				tmp_stream << tmp_float;
				
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				break;
			case DATA_DOUBLE:
				child->element().get(tmp_double);
				
				tmp_stream.clear();
                
				tmp_stream << tmp_double;
                
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				break;
			case DATA_CHAR:
                child->element().get(&tmp_pstr);
                // following badgerfish xml->json and xml->ruby convention for attributes..
                if (nodeName.substr(0,1)==string("@")) {
                    elxml->SetAttribute(nodeName.substr(1).c_str(), tmp_pstr);
                    delete element;
                    element = NULL;
                } else {
                    text = new TiXmlText(tmp_pstr);
                    element->LinkEndChild(text);
                }
                delete tmp_pstr;
                break;
                
			case DATA_STRING:
                child->element().get(tmp);
                if (nodeName.substr(0,1)==string("@")) {
                    elxml->SetAttribute(nodeName.substr(1).c_str(), tmp.c_str());
                    delete element;
                    element = NULL;
                } else {
                    text = new TiXmlText(tmp.c_str());
                    element->LinkEndChild(text);
                }
                break;
				
			case DATA_STRVECTOR:
				child->element().get(tmp_stringvect);
				
				tmp_stream.clear();
				
				for (tmp_stringvect_i = tmp_stringvect.begin(); tmp_stringvect_i != tmp_stringvect.end(); tmp_stringvect_i++)
				{
					tmp_node = new TiXmlElement("str");
					text = new TiXmlText((*tmp_stringvect_i).c_str());
					tmp_node->LinkEndChild(text);
					element->LinkEndChild(tmp_node);
				}
                
				tmp_stringvect.clear();
				break;
			case DATA_INTVECTOR:
				child->element().get(tmp_intvect);
				
				tmp_stream.clear();
				
				for (tmp_intvect_i = tmp_intvect.begin(); tmp_intvect_i != tmp_intvect.end(); tmp_intvect_i++)
				{
					tmp_stream << (*tmp_intvect_i);
					if (tmp_intvect_i != tmp_intvect.end()-1) tmp_stream << " ";
				}
				
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				tmp_intvect.clear();
				break;
			case DATA_LONGVECTOR:
				child->element().get(tmp_longvect);
				
				tmp_stream.clear();
				
				for (tmp_longvect_i = tmp_longvect.begin(); tmp_longvect_i != tmp_longvect.end(); tmp_longvect_i++)
				{
					tmp_stream << (*tmp_longvect_i);
					if (tmp_longvect_i != tmp_longvect.end()-1) tmp_stream << " ";
				}
				
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				tmp_longvect.clear();
				break;
			case DATA_FLOATVECTOR:
				child->element().get(tmp_floatvect);
				
				tmp_stream.clear();
				
				for (tmp_floatvect_i = tmp_floatvect.begin(); tmp_floatvect_i != tmp_floatvect.end(); tmp_floatvect_i++)
				{
					tmp_stream << (*tmp_floatvect_i);
					if (tmp_floatvect_i != tmp_floatvect.end()-1) tmp_stream << " ";
				}
				
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				tmp_floatvect.clear();
				break;
			case DATA_DOUBLEVECTOR:
				child->element().get(tmp_doublevect);
				
				tmp_stream.clear();
				
				for (tmp_doublevect_i = tmp_doublevect.begin(); tmp_doublevect_i != tmp_doublevect.end(); tmp_doublevect_i++)
				{
					tmp_stream << (*tmp_doublevect_i);
					if (tmp_doublevect_i != tmp_doublevect.end()-1) tmp_stream << " ";
				}
				
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				tmp_doublevect.clear();
				break;
                
			case DATA_XYZ:
				child->element().get(tmp_xyz);
                
				tmp_stream.clear();
				tmp_stream << tmp_xyz.x;
				tmp_stream << ",";
				tmp_stream << tmp_xyz.y;
				tmp_stream << ",";
				tmp_stream << tmp_xyz.z;
                
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
                break;
                
			case DATA_XYZVECTOR:
				child->element().get(tmp_xyzvect);
                
				tmp_stream.clear();
                
				for (tmp_xyzvect_i = tmp_xyzvect.begin(); tmp_xyzvect_i != tmp_xyzvect.end(); tmp_xyzvect_i++)
				{
					tmp_stream << (*tmp_xyzvect_i).x;
					tmp_stream << ",";
					tmp_stream << (*tmp_xyzvect_i).y;
					tmp_stream << ",";
					tmp_stream << (*tmp_xyzvect_i).z;
					if (tmp_xyzvect_i != tmp_xyzvect.end()-1) tmp_stream << " ";
				}
                
				text = new TiXmlText(tmp_stream.str().c_str());
				element->LinkEndChild(text);
				tmp_xyzvect.clear();
                break;				
		}
		
        if (element) {
            elxml->LinkEndChild(element);
            
            if (child->numChildren())
            {
                nodeToXML(child,element);
            }
        }
	}
    
	
	elem->rewind();
}


void DataTree::printXML()	/* get serialized size + return node names header */
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
    
	DataNode *root = &rootNode();
    
    string rootName = root->getName();
    
	TiXmlElement *element = new TiXmlElement(rootName.empty()?"root":rootName.c_str());
	doc.LinkEndChild( element );
	
	if (!root->numChildren()) doc.Print();
	
	nodeToXML(root,element);
	
	root->rewind();
	
	doc.Print();
}




long DataTree::getSerializedSize(DataElement &de_node_names, bool debug)	/* get serialized size + return node names header */
{
	long total_size = 0;
    
	stack<DataNode *> dn_stack;
	vector<string> node_names;
	map<string, int, string_less> node_name_index_map;
	
	DataElement de_name_index;	// just used for sizing purposes
	DataElement de_num_children;
    
	de_name_index.set((int)0);
	de_num_children.set((int)0);
    
	int de_name_index_size = de_name_index.getSerializedSize();
	int de_num_children_size = de_num_children.getSerializedSize();
	
    
	dn_stack.push(&dn_root);
    
	while (!dn_stack.empty())
	{
        int name_index;
        //					int num_children;
        
        /* build the name list */
        if (dn_stack.top()->getName().empty())
        {
            name_index = 0;	/* empty string */
        }
        else if (node_name_index_map[dn_stack.top()->getName().c_str()] == 0)
        {
            node_names.push_back(string(dn_stack.top()->getName()));
            name_index = node_names.size();
            node_name_index_map[dn_stack.top()->getName().c_str()] = name_index;
        }
        else
        {
            name_index = node_name_index_map[dn_stack.top()->getName().c_str()];
        }
        
        /* add on the size of the name index and number of children */
        total_size += de_name_index_size;
        total_size += de_num_children_size;
        total_size += dn_stack.top()->element().getSerializedSize();
        
        /* debug output */
        if (debug)
        {
            for (unsigned int i = 0; i < dn_stack.size()-1; i++)	cout << "--";
            cout << (dn_stack.top()->getName().empty()?"NULL":dn_stack.top()->getName()) << "(" << dn_stack.top()->element().getSerializedSize() << ")";
            cout << " type: " << dn_stack.top()->element().getDataType() << endl;
            //cout << " index: " << name_index << endl;
        }
        /* end debug output */
        
        /* if it has children, traverse into them */
        if (dn_stack.top()->hasAnother())
        {
            dn_stack.push(&dn_stack.top()->getNext());
            dn_stack.top()->rewind();
        }
        else
        {
            /* no more children, back out until we have children, then add next child to the top */
            while (!dn_stack.empty())
            {
                if (!dn_stack.top()->hasAnother()) 
                {
                    dn_stack.top()->rewind();
                    dn_stack.pop();
                }
                else break;
            }
            
            if (!dn_stack.empty())
            {
                dn_stack.push(&dn_stack.top()->getNext());
                dn_stack.top()->rewind();	
            }
        }
	}		
	
	/* set the header for use in serialization */
	de_node_names.set(node_names);
	
	total_size += de_node_names.getSerializedSize();
	
	return total_size;
}

void DataNode::findAll(const char *name_in, vector<DataNode *> &node_list_out)
{
	stack<DataNode *> dn_stack;

  	/* start at the root */
	dn_stack.push(this);
    
    if (string(getName()) == string(name_in)) node_list_out.push_back(this);
    
	while (!dn_stack.empty())
	{
        while (dn_stack.top()->hasAnother(name_in)) {
            node_list_out.push_back(&dn_stack.top()->getNext(name_in));
        }
            
        
        /* if it has children, traverse into them */
        if (dn_stack.top()->hasAnother())
        {
            dn_stack.push(&dn_stack.top()->getNext());
            dn_stack.top()->rewind();
        }
        else
        {
            /* no more children, back out until we have children, then add next child to the top */
            while (!dn_stack.empty())
            {
                if (!dn_stack.top()->hasAnother()) 
                {
                    dn_stack.top()->rewind();
                    dn_stack.pop();
                }
                else break;
            }
            
            if (!dn_stack.empty())
            {
                dn_stack.push(&dn_stack.top()->getNext());
                dn_stack.top()->rewind();	
            }
        }
	}		
  
}


long DataTree::getSerialized(char **ser_str, bool debug)
{
	long data_ptr = 0;
	long data_size = 0;
    
	stack<DataNode *> dn_stack;
	vector<string> node_names;
	map<string, int, string_less> node_name_index_map;
	
	/* header of node names, grabbed from getserializedsize to avoid having to memmove() or realloc() */
	DataElement de_node_names;
	
	data_size = getSerializedSize(de_node_names,debug);
	
	*ser_str = (char *)malloc(data_size);
	
	char *data_out = *ser_str;
	
	/* name list header */
	char *de_node_names_serialized;
	long de_node_names_serialized_size;
	
	de_node_names.getSerialized(&de_node_names_serialized);
	de_node_names_serialized_size = de_node_names.getSerializedSize();
    
	/* copy the header and increase the pointer */
	memcpy(data_out,de_node_names_serialized,de_node_names_serialized_size);
	data_ptr += de_node_names_serialized_size;
	
    //    if (debug) Logger::log(LOG_DEBUG,"serialized header size: %d",de_node_names_serialized_size);
    
	/* start at the root */
	dn_stack.push(&dn_root);
    
	while (!dn_stack.empty())
	{
        int name_index;
        int num_children;
        
        DataElement de_name_index;
        DataElement de_num_children;
        
        char *de_name_index_serialized;
        char *de_num_children_serialized;
        char *element_serialized;
        
        long de_name_index_serialized_size;
        long de_num_children_serialized_size;
        long element_serialized_size;
        
        /* build the name list */
        if (dn_stack.top()->getName().empty())
        {
            name_index = 0;	/* empty string */
        }
        else if (node_name_index_map[dn_stack.top()->getName().c_str()] == 0)
        {
            node_names.push_back(string(dn_stack.top()->getName()));
            name_index = node_names.size();
            node_name_index_map[dn_stack.top()->getName().c_str()] = name_index;
        }
        else
        {
            name_index = node_name_index_map[dn_stack.top()->getName().c_str()];
        }					
        
        num_children = dn_stack.top()->numChildren();
        
        de_name_index.set(name_index);
        de_num_children.set(num_children);
        
        de_name_index_serialized_size = de_name_index.getSerializedSize();
        de_num_children_serialized_size = de_num_children.getSerializedSize();
        element_serialized_size = dn_stack.top()->element().getSerializedSize();
        
        de_name_index.getSerialized(&de_name_index_serialized);
        de_num_children.getSerialized(&de_num_children_serialized);
        dn_stack.top()->element().getSerialized(&element_serialized);
        
        /* add on the name index and number of children */
        memcpy(data_out+data_ptr,de_name_index_serialized,de_name_index_serialized_size);
        data_ptr += de_name_index_serialized_size;
        
        memcpy(data_out+data_ptr,de_num_children_serialized,de_num_children_serialized_size);
        data_ptr += de_num_children_serialized_size;
        
        /* add on the data element */
        memcpy(data_out+data_ptr,element_serialized,element_serialized_size);
        data_ptr += element_serialized_size;
        
        delete de_name_index_serialized;
        delete de_num_children_serialized;
        delete element_serialized;
        
        /* if it has children, traverse into them */
        if (dn_stack.top()->hasAnother())
        {
            dn_stack.push(&dn_stack.top()->getNext());
            dn_stack.top()->rewind();
        }
        else
        {
            /* no more children, back out until we have children, then add next child to the top */
            while (!dn_stack.empty())
            {
                if (!dn_stack.top()->hasAnother()) 
                {
                    dn_stack.top()->rewind();
                    dn_stack.pop();
                }
                else break;
            }
            
            if (!dn_stack.empty())
            {
                dn_stack.push(&dn_stack.top()->getNext());
                dn_stack.top()->rewind();	
            }
        }
	}		
	
	return data_size;
}

void DataTree::setSerialized(char *ser_str, bool debug)
{
	long data_ptr = 0;
    //	long data_size = 0;
    
	stack<DataNode *> dn_stack;
	stack<int> dn_childcount_stack;
	vector<string> node_names;
	
	DataElement de_node_names;
	
	de_node_names.setSerialized(ser_str);
	data_ptr+=de_node_names.getSerializedSize();
	de_node_names.get(node_names);
	
	DataElement de_name_index;
	DataElement de_num_children;
	DataElement de_element;
    
	dn_stack.push(&dn_root);
	dn_childcount_stack.push(0);	/* root (parent null) has no siblings */
    
    
	/* unserialization is a little less straightforward since we have to do a countdown of remaining children */
	while (!dn_stack.empty())
	{
		int name_index;
		int num_children;
        
		/* pull the index of the name of this node */
		de_name_index.setSerialized(ser_str+data_ptr);
		data_ptr += de_name_index.getSerializedSize();
		
		/* pull the number of children this node has */
		de_num_children.setSerialized(ser_str+data_ptr);
		data_ptr += de_num_children.getSerializedSize();
        
		/* get values from the temp dataelements */
		de_name_index.get(name_index);
		de_num_children.get(num_children);
        
		/* pull the node's element */
		dn_stack.top()->element().setSerialized(ser_str+data_ptr);
		data_ptr += dn_stack.top()->element().getSerializedSize();
        
		/* debug output */
		if (debug)
		{
			for (unsigned int i = 0; i < dn_stack.size()-1; i++) cout << "--";
			cout << (name_index?node_names[name_index-1]:"NULL") << "(" << dn_stack.top()->element().getSerializedSize() << ")";
			cout << " index: " << name_index << endl;
		}
		/* end debug output */
        
        
		/* name index >= 1 means it has a name */
		if (name_index) 
		{
			dn_stack.top()->setName(node_names[name_index-1].c_str());
			
		}
		else /* name is nil */
		{
			dn_stack.top()->setName("");
		}
        
		if (num_children) /* Has children, create first child and push it to the top */
		{	
			dn_childcount_stack.push(num_children); /* push the child count onto the stack */
            
			de_name_index.setSerialized(ser_str+data_ptr);	/* peek at the new child name but don't increment pointer */
			de_name_index.get(name_index);
			/* add this child onto the top of the stack */
			dn_stack.push(&dn_stack.top()->newChild((name_index?node_names[name_index-1]:string("")).c_str())); 
			dn_childcount_stack.top()--;	/* decrement to count the new child */
		}
		else /* No children, move on to the next sibling */
		{
			if (dn_childcount_stack.top()) /* any siblings remaining? */
			{
				de_name_index.setSerialized(ser_str+data_ptr);	/* peek at the new child name but don't increment pointer */
				de_name_index.get(name_index);
                
				dn_stack.pop();	
				dn_stack.push(&dn_stack.top()->newChild((name_index?node_names[name_index-1]:string("")).c_str())); /* create the next sibling and throw it on the stack */
				dn_childcount_stack.top()--;	/* decrement to count the new sibling */
			}
			else	/* This is the last sibling, move up the stack and find the next */
			{
				while (!dn_stack.empty()) /* move up the stack until we find the next sibling */
				{
					if (dn_childcount_stack.top())
					{
						de_name_index.setSerialized(ser_str+data_ptr);	/* peek at the new child name but don't increment pointer */
						de_name_index.get(name_index);
                        
						dn_stack.pop();	
						dn_stack.push(&dn_stack.top()->newChild((name_index?node_names[name_index-1]:string("")).c_str()));	/* throw it on the stack */
						dn_childcount_stack.top()--; /* count it */
						break;
					}
					else
					{
						dn_childcount_stack.pop();
						dn_stack.pop();	/* if no more siblings found the stack will empty naturally */
					}
				}
			}
		}
	}
}


bool DataTree::LoadFromFileXML(const std::string& filename, DT_FloatingPointPolicy fpp)
{
	TiXmlDocument doc(filename.c_str());
	bool loadOkay = doc.LoadFile();
	
	if (!loadOkay)
	{
		Logger::log(LOG_ERROR,"LoadFromFileXML[error loading]: \"%s\"\n",filename.c_str());
		return false;
	}
	
	TiXmlNode *xml_root_node = doc.RootElement();
	
	if (!xml_root_node)
	{
		Logger::log(LOG_ERROR,"LoadFromFileXML[error no root]: \"%s\"\n",filename.c_str());
		return false;
	}
	
    rootNode().setName(xml_root_node->ToElement()->Value());
    
	setFromXML(&rootNode(),xml_root_node,true,fpp);
    
	return true;
}


bool DataTree::SaveToFileXML(const std::string& filename)
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
    
    string rootName = rootNode().getName();
    
    TiXmlElement *element = new TiXmlElement(rootName.empty()?"root":rootName.c_str());
    
    doc.LinkEndChild( element );
    
    nodeToXML(&rootNode(),element);
    
    doc.SaveFile(filename.c_str());
  
	return true;
}

/*
bool DataTree::SaveToFile(const std::string& filename)
{
    char *serialized;

    long dataSize = getSerialized(&serialized);

    std::ofstream fout(filename.c_str(), ios::binary);

    fout.write(serialized, dataSize);

    fout << flush;
    fout.close();
    
    delete serialized;
    
    return true;
}

bool DataTree::LoadFromFile(const std::string& filename)
{
 	char *serialized;
	long dataSize;
	
	ifstream fin(filename.c_str(), ios::binary);
    
    fin.seekg (0, ios::end);
    dataSize = fin.tellg();
    fin.seekg (0, ios::beg);
   	
	serialized = new char[dataSize];
	fin.read(serialized,dataSize);
	
	fin.close();
	
	setSerialized(serialized);
	
	delete serialized;	
    
	return true;
}
*/


bool DataTree::SaveToFile(const std::string& filename, bool compress, int compress_level)
{
	long dataSize, compressedSize, headerSize;  
    char *serialized, *hdr_serialized, *compressed;
    DataTree dtHeader;
    
    dataSize = getSerialized(&serialized);
    
    if (compress) {
        compressed = new char[(int)ceil(dataSize*1.5)];
        
        compressedSize = fastlz_compress_level(compress_level, serialized, dataSize, compressed);
        
        compressed = (char *)realloc(compressed, compressedSize);
        
        delete serialized;
    }
    
    
    DataNode &header = dtHeader.rootNode();
    
    header^"version" = 1.0f;
    header^"compression" = string(compress?"FastLZ":"none");
    header^"uncompressed_size" = dataSize;
    
    headerSize = dtHeader.getSerialized(&hdr_serialized);
    
    std::ofstream fout(filename.c_str(), ios::binary);
    
    fout.write((char *) &headerSize, sizeof(long));
	fout.write((char *) &(compress?compressedSize:dataSize), sizeof(long));
    
    fout.write(hdr_serialized, headerSize);
    fout.write(compress?compressed:serialized, compress?compressedSize:dataSize);
    
    fout << flush;
    fout.close();
    
    delete hdr_serialized;
    
    if (!compress) {
        delete serialized;   
    } else {
        delete compressed;   
    }
    
    return true;
}


bool DataTree::LoadFromFile(const std::string& filename)
{
    char *compressed,*serialized,*hdr_serialized;
	long dataSize,headerSize,compressedSize;
	
	ifstream fin(filename.c_str(), ios::binary);
	
	fin.read((char *)&headerSize, sizeof(long));
	fin.read((char *)&compressedSize, sizeof(long));
    
    hdr_serialized = new char[headerSize];
	fin.read(hdr_serialized,headerSize);
    
    DataTree dtHeader;
	dtHeader.setSerialized(hdr_serialized);
    DataNode &header = dtHeader.rootNode();
    
    string compressionType = header["compression"];
    dataSize = header["uncompressed_size"];
    
    bool uncompress = false;
    
    if (compressionType=="FastLZ") { 
        uncompress = true; 
    }
    
    if (uncompress) {
        compressed = new char[compressedSize];
        fin.read(compressed,compressedSize);
        
        serialized = new char[dataSize];
        fastlz_decompress(compressed, compressedSize, serialized, dataSize);
        
        delete compressed;
    } else {
        serialized = new char[dataSize];
        fin.read(serialized,dataSize);
    }
    
	fin.close();
    
	setSerialized(serialized);
	
	delete serialized;
	delete hdr_serialized;
	
	return true;
}


