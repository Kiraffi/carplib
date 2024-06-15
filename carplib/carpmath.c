#include "carpmath.h"

#include <math.h>
#include <stdio.h>
#include <string.h> // memset?

// for memory
// "movl $0x3F800000, 0xc(%%rax)\n\t" // set w to 1.0f
#if (__TINYC__) && _WIN32
#define USE_ASM_WINDOWS 1
#elif (__TINYC__)
#define USE_ASM_LINUX 1
#endif

#if USE_ASM_LINUX || USE_ASM_WINDOWS
#define NO_INLINE_FN __attribute__ ((noinline))
#else
#define NO_INLINE_FN
#endif

//////////////////////////////
//
// f32 begins
//
///////////////////////////////

NO_INLINE_FN
f32 carp_math_abs_f(f32 f)
{
    f32 result;
#if USE_ASM_LINUX || USE_ASM_WINDOWS
    static const CarpV3A allButHighestBit = {.intArr = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff}};

    __asm__ volatile  (
        "pand %[v4], %%xmm0\n\t"
        "movd %%xmm0, %[res]\n\t"

        : [res] "+m"(result)
        : [v4] "m"(allButHighestBit), [f]"m"(f)
    );
#else
    result = fabsf(f);
#endif
    return result;
}



NO_INLINE_FN
f32 carp_math_min_f_f(f32 a, f32 b)
{
    f32 result;
#if USE_ASM_LINUX || USE_ASM_WINDOWS

    __asm__ volatile  (
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    result = a < b ? a : b;
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_max_f_f(f32 a, f32 b)
{
    f32 result;
#if USE_ASM_LINUX || USE_ASM_WINDOWS

    __asm__ volatile  (
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    result = a > b ? a : b;
#endif

    return result;
}










//////////////////////////////
//
// V2 begins
//
///////////////////////////////








NO_INLINE_FN
void carp_math_broadcast_v2(f32 f, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : "+m" (outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, (%%rdi)\n\t"
        : "+m" (outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "x"(f)
        #endif
    );
#else
    outV2->x = outV2->y = f;

#endif
}

NO_INLINE_FN
void carp_math_neg_v2(const CarpV2* a, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    static const CarpV2 negZeroV2 = { -0.0f, -0.0f };
    __asm__ volatile  (
        "movq %[zerov2], %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV2)
        : [zerov2]"m"(negZeroV2), "m"(a)
        #if defined(__clang__) || defined(__GNUC__)
        #endif
    );
#elif USE_ASM_LINUX
    static const CarpV2 negZeroV2 = { -0.0f, -0.0f };
    __asm__ volatile  (
        "movq %[zerov2], %%xmm1\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rsi)\n\t"
        : [c] "+m"(outV2)
        : [zerov2]"m"(negZeroV2), "m"(a)
        #if defined(__clang__) || defined(__GNUC__)
        #endif
    );
#else
    outV2->x = -a->x;
    outV2->y = -a->y;

#endif
}
NO_INLINE_FN
void carp_math_normalize_v2(const CarpV2* a, CarpV2* outV2)
{
    f32 d2 = carp_math_sqrLen_v2(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV2 result = {0};
        *outV2 = result;
        return;
    }
    d2 = sqrtf(d2);
    outV2->x = a->x / d2;
    outV2->y = a->y / d2;
}

NO_INLINE_FN
void carp_math_lerp_v2(const CarpV2* a, const CarpV2* b, f32 t, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm2, %%xmm2\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "subps %%xmm0, %%xmm1\n\t"
        "mulps %%xmm1, %%xmm2\n\t"
        "addps %%xmm2, %%xmm0\n\t"
        "movq %%xmm0, (%%r9)\n\t"
        : [c] "=m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b), "m"(t)
        #endif

    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm2\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "subps %%xmm2, %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "addps %%xmm2, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "=m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b), "m"(t)
        #endif

    );
#else
    outV2->x = a->x + (b->x - a->x) * t;
    outV2->y = a->y + (b->y - a->y) * t;
#endif
}

NO_INLINE_FN
void carp_math_add_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );

#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV2->x = a->x + b->x;
    outV2->y = a->y + b->y;
#endif
}

NO_INLINE_FN
void carp_math_add_v2_f(const CarpV2* a, f32 f, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm1\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rsi)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#else
    outV2->x = a->x + f;
    outV2->y = a->y + f;
#endif
}



NO_INLINE_FN
void carp_math_sub_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV2->x = a->x - b->x;
    outV2->y = a->y - b->y;
#endif
}
NO_INLINE_FN
void carp_math_sub_v2_f(const CarpV2* a, f32 f, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "subps %%xmm0, %%xmm1\n\t"
        "movq %%xmm1, (%%rsi)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif

    );
#else
    outV2->x = a->x - f;
    outV2->y = a->y - f;
#endif
}


NO_INLINE_FN
void carp_math_mul_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV2->x = a->x * b->x;
    outV2->y = a->y * b->y;
#endif
}
NO_INLINE_FN
void carp_math_mul_v2_f(const CarpV2* a, f32 f, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rsi)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#else
    outV2->x = a->x * f;
    outV2->y = a->y * f;
#endif
}


NO_INLINE_FN
void carp_math_div_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV2->x = a->x / b->x;
    outV2->y = a->y / b->y;
#endif
}
NO_INLINE_FN
void carp_math_div_v2_f(const CarpV2* a, f32 f, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "divps %%xmm0, %%xmm1\n\t"
        "movq %%xmm1, (%%rsi)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#else
    outV2->x = a->x / f;
    outV2->y = a->y / f;
#endif
}
NO_INLINE_FN
void carp_math_min_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m" (outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "+m" (outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV2->x = carp_math_min_f_f(a->x, b->x);
    outV2->y = carp_math_min_f_f(a->y, b->y);
