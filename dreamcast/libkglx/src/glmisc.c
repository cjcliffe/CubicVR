/* KallistiGL for KOS 1.1.9

   glmisc.c
   (c)2001 Dan Potter
*/

#include <GL/gl.h>
#include <GL/glu.h>
#include "glinternal.h"
#include "glmacros.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// CVSID("$Id: glmisc.c,v 1.17 2002/07/08 05:28:28 axlen Exp $");

GLboolean gl_depth_test=GL_FALSE;
GLboolean gl_depth_mask=GL_TRUE;
GLboolean gl_depth_mask_def=PVR_DEPTHWRITE_ENABLE;
GLenum gl_depth_func=GL_LESS;
GLenum gl_depth_func_def=PVR_DEPTHCMP_GEQUAL;
GLint gl_blend_src_def=PVR_BLEND_ONE;
GLint gl_blend_dst_def=PVR_BLEND_ZERO;

extern GLboolean gl_use_4444;

#define ATTR_STACK_CNT 1024
static unsigned gl_attr_stack[ATTR_STACK_CNT+16];
static int gl_attr_top = 0;


/* Miscellaneous APIs */
GLenum glGetError (void)
{
	return gl_error;
}

const GLubyte* gluErrorString(GLenum errorCode){
  return (const GLubyte*)"glGetError returns the value of the error flag";
}

void glPushAttrib(GLbitfield mask){
	if (gl_attr_top < ATTR_STACK_CNT)
	{
		if (mask&GL_DEPTH_BUFFER_BIT)
		{
			gl_attr_stack[gl_attr_top++]=gl_depth_test;
			gl_attr_stack[gl_attr_top++]=gl_depth_mask;
			gl_attr_stack[gl_attr_top++]=gl_depth_func;
		}
		if (mask&GL_COLOR_BUFFER_BIT)
		{
			unsigned *col=(unsigned *)&gl_clear_color[0];
			gl_attr_stack[gl_attr_top++]=gl_first_list;
			gl_attr_stack[gl_attr_top++]=gl_blend_src_def;
			gl_attr_stack[gl_attr_top++]=gl_blend_dst_def;
			gl_attr_stack[gl_attr_top++]=col[0];
			gl_attr_stack[gl_attr_top++]=col[1];
			gl_attr_stack[gl_attr_top++]=col[2];
			gl_attr_stack[gl_attr_top++]=col[3];
		}
		if (mask&GL_TEXTURE_BIT)
		{
			gl_attr_stack[gl_attr_top++]=gl_poly_cxt.txr.uv_clamp;
			gl_attr_stack[gl_attr_top++]=gl_poly_cxt.txr.filter;
			gl_attr_stack[gl_attr_top++]=gl_poly_cxt.txr.enable;
			gl_attr_stack[gl_attr_top++]=(unsigned)gl_cur_texture;
		}
		if (mask&GL_POLYGON_BIT)
		{
			gl_attr_stack[gl_attr_top++]=gl_cull_face;
		}
		gl_attr_stack[gl_attr_top++]=mask;
	}
}

