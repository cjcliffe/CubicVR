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

#include <CubicVR/Light.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/math/Maths.h>

#ifdef ARCH_PSP
GLint Light::gl_lights[MAX_LIGHTS] = {GU_LIGHT0,GU_LIGHT1,GU_LIGHT2,GU_LIGHT3};
#else
//GLint Light::gl_lights[MAX_LIGHTS] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
GLint Light::active_lights[MAX_LIGHTS] = {-1,-1,-1,-1,-1,-1,-1,-1};
GLint Light::active_light_types[MAX_LIGHTS] = {-1,-1,-1,-1,-1,-1,-1,-1};
GLint Light::active_shadow = -1;
GLint Light::active_projector = -1;
#endif


Light::Light(bool dynamic_in) : shadowmapTex(0),  coneTex(0), glLightId(0), nearclip(1.0f), farclip(200.0f), has_target(false), has_shadow(false), has_projector(false), parent(NULL), sceneObjTarget(NULL)
{
	setType(LIGHT_POINT);
	setAmbient(RGB(0.0f,0.0f,0.0f));
	setDiffuse(RGB(1.0f,1.0f,1.0f));
	setSpecular(RGB(0.0f,0.0f,0.0f)); 
	
	setConstantAttenuation(0.4f);
	setLinearAttenuation(0.01f);
	setQuadraticAttenuation(0.001f);
	dynamic = dynamic_in;
}



Light::~Light()
{
	//	delete_texture_context();
	// !mac shutdown_pbuffer(&shadowmap_pbuf);
}

void Light::setClipping(float near_in, float far_in)
{
	nearclip = near_in;
	farclip = far_in;
}


RGB Light::global_ambient = RGB(0.1f,0.1f,0.1f);

void Light::setGlobalAmbient(const RGB &global_ambient_rgb)
{
	global_ambient = global_ambient_rgb;
#ifndef ARCH_DC
	float global_amb[] = { global_ambient_rgb.r, global_ambient_rgb.g, global_ambient_rgb.b, 1.0f };
#ifndef ARCH_PSP
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_amb);
#endif
#endif
}

void Light::setLightId(GLint glLightId_in)
{
	glLightId = glLightId_in;
}

void  Light::control(int controllerId, int motionId, float value)
{
	switch (controllerId)
	{
		case CONTROLLER_POSITION:
			switch (motionId)
			{
				case MOTION_X: position.x = value; break;
				case MOTION_Y: position.y = value; break;
				case MOTION_Z: position.z = value; break;
			}
			break;
			
		case CONTROLLER_ROTATION:
			switch (motionId)
			{
				case MOTION_X: rotation.x = value; break;
				case MOTION_Y: rotation.y = value; break;
				case MOTION_Z: rotation.z = value; break;
			}
			break;
	}
};