#endif
}
NO_INLINE_FN
void carp_math_max_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m" (outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rdx)\n\t"
        : [c] "+m" (outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV2->x = carp_math_max_f_f(a->x, b->x);
    outV2->y = carp_math_max_f_f(a->y, b->y);
#endif
}
NO_INLINE_FN
f32 carp_math_dot_v2(const CarpV2* a, const CarpV2* b)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );

    result = tmpstack.x + tmpstack.y;
#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );

    result = tmpstack.x + tmpstack.y;
#else
    result = a->x * b->x + a->y * b->y;
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_min_v2(const CarpV2* a)
{
    f32 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movd (%%rcx), %%xmm0\n\t"
        "movd +4(%%rcx), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#else
    result = carp_math_min_f_f(a->x, a->y);
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_max_v2(const CarpV2* a)
{
     f32 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movd (%%rcx), %%xmm0\n\t"
        "movd +4(%%rcx), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#else
    result = carp_math_max_f_f(a->x, a->y);
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_sqrLen_v2(const CarpV2* a)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rcx), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
    result = tmpstack.x + tmpstack.y;

#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rdi), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
    result = tmpstack.x + tmpstack.y;

#else
    result = a->x * a->x + a->y * a->y;
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_len_v2(const CarpV2* a)
{
    return sqrtf(carp_math_sqrLen_v2(a));
}















//////////////////////////////
//
// V3a begins
//
///////////////////////////////


NO_INLINE_FN
void carp_math_zero_v3(CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "pxor %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rcx)\n\t"
        : [c] "=m"(outV3)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "pxor %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rdi)\n\t"
        : [c] "=m"(outV3)
    );
#else
    outV3->simdv3a = _mm_setzero_ps();
#endif
}


NO_INLINE_FN
void carp_math_broadcast_v3(f32 f, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        //"movl $0, +0xc%[c]\n\t"
        : [c] "=m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rdi)\n\t"
        //"movl $0, +0xc%[c]\n\t"
        : [c] "=m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "x"(f)
        #endif
    );

#else
    outV3->simdv3a = _mm_set_ps(f, f, f, f);
#endif
}




NO_INLINE_FN
void carp_math_neg_v3(const CarpV3A* a, CarpV3A* outV3)
{

    static const CarpV3A neg = { -0.0f, -0.0f, -0.0f, -0.0f };
#if USE_ASM_WINDOWS
    // this was using rdx as a since originally there was return value, and
    // return value was written into rax and rcx
    __asm__ volatile  (
        "movups %[negin], %%xmm0\n\t"
        "pxor (%%rcx), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV3)
        : [negin]"m"(neg), "m"(a)
        #if defined(__clang__) || defined(__GNUC__)
        #endif
    );

#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups %[negin], %%xmm1\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, (%%rsi)\n\t"
        : [c] "+m"(outV3)
        : [negin]"m"(neg), "m"(a)
        #if defined(__clang__) || defined(__GNUC__)
        #endif
    );
#else
    outV3->simdv3a = _mm_xor_ps(a->simdv3a, neg.simdv3a);
#endif
}

NO_INLINE_FN
void carp_math_normalize_v3(const CarpV3A* a, CarpV3A* outV3)
{
    f32 d2 = carp_math_sqrLen_v3(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV3A result = {0};
        *outV3 = result;
        return;
    }
    d2 = sqrtf(d2);
    carp_math_div_v3_f(a, d2, outV3);
    outV3->w = 0.0f;
}
NO_INLINE_FN
void carp_math_lerp_v3(const CarpV3A* a, const CarpV3A* b, f32 t, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm2, %%xmm2\n\t"
        "punpckldq %%xmm2, %%xmm2\n\t"
        "movups (%%rdx), %%xmm0\n\t"
        "subps (%%rcx), %%xmm0\n\t"
        "mulps %%xmm2, %%xmm0\n\t"
        "addps (%%rcx), %%xmm0\n\t"
        "movups %%xmm0, (%%r9)\n\t"
        : [c] "=m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b), "m"(t)
        #endif
    );

#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups (%%rsi), %%xmm1\n\t"
        "subps (%%rdi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "addps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        : [c] "=m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b), "m"(t)
        #endif
    );
#else
    carp_math_sub_v3_v3(b, a, outV3);
    carp_math_mul_v3_f(outV3, t, outV3);
    carp_math_add_v3_v3(a, outV3, outV3);
#endif
}


NO_INLINE_FN
void carp_math_add_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        //"movups (%%rcx), %%xmm0\n\t"
        "movups (%%rcx), %%xmm0\n\t"
        "addps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "addps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV3->simdv3a = _mm_add_ps(a->simdv3a, b->simdv3a);
#endif
}
NO_INLINE_FN
void carp_math_add_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "addps (%%rcx), %%xmm1\n\t"
        "movups %%xmm1, (%%r8)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "addps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, (%%rsi)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#else
    outV3->simdv3a = _mm_add_ps(a->simdv3a, _mm_set_ps1(f));
#endif
}



NO_INLINE_FN
void carp_math_sub_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rcx), %%xmm0\n\t"
        "subps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "subps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV3->simdv3a = _mm_sub_ps(a->simdv3a, b->simdv3a);
#endif
}
NO_INLINE_FN
void carp_math_sub_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movups (%%rcx), %%xmm0\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups (%%rdi), %%xmm1\n\t"
        "subps %%xmm0, %%xmm1\n\t"
        "movups %%xmm1, (%%rsi)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#else
    outV3->simdv3a = _mm_sub_ps(a->simdv3a, _mm_set1_ps(f));
