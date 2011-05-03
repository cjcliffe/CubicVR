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

/*
 *  ResourcePool.cpp
 *
 *	based on DIRTREE.C  V 1.0 by Glen E. Gardner, Jr.
 */


#include <CubicVR/ResourcePool.h>
#include <CubicVR/Logger.h>
#ifdef ARCH_PSP
#include <sys/unistd.h>
#endif


ResourcePool::ResourcePool()
{
	strMap = new MapStringPool(*this);
};

ResourcePool::~ResourcePool()
{
	delete strMap;
}


const char *ResourcePool::findByFilename(const char *fn)
{
	tmp_str.clear();
	
	if (fileMap.find(fn) != fileMap.end())
	{
		tmp_str = fileMap[fn][0];
	}
	
	return tmp_str.c_str();
}


void ResourcePool::findByFilename(const char *fn,std::vector<std::string> &fileList)
{
	if (fileMap.find(fn) != fileMap.end())
	{
		vector<string>::iterator file_i;
		
		for (file_i = fileMap[fn].begin(); file_i != fileMap[fn].end(); file_i++)
		{
			fileList.push_back(*file_i);
		}
	}
}

void ResourcePool::findByExtension(const char *ext,std::vector<std::string> &fileList)
{
	if (extMap.find(ext) != extMap.end())
	{
		vector<string>::iterator file_i;
		
		for (file_i = extMap[ext].begin(); file_i != extMap[ext].end(); file_i++)
		{
			fileList.push_back(*file_i);
		}
	}
}


int ResourcePool::getSize(const char *dirname)
{/* finds and returns the size of the directory entry (in bytes) */
	
	DIR *dir;
	struct dirent *ent;
	int size=0;
	
	
	if ((dir = opendir(dirname)) == NULL)
	{
		perror("Unable to open directory");
		return(0);
	}
	while ((ent = readdir(dir)) != NULL);
#ifdef ARCH_PSP
	closedir(dir);
	dir=opendir(dirname);
#else
	rewinddir(dir);
#endif
	while ((ent = readdir(dir)) != NULL)size=size+sizeof(ent->d_name)+1;
	
	if (closedir(dir) != 0)
		perror("Unable to close directory");
	return(size);
}


int ResourcePool::scanDir(const char *dirname,char *entries)
{/* scan the directory and store the entries in a buffer */
	DIR *dir;
	struct dirent *ent;
	int count=1;
	char name[RESOURCEPOOL_MAX_PATH];
	
	if ((dir = opendir(dirname)) == NULL)
	{
		perror("Unable to open directory");
		return(0);
	}
	
	while ((ent = readdir(dir)) != NULL)count++;
	
#ifdef ARCH_PSP
	closedir(dir);
	dir=opendir(dirname);
#else
	rewinddir(dir);
#endif
	
	while ((ent = readdir(dir)) != NULL)
	{
		strcpy(name,ent->d_name);
		sprintf(entries,"%s",name);
		entries=entries+strlen(name)+1;
		count++;
	}
	
	if (closedir(dir) != 0)
		perror("Unable to close directory");
	
	return(count);
}

void ResourcePool::refresh()
{
	fileMap.clear();
	extMap.clear();
	
	set<string> pathCopy;
	set<string>::iterator pathList_i;

	for (pathList_i = pathList.begin(); pathList_i != pathList.end(); pathList_i++)
	{
		pathCopy.insert(*pathList_i);
	}
		
	for (pathList_i = pathCopy.begin(); pathList_i != pathCopy.end(); pathList_i++)
	{
		addPath((*pathList_i).c_str(),recList[*pathList_i]);
	}
}

void ResourcePool::clearPaths()
{
	pathList.clear();
	recList.clear();
	
	refresh();
}


void ResourcePool::addPath(const char *dirname, bool recursive)
{/* scan the buffer and recursively enter any directories found */
	pathList.insert(dirname);
	recList[dirname] = recursive;
	
	char initialpath[RESOURCEPOOL_MAX_PATH];
	
	getcwd(initialpath,RESOURCEPOOL_MAX_PATH);
	
	if (chdir(dirname) != 0)
	{
		Logger::log(LOG_ERROR,"Resource Pool: Unable to locate path: %s\n",dirname);
		return;
	}
	
	dirname = ".";
	
	int size;
	char *entries;
	char name[RESOURCEPOOL_MAX_PATH];
	char Path[RESOURCEPOOL_MAX_PATH];
	char *path;
	char oldpath[RESOURCEPOOL_MAX_PATH];
	char old[RESOURCEPOOL_MAX_PATH];
	
	FILE *filein;
	path=Path;
	size=getSize(dirname);
	entries=(char *)malloc(size);
	scanDir(dirname,entries);
	getcwd(Path,RESOURCEPOOL_MAX_PATH);
	strcpy(old,Path);
	
	while(scanString(name,entries)!=EOF)
	{
		
		/* store the path for use later */
		getcwd(Path,RESOURCEPOOL_MAX_PATH);
		
		if(name[0] != '.')
		{
			
			/* this is where the valid path variable can be found */
			//				if(!strcmp(Path,old))printf("PATH: %s\n",Path);
			strcpy(oldpath,Path);
			
			/* add the next entry to the path for testing */
			strcat(Path,RESOURCEPOOL_PATH_CHAR);
			strcat(Path,name);
			
			/* see if the entry is a file */
			/* here is where the valid filename associated with the path can be found */
			if(chdir(Path)==0)
			{	
				/* if the entry is a valid directory, go there */
				getcwd(path,RESOURCEPOOL_MAX_PATH);
				
				if (recursive)
				{
					/* start the recursive traversal */
					addPath(".",recursive);
				}
				
				/* restore the path */
				strcpy(Path,oldpath);
				chdir(Path);
				getcwd(path,RESOURCEPOOL_MAX_PATH);
			}
			else if((filein=fopen(Path,"r"))!=0)
			{
				//printf("PATH: %s, ",Path);
				//printf("NAME: %s\n",name);
				
				string nameStr = string(name);
				
				fclose(filein);
				
				fileMap[name].push_back(Path);
				
				string::size_type idx = nameStr.find_last_of ( '.' );

				if ( idx == string::npos )
				{
//					printf("No file extension\n");
				}
				else
				{
					string extStr = StringToLower(nameStr.substr(idx+1));
					
					extMap[extStr].push_back(Path);
//					printf("The file extension is %s \n",nameStr.substr(idx+1).c_str());
				}
			}
			
			/* set the sentinel variable for state changes */
			strcpy(old,Path);
			strcpy(Path,oldpath);
			chdir(Path);
			getcwd(path,RESOURCEPOOL_MAX_PATH);
		}
		
		entries=entries+strlen(name)+1;
	}
	
	chdir(initialpath);
	
	return;
}


std::string ResourcePool::StringToLower(std::string str)
{
	const int length = str.length();
	for(int i=0; i!=length; ++i)
	{
		str[i] = std::tolower(str[i]);
	}
	return str;
}


int ResourcePool::scanString(char *string,char *buffer)
{
	int i;
	int size;
	char *temp;
	
	size=0;
	temp=buffer;
	while(*buffer!=0)
	{
		size++;
		buffer++;
	}
	
	buffer=temp;
	
	for(i=0;i<=size;i++)
	{
		*string=*buffer;
		buffer++;
		string++;
	}
	*string=*buffer;
	buffer=buffer-2;
	
	if(*buffer==0)return(EOF);
	return(size);
}


MapString &ResourcePool::getMap() 
{ 
	return *strMap; 
};

