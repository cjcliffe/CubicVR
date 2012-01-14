/* KGL-X 0.2

   glapi.c


   (c) 2002 heinrich tillack

   under KOS license
*/

/*

	glapi.c defines thos glxxx(*) functions which
	could be or actually are part of a displaylist.

	XXX TO DO: glFog*() ??!
*/

#include "kfeatures.h"

#define USE_FAST_TRIANGLES_PATCH
#define USE_FAST_QUADS_PATCH
#define USE_FAST_QUAD_STRIP_PATCH


#include "glinternal.h"

#include <assert.h>

extern void gl_add_op(GLParam *p);

/* glBegin / glEnd */

void glBegin(GLenum mode) {

    GLParam p[2];

    p[0].op=OP_Begin;
    p[1].i=mode;

    gl_add_op(p);
}

void glEnd(void) {
    GLParam p[1];

    p[0].op=OP_End;

    gl_add_op(p);
}



/* Vertex */

void glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    GLParam p[4];

    p[0].op=OP_Vertex;
    p[1].f=x;
    p[2].f=y;
    p[3].f=z;

    gl_add_op(p);
}

void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    glVertex3f(x, y, z);
}

void glVertex4fv(const GLfloat *v) {
    glVertex3f(v[0], v[1], v[2]);
}

void glVertex3fv(const GLfloat *v) {
    glVertex3f(v[0], v[1], v[2]);
}

void glVertex2f(GLfloat x,GLfloat y) {
    glVertex3f(x, y, 0.0f);
}

void glVertex2i(GLint x,GLint y) {
	glVertex3f((GLfloat)x,(GLfloat)y,0.0f);
}

void glVertex2fv(const GLfloat *v) {
    glVertex3f(v[0], v[1], 0.0f);
}

/* Normal */

void  glNormal3f(float x,float y,float z)
{
    GLParam p[4];

    p[0].op=OP_Normal;
    p[1].f=x;
    p[2].f=y;
    p[3].f=z;

    gl_add_op(p);
}

void glNormal3fv( GLfloat  *v)
{
    glNormal3f(v[0],v[1],v[2]);
}

/* Color */

void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {

    GLParam p[5];

    p[0].op=OP_Color;
    p[1].f=red;
    p[2].f=green;
    p[3].f=blue;
    p[4].f=alpha;

    gl_add_op(p);
}

void glColor4fv(const GLfloat *v) {

    GLParam p[5];

    p[0].op=OP_Color;
    p[1].f=v[0];
    p[2].f=v[1];
    p[3].f=v[2];
    p[4].f=v[3];

    gl_add_op(p);
}

void glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
    glColor4f(red,green,blue,1.0f);
}

void glColor3fv(const GLfloat *v)
{
    glColor4f(v[0],v[1],v[2],1.0f);
}


void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) {

    glColor4f((float)red/255,(float)green/255,(float)blue/255,(float)alpha/255);
}

// void glColor4ubv( GLubyte *v) {
// 	SET_COLOR(v[0],v[1],v[2],v[3]);
// }

void glColor3ub(GLubyte red, GLubyte green, GLubyte blue) {
    glColor4f((float)red/255,(float)green/255,(float)blue/255,1);
}

// void glColor3ubv( GLubyte *v) {
// 	SET_COLOR(v[0],v[1],v[2],0xff);
// }



/* TexCoord */

void  glTexCoord2f(GLfloat s, GLfloat t)
{
    GLParam p[3];

    p[0].op=OP_TexCoord;
    p[1].f=s;
    p[2].f=t;

    gl_add_op(p);
}

void glTexCoord2fv( const GLfloat *v)
{
    glTexCoord2f(v[0],v[1]);
}

void glTexCoord1f( GLfloat s ){
    glTexCoord2f(s,1.0f);
}

void glTexCoord1fv( const GLfloat *v){
    glTexCoord2f(v[0],1.0f);
}

void glTexCoord3f( GLfloat s, GLfloat t, GLfloat r){
    glTexCoord2f(s,t);
}

void glTexCoord3fv( const GLfloat *v){
    glTexCoord2f(v[0],v[1]);
}

void glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q){
    glTexCoord2f(s,t);
}

void glTexCoord4fv( const GLfloat *v){
    glTexCoord2f(v[0],v[1]);
}

// void glEdgeFlag(int flag)
// {
//   GLParam p[2];

//   p[0].op=OP_EdgeFlag;
//   p[1].i=flag;

//   gl_add_op(p);
// }


/* Depth */

// /* Depth buffer (non-functional, just stubs) */
// void glClearDepth(GLclampd depth) {

// }

// void glDepthFunc(GLenum func) {

//     GLParam p[2];

//     p[0].op=OP_DepthFunc;
//     p[1].i=func;


//     gl_add_op(p);

// }

// void glDepthMask(GLboolean flag) {

//     GLParam p[2];

//     p[0].op=OP_DepthMask;
//     //XXX conversion ok?
//     p[1].i=(int)flag;


//     gl_add_op(p);

// }

/* Transform */

void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    GLParam p[4];

    p[0].op=OP_Translatef;
    p[1].f=x;
    p[2].f=y;
    p[3].f=z;

    gl_add_op(p);
}

void glTranslated( GLdouble x, GLdouble y, GLdouble z ) {
	glTranslatef(x,y,z);
}

void glRotatef(GLfloat angle,
               GLfloat x, GLfloat y, GLfloat z) {

    GLParam p[5];

    p[0].op=OP_Rotatef;
    p[1].f=angle;
    p[2].f=x;
    p[3].f=y;
    p[4].f=z;

    gl_add_op(p);
}


void glScalef(GLfloat x, GLfloat y, GLfloat z) {

    GLParam p[4];

    p[0].op=OP_Scalef;
    p[1].f=x;
    p[2].f=y;
    p[3].f=z;

    gl_add_op(p);
}

/* Texture */

void glBindTexture(GLenum type, GLuint texture) {

 if (texture)
 {
    GLParam p[3];

    p[0].op=OP_BindTexture;
    p[1].i=(int)type;
    p[2].ui=(unsigned int)texture;

    gl_add_op(p);
 }
}

