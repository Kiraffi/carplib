#ifndef CARP_ECS_TEST_STRUCTS_HH
#define CARP_ECS_TEST_STRUCTS_HH

#include "carplib/carpecsentities.h"
#include "carplib/carplib.h"
#include "carplib/carpmath.h"

#include <stdalign.h> //alignof


typedef enum CarpEcsEntityType
{
    CarpEcsEntityTypeNone,
    CarpEcsEntityTypePlayerEntity, // size per entity: 129
    CarpEcsEntityTypePlayer2DEntity, // size per entity: 17
    CarpEcsEntityTypeCount,
} CarpEcsEntityType;

// size: 64, align: 16
typedef struct MatComponent
{
    CarpM44 matComponentM4; // offset: 0, size: 64, alignment: 16, padding: 0
} MatComponent;
static_assert(sizeof(MatComponent) == 64, "size not matching!");
static_assert(alignof(MatComponent) == 16, "align not matching!");

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

// size: 8, align: 8
typedef struct Pos2DComponent
{
    CarpV2 pos2DComponentPos; // offset: 0, size: 8, alignment: 8, padding: 0
} Pos2DComponent;
static_assert(sizeof(Pos2DComponent) == 8, "size not matching!");
static_assert(alignof(Pos2DComponent) == 8, "align not matching!");

// size: 8, align: 8
typedef struct Vel2DComponent
{
    CarpV2 vel2DComponentVel; // offset: 0, size: 8, alignment: 8, padding: 0
} Vel2DComponent;
static_assert(sizeof(Vel2DComponent) == 8, "size not matching!");
static_assert(alignof(Vel2DComponent) == 8, "align not matching!");

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
            outEntities->carpEcsEntitiesData = carp_lib_calloc(129, amount);
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
        {
            outEntities->carpEcsEntitiesData = carp_lib_calloc(17, amount);
            break;
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

static bool carp_ecs_getMatComponentMut(CarpEcsEntities* entities, MatComponent** outComponents)
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
            *outComponents = (MatComponent*)(entities->carpEcsEntitiesData + (64 * entities->carpEcsEntitiesCapacity));
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
            return false;

    };
    return true;
}

static bool carp_ecs_getMatComponent(const CarpEcsEntities* entities, const MatComponent** outComponents)
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
            *outComponents = (const MatComponent*)(entities->carpEcsEntitiesData + (64 * entities->carpEcsEntitiesCapacity));
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
            return false;

    };
    return true;
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
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
            return false;

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
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
            return false;

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
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
            return false;

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
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
            return false;

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
            *outComponents = (UsedComponent*)(entities->carpEcsEntitiesData + (128 * entities->carpEcsEntitiesCapacity));
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
        {
            *outComponents = (UsedComponent*)(entities->carpEcsEntitiesData + (16 * entities->carpEcsEntitiesCapacity));
            break;
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
            *outComponents = (const UsedComponent*)(entities->carpEcsEntitiesData + (128 * entities->carpEcsEntitiesCapacity));
            break;
        }

        case CarpEcsEntityTypePlayer2DEntity:
        {
            *outComponents = (const UsedComponent*)(entities->carpEcsEntitiesData + (16 * entities->carpEcsEntitiesCapacity));
            break;
        }

    };
    return true;
}

static bool carp_ecs_getPos2DComponentMut(CarpEcsEntities* entities, Pos2DComponent** outComponents)
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
            return false;

        case CarpEcsEntityTypePlayer2DEntity:
        {
            *outComponents = (Pos2DComponent*)(entities->carpEcsEntitiesData + (0 * entities->carpEcsEntitiesCapacity));
            break;
        }

    };
    return true;
}

static bool carp_ecs_getPos2DComponent(const CarpEcsEntities* entities, const Pos2DComponent** outComponents)
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
            return false;

        case CarpEcsEntityTypePlayer2DEntity:
        {
            *outComponents = (const Pos2DComponent*)(entities->carpEcsEntitiesData + (0 * entities->carpEcsEntitiesCapacity));
            break;
        }

    };
    return true;
}

static bool carp_ecs_getVel2DComponentMut(CarpEcsEntities* entities, Vel2DComponent** outComponents)
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
            return false;

        case CarpEcsEntityTypePlayer2DEntity:
        {
            *outComponents = (Vel2DComponent*)(entities->carpEcsEntitiesData + (8 * entities->carpEcsEntitiesCapacity));
            break;
        }

    };
    return true;
}

static bool carp_ecs_getVel2DComponent(const CarpEcsEntities* entities, const Vel2DComponent** outComponents)
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
            return false;

        case CarpEcsEntityTypePlayer2DEntity:
        {
            *outComponents = (const Vel2DComponent*)(entities->carpEcsEntitiesData + (8 * entities->carpEcsEntitiesCapacity));
            break;
        }

    };
    return true;
}

#endif // CARP_ECS_TEST_STRUCTS_HH

