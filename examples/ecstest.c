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
#define USE_SIMD2D 0


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

    "layout(std140, binding = 3) restrict readonly buffer InstanceDataVel2D\n"
    "{\n"
    "    vec2 vel[];\n"
    "} instanceDataVel2D;\n"

    "void main()\n"
    "{\n"
    "    vec4 p = vertexPos * 4.0;\n"
#if USE_2D_POS
    "    vec2 vel = instanceDataVel2D.vel[gl_InstanceID].xy;\n"
    "    float angle = atan(vel.y, vel.x);\n"
    "    float s = sin(angle);\n"
    "    float c = cos(angle);\n"
    "    vec2 tmpPos = p.xy;"
    "    p.x = tmpPos.x * c - tmpPos.y * s;\n"
    "    p.y = tmpPos.x * s + tmpPos.y * c;\n"
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
    "HitCountComponent \n"
    "{\n"
    "    hits : s32 \n"
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
    //"    used : UsedComponent\n"
    "}\n"

    "RegionEntity \n"
    "{\n"
    "    pos : Pos2DComponent\n"
    "    vel : Vel2DComponent\n"
    "    s32 : HitCountComponent\n"
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


static bool sUpdateRegion(
    s32 w,
    s32 h,
    s32 regionsX,
    s32 regionsY,
    const CarpEcsEntities* entities,
    CarpEcsEntities* regions)
{
    const Pos2DComponent* posEnt;
    const Vel2DComponent* velEnt;
    if(!carp_ecs_getPos2DComponent(entities, &posEnt)
        || !carp_ecs_getVel2DComponent(entities, &velEnt)
    )
    {
        return false;
    }


    Pos2DComponent* posReg;
    Vel2DComponent* velReg;
    HitCountComponent* hitReg;
    if(!carp_ecs_getPos2DComponentMut(regions, &posReg)
        || !carp_ecs_getVel2DComponentMut(regions, &velReg)
        || !carp_ecs_getHitCountComponentMut(regions, &hitReg)
    )
    {
        return false;
    }
    carp_lib_memset(posReg, 0, sizeof(Pos2DComponent) * regions->carpEcsEntitiesSize);
    carp_lib_memset(velReg, 0, sizeof(Vel2DComponent) * regions->carpEcsEntitiesSize);
    carp_lib_memset(hitReg, 0, sizeof(HitCountComponent) * regions->carpEcsEntitiesSize);

    CarpV2 cellSize = { (w + 4 + (regionsX - 1)) / regionsX, (w + 4 + (regionsY - 1)) / regionsY };
    cellSize.x = 1.0f / cellSize.x;
    cellSize.y = 1.0f / cellSize.y;

    s32 capacity = entities->carpEcsEntitiesCapacity;
    const CarpV2* p = &(posEnt->pos2DComponentPos);
    const CarpV2* v = &(velEnt->vel2DComponentVel);
    const CarpV2* pEnd = p + capacity;

    while(p < pEnd)
    {
        f32 x = p->x * cellSize.x;
        f32 y = p->y * cellSize.y;
        s32 xI = x;
        s32 yI = y;
        while(xI < 0) xI += regionsX;
        while(xI >= regionsX) xI -= regionsX;
        while(yI < 0) yI += regionsY;
        while(yI >= regionsY) yI -= regionsY;

        CARP_ASSERT_RETURN(xI >= 0, false);
        CARP_ASSERT_RETURN(yI >= 0, false);
        CARP_ASSERT_RETURN(xI < regionsX, false);
        CARP_ASSERT_RETURN(yI < regionsY, false);

        CarpV2* pReg = &(posReg[xI + yI * regionsX].pos2DComponentPos);
        CarpV2* vReg = &(velReg[xI + yI * regionsX].vel2DComponentVel);
        s32* hReg = &(hitReg[xI + yI * regionsX].hitCountComponentHits);

        carp_math_add_v2_v2(p, pReg, pReg);
        carp_math_add_v2_v2(v, vReg, vReg);
        *hReg += 1;

        ++p;
        ++v;
    }
    for(s32 i = 0; i < regionsX * regionsY; ++i)
    {
        CarpV2* pReg = &(posReg[i].pos2DComponentPos);
        s32* hReg = &(hitReg[i].hitCountComponentHits);
        if(*hReg > 0)
            carp_math_div_v2_f(pReg, *hReg, pReg);
    }


    return true;
}


