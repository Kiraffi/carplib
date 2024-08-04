#include "compileall.c"

#include "carplib/carpecsparser.h"
#include "carplib/carpgl.h"
#include "carplib/carpkeyboard.h"
#include "carplib/carplog.h"
#include "carplib/carpmemory.h"
#include "carplib/carpmouse.h"
#include "carplib/carpshader.h"
#include "carplib/carpwindow.h"

#include "ecsteststructs.h"

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

static const char ecsData[] =
    " \n"
    "TestComponent \n"
    "{\n"
    "    as08  : s8 \n"
    "    au08  : u8 \n"
    "    au16  : u16 \n"
    "    as16  : s16 \n"
    "    as32  : s32 \n"
    "    as64  : s64 \n"
    "    au32  : u32 \n"
    "    au64  : u64 \n"
    "    af32  : f32 \n"
    "    af64  : f64 \n"
    "    av2   : Vec2 \n"
    "    av3   : Vec3 \n"
    "    aquat : Quat \n"
    "    am34  : M34 \n"
    "    am44  : M44 \n"
    "    forcepadding : s8 \n"
    "}\n"
    "TestSmallComponent \n"
    "{\n"
    "    v : s8\n"
    "}\n"
    /* Empty component fails to pass
    "TestEmptyComponent \n"
    "{\n"
    "}\n"
    */
    "TestArrComponent \n"
    "{\n"
    "    as08_32  : s8[32] \n"
    // empty array is not valid
    //"    as08_0  : s8[0] \n"
    "}\n"
    "TransformComponent \n"
    "{\n"
    "    pos : Vec3 \n"
    "    rot : Quat \n"
    "    scale : Vec3 \n"
    "}\n"
    "VelocityComponent \n"
    "{\n"
    "    vel : Vec3 \n"
    "}\n"
    ""


    "";
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
    if(!shader.carp_pixelshader_isValid)
    {
        CARP_LOGERROR("Failed to compile shader\n");
        carp_shader_deletePixelShader(&shader);
        return -1;
    }

    GLuint vertexbuffer = 0;
    {
        glUseProgram(shader.carp_pixelshader_program);


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
    memory->carp_window.carp_window_running = true;

    while(memory->carp_window.carp_window_running)
    {
        carp_window_update(&memory->carp_window, 0.0f);
        if(carp_keyboard_wasKeyPressed(CarpKeyboardKey_Escape))
            memory->carp_window.carp_window_running = false;

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

s32 main(s32 argc, char** argv)
{
    if(!carp_memory_init())
    {
        CARP_LOGERROR("Failed to init the memory\n");
        return -1;
    }
    CarpECSParsedFile file = { 0 };

    if(!carp_ecs_parseEcsData(ecsData, &file))
    {
        CARP_LOGERROR("Failed to parse ecs data\n");
        carp_memory_destroy();
        return -1;
    }
    
    s32 result = sMain();

    if(!carp_lib_writeFile(
        "examples/ecsteststructs.h",
        (const char*)file.data.carp_buffer_data,
        file.data.carp_buffer_size))
    {
        CARP_LOGERROR("Failed to write file\n");
        carp_buffer_free(&file.data);
        carp_memory_destroy();
        return -1;
    }
    carp_buffer_free(&file.data);
    carp_memory_destroy();

    return result;
}