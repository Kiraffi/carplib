#include "carplib.h"

#include "carpassert.h"
#include "carptype.h"

#include <ctype.h> //tolower, toupper?
#include <stdlib.h>
#include <string.h>

static void s_carp_lib_fopen(FILE** f, const char* fileName, const char* mode)
{
    #if CARP_WIN32 && !CARP_TCC
        fopen_s(f, fileName, mode);
    #else
        *f = fopen(fileName, mode);
    #endif

}

static bool s_carp_lib_loadFileHelper(FILE* f, CarpBuffer* outBuffer)
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
    s_carp_lib_fopen(&f, fileName, "rb");

    CARP_ASSERT_RETURN(f, false);
    bool result = s_carp_lib_loadFileHelper(f, outBuffer);

    fclose(f);
    if(!result)
    {
        carp_buffer_free(outBuffer);
    }

    CARP_ASSERT_RETURN(result, false);
    return result;
}
CARP_FN bool carp_lib_writeFile(const char* fileName, const char* data, s32 dataLen)
{
    CARP_ASSERT_RETURN(fileName, false);
    CARP_ASSERT_RETURN(data, false);

    FILE* f = NULL;
    s_carp_lib_fopen(&f, fileName, "wb");

    CARP_ASSERT_RETURN(f, false);

    fwrite(data, 1, dataLen, f);
    s32 err = ferror(f);

    return fclose(f) == 0 && err == 0;
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
CARP_FN s64 carp_lib_strlen(const char* src)
{
    return (s64)strlen(src);
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

CARP_FN bool carp_lib_isNumber(char c)
{
    return (c >= '0' && c <= '9');
}

CARP_FN bool carp_lib_isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

CARP_FN char carp_lib_toLower(char c)
{
    return (char)tolower(c);
}
CARP_FN char carp_lib_toUpper(char c)
{
    return (char)toupper(c);
}
