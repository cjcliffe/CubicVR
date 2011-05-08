#ifndef DATATREE_H
#define DATATREE_H

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


#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <stack>
#include <iostream>
#include <CubicVR/XYZ.h>
#include <CubicVR/map_string.h>
#include <CubicVR/tinyxml/tinyxml.h>


using namespace std;


/* type defines */
#define DATA_NULL					0
#define DATA_INT					1
#define DATA_LONG					2
#define DATA_FLOAT				3
#define DATA_DOUBLE				4
#define DATA_CHAR					5
#define DATA_STRING				6
#define DATA_STRVECTOR		7
#define DATA_INTVECTOR		8
#define DATA_LONGVECTOR		9
#define DATA_FLOATVECTOR		10
#define DATA_DOUBLEVECTOR		11
#define DATA_XYZ				12
#define DATA_XYZVECTOR			13
//#define DATA_BOOL				14


/* Data Exceptions */
class DataException
{
private:
    string reason;
    
public:
    DataException(const char *why) : reason(why) {}
    string what() { return reason; } 
    operator string() { return reason; }
};


class DataTypeMismatchException : public DataException
{
public:
    DataTypeMismatchException(const char *why) : DataException(why) { }
};


class DataInvalidChildException : public DataException
{
public:
    DataInvalidChildException(const char *why) : DataException(why) { }
};


class DataElement
{
private:
    short int data_type;
    long data_size;
    
    char *data_val;
    
    void data_init(long data_size_in);
    
public:
    DataElement();
    ~DataElement();
    
    int getDataType();
    long getDataSize();
    
    /* set overloads */		
    //		void set(const bool &bool_in);
    void set(const int &int_in);
    void set(const long &long_in);
    void set(const float &float_in);
    void set(const double &double_in);
    
    void set(const char *data_in, long size_in); /* voids, file chunks anyone? */
    void set(const char *data_in);	/* strings, stops at NULL, returns as string */
    
    void set(const string &str_in);
    
    void set(vector<string> &strvect_in);
    void set(std::set<string> &strset_in);
    void set(vector<int> &intvect_in);
    void set(vector<long> &longvect_in);
    void set(vector<float> &floatvect_in);
    void set(vector<double> &doublevect_in);
    void set(const XYZ &xyz_in);
    void set(vector<XYZ> &xyzvect_in);
    
    
    /* get overloads */
    //		void get(bool &bool_in) throw (DataTypeMismatchException);
    void get(int &int_in) throw (DataTypeMismatchException);
    void get(long &long_in) throw (DataTypeMismatchException);
    void get(float &float_in) throw (DataTypeMismatchException);
    void get(double &double_in) throw (DataTypeMismatchException);
    void get(XYZ &xyz_in) throw (DataTypeMismatchException);
    
    void get(char **data_in) throw (DataTypeMismatchException); /* getting a void or string */
    void get(string &str_in) throw (DataTypeMismatchException); 
    void get(std::set<string> &strset_in) throw (DataTypeMismatchException);
    
    void get(vector<string> &strvect_in) throw (DataTypeMismatchException);
    void get(vector<int> &intvect_in) throw (DataTypeMismatchException);
    void get(vector<long> &longvect_in) throw (DataTypeMismatchException);
    void get(vector<float> &floatvect_in) throw (DataTypeMismatchException);
    void get(vector<double> &doublevect_in) throw (DataTypeMismatchException);
    void get(vector<XYZ> &xyzvect_in) throw (DataTypeMismatchException);
    
    
    /* special get functions, saves creating unnecessary vars */
    //		int getBool() throw (DataTypeMismatchException) { bool i_get; get(i_get); return i_get; };
    int getInt() throw (DataTypeMismatchException) { int i_get; get(i_get); return i_get; };
    long getLong() throw (DataTypeMismatchException)  { long l_get; get(l_get); return l_get; };
    float getFloat() throw (DataTypeMismatchException)  { float f_get; get(f_get); return f_get; };
    double getDouble() throw (DataTypeMismatchException)  { double d_get; get(d_get); return d_get; };
    
    
    /* serialize functions */
    long getSerializedSize();
    long getSerialized(char **ser_str);
    
    void setSerialized(char *ser_str);
};


class DataNode
{
private:
    vector<DataNode *> children;
    map<string, vector<DataNode *>, string_less> childmap;
    map<string, unsigned int, string_less> childmap_ptr;
    
    string node_name;
    DataElement data_elem;
    unsigned int ptr;
    
    
public:
    DataNode();
    DataNode(const char *name_in);
    
    ~DataNode();		
    
    void setName(const char *name_in);
    string &getName() { return node_name; }
    
    int numChildren();	/* Number of children */
    int numChildren(const char *name_in); /* Number of children named 'name_in' */
    
    DataElement &element(); /* DataElement at this node */
    
    DataNode &newChild(const char *name_in);
    DataNode &child(const char *name_in, int index = 0) throw (DataInvalidChildException);
    DataNode &child(int index) throw (DataInvalidChildException);
    
    bool hasAnother(const char *name_in);	/* useful for while() loops in conjunction with getNext() */
    bool hasAnother();
    DataNode &getNext(const char *name_in) throw (DataInvalidChildException); /* get next of specified name */
    DataNode &getNext() throw (DataInvalidChildException);	/* get next child */
    void rewind(const char *name_in);	/* rewind specific */
    void rewind();	/* rewind generic */
};


class DataTree
{
private:
    DataNode dn_root;
    
public:
    DataTree(const char *name_in);
    DataTree();
    ~DataTree();		
    
    DataNode &rootNode();
    
    void nodeToXML(DataNode *elem, TiXmlElement *elxml);
    void setFromXML(DataNode *elem, TiXmlNode *elxml, bool root_node=true);
    
    void printXML();	/* print datatree as XML */
    long getSerializedSize(DataElement &de_node_names, bool debug=false);	/* get serialized size + return node names header */
    long getSerialized(char **ser_str, bool debug=false);	
    void setSerialized(char *ser_str, bool debug=false);
    
    bool LoadFromFileXML(const std::string& filename);
    bool SaveToFileXML(const std::string& filename);
    bool SaveToFile(const std::string& filename);
    bool LoadFromFile(const std::string& filename);
};


#endif