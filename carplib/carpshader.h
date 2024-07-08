#ifndef CARP_SHADER_HH
#define CARP_SHADER_HH

#include "carptype.h"

typedef s32 GLint;
typedef u32 GLuint;


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
