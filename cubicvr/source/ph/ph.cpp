// File:  ph.c
// Brief: Implements "Philip's GL utility API", or "ph" for short.
#if !defined(ARCH_PSP)
#include <CubicVR/ph/ph.h>
#include <CubicVR/Logger.h>

const PHsurface *boundSurface;


void phDestroySurface(PHsurface *surface)
{
    glDeleteTextures(1, &surface->texture);
	if (surface->depth) glDeleteRenderbuffersEXT(1, &surface->depth);
	glDeleteFramebuffersEXT(1, &surface->fbo);
	
	surface->texture = 0;
	surface->depth = 0;
	surface->fbo = 0;
}

void phCreateSurface(PHsurface *surface, GLboolean depth, int fp, GLboolean linear, GLboolean stencil, int attachments)
{
    GLenum internalFormat = 0;
	if (fp==0) internalFormat = GL_RGBA;
	if (fp==1) internalFormat = GL_RGBA32F_ARB;
	if (fp==2) internalFormat = GL_RGBA16F_ARB;
    GLenum type = fp ? GL_HALF_FLOAT_ARB : GL_UNSIGNED_BYTE;
    GLenum filter = linear ? GL_LINEAR : GL_NEAREST;
	int i;
    
    // create a color texture
    glGenTextures(1, &surface->texture);
    glBindTexture(GL_TEXTURE_2D, surface->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->width, surface->height, 0, GL_RGB, type, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	static const GLfloat borderColor[4] = {0, 0, 0, 0}; 
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
	for (i = 0; i < attachments-1; i++)
	{
		glGenTextures(1, &surface->attachments[i]);
		glBindTexture(GL_TEXTURE_2D, surface->attachments[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->width, surface->height, 0, GL_RGB, type, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
    phCheckError("Creation of the color texture for the FBO");
	
    // create depth renderbuffer
    if (depth)
    {		
        glGenRenderbuffersEXT(1, &surface->depth);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, surface->depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, surface->width, surface->height);
        phCheckError("Creation of the depth renderbuffer for the FBO");
    }
    else
    {
        surface->depth = 0;
    }

    // create FBO itself
    glGenFramebuffersEXT(1, &surface->fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, surface->fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, surface->texture, 0);
    if (depth)
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, surface->depth);

	if (attachments > 1)
	{
		for (i = 0; i < attachments-1; i++)
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT+i, GL_TEXTURE_2D, surface->attachments[i], 0);			
		}
	}
	
	surface->numAttachments = attachments;
	
	phCheckFBO();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    phCheckError("Creation of the FBO itself");
}

void phCheckFBO()
{
    char enums[][20] =
    {
        "attachment",         // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT........... All framebuffer attachment points are 'framebuffer attachment complete'.
        "missing attachment", // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT....There is at least one image attached to the framebuffer.
        "",                   //
        "dimensions",         // GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT............All attached images have the same width and height.
        "formats",            // GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT...............All images attached to the attachment points COLOR_ATTACHMENT0_EXT through COLOR_ATTACHMENTn_EXT must have the same internal format.
        "draw buffer",        // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT...........The value of FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT must not be NONE for any color attachment point(s) named by DRAW_BUFFERi.
        "read buffer",        // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT...........If READ_BUFFER is not NONE, then the value of FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT must not be NONE for the color attachment point named by READ_BUFFER.
        "unsupported format"  // GL_FRAMEBUFFER_UNSUPPORTED_EXT......................The combination of internal formats of the attached images does not violate an implementation-dependent set of restrictions.
    };

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status == GL_FRAMEBUFFER_COMPLETE_EXT)
        return;

    status -= GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT;
    Logger::log(LOG_ERROR,"incomplete framebuffer object due to %s", enums[status]);
}

void phCheckError(const char *call)
{
    char enums[][20] =
    {
        "invalid enumeration", // GL_INVALID_ENUM
        "invalid value",       // GL_INVALID_VALUE
        "invalid operation",   // GL_INVALID_OPERATION
        "stack overflow",      // GL_STACK_OVERFLOW
        "stack underflow",     // GL_STACK_UNDERFLOW
        "out of memory"        // GL_OUT_OF_MEMORY
    };

    GLenum errcode = glGetError();
    if (errcode == GL_NO_ERROR)
        return;

    errcode -= GL_INVALID_ENUM;
    Logger::log(LOG_ERROR,"OpenGL %s in '%s'", enums[errcode], call);
}

void phBindSurface(const PHsurface *surface)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, surface->fbo);
    glViewport(surface->viewport.x, surface->viewport.y, surface->viewport.width, surface->viewport.height);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(surface->projection);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(surface->modelview);
    boundSurface = surface;
}

void phClearSurface()
{
    const PHsurface *surface = boundSurface;
    glClearColor(surface->clearColor[0], surface->clearColor[1], surface->clearColor[2], surface->clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | (surface->depth ? GL_DEPTH_BUFFER_BIT : 0));
}

GLuint phCompile(const char *vert, const char *frag)
{
    GLchar buf[256];
    GLuint vertShader, fragShader, program;
    GLint success;

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, (const GLchar**) &vert, 0);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertShader, sizeof(buf), 0, buf);
        Logger::log(LOG_ERROR,buf);
        Logger::log(LOG_ERROR,"Unable to compile vertex shader.\n");
    }

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, (const GLchar**) &frag, 0);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragShader, sizeof(buf), 0, buf);
        Logger::log(LOG_ERROR,buf);
        Logger::log(LOG_ERROR,"Unable to compile fragment shader.\n");
    }

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, sizeof(buf), 0, buf);
        Logger::log(LOG_ERROR,buf);
        Logger::log(LOG_ERROR,"Unable to link shaders.\n");
    }

    return program;
}

void phNormalize(PHvec3 *v)
{
    GLfloat mag = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (mag)
    {
        v->x /= mag;
        v->y /= mag;
        v->z /= mag;
    }
}

PHvec3 phAdd(const PHvec3 *a, const PHvec3 *b)
{
    PHvec3 c;
    c.x = a->x + b->x;
    c.y = a->y + b->y;
    c.z = a->z + b->z;
    return c;
}

PHvec3 phSub(const PHvec3 *a, const PHvec3 *b)
{
    PHvec3 c;
    c.x = a->x - b->x;
    c.y = a->y - b->y;
    c.z = a->z - b->z;
    return c;
}
#endif
