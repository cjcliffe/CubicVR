/*
   KGL-x 0.4

   gltex.c
   (c) 2002-2003 heinrich tillack

   derived version from

   KallistiGL for KOS 1.1.7

   gltex.c
   (c)2001 Dan Potter
   (c)2002 BERO


*/



#include <GL/gl.h>
#include <GL/glu.h>
#include "glinternal.h"
#include "glmacros.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>

GLboolean gl_use_4444=GL_FALSE;

/* Texture handling */

/* Allocate "texture structures" (really poly context structs) */
void glGenTextures(GLsizei n, GLuint *textures) {
    int i;
    pvr_poly_cxt_t *p;
    if (textures)
    for (i=0; i<n; i++) {
        p = (pvr_poly_cxt_t*)malloc(sizeof(pvr_poly_cxt_t));

        /* initial state for this texture object */
//        p->txr.env = PVR_TXRENV_MODULATEALPHA; //PVR_TXRENV_MODULATE;
p->txr.env = PVR_TXRENV_MODULATE;
        p->txr.enable = PVR_TEXTURE_ENABLE;
        p->txr.filter = PVR_FILTER_NONE;
        p->txr.alpha = PVR_TXRALPHA_DISABLE;
        p->txr.mipmap = PVR_MIPMAP_DISABLE;
        p->txr.mipmap_bias = PVR_MIPBIAS_NORMAL;
        p->txr.uv_flip = PVR_UVFLIP_NONE;
        p->txr.uv_clamp = PVR_UVCLAMP_NONE;
        p->txr.format = PVR_TXRFMT_NONE;
        p->txr.base = NULL;
        p->txr.width = -1;
        p->txr.height = -1;

        textures[i] = (GLuint)p;
    }
}

/*  Delete textures from VRAM */
void glDeleteTextures(GLsizei n, const GLuint *textures) {
    int i;

    if (textures)
    for (i=0; i<n; i++) {
	if (((pvr_poly_cxt_t *)textures[i])->txr.base)
        	pvr_mem_free(((pvr_poly_cxt_t *)textures[i])->txr.base);
	if (textures[i])
        	free((void*)textures[i]);
    }
}

#ifndef KGL_FEATURE_DISPLAYLISTS

/* Bind a texture */
void glBindTexture(GLenum type, GLuint texture) {
    type&=0x0F; type|=1;
    assert(type == GL_TEXTURE_2D);

    if (texture)
    {
    	gl_cur_texture = (pvr_poly_cxt_t *)texture;
    	gl_pbuf_submitted = GL_FALSE;
    }
}
#else
/* Bind a texture */
void glopBindTexture(GLParam * p) {

    p[1].i&=0x0F; p[1].i|=1;
    assert(p[1].i == GL_TEXTURE_2D);

    if (p[2].ui)
    {
    	gl_cur_texture = (pvr_poly_cxt_t *)p[2].ui;
    	gl_pbuf_submitted = GL_FALSE;
    }
}
#endif

typedef struct { unsigned char r,g,b; } internal_rgb_t;
typedef struct { unsigned char r,g,b,a; } internal_rgba_t;

static void pixconvRGB(unsigned short *dst, unsigned dst_w, unsigned dst_h, internal_rgb_t *src, unsigned src_w, unsigned src_h)
{
	if (src_w!=dst_w || src_h!=dst_h)
	{
		int x,y;
		float dx, dy, oox, ooy;
		dx=((float)src_w)/((float)dst_w);
		dy=((float)src_h)/((float)dst_h);
		for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy)
		{
			register int py=((int)ooy)*src_w;
			for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
			{
				register int px=py+((int)oox);
				*dst++= ((((unsigned)src[px].r)>>3)<<11) |
					((((unsigned)src[px].g)>>2)<<5) |
					((((unsigned)src[px].b)>>3)/*<<0*/);
			}
		}
	}
	else
	{
		register unsigned i;
		register unsigned n=src_w*src_h;
		register unsigned char *s=(unsigned char *)src;
		for(i=0;i<n;i++,s+=3)
			*dst++= ((((unsigned)(s[0]))>>3)<<11) |
				((((unsigned)(s[1]))>>2)<<5) |
				((((unsigned)(s[2]))>>3)/*<<0*/);
	}

}

