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

f32 carp_math_min_f_f(f32 a, f32 b)
{
    f32 result;
#if USE_ASM_LINUX || USE_ASM_WINDOWS

    __asm__ volatile  (
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result = a < b ? a : b;
#endif
    return result;
}

f32 carp_math_max_f_f(f32 a, f32 b)
{
    f32 result;
#if USE_ASM_LINUX || USE_ASM_WINDOWS

    __asm__ volatile  (
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result = a > b ? a : b;
#endif

    return result;
}


CarpV2 carp_math_broadcast_v2(f32 f)
{
    CarpV2 result;
#if USE_ASM_LINUX || USE_ASM_WINDOWS

    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, +0x0%[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result.x = result.y = f;

#endif

    return result;
}


CarpV2 carp_math_neg_v2(const CarpV2* a)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    static const CarpV2 negZeroV2 = { -0.0f, -0.0f };
    __asm__ volatile  (
        "movq %[zerov2], %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [zerov2]"m"(negZeroV2)
    );
#elif USE_ASM_LINUX
    static const CarpV2 negZeroV2 = { -0.0f, -0.0f };
    __asm__ volatile  (
        "movq %[zerov2], %%xmm1\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [zerov2]"m"(negZeroV2)
    );
#else
    result.x = -a->x;
    result.y = -a->y;

#endif
    return result;
}

CarpV2 carp_math_normalize_v2(const CarpV2* a)
{
    f32 d2 = carp_math_sqrLen_v2(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV2 result = {0};
        return result;
    }
    d2 = sqrtf(d2);
    CarpV2 result;
    result.x = a->x / d2;
    result.y = a->y / d2;
    return result;
}
CarpV2 carp_math_lerp_v2(const CarpV2* a, const CarpV2* b, f32 t)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm2, %%xmm2\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "subps %%xmm0, %%xmm1\n\t"
        "mulps %%xmm1, %%xmm2\n\t"
        "addps %%xmm2, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "=m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm2\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "subps %%xmm2, %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "addps %%xmm2, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "=m"(result)
    );
#else
    result.x = a->x + (b->x - a->x) * t;
    result.y = a->y + (b->y - a->y) * t;
#endif
    return result;
}

CarpV2 carp_math_add_v2_v2(const CarpV2* a, const CarpV2* b)
{
    //printf("got here\n");
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [a] "m" (a), [b] "m" (b)
    );

#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [a] "m" (a), [b] "m" (b)
    );
#else
    result.x = a->x + b->x;
    result.y = a->y + b->y;
#endif
    return result;
}

CarpV2 carp_math_add_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm1\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x + f;
    result.y = a->y + f;
#endif
    //carp_math_print_v2(&result, "add v2 f");

    return result;
}
CarpV2 carp_math_add_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = b->x + f;
    result.y = b->y + f;
#endif

    return result;
}



CarpV2 carp_math_sub_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x - b->x;
    result.y = a->y - b->y;
#endif
    return result;
}
CarpV2 carp_math_sub_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "subps %%xmm0, %%xmm1\n\t"
        "movq %%xmm1, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x - f;
    result.y = a->y - f;
#endif

    return result;
}



CarpV2 carp_math_mul_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x * b->x;
    result.y = a->y * b->y;
#endif
    return result;
}
CarpV2 carp_math_mul_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x * f;
    result.y = a->y * f;
#endif

    return result;
}
CarpV2 carp_math_mul_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = b->x * f;
    result.y = b->y * f;
#endif

    return result;
}



CarpV2 carp_math_div_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x / b->x;
    result.y = a->y / b->y;
#endif
    return result;
}
CarpV2 carp_math_div_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movq (%%rcx), %%xmm0\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movq (%%rdi), %%xmm1\n\t"
        "divps %%xmm0, %%xmm1\n\t"
        "movq %%xmm1, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x / f;
    result.y = a->y / f;
#endif
    return result;
}

CarpV2 carp_math_min_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#else
    result.x = carp_math_min_f_f(a->x, b->x);
    result.y = carp_math_min_f_f(a->y, b->y);
#endif
    return result;
}
CarpV2 carp_math_max_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#else
    result.x = carp_math_max_f_f(a->x, b->x);
    result.y = carp_math_max_f_f(a->y, b->y);
#endif
    return result;
}
f32 carp_math_dot_v2(const CarpV2* a, const CarpV2* b)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rcx), %%xmm0\n\t"
        "movq (%%rdx), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y;
#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y;
#else
    result = a->x * b->x + a->y * b->y;
#endif
    return result;
}
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
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#else
    result = carp_math_min_f_f(a->x, a->y);
#endif
    return result;
}
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
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#else
    result = carp_math_max_f_f(a->x, a->y);
#endif
    return result;
}
f32 carp_math_sqrLen_v2(const CarpV2* a)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rcx), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );
    result = tmpstack.x + tmpstack.y;

