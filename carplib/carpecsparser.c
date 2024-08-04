#include "carpecsparser.h"

#include "carpassert.h"
#include "carplib.h"


typedef struct CarpECSParseMemory
{
    const char* dataPtrStart;
    const char* dataPtr;
    struct CarpECSVariableDef* carpECSParseMemorys;
    struct CarpECSComponentDef* carpECSComponentDefs;

    s32 carpECSParseMemoryAmount;
    s32 carpECSComponentDefAmount;
} CarpECSParseMemory;

typedef enum CarpECSVariable
{
    CarpECSVariableNone,

    CarpECSVariableI8,
    CarpECSVariableI16,
    CarpECSVariableI32,
    CarpECSVariableI64,

    CarpECSVariableU8,
    CarpECSVariableU16,
    CarpECSVariableU32,
    CarpECSVariableU64,

    CarpECSVariableF32,
    CarpECSVariableF64,

    CarpECSVariableVec2,
    CarpECSVariableVec3A,

    CarpECSVariableMat34,
    CarpECSVariableMat44,

    CarpECSVariableLast,
} CarpECSVariable;

typedef struct CarpECSVariableDef
{
    const char* carpECSVariableDefName;
    s32 carpECSVariableDefNameLen;
    CarpECSVariable carpECSVariableDefType;
    s32 carpECSVariableDefArrSize;

} CarpECSVariableDef;

typedef struct CarpECSComponentDef
{
    const char* carpEcsComponentDefName;
    s32 carpEcsComponentDefNameLen;

    s32 carpEcsComponentDefVarIndexStart;
    s32 carpEcsComponentDefVarAmount;
    
    s32 carpEcsComponentDefComponentSizeInBytes;
} CarpECSComponentDef;

static void s_printComponent(const CarpECSComponentDef* component)
{
    CARP_ASSERT_RETURN(component, );
    CARP_LOG("Component: %-.*s\n", 
        component->carpEcsComponentDefNameLen, 
        component->carpEcsComponentDefName);

}

static bool s_carp_ecs_tryParseComponent(
    const char** data,
    s32 len,
    CarpECSComponentDef* outComponent)
{
    CARP_ASSERT_RETURN(data, false);
    CARP_ASSERT_RETURN(outComponent, false);
    
    static const s32 ComponentWordLen = sizeof("Component") - 1;
    if(len < ComponentWordLen + 1)
    {
        return false;
    }
    const char* componentBegin = (*data) - ComponentWordLen;
    if(carp_lib_memcmp(componentBegin, "Component", ComponentWordLen) == 0)
    {
        outComponent->carpEcsComponentDefName = (*data) - len;
        outComponent->carpEcsComponentDefNameLen = len;
    }
    return true;
}


CARP_FN bool carp_ecs_parseEcsData(const char* data, CarpECSParsedFile* outParsedFile)
{
    CARP_ASSERT_RETURN(data, false);
    CARP_ASSERT_RETURN(outParsedFile, false);

    
    CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&data), false);
    if(*data == '\0')
    {
        return true;
    }
    const char* currentPos = data;
    CARP_ASSERT_RETURN(carp_lib_skipUntilWhiteSpace(&data), false);
    s32 len = (s32)(data - currentPos);
    CARP_ASSERT_RETURN(len > 0, false);

    CarpECSComponentDef newComponent = { 0 };

    if(s_carp_ecs_tryParseComponent(&data, len, &newComponent))
    {
        s_printComponent(&newComponent);
    }
    else
    {
        CARP_ASSERT_RETURN(0 && "Failed to find more data.", false);
    }
    return true;
}



