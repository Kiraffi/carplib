#include "carpbuffer.h"

#include "carpassert.h"
#include "carplib.h"

// Note, aligned_alloc seemed to only use 32bit version returning int instead of void*.

static bool s_carp_buffer_createAlignedBuffer(s32 size, s32 alignment, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(outBuffer->carp_buffer_actual_pointer == NULL, false);
    CARP_ASSERT_RETURN(((alignment - 1) & alignment) == 0, false);
    CARP_ASSERT_RETURN(alignment >= 16, false);

    u8* mem = (u8*)carp_lib_calloc(size + alignment, alignment);
    outBuffer->carp_buffer_actual_pointer = mem;

    intptr_t alignmentPtrMinusOne = alignment - 1;
    intptr_t memPtr = (intptr_t)mem;

    outBuffer->carp_buffer_data = (u8*)((memPtr + alignmentPtrMinusOne) & (~alignmentPtrMinusOne));
    outBuffer->carp_buffer_alignment = alignment;
    outBuffer->carp_buffer_capacity = size;
    outBuffer->carp_buffer_size = 0;
    return true;
}

CARP_FN bool carp_buffer_create(s32 size, s32 alignment, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(alignment >= 16, false);
    CARP_ASSERT_RETURN(size >= 32 , false);
    CARP_ASSERT_RETURN(outBuffer->carp_buffer_data == NULL, false);
    
    size = (size + alignment - 1) & (~(alignment - 1)); 
    // only accept power of 2 reserves
    //CARP_ASSERT_RETURN(((size - 1) & size) == 0, false);
    // only accept power of 2 alignments
    CARP_ASSERT_RETURN(((alignment - 1) & alignment) == 0, false);

    CARP_ASSERT_RETURN(s_carp_buffer_createAlignedBuffer(size, alignment, outBuffer), false);
    return true;
}

CARP_FN bool carp_buffer_free(CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(outBuffer->carp_buffer_data, false);
    carp_lib_free(outBuffer->carp_buffer_actual_pointer);
    CarpBuffer tmp = {0};
    *outBuffer = tmp;
    return true;
}


CARP_FN bool carp_buffer_pushBuffer(const u8* pushBuffer, s32 pushBufferSize, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(outBuffer->carp_buffer_data, false);

    if(outBuffer->carp_buffer_size + pushBufferSize >= outBuffer->carp_buffer_capacity)
    {
        s32 oldSize = outBuffer->carp_buffer_size;
        s32 capacity = outBuffer->carp_buffer_capacity;
        if(capacity < 32)
            capacity = 32;

        while(capacity < oldSize + pushBufferSize)
            capacity *= 2;
        CarpBuffer newBuffer = {0};
        CARP_ASSERT_RETURN(s_carp_buffer_createAlignedBuffer(
            capacity,
            outBuffer->carp_buffer_alignment,
            &newBuffer
        ), false);
        newBuffer.carp_buffer_size = oldSize;

        carp_lib_memcopy(
            newBuffer.carp_buffer_data, outBuffer->carp_buffer_data, oldSize);
        carp_buffer_free(outBuffer);

        *outBuffer = newBuffer;
    }
    carp_lib_memcopy(
        outBuffer->carp_buffer_data + outBuffer->carp_buffer_size, pushBuffer, pushBufferSize);
    outBuffer->carp_buffer_size += pushBufferSize;
    return true;
}
CARP_FN bool carp_buffer_pushS32ToStr(s32 value, CarpBuffer* outBuffer)
{
    u8 tmpBuff[64] = { 0 };
    s32 len = snprintf((char*)tmpBuff, 63, "%d", value);
    return carp_buffer_pushBuffer(tmpBuff, len, outBuffer);

}


#define BufferPushHelper(helperFnName, helperFnType) \
CARP_FN bool carp_buffer_push##helperFnName (helperFnType value, CarpBuffer* outBuffer) \
{ \
    return carp_buffer_pushBuffer((const u8*)(&value), sizeof(helperFnType), outBuffer); \
}

#define BufferPopHelper(helperFnName, helperFnType) \
CARP_FN bool carp_buffer_pop##helperFnName (helperFnType* outValue, CarpBuffer* outBuffer) \
{ \
    CARP_ASSERT_RETURN(outBuffer, false); \
    CARP_ASSERT_RETURN(outValue, false); \
    CARP_ASSERT_RETURN(outBuffer->carp_buffer_data, false); \
    CARP_ASSERT_RETURN(outBuffer->carp_buffer_size < sizeof(helperFnType), false); \
    outBuffer->carp_buffer_size -= sizeof(helperFnType); \
    *outValue = *((helperFnType*)(outBuffer->carp_buffer_data + outBuffer->carp_buffer_size)); \
    return true; \
}

BufferPushHelper(S32, s32)
BufferPopHelper(S32, s32)