#endif
}
NO_INLINE_FN
void carp_math_mul_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rcx), %%xmm0\n\t"
        "mulps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0,(%%r8)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV3->simdv3a = _mm_mul_ps(a->simdv3a, b->simdv3a);
#endif
}
NO_INLINE_FN
void carp_math_mul_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "mulps (%%rcx), %%xmm1\n\t"
        "movups %%xmm1, (%%r8)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "mulps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, (%%rsi)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#else
    outV3->simdv3a = _mm_mul_ps(a->simdv3a, _mm_set1_ps(f));
#endif
}

NO_INLINE_FN
void carp_math_div_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rcx), %%xmm0\n\t"
        "divps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        //"movl $0, 0xc(%%r8)\n\t" // set w to 0.0f
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "divps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        //"movl $0, 0xc(%%rdx)\n\t" // set w to 0.0f
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV3->x = a->x / b->x;
    outV3->y = a->y / b->y;
    outV3->z = a->z / b->z;
    outV3->w = a->w / b->w;
    //outV3->w = 0.0f;
#endif
#if defined(__GNUC__)
    //outV3->w = 0.0f;
#endif
}
NO_INLINE_FN
void carp_math_div_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movups (%%rcx), %%xmm0\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        //"movl $0, +0xc(%%r8)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups (%%rdi), %%xmm1\n\t"
        "divps %%xmm0, %%xmm1\n\t"
        "movups %%xmm1, (%%rsi)\n\t"
        //"movl $0, +0xc(%%rsi)\n\t"
        : [c] "+m"(outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "x"(f)
        #endif
    );
#else
    outV3->x = a->x / f;
    outV3->y = a->y / f;
    outV3->z = a->z / f;
    outV3->w = a->w / f;
    //outV3->w = 0.0f;
#endif
#if defined(__GNUC__)
    //outV3->w = 0.0f;
#endif
}
NO_INLINE_FN
void carp_math_min_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rcx), %%xmm0\n\t"
        "minps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        : [c] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "minps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        : [c] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV3->simdv3a = _mm_min_ps(a->simdv3a, b->simdv3a);
#endif
}

NO_INLINE_FN
void carp_math_max_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rcx), %%xmm0\n\t"
        "maxps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        : [c] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "maxps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, (%%rdx)\n\t"
        : [c] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );
#else
    outV3->simdv3a = _mm_max_ps(a->simdv3a, b->simdv3a);
#endif
}

NO_INLINE_FN
void carp_math_cross_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
    outV3->x = a->y * b->z - a->z * b->y;
    outV3->y = a->z * b->x - a->x * b->z;
    outV3->z = a->x * b->y - a->y * b->x;
    outV3->w = 0.0f;
}

NO_INLINE_FN
void carp_math_project_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
    carp_math_mul_v3_f(b, (carp_math_dot_v3(a, b) / carp_math_dot_v3(b, b)), outV3);
}
NO_INLINE_FN
void carp_math_reject_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
    carp_math_project_v3(a, b, outV3);
    carp_math_sub_v3_v3(a, outV3, outV3);
}
NO_INLINE_FN
f32 carp_math_dot_v3(const CarpV3A* a, const CarpV3A* b)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rcx), %%xmm0\n\t"
        "mulps (%%rdx), %%xmm0\n\t"
        "movaps %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );

    result = tmpstack.x + tmpstack.y + tmpstack.z;
#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps (%%rsi), %%xmm0\n\t"
        "movaps %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a), "m"(b)
        #endif
    );

    result = tmpstack.x + tmpstack.y + tmpstack.z;
