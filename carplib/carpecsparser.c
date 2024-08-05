#include "carpecsparser.h"

#include "carpassert.h"
#include "carplib.h"
#include "carpmath.h"

#include <stdalign.h>

static const s32 CarpECSVariableDefMaxAmount = 8 * 1024 * 1024;
static const s32 CarpECSComponentDefsMaxAmount = 1024 * 64;

typedef struct CarpECSParseMemory
{
    const char* dataPtrStart;
    const char* dataPtr;
    struct CarpECSVariableDef* carpECSVariableDefs;
    struct CarpECSComponentDef* carpECSComponentDefs;

    s32 CarpECSVariableDefCount;
    s32 carpECSComponentDefCount;
} CarpECSParseMemory;

typedef enum CarpECSVariableDefType
{
    CarpECSVariableDefTypeNone,

    CarpECSVariableDefTypeS8,
    CarpECSVariableDefTypeS16,
    CarpECSVariableDefTypeS32,
    CarpECSVariableDefTypeS64,

    CarpECSVariableDefTypeU8,
    CarpECSVariableDefTypeU16,
    CarpECSVariableDefTypeU32,
    CarpECSVariableDefTypeU64,

    CarpECSVariableDefTypeF32,
    CarpECSVariableDefTypeF64,

    CarpECSVariableDefTypeVec2,
    CarpECSVariableDefTypeVec3A,
    CarpECSVariableDefTypeQuat,

    CarpECSVariableDefTypeMat34,
    CarpECSVariableDefTypeMat44,

    CarpECSVariableDefTypeCount,
} CarpECSVariableDefType;

typedef struct CarpECSVariableDef
{
    const char* carpECSVariableDefName;
    s32 carpECSVariableDefNameLen;
    CarpECSVariableDefType carpECSVariableDefType;
    s32 carpECSVariableDefArrSize;
    s32 carpECSVariableDefComponentMemOffset;
    s8 carpEcsComponentDefPadding[8];

} CarpECSVariableDef;
static_assert(sizeof(CarpECSVariableDef) == 4 * 8, "Size is not matching!");
static_assert(alignof(CarpECSVariableDef) == 8, "Align is not matching!");

typedef struct CarpECSComponentDef
{
    const char* carpEcsComponentDefName;
    s32 carpEcsComponentDefNameLen;

    s32 carpEcsComponentDefVarIndexStart;
    s32 carpEcsComponentDefVarAmount;

    s32 carpEcsComponentDefComponentSizeInBytes;
    s32 carpEcsComponentDefComponentAlignInBytes;

    s8 carpEcsComponentDefPadding[4];
} CarpECSComponentDef;
static_assert(sizeof(CarpECSComponentDef) == 4 * 8, "Size is not matching!");
static_assert(alignof(CarpECSComponentDef) == 8, "Align is not matching!");

static bool s_carp_ecs_pushStringToBuffer(const char* str, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(str, false);
    CARP_ASSERT_RETURN(outBuffer, false);

    s32 len = carp_lib_strlen(str);
    CARP_ASSERT_RETURN(carp_buffer_pushBuffer((const u8*)str, len, outBuffer), false);
    return true;
}

static bool s_carp_ecs_advanceIfChar(char c, const char** ptr)
{
    CARP_ASSERT_RETURN(ptr, false);
    if(c != **ptr)
    {
        return false;
    }
    ++(*ptr);
    return true;
}

static s32 s_carp_ecs_parseNameLen(const char** c)
{
    CARP_ASSERT_RETURN(c, -1);
    CARP_ASSERT_RETURN(!carp_lib_isNumber(**c), -1);
    s32 len = 0;

    while(carp_lib_isNumber(**c) || carp_lib_isAlpha(**c) || **c == '_')
    {
        ++(*c);
        ++len;
    }
    return len;
}

