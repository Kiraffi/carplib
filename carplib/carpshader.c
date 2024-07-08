#include "carpshader.h"

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif


#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_FALSE
#define GL_FALSE 0
#endif

#ifndef GL_TRUE
#define GL_TRUE 1
#endif

static GLuint sCompileShader(s32 shaderType, const char* code)
{
    GLint isCompiled = 0;
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code, 0);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        maxLength = maxLength < 1023 ? maxLength : 1023;
        char buffer[1024] = {0};

        glGetShaderInfoLog(shader, maxLength, &maxLength, buffer);
        buffer[1023] = '\0';

        CARP_LOGERROR("Failed to compile shader: %s\n", buffer);
        return 0;
    }
    return shader;
}



CARP_FN CarpPixelShader carp_shader_compilePixelShader(const char* vertexCode, const char* fragCode)
{
    CarpPixelShader result = {0};

    GLuint vertexShader = sCompileShader(GL_VERTEX_SHADER, vertexCode);
    if(vertexShader == 0)
    {
        glDeleteShader(vertexShader);
        return result;
    }

    GLuint fragmentShader = sCompileShader(GL_FRAGMENT_SHADER, fragCode);
    if(vertexShader == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return result;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (s32 *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        maxLength = maxLength < 1023 ? maxLength : 1023;
        char buffer[1024] = {0};

        glGetProgramInfoLog(program, maxLength, &maxLength, buffer);
        buffer[1023] = '\0';

        CARP_LOGERROR("Failed to link shader: %s\n", buffer);

        glDeleteProgram(program);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return result;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    result.carp_pixelshader_vertexShader = vertexShader;
    result.carp_pixelshader_fragmentShader = fragmentShader;
    result.carp_pixelshader_program = program;
    result.carp_pixelshader_isValid = true;

    return result;
}

CARP_FN void carp_shader_deletePixelShader(CarpPixelShader* shader)
{
    if(shader)
    {
        if(shader->carp_pixelshader_program)
            glDeleteProgram(shader->carp_pixelshader_program);

        if(shader->carp_pixelshader_vertexShader)
            glDeleteShader(shader->carp_pixelshader_vertexShader);
        if(shader->carp_pixelshader_fragmentShader)
            glDeleteShader(shader->carp_pixelshader_fragmentShader);

    }
    CarpPixelShader tmp = {0};
    *shader = tmp;
}