void Light::setup(GLint glLightId_in)
{	
	GLint lightId = (glLightId_in != -1)?glLightId_in:glLightId;
	
	glLightId = lightId;
	
	enable();
	glMatrixMode(GL_MODELVIEW);
	
	
	if (has_target && (type == LIGHT_SPOT || type == LIGHT_AREA))
	{
//		Vector direction;
		
		direction = position;
		direction -= target;
		//direction.makeUnit();
		
//#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
//		GLShader::defaultShader.setShaderValue("lightDir",direction);
//#endif
		
#ifdef ARCH_PSP
		ScePspFVector3 lightPosition = { position.x, position.y, position.z };
		ScePspFVector3 lightDirection = { direction.x, direction.y, direction.z };

	    sceGuLight( lightId, GU_SPOTLIGHT, GU_DIFFUSE_AND_SPECULAR, &lightPosition );
	    //sceGuLight( lightId, GU_SPOTLIGHT, GU_SPECULAR, &lightPosition );
		sceGuLightSpot(lightId, &lightDirection, exponent, cutoff);
#else		
#ifndef ARCH_DC
		GLfloat Direction[] = { direction.x, direction.y, direction.z, 1.0f };
		GLfloat Position[] = { position.x, position.y, position.z, 1.0f };
		glLightfv(lightId, GL_POSITION, Position);
		glLightfv(lightId, GL_SPOT_DIRECTION, Direction);
#endif
#endif
	}
	else
	{
		if (type==LIGHT_DIRECTIONAL)
		{
			direction = XYZ(1,0,0);
			
			if (rotation.x || rotation.y || rotation.z || rotation != rotation_last)
			{
				Vector direction_in;
				
				direction_in = XYZ(1,0,0);

//				float dirTransform[16];
//				glMatrixMode(GL_MODELVIEW);
//				glPushMatrix();
//					glLoadIdentity();
//					glRotatef(rotation.z,0,0,1);
//					glRotatef(rotation.y,0,1,0);
//					glRotatef(rotation.x,1,0,0);
//					glGetFloatv(GL_MODELVIEW_MATRIX,dirTransform);
//				glPopMatrix();
//				
//				matTransform(dirTransform,direction_in,direction);
//				direction.makeUnit();
				
				directionTransform.pushMatrix();
				if (rotation.z)	directionTransform.rotate(rotation.z,0,0,1);
				if (rotation.y)	directionTransform.rotate(rotation.y,0,1,0);
				if (rotation.x)	directionTransform.rotate(rotation.x,1,0,0);
				directionTransform.apply(direction_in, direction);
				directionTransform.popMatrix();
				

//				direction = XYZ(cos(DEGTORAD(rotation.x)),sin(DEGTORAD(rotation.y)),sin(DEGTORAD(rotation.z)));
				direction.makeUnit();
				
				rotation_last = rotation;
			}
			
#ifdef ARCH_PSP
			ScePspFVector3 lightDirection = { direction.x, direction.y, direction.z };
			sceGuLight( lightId, GU_DIRECTIONAL, GU_DIFFUSE_AND_SPECULAR, &lightDirection );
			//sceGuLight( lightId, GU_DIRECTIONAL, GU_SPECULAR, &lightDirection );
#else		
			GLfloat Position[] = { direction.x, direction.y, direction.z, 0.0f };
//			GLfloat Position[] = { 1, 0, 0, 0.0 };

#ifndef ARCH_DC
			glLightfv(lightId, GL_POSITION, Position);
#endif
#endif			
		}
#ifndef ARCH_PSP
#ifndef ARCH_DC
		else
		{
			if (parent)
			{
				glPushMatrix();
				parent->transformBegin();
			}			

			glPushMatrix();

			ObjectController lCtl;
			
			lCtl.setRotation(rotation);
			lCtl.setPosition(position);
			
			lCtl.transformBegin();
			
//
//			if (position.x || position.y || position.z)
//			{
//				glTranslatef(position.x,position.y,position.z);
//			}
//			
//			if (rotation.x || rotation.y || rotation.z)
//			{
//				if (rotation.z)
//				{
//					glRotatef(rotation.z,0,0,1);
//				}
//				if (rotation.y)
//				{
//					glRotatef(rotation.y,0,1,0);
//				}
//				if (rotation.x)
//				{
//					glRotatef(rotation.x,1,0,0);
//				}
//			}			

			GLfloat Direction[] = { 0, 0, -1, 1 };
			glLightfv(lightId, GL_SPOT_DIRECTION, Direction);
			GLfloat Position[] = { 0, 0, 0, 1 };
			glLightfv(lightId, GL_POSITION, Position);
			
			glGetFloatv(GL_MODELVIEW_MATRIX,transMatrix);
			
			lCtl.transformEnd();
			glPopMatrix();
			
			
//			glPushMatrix();
//			#ifdef ARCH_PSP
//					ScePspFVector3 lightPosition = { position.x, position.y, position.z };
//					//sceGuLight( lightId, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &lightPosition );
//					sceGuLight( lightId, GU_POINTLIGHT, GU_SPECULAR, &lightPosition );
//			#else		
//					GLfloat Position[] = { position.x, position.y, position.z, 1.0 };
//					glLightfv(lightId, GL_POSITION, Position);
//			#endif
//			glPopMatrix();
			
			if (parent)
			{
				parent->transformEnd();
				glPopMatrix();
			}

		}
#endif
#endif
		
//		if (type == LIGHT_DIRECTIONAL)
//	{
//			GLfloat Position[] = { direction.x, direction.y, direction.z, 0 };
//			glLightfv(lightId, GL_POSITION, Position);
//	}
	}


	
#ifndef ARCH_DC
	if (type == LIGHT_POINT)
	{
#ifdef ARCH_PSP
		ScePspFVector3 lightPosition = { position.x, position.y, position.z };
	    sceGuLight( lightId, GU_POINTLIGHT, GU_DIFFUSE_AND_SPECULAR, &lightPosition );
//	    sceGuLight( lightId, GU_POINTLIGHT, GU_SPECULAR, &lightPosition );
#else		
		GLfloat Position[] = { position.x, position.y, position.z, 1 };
		glLightfv(lightId, GL_POSITION, Position);
//		printf("pos: %d, %f, %f, %f\n",lightId,Position[0],Position[1],Position[2]);
#endif

#ifndef ARCH_PSP
		if (cutoff)
		{
			glLightf(lightId, GL_SPOT_EXPONENT, 1.0f/cutoff); 
		}
		else
		{
			glLightf(lightId, GL_SPOT_EXPONENT, 0); 	
		}
#endif
	}

	
#ifndef ARCH_PSP
	if ((type == LIGHT_SPOT)) //  || type == LIGHT_PROJECTOR
	{
		glLightf(lightId, GL_SPOT_CUTOFF, cutoff/2.0f);   
		glLightf(lightId, GL_SPOT_EXPONENT, 1.0);
	}
#endif
	

#ifdef ARCH_PSP
		sceGuLightMode(GU_SEPARATE_SPECULAR_COLOR);

		sceGuLightColor( lightId, GU_AMBIENT, GU_COLOR( ambient.r, ambient.g, ambient.b, 1.0f ));
		sceGuLightColor( lightId, GU_DIFFUSE, GU_COLOR( diffuse.r, diffuse.g, diffuse.b, 1.0f ));
		sceGuLightColor( lightId, GU_SPECULAR, GU_COLOR( specular.r, specular.g, specular.b, 1.0f ));
#else
	
//#ifndef OPENGL_ES
	//		glEnable(GL_SEPARATE_SPECULAR_COLOR);
	//		glLightModeli (GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
	//		glEnable(GL_COLOR_SUM);
//#endif
		GLfloat Ambient[] = { ambient.r, ambient.g, ambient.b, 1 };
		GLfloat Diffuse[] = { diffuse.r, diffuse.g, diffuse.b, 1 };
		GLfloat Specular[] = { specular.r, specular.g, specular.b, 1 };

		glLightfv(lightId, GL_AMBIENT, Ambient);
		glLightfv(lightId, GL_DIFFUSE, Diffuse);
		glLightfv(lightId, GL_SPECULAR, Specular);
#endif	
	
#ifdef ARCH_PSP
	sceGuEnable(gl_lights[lightId]);
	sceGuLightAtt(lightId, constant_attenuation, linear_attenuation, quadratic_attenuation);
#else
	glLightf(lightId, GL_CONSTANT_ATTENUATION, constant_attenuation);
	glLightf(lightId, GL_LINEAR_ATTENUATION, linear_attenuation);
	glLightf(lightId, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);
	
	glEnable(lightId);
#endif
#endif
	
};


