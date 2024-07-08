#ifndef CARPLANG_HH
#define CARPLANG_HH

#include "carpbuffer.h"

typedef struct CarpSpecialBuffer
{
    u8* CarpSpecialBufferData;
    s32 CarpSpecialBufferCapacity;
    s32 CarpSpecialBufferSize;

} CarpSpecialBuffer;

CARP_FN bool carp_lang_compileToSpecialBuffer(
    const char* input,
    int32_t inputLen,
    CarpSpecialBuffer* outBuffer);

CARP_FN void carp_lang_deleteSpecialBuffer(CarpSpecialBuffer* buffer);


#endif //CARPLANG_HH
