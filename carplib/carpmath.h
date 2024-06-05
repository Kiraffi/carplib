#ifndef CARP_MATH_HH
#define CARP_MATH_HH

#include "carptype.h"

#ifndef __TINYC__
    #include <immintrin.h>
#endif

#define CARP_EPSILON (1.0e-5f)

f32 carp_math_min_f(f32 a, f32 b);
f32 carp_math_max_f(f32 a, f32 b);

typedef struct Vec2
{
    _Alignas(8) struct
    { 
        f32 x;
        f32 y;
    };
} Vec2;

Vec2 carp_math_neg_v2(Vec2 a);
Vec2 carp_math_normalize_v2(Vec2 a);
Vec2 carp_math_lerp_v2(Vec2 a, Vec2 b, f32 t);

Vec2 carp_math_add_v2_v2(Vec2 a, Vec2 b);
Vec2 carp_math_add_v2_f(Vec2 a, f32 f);
Vec2 carp_math_add_f_v2(f32 f, Vec2 b);

Vec2 carp_math_sub_v2_v2(Vec2 a, Vec2 b);
Vec2 carp_math_sub_v2_f(Vec2 a, f32 f);
Vec2 carp_math_sub_f_v2(f32 f, Vec2 b);

Vec2 carp_math_mul_v2_v2(Vec2 a, Vec2 b);
Vec2 carp_math_mul_v2_f(Vec2 a, f32 f);
Vec2 carp_math_mul_f_v2(f32 f, Vec2 b);

Vec2 carp_math_div_v2_v2(Vec2 a, Vec2 b);
Vec2 carp_math_div_v2_f(Vec2 a, f32 f);
Vec2 carp_math_div_f_v2(f32 f, Vec2 b);

Vec2 carp_math_min_v2_v2(Vec2 a, Vec2 b);
Vec2 carp_math_max_v2_v2(Vec2 a, Vec2 b);
f32 carp_math_dot_v2(Vec2 a, Vec2 b);
f32 carp_math_min_v2_f(Vec2 a);
f32 carp_math_max_v2_f(Vec2 a);
f32 carp_math_sqrLen_v2(Vec2 a);
f32 carp_math_len_v2(Vec2 a);



typedef struct Vec3A
{
    _Alignas(16) union
    {
        struct 
        {
            float x;
            float y;
            float z;
            float w;
        };
        float v[4];
        #ifndef __TINYC__
            __m128 simdv3a;
        #endif

    };
} Vec3A;

Vec3A carp_math_neg_v3(Vec3A a);
Vec3A carp_math_normalize_v3(Vec3A a);
Vec3A carp_math_lerp_v3(Vec3A a, Vec3A b, f32 t);

Vec3A carp_math_add_v3_v3(Vec3A a, Vec3A b);
Vec3A carp_math_add_v3_3(Vec3A a, f32 f);
Vec3A carp_math_add_f_v3(f32 f, Vec3A);

Vec3A carp_math_sub_v3_v3(Vec3A a, Vec3A b);
Vec3A carp_math_sub_v3_3(Vec3A a, f32 f);
Vec3A carp_math_sub_f_v3(f32 f, Vec3A);

Vec3A carp_math_mul_v3_v3(Vec3A a, Vec3A b);
Vec3A carp_math_mul_v3_3(Vec3A a, f32 f);
Vec3A carp_math_mul_f_v3(f32 f, Vec3A);

Vec3A carp_math_div_v3_v3(Vec3A a, Vec3A b);
Vec3A carp_math_div_v3_3(Vec3A a, f32 f);
Vec3A carp_math_div_f_v3(f32 f, Vec3A);

Vec3A carp_math_min_v3_v3(Vec3A a, Vec3A b);
Vec3A carp_math_max_v3_v3(Vec3A a, Vec3A b);
Vec3A carp_math_cross(Vec3A, Vec3A b);
Vec3A carp_math_proj(Vec3A, Vec3A b);
Vec3A carp_math_reject(Vec3A, Vec3A b);

f32 carp_math_dot_v3(Vec3A a, Vec3A b);
f32 carp_math_min_v3_f(Vec3A a);
f32 carp_math_max_v3_f(Vec3A a);
f32 carp_math_sqrLen_v3(Vec3A a);
f32 carp_math_len_v3(Vec3A a);





#endif // CARP_MATH_HH