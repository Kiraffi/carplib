#ifndef CARP_ECS_ENTITY_HEADER_HH
#define CARP_ECS_ENTITY_HEADER_HH

#include "carptype.h"


typedef struct CarpEcsEntityHeader
{
    //enum CarpEcsEntityType carpEcsEntityHeaderType;
    s32 carpEcsEntityHeaderEntityType;
    s32 carpEcsEntityHeaderVersion;
} CarpEcsEntityHeader;

#endif // CARP_ECS_ENTITY_HEADER_HH