void glPopAttrib(void){
	if (gl_attr_top > 0)
	{
		GLbitfield mask=(GLbitfield) gl_attr_stack[--gl_attr_top];
		if (mask&GL_POLYGON_BIT)
		{
			gl_cull_face=gl_attr_stack[--gl_attr_top];
			if (gl_cull_face)
				gl_capabilities|=GL_CULL_FACE;
			else
				gl_capabilities&= ~(GL_CULL_FACE);
		}
		if (mask&GL_TEXTURE_BIT)
		{
			gl_cur_texture=(pvr_poly_cxt_t *)gl_attr_stack[--gl_attr_top];
			gl_poly_cxt.txr.enable=gl_attr_stack[--gl_attr_top];
			gl_poly_cxt.txr.uv_clamp=gl_attr_stack[--gl_attr_top];
			gl_poly_cxt.txr.filter=gl_attr_stack[--gl_attr_top];
			gl_pbuf_submitted = GL_FALSE;
			if (gl_poly_cxt.txr.enable)
				gl_capabilities|=GL_TEXTURE_2D;
			else
				gl_capabilities&= ~(GL_TEXTURE_2D);
		}
		if (mask&GL_COLOR_BUFFER_BIT)
		{
			unsigned *col=(unsigned *)&gl_clear_color[0];
			col[3]=gl_attr_stack[--gl_attr_top];
			col[2]=gl_attr_stack[--gl_attr_top];
			col[1]=gl_attr_stack[--gl_attr_top];
			col[0]=gl_attr_stack[--gl_attr_top];
			pvr_set_bg_color(gl_clear_color[0]*gl_clear_color[3],
					 gl_clear_color[1]*gl_clear_color[3],
					 gl_clear_color[2]*gl_clear_color[3]);
			gl_blend_dst_def=gl_attr_stack[--gl_attr_top];
			gl_blend_src_def=gl_attr_stack[--gl_attr_top];
			gl_first_list=gl_attr_stack[--gl_attr_top];
			if (gl_first_list == GL_LIST_TRANS_POLY)
			{
				if (gl_active_list != GL_LIST_TRANS_POLY)
					        glKosFinishList();
				gl_blend_src=gl_blend_src_def;
				gl_blend_dst=gl_blend_dst_def;
				gl_capabilities|=GL_BLEND;
			}
			else
			{
				gl_blend_src=PVR_BLEND_ONE;
				gl_blend_dst=PVR_BLEND_ZERO;
				gl_capabilities&= ~(GL_BLEND);
			}
		}
		if (mask&GL_DEPTH_BUFFER_BIT)
		{
			gl_depth_func=gl_attr_stack[--gl_attr_top];
			gl_depth_mask=gl_attr_stack[--gl_attr_top];
			gl_depth_test=gl_attr_stack[--gl_attr_top];

			if (gl_depth_test)
			{
				gl_capabilities|=GL_DEPTH_TEST;
				glDepthFunc(gl_depth_func);
				glDepthMask(gl_depth_mask);
				gl_capabilities|=GL_DEPTH_TEST;
			}
			else
			{
				GLenum df=gl_depth_func;
				GLboolean dm=gl_depth_mask;
				gl_depth_test=GL_TRUE;
				glDepthFunc(GL_ALWAYS);
				glDepthMask(GL_FALSE);
				gl_depth_test=GL_FALSE;
				gl_depth_func=df;
				gl_depth_mask=dm;
				gl_capabilities&= ~(GL_DEPTH_TEST);
			}
		}
    		gl_pbuf_submitted = GL_FALSE;
	}
}

/*
 * Translate the nth element of list from type to GLuint.
 */
static GLuint translate_id( GLsizei n, GLenum type, const GLvoid *list )
{
   GLbyte *bptr;
   GLubyte *ubptr;
   GLshort *sptr;
   GLushort *usptr;
   GLint *iptr;
   GLuint *uiptr;
   GLfloat *fptr;

   switch (type) {
      case GL_BYTE:
         bptr = (GLbyte *) list;
         return (GLuint) *(bptr+n);
      case GL_UNSIGNED_BYTE:
         ubptr = (GLubyte *) list;
         return (GLuint) *(ubptr+n);
      case GL_SHORT:
         sptr = (GLshort *) list;
         return (GLuint) *(sptr+n);
      case GL_UNSIGNED_SHORT:
         usptr = (GLushort *) list;
         return (GLuint) *(usptr+n);
      case GL_INT:
         iptr = (GLint *) list;
         return (GLuint) *(iptr+n);
      case GL_UNSIGNED_INT:
         uiptr = (GLuint *) list;
         return (GLuint) *(uiptr+n);
      case GL_FLOAT:
         fptr = (GLfloat *) list;
         return (GLuint) *(fptr+n);
      case GL_2_BYTES:
         ubptr = ((GLubyte *) list) + 2*n;
         return (GLuint) *ubptr * 256 + (GLuint) *(ubptr+1);
      case GL_3_BYTES:
         ubptr = ((GLubyte *) list) + 3*n;
         return (GLuint) *ubptr * 65536
              + (GLuint) *(ubptr+1) * 256
              + (GLuint) *(ubptr+2);
      case GL_4_BYTES:
         ubptr = ((GLubyte *) list) + 4*n;
         return (GLuint) *ubptr * 16777216
              + (GLuint) *(ubptr+1) * 65536
              + (GLuint) *(ubptr+2) * 256
              + (GLuint) *(ubptr+3);
      default:
         return 0;
   }
}
GLuint lb=0;
void glListBase( GLuint base ){
 lb = base;
// printf("kglx list base= %d\n",lb);
    
}

