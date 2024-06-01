#ifndef CARP_SHADER_HH
#define CARP_SHADER_HH

typedef int GLint;
typedef unsigned int GLuint;

#include <stdbool.h>

typedef struct CarpPixelShader
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
    bool isValid;
} CarpPixelShader;


CarpPixelShader carp_shader_compilePixelShader(const char* vertexCode, const char* fragCode);
void carp_shader_deletePixelShader(CarpPixelShader* shader);



#endif // CARP_SHADER_HH