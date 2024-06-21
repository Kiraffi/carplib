#ifndef CARPGL_HH
#define CARPGL_HH


#include "../external/glad/gl.h"
//#include <gl/GL.h>

#include "carptype.h"


#if defined(CARP_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY APIENTRY
#endif

#ifndef GLAPI
#define GLAPI extern
#endif

//typedef char GLchar;
//typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

typedef void* (* carpGlLoadProc)(const char *name);
CARP_FN b8 loadGLFuncs(carpGlLoadProc proc);



#endif /*CARPGL_HH*/
