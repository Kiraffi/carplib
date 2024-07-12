#include "carpbuffer.h"

#include "carpassert.h"
#include "carplib.h"

// Note, aligned_alloc seemed to only use 32bit version returning int instead of void*.

static bool carp_buffer_createAlignedBuffer(s32 size, s32 alignment, CarpBuffer* outBuffer)
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
    CARP_ASSERT_RETURN(buffer->carp_buffer_data, false);
    carp_lib_free(buffer->carp_buffer_actual_pointer);
    CarpBuffer tmp = {0};
    *buffer = tmp;
    return true;
}


CARP_FN bool carp_buffer_pushBuffer(CarpBuffer* buffer, const u8* pushBuffer, s32 pushBufferSize)
{
    CARP_ASSERT_RETURN(buffer, false);
    CARP_ASSERT_RETURN(buffer->carp_buffer_data, false);

    if(buffer->carp_buffer_size + pushBufferSize >= buffer->carp_buffer_capacity)
    {
        s32 oldSize = buffer->carp_buffer_size;
        s32 capacity = buffer->carp_buffer_capacity;
        if(capacity < 32)
            capacity = 32;

        while(capacity < oldSize + pushBufferSize)
            capacity *= 2;
        CarpBuffer newBuffer = {0};
        CARP_ASSERT_RETURN(carp_buffer_createAlignedBuffer(
            capacity,
            buffer->carp_buffer_alignment,
            &newBuffer
        ), false);
        newBuffer.carp_buffer_size = oldSize;

        carp_lib_memcopy(
            newBuffer.carp_buffer_data, buffer->carp_buffer_data, oldSize);
        carp_buffer_free(buffer);

        *buffer = newBuffer;
    }
    carp_lib_memcopy(
        buffer->carp_buffer_data + buffer->carp_buffer_size, pushBuffer, pushBufferSize);
    buffer->carp_buffer_size += pushBufferSize;
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
    CARP_ASSERT_RETURN(buffer->carp_buffer_data, false); \
    CARP_ASSERT_RETURN(buffer->carp_buffer_size < sizeof(helperFnType), false); \
    buffer->carp_buffer_size -= sizeof(helperFnType); \
    *outValue = *((helperFnType*)(buffer->carp_buffer_data + buffer->carp_buffer_size)); \
    return true; \
}

BufferPushHelper(S32, s32)
BufferPopHelper(S32, s32)