#else
    result = a->x * b->x + a->y * b->y + a->z * b->z;
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_min_v3(const CarpV3A* a)
{
    float result;
#if USE_ASM_WINDOWS
    __asm__ volatile (
        "movd (%%rcx), %%xmm0\n\t"
        "movd +4(%%rcx), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd +8(%%rcx), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[result]\n\t"
        : [result] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd +8(%%rdi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[result]\n\t"
        : [result] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#else
    result = carp_math_min_f_f(a->x, carp_math_min_f_f(a->y, a->z));
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_max_v3(const CarpV3A* a)
{
    float result;
#if USE_ASM_WINDOWS
    __asm__ volatile (
        "movd (%%rcx), %%xmm0\n\t"
        "movd +4(%%rcx), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd +8(%%rcx), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[result]\n\t"
        : [result] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd +8(%%rdi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[result]\n\t"
        : [result] "+m" (result)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
#else
    result = carp_math_max_f_f(a->x, carp_math_max_f_f(a->y, a->z));
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_sqrLen_v3(const CarpV3A* a)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rcx), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movaps %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
    result = tmpstack.x + tmpstack.y + tmpstack.z;

#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movaps %%xmm0, %[c]\n\t"
        : [c] "+m" (tmpstack)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(a)
        #endif
    );
    result = tmpstack.x + tmpstack.y + tmpstack.z;
#else
    result = a->x * a->x + a->y * a->y + a->z * a->z;
#endif
    return result;
}
NO_INLINE_FN
f32 carp_math_len_v3(const CarpV3A* a)
{
    return sqrtf(carp_math_sqrLen_v3(a));
}





//////////////////////////////
//
// Quat begins
//
///////////////////////////////


void carp_math_neg_q(const CarpQuat* q, CarpQuat* outQuat)
{
    carp_math_neg_v3((const CarpV3A*)q, (CarpV3A*) outQuat);
}

void carp_math_add_q_q(const CarpQuat* q1, const CarpQuat* q2, CarpQuat* outQuat)
{
    carp_math_add_v3_v3((const CarpV3A*)q1, (const CarpV3A*)q2, (CarpV3A*) outQuat);
}

void carp_math_sub_q_q(const CarpQuat* q1, const CarpQuat* q2, CarpQuat* outQuat)
{
    carp_math_sub_v3_v3((const CarpV3A*)q1, (const CarpV3A*)q2, (CarpV3A*) outQuat);
}

void carp_math_mul_q_f(const CarpQuat* q, float t, CarpQuat* outQuat)
{
    carp_math_mul_v3_f((const CarpV3A*)q, t, (CarpV3A*) outQuat);
}


float carp_math_dot_q(const CarpQuat* q1, const CarpQuat* q2)
{
    return q1->vx * q2->vx + q1->vy * q2->vy + q1->vz * q2->vz + q1->w * q2->w;
}


void carp_math_mul_q_q(const CarpQuat* a, const CarpQuat* b, CarpQuat* outQuat)
{
    const CarpV3A *av = (const CarpV3A *)a; // {a->vx, a->vy, a->vz, 0.0f};
    const CarpV3A *bv = (const CarpV3A *)b; //{b->vx, b->vy, b->vz, 0.0f};
    CarpV3A tmp;
    CarpV3A tmp2;
    carp_math_cross_v3(av, bv, &tmp);
    carp_math_mul_v3_f(bv, a->w, &tmp2);
    carp_math_add_v3_v3(&tmp, &tmp2, &tmp2);

    carp_math_mul_v3_f(av, b->w, &tmp);

    carp_math_add_v3_v3(&tmp, &tmp2, (CarpV3A*)outQuat);
    outQuat->w = a->w * b->w - carp_math_dot_v3(av, bv);
}

bool carp_math_normalize_q(const CarpQuat* q, CarpQuat* outQuat)
{
    float sqrLength = carp_math_dot_q(q, q);
    if(sqrLength <CARP_EPSILON)
    {
        carp_math_zero_v3((CarpV3A*) outQuat);
        //DEBUG_BREAK_MACRO_MATH();
        return false;
    }
    float length = 1.0f / sqrtf(sqrLength);
    carp_math_mul_q_f(q, length, outQuat);
    return true;
}

void carp_math_conjugate_q(const CarpQuat* q, CarpQuat* outQuat)
{
    carp_math_neg_v3((const CarpV3A*)q, (CarpV3A*) outQuat);
    outQuat->w = q->w;
}

void carp_math_rotate_v3WithQuat(const CarpV3A* v, const CarpQuat* q, CarpV3A* outV3)
{
    const CarpV3A* qv = (const CarpV3A*)q;

    CarpV3A tmp1;
    CarpV3A tmp2;

    float d = carp_math_sqrLen_v3((const CarpV3A*)q);

    //2.0f * (qv * dot(v, qv) + cross(qv, v) * q->w)
    carp_math_mul_v3_f(qv, carp_math_dot_v3(v, qv), &tmp1);
    carp_math_cross_v3(qv, v, &tmp2);
    carp_math_mul_v3_f(&tmp2, q->w, &tmp2);
    carp_math_add_v3_v3(&tmp1, &tmp2, &tmp2);
    carp_math_mul_v3_f(&tmp2, 2.0f, &tmp2);

    // v * (q->w * q->w - d)
    carp_math_mul_v3_f(v, (q->w * q->w - d), &tmp1);


    carp_math_add_v3_v3(&tmp1, &tmp2, outV3);

    //return (v * (q->w * q->w - d) + 2.0f * (qv * dot(v, qv) + cross(qv, v) * q->w))
}

void carp_math_lerp_q(CarpQuat const* q1, CarpQuat const* q2, float t, CarpQuat* outQuat)
{
    float dotAngle = carp_math_dot_q(q1, q2);
    if (dotAngle < 0.0f)
    {
        outQuat->vx = q1->vx - t * (q1->vx + q2->vx);
        outQuat->vy = q1->vy - t * (q1->vy + q2->vy);
        outQuat->vz = q1->vz - t * (q1->vz + q2->vz);
        outQuat->w = q1->w - t * (q1->w + q2->w);
    }
    else
    {
        carp_math_lerp_v3((const CarpV3A*)q1, (const CarpV3A*)q2, t, (CarpV3A*)outQuat);
        //outQuat->vx = q1->vx - t * (q1->vx - q2->vx);
        //outQuat->vy = q1->vy - t * (q1->vy - q2->vy);
        //outQuat->vz = q1->vz - t * (q1->vz - q2->vz);
        //outQuat->w = q1->w - t * (q1->w - q2->w);
    }
}

bool carp_math_slerp_q(CarpQuat const* q1, CarpQuat const* q2, float t, CarpQuat* outQuat)
{
    float dotAngle = carp_math_dot_q(q1, q2);

    if (dotAngle < 0.0f)
    {
        dotAngle = -dotAngle;
    }
    if (dotAngle > 0.9995)
    {
        carp_math_lerp_q(q1, q2, t, outQuat);
        return carp_math_normalize_q(outQuat, outQuat);
    }

    float theta0 = cosf(dotAngle);
    float theta = theta0 * t;

    float sinTheta = sinf(theta);
    float sinTheta0 = sinf(theta0);

    float s2 = sinTheta / sinTheta0;
    float s1 = sinf(theta) - dotAngle * s2;

    CarpQuat tmp;
    carp_math_mul_q_f(q1, s1, &tmp);
    carp_math_mul_q_f(q2, s2, outQuat);
    carp_math_add_q_q(&tmp, outQuat, outQuat);
    return carp_math_normalize_q(outQuat, outQuat);
}


void carp_math_getAxisFromQuat(const CarpQuat* quat, CarpV3A* right, CarpV3A* up, CarpV3A* forward)
{
    right->x = 1.0f - 2.0f * quat->vy * quat->vy - 2.0f * quat->vz * quat->vz;
    right->y = 2.0f * quat->vx * quat->vy + 2.0f * quat->w * quat->vz;
    right->z = 2.0f * quat->vx * quat->vz - 2.0f * quat->w * quat->vy;

    up->x = 2.0f * quat->vx * quat->vy - 2.0f * quat->w * quat->vz;
    up->y = 1.0f - 2.0f * quat->vx * quat->vx - 2.0f * quat->vz * quat->vz;
    up->z = 2.0f * quat->vy * quat->vz + 2.0f * quat->w * quat->vx;

    forward->x = 2.0f * quat->vx * quat->vz + 2.0f * quat->w * quat->vy;
    forward->y = 2.0f * quat->vy * quat->vz - 2.0f * quat->w * quat->vx;
    forward->z = 1.0f - 2.0f * quat->vx * quat->vx - 2.0f * quat->vy * quat->vy;
}


void carp_math_getQuatFromAxisAngle(const CarpV3A* v, float angle, CarpQuat* outQuat)
{
    float s = sinf(angle * 0.5f);
    carp_math_normalize_v3(v, (CarpV3A*)outQuat);
    carp_math_mul_q_f(outQuat, s, outQuat);
    outQuat->w = cosf(angle * 0.5f);
}

bool carp_math_getQuatFromNormalizedVectors(const CarpV3A* from, const CarpV3A* toVector, CarpQuat* outQuat)
{
    float d = carp_math_dot_v3(from, toVector);
    CarpV3A* outV3 = (CarpV3A*)outQuat;

    if(d >= 1.0f - CARP_EPSILON)
        return false;

    if(d <= -1.0f + CARP_EPSILON)
    {
        // Generate a rotation axis to do 180 degree rotation
        if(carp_math_abs_f(from->x) < 0.707f)
        {
            static const CarpV3A RightVector = {1.0f, 0.0f, 0.0f, 0.0f};
            carp_math_cross_v3(&RightVector, from, outV3);
        }
        else
        {
            static const CarpV3A UpVector = {1.0f, 0.0f, 0.0f, 0.0f};
            carp_math_cross_v3(&UpVector, from, outV3);
        }
        carp_math_getQuatFromAxisAngle(outV3, CARP_PI, outQuat);
    }
    else
    {
        float s = sqrtf((1.0f + d) * 2.0f);
        carp_math_cross_v3(from, toVector, outV3);
        outV3->w = 0.5f * s;
        carp_math_normalize_q(outQuat, outQuat);
    }
    return true;
}



void getDirectionsFromPitchYawRoll(
    float pitch, float yaw, float roll, CarpV3A* rightDir, CarpV3A* upDir, CarpV3A* forwardDir)
{
    static const CarpV3A Right =    {0.0f, 0.0f, 1.0f, 0.0f};
    static const CarpV3A Up =       {0.0f, 1.0f, 0.0f, 0.0f};
    static const CarpV3A Forward =  {1.0f, 0.0f, 0.0f, 0.0f};

    CarpQuat rotation;
    carp_math_getQuatFromAxisAngle(&Forward, roll, &rotation);

    CarpQuat tmpRotation;
    carp_math_getQuatFromAxisAngle(&Right, pitch, &tmpRotation);
    carp_math_mul_q_q(&tmpRotation, &rotation, &rotation);
    carp_math_getQuatFromAxisAngle(&Up, yaw, &tmpRotation);
    carp_math_mul_q_q(&tmpRotation, &rotation, &rotation);

    carp_math_rotate_v3WithQuat(&Right, &rotation, rightDir);
    carp_math_rotate_v3WithQuat(&Up, &rotation, upDir);
    carp_math_rotate_v3WithQuat(&Forward, &rotation, forwardDir);
}














//////////////////////////////
//
// Mat begins
//
///////////////////////////////

NO_INLINE_FN
void carp_math_zero_m34(CarpM34* outM34)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "pxor %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rcx)\n\t"
        "movups %%xmm0, +16(%%rcx)\n\t"
        "movups %%xmm0, +32(%%rcx)\n\t"
        : [c] "=m"(outM34)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "pxor %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rdi)\n\t"
        "movups %%xmm0, +16(%%rdi)\n\t"
        "movups %%xmm0, +32(%%rdi)\n\t"
        : [c] "=m"(outM34)
    );
#else
    outM34->r0.simdv3a = _mm_setzero_ps();
    outM34->r1.simdv3a = _mm_setzero_ps();
    outM34->r2.simdv3a = _mm_setzero_ps();
#endif
}



NO_INLINE_FN
void carp_math_zero_m44(CarpM44* outM44)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "pxor %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rcx)\n\t"
        "movups %%xmm0, +16(%%rcx)\n\t"
        "movups %%xmm0, +32(%%rcx)\n\t"
        "movups %%xmm0, +48(%%rcx)\n\t"
        : [c] "=m"(outM44)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "pxor %%xmm0, %%xmm0\n\t"
        "movups %%xmm0, (%%rdi)\n\t"
        "movups %%xmm0, +16(%%rdi)\n\t"
        "movups %%xmm0, +32(%%rdi)\n\t"
        "movups %%xmm0, +48(%%rdi)\n\t"
        : [c] "=m"(outM44)
    );
