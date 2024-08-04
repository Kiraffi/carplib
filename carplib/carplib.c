#include "carplib.h"

#include "carpassert.h"

#include <stdlib.h>
#include <string.h>

static bool sCarp_lib_loadFileHelper(FILE* f, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(f, false);
    CARP_ASSERT_RETURN(outBuffer, false);

    CARP_ASSERT_RETURN(fseek(f, 0L, SEEK_END) == 0, false);

    long bufferSize = ftell(f) + 1;
    CARP_ASSERT_RETURN(bufferSize > 0, false);

    carp_buffer_create(bufferSize, 16, outBuffer);
        
    CARP_ASSERT_RETURN(fseek(f, 0L, SEEK_SET) == 0, false);

    size_t newLen = fread(outBuffer->carp_buffer_data, sizeof(char), bufferSize, f);
    CARP_ASSERT_RETURN(ferror(f) == 0, false);

    return true;
}

CARP_FN bool carp_lib_loadFile(const char* fileName, CarpBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(fileName, false);
    CARP_ASSERT_RETURN(outBuffer, false);
    FILE* f = NULL;
    
    #if CARP_WIN32
    fopen_s(&f, fileName, "rb");
    #else
    fopen(fileName, "rb");
    #endif
    CARP_ASSERT_RETURN(f, false);
    bool result = sCarp_lib_loadFileHelper(f, outBuffer);

    fclose(f);
    if(!result)
    {
        carp_buffer_free(outBuffer);
    }

    CARP_ASSERT_RETURN(result, false);
    return result;
}

CARP_FN bool carp_lib_skipWhiteSpace(const char** pos)
{
    CARP_ASSERT_RETURN(pos, false);
    while(**pos)
    {
        switch(**pos)
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            default:
                return true;
        }
        ++(*pos);
    }
    return true;
}

CARP_FN bool carp_lib_skipUntilWhiteSpace(const char** pos)
{
    CARP_ASSERT_RETURN(pos, false);
    while(**pos)
    {
        switch(**pos)
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                return true;
            default:
                break;
        }
        ++(*pos);
    }
    return true;
}


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

CARP_FN bool carp_lib_isnumber(char c)
{
    return (c >= '0' && c <= '9');
}
