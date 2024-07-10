#ifndef CARPLIB_HH
#define CARPLIB_HH

#include "carptype.h"

CARP_FN s64 carp_lib_strtoll(const char* src, const char** end, size_t radix);
CARP_FN f64 carp_lib_strtod(const char* src, const char** end);


CARP_FN void* carp_lib_calloc(size_t cnt, size_t amnt);
CARP_FN void carp_lib_free(void* ptr);

CARP_FN s32 carp_lib_memcmp(const void* s1, const void* s2, size_t size);

CARP_FN void carp_lib_memset(void* dst, u8 value, size_t size);
CARP_FN void carp_lib_memcopy(void* dst, const void* src, size_t size);
CARP_FN void carp_lib_memmove(void* dst, const void* src, size_t size);


#endif // CARPLIB_HH