#else
    outM44->r0.simdv3a = _mm_setzero_ps();
    outM44->r1.simdv3a = _mm_setzero_ps();
    outM44->r2.simdv3a = _mm_setzero_ps();
    outM44->r3.simdv3a = _mm_setzero_ps();
#endif
}


void carp_math_getM34Identity(CarpM34* outM34)
{
    outM34->v[0] = 1.0f;
    outM34->v[1] = 0.0f;
    outM34->v[2] = 0.0f;
    outM34->v[3] = 0.0f;

    outM34->v[4] = 0.0f;
    outM34->v[5] = 1.0f;
    outM34->v[6] = 0.0f;
    outM34->v[7] = 0.0f;

    outM34->v[4] = 0.0f;
    outM34->v[5] = 0.0f;
    outM34->v[6] = 1.0f;
    outM34->v[7] = 0.0f;
}

void carp_math_getM44Identity(CarpM44* outM44)
{
    outM44->v[0] = 1.0f;
    outM44->v[1] = 0.0f;
    outM44->v[2] = 0.0f;
    outM44->v[3] = 0.0f;

    outM44->v[4] = 0.0f;
    outM44->v[5] = 1.0f;
    outM44->v[6] = 0.0f;
    outM44->v[7] = 0.0f;

    outM44->v[4] = 0.0f;
    outM44->v[5] = 0.0f;
    outM44->v[6] = 1.0f;
    outM44->v[7] = 0.0f;

    outM44->v[8] = 0.0f;
    outM44->v[9] = 0.0f;
    outM44->v[10] = 0.0f;
    outM44->v[11] = 1.0f;
}



