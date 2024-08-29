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

#define USE_2D_POS 1
#define USE_SIMD2D 1


static const char vertexShaderCode[] =
    "#version 450 core\n"

    "layout(location = 0) in vec4 vertexPos;\n"

    "layout(std140, binding = 0) uniform FrameData\n"
    "{\n"
    "    vec4 screenSize;\n"
    "} frameData;\n"

    "layout(std140, binding = 1) restrict readonly buffer InstanceData\n"
    "{\n"
    "    vec4 pos[];\n"
    "} instanceData;\n"

    "layout(std140, binding = 2) restrict readonly buffer InstanceData2D\n"
    "{\n"
    "    vec2 pos[];\n"
    "} instanceData2D;\n"

    "void main()\n"
    "{\n"
    "    vec4 p = vertexPos * 3.0;\n"
#if USE_2D_POS
    "    p.xy += instanceData2D.pos[gl_InstanceID].xy;\n"
#else
    "    p.xy += instanceData.pos[gl_InstanceID].xy;\n"
#endif
    "    p.xy = (2.0 * p.xy / frameData.screenSize.xy) - 1.0;\n"
    "    p.z = 0.0;\n"
    "    p.w = 1.0;\n"
    "    gl_Position = p;\n"
    "};\n";

static const char fragmentShaderCode[] =
    "#version 450 core \n"
    "layout(location = 0) out vec4 color; \n"

    "void main() \n"
    "{ \n"
    "    color = vec4(1.0, 0.0, 0.0, 1.0); \n"
    "} \n";

static const char ecsData[] =
    " \n"
    /*
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
    */
    /* Empty component fails to pass
    "TestEmptyComponent \n"
    "{\n"
    "}\n"
    */

/*

    "TestArrComponent \n"
    "{\n"
    "    as08_32  : s8[32] \n"
    // empty array is not valid
    //"    as08_0  : s8[0] \n"
    "}\n"
    */
    "MatComponent\n"
    "{\n"
    "    m4 : M44 \n"
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
    "UsedComponent \n"
    "{\n"
    "    used : u8 \n"
    "}\n"


    "Pos2DComponent \n"
    "{\n"
    "    pos : Vec2 \n"
    "}\n"
    "Vel2DComponent \n"
    "{\n"
    "    vel : Vec2 \n"
    "}\n"


    "PlayerEntity \n"
    "{\n"
    "    transform : TransformComponent\n"
    "    velocity : VelocityComponent\n"
    "    mat: MatComponent\n"
    "    used : UsedComponent\n"
    "}\n"

    "Player2DEntity \n"
    "{\n"
    "    pos : Pos2DComponent\n"
    "    vel : Vel2DComponent\n"
    "    used : UsedComponent\n"
    "}\n"
/*
    "TestEntity \n"
    "{\n"
    "    first : TestArrComponent\n"
    "    used : UsedComponent\n"
    "}\n"
*/
    ""


    "";
static void sWindowSizeChanged(int width, int height)
{
    CARP_LOGINFO("Window size changed: %i - %i\n", width, height);
    if(glad_glViewport)
        glad_glViewport(0, 0, width, height);
}


static bool sUpdate3D(f32 dt, s32 w, s32 h, CarpEcsEntities* entities)
{
    CarpV3A newVel;
    TransformComponent* transform;
    VelocityComponent* vel;
    if(!carp_ecs_getTransformComponentMut(entities, &transform)
        || !carp_ecs_getVelocityComponentMut(entities, &vel)
    )
    {
        return false;
    }
    for(s32 i = 0; i < entities->carpEcsEntitiesCapacity; ++i)
    {
        CarpV3A* p = &(transform[i].transformComponentPos);
        CarpV3A* v = &(vel[i].velocityComponentVel);

        carp_math_mul_v3_f(v, dt, &newVel);
        carp_math_add_v3_v3(p, &newVel, p);

        if(p->x < 0.0f | p->x > w)
            v->x = -v->x;
        if(p->y < 0.0f | p->y > h)
            v->y = -v->y;
    }

    return true;
}


