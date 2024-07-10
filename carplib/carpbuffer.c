#include "carpbuffer.h"

#include "carpassert.h"
#include "carplib.h"

// Note, aligned_alloc seemed to only use 32bit version returning int instead of void*.

static bool carp_buffer_createAlignedBuffer(s32 size, s32 alignment, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(outBuffer->carpBufferActualPointer == NULL, false);
    CARP_ASSERT_RETURN(((alignment - 1) & alignment) == 0, false);
    CARP_ASSERT_RETURN(alignment >= 16, false);

    u8* mem = (u8*)carp_lib_calloc(size + alignment, alignment);
    outBuffer->carpBufferActualPointer = mem;

    intptr_t alignmentPtrMinusOne = alignment - 1;
    intptr_t memPtr = (intptr_t)mem;

    outBuffer->carpBufferData = (u8*)((memPtr + alignmentPtrMinusOne) & (~alignmentPtrMinusOne));
    outBuffer->carpBufferAlignment = alignment;
    outBuffer->carpBufferCapacity = size;
    outBuffer->carpBufferSize = 0;
    return true;
}

CARP_FN bool carp_buffer_create(s32 size, s32 alignment, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(alignment >= 16, false);
    CARP_ASSERT_RETURN(size >= 32 , false);
    CARP_ASSERT_RETURN(outBuffer->carpBufferData == NULL, false);
    // only accept power of 2 reserves
    CARP_ASSERT_RETURN(((size - 1) & size) == 0, false);
    // only accept power of 2 alignments
    CARP_ASSERT_RETURN(((alignment - 1) & alignment) == 0, false);

    CARP_ASSERT_RETURN(carp_buffer_createAlignedBuffer(size, alignment, outBuffer), false);
    return true;
}

CARP_FN bool carp_buffer_free(CarpBuffer* buffer)
{
    CARP_ASSERT_RETURN(buffer, false);
    CARP_ASSERT_RETURN(buffer->carpBufferData, false);
    carp_lib_free(buffer->carpBufferActualPointer);
    CarpBuffer tmp = {0};
    *buffer = tmp;
    return true;
}


CARP_FN bool carp_buffer_pushBuffer(CarpBuffer* buffer, const u8* pushBuffer, s32 pushBufferSize)
{
    CARP_ASSERT_RETURN(buffer, false);
    CARP_ASSERT_RETURN(buffer->carpBufferData, false);

    if(buffer->carpBufferSize + pushBufferSize >= buffer->carpBufferCapacity)
    {
        s32 oldSize = buffer->carpBufferSize;
        s32 capacity = buffer->carpBufferCapacity;
        if(capacity < 32)
            capacity = 32;

        while(capacity < oldSize + pushBufferSize)
            capacity *= 2;
        CarpBuffer newBuffer = {0};
        CARP_ASSERT_RETURN(carp_buffer_createAlignedBuffer(
            capacity,
            buffer->carpBufferAlignment,
            &newBuffer
        ), false);
        newBuffer.carpBufferSize = oldSize;

        carp_lib_memcopy(
            newBuffer.carpBufferData, buffer->carpBufferData, oldSize);
        carp_buffer_free(buffer);

        *buffer = newBuffer;
    }
    carp_lib_memcopy(
        buffer->carpBufferData + buffer->carpBufferSize, pushBuffer, pushBufferSize);
    buffer->carpBufferSize += pushBufferSize;
    return true;
}

#define BufferPushHelper(helperFnName, helperFnType) \
CARP_FN bool carp_buffer_push##helperFnName (CarpBuffer* buffer, helperFnType value) \
{ \
    return carp_buffer_pushBuffer(buffer, (const u8*)(&value), sizeof(helperFnType)); \
}

#define BufferPopHelper(helperFnName, helperFnType) \
CARP_FN bool carp_buffer_pop##helperFnName (CarpBuffer* buffer, helperFnType* outValue) \
{ \
    CARP_ASSERT_RETURN(buffer, false); \
    CARP_ASSERT_RETURN(outValue, false); \
    CARP_ASSERT_RETURN(buffer->carpBufferData, false); \
    CARP_ASSERT_RETURN(buffer->carpBufferSize < sizeof(helperFnType), false); \
    buffer->carpBufferSize -= sizeof(helperFnType); \
    *outValue = *((helperFnType*)(buffer->carpBufferData + buffer->carpBufferSize)); \
    return true; \
}

BufferPushHelper(S32, s32)
BufferPopHelper(S32, s32)

