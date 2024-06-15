#include "carpmath.h"

#include <math.h>
#include <stdio.h>

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
void carp_math_add_f_v2(f32 f, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(b), "x"(f)
        #endif
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%rsi)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(b), "x"(f)
        #endif
    );
#else
    outV2->x = b->x + f;
    outV2->y = b->y + f;
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
void carp_math_mul_f_v2(f32 f, const CarpV2* b, CarpV2* outV2)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, (%%r8)\n\t"
        : [c] "+m"(outV2)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(b), "x"(f)
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
        : "m"(b), "x"(f)
        #endif
    );
#else
    outV2->x = b->x * f;
    outV2->y = b->y * f;
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
void carp_math_add_f_v3(f32 f, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "addps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(b), "x"(f)
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
        : "m"(b), "x"(f)
        #endif
    );
#else
    outV3->simdv3a = _mm_add_ps(b->simdv3a, _mm_set_ps1(f));
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
void carp_math_mul_f_v3(f32 f, const CarpV3A* b, CarpV3A* outV3)
{
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "mulps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, (%%r8)\n\t"
        : [res] "+m" (outV3)
        #if defined(__clang__) || defined(__GNUC__)
        : "m"(b), "x"(f)
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
        : "m"(b), "x"(f)
        #endif
    );
#else
    outV3->simdv3a = _mm_mul_ps(b->simdv3a, _mm_set1_ps(f));
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
void carp_math_cross(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
    outV3->x = a->y * b->z - a->z * b->y;
    outV3->y = a->z * b->x - a->x * b->z;
    outV3->z = a->x * b->y - a->y * b->x;
    outV3->w = 0.0f;
}