/*
 * Execute glCallLists:  call multiple display lists.
 */

void glCallLists( GLsizei n, GLenum type, const GLvoid *lists )
{
//   GET_CURRENT_CONTEXT(ctx);
   GLuint list;
   GLint i;
//   GLboolean save_compile_flag;

//   if (MESA_VERBOSE & VERBOSE_API)
//      _mesa_debug(ctx, "glCallLists %d\n", n);

   switch (type) {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FLOAT:
      case GL_2_BYTES:
      case GL_3_BYTES:
      case GL_4_BYTES:
         /* OK */
         break;
      default:
         assert_msg(!type, "glCallLists(type)");
         return;
   }

//   /* Save the CompileFlag status, turn it off, execute display list,
//    * and restore the CompileFlag.
//    */
//   save_compile_flag = ctx->CompileFlag;
//   ctx->CompileFlag = GL_FALSE;

   for (i=0;i<n;i++) {
      list = translate_id( i, type, lists );
      glCallList( lb + list );
      
   }

//   ctx->CompileFlag = save_compile_flag;
//
//   /* also restore API function pointers to point to "save" versions */
//   if (save_compile_flag) {
//      ctx->CurrentDispatch = ctx->Save;
//      _glapi_set_dispatch( ctx->CurrentDispatch );
//   }
}



typedef void (*funcptr)();

const static struct {
	char *name;
	funcptr addr;
} glfuncs[] = {
#define	DEF(func)	{#func,&func}
#undef	DEF
};
 
funcptr glKosGetProcAddress(const char *extension)
{
	int i;
	for(i=0;i<sizeof(glfuncs)/sizeof(glfuncs[0]);i++) {
		if (strcmp(extension,glfuncs[i].name)==0) return glfuncs[i].addr;
	}
	return NULL;
} 

void glPixelStorei(GLenum pname,GLint param)
{
}

/* Set the background clear color */
void glClearColor(GLclampf red,
                  GLclampf green,
                  GLclampf blue,
                  GLclampf alpha) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    gl_clear_color[0] = CLAMP01(red);
    gl_clear_color[1] = CLAMP01(green);
    gl_clear_color[2] = CLAMP01(blue);
    gl_clear_color[3] = CLAMP01(alpha);
    pvr_set_bg_color(gl_clear_color[0]*gl_clear_color[3],
                     gl_clear_color[1]*gl_clear_color[3],
                     gl_clear_color[2]*gl_clear_color[3]);
}

/* NOP: there's nothing to clear on the PVR2 */
void glClear(GLbitfield mask) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
}

/* Set front face */
void glFrontFace(GLenum mode) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    switch(mode) {
    case GL_CW:
    case GL_CCW:
        gl_front_face = mode;
        gl_pbuf_submitted = GL_FALSE;
        break;
    }
}

/* Set face culling mode */
void glCullFace(GLenum mode) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    switch(mode) {
    case GL_FRONT:
    case GL_BACK:
        gl_cull_mode = mode;
        gl_pbuf_submitted = GL_FALSE;
        break;
    }
}

