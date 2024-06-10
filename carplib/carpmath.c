#include "carpmath.h"

#include <math.h>
#include <stdio.h>

// for memory
// "movl $0x3F800000, 0xc(%%rax)\n\t" // set w to 1.0f

#define USE_ASM (__TINYC__)



f32 carp_math_min_f_f(f32 a, f32 b)
{
    f32 result;
#if !USE_ASM
    result = a < b ? a : b;
#else
    __asm__ volatile  (
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );

#endif
    return result;
}

f32 carp_math_max_f_f(f32 a, f32 b)
{
    f32 result;
#if !USE_ASM
    result = a > b ? a : b;
#else
    __asm__ volatile  (
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[res]\n\t"
        : [res] "+m" (result)
    );

#endif

    return result;
}


CarpV2 carp_math_broadcast_v2(f32 f)
{
    CarpV2 result;
#if !USE_ASM
    result.x = result.y = f;
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[res]\n\t"
        "movd %%xmm0, +0x4%[res]\n\t"
        : [res] "+m" (result)
    );

#endif

    return result;
}


CarpV2 carp_math_neg_v2(const CarpV2* a)
{
    static const CarpV2 negZeroV2 = { -0.0f, -0.0f };
    CarpV2 result;
#if !USE_ASM
    result.x = -a->x;
    result.y = -a->y;
#else
    __asm__ volatile  (
        "movq %[zerov2], %%xmm1\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [zerov2]"m"(negZeroV2)
    );
#endif
    return result;
}

CarpV2 carp_math_normalize_v2(const CarpV2* a)
{
    f32 d2 = carp_math_sqrLen_v2(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV2 result = {};
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
#if !USE_ASM
    result.x = a->x + (b->x - a->x) * t;
    result.y = a->y + (b->y - a->y) * t;
#else
    __asm__ volatile  (
        "movq %%xmm0, %[c]\n\t"
        "movq %%xmm0, +4%[c]\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "movq %[c], %%xmm2\n\t"
        "subps %%xmm0, %%xmm1\n\t"
        "mulps %%xmm2, %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "=m"(result)
    );
#endif
    return result;
}

CarpV2 carp_math_add_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x + b->x;
    result.y = a->y + b->y;
#else
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}

CarpV2 carp_math_add_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x + f;
    result.y = a->y + f;
#else
    __asm__ volatile  (
        "movd %%xmm0, %[c]\n\t"
        "movd %%xmm0, +4%[c]\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "movq %[c], %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif

    return result;
}
CarpV2 carp_math_add_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = b->x + f;
    result.y = b->y + f;
#else
    __asm__ volatile  (
        "movd %%xmm0, %[c]\n\t"
        "movd %%xmm0, +4%[c]\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "movq %[c], %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif

    return result;
}



CarpV2 carp_math_sub_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x - b->x;
    result.y = a->y - b->y;
#else
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}
CarpV2 carp_math_sub_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x - f;
    result.y = a->y - f;
#else
    __asm__ volatile  (
        "movd %%xmm0, %[c]\n\t"
        "movd %%xmm0, +4%[c]\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "movq %[c], %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif

    return result;
}



CarpV2 carp_math_mul_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x * b->x;
    result.y = a->y * b->y;
#else
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}
CarpV2 carp_math_mul_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x * f;
    result.y = a->y * f;
#else
    __asm__ volatile  (
        "movd %%xmm0, %[c]\n\t"
        "movd %%xmm0, +4%[c]\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "movq %[c], %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif

    return result;
}
CarpV2 carp_math_mul_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = b->x * f;
    result.y = b->y * f;
#else
    __asm__ volatile  (
        "movd %%xmm0, %[c]\n\t"
        "movd %%xmm0, +4%[c]\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "movq %[c], %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif

    return result;
}



CarpV2 carp_math_div_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x / b->x;
    result.y = a->y / b->y;
#else
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}
CarpV2 carp_math_div_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result;
#if !USE_ASM
    result.x = a->x / f;
    result.y = a->y / f;
#else
    __asm__ volatile  (
        "movd %%xmm0, %[c]\n\t"
        "movd %%xmm0, +4%[c]\n\t"
        "movq (%%rdi), %%xmm0\n\t"
        "movq %[c], %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}

CarpV2 carp_math_min_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = carp_math_min_f_f(a->x, b->x);
    result.y = carp_math_min_f_f(a->y, b->y);
#else
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#endif
    return result;
}
CarpV2 carp_math_max_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
#if !USE_ASM
    result.x = carp_math_max_f_f(a->x, b->x);
    result.y = carp_math_max_f_f(a->y, b->y);
