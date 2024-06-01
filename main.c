#define WIN32_LEAN_AND_MEAN 1

#include "external/glad/gl.c"


#if _WIN32
//#if _MSC_VER > 0
    #include "carplib/carpwindow_win32.c"
#else
    #include "carplib/carpwindow_linux.c"
#endif

#include "carplib/carpwindow.h"

#include "carplib/mymemory.h"

#include "carplib/mymemory.c"
#include "carplib/carpgl.h"
#include "carplib/carpgl.c"
#include "carplib/mykey.c"
#include "carplib/mykey.h"

#include "carplib/shader.h"
#include "carplib/shader.c"



#include <malloc.h> // malloc, otherwise tcc will coredump
#include <stdalign.h> //alignof
#include <stdbool.h> //bool
#include <stdio.h> //printf
#include <stdlib.h> //free
//#include <stdatomic.h>
#include <stdint.h>



#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_POS_X 1000
#define WINDOW_POS_Y 1000

static const char vertexShaderCode[] =
    "#version 450 core\n"
    "    layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "    void main()\n"
    "    {\n"
    "        gl_Position.xyz = vertexPosition_modelspace;\n"
    "        gl_Position.w = 1.0;\n"
    "    };\n";

static const char fragmentShaderCode[] =
    "#version 450 core \n"
    "layout(location = 0) out vec4 color; \n"

    "void main() \n"
    "{ \n"
    "    color = vec4(1.0, 0.0, 0.0, 1.0); \n"
    "} \n";




static void sWindowSizeChanged(int width, int height)
{
    printf("Window size changed: %i - %i\n", width, height);
    if(glad_glViewport)
        glad_glViewport(0, 0, width, height);
}



static s32 sMainAfterWindow()
{
    MyMemory* memory = myMemory_get();

    CarpPixelShader shader = carpShader_compilePixelShader(vertexShaderCode, fragmentShaderCode);
    if(!shader.isValid)
    {
        printf("Failed to compile shader\n");
        carpShader_deletePixelShader(&shader);
        return -1;
    }

    GLuint vertexbuffer = 0;
    {
        glUseProgram(shader.program);


        GLuint vertexArrayID = 0;
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        // An array of 3 vectors which represents 3 vertices
        float vertexBufferData[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
        };

        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData),
            vertexBufferData, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    }












    carpWindow_setWindowSizeChangedFn(&memory->window, sWindowSizeChanged);
    carpWindow_enableVSync(&memory->window, false);

    printf("Window start running\n");
    memory->window.running = true;

    while(memory->window.running)
    {
        carpWindow_update(&memory->window, 0.0f);
        if(carpKeys_wasPressed(MyKey_Escape))
            memory->window.running = false;


        glClearColor(0.2f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        carpWindow_swapBuffers(&memory->window);
    }
    printf("Window finished running\n");


    carpShader_deletePixelShader(&shader);

    return 0;
}

static s32 sMain()
{
    MyMemory* memory = myMemory_get();

    printf("Creating window\n");

    if(!carpWindow_init(&memory->window,
        "My window",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        WINDOW_POS_X,
        WINDOW_POS_Y))
    {
        printf("Failed to init windows\n");
        return -1;
    }
    sMainAfterWindow();

   carpWindow_destroy(&memory->window);

    return 0;
}

int main(int argc, char** argv)
{
    if(!myMemory_init())
    {
        printf("Failed to init the memory\n");
        return -1;
    }
    int result = sMain();

    myMemory_deinit();

    return result;
}