static void pixconvRGBA(unsigned short *dst, unsigned dst_w, unsigned dst_h, internal_rgba_t *src, unsigned src_w, unsigned src_h)
{
	if (src_w!=dst_w || src_h!=dst_h)
	{
		int x,y;
		float dx, dy, oox, ooy;
		dx=((float)src_w)/((float)dst_w);
		dy=((float)src_h)/((float)dst_h);
		for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy)
		{
			register int py=((int)ooy)*src_w;
			for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
			{
				register int px=py+((int)oox);
				*dst++= ((((unsigned)src[px].a)/*&1*/)<<15) |
					((((unsigned)src[px].r)>>3)<<10) |
					((((unsigned)src[px].g)>>3)<<5) |
					((((unsigned)src[px].b)>>3)/*<<0*/);
			}
		}
	}
	else
	{
		register unsigned i;
		register unsigned n=src_w*src_h;
		register unsigned char *s=(unsigned char *)src;
		for(i=0;i<n;i++,s+=4)
			*dst++= ((((unsigned)s[0])>>3)<<10) |
				((((unsigned)s[1])>>3)<<5) |
				((((unsigned)s[2])>>3)) |
				((((unsigned)s[3]))<<15);
	}
}

static void pixconvRGBA4444(unsigned short *dst, unsigned dst_w, unsigned dst_h, internal_rgba_t *src, unsigned src_w, unsigned src_h)
{
	if (src_w!=dst_w || src_h!=dst_h)
	{
		int x,y;
		float dx, dy, oox, ooy;
		dx=((float)src_w)/((float)dst_w);
		dy=((float)src_h)/((float)dst_h);
		for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy)
		{
			register int py=((int)ooy)*src_w;
			for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
			{
				register int px=py+((int)oox);
				*dst++= ((((unsigned)src[px].a)>>4)<<12) |
					((((unsigned)src[px].r)>>4)<<8) |
					((((unsigned)src[px].g)>>4)<<4) |
					((((unsigned)src[px].b)>>4)/*<<0*/);
			}
		}
	}
	else
	{
		register unsigned i;
		register unsigned n=src_w*src_h;
		register unsigned char *s=(unsigned char *)src;
		for(i=0;i<n;i++,s+=4)
			*dst++= ((((unsigned)s[0])>>4)<<8) |
				((((unsigned)s[1])>>4)<<4) |
				((((unsigned)s[2])>>4)) |
				((((unsigned)s[3])>>4)<<12);
	}
}


static void pixconv16(unsigned short *dst, unsigned dst_w, unsigned dst_h, unsigned short *src, unsigned src_w, unsigned src_h)
{
	int x,y;
	float dx, dy, oox, ooy;
	dx=((float)src_w)/((float)dst_w);
	dy=((float)src_h)/((float)dst_h);
	for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy)
	{
		register int py=((int)ooy)*src_w;
		for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
		{
			register int px=py+((int)oox);
			*dst++=src[px];
		}
	}
}