#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rdi), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );
    result = tmpstack.x + tmpstack.y;

#else
    result = a->x * a->x + a->y * a->y;
#endif
    return result;
}
f32 carp_math_len_v2(const CarpV2* a)
{
    return sqrtf(carp_math_sqrLen_v2(a));
}








CarpV3A carp_math_broadcast_v3(f32 f)
{
    CarpV3A result;
#if USE_ASM_WINDOWS || USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "movups %%xmm1, %[c]\n\t"
        "movl $0, +0xc%[c]\n\t"
        : [c] "=m"(result)
    );

#else
    result.simdv3a = _mm_set_ps(0.0f, f, f, f);
#endif

    return result;
}





CarpV3A carp_math_neg_v3(const CarpV3A* a)
{
    static const CarpV3A neg = { -0.0f, -0.0f, -0.0f, -0.0f };
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups %[negin], %%xmm0\n\t"
        "pxor (%%rdx), %%xmm0\n\t" // why rdx???? rcx = neg?
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [negin]"m"(neg)
    );

#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups %[negin], %%xmm1\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [negin]"m"(neg)
    );
#else
    result.simdv3a = _mm_xor_ps(a->simdv3a, neg.simdv3a);
#endif
    return result;
}

CarpV3A carp_math_normalize_v3(const CarpV3A* a)
{
    f32 d2 = carp_math_sqrLen_v3(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV3A result = {0};
        return result;
    }
    d2 = sqrtf(d2);
    CarpV3A result = carp_math_div_v3_f(a, d2);
    return result;
}

CarpV3A carp_math_lerp_v3(const CarpV3A* a, const CarpV3A* b, f32 t)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm3, %%xmm3\n\t"
        "punpckldq %%xmm3, %%xmm3\n\t"
        "movups (%%r8), %%xmm0\n\t"
        "subps (%%rdx), %%xmm0\n\t"
        "mulps %%xmm3, %%xmm0\n\t"
        "addps (%%rdx), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "=m"(result)
    );

#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups (%%rsi), %%xmm1\n\t"
        "subps (%%rdi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "addps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "=m"(result)
    );
#else
    CarpV3A sub = carp_math_sub_v3_v3(b, a);
    CarpV3A subt = carp_math_mul_v3_f(&sub, t);
    result = carp_math_add_v3_v3(a, &subt);
#endif
    return result;
}



CarpV3A carp_math_add_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        //"movups (%%rcx), %%xmm0\n\t"
        "movups (%%rdx), %%xmm0\n\t"
        "addps (%%r8), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "addps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.simdv3a = _mm_add_ps(a->simdv3a, b->simdv3a);
#endif
    return result;
}
CarpV3A carp_math_add_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm2, %%xmm2\n\t"
        "punpckldq %%xmm2, %%xmm2\n\t"
        //"addps (%%rcx), %%xmm1\n\t" rcx is for return?
        "addps (%%rdx), %%xmm2\n\t"
        "movups %%xmm2, %[res]\n\t"
        : [res] "+m" (result)
        : [f] "r" (f)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "addps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result.simdv3a = _mm_add_ps(a->simdv3a, _mm_set_ps1(f));
#endif

    return result;
}
CarpV3A carp_math_add_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "addps (%%r8), %%xmm1\n\t"
        "movups %%xmm1, %[res]\n\t"
        : [res] "+m" (result)
    );

#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "addps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result.simdv3a = _mm_add_ps(b->simdv3a, _mm_set_ps1(f));
#endif

    return result;
}




CarpV3A carp_math_sub_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rdx), %%xmm0\n\t"
        "subps (%%r8), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "subps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.simdv3a = _mm_sub_ps(a->simdv3a, b->simdv3a);
#endif
    return result;
}
CarpV3A carp_math_sub_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm2, %%xmm2\n\t"
        "punpckldq %%xmm2, %%xmm2\n\t"
        "movups (%%rdx), %%xmm0\n\t"
        "subps %%xmm2, %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups (%%rdi), %%xmm1\n\t"
        "subps %%xmm0, %%xmm1\n\t"
        "movups %%xmm1, %[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result.simdv3a = _mm_sub_ps(a->simdv3a, _mm_set1_ps(f));
#endif

    return result;
}

CarpV3A carp_math_mul_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rdx), %%xmm0\n\t"
        "mulps (%%r8), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.simdv3a = _mm_mul_ps(a->simdv3a, b->simdv3a);
#endif
    return result;
}
CarpV3A carp_math_mul_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm2, %%xmm2\n\t"
        "punpckldq %%xmm2, %%xmm2\n\t"
        "mulps (%%rdx), %%xmm2\n\t"
        "movups %%xmm2, %[res]\n\t"
        : [res] "+m" (result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "mulps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result.simdv3a = _mm_mul_ps(a->simdv3a, _mm_set1_ps(f));
#endif
    return result;
}

