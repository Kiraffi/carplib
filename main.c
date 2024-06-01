#define WIN32_LEAN_AND_MEAN 1

#include "external/glad/gl.c"


#if _WIN32
//#if _MSC_VER > 0
    #include "carplib/carpwindow_win32.c"
#else
    #include "carplib/carpwindow_linux.c"
#endif


#include "carplib/carpgl.c"
#include "carplib/carpkeyboard.c"
#include "carplib/carpmemory.c"
#include "carplib/carpmouse.c"
#include "carplib/carpshader.c"


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
    CarpMemory* memory = carpMemory_get();

    CarpPixelShader shader = carp_shader_compilePixelShader(vertexShaderCode, fragmentShaderCode);
    if(!shader.isValid)
    {
        printf("Failed to compile shader\n");
        carp_shader_deletePixelShader(&shader);
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












    carpWindow_setWindowSizeChangedFn(&memory->carp_window, sWindowSizeChanged);
    carpWindow_enableVSync(&memory->carp_window, true);

    printf("Window start running\n");
    memory->carp_window.running = true;

    while(memory->carp_window.running)
    {
        carpWindow_update(&memory->carp_window, 0.0f);
        if(carp_keyboard_wasKeyPressed(CarpKeyboardKey_Escape))
            memory->carp_window.running = false;
        printf("Mouse x:%i, y: %i, l:%i, m:%i, r:%i, b4:%i, b5:%i, wheel x:%i, y:%i\n",
            carp_mouse_getMousePositionX(),
            carp_mouse_getMousePositionY(),
            carp_mouse_isButtonDown(CarpMouseButton_Left),
            carp_mouse_isButtonDown(CarpMouseButton_Middle),
            carp_mouse_isButtonDown(CarpMouseButton_Right),
            carp_mouse_isButtonDown(CarpMouseButton_Button4),
            carp_mouse_isButtonDown(CarpMouseButton_Button5),
            carp_mouse_getMouseWheelX(),
            carp_mouse_getMouseWheelY()
        );

        glClearColor(0.2f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        carpWindow_swapBuffers(&memory->carp_window);
    }
    printf("Window finished running\n");


    carp_shader_deletePixelShader(&shader);

    return 0;
}

static s32 sMain()
{
    CarpMemory* memory = carpMemory_get();

    printf("Creating window\n");

    if(!carpWindow_init(&memory->carp_window,
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

   carpWindow_destroy(&memory->carp_window);

    return 0;
}

int main(int argc, char** argv)
{
    if(!carpMemory_init())
    {
        printf("Failed to init the memory\n");
        return -1;
    }
    int result = sMain();

    carpMemory_deinit();

    return result;
}