/* Load a texture from a 2D image */
void glTexImage2D(GLenum target, GLint level,
                  GLint internal_fmt,
                  GLsizei width, GLsizei height,
                  GLint border, GLenum format, GLenum type,
                  const GLvoid *pixels) {
    unsigned short *pix16=(unsigned short *)pixels;
    pvr_ptr_t txr;
    int w=width;
    int h=height;
    int i;

    if (level) return;
    assert_msg(border == 0, "Borders not supported.");
//    assert_msg(border == 0 && level == 0, "Borders and levels not supported.");
    if (format!=GL_RGB && format!=GL_RGBA)
    	assert_msg((internal_fmt & ~1) == (format & ~1), "Pixel conversion not supported.");

    for(i=8;i<1024;i*=2)
	    if (width<=i) { w=i; break; }
    for(i=8;i<1024;i*=2)
	    if (height<=i) { h=i; break; }

    /* Allocate space for it */
    txr = pvr_mem_malloc(w * h * 2);

    /* Load the texture data */
    switch(format)
    {
	case GL_RGB:
		pixconvRGB(txr,w,h,(internal_rgb_t *)pixels,width,height);
		internal_fmt=GL_RGB565;
		break;
	case GL_RGBA:
		if (!gl_use_4444)
		{
			pixconvRGBA(txr,w,h,(internal_rgba_t *)pixels,width,height);
			internal_fmt=GL_ARGB1555;
		}
		else
		{
			pixconvRGBA4444(txr,w,h,(internal_rgba_t *)pixels,width,height);
			internal_fmt=GL_ARGB4444;
		}
		break;
	default:
		if (h!=height || w!=width)
		{
			pix16=malloc(h*w*2);
			pixconv16(pix16,w,h,(unsigned short *)pixels,width,height);
		}
    		if ((internal_fmt & 1) != (format & 1))
        		pvr_txr_load_ex((GLvoid *)pix16, txr, w, h, PVR_TXRLOAD_16BPP);
   	 	else
        		pvr_txr_load((GLvoid *)pix16, txr, w * h * 2);
    		if (pix16!=(unsigned short *)pixels)
			free(pix16);
    }


    /* Store texture state in context */
    gl_cur_texture->txr.mipmap=(width<<16)|((height<<1)&0xFFFE);
    gl_cur_texture->txr.width = w;
    gl_cur_texture->txr.height =  h;
    gl_cur_texture->txr.format = internal_fmt;
    gl_cur_texture->txr.base = txr;
    gl_pbuf_submitted = GL_FALSE;
}

void glTexImage1D(GLenum target, GLint level,
                  GLint internal_fmt,
                  GLsizei width,
                  GLint border, GLenum format, GLenum type,
                  const GLvoid *pixels) {

    glTexImage2D(target, level, internal_fmt, width, 1, border, format, type, pixels);
}
/* KOS shortcut function (if you loaded your own texture) */
void glKosTex2D(GLint internal_fmt, GLsizei width, GLsizei height,
                pvr_ptr_t txr_address) {

    /* Store texture state in context */
    gl_cur_texture->txr.width = width;
    gl_cur_texture->txr.height =  height;
    gl_cur_texture->txr.format =  internal_fmt;
    gl_cur_texture->txr.base = txr_address;

    gl_pbuf_submitted = GL_FALSE;
}

static void subpixconvRGB(unsigned offs_x, unsigned offs_y, internal_rgb_t *src, unsigned src_w, unsigned src_h, unsigned w, unsigned h)
{
	unsigned short *dst=gl_cur_texture->txr.base;
	unsigned dst_w=gl_cur_texture->txr.width;
	unsigned dst_h=gl_cur_texture->txr.height;

	if (w!=dst_w || h!=dst_h)
	{
		int x,y,pad;
		float dx, dy, oox, ooy;
		dx=((float)w)/((float)dst_w);
		dy=((float)h)/((float)dst_h);
		dst+=((offs_x*dst_w)/w)+(((offs_y*dst_h)/h)*dst_w);
		pad=dst_w;
		dst_w=(src_w*dst_w)/w;
		dst_h=(src_h*dst_h)/h;
		pad-=dst_w;
		for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy,dst+=pad)
		{
			register int py=((int)ooy)*src_w;
			for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
			{
				register int px=py+((int)oox);
				*dst++= ((((unsigned)src[px].r)>>3)<<11) |
					((((unsigned)src[px].g)>>2)<<5) |
					((((unsigned)src[px].b)>>3)/*<<0*/);
			}
		}
	}
	else
	{
		register unsigned y;
		register unsigned char *s=(unsigned char *)src;
		dst+=offs_x+(offs_y*dst_w);
		dst_w-=src_w;
		for(y=0;y<src_h;y++,dst+=dst_w)
		{
			register unsigned x;
			for(x=0;x<src_w;x++,s+=3)
				*dst++= ((((unsigned)(s[0]))>>3)<<11) |
					((((unsigned)(s[1]))>>2)<<5) | 
					((((unsigned)(s[2]))>>3)/*<<0*/); 
		}
	}
}