void Light::enable()
{
#ifdef ARCH_PSP
	sceGuEnable( GU_LIGHTING );		
	sceGuEnable( gl_lights[glLightId] );		
#else
#ifndef ARCH_DC
	glEnable(GL_LIGHTING);
	glEnable(glLightId);
#endif
#endif
};


void Light::disable(GLint glLightId_in)
{
	return;
	GLint lightId = (glLightId_in != -1)?glLightId_in:glLightId;

#ifdef ARCH_PSP
	sceGuDisable( lightId );	
#else
	glDisable(lightId);
	active_lights[lightId] = -1;
//	GLShader::setLightActivation(glLightId,0);
#endif
};


void Light::setTargeted(bool has_target_in) 
{ 
	has_target = has_target_in; 
};


void Light::bindTarget(SceneObject *sceneObj)
{
	has_target = sceneObj?true:false;

	sceneObjTarget = sceneObj;
};



void Light::bindParent(SceneObject &parent_in)
{
	parent = &parent_in;
};


void Light::setType(unsigned short type_in)
{
	type = type_in;
};


void Light::setAmbient(const RGB &rgb_in)
{
	ambient = rgb_in;
};


void Light::setDiffuse(const RGB &rgb_in)
{
	diffuse = rgb_in;
};


void Light::setSpecular(const RGB &rgb_in)
{
	specular = rgb_in;
};