void carp_math_getM34FromQuat(const CarpQuat* quat, CarpM34* outM34)
{
    float xy2 = 2.0f * quat->vx * quat->vy;
    float xz2 = 2.0f * quat->vx * quat->vz;
    float yz2 = 2.0f * quat->vy * quat->vz;

    float wx2 = 2.0f * quat->w * quat->vx;
    float wy2 = 2.0f * quat->w * quat->vy;
    float wz2 = 2.0f * quat->w * quat->vz;

    float xx2 = 2.0f * quat->vx * quat->vx;
    float yy2 = 2.0f * quat->vy * quat->vy;
    float zz2 = 2.0f * quat->vz * quat->vz;

    outM34->_00 = 1.0f - yy2 - zz2;
    outM34->_01 = xy2 - wz2;
    outM34->_02 = xz2 + wy2;

    outM34->_10 = xy2 + wz2;
    outM34->_11 = 1.0f - xx2 - zz2;
    outM34->_12 = yz2 - wx2;

    outM34->_20 = xz2 - wy2;
    outM34->_21 = yz2 + wx2;
    outM34->_22 = 1.0f - xx2 - yy2;

    outM34->_03 = 0.0f;
    outM34->_13 = 0.0f;
    outM34->_23 = 0.0f;
}

void carp_math_getM34FromScale(const CarpV3A* scale, CarpM34* outM34)
{
    carp_math_zero_m34(outM34);
    outM34->_00 = scale->x;
    outM34->_11 = scale->y;
    outM34->_22 = scale->z;

}
void carp_math_getM34FromTranslation(const CarpV3A* pos, CarpM34* outM34)
{
    carp_math_zero_m34(outM34);
    outM34->_03 = pos->x;
    outM34->_13 = pos->y;
    outM34->_23 = pos->z;
}

bool carp_math_createOrthoM44(float width, float height, float nearPlane, float farPlane, CarpM44* outM44)
{
    //ASSERT_MATH(sAbsF(width) >= 1.0f);
    //ASSERT_MATH(sAbsF(height) >= 1.0f);
    //ASSERT_MATH(sAbsF(farPlane - nearPlane) > 0.00001f);

    carp_math_zero_m44(outM44);

    float fRange = 1.0f / (farPlane - nearPlane);

    outM44->_00 = 2.0f / width;
    outM44->_11 = 2.0f / height;
    outM44->_22 = fRange;
    outM44->_23 = -fRange * nearPlane;
    outM44->_33 = 1.0f;
    return true;
}

bool carp_math_createPerspectiveM44(float fov, float aspectRatio, float nearPlane, float farPlane, CarpM44* outM44)
{
    //ASSERT_MATH(carp_math_abs_f(fov) > 0.00001f);
    //ASSERT_MATH(carp_math_abs_f(aspectRatio) > 0.001f);
    //ASSERT_MATH(carp_math_abs_f(farPlane - nearPlane) > 0.00001f);
    //ASSERT_MATH(carp_math_abs_f(nearPlane) > 0.0f);

    carp_math_zero_m44(outM44);

    float yScale = 1.0f / sTanF(sToRadians(fov * 0.5f));
    float xScale = yScale / aspectRatio;
    float fRange = farPlane / (farPlane - nearPlane);

    outM44->_00 = xScale;
    outM44->_11 = yScale;

    outM44->_22 = -fRange;
    outM44->_23 = -nearPlane * fRange;
    outM44->_32 = -1.0f;
    outM44->_33 = 0.0f;
}

void carp_math_getM44fromLookAt(const CarpV3A* pos, const CarpV3A* target, const CarpV3A* up, CarpM44* outM44)
{
    CarpV3A* r0 = &outM44->r0;
    CarpV3A* r1 = &outM44->r1;
    CarpV3A* r2 = &outM44->r2;

    // Forward (r2), normalize target - position, with positive direction being -z
    carp_math_sub_v3_v3(pos, target, r2);
    carp_math_normalize_v3(r2, r2);

    // Right (r0), cross with forward and camera up
    carp_math_cross_v3(up, r2, r0);
    carp_math_normalize_v3(r0, r0);

    // Real up, cross with forward and right
    carp_math_cross_v3(r2, r0, r1);
    carp_math_normalize_v3(r1, r1);

    outM44->_03 = -carp_math_dot_v3(pos, r0);
    outM44->_13 = -carp_math_dot_v3(pos, r1);
    outM44->_23 = -carp_math_dot_v3(pos, r2);

    outM44->_30 = 0.0;
    outM44->_31 = 0.0;
    outM44->_32 = 0.0;
    outM44->_33 = 1.0;

}