static bool sUpdate2D(f32 dt, s32 w, s32 h, CarpEcsEntities* entities)
{
#if USE_SIMD2D
    CarpV3A newVel;
#else
    CarpV2 newVel;
#endif
    Pos2DComponent* pos;
    Vel2DComponent* vel;
    if(!carp_ecs_getPos2DComponentMut(entities, &pos)
        || !carp_ecs_getVel2DComponentMut(entities, &vel)
    )
    {
        return false;
    }
#if USE_SIMD2D
    s32 capacity = entities->carpEcsEntitiesCapacity / 2;
    CarpV3A* p = (CarpV3A*)&(pos->pos2DComponentPos);
    CarpV3A* v = (CarpV3A*)&(vel->vel2DComponentVel);
    CarpV3A* pEnd = p + capacity;
#else
    s32 capacity = entities->carpEcsEntitiesCapacity;
    CarpV2* p = &(pos->pos2DComponentPos);
    CarpV2* v = &(vel->vel2DComponentVel);
    CarpV2* pEnd = p + capacity;
#endif
    while(p < pEnd)
    {
#if USE_SIMD2D

        carp_math_mul_v3_f(v, dt, &newVel);
        carp_math_add_v3_v3(p, &newVel, p);

        v->x = (p->x < 0.0f | p->x > w) ? -v->x : v->x;
        v->y = (p->y < 0.0f | p->y > h) ? -v->y : v->y;
        v->z = (p->z < 0.0f | p->z > w) ? -v->z : v->z;
        v->w = (p->w < 0.0f | p->w > h) ? -v->w : v->w;

#else
        carp_math_mul_v2_f(v, dt, &newVel);
        carp_math_add_v2_v2(p, &newVel, p);

        v->x = (p->x < 0.0f | p->x > w) ? -v->x : v->x;
        v->y = (p->y < 0.0f | p->y > h) ? -v->y : v->y;
#endif
        ++p;
        ++v;
    }

    return true;
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
    static const s32 EntitiyCount = 512 * 1024;
    CarpEcsEntities entities = {0};
    CarpEcsEntities entities2D = {0};

    if(!carp_ecs_createEntities(CarpEcsEntityTypePlayerEntity, EntitiyCount, &entities))
    {
        CARP_LOGERROR("Failed to create entities\n");
        carp_ecs_freeEntities(&entities);
        return -1;
    }
    if(!carp_ecs_createEntities(CarpEcsEntityTypePlayer2DEntity, EntitiyCount, &entities2D))
    {
        CARP_LOGERROR("Failed to create entities\n");
        return -1;
    }

    CarpOGLBuffer vertexBuffer = {0};
    CarpOGLBuffer uniformBuffer = {0};
    CarpOGLBuffer instanceDataBuffer = {0};
    CarpOGLBuffer instanceDataBuffer2D = {0};

    GLuint vertexbuffer = 0;
    {
        glUseProgram(shader.carp_pixelshader_program);


        GLuint vertexArrayID = 0;
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        // An array of 3 vectors which represents 3 vertices
        static const f32 VertexBufferData[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
        };

        carp_ogl_createBuffer(
            1024,
            NULL,
            GL_STATIC_DRAW,
            &vertexBuffer);

        carp_ogl_createBuffer(
            16 * 1024,
            NULL,
            GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT,
            &uniformBuffer);

        carp_ogl_createBuffer(
            sizeof(TransformComponent) * EntitiyCount,
            NULL,
            GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT,
            &instanceDataBuffer);

        carp_ogl_createBuffer(
            sizeof(CarpV2) * EntitiyCount,
            NULL,
            GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT,
            &instanceDataBuffer2D);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.carp_OGLBuffer_handle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBufferData),
            VertexBufferData, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.carp_OGLBuffer_handle);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Init entities
    {
        TransformComponent* transform;
        VelocityComponent *vel;
        if(carp_ecs_getTransformComponentMut(&entities, &transform)
            && carp_ecs_getVelocityComponentMut(&entities, &vel))
        {
            entities.carpEcsEntitiesSize = entities.carpEcsEntitiesCapacity;
            CarpV3A mulC = {
                memory->carp_window.carp_window_width,
                memory->carp_window.carp_window_height,
                1.0f
            };
            for(s32 i = 0; i < entities.carpEcsEntitiesCapacity; ++i)
            {
                CarpV3A* p = &(transform[i].transformComponentPos);
                p->x = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;
                p->y = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;
                p->z = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;

                carp_math_mul_v3_v3(p, &mulC, p);
                p->w = 1.0f;

                CarpV3A* v = &(vel[i].velocityComponentVel);

                v->x = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;
                v->y = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;
                v->z = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;

                carp_math_mul_v3_f(v, 5.0f, v);
                v->w = 1.0f;
            }
        }
    }
    {
        Pos2DComponent* pos2D;
        Vel2DComponent* vel2D;
        if(carp_ecs_getPos2DComponentMut(&entities2D, &pos2D)
            && carp_ecs_getVel2DComponentMut(&entities2D, &vel2D))
        {
            entities2D.carpEcsEntitiesSize = entities.carpEcsEntitiesCapacity;
            CarpV2 mulC = {
                memory->carp_window.carp_window_width,
                memory->carp_window.carp_window_height,
            };
            for(s32 i = 0; i < entities.carpEcsEntitiesCapacity; ++i)
            {
                CarpV2* p = &(pos2D[i].pos2DComponentPos);
                p->x = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;
                p->y = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;

                carp_math_mul_v2_v2(p, &mulC, p);

                CarpV2* v = &(vel2D[i].vel2DComponentVel);

                v->x = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;
                v->y = ((float)(rand() % 65536) / 65536.0f) * 2.0f - 1.0f;

                carp_math_mul_v2_f(v, 5.0f, v);
            }
        }
    }









    carp_window_setWindowSizeChangedCallbackFn(&memory->carp_window, sWindowSizeChanged);
    carp_window_enableVSync(&memory->carp_window, false);

    CARP_LOGINFO("Window start running\n");
    memory->carp_window.carp_window_running = true;

    CarpTime carpTime;
    carp_time_getCurrentTime(&carpTime);

    f64 sincePrint = 0.0;
    s32 frames = 0;
    while(memory->carp_window.carp_window_running)
    {
        CarpTime newTime;
        carp_time_getCurrentTime(&newTime);
        f64 dt = carp_time_getDifference(&carpTime, &newTime);
        ++frames;
        sincePrint += dt;
        if(sincePrint > 1)
        {
            CARP_LOGINFO("Fps: %i\n", frames);
            frames = 0;
            sincePrint = 0.0;
        }

        carpTime = newTime;

        carp_window_update(&memory->carp_window, dt);
        if(carp_keyboard_wasKeyPressed(CarpKeyboardKey_Escape))
            memory->carp_window.carp_window_running = false;
#if USE_2D_POS
        if(sUpdate2D(
            dt,
            memory->carp_window.carp_window_width,
            memory->carp_window.carp_window_height,
            &entities2D))
        {
            const Pos2DComponent* pos;
            if(carp_ecs_getPos2DComponent(&entities2D, &pos))
            {
                carp_ogl_updateBuffer(&instanceDataBuffer2D,
                    0,
                    pos,
                    sizeof(CarpV2) * EntitiyCount);
            }
        }
#else
        if(sUpdate3D(
            dt,
            memory->carp_window.carp_window_width,
            memory->carp_window.carp_window_height,
            &entities))
        {
            const TransformComponent* transform;
            if(carp_ecs_getTransformComponent(&entities, &transform))
            {
                carp_ogl_updateBuffer(&instanceDataBuffer,
                    0,
                    transform,
                    sizeof(TransformComponent) * EntitiyCount);
            }
        }
#endif
        f32 sizes[64] = {
            memory->carp_window.carp_window_width,
            memory->carp_window.carp_window_height,
        };
        carp_ogl_updateBuffer(&uniformBuffer,
            0,
            sizes,
            sizeof(sizes));

        glClearColor(0.2f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

        carp_ogl_bindBuffer(&uniformBuffer, 0, GL_UNIFORM_BUFFER);
        carp_ogl_bindBuffer(&instanceDataBuffer, 1, GL_SHADER_STORAGE_BUFFER);
        carp_ogl_bindBuffer(&instanceDataBuffer2D, 2, GL_SHADER_STORAGE_BUFFER);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.carp_OGLBuffer_handle);

        glDrawArraysInstanced(
            GL_TRIANGLES,
            0,
            3,
            EntitiyCount);

        glDisableVertexAttribArray(0);

        carp_window_swapBuffers(&memory->carp_window);
    }
    CARP_LOGINFO("Window finished running\n");


    const TransformComponent* transform;
    if(carp_ecs_getTransformComponent(&entities, &transform))
    {
        {
            const CarpV3A* v = &transform->transformComponentPos;
            CARP_LOGINFO("x: %f, y: %f, z: %f\n", v->x, v->y, v->z);
        }
        {
            const CarpV3A* v = &(transform[entities.carpEcsEntitiesCapacity - 1].transformComponentPos);
            CARP_LOGINFO("x: %f, y: %f, z: %f\n", v->x, v->y, v->z);
        }
    }

    carp_shader_deletePixelShader(&shader);
    carp_ecs_freeEntities(&entities);
    carp_ecs_freeEntities(&entities2D);
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

#if 1
    CarpECSParsedFile file = { 0 };

    if(!carp_ecs_parseEcsData(ecsData, "CARP_ECS_TEST_STRUCTS_HH", &file))
    {
        CARP_LOGERROR("Failed to parse ecs data\n");
        carp_memory_destroy();
        return -1;
    }


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

    s32 result = sMain();

    carp_buffer_free(&file.data);
#else
    s32 result = sMain();
#endif

    carp_memory_destroy();

    return result;
}
