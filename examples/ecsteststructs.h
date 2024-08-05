#ifndef CARP_ECS_TEST_STRUCTS_HH
#define CARP_ECS_TEST_STRUCTS_HH

#include "carplib/carpmath.h"

#include <stdalign.h> //alignof

// size: 224, align: 16
typedef struct TestComponent
{
    s8 testComponentAs08; // offset: 0, size: 1, alignment: 1, padding: 0
    u8 testComponentAu08; // offset: 1, size: 1, alignment: 1, padding: 0
    u16 testComponentAu16; // offset: 2, size: 2, alignment: 2, padding: 0
    s16 testComponentAs16; // offset: 4, size: 2, alignment: 2, padding: 0
    s32 testComponentAs32; // offset: 8, size: 4, alignment: 4, padding: 2
    s64 testComponentAs64; // offset: 16, size: 8, alignment: 8, padding: 4
    u32 testComponentAu32; // offset: 24, size: 4, alignment: 4, padding: 0
    u64 testComponentAu64; // offset: 32, size: 8, alignment: 8, padding: 4
    f32 testComponentAf32; // offset: 40, size: 4, alignment: 4, padding: 0
    f64 testComponentAf64; // offset: 48, size: 8, alignment: 8, padding: 4
    CarpV2 testComponentAv2; // offset: 56, size: 8, alignment: 8, padding: 0
    CarpV3A testComponentAv3; // offset: 64, size: 16, alignment: 16, padding: 0
    CarpQuat testComponentAquat; // offset: 80, size: 16, alignment: 16, padding: 0
    CarpM34 testComponentAm34; // offset: 96, size: 48, alignment: 16, padding: 0
    CarpM44 testComponentAm44; // offset: 144, size: 64, alignment: 16, padding: 0
    s8 testComponentForcepadding; // offset: 208, size: 1, alignment: 1, padding: 0
    s8 testComponentPadding[15]; // offset: 209, size: 15, alignment: 1, padding: 0
} TestComponent;
static_assert(sizeof(TestComponent) == 224, "size not matching!");
static_assert(alignof(TestComponent) == 16, "align not matching!");

// size: 1, align: 1
typedef struct TestSmallComponent
{
    s8 testSmallComponentV; // offset: 0, size: 1, alignment: 1, padding: 0
} TestSmallComponent;
static_assert(sizeof(TestSmallComponent) == 1, "size not matching!");
static_assert(alignof(TestSmallComponent) == 1, "align not matching!");

// size: 32, align: 1
typedef struct TestArrComponent
{
    s8 testArrComponentAs08_32[32]; // offset: 0, size: 32, alignment: 1, padding: 0
} TestArrComponent;
static_assert(sizeof(TestArrComponent) == 32, "size not matching!");
static_assert(alignof(TestArrComponent) == 1, "align not matching!");

// size: 48, align: 16
typedef struct TransformComponent
{
    CarpV3A transformComponentPos; // offset: 0, size: 16, alignment: 16, padding: 0
    CarpQuat transformComponentRot; // offset: 16, size: 16, alignment: 16, padding: 0
    CarpV3A transformComponentScale; // offset: 32, size: 16, alignment: 16, padding: 0
} TransformComponent;
static_assert(sizeof(TransformComponent) == 48, "size not matching!");
static_assert(alignof(TransformComponent) == 16, "align not matching!");

// size: 16, align: 16
typedef struct VelocityComponent
{
    CarpV3A velocityComponentVel; // offset: 0, size: 16, alignment: 16, padding: 0
} VelocityComponent;
static_assert(sizeof(VelocityComponent) == 16, "size not matching!");
static_assert(alignof(VelocityComponent) == 16, "align not matching!");

#endif // CARP_ECS_TEST_STRUCTS_HH

