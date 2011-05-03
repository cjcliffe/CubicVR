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

#include "CubicVR/GLShader.h"

GLShader GLShader::defaultShader;
GLhandleARB GLShader::activeProgram = NULL;
GLint GLShader::binding_inc = 0;

//int GLShader::last_active_lights[MAX_LIGHTS];
//int GLShader::active_lights[MAX_LIGHTS];

//int GLShader::last_active_light_types[MAX_LIGHTS];
//int GLShader::active_light_types[MAX_LIGHTS];

bool GLShader::defaultShaderLoaded = false;

std::map<unsigned long, std::map<unsigned long, std::vector<GLShader *>, ltulong>, ltulong> GLShader::lightShader;
std::map<unsigned long, std::string, ltulong> GLShader::light_frag_str;
std::map<unsigned long, std::string, ltulong> GLShader::light_frag_str2;
std::map<unsigned long, std::string, ltulong> GLShader::light_vert_str;
std::map<unsigned long, bool, ltulong> GLShader::light_multi;


void GLShader::setShaderAttrib(const char *varId, void *value)
{	
	if (shaderVals.find(varId) == shaderVals.end())
	{
		//		if (shaderVals[varId].binding != -1) use();
		shaderVals[varId].binding = glGetAttribLocation((unsigned long)program, varId);
		shaderVals[varId].valType = SHADER_VAL_ATTRIB;
	}
	
	if (shaderVals[varId].curVal != (unsigned long)value)
	{
		shaderVals[varId].lastVal = (unsigned int)shaderVals[varId].curVal;
		shaderVals[varId].curVal = (unsigned long)value;
	}	
}



void GLShader::setShaderValue(const char *varId, unsigned int value)
{
//	glUniform1iARB(glGetUniformLocationARB(program, varId), (GLuint)value);
//	if (!program) return;

	if (shaderVals.find(varId) == shaderVals.end())
	{
//		if (shaderVals[varId].binding != -1) use();
		shaderVals[varId].binding = glGetUniformLocationARB(program, varId);
		shaderVals[varId].valType = SHADER_VAL_INT;
	}
		
	if (shaderVals[varId].curVal != value)
	{
		shaderVals[varId].lastVal = shaderVals[varId].curVal;
		shaderVals[varId].curVal = value;
	}	
}


void GLShader::setShaderValue(const char *varId, XYZ &value)
{
//	if (!program) return;
	
	if (shaderVals.find(varId) == shaderVals.end())
	{
//		if (shaderVals[varId].binding != -1) use();
		shaderVals[varId].binding = glGetUniformLocationARB(program, varId);
		shaderVals[varId].valType = SHADER_VAL_XYZ;
	}
	
	if (shaderVals[varId].xyzVal != value)
	{
		shaderVals[varId].xyzLastVal = shaderVals[varId].xyzVal;
		shaderVals[varId].xyzVal = value;
	}	
}


void GLShader::setShaderValue(const char *varId, float value)
{
//	if (!program) return;

	if (shaderVals.find(varId) == shaderVals.end())
	{
//		if (shaderVals[varId].binding != -1) use();
		shaderVals[varId].binding = glGetUniformLocationARB(program, varId);
		shaderVals[varId].valType = SHADER_VAL_FLOAT;
	}
		
	if (shaderVals[varId].xyzVal.x != value)
	{
		shaderVals[varId].xyzLastVal.x = shaderVals[varId].xyzVal.x;
		shaderVals[varId].xyzVal.x = value;
	}	
}


void GLShader::setShaderValue(const char *varId, float *value)
{
//	if (!program) return;

	if (shaderVals.find(varId) == shaderVals.end())
	{
//		if (shaderVals[varId].binding != -1) use();
		shaderVals[varId].binding = glGetUniformLocationARB(program, varId);
		shaderVals[varId].valType = SHADER_VAL_4X4;
		if (shaderVals[varId].binding!=-1) shaderVals[varId].mat = NULL;
		shaderVals[varId].mat = new float[16];
	}

	
//	if (shaderVals[varId].xyzVal.x != value)
//	{
//		if (!shaderVals[varId].mat) shaderVals[varId].mat = new float[16];
		memcpy(shaderVals[varId].mat,value,sizeof(float)*16);
//	}	
	
//	glUniformMatrix4fvARB(shaderVals[varId].binding,1,false,value);		
}


