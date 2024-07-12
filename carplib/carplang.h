#ifndef CARPLANG_HH
#define CARPLANG_HH

#include "carpbuffer.h"

typedef struct CarpSpecialBuffer
{
    u8* carp_specialBuffer_data;
    s32 carp_specialBuffer_capacity;
    s32 carp_specialBuffer_size;

} CarpSpecialBuffer;

typedef struct CarpLangFns
{

}CarpLangFns;

CARP_FN bool carp_lang_compileToSpecialBuffer(
    const char* input,
    s32 inputLen,
    CarpSpecialBuffer* outBuffer);

CARP_FN void carp_lang_deleteSpecialBuffer(CarpSpecialBuffer* buffer);


#endif //CARPLANG_HH
