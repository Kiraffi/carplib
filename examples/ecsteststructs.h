#ifndef CARP_ECS_STRUCTS_HH
#define CARP_ECS_STRUCTS_HH

#include "carplib/carpmath.h"

#include <stdalign.h> //alignof

// size: 224, align: 16
typedef struct TestComponent
{
    s8 testComponentAs08; // offset: 0
    u8 testComponentAu08; // offset: 1
    u16 testComponentAu16; // offset: 2
    s16 testComponentAs16; // offset: 4
    s32 testComponentAs32; // offset: 8
    s64 testComponentAs64; // offset: 16
    u32 testComponentAu32; // offset: 24
    u64 testComponentAu64; // offset: 32
    f32 testComponentAf32; // offset: 40
    f64 testComponentAf64; // offset: 48
    CarpV2 testComponentAv2; // offset: 56
    CarpV3A testComponentAv3; // offset: 64
    CarpQuat testComponentAquat; // offset: 80
    CarpM34 testComponentAm34; // offset: 96
    CarpM44 testComponentAm44; // offset: 144
    s8 testComponentForcepadding; // offset: 208
    s8 testComponentPadding[15]; // offset: 209
} TestComponent;
static_assert(sizeof(TestComponent) == 224, "size not matching!");
static_assert(alignof(TestComponent) == 16, "align not matching!");

// size: 1, align: 1
typedef struct TestSmallComponent
{
    s8 testSmallComponentV; // offset: 0
} TestSmallComponent;
static_assert(sizeof(TestSmallComponent) == 1, "size not matching!");
static_assert(alignof(TestSmallComponent) == 1, "align not matching!");

// size: 32, align: 1
typedef struct TestArrComponent
{
    s8 testArrComponentAs08_32[32]; // offset: 0
} TestArrComponent;
static_assert(sizeof(TestArrComponent) == 32, "size not matching!");
static_assert(alignof(TestArrComponent) == 1, "align not matching!");

// size: 48, align: 16
typedef struct TransformComponent
{
    CarpV3A transformComponentPos; // offset: 0
    CarpQuat transformComponentRot; // offset: 16
    CarpV3A transformComponentScale; // offset: 32
} TransformComponent;
static_assert(sizeof(TransformComponent) == 48, "size not matching!");
static_assert(alignof(TransformComponent) == 16, "align not matching!");

// size: 16, align: 16
typedef struct VelocityComponent
{
    CarpV3A velocityComponentVel; // offset: 0
} VelocityComponent;
static_assert(sizeof(VelocityComponent) == 16, "size not matching!");
static_assert(alignof(VelocityComponent) == 16, "align not matching!");

#endif // CARP_ECS_STRUCTS_HH