void GLShader::clearShader()
{
	shaderVals.clear();
}

void GLShader::setupShader()
{	
	use();
	std::map<const char *, shaderValPair>::iterator shaderVals_i;
	
	for (shaderVals_i = shaderVals.begin(); shaderVals_i != shaderVals.end(); shaderVals_i++)
	{
		if ((*shaderVals_i).second.binding == -1) continue;
				
		switch ((*shaderVals_i).second.valType)
		{
			case SHADER_VAL_XYZ:
				if ((*shaderVals_i).second.xyzVal != (*shaderVals_i).second.xyzLastVal)
				{
					cubicvr_profiler.shaderval_count++;
					glUniform3f((*shaderVals_i).second.binding, (float)(*shaderVals_i).second.xyzVal.x,(float)(*shaderVals_i).second.xyzVal.y,(float)(*shaderVals_i).second.xyzVal.z);
					shaderVals[(*shaderVals_i).first].xyzLastVal = (*shaderVals_i).second.xyzVal;
				}
			break;
			case SHADER_VAL_INT:
				if ((*shaderVals_i).second.curVal != (*shaderVals_i).second.lastVal)
				{
					cubicvr_profiler.shaderval_count++;
					glUniform1i((*shaderVals_i).second.binding, (GLuint)(*shaderVals_i).second.curVal);
					shaderVals[(*shaderVals_i).first].lastVal = (*shaderVals_i).second.curVal;
				}
			break;
			case SHADER_VAL_FLOAT:
				if ((*shaderVals_i).second.xyzVal.x != (*shaderVals_i).second.xyzLastVal.x)
				{
					cubicvr_profiler.shaderval_count++;
					glUniform1f((*shaderVals_i).second.binding, (float)(*shaderVals_i).second.xyzVal.x);
					shaderVals[(*shaderVals_i).first].xyzLastVal.x = (*shaderVals_i).second.xyzVal.x;
				}
			break;
			case SHADER_VAL_4X4:
				cubicvr_profiler.shaderval_count++;
//				printf("set matrix %s\n",(*shaderVals_i).first.c_str());
				glUniformMatrix4fvARB(shaderVals[(*shaderVals_i).first].binding,1,false,(*shaderVals_i).second.mat);
//				shaderVals[(*shaderVals_i).first].xyzLastVal.x = (*shaderVals_i).second.xyzVal.x;
			break;
			case SHADER_VAL_ATTRIB:
				cubicvr_profiler.shaderval_count++;
				if ((*shaderVals_i).second.curVal != (*shaderVals_i).second.lastVal)
				{
					//glEnableVertexAttribArrayARB(shaderVals[(*shaderVals_i).first].binding);
//					extern void glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
//					GL_BYTE,
//					GL_UNSIGNED_BYTE,
//					GL_SHORT,
//					GL_UNSIGNED_SHORT,
//					GL_INT,
//					GL_UNSIGNED_INT,
//					GL_FLOAT, or
//					GL_DOUBLE
					glVertexAttribPointer(shaderVals[(*shaderVals_i).first].binding, 3, GL_FLOAT, 0, 0, 0);
					glEnableVertexAttribArray(shaderVals[(*shaderVals_i).first].binding);
				}
			break;
		}
	}
}