static bool sUpdate2D(
    f32 dt,
    s32 w,
    s32 h,
    s32 regionsX,
    s32 regionsY,
    const CarpEcsEntities* regions,
    CarpEcsEntities* entities)
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

    const Pos2DComponent* posReg;
    const Vel2DComponent* velReg;
    const HitCountComponent* hitReg;
    if(!carp_ecs_getPos2DComponent(regions, &posReg)
        || !carp_ecs_getVel2DComponent(regions, &velReg)
        || !carp_ecs_getHitCountComponent(regions, &hitReg)
    )
    {
        return false;
    }

    s32 capacity = entities->carpEcsEntitiesCapacity;
    CarpV2* p = &(pos->pos2DComponentPos);
    CarpV2* v = &(vel->vel2DComponentVel);
    CarpV2* pEnd = p + capacity;

    CarpV2 cellSize = { (w + 4 + (regionsX - 1)) / regionsX, (w + 4 + (regionsY - 1)) / regionsY };
    cellSize.x = 1.0f / cellSize.x;
    cellSize.y = 1.0f / cellSize.y;

    while(p < pEnd)
    {
        CarpV2 posSum; carp_math_zero_v2(&posSum);
        CarpV2 velSum; carp_math_zero_v2(&velSum);
        CarpV2 diffSum; carp_math_zero_v2(&velSum);

        s32 inRangeCount = 0;

        f32 vLen = carp_math_len_v2(v);
        CarpV2 vUnit; carp_math_mul_v2_f(v, 1.0f / vLen, &vUnit);

        f32 range = 75.0f;
#if 1
        {
            s32 xLen = range * cellSize.x + 1;
            s32 yLen = range * cellSize.y + 1;

            s32 xs = range * cellSize.x;
            s32 ys = range * cellSize.y;

            s32 xp = p->x * cellSize.x;
            s32 yp = p->y * cellSize.y;

            s32 xEnd = xp + xLen;
            s32 yEnd = yp + yLen;

            xs = xp - xs;
            ys = yp - ys;

            for(s32 y = ys; y < yEnd; ++y)
            {
                s32 yi = (y + regionsY) % regionsY;
                for(s32 x = xs; x < xEnd; ++x)
                {
                    s32 xi = (x + regionsX) % regionsX;
                    s32 i = xi + yi * regionsX;
                    const s32* hReg = &(hitReg[i].hitCountComponentHits);

                    if(*hReg <= 0)
                    {
                        continue;
                    }
                    const CarpV2* pReg = &(posReg[i].pos2DComponentPos);


                    CarpV2 tmp = *pReg;

                    s32 vx = xp - x;
                    if(vx > xLen + 1) tmp.x += w;
                    if(vx < -xLen - 1) tmp.x -= w;

                    s32 vy = y - yp;
                    if(vy > yLen + 1) tmp.y += h;
                    if(vy < -yLen - 1) tmp.y -= h;

                    CarpV2 diff; carp_math_sub_v2_v2(p, &tmp, &diff);

                    f32 diffLenSq = carp_math_sqrLen_v2(&diff);
                    f32 diffLen = sqrtf(diffLenSq);

                    f32 d = -carp_math_dot_v2(&diff, &vUnit);
                    if(diffLenSq > 0.0f && diffLenSq < range * range && d / diffLen >= 0.15f)
                    {
                        const CarpV2* vReg = &(velReg[i].vel2DComponentVel);

                        CarpV2 diffUnitSq; carp_math_mul_v2_f(&diff, 1.0f * (*hReg) / diffLen, &diffUnitSq);
                        // put back the multiply
                        carp_math_mul_v2_f(pReg, *hReg, &tmp);

                        carp_math_add_v2_v2(&diffSum, &diffUnitSq, &diffSum);
                        carp_math_add_v2_v2(&posSum, &tmp, &posSum);
                        carp_math_add_v2_v2(&velSum, vReg, &velSum);

                        inRangeCount += *hReg;
                    }

                }
            }
        }
#else
        {
            CarpV2* pStart = &(pos->pos2DComponentPos);
            CarpV2* vStart = &(vel->vel2DComponentVel);

            while(pStart < pEnd)
            {
                if(pStart == p)
                {
                    ++pStart;
                    ++vStart;
                    continue;
                }
                CarpV2 diff; carp_math_sub_v2_v2(pStart, p, &diff);
                if(diff.x > w * 0.5f) diff.x = w - diff.x;
                if(diff.x < -w * 0.5f) diff.x = w + diff.x;
                if(diff.y > h * 0.5f) diff.y = h - diff.y;
                if(diff.y < -h * 0.5f) diff.y = h + diff.y;

                f32 diffLenSq = carp_math_sqrLen_v2(&diff);
                if(diffLenSq > 0.0f && diffLenSq < range * range)
                {
                    f32 diffLen = sqrtf(diffLenSq);
                    CarpV2 diffUnit; carp_math_mul_v2_f(&diff, 1.0f / diffLen, &diffUnit);
                    f32 see = carp_math_dot_v2(&diffUnit, &vUnit);
                    if(see > 0.0f)
                    {
                        ++inRangeCount;

                        //CarpV2 diffUnitSq; carp_math_mul_v2_f(&diff, 50.0f / diffLenSq, &diffUnitSq);

                        carp_math_add_v2_v2(&posSum, pStart, &posSum);
                        carp_math_add_v2_v2(&velSum, vStart, &velSum);
                        carp_math_add_v2_v2(&diffSum, &diffUnit, &diffSum);
                    }
                }

                ++pStart;
                ++vStart;

            }
        }
#endif
        if(inRangeCount > 0)
        {
            carp_math_div_v2_f(&posSum, inRangeCount, &posSum);
            carp_math_div_v2_f(&velSum, inRangeCount, &velSum);

            carp_math_sub_v2_v2(&posSum, p, &posSum);

            f32 posLen = carp_math_sqrLen_v2(&posSum);
            f32 velLen = carp_math_sqrLen_v2(&velSum);
            f32 velDiffLen = carp_math_sqrLen_v2(&diffSum);
            #if 1
            if(posLen > 0.0f)
            {
                carp_math_mul_v2_f(&posSum, (dt * 12.5f) / sqrtf(posLen), &posSum);
                //carp_math_mul_v2_f(&posSum, (dt * 5.0f), &posSum);
                carp_math_add_v2_v2(v, &posSum, v);
            }
            #endif
            #if 1
            if(velLen > 0.0f)
            {
                //carp_math_mul_v2_f(&velSum, (dt * 5.0f), &velSum);
                carp_math_mul_v2_f(&velSum, (dt * 7.5f) / sqrtf(velLen), &velSum);
                carp_math_add_v2_v2(v, &velSum, v);
            }
            #endif
            #if 1
            if(velDiffLen > 0.0f)
            {
                carp_math_mul_v2_f(&diffSum, (dt * 7.5f) / sqrtf(velDiffLen), &diffSum);
                //carp_math_mul_v2_f(&diffSum, (dt * 5.0f), &diffSum);
                carp_math_add_v2_v2(v, &diffSum, v);
            }
            #endif

        }

//bounce from walls
/*
        if(p->x < 100.0f) v->intArr[0] &= 0x7fffffff;
        else if(p->x > w - 100.0f) v->intArr[0] |= 0x80000000;
        if(p->y < 100.0f) v->intArr[1] &= 0x7fffffff;
        else if(p->y > h - 100.0f) v->intArr[1] |= 0x80000000;
*/


        {
            f32 velLen = carp_math_len_v2(v);
            if(velLen > 0.0f)
            {
                CarpV2 vUnit; carp_math_mul_v2_f(v, dt * 5.0f / velLen, &vUnit);
                carp_math_add_v2_v2(v, &vUnit, v);
            }
        }

        {
            f32 velLen = carp_math_len_v2(v);
            if(velLen > 50.0f)
            {
                carp_math_mul_v2_f(v, 50.0f / velLen, v);
            }
        }


/*
        {
            f32 velLen = carp_math_len_v2(v);
            if(velLen > 50.0f)
            {
                carp_math_mul_v2_f(v, 50.0f / velLen, v);
            }
        }
*/
        carp_math_mul_v2_f(v, dt, &newVel);
        carp_math_add_v2_v2(p, &newVel, p);

        while(p->x < 0.0f) p->x += w;
        while(p->y < 0.0f) p->y += h;
        while(p->x >= w) p->x -= w;
        while(p->y >= h) p->y -= h;

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
    static const s32 EntitiyCount = 16 * 256 * 4;

    static const s32 RegionsX = 32 * 1;
    static const s32 RegionsY = 32 * 1;

    const s32 Regions = RegionsX * RegionsY;


    CarpEcsEntities entitiesRegions = {0};

#if USE_2D_POS
    CarpEcsEntities entities2D = {0};
#else
    CarpEcsEntities entities = {0};
#endif
#if USE_2D_POS
    if(!carp_ecs_createEntities(CarpEcsEntityTypePlayer2DEntity, EntitiyCount, &entities2D))
    {
        CARP_LOGERROR("Failed to create entities\n");
        return -1;
    }
    if(!carp_ecs_createEntities(CarpEcsEntityTypeRegionEntity, Regions, &entitiesRegions))
    {
        CARP_LOGERROR("Failed to create entities\n");
        return -1;
    }
    entities2D.carpEcsEntitiesSize = entities2D.carpEcsEntitiesCapacity;
    entitiesRegions.carpEcsEntitiesSize = entitiesRegions.carpEcsEntitiesCapacity;
#else
    if(!carp_ecs_createEntities(CarpEcsEntityTypePlayerEntity, EntitiyCount, &entities))
    {
        CARP_LOGERROR("Failed to create entities\n");
        carp_ecs_freeEntities(&entities);
        return -1;
    }
#endif
    CarpOGLBuffer vertexBuffer = {0};
    CarpOGLBuffer uniformBuffer = {0};
    CarpOGLBuffer instanceDataBuffer = {0};
    CarpOGLBuffer instanceDataBuffer2DPos = {0};
    CarpOGLBuffer instanceDataBuffer2DVelocity = {0};

    GLuint vertexbuffer = 0;
    {
        glUseProgram(shader.carp_pixelshader_program);


        GLuint vertexArrayID = 0;
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        // An array of 3 vectors which represents 3 vertices
        static const f32 VertexBufferData[] = {
            -1.0f, -1.0f, 0.0f,
            2.0f,   0.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,

//            -1.0f, -1.0f, 0.0f,
//            1.0f, -1.0f, 0.0f,
//            0.0f,  2.0f, 0.0f,
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
            &instanceDataBuffer2DPos);

        carp_ogl_createBuffer(
            sizeof(CarpV2) * EntitiyCount,
            NULL,
            GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT,
            &instanceDataBuffer2DVelocity);


        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.carp_OGLBuffer_handle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBufferData),
            VertexBufferData, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.carp_OGLBuffer_handle);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Init entities
