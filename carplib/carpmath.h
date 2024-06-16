#ifndef CARP_MATH_HH
#define CARP_MATH_HH

#include "carptype.h"

#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)

#ifndef __TINYC__
    #include <immintrin.h>
#else

    typedef double __m128d;
    typedef f32 __m128;

#endif

#if __TINYC__
    #define ASM_LABEL(name) #name
#else
    #define ASM_LABEL(name) #name"%="
#endif

#define CARP_EPSILON (1.0e-5f)
#define CARP_PI (3.1415926535897932384626433f)

typedef struct CarpV2
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
        };
        _Alignas(8) f32 v[2];
        _Alignas(8) int intArr[2];
        _Alignas(8) uint64_t u64;
    };
} CarpV2;



typedef struct CarpV3A
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };
        _Alignas(16) f32 v[4];
        _Alignas(16) int intArr[4];
        _Alignas(16) __m128 simdv3a;
    };
} CarpV3A;




typedef struct CarpQuat
{
    union
    {
        struct
        {
            f32 vx;
            f32 vy;
            f32 vz;
            f32 w;
        };
        _Alignas(16) f32 v[4];
        _Alignas(16) __m128 simdQuat;
    };
} CarpQuat;






typedef struct CarpM44
{
    union
    {
        struct
        {
            f32 _00;
            f32 _01;
            f32 _02;
            f32 _03;

            f32 _10;
            f32 _11;
            f32 _12;
            f32 _13;

            f32 _20;
            f32 _21;
            f32 _22;
            f32 _23;

            f32 _30;
            f32 _31;
            f32 _32;
            f32 _33;
        };
        _Alignas(16) f32 v[16];
        struct
        {
            CarpV3A r0;
            CarpV3A r1;
            CarpV3A r2;
            CarpV3A r3;
        };
    };
} CarpM44;

typedef struct CarpM34
{
    union
    {
        struct
        {
            f32 _00;
            f32 _01;
            f32 _02;
            f32 _03;

            f32 _10;
            f32 _11;
            f32 _12;
            f32 _13;

            f32 _20;
            f32 _21;
            f32 _22;
            f32 _23;
        };
        _Alignas(16) f32 v[12];
        struct
        {
            CarpV3A r0;
            CarpV3A r1;
            CarpV3A r2;
        };
    };
} CarpM34;




#ifdef __cplusplus
extern "C" {
#endif

// f32
f32 carp_math_abs_f(f32 a);
f32 carp_math_min_f_f(f32 a, f32 b);
f32 carp_math_max_f_f(f32 a, f32 b);
f32 carp_math_degToRad(f32 degrees);
f32 carp_math_radToDeg(f32 radians);

// v2
void carp_math_zero_v2(CarpV2* outV2);
void carp_math_set_v2(const CarpV2* a, CarpV2* outV2);
void carp_math_broadcast_v2(f32 f, CarpV2* outV2);
void carp_math_neg_v2(const CarpV2* a, CarpV2* outV2);
void carp_math_normalize_v2(const CarpV2* a, CarpV2* outV2);
void carp_math_lerp_v2(const CarpV2* a, const CarpV2* b, f32 t, CarpV2* outV2);

void carp_math_add_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_add_v2_f(const CarpV2* a, f32 f, CarpV2* outV2);

void carp_math_sub_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_sub_v2_f(const CarpV2* a, f32 f, CarpV2* outV2);

void carp_math_mul_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_mul_v2_f(const CarpV2* a, f32 f, CarpV2* outV2);

void carp_math_div_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_div_v2_f(const CarpV2* a, f32 f, CarpV2* outV2);

void carp_math_min_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_max_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
f32 carp_math_dot_v2(const CarpV2* a, const CarpV2* b);
f32 carp_math_min_v2(const CarpV2* a);
f32 carp_math_max_v2(const CarpV2* a);
f32 carp_math_sqrLen_v2(const CarpV2* a);
f32 carp_math_len_v2(const CarpV2* a);



// v3a
void carp_math_zero_v3(CarpV3A* outV3);
void carp_math_set_v3(const CarpV3A* a, CarpV3A* outV3);
void carp_math_broadcast_v3(f32 f, CarpV3A* outV3);
void carp_math_neg_v3(const CarpV3A* a, CarpV3A* outV3);
void carp_math_normalize_v3(const CarpV3A* a, CarpV3A* outV3);
void carp_math_lerp_v3(const CarpV3A* a, const CarpV3A* b, f32 t, CarpV3A* outV3);

void carp_math_add_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_add_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3);

void carp_math_sub_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_sub_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3);

void carp_math_mul_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_mul_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3);

void carp_math_div_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_div_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3);

void carp_math_min_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_max_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_cross_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_project_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_reject_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);

f32 carp_math_dot_v3(const CarpV3A* a, const CarpV3A* b);
f32 carp_math_min_v3(const CarpV3A* a);
f32 carp_math_max_v3(const CarpV3A* a);
f32 carp_math_sqrLen_v3(const CarpV3A* a);
f32 carp_math_len_v3(const CarpV3A* a);




