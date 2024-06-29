#ifndef CARP_SHADER_HH
#define CARP_SHADER_HH

typedef int GLint;
typedef unsigned int GLuint;

#include "carptype.h"

typedef struct CarpPixelShader
{
    GLuint carp_pixelshader_vertexShader;
    GLuint carp_pixelshader_fragmentShader;
    GLuint carp_pixelshader_program;
    bool carp_pixelshader_isValid;
} CarpPixelShader;


CARP_FN CarpPixelShader carp_shader_compilePixelShader(const char* vertexCode, const char* fragCode);
CARP_FN void carp_shader_deletePixelShader(CarpPixelShader* shader);



#endif // CARP_SHADER_HH