#if USE_2D_POS
    {
        Pos2DComponent* pos2D;
        Vel2DComponent* vel2D;
        if(carp_ecs_getPos2DComponentMut(&entities2D, &pos2D)
            && carp_ecs_getVel2DComponentMut(&entities2D, &vel2D))
        {
            entities2D.carpEcsEntitiesSize = entities2D.carpEcsEntitiesCapacity;
            CarpV2 mulC = {
                memory->carp_window.carp_window_width - 100,
                memory->carp_window.carp_window_height - 100,
            };
            CarpV2 addC = {
                50,
                50,
            };
            for(s32 i = 0; i < entities2D.carpEcsEntitiesCapacity; ++i)
            {
                CarpV2* p = &(pos2D[i].pos2DComponentPos);
                p->x = (float)(rand()) / (float)RAND_MAX;
                p->y = (float)(rand()) / (float)RAND_MAX;

                carp_math_mul_v2_v2(p, &mulC, p);
                carp_math_add_v2_v2(p, &addC, p);

                CarpV2* v = &(vel2D[i].vel2DComponentVel);

                v->x = (((float)(rand())) / (float)RAND_MAX) * 2.0f - 1.0f;
                v->y = (((float)(rand())) / (float)RAND_MAX) * 2.0f - 1.0f;

                carp_math_mul_v2_f(v, 16.0f, v);
            }
        }
    }