void glBlendFunc(GLenum sfactor, GLenum dfactor) {
	GLParam p[3];

	p[0].op=OP_BlendFunc;
	p[1].i=(int)sfactor;
	p[2].i=(int)dfactor;

    	gl_add_op(p);
}

/* Misc API */
void glPolygonMode (GLenum face, GLenum mode)
{
    GLParam p[3];

    p[0].op=OP_PolygonMode;

    p[1].i=face;
    p[2].i=mode;

    gl_add_op(p);

}


/* Special Functions */


void glCallList( GLuint list )
{
    GLParam p[2];

    p[0].op=OP_CallList;
    p[1].ui=list;

    gl_add_op(p);
}


/* Non standard functions */

void glDebug(int mode)
{
    kos_print_flag=mode;
}



/* Arrays */

extern int gl_color_array_size;
extern GLfloat *gl_color_array;
extern int gl_color_array_stride;
extern int gl_vertex_array_size;
extern GLfloat vert_u;
extern GLfloat vert_v;
extern gl_vertex_t *vtxbuf;
extern int vbuf_top;
extern GLuint vert_rgba;
extern GLfloat *gl_texcoord_array;
extern GLfloat *gl_vertex_array;
extern int gl_texcoord_array_stride;
extern int gl_vertex_array_stride;
extern GLbitfield gl_client_states;

/* */
static __inline__ void internal_glArrayElement(GLint i) {
    GLParam p[2];
    p[0].op = OP_ArrayElement;
    p[1].i = i;
    gl_add_op(p);
}

void glArrayElement(GLint i) {
	internal_glArrayElement(i);
}


/* */
static void glDrawArrays_old( GLenum mode, GLint first, GLsizei count )
{
    int i;

    glBegin(mode);
    for(i=first;i<first+count;i++) {
        internal_glArrayElement(i);
    }
    glEnd();
}

/* */
static void glDrawElements_old( GLenum mode, GLsizei count, const GLint *index )
{
	int i;
	glBegin(mode);
	for(i=0;i<count;i++)
		internal_glArrayElement(index[i]);
	glEnd();
}

#if defined(USE_FAST_TRIANGLES_PATCH) || defined(USE_FAST_QUADS_PATCH) || defined(USE_FAST_QUAD_STRIP_PATCH)
extern pvr_poly_hdr_t gl_user_clip;

void send_poly_hdr(void);

//#define CLIP_Z  0.001f
#define	CLIP_Z	0.000001f
#define	Z_NEAR_VALUE CLIP_Z
#define LESS_Z  1 
#define GEQUAL_Z 0

#define mat_rot_single3(x, y, z) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	__asm__ __volatile__( \
		"fldi1  fr3\n" \
		"ftrv   xmtrx,fv0\n" \
		"fmov   fr3,fr2" \
		: "=f" (__x), "=f" (__y), "=f" (__z) \
		: "0" (__x), "1" (__y), "2" (__z) \
		: "fr3" ); \
	x = __x; y = __y; z = __z;  \
}

#define getXYZF(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*index[i++])); \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}

#define getXYZF_array(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*i)); i++; \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}

#define sendXYZ(FL,V) \
{ \
	float rhw= 1 / (V).z; \
	vert->flags = (FL); \
	vert->x = (V).x*rhw; \
	vert->y = (V).y*rhw; \
	vert->z = rhw; \
/* vert->u=vert_u; */ \
/* vert->u=vert_v; */ \
/* vert->argb = vert_rgba; */ \
/* vert->oargb=0xff000000; */ \
	pvr_dr_commit(vert); \
	vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 )); \
}

#define getXYZUVF(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*index[i])); \
	float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*index[i++])); \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	(V).u = *t++; \
	(V).v = *t; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}

#define getXYZUVF_array(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*i)); \
	float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*i)); i++; \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	(V).u = *t++; \
	(V).v = *t; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}
#define sendXYZUV(FL,V) \
{ \
	float rhw= 1 / (V).z; \
	vert->flags = (FL); \
	vert->x = (V).x*rhw; \
	vert->y = (V).y*rhw; \
	vert->u = (V).u; \
	vert->v = (V).v; \
	vert->z = rhw; \
/* vert->argb = vert_rgba; */ \
/* vert->oargb=0xff000000; */ \
	pvr_dr_commit(vert); \
	vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 )); \
}

#define getXYZFAO(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*index[i])); \
	float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*index[i++])); \
	float r=*c++; \
	float g=*c++; \
	float b=*c++; \
	float a=size_c > 3 ? *c : 1.0f; \
	(V).argb=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff))  \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}

#define getXYZFAO_array(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*i)); \
	float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*i)); i++; \
	float r=*c++; \
	float g=*c++; \
	float b=*c++; \
	float a=size_c > 3 ? *c : 1.0f; \
	(V).argb=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff))  \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}
#define sendXYZAO(FL,V) \
{ \
	float rhw= 1 / (V).z; \
	vert->flags = (FL); \
	vert->x = (V).x*rhw; \
	vert->y = (V).y*rhw; \
	vert->z = rhw; \
	vert->argb = (V).argb; \
/* vert->u = vert_u; */ \
/* vert->v = vert_v; */ \
/* vert->oargb=0xff000000; */ \
	pvr_dr_commit(vert); \
	vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 )); \
}

#define getXYZFUVAO(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*index[i])); \
	float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*index[i])); \
	float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*index[i++])); \
	float r=*c++; \
	float g=*c++; \
	float b=*c++; \
	float a=size_c > 3 ? *c : 1.0f; \
	(V).argb=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff))  \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	(V).u = *t++; \
	(V).v = *t; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}

#define getXYZFUVAO_array(V)  \
{ \
	float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*i)); \
	float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*i)); \
	float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*i)); i++; \
	float r=*c++; \
	float g=*c++; \
	float b=*c++; \
	float a=size_c > 3 ? *c : 1.0f; \
	(V).argb=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff))  \
	(V).x = *v++; \
	(V).y = *v++; \
	(V).z = size_v > 2 ? *v : 0.0f; \
	(V).u = *t++; \
	(V).v = *t; \
	mat_rot_single3((V).x,(V).y,(V).z) \
	if ((V).z < Z_NEAR_VALUE) \
	{ \
		(V).flags=LESS_Z; \
		n++; \
	} \
	else \
		(V).flags=GEQUAL_Z; \
}

