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


CarpPixelShader carpShader_compilePixelShader(const char* vertexCode, const char* fragCode);
void carpShader_deletePixelShader(CarpPixelShader* shader);



#endif // CARP_SHADER_HH