void carp_math_transpose_m44(const CarpM44* m, CarpM44* outM44)
{
    if(m == outM44)
    {
        CarpM44 tmp;
        memcpy(&tmp, m, sizeof(CarpM44));
        carp_math_transpose_m44(&tmp, outM44);
        return;
    }
    outM44->_00 = m->_00;
    outM44->_01 = m->_10;
    outM44->_02 = m->_20;
    outM44->_03 = m->_30;

    outM44->_10 = m->_01;
    outM44->_11 = m->_11;
    outM44->_12 = m->_21;
    outM44->_13 = m->_31;

    outM44->_20 = m->_02;
    outM44->_21 = m->_12;
    outM44->_22 = m->_22;
    outM44->_23 = m->_32;

    outM44->_30 = m->_03;
    outM44->_31 = m->_13;
    outM44->_32 = m->_23;
    outM44->_33 = m->_33;
}

bool carp_math_inverse_m44(const CarpM44* m, CarpM44* outM44)
{
    outM44->v[0] = (
        (m->v[5]  * m->v[10] * m->v[15] - m->v[5]  * m->v[11] * m->v[14]) -
        (m->v[9]  * m->v[6]  * m->v[15] - m->v[9]  * m->v[7]  * m->v[14]) +
        (m->v[13] * m->v[6]  * m->v[11] - m->v[13] * m->v[7]  * m->v[10]));

    outM44->v[1] = -(
        (m->v[1]  * m->v[10] * m->v[15] - m->v[1]  * m->v[11] * m->v[14]) -
        (m->v[9]  * m->v[2]  * m->v[15] - m->v[9]  * m->v[3]  * m->v[14]) +
        (m->v[13] * m->v[2]  * m->v[11] - m->v[13] * m->v[3]  * m->v[10]));

    outM44->v[2] = (
        (m->v[1]  * m->v[6] * m->v[15] - m->v[1]  * m->v[7] * m->v[14]) -
        (m->v[5]  * m->v[2] * m->v[15] - m->v[5]  * m->v[3] * m->v[14]) +
        (m->v[13] * m->v[2] * m->v[7]  - m->v[13] * m->v[3] * m->v[6]));

    outM44->v[3] = -(
        (m->v[1] * m->v[6] * m->v[11] - m->v[1] * m->v[7] * m->v[10]) -
        (m->v[5] * m->v[2] * m->v[11] - m->v[5] * m->v[3] * m->v[10]) +
        (m->v[9] * m->v[2] * m->v[7]  - m->v[9] * m->v[3] * m->v[6]));




    outM44->v[4] = - (
        (m->v[4]  * m->v[10] * m->v[15] - m->v[4]  * m->v[11] * m->v[14]) -
        (m->v[8]  * m->v[6]  * m->v[15] - m->v[8]  * m->v[7]  * m->v[14]) +
        (m->v[12] * m->v[6]  * m->v[11] - m->v[12] * m->v[7]  * m->v[10]));

    outM44->v[5] = (
        (m->v[0]  * m->v[10] * m->v[15] - m->v[0]  * m->v[11] * m->v[14]) -
        (m->v[8]  * m->v[2]  * m->v[15] - m->v[8]  * m->v[3]  * m->v[14]) +
        (m->v[12] * m->v[2]  * m->v[11] - m->v[12] * m->v[3]  * m->v[10]));

    outM44->v[6] = -(
        (m->v[0] * m->v[6] * m->v[15] - m->v[0] * m->v[7] * m->v[14]) -
        (m->v[4] * m->v[2] * m->v[15] - m->v[4] * m->v[3] * m->v[14]) +
        (m->v[12] * m->v[2] * m->v[7] - m->v[12] * m->v[3] * m->v[6]));

    outM44->v[7] = (
        (m->v[0] * m->v[6] * m->v[11] - m->v[0] * m->v[7] * m->v[10]) -
        (m->v[4] * m->v[2] * m->v[11] - m->v[4] * m->v[3] * m->v[10]) +
        (m->v[8] * m->v[2] * m->v[7]  - m->v[8] * m->v[3] * m->v[6]));




    outM44->v[8] = (
        (m->v[4] * m->v[9] * m->v[15] - m->v[4] * m->v[11] * m->v[13]) -
        (m->v[8] * m->v[5] * m->v[15] - m->v[8] * m->v[7] * m->v[13]) +
        (m->v[12] * m->v[5] * m->v[11] - m->v[12] * m->v[7] * m->v[9]));

    outM44->v[9] = -(
        (m->v[0] * m->v[9] * m->v[15] - m->v[0] * m->v[11] * m->v[13]) -
        (m->v[8] * m->v[1] * m->v[15] - m->v[8] * m->v[3] * m->v[13]) +
        (m->v[12] * m->v[1] * m->v[11] - m->v[12] * m->v[3] * m->v[9]));

    outM44->v[10] = (
        (m->v[0] * m->v[5] * m->v[15] - m->v[0] * m->v[7] * m->v[13]) -
        (m->v[4] * m->v[1] * m->v[15] - m->v[4] * m->v[3] * m->v[13]) +
        (m->v[12] * m->v[1] * m->v[7] - m->v[12] * m->v[3] * m->v[5]));

    outM44->v[11] = -(
        (m->v[0] * m->v[5] * m->v[11] - m->v[0] * m->v[7] * m->v[9]) -
        (m->v[4] * m->v[1] * m->v[11] - m->v[4] * m->v[3] * m->v[9]) +
        (m->v[8] * m->v[1] * m->v[7]  - m->v[8] * m->v[3] * m->v[5]));




    outM44->v[12] = -(
        (m->v[4] * m->v[9] * m->v[14] - m->v[4] * m->v[10] * m->v[13]) -
        (m->v[8] * m->v[5] * m->v[14] - m->v[8] * m->v[6] * m->v[13]) +
        (m->v[12] * m->v[5] * m->v[10] - m->v[12] * m->v[6] * m->v[9]));

    outM44->v[13] = (
        (m->v[0] * m->v[9] * m->v[14] - m->v[0] * m->v[10] * m->v[13]) -
        (m->v[8] * m->v[1] * m->v[14] - m->v[8] * m->v[2] * m->v[13]) +
        (m->v[12] * m->v[1] * m->v[10] - m->v[12] * m->v[2] * m->v[9]));

    outM44->v[14] = -(
        (m->v[0] * m->v[5] * m->v[14] - m->v[0] * m->v[6] * m->v[13]) -
        (m->v[4] * m->v[1] * m->v[14] - m->v[4] * m->v[2] * m->v[13]) +
        (m->v[12] * m->v[1] * m->v[6] - m->v[12] * m->v[2] * m->v[5]));

    outM44->v[15] = (
        (m->v[0] * m->v[5] * m->v[10] - m->v[0] * m->v[6] * m->v[9]) -
        (m->v[4] * m->v[1] * m->v[10] - m->v[4] * m->v[2] * m->v[9]) +
        (m->v[8] * m->v[1] * m->v[6]  - m->v[8] * m->v[2] * m->v[5]));

    float det = m->v[0] * outM44->v[0] + m->v[1] * outM44->v[4] + m->v[2] * outM44->v[8] + m->v[3] * outM44->v[12];

    if (carp_math_abs_f(det) < CARP_EPSILON)
    {
        return false;
    }
    det = 1.0f / det;
    for (int i = 0; i < 16; i++)
        outM44->v[i] *= det;

    return true;

}