static const char* s_carp_ecs_getVariableDefType(CarpECSVariableDefType varType)
{
    switch(varType)
    {
        case CarpECSVariableDefTypeNone: return NULL;

        case CarpECSVariableDefTypeS8: return "s8";
        case CarpECSVariableDefTypeS16: return "s16";
        case CarpECSVariableDefTypeS32: return "s32";
        case CarpECSVariableDefTypeS64: return "s64";

        case CarpECSVariableDefTypeU8: return "u8";
        case CarpECSVariableDefTypeU16: return "u16";
        case CarpECSVariableDefTypeU32: return "u32";
        case CarpECSVariableDefTypeU64: return "u64";

        case CarpECSVariableDefTypeF32: return "f32";
        case CarpECSVariableDefTypeF64: return "f64";

        case CarpECSVariableDefTypeVec2: return "CarpV2";
        case CarpECSVariableDefTypeVec3A: return "CarpV3A";
        case CarpECSVariableDefTypeQuat: return "CarpQuat";

        case CarpECSVariableDefTypeMat34: return "CarpM34";
        case CarpECSVariableDefTypeMat44: return "CarpM44";

        case CarpECSVariableDefTypeCount: return NULL;

    }
    return NULL;
}

static s32 s_carp_ecs_getVariableDefTypeSize(CarpECSVariableDefType varType)
{
    switch(varType)
    {
        case CarpECSVariableDefTypeNone: return 0;

        case CarpECSVariableDefTypeS8: return 1;
        case CarpECSVariableDefTypeS16: return 2;
        case CarpECSVariableDefTypeS32: return 4;
        case CarpECSVariableDefTypeS64: return 8;

        case CarpECSVariableDefTypeU8: return 1;
        case CarpECSVariableDefTypeU16: return 2;
        case CarpECSVariableDefTypeU32: return 4;
        case CarpECSVariableDefTypeU64: return 8;

        case CarpECSVariableDefTypeF32: return 4;
        case CarpECSVariableDefTypeF64: return 8;

        case CarpECSVariableDefTypeVec2: return (s32)sizeof(CarpV2);
        case CarpECSVariableDefTypeVec3A: return (s32)sizeof(CarpV3A);
        case CarpECSVariableDefTypeQuat: return (s32)sizeof(CarpQuat);

        case CarpECSVariableDefTypeMat34: return (s32)sizeof(CarpM34);
        case CarpECSVariableDefTypeMat44: return (s32)sizeof(CarpM44);

        case CarpECSVariableDefTypeCount: return 0;

    }
    return 0;
}

static s32 s_carp_ecs_getVariableDefTypeAlignSize(CarpECSVariableDefType varType)
{
    switch(varType)
    {
        case CarpECSVariableDefTypeNone: return 0;

        case CarpECSVariableDefTypeS8: return 1;
        case CarpECSVariableDefTypeS16: return 2;
        case CarpECSVariableDefTypeS32: return 4;
        case CarpECSVariableDefTypeS64: return 8;

        case CarpECSVariableDefTypeU8: return 1;
        case CarpECSVariableDefTypeU16: return 2;
        case CarpECSVariableDefTypeU32: return 4;
        case CarpECSVariableDefTypeU64: return 8;

        case CarpECSVariableDefTypeF32: return 4;
        case CarpECSVariableDefTypeF64: return 8;

        case CarpECSVariableDefTypeVec2: return (s32)alignof(CarpV2);
        case CarpECSVariableDefTypeVec3A: return (s32)alignof(CarpV3A);
        case CarpECSVariableDefTypeQuat: return (s32)alignof(CarpQuat);

        case CarpECSVariableDefTypeMat34: return (s32)alignof(CarpM34);
        case CarpECSVariableDefTypeMat44: return (s32)alignof(CarpM44);

        case CarpECSVariableDefTypeCount: return 0;

    }
    return 0;
}