#else
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
                p->x = (float)(rand()) / (float)RAND_MAX;
                p->y = (float)(rand()) / (float)RAND_MAX;
                p->z = (float)(rand()) / (float)RAND_MAX;

                carp_math_mul_v3_v3(p, &mulC, p);
                p->w = 1.0f;

                CarpV3A* v = &(vel[i].velocityComponentVel);

                v->x = (((float)(rand())) / (float)RAND_MAX) * 2.0f - 1.0f;
                v->y = (((float)(rand())) / (float)RAND_MAX) * 2.0f - 1.0f;
                v->z = (((float)(rand())) / (float)RAND_MAX) * 2.0f - 1.0f;

                carp_math_mul_v3_f(v, 5.0f, v);
                v->w = 1.0f;
            }
        }
    }
#endif








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
        s32 w = memory->carp_window.carp_window_width;
        s32 h = memory->carp_window.carp_window_height;

        if(sUpdateRegion(w, h, RegionsX, RegionsY, &entities2D, &entitiesRegions)
            && sUpdate2D(dt, w, h, RegionsX, RegionsY, &entitiesRegions, &entities2D))
        {
            const Pos2DComponent* pos;
            const Vel2DComponent* vel;
            if(carp_ecs_getPos2DComponent(&entities2D, &pos)
                && carp_ecs_getVel2DComponent(&entities2D, &vel))
            {
                carp_ogl_updateBuffer(&instanceDataBuffer2DPos,
                    0,
                    pos,
                    sizeof(CarpV2) * EntitiyCount);
                carp_ogl_updateBuffer(&instanceDataBuffer2DVelocity,
                    0,
                    vel,
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
        carp_ogl_bindBuffer(&instanceDataBuffer2DPos, 2, GL_SHADER_STORAGE_BUFFER);
        carp_ogl_bindBuffer(&instanceDataBuffer2DVelocity, 3, GL_SHADER_STORAGE_BUFFER);

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

#if USE_2D_POS
#else
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
#endif
    carp_shader_deletePixelShader(&shader);
#if USE_2D_POS
    carp_ecs_freeEntities(&entities2D);
#else
    carp_ecs_freeEntities(&entities);
#endif
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

#if 0
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