void GLShader::setupShader(GLShader &extShader)
{	
	std::map<const char *, shaderValPair, string_less>::iterator shaderVals_i;
	
	for (shaderVals_i = shaderVals.begin(); shaderVals_i != shaderVals.end(); shaderVals_i++)
	{
		
//		printf("svar: %s, %d",(*shaderVals_i).first.c_str(),(*shaderVals_i).second.curVal);
		switch((*shaderVals_i).second.valType)
		{
			case SHADER_VAL_XYZ:
				extShader.setShaderValue((*shaderVals_i).first,(*shaderVals_i).second.xyzVal);
			break;
			case SHADER_VAL_INT:
				extShader.setShaderValue((*shaderVals_i).first,(*shaderVals_i).second.curVal);
			break;
			case SHADER_VAL_FLOAT:
				extShader.setShaderValue((*shaderVals_i).first,(float)(*shaderVals_i).second.xyzVal.x);
			break;
			case SHADER_VAL_4X4:
				extShader.setShaderValue((*shaderVals_i).first,(*shaderVals_i).second.mat);
			break;
			case SHADER_VAL_ATTRIB:
				extShader.setShaderAttrib((*shaderVals_i).first,(void *)(*shaderVals_i).second.curVal);
			break;
				
		}
		
	}
	extShader.setupShader();
}


bool GLShader::loadDefaultShader(const char *vf, const char *ff, const char *ff2)
{
//	defaultShaderLoaded = defaultShader.load(vf,ff,ff2);
	
//	return defaultShaderLoaded;
	
//	LIGHT_NULL
	
	loadLightShader(LIGHT_NULL,vf,ff,ff2);
	
	defaultShaderLoaded = true;
	
	return true;
}


bool GLShader::loadShaderStrings(std::string &vs, std::string &fs, std::string &fs2, const char *vf, const char *ff, const char *ff2)
{	
	char tmp_str[1024];
		
	FILE *fp;
	fp = fopen(vf,"rt");
	
	while (!feof(fp))
	{
		if (fgets(tmp_str,1024,fp)) vs.append(tmp_str);		
	}

	fclose(fp);

	fp = fopen(ff,"rt");


	while (!feof(fp))
	{
		if (fgets(tmp_str,1024,fp)) fs.append(tmp_str);		
	}
	
	fclose(fp);

#ifndef _WIN32
	if (ff2 != NULL)
	{
		fp = fopen(ff2,"rt");

		while (!feof(fp))
		{
			if (fgets(tmp_str,1024,fp)) fs2.append(tmp_str);		
		}
		
		fclose(fp);
	}
#endif

	return true;
}

void GLShader::findLoops(std::string &shaderStr, const char *loopStart, const char *loopEnd, std::vector<unsigned int> &loopStarts, std::vector<unsigned int> &loopEnds)
{
	size_t pos = 0;
	int loopStartLen = strlen(loopStart);
	int loopEndLen = strlen(loopEnd);
	
	while ((pos = shaderStr.find(loopStart,pos)) != std::string::npos)
	{
		loopStarts.push_back(pos);
		pos += loopStartLen;
	}
	

	pos = 0;
	
	while ((pos = shaderStr.find(loopEnd,pos)) != std::string::npos)
	{
		loopEnds.push_back(pos);
		pos += loopEndLen;
	}
	
}


void GLShader::searchAndReplace(std::string &haystack, const char *needle, const char *replacement)
{
	size_t pos;
	
	while ((pos = haystack.find(needle)) != std::string::npos)
	{
		haystack.replace(pos,strlen(needle),replacement,strlen(replacement));
	}

}