static void s_carp_ecs_printVariable(s32 varIndex, const CarpECSParseMemory* mem)
{
    CARP_ASSERT_RETURN(mem, );
    CARP_ASSERT_RETURN(varIndex >= 0, );
    CARP_ASSERT_RETURN(varIndex < mem->CarpECSVariableDefCount, );

    const CarpECSVariableDef* var = mem->carpECSVariableDefs + varIndex;

    CARP_ASSERT_RETURN(var, );
    if(var->carpECSVariableDefArrSize > 0)
    {
        CARP_LOG("Variable: %-.*s  : %s[%i], offset: %i\n",
            var->carpECSVariableDefNameLen,
            var->carpECSVariableDefName,
            s_carp_ecs_getVariableDefType(var->carpECSVariableDefType),
            var->carpECSVariableDefArrSize,
            var->carpECSVariableDefComponentMemOffset
        );
    }
    else
    {
        CARP_LOG("Variable: %-.*s  : %s, offset: %i\n",
            var->carpECSVariableDefNameLen,
            var->carpECSVariableDefName,
            s_carp_ecs_getVariableDefType(var->carpECSVariableDefType),
            var->carpECSVariableDefComponentMemOffset
        );

    }
}

static void s_carp_ecs_printComponent(s32 compIndex, const CarpECSParseMemory* mem)
{
    CARP_ASSERT_RETURN(mem, );
    CARP_ASSERT_RETURN(compIndex >= 0, );
    CARP_ASSERT_RETURN(compIndex < mem->carpECSComponentDefCount, );

    const CarpECSComponentDef* comp = mem->carpECSComponentDefs + compIndex;

    CARP_LOG("Component: %-.*s, size:%i\n",
        comp->carpEcsComponentDefNameLen,
        comp->carpEcsComponentDefName,
        comp->carpEcsComponentDefComponentSizeInBytes);

    for(s32 i = 0; i < comp->carpEcsComponentDefVarAmount; ++i)
    {
        CARP_LOG("    ");
        s_carp_ecs_printVariable(comp->carpEcsComponentDefVarIndexStart + i, mem);
    }
}
static void s_carp_ecs_printBuffer(const CarpECSParsedFile* parsedFile)
{
    CARP_ASSERT_RETURN(parsedFile, );
    CARP_LOG("%s\n", (const char*)parsedFile->data.carp_buffer_data);
}



static CarpECSVariableDefType s_carp_ecs_getVariableTypeFromStr(const char* str, s32 sLen)
{
    if(sLen == 2)
    {
        if(carp_lib_memcmp(str, "s8", sLen) == 0)  return CarpECSVariableDefTypeS8;
        if(carp_lib_memcmp(str, "u8", sLen) == 0)  return CarpECSVariableDefTypeU8;
    }
    else if(sLen == 3)
    {
        if(carp_lib_memcmp(str, "f32", sLen) == 0) return CarpECSVariableDefTypeF32;
        if(carp_lib_memcmp(str, "f64", sLen) == 0) return CarpECSVariableDefTypeF64;

        if(carp_lib_memcmp(str, "s16", sLen) == 0) return CarpECSVariableDefTypeS16;
        if(carp_lib_memcmp(str, "s32", sLen) == 0) return CarpECSVariableDefTypeS32;
        if(carp_lib_memcmp(str, "s64", sLen) == 0) return CarpECSVariableDefTypeS64;

        if(carp_lib_memcmp(str, "u16", sLen) == 0) return CarpECSVariableDefTypeU16;
        if(carp_lib_memcmp(str, "u32", sLen) == 0) return CarpECSVariableDefTypeU32;
        if(carp_lib_memcmp(str, "u64", sLen) == 0) return CarpECSVariableDefTypeU64;
    }
    if(carp_lib_memcmp(str, "Vec2", sLen) == 0) return CarpECSVariableDefTypeVec2;
    if(carp_lib_memcmp(str, "Vec3", sLen) == 0) return CarpECSVariableDefTypeVec3A;
    if(carp_lib_memcmp(str, "Quat", sLen) == 0) return CarpECSVariableDefTypeQuat;

    if(carp_lib_memcmp(str, "M34", sLen) == 0) return CarpECSVariableDefTypeMat34;
    if(carp_lib_memcmp(str, "M44", sLen) == 0) return CarpECSVariableDefTypeMat44;

    return CarpECSVariableDefTypeNone;
}


