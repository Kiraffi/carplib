#include "carplib.h"

#include "carpassert.h"

#include <stdlib.h>
#include <string.h>

CARP_FN s64 carp_lib_strtoll(const char* src, const char** end, size_t radix)
{
    return strtoll(src, (char**)end, radix);
}
CARP_FN f64 carp_lib_strtod(const char* src, const char** end)
{
    return strtod(src, (char**)end);
}

CARP_FN void* carp_lib_calloc(size_t cnt, size_t amnt)
{
    return calloc(cnt, amnt);
}

CARP_FN void carp_lib_free(void* ptr)
{
    CARP_ASSERT_RETURN(ptr, ;);
    free(ptr);
}

CARP_FN s32 carp_lib_memcmp(const void* s1, const void* s2, size_t size)
{
    return memcmp(s1, s2, size);
}


CARP_FN void carp_lib_memset(void* dst, u8 value, size_t size)
{
    memset(dst, value, size);
}

CARP_FN void carp_lib_memcopy(void* dst, const void* src, size_t size)
{
    memcpy(dst, src, size);
}

CARP_FN void carp_lib_memmove(void* dst, const void* src, size_t size)
{
    u8* dstPtr = (u8*)dst;
    const u8* srcPtr = (const u8*)src;
    CARP_ASSERT_RETURN((dstPtr + size <= srcPtr || dstPtr >= srcPtr + size), ;);
    memmove(dst, src, size);
}
