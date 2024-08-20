#ifndef CARP_ECS_ENTITIES_HH
#define CARP_ECS_ENTITIES_HH

#include "carptype.h"

static const s32 CarpEcsCurrentVersion = 1;

typedef struct CarpEcsEntities
{
    u8* carpEcsEntitiesData;
    s64 carpEcsEntitiesNextIndex;
    s32 carpEcsEntitiesEntityType;
    s32 carpEcsEntitiesVersion;
    s32 carpEcsEntitiesSize;
    s32 carpEcsEntitiesCapacity;
} CarpEcsEntities;


#endif // CARP_ECS_ENTITIES_HH



