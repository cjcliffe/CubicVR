// File:  ph.h
// Brief: Declares "Philip's GL utility API", or "ph" for short.

#pragma once

#include "CubicVR/cvr_defines.h"
#include "CubicVR/GLExt.h"
#include "math.h"


#define PH_PI (3.14159265358979323846f)

struct PHviewportRec
{
    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
};

struct PHsurfaceRec
{
    GLsizei width;
    GLsizei height;
    struct PHviewportRec viewport;
    GLfloat clearColor[4];
    GLfloat modelview[16];
    GLfloat projection[16];
    GLuint texture;
    GLuint attachments[4];
	int numAttachments;
    GLuint depth;
    GLuint fbo;
};

struct PHvec3Rec
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

typedef struct PHviewportRec PHviewport;
typedef struct PHsurfaceRec PHsurface;
typedef struct PHvec3Rec PHvec3;

void phCreateSurface(PHsurface *, GLboolean depth, int fp, GLboolean linear, GLboolean stencil = false, int attachments=1);
void phDestroySurface(PHsurface *surface);
void phBindSurface(const PHsurface *);
void phClearSurface();
void phCheckFBO();
void phCheckError(const char *);
GLuint phCompile(const char *vert, const char *frag);
void phNormalize(PHvec3 *);
PHvec3 phAdd(const PHvec3 *, const PHvec3 *);
PHvec3 phSub(const PHvec3 *, const PHvec3 *);
