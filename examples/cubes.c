#include "compileall.c"

#include "carplib/carpgl.h"
#include "carplib/carpkeyboard.h"
#include "carplib/carplog.h"
#include "carplib/carpmemory.h"
#include "carplib/carpmouse.h"
#include "carplib/carpshader.h"
#include "carplib/carptime.h"
#include "carplib/carpwindow.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_POS_X 1000
#define WINDOW_POS_Y 1000

////layout (std140, binding=0, row_major) uniform frame_data


static const char vertexShaderCode[] =
    "#version 450 core\n"

    "   layout (std140, binding=0) uniform frame_data\n"
    "   {\n"
    "       mat4 viewProjMat;\n"
    "   } frameData;\n"

    "   layout (std430, binding=1, row_major) restrict readonly buffer mat_data\n"
    "   {\n"
    "       mat4 entityMats[];\n"
    "   } instanceMatData;\n"

    "   layout (std430, binding=2) restrict readonly buffer color_data\n"
    "   {\n"
    "       vec4 colors[];\n"
    "   } instanceColorData;\n"


    "   layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "   layout (location = 0) out vec4 outColor\n;"

    "   void main()\n"
    "   {\n"
    "       int instanceID = gl_InstanceID;\n"

    "       mat4 instanceMat = instanceMatData.entityMats[instanceID];\n"
    "       mat4 camMat = frameData.viewProjMat;\n"

    "       vec4 color = instanceColorData.colors[instanceID];\n"

    "       vec4 pos = vec4(vertexPosition_modelspace.xyz, 1.0);\n"
    "       pos = camMat * (instanceMat * pos);\n"
    "       gl_Position = pos;\n"
    "       outColor = color;\n"
    "       outColor.rgb *= 1.0 - (abs(pos.z) / 20.0f);\n"
    "   };\n";

