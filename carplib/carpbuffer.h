#ifndef CARPBUFFER_HH
#define CARPBUFFER_HH

#include "carptype.h"

typedef struct CarpBuffer
{
    // this is aligned pointer
    u8* carpBufferData;
    u8* carpBufferActualPointer;
    s32 carpBufferCapacity;
    s32 carpBufferSize;
    s32 carpBufferAlignment;
} CarpBuffer;

CARP_FN bool carp_buffer_create(s32 size, s32 alignment, CarpBuffer* outBuffer);
CARP_FN bool carp_buffer_free(CarpBuffer* buffer);



CARP_FN bool carp_buffer_pushBuffer(
    CarpBuffer* buffer, const u8* pushBuffer, s32 pushBufferSize);
CARP_FN bool carp_buffer_pushS32(CarpBuffer* buffer, s32 value);
CARP_FN bool carp_buffer_popS32(CarpBuffer* buffer, s32* outValue);



#endif //CARPBUFFER_HH

