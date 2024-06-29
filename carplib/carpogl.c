#include "carpogl.h"

#include "carpassert.h"
#include "carpgl.h"
#include "carplog.h"

CARP_FN bool carp_ogl_createBuffer(
    u32 size,
    const void* cpuData,
    u64 bufferCreateFlags,
    CarpOGLBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(outBuffer && "Needs non null outbuffer", false);
    CARP_ASSERT_RETURN(outBuffer->carp_OGLBuffer_handle == 0
        && "Buffer should not be initialized!",
        false);
    CARP_ASSERT_RETURN(size > 0
        && "Buffer size must be greater than 0 bytes",
        false);
    //if(bufferType != GL_ELEMENT_ARRAY_BUFFER)
    CARP_ASSERT_RETURN(size % 16 == 0
        && "Buffer size should be 16 byte size multiplier.",
        false);

    glCreateBuffers(1, &outBuffer->carp_OGLBuffer_handle);
    glNamedBufferStorage(outBuffer->carp_OGLBuffer_handle,
        size,
        cpuData,
        bufferCreateFlags);
    outBuffer->carp_OGLBuffer_size = size;
    return true;
}

CARP_FN bool carp_ogl_destroyBuffer(CarpOGLBuffer* buffer)
{
    CARP_ASSERT_RETURN(buffer && "Needs non null outbuffer", false);
    CARP_ASSERT_RETURN(buffer->carp_OGLBuffer_handle != 0
        && "Buffer should be initialized!",
        false);

    glDeleteBuffers(1, &buffer->carp_OGLBuffer_handle);
    CarpOGLBuffer emptyBuffer = {};
    (*buffer) = emptyBuffer;
    return true;
}

CARP_FN bool carp_ogl_bindBuffer(const CarpOGLBuffer* buffer, int slot, u32 bufferType)
{
    CARP_ASSERT_RETURN(buffer && "Needs non null outbuffer", false);
    CARP_ASSERT_RETURN(buffer->carp_OGLBuffer_handle != 0
        && "Buffer should be initialized!",
        false);

    glBindBufferBase(bufferType, slot, buffer->carp_OGLBuffer_handle);

    return true;
}

CARP_FN bool carp_ogl_updateBuffer(
    const CarpOGLBuffer* buffer,
    u32 offset,
    const void* cpuData,
    u32 cpuDataSize)
{
    CARP_ASSERT_RETURN(buffer && "Needs non null outbuffer", false);
    CARP_ASSERT_RETURN(buffer->carp_OGLBuffer_handle != 0
        && "Buffer should be initialized!",
        false);
    CARP_ASSERT_RETURN(cpuData
        && "Cpu data pointer needs to be nonnull!",
        false);

    CARP_ASSERT_RETURN(cpuDataSize > 0
        && "Data size should be over 0 bytes.",
        false);

    CARP_ASSERT_RETURN(cpuDataSize % 16 == 0
        && "Data size should be divisable by 16 bytes.",
        false);

    CARP_ASSERT_RETURN(offset >= 0
        && "Offset should not be negative.",
        false);


    CARP_ASSERT_RETURN(offset + cpuDataSize <= buffer->carp_OGLBuffer_size
        && "Offset + cpudatasize needs to be smaller than buffer size.",
        false);

    glNamedBufferSubData(buffer->carp_OGLBuffer_handle,
        offset,
        cpuDataSize,
        cpuData);

    return true;

}