static bool s_carp_ecs_tryParseVariables(CarpECSParseMemory* outMem)
{
    CARP_ASSERT_RETURN(outMem, false);
    CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);
    CARP_ASSERT_RETURN(s_carp_ecs_advanceIfChar('{', &outMem->dataPtr), false);

    CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);

    s32 componentSizeInBytes = 0;
    s32 highestAlign = 1;

    while(*outMem->dataPtr && *outMem->dataPtr != '}')
    {
        const char* nameStart = outMem->dataPtr;
        s32 nameLen = s_carp_ecs_parseNameLen(&outMem->dataPtr);
        CARP_ASSERT_RETURN(nameLen > 0, false);
        CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);

        CARP_ASSERT_RETURN(s_carp_ecs_advanceIfChar(':', &outMem->dataPtr), false);

        CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);

        const char* typeStart = outMem->dataPtr;
        s32 typeLen = s_carp_ecs_parseNameLen(&outMem->dataPtr);
        CARP_ASSERT_RETURN(typeLen > 0, false);



        CarpECSVariableDefType varType = s_carp_ecs_getVariableTypeFromStr(typeStart, typeLen);
        CARP_ASSERT_RETURN(varType != CarpECSVariableDefTypeNone, false);
        s32 varSize = s_carp_ecs_getVariableDefTypeSize(varType);
        s32 alignSize = s_carp_ecs_getVariableDefTypeAlignSize(varType);
        CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);

        CARP_ASSERT_RETURN(outMem->CarpECSVariableDefCount < CarpECSVariableDefMaxAmount, false);

        CarpECSVariableDef* newVar = outMem->carpECSVariableDefs + outMem->CarpECSVariableDefCount;
        ++outMem->CarpECSVariableDefCount;

        newVar->carpECSVariableDefName = nameStart;
        newVar->carpECSVariableDefNameLen = nameLen;
        newVar->carpECSVariableDefType = varType;


        componentSizeInBytes = (componentSizeInBytes + alignSize - 1) & (~(alignSize - 1));
        newVar->carpECSVariableDefComponentMemOffset = componentSizeInBytes;

        CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);
        if(s_carp_ecs_advanceIfChar('[', &outMem->dataPtr))
        {
            CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);
            const char* tmp = outMem->dataPtr;
            s32 num = (s32)carp_lib_strtoll(tmp, &outMem->dataPtr, 10);
            CARP_ASSERT_RETURN(outMem->dataPtr != tmp, false);
            CARP_ASSERT_RETURN(num > 0, false);

            newVar->carpECSVariableDefArrSize = num;
            varSize *= num;

            CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);

            CARP_ASSERT_RETURN(s_carp_ecs_advanceIfChar(']', &outMem->dataPtr), false);

            CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);
        }
        else
        {
            newVar->carpECSVariableDefArrSize = 0;
        }

        highestAlign = alignSize > highestAlign ? alignSize : highestAlign;
        componentSizeInBytes += varSize;

    };
    CARP_ASSERT_RETURN(s_carp_ecs_advanceIfChar('}', &outMem->dataPtr), false);

    s32 compSize = componentSizeInBytes;
    componentSizeInBytes = (componentSizeInBytes + highestAlign - 1) & (~(highestAlign - 1));
    if(compSize != componentSizeInBytes)
    {
        s32 paddingSize = componentSizeInBytes - compSize;

        CARP_ASSERT_RETURN(outMem->CarpECSVariableDefCount < CarpECSVariableDefMaxAmount, false);

        CarpECSVariableDef* newVar = outMem->carpECSVariableDefs + outMem->CarpECSVariableDefCount;
        ++outMem->CarpECSVariableDefCount;

        newVar->carpECSVariableDefName = "padding";
        newVar->carpECSVariableDefNameLen = sizeof("padding") - 1;
        newVar->carpECSVariableDefType = CarpECSVariableDefTypeS8;
        newVar->carpECSVariableDefArrSize = paddingSize;
        newVar->carpECSVariableDefComponentMemOffset = compSize;
    }


    return true;
}

