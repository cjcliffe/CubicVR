
/*
	KGL-X 0.0

	gloparray.c

	(c) 2002 heinrich tillack

	

   under KOS license


*/

#include <GL/gl.h>

#include "glinternal.h"

#if 0
extern void glopColor(GLParam * p) ;
extern void glopVertex(GLParam * p) ;

extern void gl_add_op(GLParam *p);
#endif


int gl_vertex_array_size;
int gl_vertex_array_stride;
GLfloat  *gl_vertex_array;

int gl_normal_array_stride ;
GLfloat *gl_normal_array ;

int gl_color_array_size ;
int gl_color_array_stride;
GLfloat *gl_color_array ;

int gl_texcoord_array_size ;
int gl_texcoord_array_stride;
GLfloat *gl_texcoord_array ;

GLbitfield gl_client_states=0;

//extern GLfloat vert_u,vert_v;

/* */

void glopArrayElement(GLParam *param) {
    int idx = param[1].i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(gl_client_states & COLOR_ARRAY) {
        GLParam p[5];
        int size = gl_color_array_size;
	float *a=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*idx));
        p[1].f = *a++;
        p[2].f = *a++;
        p[3].f = *a++;
        p[4].f = size > 3 ? *a : 1.0f;
        glopColor(p);
    }

#if NOT_IMPLEMENTED
    if(gl_client_states & NORMAL_ARRAY) {
	float *a=(float *)(((unsigned)gl_normal_array)+(gl_normal_array_stride*idx));
        nx = *a++;
        ny = *a++;
        nz = *a++;
        nw=.....
       }
#endif

       if(gl_client_states & TEXCOORD_ARRAY) {
	   float *a=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*idx));
           vert_u = *a++;
           vert_v = *a;
       }

       if(gl_client_states & VERTEX_ARRAY) {
           GLParam p[4];
           int	size = gl_vertex_array_size;
	   float *a=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
           p[1].f = a[0];
           p[2].f = a[1];
           p[3].f = size > 2 ? a[2] : 0.0f;
           glopVertex(p);
       }
   }


   /* */
   void glopEnableClientState( GLParam *p) {
       gl_client_states |= p[1].i;
   }

   /* */
   void glopDisableClientState( GLParam *p) {
       gl_client_states &= p[1].i;
   }

   /* */
   void glopVertexPointer( GLParam *p) {
       gl_vertex_array_size = p[1].i;
       if (p[2].i)
       	gl_vertex_array_stride = p[2].i;
       else
	gl_vertex_array_stride=gl_vertex_array_size*sizeof(float);
       gl_vertex_array = (GLfloat*)p[3].p;
   }

   /* */
   void glopColorPointer( GLParam *p) {
       gl_color_array_size = p[1].i;
       if (p[2].i)
       	gl_color_array_stride = p[2].i;
       else
	gl_color_array_stride=gl_color_array_size*sizeof(float);
       gl_color_array = (GLfloat*)p[3].p;
   }

   /* */
   void glopNormalPointer( GLParam *p) {
       if (p[1].i)
        gl_normal_array_stride = p[1].i;
       else
	gl_normal_array_stride=3*sizeof(float);
       gl_normal_array = (GLfloat*)p[2].p;
   }

   /* */
   void glopTexCoordPointer( GLParam *p) {
       gl_texcoord_array_size = p[1].i;
       if (p[2].i)
       	gl_texcoord_array_stride = p[2].i;
       else
	gl_texcoord_array_stride=gl_texcoord_array_size*sizeof(float);
       gl_texcoord_array = (GLfloat*)p[3].p;
   }