#else
    __asm__ volatile  (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#endif
    return result;
}
f32 carp_math_dot_v2(const CarpV2* a, const CarpV2* b)
{
    f32 result;
#if !USE_ASM
    result = a->x * b->x + a->y * b->y;
#else
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rdi), %%xmm0\n\t"
        "movq (%%rsi), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movq %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y;
#endif
    return result;
}
f32 carp_math_min_v2(const CarpV2* a)
{
    f32 result;
#if !USE_ASM
    result = carp_math_min_f_f(a->x, a->y);
#else
    __asm__ volatile  (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#endif
    return result;
}
f32 carp_math_max_v2(const CarpV2* a)
{
     f32 result;
#if !USE_ASM
    result = carp_math_max_f_f(a->x, a->y);
#else
    __asm__ volatile  (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#endif
    return result;
}
f32 carp_math_sqrLen_v2(const CarpV2* a)
{
    f32 result;
#if !USE_ASM
    result = a->x * a->x + a->y * a->y;
#else
    CarpV3A tmpstack;
    __asm__ volatile (
        "movq (%%rdi), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movq %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y;
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
    #if !USE_ASM
        result.simdv3a = _mm_set_ps(0.0f, f, f, f);
    #else
        __asm__ volatile  (
            "movd %%xmm0, +0x0%[c]\n\t"
            "movd %%xmm0, +0x4%[c]\n\t"
            "movd %%xmm0, +0x8%[c]\n\t"
            "movl $0, +0xc%[c]\n\t"
            : [c] "=m"(result)
        );
    #endif

    return result;
}





CarpV3A carp_math_neg_v3(const CarpV3A* a)
{
    static const CarpV3A neg = { -0.0f, -0.0f, -0.0f, -0.0f };
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_xor_ps(a->simdv3a, neg.simdv3a);
#else
    __asm__ volatile  (
        "movups %[negin], %%xmm1\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "pxor %%xmm1, %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
        : [negin]"m"(neg)
    );
#endif
    return result;
}

CarpV3A carp_math_normalize_v3(const CarpV3A* a)
{
    f32 d2 = carp_math_sqrLen_v3(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV3A result = {};
        return result;
    }
    d2 = sqrtf(d2);
    CarpV3A result = carp_math_div_v3_f(a, d2);
    return result;
}

CarpV3A carp_math_lerp_v3(const CarpV3A* a, const CarpV3A* b, f32 t)
{
    CarpV3A result;
#if !USE_ASM
    CarpV3A sub = carp_math_sub_v3_v3(b, a);
    CarpV3A subt = carp_math_mul_v3_f(&sub, t);
    result = carp_math_add_v3_v3(a, &subt);
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[c]\n\t"
        "movd %%xmm0, +0x4%[c]\n\t"
        "movd %%xmm0, +0x8%[c]\n\t"
        "movd %%xmm0, +0xc%[c]\n\t"
        "movups (%%rsi), %%xmm0\n\t"
        "subps (%%rdi), %%xmm0\n\t"
        "mulps %[c], %%xmm0\n\t"
        "addps (%%rdi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "=m"(result)
    );
#endif
    return result;
}



CarpV3A carp_math_add_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_add_ps(a->simdv3a, b->simdv3a);
#else
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "addps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}
CarpV3A carp_math_add_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_add_ps(a->simdv3a, _mm_set_ps1(f));
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[res]\n\t"
        "movd %%xmm0, +0x4%[res]\n\t"
        "movd %%xmm0, +0x8%[res]\n\t"
        "movd %%xmm0, +0xc%[res]\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "addps %[res], %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "=m" (result)
    );
#endif

    return result;
}
CarpV3A carp_math_add_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_add_ps(b->simdv3a, _mm_set_ps1(f));
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[res]\n\t"
        "movd %%xmm0, +0x4%[res]\n\t"
        "movd %%xmm0, +0x8%[res]\n\t"
        "movd %%xmm0, +0xc%[res]\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "addps %[res], %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "=m" (result)
    );
#endif

    return result;
}




CarpV3A carp_math_sub_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_sub_ps(a->simdv3a, b->simdv3a);
#else
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "subps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}
CarpV3A carp_math_sub_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_sub_ps(a->simdv3a, _mm_set1_ps(f));
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[res]\n\t"
        "movd %%xmm0, +0x4%[res]\n\t"
        "movd %%xmm0, +0x8%[res]\n\t"
        "movd %%xmm0, +0xc%[res]\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "subps %[res], %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "=m" (result)
    );
