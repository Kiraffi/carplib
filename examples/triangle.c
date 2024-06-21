#include "compileall.c"

#include "carplib/carpgl.h"
#include "carplib/carpkeyboard.h"
#include "carplib/carplog.h"
#include "carplib/carpmemory.h"
#include "carplib/carpmouse.h"
#include "carplib/carpshader.h"
#include "carplib/carpwindow.h"

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
    CARP_LOGINFO("Window size changed: %i - %i\n", width, height);
    if(glad_glViewport)
        glad_glViewport(0, 0, width, height);
}



static s32 sMainAfterWindow(void)
{
    CarpMemory* memory = carp_memory_get();

    CarpPixelShader shader = carp_shader_compilePixelShader(vertexShaderCode, fragmentShaderCode);
    if(!shader.isValid)
    {
        CARP_LOGERROR("Failed to compile shader\n");
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
        f32 vertexBufferData[] = {
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












    carp_window_setWindowSizeChangedCallbackFn(&memory->carp_window, sWindowSizeChanged);
    carp_window_enableVSync(&memory->carp_window, true);

    CARP_LOGINFO("Window start running\n");
    memory->carp_window.running = true;

    while(memory->carp_window.running)
    {
        carp_window_update(&memory->carp_window, 0.0f);
        if(carp_keyboard_wasKeyPressed(CarpKeyboardKey_Escape))
            memory->carp_window.running = false;
            /*
        CARP_LOGDEBUG("Mouse x:%i, y: %i, l:%i, m:%i, r:%i, b4:%i, b5:%i, wheel x:%i, y:%i\n",
            carp_mouse_getPositionX(),
            carp_mouse_getPositionY(),
            carp_mouse_isButtonDown(CarpMouseButton_Left),
            carp_mouse_isButtonDown(CarpMouseButton_Middle),
            carp_mouse_isButtonDown(CarpMouseButton_Right),
            carp_mouse_isButtonDown(CarpMouseButton_Button4),
            carp_mouse_isButtonDown(CarpMouseButton_Button5),
            carp_mouse_getWheelX(),
            carp_mouse_getWheelY()
        );
*/
        glClearColor(0.2f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        carp_window_swapBuffers(&memory->carp_window);
    }
    CARP_LOGINFO("Window finished running\n");


    carp_shader_deletePixelShader(&shader);

    return 0;
}

static s32 sMain(void)
{
    CarpMemory* memory = carp_memory_get();

    CARP_LOGINFO("Creating window\n");

    if(!carp_window_init(&memory->carp_window,
        "My window",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        WINDOW_POS_X,
        WINDOW_POS_Y))
    {
        CARP_LOGERROR("Failed to init windows\n");
        return -1;
    }
    sMainAfterWindow();

   carp_window_destroy(&memory->carp_window);

    return 0;
}

int main(int argc, char** argv)
{
    if(!carp_memory_init())
    {
        CARP_LOGERROR("Failed to init the memory\n");
        return -1;
    }
    int result = sMain();

    carp_memory_destroy();

    return result;
}