/* Enable / disable capabilities */
void glEnable(GLenum cap) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    switch(cap) {
    case GL_DEPTH_TEST:
	if (!gl_depth_test)
	{
		gl_depth_test=GL_TRUE;
		glDepthFunc(gl_depth_func);
		glDepthMask(gl_depth_mask);
	}
        break;
    case GL_TEXTURE_2D:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.txr.enable = PVR_TEXTURE_ENABLE;
        break;
    case GL_BLEND:
	if (gl_first_list != GL_LIST_TRANS_POLY)
	{
        	gl_first_list=GL_LIST_TRANS_POLY;
		gl_blend_src=gl_blend_src_def;
		gl_blend_dst=gl_blend_dst_def;
    		gl_pbuf_submitted = GL_FALSE;
	}
	if (gl_active_list != GL_LIST_TRANS_POLY)
        	glKosFinishList();
        break;
    case GL_CULL_FACE:
        gl_pbuf_submitted = GL_FALSE;
        gl_cull_face = GL_TRUE;
        break;
    case GL_FOG:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.fog_type = PVR_FOG_TABLE;
        break;
    case GL_SCISSOR_TEST:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
        if (USERCLIP_OUTSIDE_ENABLED)
            gl_capabilities &= ~((GLbitfield)GL_KOS_USERCLIP_OUTSIDE);
        break;
    case GL_KOS_USERCLIP_OUTSIDE:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.clip_mode = PVR_USERCLIP_OUTSIDE;
        if (SCISSOR_ENABLED)
            gl_capabilities &= ~((GLbitfield)GL_SCISSOR_TEST);
        break;
    case GL_KOS_MODIFIER:
        assert_msg(0, "GL_KOS_MODIFER not implemented");
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.fmt.modifier = PVR_MODIFIER_ENABLE;
        break;
    case GL_KOS_CHEAP_SHADOW:
        assert_msg(0, "GL_KOS_CHEAP_SHADOW not implemented");
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.modifier_mode = PVR_MODIFIER_CHEAP_SHADOW;
        break;
    case GL_KOS_NEARZ_CLIPPING:
        // gl_matrix_dirty = GL_TRUE;
        // kos_nearz_clipping=1;
        break;
    case GL_LIGHT0:
        assert_msg(0, "GL_LIGHTi not implemented");
        break;
    case GL_ARGB4444:
    	gl_use_4444 = GL_TRUE;
    	break;
    default:
     	return;
    }
    gl_capabilities |= (GLbitfield)cap;
}

void glDisable(GLenum cap) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    switch(cap) {
    case GL_DEPTH_TEST:
	if (gl_depth_test)
	{
		GLenum df=gl_depth_func;
		GLboolean dm=gl_depth_mask;
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);
		gl_depth_test=GL_FALSE;
		gl_depth_func=df;
		gl_depth_mask=dm;
	}
        break;
    case GL_TEXTURE_2D:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.txr.enable = PVR_TEXTURE_DISABLE;
        break;
    case GL_BLEND:
	if (gl_first_list != GL_LIST_FIRST)
	{
		gl_first_list=GL_LIST_FIRST;
		gl_blend_src=PVR_BLEND_ONE;
		gl_blend_dst=PVR_BLEND_ZERO;
    		gl_pbuf_submitted = GL_FALSE;
	}
        break;
    case GL_CULL_FACE:
        gl_pbuf_submitted = GL_FALSE;
        gl_cull_face = GL_FALSE;
        break;
    case GL_FOG:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.fog_type = PVR_FOG_DISABLE;
        break;
    case GL_SCISSOR_TEST:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.clip_mode = PVR_USERCLIP_DISABLE;
        if (USERCLIP_OUTSIDE_ENABLED)
            gl_capabilities &= ~((GLbitfield)GL_KOS_USERCLIP_OUTSIDE);
        break;
    case GL_KOS_USERCLIP_OUTSIDE:
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.clip_mode = PVR_USERCLIP_DISABLE;
        if (SCISSOR_ENABLED)
            gl_capabilities &= ~((GLbitfield)GL_SCISSOR_TEST);
        break;
    case GL_KOS_MODIFIER:
        assert_msg(0, "GL_KOS_MODIFER not implemented");
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.fmt.modifier = PVR_MODIFIER_DISABLE;
        break;
    case GL_KOS_CHEAP_SHADOW:
        assert_msg(0, "GL_KOS_CHEAP_SHADOW not implemented");
        gl_pbuf_submitted = GL_FALSE;
        gl_poly_cxt.gen.modifier_mode = PVR_MODIFIER_NORMAL;
        break;
    case GL_KOS_NEARZ_CLIPPING:
        //gl_matrix_dirty = GL_TRUE;
        //kos_nearz_clipping=0;
        break;
    case GL_LIGHT0:
        assert_msg(0, "GL_LIGHTi not implemented");
        break;
    case GL_ARGB4444:
    	gl_use_4444 = GL_FALSE;
    	break;
    default:
     	return;
    }
    gl_capabilities &= ~((GLbitfield)cap);
}

