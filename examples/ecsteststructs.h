#ifndef CARP_ECS_TEST_STRUCTS_HH
#define CARP_ECS_TEST_STRUCTS_HH

#include "carplib/carpecsentities.h"
#include "carplib/carplib.h"
#include "carplib/carpmath.h"

#include <stdalign.h> //alignof


typedef enum CarpEcsEntityType
{
    CarpEcsEntityTypeNone,
    CarpEcsEntityTypePlayerEntity, // size per entity: 65
    CarpEcsEntityTypeCount,
} CarpEcsEntityType;

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

// size: 1, align: 1
typedef struct UsedComponent
{
    u8 usedComponentUsed; // offset: 0, size: 1, alignment: 1, padding: 0
} UsedComponent;
static_assert(sizeof(UsedComponent) == 1, "size not matching!");
static_assert(alignof(UsedComponent) == 1, "align not matching!");

static bool carp_ecs_createEntities(CarpEcsEntityType type, s32 amount, CarpEcsEntities* outEntities)
{
    // amount needs to be divisable by 32
    CARP_ASSERT_RETURN(amount > 0 && (amount & 31) == 0, false);
    CARP_ASSERT_RETURN(outEntities, false);
    CARP_ASSERT_RETURN(outEntities->carpEcsEntitiesData == NULL, false);
    CARP_ASSERT_RETURN((s32)type > (s32)CarpEcsEntityTypeNone, false);
    CARP_ASSERT_RETURN((s32)type < (s32)CarpEcsEntityTypeCount, false);

    outEntities->carpEcsEntitiesVersion = 1;
    outEntities->carpEcsEntitiesEntityType = (s32)type;
    outEntities->carpEcsEntitiesNextIndex = 0;
    outEntities->carpEcsEntitiesSize = 0;
    outEntities->carpEcsEntitiesCapacity = amount;

    switch(type)
    {
        case CarpEcsEntityTypeNone:
        case CarpEcsEntityTypeCount:
            return false;

        case CarpEcsEntityTypePlayerEntity:
        {
            outEntities->carpEcsEntitiesData = carp_lib_calloc(65, amount);
        }

    };
    return true;
}

static void carp_ecs_freeEntities(CarpEcsEntities* outEntities)
{
    CARP_ASSERT_RETURN(outEntities, );
    CARP_ASSERT_RETURN(outEntities->carpEcsEntitiesData != NULL, );
    carp_lib_free(outEntities->carpEcsEntitiesData);
    outEntities->carpEcsEntitiesData = NULL;
}

static bool carp_ecs_getTransformComponentMut(CarpEcsEntities* entities, TransformComponent** outComponents)
{
    CARP_ASSERT_RETURN(entities, false);
    CARP_ASSERT_RETURN(outComponents, false);
    CARP_ASSERT_RETURN(entities->carpEcsEntitiesEntityType > 0 && entities->carpEcsEntitiesEntityType < CarpEcsEntityTypeCount, false);
    *outComponents = NULL;
    switch((CarpEcsEntityType)entities->carpEcsEntitiesEntityType)
    {
        case CarpEcsEntityTypeNone:
        case CarpEcsEntityTypeCount:
            return false;

        case CarpEcsEntityTypePlayerEntity:
        {
            *outComponents = (TransformComponent*)(entities->carpEcsEntitiesData + (0 * entities->carpEcsEntitiesCapacity));
        }

    };
    return true;
}

static bool carp_ecs_getTransformComponent(const CarpEcsEntities* entities, const TransformComponent** outComponents)
{
    CARP_ASSERT_RETURN(entities, false);
    CARP_ASSERT_RETURN(outComponents, false);
    CARP_ASSERT_RETURN(entities->carpEcsEntitiesEntityType > 0 && entities->carpEcsEntitiesEntityType < CarpEcsEntityTypeCount, false);
    *outComponents = NULL;
    switch((CarpEcsEntityType)entities->carpEcsEntitiesEntityType)
    {
        case CarpEcsEntityTypeNone:
        case CarpEcsEntityTypeCount:
            return false;

        case CarpEcsEntityTypePlayerEntity:
        {
            *outComponents = (const TransformComponent*)(entities->carpEcsEntitiesData + (0 * entities->carpEcsEntitiesCapacity));
        }

    };
    return true;
}

