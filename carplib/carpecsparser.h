#ifndef CARP_ECS_PARSER_HH
#define CARP_ECS_PARSER_HH

#include "carpbuffer.h"
#include "carptype.h"

typedef struct CarpECSParsedFile
{
    CarpBuffer data;
} CarpECSParsedFile;


CARP_FN bool carp_ecs_parseEcsStructs(const char* data, CarpECSParsedFile* outParsedFile);



#endif // CARP_ECS_PARSER_HH