CarpV3A carp_math_mul_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm1, %%xmm1\n\t"
        "punpckldq %%xmm1, %%xmm1\n\t"
        "mulps (%%r8), %%xmm1\n\t"
        "movups %%xmm1, %[res]\n\t"
        : [res] "+m" (result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "mulps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );
#else
    result.simdv3a = _mm_mul_ps(b->simdv3a, _mm_set1_ps(f));
#endif
    return result;
}

CarpV3A carp_math_div_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rdx), %%xmm0\n\t"
        "divps (%%r8), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        "movl $0, 0xc%[c]\n\t" // set w to 0.0f
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "divps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        "movl $0, 0xc%[c]\n\t" // set w to 0.0f
        : [c] "+m"(result)
    );
#else
    result.x = a->x / b->x;
    result.y = a->y / b->y;
    result.z = a->z / b->z;
    result.w = 0.0f;
#endif
    return result;
}

CarpV3A carp_math_div_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "punpckldq %%xmm2, %%xmm2\n\t"
        "punpckldq %%xmm2, %%xmm2\n\t"
        "movups (%%rdx), %%xmm0\n\t"
        "divps %%xmm2, %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        "movl $0, +0xc%[c]\n\t"
        : [c] "+m"(result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "punpckldq %%xmm0, %%xmm0\n\t"
        "punpckldq %%xmm0, %%xmm0\n\t"
        "movups (%%rdi), %%xmm1\n\t"
        "divps %%xmm0, %%xmm1\n\t"
        "movups %%xmm1, %[c]\n\t"
        "movl $0, +0xc%[c]\n\t"
        : [c] "+m"(result)
    );
#else
    result.x = a->x / f;
    result.y = a->y / f;
    result.z = a->z / f;
    result.w = 0.0f;
#endif
    return result;
}

CarpV3A carp_math_min_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rdx), %%xmm0\n\t"
        "minps (%%r8), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "minps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#else
    result.simdv3a = _mm_min_ps(a->simdv3a, b->simdv3a);
#endif
    return result;
}

CarpV3A carp_math_max_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if USE_ASM_WINDOWS
    __asm__ volatile  (
        "movups (%%rdx), %%xmm0\n\t"
        "maxps (%%r8), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#elif USE_ASM_LINUX
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "maxps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#else
    result.simdv3a = _mm_max_ps(a->simdv3a, b->simdv3a);
#endif
    return result;
}

CarpV3A carp_math_cross(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;

    result.x = a->y * b->z - a->z * b->y;
    result.y = a->z * b->x - a->x * b->z;
    result.z = a->x * b->y - a->y * b->x;
    result.w = 0.0f;

    return result;

}

CarpV3A carp_math_project(const CarpV3A* a, const CarpV3A* b)
{
    return carp_math_mul_v3_f(b, (carp_math_dot_v3(a, b) / carp_math_dot_v3(b, b)));
}

CarpV3A carp_math_reject(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A proj = carp_math_project(a, b);
    return carp_math_sub_v3_v3(a, &proj);
}

f32 carp_math_dot_v3(const CarpV3A* a, const CarpV3A* b)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rcx), %%xmm0\n\t"
        "mulps (%%rdx), %%xmm0\n\t"
        "movaps %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y + tmpstack.z;
#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps (%%rsi), %%xmm0\n\t"
        "movaps %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y + tmpstack.z;
#else
    result = a->x * b->x + a->y * b->y + a->z * b->z;
#endif
    return result;
}

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
    );
#else
    result = carp_math_min_f_f(a->x, carp_math_min_f_f(a->y, a->z));
#endif
    return result;
}

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
    );
#else
    result = carp_math_max_f_f(a->x, carp_math_max_f_f(a->y, a->z));
#endif
    return result;
}

f32 carp_math_sqrLen_v3(const CarpV3A* a)
{
    f32 result;
#if USE_ASM_WINDOWS
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rcx), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movaps %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );
    result = tmpstack.x + tmpstack.y + tmpstack.z;

#elif USE_ASM_LINUX
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movaps %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );
    result = tmpstack.x + tmpstack.y + tmpstack.z;
#else
    result = a->x * a->x + a->y * a->y + a->z * a->z;
#endif
    return result;
}
f32 carp_math_len_v3(const CarpV3A* a) { return sqrtf(carp_math_sqrLen_v3(a)); }




void carp_math_print_v2(const CarpV2* v, const char* name)
{
    printf("%s: {%f, %f}\n", name, v->x, v->y);
}

void carp_math_print_v3a(const CarpV3A* v, const char* name)
{
    printf("%s: {%f, %f, %f, %f}\n", name, v->x, v->y, v->z, v->w);
}