#define sendXYZUVAO(FL,V) \
{ \
	float rhw= 1 / (V).z; \
	vert->flags = (FL); \
	vert->x = (V).x*rhw; \
	vert->y = (V).y*rhw; \
	vert->u = (V).u; \
	vert->v = (V).v; \
	vert->z = rhw; \
	vert->argb = (V).argb; \
/* vert->oargb=0xff000000; */ \
	pvr_dr_commit(vert); \
	vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 )); \
}




#define ARGB_A(c)       (c>>24)                                                 
#define ARGB_R(c)       ((c>>16)&0xff)
#define ARGB_G(c)       ((c>>8)&0xff)
#define ARGB_B(c)       (c&0xff)

				

static __inline__ void *pvr_commit_init(void)
{
	QACR0 = ((((uint32)PVR_TA_INPUT) >> 26) << 2) & 0x1c;
	QACR1 = ((((uint32)PVR_TA_INPUT) >> 26) << 2) & 0x1c;
	return (void*)(0xe0000000);
}

static __inline__ GLuint mix_color(GLuint argb0,GLuint argb1,float d)
{
	if (argb0 != argb1)
	{
		int a,r,g,b;
		a = ARGB_A(argb0) + (int)((ARGB_A(argb1) - ARGB_A(argb0))*d);
		r = ARGB_R(argb0) + (int)((ARGB_R(argb1) - ARGB_R(argb0))*d);
		g = ARGB_G(argb0) + (int)((ARGB_G(argb1) - ARGB_G(argb0))*d);
		b = ARGB_B(argb0) + (int)((ARGB_B(argb1) - ARGB_B(argb0))*d);
		return (((a<<8) | r)<<16) | (g<<8) | (b);
	}	
	return argb0;
}

static gl_vertex_t tempvert[4]  __attribute__((aligned(32)));

#define ClipZ_simple(J,I) \
{ \
	float d = (CLIP_Z - vert[(I)]->z)/(vert[(I)+1]->z - vert[(I)]->z); \
	tempvert[(J)].x = vert[(I)]->x + (vert[(I)+1]->x - vert[(I)]->x)*d; \
	tempvert[(J)].y = vert[(I)]->y + (vert[(I)+1]->y - vert[(I)]->y)*d; \
	tempvert[(J)].z = CLIP_Z; \
}

#define ClipZ_texture(J,I) \
{ \
	float d = (CLIP_Z - vert[(I)]->z)/(vert[(I)+1]->z - vert[(I)]->z); \
	tempvert[(J)].x = vert[(I)]->x + (vert[(I)+1]->x - vert[(I)]->x)*d; \
	tempvert[(J)].y = vert[(I)]->y + (vert[(I)+1]->y - vert[(I)]->y)*d; \
	tempvert[(J)].z = CLIP_Z; \
	tempvert[(J)].u = vert[(I)]->u + (vert[(I)+1]->u - vert[(I)]->u)*d; \
	tempvert[(J)].v = vert[(I)]->v + (vert[(I)+1]->v - vert[(I)]->v)*d; \
}

#define ClipZ_color(J,I) \
{ \
	float d = (CLIP_Z - vert[(I)]->z)/(vert[(I)+1]->z - vert[(I)]->z); \
	tempvert[(J)].x = vert[(I)]->x + (vert[(I)+1]->x - vert[(I)]->x)*d; \
	tempvert[(J)].y = vert[(I)]->y + (vert[(I)+1]->y - vert[(I)]->y)*d; \
	tempvert[(J)].z = CLIP_Z; \
	tempvert[(J)].argb = mix_color(vert[(I)]->argb,vert[(I)+1]->argb,d); \
	tempvert[(J)].oargb = mix_color(vert[(I)]->oargb,vert[(I)+1]->oargb,d); \
}

#define ClipZ_full(J,I) \
{ \
	float d = (CLIP_Z - vert[(I)]->z)/(vert[(I)+1]->z - vert[(I)]->z); \
	tempvert[(J)].x = vert[(I)]->x + (vert[(I)+1]->x - vert[(I)]->x)*d; \
	tempvert[(J)].y = vert[(I)]->y + (vert[(I)+1]->y - vert[(I)]->y)*d; \
	tempvert[(J)].z = CLIP_Z; \
	tempvert[(J)].u = vert[(I)]->u + (vert[(I)+1]->u - vert[(I)]->u)*d; \
	tempvert[(J)].v = vert[(I)]->v + (vert[(I)+1]->v - vert[(I)]->v)*d; \
	tempvert[(J)].argb = mix_color(vert[(I)]->argb,vert[(I)+1]->argb,d); \
	tempvert[(J)].oargb = mix_color(vert[(I)]->oargb,vert[(I)+1]->oargb,d); \
}

#define ZClip_template(NAME) \
static int ZClip_##NAME(gl_vertex_t *v0, gl_vertex_t *v1, gl_vertex_t *v2) \
{ \
	gl_vertex_t *vert[4]; \
	int i=0,j=0; \
	vert[0]=vert[3]=v0; vert[1]=v1; vert[2]=v2; \
	while(i<3) \
	{ \
		if (vert[i]->flags){ \
			if (!vert[i+1]->flags){ \
				tempvert[j]=*(vert[i]); \
				ClipZ_##NAME(j,i) \
				j++; \
			} \
		} \
		else \
			if(vert[i+1]->flags) \
			{ \
				tempvert[j]=*(vert[i]); \
				tempvert[j+1]=*(vert[i+1]); \
				ClipZ_##NAME(j+1,i) \
				j+=2; \
			} \
			else \
			{ \
				tempvert[j]=(*vert[i]); \
				j++; \
			} \
 \
		i++; \
	} \
	return j; \
}

