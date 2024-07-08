#include "carpbuffer.h"

#include "carpassert.h"

#include <stdlib.h>
#include <string.h>


CARP_FN bool carp_buffer_create(int32_t size, int32_t alignment, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(alignment > 0 , false);
    CARP_ASSERT_RETURN(size >= 32 , false);
    CARP_ASSERT_RETURN(outBuffer->carpBufferData == NULL, false);
    // only accept power of 2 reserves
    CARP_ASSERT_RETURN(((size - 1) & size) == 0, false);
    // only accept power of 2 alignments
    CARP_ASSERT_RETURN(((alignment - 1) & alignment) == 0, false);

    outBuffer->carpBufferData = aligned_alloc(alignment, size);
    memset(outBuffer->carpBufferData, 0, size);

    outBuffer->carpBufferCapacity = size;
    outBuffer->carpBufferAlignment = alignment;
    outBuffer->carpBufferSize = 0;

    return true;
}

CARP_FN bool carp_buffer_free(CarpBuffer* buffer)
{
    CARP_ASSERT_RETURN(buffer, false);
    CARP_ASSERT_RETURN(buffer->carpBufferData, false);
    free(buffer->carpBufferData);
    buffer->carpBufferData = NULL;
    return true;
}


CARP_FN bool carp_buffer_pushBuffer(
    CarpBuffer* buffer,
    const u8* pushBuffer,
    s32 pushBufferSize)
{
    CARP_ASSERT_RETURN(buffer, false);
    CARP_ASSERT_RETURN(buffer->carpBufferData, false);

    if(buffer->carpBufferSize + pushBufferSize >= buffer->carpBufferCapacity)
    {
        if(buffer->carpBufferCapacity < 32)
            buffer->carpBufferCapacity = 32;

        while(buffer->carpBufferCapacity < buffer->carpBufferSize + pushBufferSize)
            buffer->carpBufferCapacity *= 2;
        u8* newBuffer = (u8*)aligned_alloc(buffer->carpBufferAlignment, buffer->carpBufferCapacity);
        memcpy(newBuffer, buffer->carpBufferData, buffer->carpBufferSize);
        memset(newBuffer + buffer->carpBufferSize, 0, buffer->carpBufferCapacity - buffer->carpBufferSize);
        free(buffer->carpBufferData);
        buffer->carpBufferData = newBuffer;
    }
    memcpy(buffer->carpBufferData + buffer->carpBufferSize, pushBuffer, pushBufferSize);
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
    CARP_ASSERT_RETURN(buffer->carpBufferData, false); \
    CARP_ASSERT_RETURN(buffer->carpBufferSize < sizeof(helperFnType), false); \
    *outValue = *((helperFnType*)(buffer->carpBufferData + buffer->carpBufferSize - sizeof(helperFnType))); \
    return true; \
}

BufferPushHelper(S32, s32);
BufferPopHelper(S32, s32);