static void subpixconvRGBA(unsigned offs_x, unsigned offs_y, internal_rgba_t *src, unsigned src_w, unsigned src_h, unsigned w, unsigned h)
{
	unsigned short *dst=gl_cur_texture->txr.base;
	unsigned dst_w=gl_cur_texture->txr.width;
	unsigned dst_h=gl_cur_texture->txr.height;

	if (w!=dst_w || h!=dst_h)
	{
		int x,y,pad;
		float dx, dy, oox, ooy;
		dx=((float)w)/((float)dst_w);
		dy=((float)h)/((float)dst_h);
		dst+=((offs_x*dst_w)/w)+(((offs_y*dst_h)/h)*dst_w);
		pad=dst_w;
		dst_w=(src_w*dst_w)/w;
		dst_h=(src_h*dst_h)/h;
		pad-=dst_w;
		for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy,dst+=pad)
		{
			register int py=((int)ooy)*src_w;
			for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
			{
				register int px=py+((int)oox);
				*dst++= ((((unsigned)src[px].a)/*&1*/)<<15) |
					((((unsigned)src[px].r)>>3)<<10) |
					((((unsigned)src[px].g)>>3)<<5) |
					((((unsigned)src[px].b)>>3)/*<<0*/);
			}
		}
	}
	else
	{
		register unsigned y;
		register unsigned char *s=(unsigned char *)src;
		dst+=offs_x+(offs_y*dst_w);
		dst_w-=src_w;
		for(y=0;y<src_h;y++,dst+=dst_w)
		{
			register unsigned x;
			for(x=0;x<src_w;x++,s+=4)
				*dst++= ((((unsigned)s[0])>>3)<<10) |
					((((unsigned)s[1])>>3)<<5) |
					((((unsigned)s[2])>>3)/*<<0*/) |
					((((unsigned)s[3])/*&1*/)<<15);
		}
	}
}

static void subpixconvRGBA4444(unsigned offs_x, unsigned offs_y, internal_rgba_t *src, unsigned src_w, unsigned src_h, unsigned w, unsigned h)
{
	unsigned short *dst=gl_cur_texture->txr.base;
	unsigned dst_w=gl_cur_texture->txr.width;
	unsigned dst_h=gl_cur_texture->txr.height;

	if (w!=dst_w || h!=dst_h)
	{
		int x,y,pad;
		float dx, dy, oox, ooy;
		dx=((float)w)/((float)dst_w);
		dy=((float)h)/((float)dst_h);
		dst+=((offs_x*dst_w)/w)+(((offs_y*dst_h)/h)*dst_w);
		pad=dst_w;
		dst_w=(src_w*dst_w)/w;
		dst_h=(src_h*dst_h)/h;
		pad-=dst_w;
		for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy,dst+=pad)
		{
			register int py=((int)ooy)*src_w;
			for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
			{
				register int px=py+((int)oox);
				*dst++= ((((unsigned)src[px].a)>>4)<<12) |
					((((unsigned)src[px].r)>>4)<<8) |
					((((unsigned)src[px].g)>>4)<<4) |
					((((unsigned)src[px].b)>>4)/*<<0*/);
			}
		}
	}
	else
	{
		register unsigned y;
		register unsigned char *s=(unsigned char *)src;
		dst+=offs_x+(offs_y*dst_w);
		dst_w-=src_w;
		for(y=0;y<src_h;y++,dst+=dst_w)
		{
			register unsigned x;
			for(x=0;x<src_w;x++,s+=4)
				*dst++= ((((unsigned)s[0])>>4)<<8) |
					((((unsigned)s[1])>>4)<<4) |
					((((unsigned)s[2])>>4)/*<<0*/) |
					((((unsigned)s[3])>>4)<<12);
		}
	}
}

