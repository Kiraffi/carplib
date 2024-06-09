#include "carpmath.h"

#include <math.h>
#include <stdio.h>

f32 carp_math_min_f(f32 a, f32 b) { return a < b ? a : b; }
f32 carp_math_max_f(f32 a, f32 b) { return a > b ? a : b; }


CarpV2 carp_math_neg_v2(const CarpV2* a) {
    CarpV2 result;
    result.x = -a->x;
    result.y = -a->y;
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
    result.x = a->x + (b->x - a->x) * t;
    result.y = a->y + (b->y - a->y) * t;
    return result;
}

CarpV2 carp_math_add_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
    result.x = a->x + b->x;
    result.y = a->y + b->y;
    return result;
}
CarpV2 carp_math_add_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result; result.x = a->x + f;
    result.y = a->y + f; return result;
}
CarpV2 carp_math_add_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result; result.x = f + b->x;
    result.y = f + b->y; return result;
}



CarpV2 carp_math_sub_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result; result.x = a->x - b->x;
    result.y = a->y - b->y;
    return result;
}
CarpV2 carp_math_sub_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result; result.x = a->x - f;
    result.y = a->y - f; return result;
}
CarpV2 carp_math_sub_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result; result.x = f - b->x;
    result.y = f - b->y; return result;
}



CarpV2 carp_math_mul_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result; result.x = a->x * b->x;
    result.y = a->y * b->y;
    return result;
}
CarpV2 carp_math_mul_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result; result.x = a->x * f;
    result.y = a->y * f; return result;
}
CarpV2 carp_math_mul_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result; result.x = f * b->x;
    result.y = f * b->y; return result;
}



CarpV2 carp_math_div_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result; result.x = a->x / b->x;
    result.y = a->y / b->y;
    return result;
}
CarpV2 carp_math_div_v2_f(const CarpV2* a, f32 f)
{
    CarpV2 result; result.x = a->x / f;
    result.y = a->y / f; return result;
}
CarpV2 carp_math_div_f_v2(f32 f, const CarpV2* b)
{
    CarpV2 result; result.x = f / b->x;
    result.y = f / b->y; return result;
}

CarpV2 carp_math_min_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
    result.x = carp_math_min_f(a->x, b->x);
    result.y = carp_math_min_f(a->y, b->y);
    return result;
}
CarpV2 carp_math_max_v2_v2(const CarpV2* a, const CarpV2* b)
{
    CarpV2 result;
    result.x = carp_math_max_f(a->x, b->x);
    result.y = carp_math_max_f(a->y, b->y);
    return result;
}
f32 carp_math_dot_v2(const CarpV2* a, const CarpV2* b) { return a->x * b->x + a->y * b->y; }
f32 carp_math_min_v2_f(const CarpV2* a) { return carp_math_min_f(a->x, a->y); }
f32 carp_math_max_v2_f(const CarpV2* a) { return carp_math_max_f(a->x, a->y); }
f32 carp_math_sqrLen_v2(const CarpV2* a) { return a->x * a->x + a->y * a->y; }
f32 carp_math_len_v2(const CarpV2* a) { return sqrtf(carp_math_sqrLen_v2(a)); }








CarpV3A carp_math_broadcast_f32_v3(f32 f)
{
    CarpV3A result;

    #ifndef __TINYC__
        result.simdv3a = (__m128){f, f, f, 0.0f};
    #else
        result.x = f;
        result.y = f;
        result.z = f;
        result.w = 0.0f;

    #endif

    return result;
}





CarpV3A carp_math_neg_v3(const CarpV3A* a)
{
    CarpV3A result;
    result.simdv3a = -a->simdv3a;
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
    CarpV3A sub = carp_math_sub_v3_v3(b, a);
    CarpV3A subt = carp_math_mul_v3_f(&sub, t);
    CarpV3A result = carp_math_mul_v3_v3(a, &subt);
    return result;
}



CarpV3A carp_math_add_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
    result.simdv3a = a->simdv3a + b->simdv3a;
    return result;
}
CarpV3A carp_math_add_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
    CarpV3A tmp = carp_math_broadcast_f32_v3(f);
    result.simdv3a = a->simdv3a + tmp.simdv3a;
    return result;
}
CarpV3A carp_math_add_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
    CarpV3A tmp = carp_math_broadcast_f32_v3(f);
    result.simdv3a = b->simdv3a + tmp.simdv3a;
    return result;
}




CarpV3A carp_math_sub_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
    result.simdv3a = a->simdv3a - b->simdv3a;
    return result;
}
CarpV3A carp_math_sub_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
    CarpV3A tmp = carp_math_broadcast_f32_v3(f);
    result.simdv3a = a->simdv3a - tmp.simdv3a;
    return result;
}
CarpV3A carp_math_sub_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
    CarpV3A tmp = carp_math_broadcast_f32_v3(f);
    result.simdv3a = b->simdv3a - tmp.simdv3a;
    return result;
}

CarpV3A carp_math_mul_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
    result.simdv3a = a->simdv3a * b->simdv3a;
    return result;
}
CarpV3A carp_math_mul_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
    CarpV3A tmp = carp_math_broadcast_f32_v3(f);
    result.simdv3a = a->simdv3a * tmp.simdv3a;
    return result;
}
CarpV3A carp_math_mul_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
    CarpV3A tmp = carp_math_broadcast_f32_v3(f);
    result.simdv3a = b->simdv3a * tmp.simdv3a;
    return result;
}

CarpV3A carp_math_div_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
    result.x = a->x / b->x;
    result.y = a->y / b->y;
    result.z = a->z / b->z;
    result.w = 0.0f;
    return result;
}
CarpV3A carp_math_div_v3_f(const CarpV3A* a, f32 f)
{
    CarpV3A result;
    result.x = a->x / f;
    result.y = a->y / f;
    result.z = a->z / f;
    result.w = 0.0f;
    return result;
}
CarpV3A carp_math_div_f_v3(f32 f, const CarpV3A* b)
{
    CarpV3A result;
    result.x = b->x / f;
    result.y = b->y / f;
    result.z = b->z / f;
    result.w = 0.0f;
    return result;
}

CarpV3A carp_math_min_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
    result.x = carp_math_min_f(a->x, b->x);
    result.y = carp_math_min_f(a->y, b->y);
    result.z = carp_math_min_f(a->z, b->z);
    result.w = 0.0f; return result;
}
CarpV3A carp_math_max_v3_v3(const CarpV3A* a, const CarpV3A* b)
{
    CarpV3A result;
    result.x = carp_math_max_f(a->x, b->x);
    result.y = carp_math_max_f(a->y, b->y);
    result.z = carp_math_max_f(a->z, b->z);
    result.w = 0.0f; return result;
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
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

f32 carp_math_min_v3_f(const CarpV3A* a) { return carp_math_min_f(a->x, carp_math_min_f(a->y, a->z)); }
f32 carp_math_max_v3_f(const CarpV3A* a) { return carp_math_max_f(a->x, carp_math_max_f(a->y, a->z)); }
f32 carp_math_sqrLen_v3(const CarpV3A* a) { return a->x * a->x + a->y * a->y + a->z * a->z; }
f32 carp_math_len_v3(const CarpV3A* a) { return sqrt(carp_math_sqrLen_v3(a)); }




void carp_math_print_v2(const CarpV2* v, const char* name)
{
    printf("%s: {%f, %f}\n", name, v->x, v->y);
}

void carp_math_print_v3a(const CarpV3A* v, const char* name)
{
    printf("%s: {%f, %f, %f, %f}\n", name, v->x, v->y, v->z, v->w);
}