static bool s_carp_ecs_tryParseComponent(CarpECSParseMemory* outMem)
{
    CARP_ASSERT_RETURN(outMem, false);

    CARP_ASSERT_RETURN(carp_lib_skipWhiteSpace(&outMem->dataPtr), false);
    if(*outMem->dataPtr == '\0')
    {
        return false;
    }

    const char* nameStart = outMem->dataPtr;
    s32 nameLen = s_carp_ecs_parseNameLen(&outMem->dataPtr);
    CARP_ASSERT_RETURN(nameLen > 0, false);

    static const s32 ComponentWordLen = sizeof("Component") - 1;
    if(nameLen < ComponentWordLen + 1)
    {
        outMem->dataPtr = nameStart;
        return false;
    }
    const char* componentBegin = outMem->dataPtr - ComponentWordLen;
    if(carp_lib_memcmp(componentBegin, "Component", ComponentWordLen) != 0)
    {
        outMem->dataPtr = nameStart;
        return false;
    }
    CARP_ASSERT_RETURN(outMem->carpECSComponentDefCount < CarpECSComponentDefsMaxAmount, false);
    CarpECSComponentDef* component = outMem->carpECSComponentDefs + outMem->carpECSComponentDefCount;
    ++(outMem->carpECSComponentDefCount);

    component->carpEcsComponentDefName = outMem->dataPtr - nameLen;
    component->carpEcsComponentDefNameLen = nameLen;

    s32 currentVariableCount = outMem->CarpECSVariableDefCount;
    component->carpEcsComponentDefVarIndexStart = currentVariableCount;

    CARP_ASSERT_RETURN(s_carp_ecs_tryParseVariables(outMem), false);
    component->carpEcsComponentDefVarAmount = outMem->CarpECSVariableDefCount - currentVariableCount;

    CARP_ASSERT_RETURN(component->carpEcsComponentDefVarAmount > 0, false);
    s32 componentSizeInBytes = 0;

    const CarpECSVariableDef* var = outMem->carpECSVariableDefs + currentVariableCount;
    s32 highestAlign = 1;
    for(s32 i = 0; i < component->carpEcsComponentDefVarAmount; ++i)
    {
        s32 alignSize = s_carp_ecs_getVariableDefTypeAlignSize(var->carpECSVariableDefType);
        highestAlign = highestAlign < alignSize ? alignSize : highestAlign;
        ++var;
    }
    --var;
    s32 varSize = s_carp_ecs_getVariableDefTypeSize(var->carpECSVariableDefType);
    varSize *= var->carpECSVariableDefArrSize > 0
        ? var->carpECSVariableDefArrSize
        : 1;

    componentSizeInBytes = var->carpECSVariableDefComponentMemOffset + varSize;

    component->carpEcsComponentDefComponentSizeInBytes = componentSizeInBytes;
    component->carpEcsComponentDefComponentAlignInBytes = highestAlign;
    return true;
}


static bool s_carp_ecs_tryParseDefinitions(CarpECSParseMemory* outMem)
{
    CARP_ASSERT_RETURN(outMem, false);

    while(*outMem->dataPtr)
    {
        if(s_carp_ecs_tryParseComponent(outMem))
        {
        }
        else if(*outMem->dataPtr != '\0')
        {
            CARP_ASSERT_RETURN(0 && "Failed to find more data.", false);
        }
    }
    return *outMem->dataPtr == '\0';
}