static void subpixconv16(unsigned offs_x, unsigned offs_y, unsigned short *src, unsigned src_w, unsigned src_h, unsigned w, unsigned h)
{
	unsigned short *dst=gl_cur_texture->txr.base;
	unsigned dst_w=gl_cur_texture->txr.width;
	unsigned dst_h=gl_cur_texture->txr.height;
printf("subpixconv16:\n offs_x=%i, offs_y=%i\n src_w=%i, src_h=%i\n dst_w=%i, dst_h=%i\n w=%i, h=%i\n",offs_x,offs_y,src_w,src_h,dst_w,dst_h,w,h);

	if (w!=dst_w || h!=dst_h)
	{
		int x,y,pad;
		float dx, dy, oox, ooy;
		dx=((float)w)/((float)dst_w);
		dy=((float)h)/((float)dst_h);
		dst+=((offs_x*dst_w)/w)+(((offs_y*dst_h)/h)*dst_w);
		pad=dst_w;
		dst_w=(src_w*dst_w)/w;
		dst_h=(src_h*dst_h)/h;
		pad-=dst_w;
		for(y=0,ooy=0.0;y<dst_h;y++,ooy+=dy,dst+=pad)
		{
			register int py=((int)ooy)*src_w;
			for(x=0,oox=0.0;x<dst_w;x++,oox+=dx)
			{
				register int px=py+((int)oox);
				*dst++=src[px];
			}
		}
	}
	else
	{
		register unsigned y;
		register unsigned short *s=(unsigned short *)src;
		dst+=offs_x+(offs_y*dst_w);
		dst_w-=src_w;
		for(y=0;y<src_h;y++,dst+=dst_w)
		{
			register unsigned x;
			for(x=0;x<src_w;x++)
				*dst++=*s++;
		}
	}
}

void glTexSubImage2D( GLenum target, GLint level,
		      GLint xoffset, GLint yoffset, 
		      GLsizei width, GLsizei height,
		      GLenum format, GLenum type,
		      const GLvoid *pixels ){
    unsigned w=gl_cur_texture->txr.mipmap>>16;
    unsigned h=(gl_cur_texture->txr.mipmap>>1)&0x7FFF;
    if (!width || !height || !gl_cur_texture->txr.base)
	    return;
    if (!w)
	    w=gl_cur_texture->txr.width;
    if (!h)
	    h=gl_cur_texture->txr.height;

    if (level) return;
//    assert_msg(level == 0, "Levels not supported.");
    switch(format)
    {
	case GL_RGB:
		subpixconvRGB(xoffset,yoffset,(internal_rgb_t *)pixels,width,height,w,h);
		break;
	case GL_RGBA:
		if (!gl_use_4444)
			subpixconvRGBA(xoffset,yoffset,(internal_rgba_t *)pixels,width,height,w,h);
		else
			subpixconvRGBA4444(xoffset,yoffset,(internal_rgba_t *)pixels,width,height,w,h);
		break;
	default:
		subpixconv16(xoffset,yoffset,(unsigned short *)pixels,width,height,w,h);
    }
}

void glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels ){
	glTexSubImage2D(GL_TEXTURE_2D,level,xoffset,0,width,1,format,type,pixels);
}

