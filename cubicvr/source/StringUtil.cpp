
#include "CubicVR/StringUtil.h"



/* replace all occurances of needle with replacement in haystack */
void str_replace(const string &needle, const string &replacement, string &haystack)
{
	int needlepos, needle_length;
	
	needle_length = needle.length();

	while ((needlepos=haystack.find(needle,0)) != string::npos) 
	{
		haystack.replace(needlepos,needle_length,replacement);
	}

}


/* split a string by 'seperator' into a vector of string pointers */
void str_explode(vector<string> &vect_out, const string &seperator, const string &in_str)
{
	unsigned int i = 0, j = 0;
	unsigned int seperator_len = seperator.length();
	unsigned int str_len = in_str.length();

	while(i < str_len) 
	{ 
		j = in_str.find_first_of(seperator,i);

		if (j == string::npos && i < str_len)  j = str_len; 
			
		if (j == string::npos) break;

		vect_out.push_back(in_str.substr(i,j-i));
						
		i = j;
		
		i+=seperator_len;
	}
}


void str_implode(string &str_out, const string &seperator, vector<string> &str_vect)
{
	vector<string>::iterator i;

	vector<string>::iterator vect_begin = str_vect.begin();
	vector<string>::iterator vect_end = str_vect.end();
	
	for (i = vect_begin; i != vect_end; i++)
	{
		if (i != vect_begin) str_out.append(seperator);
		str_out.append(*i);
	}
}


void str_file_extract(string path_in, string &path_str,string &file_str,string &file_base,string &file_ext)
{
	vector<string> fileArray;
	
	str_explode(fileArray,PATH_SEP,path_in);
	
	file_str = fileArray.back();
	fileArray.pop_back();
	
	str_implode(path_str, PATH_SEP, fileArray);
	
	vector<string> fileNameArray;
	
	str_explode(fileNameArray,".",file_str);
	
	file_ext = fileNameArray.back();
	fileNameArray.pop_back();
	
	str_implode(file_base,".",fileNameArray);	
}