GLboolean glIsEnabled(GLenum cap) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    switch(cap) {
    case GL_ALPHA_TEST:
	return GL_TRUE;
    case GL_DEPTH_TEST:
	return gl_depth_test;
    case GL_TEXTURE_2D:
	return ((gl_pbuf_submitted == GL_FALSE) && (gl_poly_cxt.txr.enable == PVR_TEXTURE_ENABLE));
    case GL_BLEND:
        return (gl_first_list == GL_LIST_TRANS_POLY);
    case GL_CULL_FACE:
	return ((gl_pbuf_submitted == GL_FALSE) && (gl_cull_face == GL_TRUE));
    case GL_FOG:
	return((gl_pbuf_submitted == GL_FALSE) && (gl_poly_cxt.gen.fog_type == PVR_FOG_TABLE));
    case GL_SCISSOR_TEST:
    	return ((gl_pbuf_submitted == GL_FALSE) && (gl_poly_cxt.gen.clip_mode == PVR_USERCLIP_INSIDE));
    case GL_KOS_USERCLIP_OUTSIDE:
        return ((gl_pbuf_submitted == GL_FALSE) && (gl_poly_cxt.gen.clip_mode == PVR_USERCLIP_OUTSIDE));
    case GL_KOS_MODIFIER:
    	return ((gl_pbuf_submitted == GL_FALSE) && (gl_poly_cxt.fmt.modifier == PVR_MODIFIER_ENABLE));
    case GL_KOS_CHEAP_SHADOW:
    	return ((gl_pbuf_submitted == GL_FALSE) && (gl_poly_cxt.gen.modifier_mode == PVR_MODIFIER_CHEAP_SHADOW));
    case GL_ARGB4444:
    	return gl_use_4444;
    }
    return GL_FALSE;
}

/* We have no rendering pipeline yet, so this is a NOP */
void glFlush() {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
}

void glFinish() {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    glKosFinishFrame();
    glKosBeginFrame();
}

/* There aren't any useful hints to set yet, so this is a NOP */
void glHint(GLenum target, GLenum mode) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
}

void glPointSize(GLfloat size) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    gl_point_size = CLAMP(size, 0.0f, 100.0f);
}

/* These strings should go somewhere else */
const GLubyte *glGetString(GLenum name) {
    assert_msg(!gl_prim_type, "Not allowed within glBegin/glEnd pair.");
    switch(name) {
    case GL_VENDOR:
        return (const GLubyte *)"Cryptic Allusions";

    case GL_RENDERER:
        return (const GLubyte *)"KallistiOS";

    case GL_VERSION:
        return (const GLubyte *)"1.1";

    case GL_EXTENSIONS:
        return (const GLubyte *)"GL_ARB_transpose_matrix ";
    }

    return (const GLubyte *)"";
}

/* A limited view into the guts of KGL  - This will change! */
#if 0
void glGetFloatv(GLenum pname, GLfloat *params) {
    int i, j;
    matrix_t *pmat = (matrix_t*)params;

    switch(pname) {
    case GL_MODELVIEW_MATRIX:
        for (i=0; i<4; i++)
            for (j=0; j<4; j++)
                (*pmat)[i][j] = gl_trans_mats[GL_MODELVIEW][i][j];
        break;
    case GL_PROJECTION_MATRIX:
        for (i=0; i<4; i++)
            for (j=0; j<4; j++)
                (*pmat)[i][j] = gl_trans_mats[GL_PROJECTION][i][j];
        break;
    case GL_TEXTURE_MATRIX:
        for (i=0; i<4; i++)
            for (j=0; j<4; j++)
                (*pmat)[i][j] = gl_trans_mats[GL_TEXTURE][i][j];
        break;
    default:
        assert_msg(0, "not glGet param not implemented");
    }
}
#else
void glGetFloatv( GLenum pname , GLfloat *m ){
    memcpy(m,
           gl_trans_mats + pname,
           sizeof(matrix_t));

}
#endif