void Light::setPosition(const XYZ &xyz_in)
{
	position = xyz_in;
};


void Light::setRotation(const XYZ &xyz_in)
{
	rotation = xyz_in;
};


void Light::setTarget(const XYZ &xyz_in)
{
	target = xyz_in;
};


void Light::setCutoff(float cutoff_in)
{
	cutoff = cutoff_in;
}


void Light::setExponent(float exponent_in)
{
	exponent = exponent_in;
};


/*
 total_light = color_intensity / (
								  constant_attenuation +
								  distance * linear_attenuation +
								  distance * distance * quadratic_attenuation )
 */
void Light::setConstantAttenuation(float constant_attenuation_in)
{
	constant_attenuation = constant_attenuation_in;
}


void Light::setLinearAttenuation(float linear_attenuation_in)
{
	linear_attenuation = linear_attenuation_in;
}


void Light::setQuadraticAttenuation(float quadratic_attenuation_in)
{
	quadratic_attenuation = quadratic_attenuation_in;
}


void Light::setShadow(unsigned int map_res_in, const std::string cone_tex)
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	map_res = map_res_in;
	static const GLfloat borderColor[4] = {1, 1, 1, 0}; 

	
	if(!(strncmp(cone_tex.c_str(),"null",4) == 0 || strncmp(cone_tex.c_str(),"Null",4) == 0 || strncmp(cone_tex.c_str(),"NULL",4) == 0))
	{
		coneTex = Texture::create(cone_tex);
		Texture::use(coneTex);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		has_projector = true;
	}
	
	
//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD_SIGNED);
	
	
	/* shadow map fbo setup */
	glGenTextures (1, (GLuint *)&shadowmapTex);
	glBindTexture (GL_TEXTURE_2D, shadowmapTex);

	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, map_res, map_res, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	
	// create fbo and attach texture to ti
	glGenFramebuffersEXT (1, (GLuint *)&shadowBuffer);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, shadowBuffer);
	
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadowmapTex, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, shadowBuffer);

	 // verify all is well and restore state
	GLenum status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		cerr << "FBO configuration unsupported" << endl;
	default:
		cerr << "FBO programmer error" << endl;
	}
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);



	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE_ARB);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LESS);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 0.6f);
	
//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

//			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 0, 0, map_res, map_res, 0);

		has_shadow = true;
#endif
#endif
#endif
};


bool Light::hasShadow()
{
	return has_shadow;
};


void Light::shadowInit()
{
#ifndef ARCH_PSP
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
#endif
};


void Light::shadowBegin()
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	GLShader::clear();
	glPushAttrib (GL_DEPTH_BUFFER_BIT|GL_VIEWPORT_BIT|GL_ENABLE_BIT);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, shadowBuffer);

	glViewport(0, 0, map_res, map_res);
	viewport[0]=0;
    viewport[1]=0;
    viewport[2]=map_res;
    viewport[3]=map_res;
		
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type==LIGHT_AREA)
	{
//		gluPerspective(cutoff, 1.0, nearclip, farclip);
		glOrtho(-ortho_size/2.0, ortho_size/2.0, -ortho_size/2.0, ortho_size/2.0, nearclip, farclip);	
	}
	else if (type==LIGHT_SPOT)
	{
		gluPerspective(cutoff, 1.0, nearclip, farclip);
	}	
	
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	


	Vector upVector(0,1,0);

	if (has_target)
	{
		if (sceneObjTarget)
		{
			target = sceneObjTarget->getPosition();
		}

		if (rotation.z && has_target)
		{
			glRotatef(-rotation.z,0,0,1);
			glPushMatrix();
		}		
		
		
		gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, upVector.x, upVector.y, upVector.z);
		
	}
	else
	{
		Vector i(0,0,1),f;
		
		matTransform(transMatrix,i,f);
				
		f.makeUnit();
		f*=-farclip;
		
		gluLookAt(position.x, position.y, position.z, position.x+f.x, position.y+f.y, position.z+f.z, upVector.x, upVector.y, upVector.z);
		
		if (parent)
		{
			parent->transformReverseBegin();
			glPushMatrix();
		}			
	}

	glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
	