static const char fragmentShaderCode[] =
    "#version 450 core \n"
    "layout (location = 0) in vec4 inColor\n;"

    "layout(location = 0) out vec4 outColor;\n"

    "void main() \n"
    "{ \n"
    "   outColor = inColor;\n"
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


    // An array of 3 vectors which represents 3 vertices
    static const f32 VertexBufferData[] = {
        // front
        -0.5f,  0.5f, 0.5f,
        0.5f,   0.5f, 0.5f,
        0.5f,  -0.5f, 0.5f,

        0.5f,  -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f,  0.5f, 0.5f,

        // back
        0.5f,  0.5f, -0.5f,
        -0.5f,   0.5f, -0.5f,
        -0.5f,  -0.5f, -0.5f,

        -0.5f,  -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        //left
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  -0.5f, 0.5f,

        -0.5f,  -0.5f,  0.5f,
        -0.5f,  -0.5f, -0.5f,
        -0.5f,   0.5f, -0.5f,

        //right
        0.5f,  0.5f,   0.5f,
        0.5f,  0.5f,  -0.5f,
        0.5f,  -0.5f, -0.5f,

        0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f,  0.5f,
        0.5f,   0.5f,  0.5f,

        //top
        -0.5f,  0.5f,  -0.5f,
         0.5f,  0.5f,  -0.5f,
         0.5f,  0.5f,   0.5f,

         0.5f,  0.5f,   0.5f,
        -0.5f,  0.5f,   0.5f,
        -0.5f,  0.5f,  -0.5f,

        //Bottom
        -0.5f, -0.5f,   0.5f,
         0.5f, -0.5f,   0.5f,
         0.5f, -0.5f,  -0.5f,

         0.5f, -0.5f,  -0.5f,
        -0.5f, -0.5f,  -0.5f,
        -0.5f, -0.5f,   0.5f,

    };


    GLuint vertexbuffer = 0;

    GLuint uniformBuffer = 0;
    GLuint matBuffer = 0;
    GLuint colorBuffer = 0;

    {
        glCreateBuffers(1, &uniformBuffer);
        glNamedBufferStorage(uniformBuffer,
            16*1024,
            NULL,
            GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);


        glCreateBuffers(1, &matBuffer);
        glNamedBufferStorage(matBuffer,
            16*1024*1024,
            NULL,
            GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

        glCreateBuffers(1, &colorBuffer);
        glNamedBufferStorage(colorBuffer,
            4*1024*1024,
            NULL,
            GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

        glUseProgram(shader.program);


        GLuint vertexArrayID = 0;
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBufferData),
            VertexBufferData, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    }












    carp_window_setWindowSizeChangedCallbackFn(&memory->carp_window, sWindowSizeChanged);
    carp_window_enableVSync(&memory->carp_window, true);

    CARP_LOGINFO("Window start running\n");
    memory->carp_window.running = true;


    CarpV3A positions[4] = {};
    positions[1].y = 2.0f;
    positions[2].y = 4.0f;
    positions[3].y = 6.0f;

    positions[0].z = -10.0f;
    positions[1].z = -10.0f;
    positions[2].z = -10.0f;
    positions[3].z = -10.0f;

    CarpV3A colors[4];
    for(int i = 0; i < 4; ++i)
    {
        colors[i].x = colors[i].y = colors[i].z = 0.6f;
        colors[i].w = 1.0f;
    }


    CarpM44 mats[4] = {};

    CarpQuat rotation; carp_math_getQuatIdentity(&rotation);
    CarpV3A scale = { 1.0f, 1.0f, 1.0f, 1.0f };

    CarpM44 cameraPerspectiveMat = {};
    CarpM44 cameraTransformMat = {};
    CarpV3A camPos = {};
    static const CarpV3A UpVector = {0.0f, 1.0f, 0.0f, 1.0f};
    static const CarpV3A RightVector = {1.0f, 0.0f, 0.0f, 1.0f};

    CarpQuat camPitchRotation;
    CarpQuat camYawRotation;
    f32 pitch = 0.0f;
    f32 yaw = 0.0f;

    CarpTime carpTime; carp_time_getCurrentTime(&carpTime);

    while(memory->carp_window.running)
    {
        CarpTime newTime; carp_time_getCurrentTime(&newTime);
        f64 dt = carp_time_getDifference(&carpTime, &newTime);
        carpTime = newTime;

        //printf("deltatime: %f\n", dt);
        static f32 MoveSpeed = 10.0f;
        static f32 RotationSpeed = 1.0f;
        carp_window_update(&memory->carp_window, 0.0f);
        if(carp_keyboard_wasKeyPressed(CarpKeyboardKey_Escape))
            memory->carp_window.running = false;


        if(carp_keyboard_isKeyDown(CarpKeyboardKey_I))
            pitch += RotationSpeed * dt;
        if(carp_keyboard_isKeyDown(CarpKeyboardKey_K))
            pitch -= RotationSpeed * dt;

        if(carp_keyboard_isKeyDown(CarpKeyboardKey_J))
            yaw += RotationSpeed * dt;
        if(carp_keyboard_isKeyDown(CarpKeyboardKey_L))
            yaw -= RotationSpeed * dt;

        CarpV3A camRight, camUp, camForward;
        carp_math_getDirectionsFromPitchYawRoll(
            pitch, yaw, 0.0f,
            &camRight, &camUp, &camForward
        );

        carp_math_mul_v3_f(&camRight, MoveSpeed * dt, &camRight);
        carp_math_mul_v3_f(&camUp, MoveSpeed * dt, &camUp);
        carp_math_mul_v3_f(&camForward, MoveSpeed * dt, &camForward);

        if(carp_keyboard_isKeyDown(CarpKeyboardKey_W))
            carp_math_sub_v3_v3(&camPos, &camForward, &camPos);
        if(carp_keyboard_isKeyDown(CarpKeyboardKey_S))
            carp_math_add_v3_v3(&camPos, &camForward, &camPos);

        if(carp_keyboard_isKeyDown(CarpKeyboardKey_A))
            carp_math_sub_v3_v3(&camPos, &camRight, &camPos);
        if(carp_keyboard_isKeyDown(CarpKeyboardKey_D))
            carp_math_add_v3_v3(&camPos, &camRight, &camPos);

        if(carp_keyboard_isKeyDown(CarpKeyboardKey_Q))
            camPos.y += MoveSpeed * dt;
        if(carp_keyboard_isKeyDown(CarpKeyboardKey_E))
            camPos.y -= MoveSpeed * dt;


        for(int i = 0; i < 4; ++i)
        {
            carp_math_getM44FromTRS(positions + i,
                &rotation,
                &scale,
                mats + i);
        }

        carp_math_createPerspectiveM44(90.0f,
            (f32)memory->carp_window.width / (f32)memory->carp_window.height,
            0.01f,
            1000.0f,
            &cameraPerspectiveMat);

        carp_math_getQuatFromAxisAngle(
            &RightVector,
            pitch,
            &camPitchRotation);
        carp_math_getQuatFromAxisAngle(
            &UpVector,
            yaw,
            &camYawRotation);
        carp_math_mul_q_q(&camYawRotation, &camPitchRotation, &camYawRotation);

        carp_math_getInverseM44FromTRS(
            &camPos, &camYawRotation, &scale, &cameraTransformMat);

        carp_math_mul_m44_m44(
            &cameraPerspectiveMat,
            &cameraTransformMat,
            &cameraPerspectiveMat);

        carp_math_transpose_m44(&cameraPerspectiveMat, &cameraPerspectiveMat);

        glNamedBufferSubData(uniformBuffer,
            0,
            sizeof(cameraPerspectiveMat),
            &cameraPerspectiveMat);

        glNamedBufferSubData(matBuffer,
            0,
            sizeof(mats),
            mats);

        glNamedBufferSubData(colorBuffer,
            0,
            sizeof(colors),
            colors);


        glClearColor(0.2f, 0.3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);



        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, matBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, colorBuffer);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);



        glDrawArraysInstanced(
            GL_TRIANGLES,
            0,
            sizeof(VertexBufferData) / sizeof(VertexBufferData[0]),
            3);
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