/* Texture environment */
void glTexEnvi(GLenum target, GLenum pname, GLint param) {
    target&=0x0F; target|=1;
    assert(target == GL_TEXTURE_2D);
//    assert(pname == GL_TEXTURE_ENV_MODE);

    switch(param) {
    case GL_REPLACE:
        gl_cur_texture->txr.env = PVR_TXRENV_REPLACE;
        break;
    case GL_MODULATE:
        gl_cur_texture->txr.env = PVR_TXRENV_MODULATE;
        break;
    case GL_DECAL:
        gl_cur_texture->txr.env = PVR_TXRENV_DECAL;
        break;
    case GL_MODULATEALPHA:
        gl_cur_texture->txr.env = PVR_TXRENV_MODULATEALPHA;
        break;
    }
    gl_pbuf_submitted = GL_FALSE;
}

/* Texture parameter */
void glGetTexParameteriv(GLenum target, GLenum pname, GLint *params )
{
    GLint ret=0;
    target&=0x0F; target|=1;
    if (target == GL_TEXTURE_2D)
    switch(pname) {
    case GL_TEXTURE_MAG_FILTER:
//    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_FILTER:
	if (gl_cur_texture->txr.filter == PVR_FILTER_NONE)
		ret = GL_NEAREST;
	else
	    	ret = GL_LINEAR;
        break;
    case GL_TEXTURE_WRAP_S: /* adjust state of UVCLAMP_U */
	if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_V || gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_NONE)
		ret = GL_REPEAT;
	else
		ret = GL_CLAMP;
        break;
    case GL_TEXTURE_WRAP_T: /* adjust state of UVCLAMP_V */
	if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_U || gl_cur_texture->txr.uv_clamp ==PVR_UVCLAMP_NONE)
		ret = GL_REPEAT;
	else
		ret = GL_CLAMP;
    default:
/*
printf("\n\n\t --> PNAME=%X\n <--\n\n",pname);
        assert_msg(0, "Unknown parameter name (pname).");
*/
        break;

    }
    *params = ret;
}

void glTexParameteri(GLenum target, GLenum pname, GLint param) {
    target&=0x0F; target|=1;
    assert(target == GL_TEXTURE_2D);

    switch(pname) {
    case GL_TEXTURE_MAG_FILTER:
//    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_FILTER:
        switch(param) {
	case GL_NEAREST:
        case GL_FILTER_NONE:
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
            gl_cur_texture->txr.filter = PVR_FILTER_NONE;
            break;
	case GL_LINEAR_MIPMAP_LINEAR:
	case GL_LINEAR:
        case GL_FILTER_BILINEAR:
            gl_cur_texture->txr.filter = PVR_FILTER_BILINEAR;
            break;
        default:
            assert_msg(0, "Unknown texture filter.");
            break;
        }
        break;
    case GL_TEXTURE_WRAP_S: /* adjust state of UVCLAMP_U */
        switch(param) {
        case GL_REPEAT:
            if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_UV)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_V;
            else if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_U)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_NONE;
            break;
        case GL_CLAMP:
            if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_NONE)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_U;
            else if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_V)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_UV;
            break;
        default:
            assert_msg(0, "Unknown texture wrap mode.");
            break;
        }
        break;
    case GL_TEXTURE_WRAP_T: /* adjust state of UVCLAMP_V */
        switch(param) {
        case GL_REPEAT:
            if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_UV)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_U;
            else if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_V)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_NONE;
            break;
        case GL_CLAMP:
            if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_NONE)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_V;
            else if (gl_cur_texture->txr.uv_clamp == PVR_UVCLAMP_U)
                gl_cur_texture->txr.uv_clamp = PVR_UVCLAMP_UV;
            break;
        default:
            assert_msg(0, "Unknown texture wrap mode.");
            break;
        }
        break;
    default:
/*
printf("\n\n\t --> PNAME=%X\n <--\n\n",pname);
        assert_msg(0, "Unknown parameter name (pname).");
*/
        break;

    }
    gl_pbuf_submitted = GL_FALSE;
}

