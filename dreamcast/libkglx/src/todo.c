#include<kos.h>
#include<stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "glinternal.h"
#include "glmacros.h"
#include "assert.h"

#ifdef TODO_USE_ASSERT
#define ASMSG(MSG) assert_msg(0, (MSG) )
#else
#ifdef TODO_WARN
#define ASMSG(MSG) puts( "KGLX Warning:" MSG )
#else
#define ASMSG(MSG)
#endif
#endif

void glPolygonStipple( const GLubyte *mask){
	ASMSG("glPolygonStipple not implemented");
}

void glPolygonOffset( GLfloat factor, GLfloat units ){
	ASMSG("glPolygonOffset not implemented");
}

void glAlphaFunc( GLenum func, GLclampf ref ){
	ASMSG("glAlphaFunc not implemented");
}

void glDrawBuffer( GLenum mode ){
	ASMSG("glDrawBuffer not implemented");
}

// glGet working in progress:
static void glGet_internal(GLenum pname, void *params) {
	assert(params!=NULL);
	switch(pname)
	{
		case GL_TEXTURE_BINDING_1D:
		case GL_TEXTURE_BINDING_2D:
				*((pvr_poly_cxt_t **)params) = gl_cur_texture;
				break;
		case GL_MAX_TEXTURE_SIZE:
				*((unsigned *)params) = 1024;
				break;
		case GL_PROJECTION_MATRIX:
				{
					int i;
					double *dst=(double *)params;
					float *src=(float *)&gl_trans_mats[gl_matrix_mode];
					for(i=0;i<16;i++)
						*dst++=*src++;
				}
				break;
		default:
			ASMSG("glGet not implemented");
	}
}

void glGetIntegerv( GLenum pname, GLint *params ){
	glGet_internal(pname,params);
}

void glGetBooleanv( GLenum pname, GLboolean *params ){
	glGet_internal(pname,params);
}

void glGetDoublev( GLenum pname, GLdouble *params ){
	glGet_internal(pname,params);
}

void glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height ){
	ASMSG("glCopyTexSubImage2D not implemented");
}

void glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width ){
	glCopyTexSubImage2D(GL_TEXTURE_2D,level,xoffset,1,x,y,width,1);
}

void glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border ){
	ASMSG("glCopyTexImage2D not implemented");
}

void glCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border ){
	glCopyTexImage2D(GL_TEXTURE_2D,level,internalformat,x,y,width,1,border);
}

void glClearStencil( GLint s ){
	ASMSG("glClearStencil not implemented");
}

void glStencilOp( GLenum fail, GLenum zfail, GLenum zpass ){
	ASMSG("glStencilOp not implemented");
}

void glStencilMask( GLuint mask ){
	ASMSG("glStencilMask not implemented");
}

void glStencilFunc( GLenum func, GLint ref, GLuint mask ){
	ASMSG("glStencilFunc not implemented");
}

void glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ){
	ASMSG("glReadPixels not implemented");
}

void glLineWidth( GLfloat width ){
	ASMSG("glLineWidth not implemented");
}

void glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ){
	ASMSG("glColorMask not implemented");
}

void glLightfv( GLenum light, GLenum pname, const GLfloat *params ){
	ASMSG("glLightfv not implemented");
}

void glLightiv( GLenum light, GLenum pname, const GLint *params ){
	ASMSG("glLightiv not implemented");
}

GLint gluBuild2DMipmaps( GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data )
{
	ASMSG("gluBuild2DMipmaps not implemented");
	glTexImage2D(target, 0, internalFormat,width,height,0,format,type,data);
	return GL_TRUE;
}

void glopPolygonMode (GLParam * p)
{
	ASMSG("glPolygonMode not implemented");

}
