#include "carpmath.h"

#include <math.h>

f32 carp_math_min_f(f32 a, f32 b) { return a < b ? a : b; }
f32 carp_math_max_f(f32 a, f32 b) { return a > b ? a : b; }


CarpV2 carp_math_neg_v2(CarpV2 a) { CarpV2 result = {-a.x, -a.y }; return result; }
CarpV2 carp_math_normalize_v2(CarpV2 a)
{
    f32 d2 = carp_math_sqrLen_v2(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV2 result = {};
        return result;
    }
    d2 = sqrtf(d2);
    CarpV2 result = { a.x / d2, a.y / d2 };
    return result;
}
CarpV2 carp_math_lerp_v2(CarpV2 a, CarpV2 b, f32 t)
{
    CarpV2 result = { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
    return result;
}

CarpV2 carp_math_add_v2_v2(CarpV2 a, CarpV2 b) { CarpV2 result = {a.x + b.x, a.y + b.y }; return result; }
CarpV2 carp_math_add_v2_f(CarpV2 a, f32 f)   { CarpV2 result = {a.x + f, a.y + f }; return result; }
CarpV2 carp_math_add_f_v2(f32 f, CarpV2 b)   { CarpV2 result = {f + b.x, f + b.y }; return result; }

CarpV2 carp_math_sub_v2_v2(CarpV2 a, CarpV2 b) { CarpV2 result = {a.x - b.x, a.y - b.y }; return result; }
CarpV2 carp_math_sub_v2_f(CarpV2 a, f32 f)   { CarpV2 result = {a.x - f, a.y - f }; return result; }
CarpV2 carp_math_sub_f_v2(f32 f, CarpV2 b)   { CarpV2 result = {f - b.x, f - b.y }; return result; }

CarpV2 carp_math_mul_v2_v2(CarpV2 a, CarpV2 b) { CarpV2 result = {a.x * b.x, a.y * b.y }; return result; }
CarpV2 carp_math_mul_v2_f(CarpV2 a, f32 f)   { CarpV2 result = {a.x * f, a.y * f }; return result; }
CarpV2 carp_math_mul_f_v2(f32 f, CarpV2 b)   { CarpV2 result = {f * b.x, f * b.y }; return result; }

CarpV2 carp_math_div_v2_v2(CarpV2 a, CarpV2 b) { CarpV2 result = {a.x / b.x, a.y / b.y }; return result; }
CarpV2 carp_math_div_v2_f(CarpV2 a, f32 f)   { CarpV2 result = {a.x / f, a.y / f }; return result; }
CarpV2 carp_math_div_f_v2(f32 f, CarpV2 b)   { CarpV2 result = {f / b.x, f / b.y }; return result; }

CarpV2 carp_math_min_v2_v2(CarpV2 a, CarpV2 b) { CarpV2 result = { carp_math_min_f(a.x, b.x), carp_math_min_f(a.y, b.y) }; return result;}
CarpV2 carp_math_max_v2_v2(CarpV2 a, CarpV2 b) { CarpV2 result = { carp_math_max_f(a.x, b.x), carp_math_max_f(a.y, b.y) }; return result;}
f32 carp_math_dot_v2(CarpV2 a, CarpV2 b) { return a.x * b.x + a.y * b.y; }
f32 carp_math_min_v2_f(CarpV2 a) { return carp_math_min_f(a.x, a.y); }
f32 carp_math_max_v2_f(CarpV2 a) { return carp_math_max_f(a.x, a.y); }
f32 carp_math_sqrLen_v2(CarpV2 a) { return a.x * a.x + a.y * a.y; }
f32 carp_math_len_v2(CarpV2 a) { return sqrtf(carp_math_sqrLen_v2(a)); }













CarpV3A carp_math_neg_v3(CarpV3A a) { CarpV3A result = { -a.x, -a.y, -a.z, -a.w }; return result; }
CarpV3A carp_math_normalize_v3(CarpV3A a)
{
    f32 d2 = carp_math_sqrLen_v3(a);
    if(d2 < CARP_EPSILON)
    {
        CarpV3A result = {};
        return result;
    }
    d2 = sqrtf(d2);
    CarpV3A result = { a.x / d2, a.y / d2, a.z / d2, 0.0f };
    return result;
}
CarpV3A carp_math_lerp_v3(CarpV3A a, CarpV3A b, f32 t)
{
    CarpV3A result = { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, 0.0f };
    return result;
}

CarpV3A carp_math_add_v3_v3(CarpV3A a, CarpV3A b) { CarpV3A result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; return result; }
CarpV3A carp_math_add_v3_f(CarpV3A a, f32 f)      { CarpV3A result = { a.x + f, a.y + f, a.z + f, a.w + f }; return result; }
CarpV3A carp_math_add_f_v3(f32 f, CarpV3A b)      { CarpV3A result = { b.x + f, b.y + f, b.z + f, b.w + f }; return result; }

CarpV3A carp_math_sub_v3_v3(CarpV3A a, CarpV3A b) { CarpV3A result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; return result; }
CarpV3A carp_math_sub_v3_f(CarpV3A a, f32 f)      { CarpV3A result = { a.x - f, a.y - f, a.z - f, a.w - f }; return result; }
CarpV3A carp_math_sub_f_v3(f32 f, CarpV3A b)      { CarpV3A result = { b.x - f, b.y - f, b.z - f, b.w - f }; return result; }

CarpV3A carp_math_mul_v3_v3(CarpV3A a, CarpV3A b) { CarpV3A result = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; return result; }
CarpV3A carp_math_mul_v3_f(CarpV3A a, f32 f)      { CarpV3A result = { a.x * f, a.y * f, a.z * f, a.w * f }; return result; }
CarpV3A carp_math_mul_f_v3(f32 f, CarpV3A b)      { CarpV3A result = { b.x * f, b.y * f, b.z * f, b.w * f }; return result; }

CarpV3A carp_math_div_v3_v3(CarpV3A a, CarpV3A b) { CarpV3A result = { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; return result; }
CarpV3A carp_math_div_v3_f(CarpV3A a, f32 f)      { CarpV3A result = { a.x / f, a.y / f, a.z / f, a.w / f }; return result; }
CarpV3A carp_math_div_f_v3(f32 f, CarpV3A b)      { CarpV3A result = { b.x / f, b.y / f, b.z / f, b.w / f }; return result; }

CarpV3A carp_math_min_v3_v3(CarpV3A a, CarpV3A b) { CarpV3A result = { carp_math_min_f(a.x, b.x), carp_math_min_f(a.y, b.y), carp_math_min_f(a.z, b.z), carp_math_min_f(a.w, b.w) }; return result;}
CarpV3A carp_math_max_v3_v3(CarpV3A a, CarpV3A b) { CarpV3A result = { carp_math_max_f(a.x, b.x), carp_math_max_f(a.y, b.y), carp_math_max_f(a.z, b.z), carp_math_max_f(a.w, b.w) }; return result;}

CarpV3A carp_math_cross(CarpV3A a, CarpV3A b)
{
    CarpV3A result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f
    };
    return result;

}

CarpV3A carp_math_project(CarpV3A a, CarpV3A b)
{
    return carp_math_mul_v3_f(b, (carp_math_dot_v3(a, b) / carp_math_dot_v3(b, b)));
}

CarpV3A carp_math_reject(CarpV3A a, CarpV3A b)
{
    return carp_math_sub_v3_v3(a, carp_math_project(a, b));
}

f32 carp_math_dot_v3(CarpV3A a, CarpV3A b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

f32 carp_math_min_v3_f(CarpV3A a) { return carp_math_min_f(a.x, carp_math_min_f(a.y, a.z)); }
f32 carp_math_max_v3_f(CarpV3A a) { return carp_math_max_f(a.x, carp_math_max_f(a.y, a.z)); }
f32 carp_math_sqrLen_v3(CarpV3A a) { return a.x * a.x + a.y * a.y + a.z * a.z; }
f32 carp_math_len_v3(CarpV3A a) { return sqrt(carp_math_sqrLen_v3(a)); }





