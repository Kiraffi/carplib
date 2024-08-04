#ifndef CARPBUFFER_HH
#define CARPBUFFER_HH

#include "carptype.h"

typedef struct CarpBuffer
{
    // this is aligned pointer
    u8* carp_buffer_data;
    u8* carp_buffer_actual_pointer;
    s32 carp_buffer_capacity;
    s32 carp_buffer_size;
    s32 carp_buffer_alignment;
} CarpBuffer;

CARP_FN bool carp_buffer_create(s32 size, s32 alignment, CarpBuffer* outBuffer);
CARP_FN bool carp_buffer_free(CarpBuffer* outBuffer);



CARP_FN bool carp_buffer_pushBuffer(const u8* pushBuffer, s32 pushBufferSize, CarpBuffer* outBuffer);
CARP_FN bool carp_buffer_pushS32ToStr(s32 value, CarpBuffer* outBuffer);
CARP_FN bool carp_buffer_pushS32(s32 value, CarpBuffer* outBuffer);
CARP_FN bool carp_buffer_popS32(s32* outValue, CarpBuffer* outBuffer);



#endif //CARPBUFFER_HH