static bool carp_ecs_getVelocityComponentMut(CarpEcsEntities* entities, VelocityComponent** outComponents)
{
    CARP_ASSERT_RETURN(entities, false);
    CARP_ASSERT_RETURN(outComponents, false);
    CARP_ASSERT_RETURN(entities->carpEcsEntitiesEntityType > 0 && entities->carpEcsEntitiesEntityType < CarpEcsEntityTypeCount, false);
    *outComponents = NULL;
    switch((CarpEcsEntityType)entities->carpEcsEntitiesEntityType)
    {
        case CarpEcsEntityTypeNone:
        case CarpEcsEntityTypeCount:
            return false;

        case CarpEcsEntityTypePlayerEntity:
        {
            *outComponents = (VelocityComponent*)(entities->carpEcsEntitiesData + (48 * entities->carpEcsEntitiesCapacity));
        }

    };
    return true;
}

static bool carp_ecs_getVelocityComponent(const CarpEcsEntities* entities, const VelocityComponent** outComponents)
{
    CARP_ASSERT_RETURN(entities, false);
    CARP_ASSERT_RETURN(outComponents, false);
    CARP_ASSERT_RETURN(entities->carpEcsEntitiesEntityType > 0 && entities->carpEcsEntitiesEntityType < CarpEcsEntityTypeCount, false);
    *outComponents = NULL;
    switch((CarpEcsEntityType)entities->carpEcsEntitiesEntityType)
    {
        case CarpEcsEntityTypeNone:
        case CarpEcsEntityTypeCount:
            return false;

        case CarpEcsEntityTypePlayerEntity:
        {
            *outComponents = (const VelocityComponent*)(entities->carpEcsEntitiesData + (48 * entities->carpEcsEntitiesCapacity));
        }

    };
    return true;
}

static bool carp_ecs_getUsedComponentMut(CarpEcsEntities* entities, UsedComponent** outComponents)
{
    CARP_ASSERT_RETURN(entities, false);
    CARP_ASSERT_RETURN(outComponents, false);
    CARP_ASSERT_RETURN(entities->carpEcsEntitiesEntityType > 0 && entities->carpEcsEntitiesEntityType < CarpEcsEntityTypeCount, false);
    *outComponents = NULL;
    switch((CarpEcsEntityType)entities->carpEcsEntitiesEntityType)
    {
        case CarpEcsEntityTypeNone:
        case CarpEcsEntityTypeCount:
            return false;

        case CarpEcsEntityTypePlayerEntity:
        {
            *outComponents = (UsedComponent*)(entities->carpEcsEntitiesData + (64 * entities->carpEcsEntitiesCapacity));
        }

    };
    return true;
}

static bool carp_ecs_getUsedComponent(const CarpEcsEntities* entities, const UsedComponent** outComponents)
{
    CARP_ASSERT_RETURN(entities, false);
    CARP_ASSERT_RETURN(outComponents, false);
    CARP_ASSERT_RETURN(entities->carpEcsEntitiesEntityType > 0 && entities->carpEcsEntitiesEntityType < CarpEcsEntityTypeCount, false);
    *outComponents = NULL;
    switch((CarpEcsEntityType)entities->carpEcsEntitiesEntityType)
    {
        case CarpEcsEntityTypeNone:
        case CarpEcsEntityTypeCount:
            return false;

        case CarpEcsEntityTypePlayerEntity:
        {
            *outComponents = (const UsedComponent*)(entities->carpEcsEntitiesData + (64 * entities->carpEcsEntitiesCapacity));
        }

    };
    return true;
}

#endif // CARP_ECS_TEST_STRUCTS_HH