#endif

    return result;
}

CarpV3A carp_math_mul_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_mul_ps(a->simdv3a, b->simdv3a);
#else
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m"(result)
    );
#endif
    return result;
}
CarpV3A carp_math_mul_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_mul_ps(a->simdv3a, _mm_set1_ps(f));
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[res]\n\t"
        "movd %%xmm0, +0x4%[res]\n\t"
        "movd %%xmm0, +0x8%[res]\n\t"
        "movd %%xmm0, +0xc%[res]\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "mulps %[res], %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "=m" (result)
    );
#endif
    return result;
}

CarpV3A carp_math_mul_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_mul_ps(b->simdv3a, _mm_set1_ps(f));
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[res]\n\t"
        "movd %%xmm0, +0x4%[res]\n\t"
        "movd %%xmm0, +0x8%[res]\n\t"
        "movd %%xmm0, +0xc%[res]\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "mulps %[res], %%xmm0\n\t"
        "movups %%xmm0, %[res]\n\t"
        : [res] "=m" (result)
    );
#endif
    return result;
}

CarpV3A carp_math_div_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.x = a->x / b->x;
    result.y = a->y / b->y;
    result.z = a->z / b->z;
    result.w = 0.0f;
#else
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "divps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        "movl $0, 0xc%[c]\n\t" // set w to 0.0f
        : [c] "+m"(result)
    );
#endif
    return result;
}

CarpV3A carp_math_div_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
#if !USE_ASM
    result.x = a->x / f;
    result.y = a->y / f;
    result.z = a->z / f;
    result.w = 0.0f;
#else
    __asm__ volatile  (
        "movd %%xmm0, +0x0%[c]\n\t"
        "movd %%xmm0, +0x4%[c]\n\t"
        "movd %%xmm0, +0x8%[c]\n\t"
        "movups (%%rdi), %%xmm0\n\t"
        "divps %[c], %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        "movl $0, +0xc%[c]\n\t"
        : [c] "+m"(result)
        //: [f] "r" (f)
    );
#endif
    return result;
}

CarpV3A carp_math_min_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_min_ps(a->simdv3a, b->simdv3a);
#else
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "minps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
#endif
    return result;
}

CarpV3A carp_math_max_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
#if !USE_ASM
    result.simdv3a = _mm_max_ps(a->simdv3a, b->simdv3a);
#else
    __asm__ volatile  (
        "movups (%%rdi), %%xmm0\n\t"
        "maxps (%%rsi), %%xmm0\n\t"
        "movups %%xmm0, %[c]\n\t"
        : [c] "+m" (result)
    );
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
#if !USE_ASM
    result = a->x * b->x + a->y * b->y + a->z * b->z;
#else
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps (%%rsi), %%xmm0\n\t"
        "movaps %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y + tmpstack.z;
#endif
    return result;
}

f32 carp_math_min_v3(const CarpV3A* a)
{
    float result;
#if !USE_ASM
    result = carp_math_min_f_f(a->x, carp_math_min_f_f(a->y, a->z));
#else
    __asm__ volatile (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd +8(%%rdi), %%xmm1\n\t"
        "minps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[result]\n\t"
        : [result] "+m" (result)
    );
#endif
    return result;
}

f32 carp_math_max_v3(const CarpV3A* a)
{
    float result;
#if !USE_ASM
    result = carp_math_max_f_f(a->x, carp_math_max_f_f(a->y, a->z));
#else
    __asm__ volatile (
        "movd (%%rdi), %%xmm0\n\t"
        "movd +4(%%rdi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd +8(%%rdi), %%xmm1\n\t"
        "maxps %%xmm1, %%xmm0\n\t"
        "movd %%xmm0, %[result]\n\t"
        : [result] "+m" (result)
    );
#endif
    return result;
}

f32 carp_math_sqrLen_v3(const CarpV3A* a)
{
    f32 result;
#if !USE_ASM
    result = a->x * a->x + a->y * a->y + a->z * a->z;
#else
    CarpV3A tmpstack;
    __asm__ volatile (
        "movups (%%rdi), %%xmm0\n\t"
        "mulps %%xmm0, %%xmm0\n\t"
        "movaps %%xmm0, %[tmp]\n\t"
        : [tmp] "+m" (tmpstack)
    );

    result = tmpstack.x + tmpstack.y + tmpstack.z;
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