#endif
#endif
#endif
};


void Light::shadowEnd()
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	if (parent)
	{
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		parent->transformEnd();
	}			
	
	if (rotation.z && has_target)
	{
		glPopMatrix();
	}		
	
//	glDepthRange (0.0, 1.0);


//	glMatrixMode(GL_MODELVIEW);
//
//	controller->transformEnd();
	
	//if (!GLExt::have_depth_texture) return;
	
	//glPopMatrix();
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib (); // restore the viewport 
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	
	
//	glPolygonOffset (0,0);
//	glDisable(GL_POLYGON_OFFSET_FILL);


#endif
#endif
#endif
};


void Light::setupTexgen(double angle, int unit)
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	glActiveTexture(GL_TEXTURE0+unit);			// todo: dynamic?
	glClientActiveTexture(GL_TEXTURE0+unit);
	
	glMatrixMode(GL_TEXTURE);
//	glLoadIdentity();
		
//	static MATRIX4X4 biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
//								0.0f, 0.5f, 0.0f, 0.0f,
//								0.0f, 0.0f, 0.5f, 0.0f,
//								0.5f, 0.5f, 0.5f, 1.0f); //bias from [-1, 1] to [0, 1]
//
//    
//	MATRIX4X4 textureMatrix= biasMatrix * MATRIX4X4(projectionMatrix) * MATRIX4X4(viewMatrix);
//	
//    //Set up texture coordinate generation.
//    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//    glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(0));
//    glEnable(GL_TEXTURE_GEN_S);
//	
//    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//    glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrix.GetRow(1));
//    glEnable(GL_TEXTURE_GEN_T);
//	
//    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//    glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrix.GetRow(2));
//    glEnable(GL_TEXTURE_GEN_R);
//	
//    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//    glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrix.GetRow(3));
//    glEnable(GL_TEXTURE_GEN_Q);
//	
//	
//	glEnable(GL_TEXTURE_GEN_S);
//	glEnable(GL_TEXTURE_GEN_T);
//	glEnable(GL_TEXTURE_GEN_R);
//	glEnable(GL_TEXTURE_GEN_Q);
	
	
	float biasMatrix[16] = {0.5f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.5f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.5f, 0.0f,
							0.5f, 0.5f, 0.5f, 1.0f};
	
//	float PS[] = {1,0,0,0};
//	float PT[] = {0,1,0,0};
//	float PR[] = {0,0,1,0};
//	float PQ[] = {0,0,0,1};
	
//	glTexGenfv(GL_S, GL_EYE_PLANE, PS);
//	glTexGenfv(GL_T, GL_EYE_PLANE, PT);
//	glTexGenfv(GL_R, GL_EYE_PLANE, PR);
//	glTexGenfv(GL_Q, GL_EYE_PLANE, PQ);
//	
//	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	
	glLoadIdentity();

	glLoadMatrixf(biasMatrix);
	glMultMatrixf(projectionMatrix);
	glMultMatrixf(viewMatrix);
	
	glMatrixMode(GL_MODELVIEW);
	
//	glTranslatef(0.5, 0.5, 0.5);
//	glScalef(0.5, 0.5, 0.5);
//	gluPerspective(angle, 1, 1, 100);
	
//	glPushMatrix();
	
	//
//	if (has_target)
//	{
//		gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, 0, 1, 0);
//	}
//	else
//	{
//		if (rotation.x || rotation.y || rotation.z)
//		{
//			if (rotation.z)
//			{
//				glRotatef(-rotation.z,0,0,1);
//			}
//			if (rotation.y)
//			{
//				glRotatef(-rotation.y,0,1,0);
//			}
//			if (rotation.x)
//			{
//				glRotatef(-rotation.x,1,0,0);
//			}
//		}
//		
//		if (position.x || position.y || position.z)
//		{
//			glTranslatef(-position.x,-position.y,-position.z);
//		}
//		
//		if (parent)
//		{
//			glPushMatrix();
//			parent->transformReverseBegin();
//		}
//	}
//	
	

//	glGetFloatv(GL_TEXTURE_MATRIX, textureMat);


#endif
#endif
#endif 	
	
	
};