bool GLShader::buildLightShaderVariant(int lightType, int variantFlag)
{
	const char *lightIds[MAX_LIGHTS] = {"0","1","2","3","4","5","6","7"};
	const char *lightIdToken = "%LIGHTID%";
	const char *lightLoopStartToken = "%LIGHTLOOP:START%";
	const char *lightLoopEndToken = "%LIGHTLOOP:END%";
	
	int lightLoopStartTokenLen = 17;
	int lightLoopEndTokenLen = 15;
	
	bool isMultiLight = light_multi[lightType];
		
	std::string lightFrag;
	std::string lightVert;
	
	lightFrag.append(light_frag_str[lightType]);
	lightVert.append(light_vert_str[lightType]);
	
	searchAndReplace(lightFrag,"%hasColorMap%",(variantFlag & SHADER_VARIANT_COLOR)?"1":"0"); 
	searchAndReplace(lightVert,"%hasColorMap%",(variantFlag & SHADER_VARIANT_COLOR)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasAlphaMap%",(variantFlag & SHADER_VARIANT_ALPHA)?"1":"0"); 
	searchAndReplace(lightVert,"%hasAlphaMap%",(variantFlag & SHADER_VARIANT_ALPHA)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasSpecMap%",(variantFlag & SHADER_VARIANT_SPECULAR)?"1":"0"); 
	searchAndReplace(lightVert,"%hasSpecMap%",(variantFlag & SHADER_VARIANT_SPECULAR)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasLumiMap%",(variantFlag & SHADER_VARIANT_LUMINOSITY)?"1":"0"); 
	searchAndReplace(lightVert,"%hasLumiMap%",(variantFlag & SHADER_VARIANT_LUMINOSITY)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasBumpMap%",(variantFlag & SHADER_VARIANT_BUMP)?"1":"0"); 
	searchAndReplace(lightVert,"%hasBumpMap%",(variantFlag & SHADER_VARIANT_BUMP)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap0%",(variantFlag & SHADER_VARIANT_SHADOW0)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap0%",(variantFlag & SHADER_VARIANT_SHADOW0)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap1%",(variantFlag & SHADER_VARIANT_SHADOW1)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap1%",(variantFlag & SHADER_VARIANT_SHADOW1)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap2%",(variantFlag & SHADER_VARIANT_SHADOW2)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap2%",(variantFlag & SHADER_VARIANT_SHADOW2)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap3%",(variantFlag & SHADER_VARIANT_SHADOW3)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap3%",(variantFlag & SHADER_VARIANT_SHADOW3)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap4%",(variantFlag & SHADER_VARIANT_SHADOW4)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap4%",(variantFlag & SHADER_VARIANT_SHADOW4)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap5%",(variantFlag & SHADER_VARIANT_SHADOW5)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap5%",(variantFlag & SHADER_VARIANT_SHADOW5)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap6%",(variantFlag & SHADER_VARIANT_SHADOW6)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap6%",(variantFlag & SHADER_VARIANT_SHADOW6)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasShadowMap7%",(variantFlag & SHADER_VARIANT_SHADOW7)?"1":"0"); 
	searchAndReplace(lightVert,"%hasShadowMap7%",(variantFlag & SHADER_VARIANT_SHADOW7)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasReflectionMap%",(variantFlag & SHADER_VARIANT_REFLECTION)?"1":"0"); 
	searchAndReplace(lightVert,"%hasReflectionMap%",(variantFlag & SHADER_VARIANT_REFLECTION)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasEnvSphereMap%",(variantFlag & SHADER_VARIANT_ENVSPHEREMAP)?"1":"0"); 
	searchAndReplace(lightVert,"%hasEnvSphereMap%",(variantFlag & SHADER_VARIANT_ENVSPHEREMAP)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasLightMap%",(variantFlag & SHADER_VARIANT_LIGHTMAP)?"1":"0"); 
	searchAndReplace(lightVert,"%hasLightMap%",(variantFlag & SHADER_VARIANT_LIGHTMAP)?"1":"0"); 
	
	searchAndReplace(lightFrag,"%hasParallaxMap%",(variantFlag & SHADER_VARIANT_PARALLAX)?"1":"0"); 
	searchAndReplace(lightVert,"%hasParallaxMap%",(variantFlag & SHADER_VARIANT_PARALLAX)?"1":"0"); 

	
	if (isMultiLight)
	{
		std::vector<unsigned int> loopStarts;
		std::vector<unsigned int> loopEnds;
				
		for (int i = 0; i < MAX_LIGHTS; i++)
		{			
			GLShader *tmpShader = new GLShader();

			std::string thisFrag;
			std::string thisVert;
			std::vector<std::string> lightVertLoops;
			std::vector<std::string> lightFragLoops;

			std::vector<std::string> lightVertLoopsAccum;
			std::vector<std::string> lightFragLoopsAccum;

			std::vector<unsigned int> startFragLoops;
			std::vector<unsigned int> endFragLoops;

			std::vector<unsigned int> startVertLoops;
			std::vector<unsigned int> endVertLoops;

//			printf("finding loops..");
			findLoops(lightVert,lightLoopStartToken,lightLoopEndToken,startVertLoops,endVertLoops);
			findLoops(lightFrag,lightLoopStartToken,lightLoopEndToken,startFragLoops,endFragLoops);
//			printf("done.\n");
			
			if (startVertLoops.size() != endVertLoops.size())
			{
				Logger::log(LOG_ERROR,"Error multilight type %d: Mismatched vertex shader light loop scopes\n",lightType);
				return false;
			}

			if (startFragLoops.size() != endFragLoops.size())
			{
				Logger::log(LOG_ERROR,"Error multilight type %d: Mismatched fragment shader light loop scopes\n",lightType);
				return false;
			}
			
			lightFragLoops.resize(startFragLoops.size());
			lightVertLoops.resize(startVertLoops.size());
			lightFragLoopsAccum.resize(startFragLoops.size());
			lightVertLoopsAccum.resize(startVertLoops.size());
			thisFrag.append(lightFrag);
			thisVert.append(lightVert);
			
			int numVertLoops = startVertLoops.size();
			int numFragLoops = startFragLoops.size();
			
			for (int j = 0; j < numVertLoops; j++)
			{
				lightVertLoops[j].append(lightVert.substr(startVertLoops[j], endVertLoops[j]-startVertLoops[j]+lightLoopEndTokenLen));

//				printf("lightVertLoops %d: %s\n\n",j,lightVertLoops[j].c_str());
				
				for (int k = 0; k <= i; k++)
				{
					std::string thisVertLoop;
					
					thisVertLoop.append(lightVert.substr(startVertLoops[j]+lightLoopStartTokenLen, endVertLoops[j]-startVertLoops[j]-lightLoopStartTokenLen));
					
					searchAndReplace(thisVertLoop,lightIdToken,lightIds[k]);

//					printf("thisVertLoop %d: %s\n\n",j,thisVertLoop.c_str());

					lightVertLoopsAccum[j].append(thisVertLoop);
				}
//				printf("lightVertLoopsAccum %d: %s\n\n",j,lightVertLoopsAccum[j].c_str());
			}
				
			
			for (int j = 0; j < numFragLoops; j++)
			{
				lightFragLoops[j].append(lightFrag.substr(startFragLoops[j], endFragLoops[j]-startFragLoops[j]+lightLoopEndTokenLen));

//				printf("lightFragLoops %d: %s\n\n",j,lightFragLoops[j].c_str());

				for (int k = 0; k <= i; k++)
				{
					std::string thisFragLoop;
					thisFragLoop.append(lightFrag.substr(startFragLoops[j]+lightLoopStartTokenLen, endFragLoops[j]-startFragLoops[j]-lightLoopStartTokenLen));
					searchAndReplace(thisFragLoop,lightIdToken,lightIds[k]);
					lightFragLoopsAccum[j].append(thisFragLoop);

//					printf("lightFragLoop %d: %s\n\n",j,thisFragLoop.c_str());
				}

//				printf("lightFragLoopsAccum %d: %s\n\n",j,lightFragLoopsAccum[j].c_str());

			}
			
			
			for (int j = 0; j < numFragLoops; j++)
			{
				searchAndReplace(thisFrag,lightFragLoops[j].c_str(),lightFragLoopsAccum[j].c_str());
			}
			
			for (int j = 0; j < numVertLoops; j++)
			{
				searchAndReplace(thisVert,lightVertLoops[j].c_str(),lightVertLoopsAccum[j].c_str());
			}
			
			//printf("building variant light shader %d, light #%d, variant flag %d\n\n",lightType,i,variantFlag);
			
			if (!tmpShader->setup(thisVert.c_str(),thisFrag.c_str(),NULL))
			{
				//printf("light shader variant build failure.\n\n");
				return false;
			}
			lightShader[lightType][variantFlag].push_back(tmpShader);
		}
	}
	else
	{
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			GLShader *tmpShader = new GLShader();
			
			std::string thisFrag;
			std::string thisVert;
			
			thisFrag.append(lightFrag);
			thisVert.append(lightVert);

			searchAndReplace(thisFrag,lightIdToken,lightIds[i]);
			searchAndReplace(thisVert,lightIdToken,lightIds[i]);

			
			//printf("building variant light shader %d, light #%d, variant flag %d\n\n",lightType,i,variantFlag);
			
			if (!tmpShader->setup(thisVert.c_str(),thisFrag.c_str(),NULL))
			{
				//printf("light shader variant build failure.\n\n");
				return false;
			}
			lightShader[lightType][variantFlag].push_back(tmpShader);
		}
	}
	
	return true;
}


std::vector<shaderQueueEntry> GLShader::shader_queue;
unsigned long GLShader::shaderQueuePtr = 0;

bool GLShader::isMultiLight(int lightType)
{
	if (light_multi.find(lightType) != light_multi.end())
	{
		return light_multi[lightType];
	}	
	else
	{
		return false;
	}
}

bool GLShader::queueLightShaderVariant(int lightType, int variantFlag)
{
	shaderQueueEntry tmp;
	
	tmp.lightType = lightType;
	tmp.shaderMask = variantFlag;
	
	shader_queue.push_back(tmp);
	
	return true;
}

bool GLShader::processShaderQueue()
{
	while (shaderQueuePtr < shader_queue.size())
	{
		if (!GLShader::activateLight(shader_queue[shaderQueuePtr].lightType,0,shader_queue[shaderQueuePtr].shaderMask,true))
		{
			GLShader::activateLight(shader_queue[shaderQueuePtr].lightType,0,shader_queue[shaderQueuePtr].shaderMask,true);
		}

		shaderQueuePtr++;
	} 
	
	return true;
}

bool GLShader::loadLightShader(int lightType, const char *vf, const char *ff, const char *ff2, bool multiLight)
{	
	std::string vs;
	std::string fs;
	std::string fs2;
	
	loadShaderStrings(vs,fs,fs2,vf,ff,ff2);
	
	light_frag_str[lightType] = fs;
	light_frag_str2[lightType] = fs2;
	light_vert_str[lightType] = vs;
	
	const char *lightLoopStartToken = "%LIGHTLOOP:START%";

	if (vs.find(lightLoopStartToken) != string::npos || fs.find(lightLoopStartToken) != string::npos)
	{
		multiLight = true;
	}
	
	light_multi[lightType] = multiLight;
	return true;
}


bool GLShader::activateLight(int lightType, int lightNum, unsigned long lightVariant, bool compile)
{
	if (lightShader[lightType][lightVariant].empty())
	{
		if (compile)
		{
			if (!buildLightShaderVariant(lightType,lightVariant))
			{
				//printf("error building light shader variant - light type: %d, variant: %d \n", lightType, lightVariant);
				return false;
			}
		}
		else
		{
			queueLightShaderVariant(lightType, lightVariant);
		}
		
		return false;
	}
		
	if (!lightShader[lightType][lightVariant][lightNum]) return false;
	
	defaultShader.setupShader(*lightShader[lightType][lightVariant][lightNum]);
	lightShader[lightType][lightVariant][lightNum]->use();
	
	return true;
}


//void GLShader::setLightShaderValue(int lightType, const char *varId, unsigned int value)
//{
//	if (lightShader[lightType].empty())
//	{
//		printf("no matching light shader for value - light type: %d \n");
//		return;
//	}
//	
//	for (int i = 0; i < MAX_LIGHTS; i++)
//	{
//		lightShader[lightType][i]->setShaderValue(varId, value);
//	}
//}


//void GLShader::setLightShaderValue(int lightType, const char *varId, XYZ &value)
//{
//	if (lightShader[lightType].empty())
//	{
//		printf("no matching light shader for value - light type: %d \n");
//		return;
//	}
//	
//	for (int i = 0; i < MAX_LIGHTS; i++)
//	{
//		lightShader[lightType][i]->setShaderValue(varId, value);
//	}
//}
//
//
//void GLShader::setLightShaderValue(int lightType, const char *varId, float *value)
//{
//	if (lightShader[lightType].empty())
//	{
//		printf("no matching light shader for value - light type: %d \n");
//		return;
//	}
//	
//	for (int i = 0; i < MAX_LIGHTS; i++)
//	{
//		lightShader[lightType][i]->setShaderValue(varId, value);
//	}
//}




GLShader::GLShader() : loaded(false),program(NULL)
{
}



GLboolean GLShader::HasGLSL(void)
{
#ifdef _WIN32
	return GL_TRUE;
#else
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	
	if (GL_FALSE == gluCheckExtension((const GLubyte*) "GL_ARB_shader_objects", extensions))
		return GL_FALSE;
	if (GL_FALSE == gluCheckExtension((const GLubyte*)"GL_ARB_vertex_shader", extensions))
		return GL_FALSE;
	if (GL_FALSE == gluCheckExtension((const GLubyte*)"GL_ARB_fragment_shader", extensions))
		return GL_FALSE;
	if (GL_FALSE == gluCheckExtension((const GLubyte*)"GL_ARB_shading_language_100", extensions))
		return GL_FALSE;
	
	return GL_TRUE;
#endif
}


GLenum GLShader::LinkProgram(GLhandleARB program)
{
	GLint	logLength;
	GLint linked;
	
	glLinkProgramARB(program);
	FailGLError("Failed glLinkProgramARB");
	glGetObjectParameterivARB(program,GL_OBJECT_LINK_STATUS_ARB,&linked);
	glGetObjectParameterivARB(program,GL_OBJECT_INFO_LOG_LENGTH_ARB,&logLength);
	if (logLength)
	{
		GLint	charsWritten;
		GLcharARB *log = (GLcharARB *)malloc(logLength+128);
		
		glGetInfoLogARB(program, logLength, &charsWritten, log);
		Logger::log(LOG_ERROR,"Link GetInfoLogARB:\n%s\n",log);
		free (log);
	}
	if (!linked) 
		FailWithMessage("shader did not link");
	return GL_NO_ERROR;
}

GLenum GLShader::CompileProgram(GLenum target, GLcharARB* sourcecode, GLhandleARB *shader)
{
	GLint	logLength;
	GLint	compiled;

	if (sourcecode != 0)
	{
		*shader = glCreateShaderObjectARB(target);
		FailGLError("Failed to create fragment shader");
		glShaderSourceARB(*shader,1,(const GLcharARB **)&sourcecode,0);
		FailGLError("Failed glShaderSourceARB");
		glCompileShaderARB(*shader);
		FailGLError("Failed glCompileShaderARB");

		glGetObjectParameterivARB(*shader,GL_OBJECT_COMPILE_STATUS_ARB,&compiled);
		glGetObjectParameterivARB(*shader,GL_OBJECT_INFO_LOG_LENGTH_ARB,&logLength);
		if (logLength)
		{
			GLcharARB *log = (GLcharARB *)malloc(logLength+128);
			glGetInfoLogARB(*shader, logLength, &logLength, log);
			//printf("Compile log: \n");
			free (log);
		}
		//if (!compiled)
		//	FailWithMessage("shader could not compile");
		
	}
	return GL_NO_ERROR;
}


void GLShader::printLog(GLuint obj)
{
	int infologLength = 0;
	int maxLength;
	
	if(glIsShader(obj))
		glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
	else
		glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
			
	char *infoLog;

	infoLog = new char[maxLength];
 
	if (glIsShader(obj))
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
 
//	if (infologLength > 0)
//		printf("%s\n",infoLog);
	
	delete infoLog;
}

bool GLShader::setup(const char *vp, const char *fp, const char *fp2)
{
	int err;

	if ( ! HasGLSL())
	{
		Logger::log (LOG_ERROR,"%s doesn't support GLSL\n", glGetString(GL_RENDERER));
		return false;
	}


//	printf("compiling vertex shader..\n");

	err = CompileProgram(GL_VERTEX_SHADER_ARB, (GLcharARB*) vp, &vertShader);
	if (0 != err)
	{
Logger::log (LOG_ERROR,"----Vertex Shader could not compile\n");
Logger::log(LOG_DEBUG," VERTEX:\n\n%s\n\n ",vp);
Logger::log(LOG_DEBUG,"FRAGMENT: \n\n%s\n\n",fp);
		return false;
	}

//	printf("compiling fragment shader..\n");

	err = CompileProgram(GL_FRAGMENT_SHADER_ARB, (GLcharARB*)fp, &fragShader);
	
	if (0 != err)
	{
Logger::log (LOG_ERROR,"----Fragment Shader could not compile\n");
Logger::log(LOG_DEBUG,"VERTEX: \n\n%s\n\n ",vp);
Logger::log(LOG_DEBUG,"FRAGMENT: \n\n%s\n\n",fp);
		return false;
	}

	if (fp2) if (strlen(fp2))
	{
//		printf("compiling fragment shader 2..\n");

		err = CompileProgram(GL_FRAGMENT_SHADER_ARB, (GLcharARB*)fp2, &fragShader2);
		
		if (0 != err)
		{
			Logger::log (LOG_ERROR,"Fragment Shader 2 could not compile\n");
			return false;
		}
	}
	
	
	program = glCreateProgramObjectARB();
	glAttachObjectARB(program,fragShader);
	glAttachObjectARB(program,vertShader);
	if (fp2) if (strlen(fp2)) glAttachObjectARB(program,fragShader2);


	//printf("Linking..\n");
	
	err = LinkProgram(program);
	if (GL_NO_ERROR != err)
	{
		return false;
	}

	//printf("ok\n\n");
	
	loaded = true;
	
	return true;
}


void GLShader::clear()
{
	glUseProgramObjectARB(0);
	activeProgram = NULL;
}


void GLShader::use()
{
	if (program != activeProgram) 
	{
		glUseProgramObjectARB(program);
		activeProgram = program;
	}
}

GLhandleARB GLShader::getProgramId()
{
	return program;
}

bool GLShader::isLoaded()
{
	return loaded;
}

bool GLShader::load(const char *vf, const char *ff, const char *ff2)
{
	std::string vs;
	std::string fs;
	std::string fs2;
	
	char tmp_str[1024];
		
	FILE *fp;
	fp = fopen(vf,"rt");
	
	while (!feof(fp))
	{
		if (fgets(tmp_str,1024,fp)) vs.append(tmp_str);		
	}

	fclose(fp);

	fp = fopen(ff,"rt");


	while (!feof(fp))
	{
		if (fgets(tmp_str,1024,fp)) fs.append(tmp_str);		
	}
	
	fclose(fp);

	if (ff2)
	{
		fp = fopen(ff2,"rt");

		while (!feof(fp))
		{
			if (fgets(tmp_str,1024,fp)) fs2.append(tmp_str);		
		}
		
		fclose(fp);
	}
	
	return setup(vs.c_str(),fs.c_str(),ff2?fs2.c_str():NULL);
}