#ifndef CARP_MATH_HH
#define CARP_MATH_HH

#include "carptype.h"

#ifndef __TINYC__
    #include <immintrin.h>
#else

    typedef double __m128d;
    typedef float __m128;

#endif

#define CARP_EPSILON (1.0e-5f)


typedef struct CarpV2
{
    _Alignas(8) struct
    {
        f32 x;
        f32 y;
    };
} CarpV2;



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
        __m128 simdv3a;
    };
} CarpV3A;


// v2
f32 carp_math_min_f(f32 a, f32 b);
f32 carp_math_max_f(f32 a, f32 b);

CarpV2 carp_math_neg_v2(const CarpV2* a);
CarpV2 carp_math_normalize_v2(const CarpV2* a);
CarpV2 carp_math_lerp_v2(const CarpV2* a, const CarpV2* b, f32 t);

CarpV2 carp_math_add_v2_v2(const CarpV2* a, const CarpV2* b);
CarpV2 carp_math_add_v2_f(const CarpV2* a, f32 f);
CarpV2 carp_math_add_f_v2(f32 f, const CarpV2* b);

CarpV2 carp_math_sub_v2_v2(const CarpV2* a, const CarpV2* b);
CarpV2 carp_math_sub_v2_f(const CarpV2* a, f32 f);
CarpV2 carp_math_sub_f_v2(f32 f, const CarpV2* b);

CarpV2 carp_math_mul_v2_v2(const CarpV2* a, const CarpV2* b);
CarpV2 carp_math_mul_v2_f(const CarpV2* a, f32 f);
CarpV2 carp_math_mul_f_v2(f32 f, const CarpV2* b);

CarpV2 carp_math_div_v2_v2(const CarpV2* a, const CarpV2* b);
CarpV2 carp_math_div_v2_f(const CarpV2* a, f32 f);
CarpV2 carp_math_div_f_v2(f32 f, const CarpV2* b);

CarpV2 carp_math_min_v2_v2(const CarpV2* a, const CarpV2* b);
CarpV2 carp_math_max_v2_v2(const CarpV2* a, const CarpV2* b);
f32 carp_math_dot_v2(const CarpV2* a, const CarpV2* b);
f32 carp_math_min_v2_f(const CarpV2* a);
f32 carp_math_max_v2_f(const CarpV2* a);
f32 carp_math_sqrLen_v2(const CarpV2* a);
f32 carp_math_len_v2(const CarpV2* a);



// v3a
CarpV3A carp_math_broadcast_f32_v3(f32 f);
CarpV3A carp_math_neg_v3(const CarpV3A* a);
CarpV3A carp_math_normalize_v3(const CarpV3A* a);
CarpV3A carp_math_lerp_v3(const CarpV3A* a, const CarpV3A* b, f32 t);

CarpV3A carp_math_add_v3_v3(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_add_v3_3(const CarpV3A* a, f32 f);
CarpV3A carp_math_add_f_v3(f32 f, const CarpV3A* b);

CarpV3A carp_math_sub_v3_v3(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_sub_v3_f(const CarpV3A* a, f32 f);
CarpV3A carp_math_sub_f_v3(f32 f, const CarpV3A* b);

CarpV3A carp_math_mul_v3_v3(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_mul_v3_f(const CarpV3A* a, f32 f);
CarpV3A carp_math_mul_f_v3(f32 f, const CarpV3A* b);

CarpV3A carp_math_div_v3_v3(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_div_v3_f(const CarpV3A* a, f32 f);
CarpV3A carp_math_div_f_v3(f32 f, const CarpV3A* b);

CarpV3A carp_math_min_v3_v3(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_max_v3_v3(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_cross(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_project(const CarpV3A* a, const CarpV3A* b);
CarpV3A carp_math_reject(const CarpV3A* a, const CarpV3A* b);

f32 carp_math_dot_v3(const CarpV3A* a, const CarpV3A* b);
f32 carp_math_min_v3_f(const CarpV3A* a);
f32 carp_math_max_v3_f(const CarpV3A* a);
f32 carp_math_sqrLen_v3(const CarpV3A* a);
f32 carp_math_len_v3(const CarpV3A* a);




// helpers
void carp_math_print_v2(const CarpV2* v, const char* name);
void carp_math_print_v3a(const CarpV3A* v, const char* name);


#endif // CARP_MATH_HH