void Light::shadowRenderStart(int stage, int unit)
{
#ifndef ARCH_PSP
#ifndef ARCH_DC
#ifndef OPENGL_ES
	if(stage == 0)	
	{
		if ((type == LIGHT_SPOT || type == LIGHT_AREA))
		{  			
//			if (GLExt::tex_usage_max == -1)
//			{
//				GLExt::setTexture((GLExt::tex_usage_max+1));
//				glBindTexture(GL_TEXTURE_2D, Texture::getTextureId("whitepixel"));
//
//				GLShader::defaultShader.setShaderValue("colorMap", GLExt::tex_usage_max); 
//				GLShader::defaultShader.setShaderValue("hasColorMap", 1);
//			}
			
			
//			glMatrixMode(GL_MODELVIEW);
//			glPushMatrix();
//			glLoadMatrixf(modelViewMat);
	
//			Texture::clearLocks();
//			Texture::clearAllTextures();
//			
//			shadow_mtex = Texture::nextTexture();
//			active_shadow = shadow_mtex;
//			Texture::setTexture(shadow_mtex);
			
			shadow_mtex = 15-unit;
			active_shadow = shadow_mtex;
			
//			Texture::lock();
//			GLShader::defaultShader.setShaderValue("shadowMap", shadow_mtex); 
			
			//Texture::use(shadowmapTex);
//			glActiveTexture(GL_TEXTURE0+shadow_mtex);			// todo: dynamic?
//			glClientActiveTexture(GL_TEXTURE0+shadow_mtex);
			
//			glEnable(GL_TEXTURE_2D);
//			glBindTexture(GL_TEXTURE_2D, shadowmapTex);
							
			setupTexgen(cutoff, unit);  	

			glMatrixMode(GL_MODELVIEW);
//			glPopMatrix();
		}	
	}
	else
	{
		//if (has_projector)
//		{
//			glMatrixMode(GL_MODELVIEW);
//			glPushMatrix();
//			glLoadMatrixf(modelViewMat);
//
//			cone_mtex = Texture::nextTexture();
//
//			active_projector = cone_mtex;
//
//			Texture::use(coneTex);
//			setupTexgen(cutoff);
//
//			glMatrixMode(GL_MODELVIEW);
//			glPopMatrix();
//		}

	}
	
#endif
#endif
#endif
};


void Light::shadowRenderFinish(int stage, int unit)
{
#ifndef ARCH_DC
#ifndef ARCH_PSP
#ifndef OPENGL_ES
	
	if(stage == 0)	
	{
		if ((type == LIGHT_SPOT || type == LIGHT_AREA))
		{
//			Texture::setTexture(shadow_mtex);
//			Texture::use(shadowmapTex);
//			glBindTexture(GL_TEXTURE_2D, shadowmapTex);

			active_shadow = -1;
			
		/*	if (GLExt::have_tex_render)
			{	
				//wglReleaseTexImageARB(shadowmap_pbuf.hpbuffer, WGL_DEPTH_COMPONENT_NV);
			}
		*/	
			
			glActiveTexture(GL_TEXTURE0+unit);			// todo: dynamic?
			glClientActiveTexture(GL_TEXTURE0+unit);

			glMatrixMode(GL_TEXTURE);

			
			if (parent)
			{
				glPopMatrix();
				parent->transformEnd();
			}
			
//			glPopMatrix();
			glLoadIdentity();

			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_Q);
			glDisable(GL_TEXTURE_2D);
		}
	}
	else
	{
//		if (has_projector)
//		{
//			Texture::setTexture(cone_mtex);
//			Texture::use(coneTex);
//			active_projector = -1;
//			
//			glMatrixMode(GL_TEXTURE);
//			glPopMatrix();
//			if (parent)
//			{
//				glPopMatrix();
//				parent->transformEnd();
//			}
//			glLoadIdentity();
//			
//#ifndef OPENGL_ES
//			glDisable(GL_TEXTURE_GEN_S);
//			glDisable(GL_TEXTURE_GEN_T);
//			glDisable(GL_TEXTURE_GEN_R);
//			glDisable(GL_TEXTURE_GEN_Q);
//			glDisable(GL_TEXTURE_2D);
//#endif
//		}
	}
	glMatrixMode(GL_MODELVIEW);
#endif
#endif
#endif
};