// quat
void carp_math_getQuatIdentity(CarpQuat* outQuat);
void carp_math_set_q(const CarpQuat* a, CarpQuat* outQuat);
void carp_math_neg_q(const CarpQuat* q, CarpQuat* outQuat);
void carp_math_add_q_q(const CarpQuat* q1, const CarpQuat* q2, CarpQuat* outQuat);
void carp_math_sub_q_q(const CarpQuat* q1, const CarpQuat* q2, CarpQuat* outQuat);
void carp_math_mul_q_f(const CarpQuat* q, f32 t, CarpQuat* outQuat);

f32 carp_math_dot_q(const CarpQuat* q1, const CarpQuat* q2);
void carp_math_mul_q_q(const CarpQuat* a, const CarpQuat* b, CarpQuat* outQuat);

bool carp_math_normalize_q(const CarpQuat* q, CarpQuat* outQuat);
void carp_math_conjugate_q(const CarpQuat* q, CarpQuat* outQuat);
void carp_math_rotate_v3WithQuat(const CarpV3A* v, const CarpQuat* q, CarpV3A* outVec);
void carp_math_lerp_q(CarpQuat const* q1, CarpQuat const* q2, f32 t, CarpQuat* outQuat);
bool carp_math_slerp_q(CarpQuat const* q1, CarpQuat const* q2, f32 t, CarpQuat* outQuat);

void carp_math_getAxisFromQuat(const CarpQuat* quat, CarpV3A* right, CarpV3A* up, CarpV3A* forward);
void carp_math_getQuatFromAxisAngle(const CarpV3A* v, f32 angle, CarpQuat* outQuat);
bool carp_math_getQuatFromNormalizedVectors(const CarpV3A* from, const CarpV3A* toVector, CarpQuat* outQuat);
void carp_math_getDirectionsFromPitchYawRoll(
    f32 pitch, f32 yaw, f32 roll, CarpV3A* rightDir, CarpV3A* upDir, CarpV3A* forwardDir);





// mat
void carp_math_zero_m34(CarpM34* outM34);
void carp_math_zero_m44(CarpM44* outM44);
void carp_math_getM34Identity(CarpM34* outM34);
void carp_math_getM44Identity(CarpM44* outM44);
void carp_math_set_m34(const CarpM34* a, CarpM34* outM34);
void carp_math_set_m44(const CarpM44* a, CarpM44* outM44);

void carp_math_getM34FromTranslation(const CarpV3A* pos, CarpM34* outM34);
void carp_math_getM34FromRotation(const CarpQuat* quat, CarpM34* outM34);
void carp_math_getM34FromScale(const CarpV3A* scale, CarpM34* outM34);

void carp_math_getM34FromTRS(const CarpV3A* pos, const CarpQuat* rot, const CarpV3A* scale, CarpM34* outM34);
void carp_math_getInverseM34FromTRS(const CarpV3A* pos, const CarpQuat* rot, const CarpV3A* scale, CarpM34* outM34);

bool carp_math_createOrthoM44(f32 width, f32 height, f32 nearPlane, f32 farPlane, CarpM44* outM44);
bool carp_math_createPerspectiveM44(f32 yFovInDegrees, f32 aspectRatio, f32 nearPlane, f32 farPlane, CarpM44* outM44);
void carp_math_getM44fromLookAt(const CarpV3A* pos, const CarpV3A* target, const CarpV3A* up, CarpM44* outM44);

void carp_math_transpose_m44(const CarpM44* m, CarpM44* outM44);
bool carp_math_inverse_m44(const CarpM44* m, CarpM44* outM44);

bool carp_math_eq_m44(const CarpM44* a, const CarpM44* b);
bool carp_math_isIdentity_m44(const CarpM44* m);

void carp_math_mul_m44_v3(const CarpM44* m, const CarpV3A* v, CarpV3A* outV3);
void carp_math_mul_v3_m44(const CarpV3A* v, const CarpM44* m, CarpV3A* outV3);

void carp_math_mul_m34_v3(const CarpM34* m, const CarpV3A* v, CarpV3A* outV3);
void carp_math_mul_v3_m34(const CarpV3A* v, const CarpM34* m, CarpV3A* outV3);

void carp_math_mul_m44_m44(const CarpM44* a, const CarpM44* b, CarpM44* outM44);
void carp_math_mul_m34_m34(const CarpM34* a, const CarpM34* b, CarpM34* outM34);













// helpers
void carp_math_print_v2(const CarpV2* v, const char* name);
void carp_math_print_v3a(const CarpV3A* v, const char* name);
void carp_math_print_q(const CarpQuat* v, const char* name);
void carp_math_print_m34(const CarpM34* v, const char* name);
void carp_math_print_m44(const CarpM44* v, const char* name);



#ifdef __cplusplus
}
#endif


#elif // defined(_M_X64) || defined(_M_AMD64)
#error "Needs x64"
#endif // defined(_M_X64) || defined(_M_AMD64)

#endif // CARP_MATH_HH