static bool s_carp_ecs_parseWriteOut(
    const CarpECSParseMemory* mem,
    const char* headerGuard,
    CarpECSParsedFile* outParsedFile)
{
    CARP_ASSERT_RETURN(outParsedFile, false);
    CARP_ASSERT_RETURN(mem, false);

    carp_buffer_create(1024, 16, &outParsedFile->data);

    s_carp_ecs_pushStringToBuffer("#ifndef ", &outParsedFile->data);
    s_carp_ecs_pushStringToBuffer(headerGuard, &outParsedFile->data);
    s_carp_ecs_pushStringToBuffer("\r\n#define ", &outParsedFile->data);
    s_carp_ecs_pushStringToBuffer(headerGuard, &outParsedFile->data);

    s_carp_ecs_pushStringToBuffer(
            "\r\n"
            "\r\n"
            "#include \"carplib/carpmath.h\"\r\n"
            "\r\n"
            "#include <stdalign.h> //alignof\r\n"
            "\r\n"
        , &outParsedFile->data);


    char compName[256] = { 0 };
    char varName[256] = { 0 };

    for(s32 i = 0; i < mem->carpECSComponentDefCount; ++i)
    {
        const CarpECSComponentDef* comp = mem->carpECSComponentDefs + i;
        CARP_ASSERT_RETURN(comp->carpEcsComponentDefNameLen < 256, false);

        s_carp_ecs_pushStringToBuffer("// size: ", &outParsedFile->data);
        carp_buffer_pushS32ToStr(comp->carpEcsComponentDefComponentSizeInBytes, &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer(", align: ", &outParsedFile->data);
        carp_buffer_pushS32ToStr(comp->carpEcsComponentDefComponentAlignInBytes, &outParsedFile->data);

        s_carp_ecs_pushStringToBuffer("\r\ntypedef struct ", &outParsedFile->data);
        carp_buffer_pushBuffer((const u8*)comp->carpEcsComponentDefName, comp->carpEcsComponentDefNameLen, &outParsedFile->data);


        carp_lib_memcopy(compName, comp->carpEcsComponentDefName, comp->carpEcsComponentDefNameLen);
        compName[comp->carpEcsComponentDefNameLen] = '\0';
        compName[0] = carp_lib_toLower(compName[0]);

        s_carp_ecs_pushStringToBuffer("\r\n{\r\n", &outParsedFile->data);
        const CarpECSVariableDef* var = mem->carpECSVariableDefs + comp->carpEcsComponentDefVarIndexStart;
        s32 currentOffset = 0;
        for(s32 j = 0; j < comp->carpEcsComponentDefVarAmount; ++j)
        {
            CARP_ASSERT_RETURN(var->carpECSVariableDefNameLen < 256, false);

            carp_lib_memcopy(varName, var->carpECSVariableDefName, var->carpECSVariableDefNameLen);
            varName[var->carpECSVariableDefNameLen] = '\0';
            varName[0] = carp_lib_toUpper(varName[0]);

            s_carp_ecs_pushStringToBuffer("    ", &outParsedFile->data);
            s_carp_ecs_pushStringToBuffer(s_carp_ecs_getVariableDefType(var->carpECSVariableDefType), &outParsedFile->data);
            s_carp_ecs_pushStringToBuffer(" ", &outParsedFile->data);
            carp_buffer_pushBuffer((const u8*)compName, comp->carpEcsComponentDefNameLen, &outParsedFile->data);
            carp_buffer_pushBuffer((const u8*)varName, var->carpECSVariableDefNameLen, &outParsedFile->data);
            if(var->carpECSVariableDefArrSize > 0)
            {
                s_carp_ecs_pushStringToBuffer("[", &outParsedFile->data);
                carp_buffer_pushS32ToStr(var->carpECSVariableDefArrSize, &outParsedFile->data);
                s_carp_ecs_pushStringToBuffer("]", &outParsedFile->data);

            }
            s_carp_ecs_pushStringToBuffer("; // offset: ", &outParsedFile->data);
            carp_buffer_pushS32ToStr(var->carpECSVariableDefComponentMemOffset, &outParsedFile->data);
            s_carp_ecs_pushStringToBuffer(", size: ", &outParsedFile->data);
            s32 varSize = s_carp_ecs_getVariableDefTypeSize(var->carpECSVariableDefType);
            varSize *= var->carpECSVariableDefArrSize > 0 ? var->carpECSVariableDefArrSize : 1;
            carp_buffer_pushS32ToStr(varSize, &outParsedFile->data);
            s_carp_ecs_pushStringToBuffer(", alignment: ", &outParsedFile->data);
            s32 varAlignment = s_carp_ecs_getVariableDefTypeAlignSize(var->carpECSVariableDefType);
            carp_buffer_pushS32ToStr(varAlignment, &outParsedFile->data);
            s_carp_ecs_pushStringToBuffer(", padding: ", &outParsedFile->data);
            s32 paddingAmnt = var->carpECSVariableDefComponentMemOffset - currentOffset;
            carp_buffer_pushS32ToStr(paddingAmnt, &outParsedFile->data);

            s_carp_ecs_pushStringToBuffer("\r\n", &outParsedFile->data);

            currentOffset = var->carpECSVariableDefComponentMemOffset + varSize;
            ++var;
        }
        s_carp_ecs_pushStringToBuffer("} ", &outParsedFile->data);
        carp_buffer_pushBuffer((const u8*)comp->carpEcsComponentDefName, comp->carpEcsComponentDefNameLen, &outParsedFile->data);

        s_carp_ecs_pushStringToBuffer(";\r\n", &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer("static_assert(sizeof(", &outParsedFile->data);
        carp_buffer_pushBuffer((const u8*)comp->carpEcsComponentDefName, comp->carpEcsComponentDefNameLen, &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer(") == ", &outParsedFile->data);
        carp_buffer_pushS32ToStr(comp->carpEcsComponentDefComponentSizeInBytes, &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer(", \"size not matching!\");\r\n", &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer("static_assert(alignof(", &outParsedFile->data);
        carp_buffer_pushBuffer((const u8*)comp->carpEcsComponentDefName, comp->carpEcsComponentDefNameLen, &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer(") == ", &outParsedFile->data);
        carp_buffer_pushS32ToStr(comp->carpEcsComponentDefComponentAlignInBytes, &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer(", \"align not matching!\");\r\n", &outParsedFile->data);
        s_carp_ecs_pushStringToBuffer("\r\n", &outParsedFile->data);

    }
    s_carp_ecs_pushStringToBuffer("#endif // ", &outParsedFile->data);
    s_carp_ecs_pushStringToBuffer(headerGuard, &outParsedFile->data);
    s_carp_ecs_pushStringToBuffer("\r\n\r\n\0", &outParsedFile->data);


    return true;
}

CARP_FN bool carp_ecs_parseEcsData(const char* data, const char* headerGuard, CarpECSParsedFile* outParsedFile)
{
    CARP_ASSERT_RETURN(data, false);
    CARP_ASSERT_RETURN(outParsedFile, false);

    CarpECSParseMemory ecsParseMem = { 0 };
    ecsParseMem.carpECSComponentDefs = carp_lib_calloc(sizeof(CarpECSComponentDef), CarpECSComponentDefsMaxAmount);
    ecsParseMem.carpECSVariableDefs = carp_lib_calloc(sizeof(CarpECSVariableDef), CarpECSVariableDefMaxAmount);

    ecsParseMem.dataPtrStart = data;
    ecsParseMem.dataPtr = data;
    bool result = s_carp_ecs_tryParseDefinitions(&ecsParseMem);

    if(result)
    {

        for(s32 i = 0; i < ecsParseMem.carpECSComponentDefCount; ++i)
        {
            //s_carp_ecs_printComponent(i, &ecsParseMem);
        }
        result = s_carp_ecs_parseWriteOut(&ecsParseMem, headerGuard, outParsedFile);
        //s_carp_ecs_printBuffer(outParsedFile);
    }
    //outParsedFile->data.

    carp_lib_free(ecsParseMem.carpECSComponentDefs);
    carp_lib_free(ecsParseMem.carpECSVariableDefs);

    CARP_ASSERT_RETURN(result, false);

    return result;
}



