#ifndef CARP_MATH_HH
#define CARP_MATH_HH

#include "carptype.h"

#ifndef __TINYC__
    #include <immintrin.h>
#endif

#define CARP_EPSILON (1.0e-5f)

f32 carp_math_min_f(f32 a, f32 b);
f32 carp_math_max_f(f32 a, f32 b);

typedef struct CarpV2
{
    _Alignas(8) struct
    { 
        f32 x;
        f32 y;
    };
} CarpV2;

CarpV2 carp_math_neg_v2(CarpV2 a);
CarpV2 carp_math_normalize_v2(CarpV2 a);
CarpV2 carp_math_lerp_v2(CarpV2 a, CarpV2 b, f32 t);

CarpV2 carp_math_add_v2_v2(CarpV2 a, CarpV2 b);
CarpV2 carp_math_add_v2_f(CarpV2 a, f32 f);
CarpV2 carp_math_add_f_v2(f32 f, CarpV2 b);

CarpV2 carp_math_sub_v2_v2(CarpV2 a, CarpV2 b);
CarpV2 carp_math_sub_v2_f(CarpV2 a, f32 f);
CarpV2 carp_math_sub_f_v2(f32 f, CarpV2 b);

CarpV2 carp_math_mul_v2_v2(CarpV2 a, CarpV2 b);
CarpV2 carp_math_mul_v2_f(CarpV2 a, f32 f);
CarpV2 carp_math_mul_f_v2(f32 f, CarpV2 b);

CarpV2 carp_math_div_v2_v2(CarpV2 a, CarpV2 b);
CarpV2 carp_math_div_v2_f(CarpV2 a, f32 f);
CarpV2 carp_math_div_f_v2(f32 f, CarpV2 b);

CarpV2 carp_math_min_v2_v2(CarpV2 a, CarpV2 b);
CarpV2 carp_math_max_v2_v2(CarpV2 a, CarpV2 b);
f32 carp_math_dot_v2(CarpV2 a, CarpV2 b);
f32 carp_math_min_v2_f(CarpV2 a);
f32 carp_math_max_v2_f(CarpV2 a);
f32 carp_math_sqrLen_v2(CarpV2 a);
f32 carp_math_len_v2(CarpV2 a);



typedef struct CarpV3A
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
} CarpV3A;

CarpV3A carp_math_neg_v3(CarpV3A a);
CarpV3A carp_math_normalize_v3(CarpV3A a);
CarpV3A carp_math_lerp_v3(CarpV3A a, CarpV3A b, f32 t);

CarpV3A carp_math_add_v3_v3(CarpV3A a, CarpV3A b);
CarpV3A carp_math_add_v3_3(CarpV3A a, f32 f);
CarpV3A carp_math_add_f_v3(f32 f, CarpV3A);

CarpV3A carp_math_sub_v3_v3(CarpV3A a, CarpV3A b);
CarpV3A carp_math_sub_v3_3(CarpV3A a, f32 f);
CarpV3A carp_math_sub_f_v3(f32 f, CarpV3A);

CarpV3A carp_math_mul_v3_v3(CarpV3A a, CarpV3A b);
CarpV3A carp_math_mul_v3_3(CarpV3A a, f32 f);
CarpV3A carp_math_mul_f_v3(f32 f, CarpV3A);

CarpV3A carp_math_div_v3_v3(CarpV3A a, CarpV3A b);
CarpV3A carp_math_div_v3_3(CarpV3A a, f32 f);
CarpV3A carp_math_div_f_v3(f32 f, CarpV3A);

CarpV3A carp_math_min_v3_v3(CarpV3A a, CarpV3A b);
CarpV3A carp_math_max_v3_v3(CarpV3A a, CarpV3A b);
CarpV3A carp_math_cross(CarpV3A, CarpV3A b);
CarpV3A carp_math_proj(CarpV3A, CarpV3A b);
CarpV3A carp_math_reject(CarpV3A, CarpV3A b);

f32 carp_math_dot_v3(CarpV3A a, CarpV3A b);
f32 carp_math_min_v3_f(CarpV3A a);
f32 carp_math_max_v3_f(CarpV3A a);
f32 carp_math_sqrLen_v3(CarpV3A a);
f32 carp_math_len_v3(CarpV3A a);





#endif // CARP_MATH_HH