NO_INLINE_FN
void carp_math_project(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
    carp_math_mul_v3_f(b, (carp_math_dot_v3(a, b) / carp_math_dot_v3(b, b)), outV3);
}
NO_INLINE_FN
void carp_math_reject(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3)
{
    carp_math_project(a, b, outV3);
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




/*
//////////////////////////////
//
// Quat begins
//
///////////////////////////////


CarpQuat carp_math_quat_neg(const CarpQuat* v)
{
    CarpQuat result = {-v->vx, -v->vy, -v->vz, -v->w};
    return result;
}

CarpQuat carp_math_quat_add_q_q(const CarpQuat* a, const CarpQuat* b
{
    CarpQuat result = {a->vx + b->vx, a->vy + b->vy, a->vz + b->vz, a->w + b->w};
    return result;
}

CarpQuat carp_math_quat_sub_q_q(const CarpQuat* a, const CarpQuat* b)
{
    CarpQuat result = {a->vx - b->vx, a->vy - b->vy, a->vz - b->vz, a->w - b->w};
    return result;
}


float carp_math_quat_dot(const CarpQuat* q1, const CarpQuat* q2)
{
    return q1->vx * q2->vx + q1->vy * q2->vy + q1->vz * q2->vz + q1->w * q2->w;
}


CarpQuat carp_math_quat_mul_q_q(const CarpQuat* a, const CarpQuat* b)
{
    const CarpV3A av = {a->vx, a->vy, a->vz, 0.0f};
    const CarpV3A bv = {b->vx, b->vy, b->vz, 0.0f};
    CarpQuat result = {
        carp_math_add_v3_v3(&carp_math_cross(&av, &bv), &carp_math_mul_f_v3(a->w, &bv)) + &carp_math_mul_f_v3(b->w, &av),
        a->w * b->w - dot(av, bv) };
    return result;
}

CarpQuat carp_math_quat_normalize(const CarpQuat* q)
{
    float sqrLength = q->vx * q->vx + q->vy * q->vy + q->vz * q->vz + q->w * q->w;
    if(sqrLength <CARP_EPSILON)
    {
        //DEBUG_BREAK_MACRO_MATH();
        return Quat();
    }
    float length = 1.0f / sSqrtF(sqrLength);
    return q * length;
}

CarpQuat carp_math_quat_conjugate(const CarpQuat* q)
{
    return Quat(-q->vx, -q->vy, -q->vz, q->w);
}

CarpV3A carp_math_quat_rotateV3(const CarpV3A* v, const CarpQuat* q)
{
    CarpV3A qv(q->vx, q->vy, q->vz);
    float d = sqrLen(qv);
    return (v * (q->w * q->w - d) + 2->0f * (qv * dot(v, qv) + cross(qv, v) * q->w))
}

void carp_math_quat_getAxis(const CarpQuat* quat, CarpV3A* right, CarpV3A* up, CarpV3A* forward)
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


CarpQuat carp_math_quat_getQuatFromAxisAngle(const CarpV3A* v, float angle)
{
    float s = sSinF(angle * 0.5f);
    CarpV3A v2 = normalize(v) * s;
    CarpQuat result(v2->x, v2->y, v2->z, sCosF(angle * 0.5f));
    return result;
}

CarpQuat carp_math_quat_getQuatFromNormalizedVectors(const CarpV3A* from, const CarpV3A* toVector)
{
    float d = dot(from, toVector);
    if(d >= 1.0f - 1e-5f)
        return Quat();

    else if(d <= -1.0f + 1e-5f)
    {
        // Generate a rotation axis to do 180 degree rotation
        if(sAbsF(from.x) < 0.707f)
            return getQuatFromAxisAngle(normalize(cross(Vec3(1.0f, 0.0f, 0.0f), from)), PI);
        else
            return getQuatFromAxisAngle(normalize(cross(Vec3(0.0f, 1.0f, 0.0f), from)), PI);
    }
    else
    {
        float s = sSqrtF((1.0f + d) * 2.0f);
        CarpV3A v = cross(from, toVector);
        return normalize(Quat(v.x, v.y, v.z, 0.5f * s));
    }
}

CarpQuat carp_math_quat_mul_q_f32(const CarpQuat* q, float t)
{
    CarpQuat result = { q->vx * t, q->vy * t, q->vz * t, q->w * t};
    return result;
}

CarpQuat carp_math_quat_mul_f32_q(float t, const CarpQuat* q)
{
    CarpQuat result = {q->vx * t, q->vy * t, q->vz * t, q->w * t};
    return result;
}

CarpQuat carp_math_quat_lerp(CarpQuat const* q1, CarpQuat const* q2, float t)
{
    float dotAngle = dot(q1, q2);
    CarpQuat result;
    if (dotAngle < 0.0f)
    {
        result.vx = q1->vx - t * (q1->vx + q2->vx);
        result.vy = q1->vy - t * (q1->vy + q2->vy);
        result.vz = q1->vz - t * (q1->vz + q2->vz);
        result.w = q1->w - t * (q1->w + q2->w);
    }
    else
    {
        result.vx = q1->vx - t * (q1->vx - q2->vx);
        result.vy = q1->vy - t * (q1->vy - q2->vy);
        result.vz = q1->vz - t * (q1->vz - q2->vz);
        result.w = q1->w - t * (q1->w - q2->w);
    }
    return result;
}

CarpQuat carp_math_quat_slerp(CarpQuat const* q1, CarpQuat const* q2, float t)
{
    float dotAngle = dot(q1, q2);

    if (dotAngle < 0.0f)
    {
        dotAngle = -dotAngle;
    }
    if (dotAngle > 0.9995)
    {
        return normalize(lerp(q1, q2, t));
    }

    float theta0 = sCosF(dotAngle);
    float theta = theta0 * t;

    float sinTheta = sSinF(theta);
    float sinTheta0 = sSinF(theta0);

    float s2 = sinTheta / sinTheta0;
    float s1 = sCosF(theta) - dotAngle * s2;

    return normalize(Quat(
        q1->vx * s1 + q2->vx * s2,
        q1->vy * s1 + q2->vy * s2,
        q1->vz * s1 + q2->vz * s2,
        q1->w * s1 + q2->w * s2));
}

void getDirectionsFromPitchYawRoll(
    float pitch, float yaw, float roll, CarpV3A* rightDir, CarpV3A* upDir, CarpV3A* forwardDir)
{
    static const CarpV3A Right =    {0.0f, 0.0f, 1.0f, 0.0f};
    static const CarpV3A Up =       {0.0f, 1.0f, 0.0f, 0.0f};
    static const CarpV3A Forward =  {1.0f, 0.0f, 0.0f, 0.0f};

    CarpQuat rotation = carp_math_getQuatFromAxisAngle(&Forward, roll);
    rotation = carp_math_quat_mul_q_q(carp_math_getQuatFromAxisAngle(&Right, pitch), &rotation);
    rotation = carp_math_quat_mul_q_q(carp_math_getQuatFromAxisAngle(&Up, yaw), &rotation);

    rightDir = rotateVector(Vec3(1.0f, 0.0, 0.0f), rotation);
    upDir = rotateVector(Vec3(0.0, 1.0, 0.0f), rotation);
    forwardDir = rotateVector(Vec3(0.0, 0.0, 1.0f), rotation);
}
*/













//////////////////////////////
//
// Mat begins
//
///////////////////////////////





//////////////////////////////
//
// Helper begins
//
///////////////////////////////


void carp_math_print_v2(const CarpV2* v, const char* name)
{
    printf("%s: {%f, %f}\n", name, v->x, v->y);
}

void carp_math_print_v3a(const CarpV3A* v, const char* name)
{
    printf("%s: {%f, %f, %f, %f}\n", name, v->x, v->y, v->z, v->w);
}
