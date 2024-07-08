#ifndef CARPOGL_HH
#define CARPOGL_HH

#include "carptype.h"

typedef struct CarpOGLBuffer
{
    u32 carp_OGLBuffer_handle;
    u32 carp_OGLBuffer_size;

} CarpOGLBuffer;


CARP_FN bool carp_ogl_createBuffer(u32 size, const void* cpuData, u64 bufferCreateFlags,
    CarpOGLBuffer* outBuffer);
CARP_FN bool carp_ogl_destroyBuffer(CarpOGLBuffer* buffer);
CARP_FN bool carp_ogl_bindBuffer(const CarpOGLBuffer* buffer, s32 slot, u32 bufferType);
CARP_FN bool carp_ogl_updateBuffer(const CarpOGLBuffer* buffer, u32 offset,
    const void* cpuData, u32 cpuDataSize);


#endif //CARPOGL_HH