ZClip_template(simple)
ZClip_template(texture)
ZClip_template(color)
ZClip_template(full)

#ifdef USE_FAST_TRIANGLES_PATCH
static void glDrawArrays_triangles(GLint first, GLsizei count)
{
	count+=first;
//glopBegin:
	if (gl_scissor_dirty) {
		pvr_prim(&gl_user_clip, sizeof(pvr_poly_hdr_t));
		gl_scissor_dirty = GL_FALSE;
	}
	if (!gl_pbuf_submitted || gl_poly_cxt.depth.comparison!=gl_depth_func_def || gl_poly_cxt.depth.write!=gl_depth_mask_def) {
		gl_poly_cxt.depth.comparison  = gl_depth_func_def;
		gl_poly_cxt.depth.write  = gl_depth_mask_def;
		send_poly_hdr();
		gl_pbuf_submitted = GL_TRUE;
	}
	if (gl_matrix_dirty) {
		mat_identity();
		glKosMatrixApply(GL_KOS_SCREENVIEW);
		glKosMatrixApply(GL_PROJECTION);
		glKosMatrixApply(GL_MODELVIEW);
		gl_matrix_dirty = GL_FALSE;
	}
	gl_prim_type = GL_TRIANGLES;

	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			int i=first;
			gl_vertex_t v0,v1,v2;
			int  size_c = gl_color_array_size;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFUVAO_array(v0)
				getXYZFUVAO_array(v1)
				getXYZFUVAO_array(v2)
				if (!n)
				{
					sendXYZUVAO(PVR_CMD_VERTEX,v0)
					sendXYZUVAO(PVR_CMD_VERTEX,v1)
					sendXYZUVAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_full(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
		else
		{
			// COLOR 
			int i=0;
			gl_vertex_t v0,v1,v2;
			int  size_v = gl_vertex_array_size;
			int  size_c = gl_color_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = vert_v;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFAO_array(v0)
				getXYZFAO_array(v1)
				getXYZFAO_array(v2)
				if (!n)
				{
					sendXYZAO(PVR_CMD_VERTEX,v0)
					sendXYZAO(PVR_CMD_VERTEX,v1)
					sendXYZAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_color(&v0,&v1,&v2))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			int i=0;
			gl_vertex_t v0,v1,v2;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->argb = v0.argb = v1.argb = v2.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZUVF_array(v0)
				getXYZUVF_array(v1)
				getXYZUVF_array(v2)
				if (!n)
				{
					sendXYZUV(PVR_CMD_VERTEX,v0)
					sendXYZUV(PVR_CMD_VERTEX,v1)
					sendXYZUV(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_texture(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
		else
		{
			// ONLY VERTEX
			int i=0;
			gl_vertex_t v0,v1,v2;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = vert_v;
			vert->argb = v0.argb = v1.argb = v2.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZF_array(v0)
				getXYZF_array(v1)
				getXYZF_array(v2)
				if (!n)
				{
					sendXYZ(PVR_CMD_VERTEX,v0)
					sendXYZ(PVR_CMD_VERTEX,v1)
					sendXYZ(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_simple(&v0,&v1,&v2))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
	}
	gl_prim_type = 0;
}
#endif

#ifdef USE_FAST_QUADS_PATCH
static void glDrawArrays_quads(GLint first, GLsizei count)
{
	count+=first;
//glopBegin:
	if (gl_scissor_dirty) {
		pvr_prim(&gl_user_clip, sizeof(pvr_poly_hdr_t));
		gl_scissor_dirty = GL_FALSE;
	}
	if (!gl_pbuf_submitted || gl_poly_cxt.depth.comparison!=gl_depth_func_def || gl_poly_cxt.depth.write!=gl_depth_mask_def) {
		gl_poly_cxt.depth.comparison  = gl_depth_func_def;
		gl_poly_cxt.depth.write  = gl_depth_mask_def;
		send_poly_hdr();
		gl_pbuf_submitted = GL_TRUE;
	}
	if (gl_matrix_dirty) {
		mat_identity();
		glKosMatrixApply(GL_KOS_SCREENVIEW);
		glKosMatrixApply(GL_PROJECTION);
		glKosMatrixApply(GL_MODELVIEW);
		gl_matrix_dirty = GL_FALSE;
	}
	gl_prim_type = GL_QUADS;

	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			int i=first;
			gl_vertex_t v0,v1,v2,v3;
			int  size_c = gl_color_array_size;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFUVAO_array(v0)
				getXYZFUVAO_array(v1)
				getXYZFUVAO_array(v2)
				getXYZFUVAO_array(v3)
				if (!n)
				{
					sendXYZUVAO(PVR_CMD_VERTEX,v0)
					sendXYZUVAO(PVR_CMD_VERTEX,v1)
					sendXYZUVAO(PVR_CMD_VERTEX,v3)
					sendXYZUVAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_full(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_full(&v0,&v2,&v3))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
		else
		{
			// COLOR 
			int i=0;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			int  size_c = gl_color_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.v = vert_v;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFAO_array(v0)
				getXYZFAO_array(v1)
				getXYZFAO_array(v2)
				getXYZFAO_array(v3)
				if (!n)
				{
					sendXYZAO(PVR_CMD_VERTEX,v0)
					sendXYZAO(PVR_CMD_VERTEX,v1)
					sendXYZAO(PVR_CMD_VERTEX,v3)
					sendXYZAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_color(&v0,&v1,&v2))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_color(&v0,&v2,&v3))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			int i=0;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZUVF_array(v0)
				getXYZUVF_array(v1)
				getXYZUVF_array(v2)
				getXYZUVF_array(v3)
				if (!n)
				{
					sendXYZUV(PVR_CMD_VERTEX,v0)
					sendXYZUV(PVR_CMD_VERTEX,v1)
					sendXYZUV(PVR_CMD_VERTEX,v3)
					sendXYZUV(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_texture(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_texture(&v0,&v2,&v3))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
		else
		{
			// ONLY VERTEX
			int i=0;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.v = vert_v;
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZF_array(v0)
				getXYZF_array(v1)
				getXYZF_array(v2)
				getXYZF_array(v3)
				if (!n)
				{
					sendXYZ(PVR_CMD_VERTEX,v0)
					sendXYZ(PVR_CMD_VERTEX,v1)
					sendXYZ(PVR_CMD_VERTEX,v3)
					sendXYZ(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_simple(&v0,&v1,&v2))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_simple(&v0,&v2,&v3))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	gl_prim_type = 0;
}
#endif

#ifdef USE_FAST_QUAD_STRIP_PATCH
static void glDrawArrays_quad_strip(GLint first, GLsizei count)
{
	count+=first;
//glopBegin:
	if (gl_scissor_dirty) {
		pvr_prim(&gl_user_clip, sizeof(pvr_poly_hdr_t));
		gl_scissor_dirty = GL_FALSE;
	}
	if (!gl_pbuf_submitted || gl_poly_cxt.depth.comparison!=gl_depth_func_def || gl_poly_cxt.depth.write!=gl_depth_mask_def) {
		gl_poly_cxt.depth.comparison  = gl_depth_func_def;
		gl_poly_cxt.depth.write  = gl_depth_mask_def;
		send_poly_hdr();
		gl_pbuf_submitted = GL_TRUE;
	}
	if (gl_matrix_dirty) {
		mat_identity();
		glKosMatrixApply(GL_KOS_SCREENVIEW);
		glKosMatrixApply(GL_PROJECTION);
		glKosMatrixApply(GL_MODELVIEW);
		gl_matrix_dirty = GL_FALSE;
	}
	gl_prim_type = GL_QUADS;

	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			int i=first;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_c = gl_color_array_size;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZFUVAO_array(v2)
				getXYZFUVAO_array(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZFUVAO_array(v2)
				getXYZFUVAO_array(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZUVAO(PVR_CMD_VERTEX,v1)
					sendXYZUVAO(PVR_CMD_VERTEX,v0)
					sendXYZUVAO(PVR_CMD_VERTEX,v3)
					sendXYZUVAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_full(&v1,&v0,&v2))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_full(&v1,&v2,&v3))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
		else
		{
			// COLOR 
			int i=0;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			int  size_c = gl_color_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.v = vert_v;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZFAO_array(v2)
				getXYZFAO_array(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZFAO_array(v2)
				getXYZFAO_array(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZAO(PVR_CMD_VERTEX,v1)
					sendXYZAO(PVR_CMD_VERTEX,v0)
					sendXYZAO(PVR_CMD_VERTEX,v3)
					sendXYZAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_color(&v1,&v0,&v2))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_color(&v1,&v2,&v3))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			int i=0;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZUVF_array(v2)
				getXYZUVF_array(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZUVF_array(v2)
				getXYZUVF_array(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZUV(PVR_CMD_VERTEX,v1)
					sendXYZUV(PVR_CMD_VERTEX,v0)
					sendXYZUV(PVR_CMD_VERTEX,v3)
					sendXYZUV(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_texture(&v1,&v0,&v2))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_texture(&v1,&v2,&v3))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
		else
		{
			// ONLY VERTEX
			int i=0;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.v = vert_v;
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZF_array(v2)
				getXYZF_array(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZF_array(v2)
				getXYZF_array(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZ(PVR_CMD_VERTEX,v1)
					sendXYZ(PVR_CMD_VERTEX,v0)
					sendXYZ(PVR_CMD_VERTEX,v3)
					sendXYZ(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_simple(&v1,&v0,&v2))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_simple(&v1,&v2,&v3))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	gl_prim_type = 0;
}
#endif


#ifdef USE_FAST_TRIANGLES_PATCH
static void glDrawElements_triangles(GLsizei count, const GLint *index )
{
//glopBegin:
	if (gl_scissor_dirty) {
		pvr_prim(&gl_user_clip, sizeof(pvr_poly_hdr_t));
		gl_scissor_dirty = GL_FALSE;
	}
	if (!gl_pbuf_submitted || gl_poly_cxt.depth.comparison!=gl_depth_func_def || gl_poly_cxt.depth.write!=gl_depth_mask_def) {
		gl_poly_cxt.depth.comparison  = gl_depth_func_def;
		gl_poly_cxt.depth.write  = gl_depth_mask_def;
		send_poly_hdr();
		gl_pbuf_submitted = GL_TRUE;
	}
	if (gl_matrix_dirty) {
		mat_identity();
		glKosMatrixApply(GL_KOS_SCREENVIEW);
		glKosMatrixApply(GL_PROJECTION);
		glKosMatrixApply(GL_MODELVIEW);
		gl_matrix_dirty = GL_FALSE;
	}
	gl_prim_type = GL_TRIANGLES;

	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			int i=0;
			gl_vertex_t v0,v1,v2;
			int  size_c = gl_color_array_size;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFUVAO(v0)
				getXYZFUVAO(v1)
				getXYZFUVAO(v2)
				if (!n)
				{
					sendXYZUVAO(PVR_CMD_VERTEX,v0)
					sendXYZUVAO(PVR_CMD_VERTEX,v1)
					sendXYZUVAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_full(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
		else
		{
			// COLOR 
			int i=0;
			gl_vertex_t v0,v1,v2;
			int  size_v = gl_vertex_array_size;
			int  size_c = gl_color_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = vert_v;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFAO(v0)
				getXYZFAO(v1)
				getXYZFAO(v2)
				if (!n)
				{
					sendXYZAO(PVR_CMD_VERTEX,v0)
					sendXYZAO(PVR_CMD_VERTEX,v1)
					sendXYZAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_color(&v0,&v1,&v2))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			int i=0;
			gl_vertex_t v0,v1,v2;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->argb = v0.argb = v1.argb = v2.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZUVF(v0)
				getXYZUVF(v1)
				getXYZUVF(v2)
				if (!n)
				{
					sendXYZUV(PVR_CMD_VERTEX,v0)
					sendXYZUV(PVR_CMD_VERTEX,v1)
					sendXYZUV(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_texture(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
		else
		{
			// ONLY VERTEX
			int i=0;
			gl_vertex_t v0,v1,v2;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = vert_v;
			vert->argb = v0.argb = v1.argb = v2.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZF(v0)
				getXYZF(v1)
				getXYZF(v2)
				if (!n)
				{
					sendXYZ(PVR_CMD_VERTEX,v0)
					sendXYZ(PVR_CMD_VERTEX,v1)
					sendXYZ(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<3)
					{
						switch(ZClip_simple(&v0,&v1,&v2))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
	}
	gl_prim_type = 0;
}
#endif

#ifdef USE_FAST_QUADS_PATCH
static void glDrawElements_quads(GLsizei count, const GLint *index )
{
//glopBegin:
	if (gl_scissor_dirty) {
		pvr_prim(&gl_user_clip, sizeof(pvr_poly_hdr_t));
		gl_scissor_dirty = GL_FALSE;
	}
	if (!gl_pbuf_submitted || gl_poly_cxt.depth.comparison!=gl_depth_func_def || gl_poly_cxt.depth.write!=gl_depth_mask_def) {
		gl_poly_cxt.depth.comparison  = gl_depth_func_def;
		gl_poly_cxt.depth.write  = gl_depth_mask_def;
		send_poly_hdr();
		gl_pbuf_submitted = GL_TRUE;
	}
	if (gl_matrix_dirty) {
		mat_identity();
		glKosMatrixApply(GL_KOS_SCREENVIEW);
		glKosMatrixApply(GL_PROJECTION);
		glKosMatrixApply(GL_MODELVIEW);
		gl_matrix_dirty = GL_FALSE;
	}
	gl_prim_type = GL_QUADS;

	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			int i=0;
			gl_vertex_t v0,v1,v2,v3;
			int  size_c = gl_color_array_size;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFUVAO(v0)
				getXYZFUVAO(v1)
				getXYZFUVAO(v2)
				getXYZFUVAO(v3)
				if (!n)
				{
					sendXYZUVAO(PVR_CMD_VERTEX,v0)
					sendXYZUVAO(PVR_CMD_VERTEX,v1)
					sendXYZUVAO(PVR_CMD_VERTEX,v3)
					sendXYZUVAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_full(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}

						switch(ZClip_full(&v0,&v2,&v3))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
		else
		{
			// COLOR 
			int i=0;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			int  size_c = gl_color_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.u = vert_v;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZFAO(v0)
				getXYZFAO(v1)
				getXYZFAO(v2)
				getXYZFAO(v3)
				if (!n)
				{
					sendXYZAO(PVR_CMD_VERTEX,v0)
					sendXYZAO(PVR_CMD_VERTEX,v1)
					sendXYZAO(PVR_CMD_VERTEX,v3)
					sendXYZAO(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_color(&v0,&v1,&v2))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_color(&v0,&v2,&v3))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			int i=0;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZUVF(v0)
				getXYZUVF(v1)
				getXYZUVF(v2)
				getXYZUVF(v3)
				if (!n)
				{
					sendXYZUV(PVR_CMD_VERTEX,v0)
					sendXYZUV(PVR_CMD_VERTEX,v1)
					sendXYZUV(PVR_CMD_VERTEX,v3)
					sendXYZUV(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_texture(&v0,&v1,&v2))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_texture(&v0,&v2,&v3))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
		else
		{
			// ONLY VERTEX
			int i=0;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.v = vert_v;
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			while(i<count)
			{
				int n=0;
				getXYZF(v0)
				getXYZF(v1)
				getXYZF(v2)
				getXYZF(v3)
				if (!n)
				{
					sendXYZ(PVR_CMD_VERTEX,v0)
					sendXYZ(PVR_CMD_VERTEX,v1)
					sendXYZ(PVR_CMD_VERTEX,v3)
					sendXYZ(PVR_CMD_VERTEX_EOL,v2)
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_simple(&v0,&v1,&v2))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_simple(&v0,&v2,&v3))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	gl_prim_type = 0;
}
#endif

#ifdef USE_FAST_QUAD_STRIP_PATCH
static void glDrawElements_quad_strip(GLsizei count, const GLint *index )
{
//glopBegin:
	if (gl_scissor_dirty) {
		pvr_prim(&gl_user_clip, sizeof(pvr_poly_hdr_t));
		gl_scissor_dirty = GL_FALSE;
	}
	if (!gl_pbuf_submitted || gl_poly_cxt.depth.comparison!=gl_depth_func_def || gl_poly_cxt.depth.write!=gl_depth_mask_def) {
		gl_poly_cxt.depth.comparison  = gl_depth_func_def;
		gl_poly_cxt.depth.write  = gl_depth_mask_def;
		send_poly_hdr();
		gl_pbuf_submitted = GL_TRUE;
	}
	if (gl_matrix_dirty) {
		mat_identity();
		glKosMatrixApply(GL_KOS_SCREENVIEW);
		glKosMatrixApply(GL_PROJECTION);
		glKosMatrixApply(GL_MODELVIEW);
		gl_matrix_dirty = GL_FALSE;
	}
	gl_prim_type = GL_QUADS;

	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			int i=0;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_c = gl_color_array_size;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZFUVAO(v2)
				getXYZFUVAO(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZFUVAO(v2)
				getXYZFUVAO(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZUVAO(PVR_CMD_VERTEX,v0)
					sendXYZUVAO(PVR_CMD_VERTEX,v1)
					sendXYZUVAO(PVR_CMD_VERTEX,v2)
					sendXYZUVAO(PVR_CMD_VERTEX_EOL,v3)
					v2.flags=v3.flags=GEQUAL_Z;
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_full(&v1,&v0,&v2))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}

						switch(ZClip_full(&v1,&v2,&v3))
						{
							case 3:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUVAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUVAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
					}
			}
		}
		else
		{
			// COLOR 
			int i=0;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			int  size_c = gl_color_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.u = vert_v;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZFAO(v2)
				getXYZFAO(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZFAO(v2)
				getXYZFAO(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZAO(PVR_CMD_VERTEX,v0)
					sendXYZAO(PVR_CMD_VERTEX,v1)
					sendXYZAO(PVR_CMD_VERTEX,v2)
					sendXYZAO(PVR_CMD_VERTEX_EOL,v3)
					v2.flags=v3.flags=GEQUAL_Z;
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_color(&v1,&v0,&v2))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_color(&v1,&v2,&v3))
						{
							case 3:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZAO(PVR_CMD_VERTEX,tempvert[0])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[1])
								sendXYZAO(PVR_CMD_VERTEX,tempvert[3])
								sendXYZAO(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			int i=0;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZUVF(v2)
				getXYZUVF(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZUVF(v2)
				getXYZUVF(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZUV(PVR_CMD_VERTEX,v0)
					sendXYZUV(PVR_CMD_VERTEX,v1)
					sendXYZUV(PVR_CMD_VERTEX,v2)
					sendXYZUV(PVR_CMD_VERTEX_EOL,v3)
					v2.flags=v3.flags=GEQUAL_Z;
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_texture(&v1,&v0,&v2))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_texture(&v1,&v2,&v3))
						{
							case 3:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZUV(PVR_CMD_VERTEX,tempvert[0])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[1])
								sendXYZUV(PVR_CMD_VERTEX,tempvert[3])
								sendXYZUV(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
		else
		{
			// ONLY VERTEX
			int i=0;
			int nb;
			gl_vertex_t v0,v1,v2,v3;
			int  size_v = gl_vertex_array_size;
			volatile pvr_vertex_t *vert=pvr_commit_init();
			vert->u = vert_u;
			vert->v = vert_v;
			vert->argb = vert_rgba;
			vert->oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			vert->u = v0.u = v1.u = v2.u = v3.u = vert_u;
			vert->v = v0.v = v1.v = v2.v = v3.v = vert_v;
			vert->argb = v0.argb = v1.argb = v2.argb = v3.argb = vert_rgba;
			vert->oargb = v0.oargb = v1.oargb = v2.oargb = v3.oargb = 0xff000000;
			vert=((volatile pvr_vertex_t *)( ((uint32)vert)^32 ));
			{
				int n=0;
				getXYZF(v2)
				getXYZF(v3)
				nb=n;
			}
			while(i<count)
			{
				int n=0;
				v0=v2;
				v1=v3;
				getXYZF(v2)
				getXYZF(v3)
				{ int ntmp=n+nb; nb=n; n=ntmp; }
				if (!n)
				{
					sendXYZ(PVR_CMD_VERTEX,v0)
					sendXYZ(PVR_CMD_VERTEX,v1)
					sendXYZ(PVR_CMD_VERTEX,v2)
					sendXYZ(PVR_CMD_VERTEX_EOL,v3)
					v2.flags=v3.flags=GEQUAL_Z;
				}
				else
					if (ZCLIP_ENABLED && n<4)
					{
						switch(ZClip_simple(&v1,&v0,&v2))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
						
						switch(ZClip_simple(&v1,&v2,&v3))
						{
							case 3:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
							case 4:
								sendXYZ(PVR_CMD_VERTEX,tempvert[0])
								sendXYZ(PVR_CMD_VERTEX,tempvert[1])
								sendXYZ(PVR_CMD_VERTEX,tempvert[3])
								sendXYZ(PVR_CMD_VERTEX_EOL,tempvert[2])
								break;
						}
					}
			}
		}
	}
	gl_prim_type = 0;
}
#endif
#endif

static void glDrawArrays_other(GLenum mode, GLint first, GLsizei count)
{
	int idx;
	count+=first;
	glBegin(mode);
	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			for(idx=first;idx<count;idx++)
			{
				GLuint rgba;
				{
				int size_c = gl_color_array_size;
				float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*idx));
				float r=*c++;
				float g=*c++;
				float b=*c++;
				float a=size_c > 3 ? *c : 1.0f;
				rgba=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff)) 
				}
				{
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = *t++;
				vtxbuf[vbuf_top].v = *t;

				vtxbuf[vbuf_top].argb = rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;

				vbuf_top++;
				}
			}
		}
		else
		{
			// COLOR 
			for(idx=first;idx<count;idx++)
			{
				GLuint rgba;
				{
				int size_c = gl_color_array_size;
				float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*idx));
				float r=*c++;
				float g=*c++;
				float b=*c++;
				float a=size_c > 3 ? *c : 1.0f;
				rgba=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff)) 
				}
				{
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = vert_u;
				vtxbuf[vbuf_top].v = vert_v;

				vtxbuf[vbuf_top].argb = rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;

				vbuf_top++;
				}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			for(idx=first;idx<count;idx++)
			{
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = *t++;
				vtxbuf[vbuf_top].v = *t;
	
				vtxbuf[vbuf_top].argb = vert_rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;
	
				vbuf_top++;
			}
		}
		else
		{
			// ONLY VERTEX
			for(idx=first;idx<count;idx++)
			{
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = vert_u;
				vtxbuf[vbuf_top].v = vert_v;
	
				vtxbuf[vbuf_top].argb = vert_rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;
		
				vbuf_top++;
			}
		}
	}
	glEnd();
}

static void glDrawElements_other( GLenum mode, GLsizei count, const GLint *index )
{
	int i;
	glBegin(mode);
	if(gl_client_states & COLOR_ARRAY)
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// COLOR + TEXTURE
			for(i=0;i<count;i++)
			{
				int idx=index[i];
				GLuint rgba;
				{
				int size_c = gl_color_array_size;
				float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*idx));
				float r=*c++;
				float g=*c++;
				float b=*c++;
				float a=size_c > 3 ? *c : 1.0f;
				rgba=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff)) 
				}
				{
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = *t++;
				vtxbuf[vbuf_top].v = *t;

				vtxbuf[vbuf_top].argb = rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;

				vbuf_top++;
				}
			}
		}
		else
		{
			// COLOR 
			for(i=0;i<count;i++)
			{
				int idx=index[i];
				GLuint rgba;
				{
				int size_c = gl_color_array_size;
				float *c=(float *)(((unsigned)gl_color_array)+(gl_color_array_stride*idx));
				float r=*c++;
				float g=*c++;
				float b=*c++;
				float a=size_c > 3 ? *c : 1.0f;
				rgba=SET_COLOR_VALUE((int)(r*0xff),(int)(g*0xff),(int)(b*0xff),(int)(a*0xff)) 
				}
				{
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = vert_u;
				vtxbuf[vbuf_top].v = vert_v;

				vtxbuf[vbuf_top].argb = rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;

				vbuf_top++;
				}
			}
		}
	}
	else
	{
		if(gl_client_states & TEXCOORD_ARRAY)
		{
			// TEXTURE
			for(i=0;i<count;i++)
			{
				int idx=index[i];
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				float *t=(float *)(((unsigned)gl_texcoord_array)+(gl_texcoord_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = *t++;
				vtxbuf[vbuf_top].v = *t;
	
				vtxbuf[vbuf_top].argb = vert_rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;
	
				vbuf_top++;
			}
		}
		else
		{
			// ONLY VERTEX
			for(i=0;i<count;i++)
			{
				int idx=index[i];
				int  size_v = gl_vertex_array_size;
				float *v=(float *)(((unsigned)gl_vertex_array)+(gl_vertex_array_stride*idx));
				vtxbuf[vbuf_top].flags = PVR_CMD_VERTEX;
				vtxbuf[vbuf_top].x = *v++;
				vtxbuf[vbuf_top].y = *v++;
				vtxbuf[vbuf_top].z =  size_v > 2 ? *v++ : 0.0f;
				vtxbuf[vbuf_top].u = vert_u;
				vtxbuf[vbuf_top].v = vert_v;
	
				vtxbuf[vbuf_top].argb = vert_rgba;
				vtxbuf[vbuf_top].oargb = 0xff000000;
		
				vbuf_top++;
			}
		}
	}
	glEnd();
}

void glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	if (indices && count && (gl_client_states & VERTEX_ARRAY))
	{
		if (kos_compile_flag)
			glDrawElements_old(mode,count,indices);
		else
#ifdef USE_FAST_TRIANGLES_PATCH
		if (mode==GL_TRIANGLES)
			glDrawElements_triangles(count,indices);
		else
#endif
#ifdef USE_FAST_QUADS_PATCH
		if (mode==GL_QUADS)
			glDrawElements_quads(count,indices);
		else
#endif
#ifdef USE_FAST_QUAD_STRIP_PATCH
		if (mode==GL_QUAD_STRIP)
			glDrawElements_quad_strip(count,indices);
		else
#endif
			glDrawElements_other(mode,count,indices);
	}
}

void glDrawArrays( GLenum mode, GLint first, GLsizei count )
{
	if (count && (gl_client_states & VERTEX_ARRAY))
	{
		if (kos_compile_flag)
			glDrawArrays_old(mode,first,count);
		else
#ifdef USE_FAST_TRIANGLES_PATCH
		if (mode==GL_TRIANGLES)
			glDrawArrays_triangles(first,count);
		else
#endif
#ifdef USE_FAST_QUADS_PATCH
		if (mode==GL_QUADS)
			glDrawArrays_quads(first,count);
		else
#endif
#ifdef USE_FAST_QUAD_STRIP_PATCH
		if (mode==GL_QUAD_STRIP)
			glDrawArrays_quad_strip(first,count);
		else
#endif
			glDrawArrays_other(mode,first,count);
	}
}

/* */
void  glEnableClientState(GLenum array) {
    GLParam p[2];
    p[0].op = OP_EnableClientState;

    switch(array) {
    case GL_VERTEX_ARRAY:			p[1].i = VERTEX_ARRAY; break;
    case GL_NORMAL_ARRAY:			p[1].i = NORMAL_ARRAY; break;
    case GL_COLOR_ARRAY:			p[1].i = COLOR_ARRAY; break;
    case GL_TEXTURE_COORD_ARRAY:	p[1].i = TEXCOORD_ARRAY; break;
    default:						assert(0); break;
    }

    gl_add_op(p);
}


/* */
void  glDisableClientState(GLenum array) {
    GLParam p[2];
    p[0].op = OP_DisableClientState;

    switch(array) {
    case GL_VERTEX_ARRAY:			p[1].i = ~VERTEX_ARRAY; break;
    case GL_NORMAL_ARRAY:			p[1].i = ~NORMAL_ARRAY; break;
    case GL_COLOR_ARRAY:			p[1].i = ~COLOR_ARRAY; break;
    case GL_TEXTURE_COORD_ARRAY:	p[1].i = ~TEXCOORD_ARRAY; break;
    default:						assert(0); break;
    }

    gl_add_op(p);
}


/* */
void  glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
 if (pointer)
 {
    GLParam p[4];
    assert(type == GL_FLOAT);
    p[0].op = OP_VertexPointer;
    p[1].i = size;
    p[2].i = stride;
    p[3].p = (void *) pointer;
    gl_add_op(p);
 }
 else
	 glDisableClientState(GL_VERTEX_ARRAY);
}


/* */
void  glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
 if (pointer)
 {
    GLParam p[4];
    assert(type == GL_FLOAT);
    p[0].op = OP_ColorPointer;
    p[1].i = size;
    p[2].i = stride;
    p[3].p = (void *) pointer;
    gl_add_op(p);
 }
 else
	 glDisableClientState(GL_COLOR_ARRAY);
}


/* FIXME:unused */
void  glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) {
 if (pointer)
 {
    GLParam p[3];
    assert(type == GL_FLOAT);
    p[0].op = OP_NormalPointer;
    p[1].i = stride;
    p[2].p = (void *) pointer;
    //gl_add_op(p);
 }
 else
	 glDisableClientState(GL_NORMAL_ARRAY);
}


/* */
void  glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
 if (pointer)
 {
    GLParam p[4];
    assert(type == GL_FLOAT);
    assert(size>0 && size<=2);

    p[0].op = OP_TexCoordPointer;
    p[1].i = size;
    p[2].i = stride;
    p[3].p = (void *) pointer;
    gl_add_op(p);
 }
 else
	 glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