bool carp_math_eq_m44(const CarpM44* a, const CarpM44* b)
{
    for(int i = 0; i < 16; ++i)
    {
        float f = carp_math_abs_f(a->v[i] - b->v[i]);
        if(f > CARP_EPSILON)
            return false;
    }
    return true;

}

bool carp_math_isIdentity_m44(const CarpM44* m)
{
    for(int i = 0; i < 16; ++i)
    {
        float f = m->v[i];

        if(i == (i / 4) + (i / 4) * 4)
            f = f - 1.0f;

        if(carp_math_abs_f(f) > CARP_EPSILON)
            return false;
    }
    return true;

}

void carp_math_mul_m44_v3(const CarpM44* m, const CarpV3A* v, CarpV3A* outV3);
void carp_math_mul_v3_m44(const CarpV3A* v, const CarpM44* m, CarpV3A* outV3);

void carp_math_mul_m34_v3(const CarpM34* m, const CarpV3A* v, CarpV3A* outV3);
void carp_math_mul_v3_m34(const CarpV3A* v, const CarpM34* m, CarpV3A* outV3);

void carp_math_mul_m44_m44(const CarpM44* a, const CarpM44* b, CarpM44* outM44);
void carp_math_mul_m44_m34(const CarpM44* a, const CarpM34* b, CarpM44* outM44);
void carp_math_mul_m34_m44(const CarpM34* a, const CarpM44* b, CarpM44* outM44);
void carp_math_mul_m34_m34(const CarpM34* a, const CarpM34* b, CarpM34* outM34);





//////////////////////////////
//
// Helper begins
//
///////////////////////////////


void carp_math_print_v2(const CarpV2* v, const char* name)
{
    printf("%s: v2: {%f, %f}\n", name, v->x, v->y);
}

void carp_math_print_v3a(const CarpV3A* v, const char* name)
{
    printf("%s: v3a: {%f, %f, %f, %f}\n", name, v->x, v->y, v->z, v->w);
}

void carp_math_print_q(const CarpQuat* q, const char* name)
{
    printf("%s: quat: {%f, %f, %f, %f}\n", name, q->vx, q->vy, q->vz, q->w);
}

void carp_math_print_m34(const CarpM34* m, const char* name)
{
    printf("%s m44 r0: {%f, %f, %f, %f}\n", name, m->v[0], m->v[1], m->v[2], m->v[3]);
    printf("%s m44 r1: {%f, %f, %f, %f}\n", name, m->v[4], m->v[5], m->v[6], m->v[7]);
    printf("%s m44 r2: {%f, %f, %f, %f}\n", name, m->v[8], m->v[9], m->v[10], m->v[11]);
}


void carp_math_print_m44(const CarpM44* m, const char* name)
{
    printf("%s m44 r0: {%f, %f, %f, %f}\n", name, m->v[0], m->v[1], m->v[2], m->v[3]);
    printf("%s m44 r1: {%f, %f, %f, %f}\n", name, m->v[4], m->v[5], m->v[6], m->v[7]);
    printf("%s m44 r2: {%f, %f, %f, %f}\n", name, m->v[8], m->v[9], m->v[10], m->v[11]);
    printf("%s m44 r3: {%f, %f, %f, %f}\n", name, m->v[12], m->v[13], m->v[14], m->v[15]);
}
