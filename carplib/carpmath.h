#ifndef CARP_MATH_HH
#define CARP_MATH_HH

#include "carptype.h"

#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)

#ifndef __TINYC__
    #include <immintrin.h>
#else

    typedef double __m128d;
    typedef float __m128;

#endif

#if __TINYC__
    #define ASM_LABEL(name) #name
#else
    #define ASM_LABEL(name) #name"%="
#endif

#define CARP_EPSILON (1.0e-5f)


typedef struct CarpV2
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
        };
        _Alignas(8) float v[2];
        _Alignas(8) uint64_t v2;
    };
} CarpV2;



typedef struct CarpV3A
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        _Alignas(16) float v[4];
        _Alignas(16) __m128 simdv3a;
    };
} CarpV3A;




typedef struct CarpQuat
{
    union
    {
        struct
        {
            float vx;
            float vy;
            float vz;
            float w;
        };
        _Alignas(16) float v[4];
        _Alignas(16) __m128 simdQuat;
    };
} CarpQuat;






typedef struct CarpMat4x4
{
    union
    {
        struct
        {
            float _00;
            float _01;
            float _02;
            float _03;

            float _10;
            float _11;
            float _12;
            float _13;

            float _20;
            float _21;
            float _22;
            float _23;

            float _30;
            float _31;
            float _32;
            float _33;
        };
        _Alignas(16) float v[16];
        struct
        {
            _Alignas(16) __m128 simdR0;
            _Alignas(16) __m128 simdR1;
            _Alignas(16) __m128 simdR2;
            _Alignas(16) __m128 simdR3;
        };
    };
} CarpMat4x4;

typedef struct CarpMat3x4
{
    union
    {
        struct
        {
            float _00;
            float _01;
            float _02;
            float _03;

            float _10;
            float _11;
            float _12;
            float _13;

            float _20;
            float _21;
            float _22;
            float _23;
        };
        _Alignas(16) float v[12];
        struct
        {
            _Alignas(16) __m128 simdR0;
            _Alignas(16) __m128 simdR1;
            _Alignas(16) __m128 simdR2;
            _Alignas(16) __m128 simdR3;
        };
    };
} CarpMat3x4;




#ifdef __cplusplus
extern "C" {
#endif

// f32
f32 carp_math_min_f_f(f32 a, f32 b);
f32 carp_math_max_f_f(f32 a, f32 b);

// v2
void carp_math_broadcast_v2(f32 f, CarpV2* outV2);
void carp_math_neg_v2(const CarpV2* a, CarpV2* outV2);
void carp_math_normalize_v2(const CarpV2* a, CarpV2* outV2);
void carp_math_lerp_v2(const CarpV2* a, const CarpV2* b, f32 t, CarpV2* outV2);

void carp_math_add_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_add_v2_f(const CarpV2* a, f32 f, CarpV2* outV2);
void carp_math_add_f_v2(f32 f, const CarpV2* b, CarpV2* outV2);

void carp_math_sub_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_sub_v2_f(const CarpV2* a, f32 f, CarpV2* outV2);

void carp_math_mul_v2_v2(const CarpV2* a, const CarpV2* b, CarpV2* outV2);
void carp_math_mul_v2_f(const CarpV2* a, f32 f, CarpV2* outV2);
void carp_math_mul_f_v2(f32 f, const CarpV2* b, CarpV2* outV2);

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
void carp_math_broadcast_v3(f32 f, CarpV3A* outV3);
void carp_math_neg_v3(const CarpV3A* a, CarpV3A* outV3);
void carp_math_normalize_v3(const CarpV3A* a, CarpV3A* outV3);
void carp_math_lerp_v3(const CarpV3A* a, const CarpV3A* b, f32 t, CarpV3A* outV3);

void carp_math_add_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_add_v3_3(const CarpV3A* a, f32 f, CarpV3A* outV3);
void carp_math_add_f_v3(f32 f, const CarpV3A* b, CarpV3A* outV3);

void carp_math_sub_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_sub_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3);

void carp_math_mul_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_mul_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3);
void carp_math_mul_f_v3(f32 f, const CarpV3A* b, CarpV3A* outV3);

void carp_math_div_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_div_v3_f(const CarpV3A* a, f32 f, CarpV3A* outV3);

void carp_math_min_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_max_v3_v3(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_cross(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_project(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);
void carp_math_reject(const CarpV3A* a, const CarpV3A* b, CarpV3A* outV3);

f32 carp_math_dot_v3(const CarpV3A* a, const CarpV3A* b);
f32 carp_math_min_v3(const CarpV3A* a);
f32 carp_math_max_v3(const CarpV3A* a);
f32 carp_math_sqrLen_v3(const CarpV3A* a);
f32 carp_math_len_v3(const CarpV3A* a);




// quat

CarpQuat carp_math_neg_q(const CarpQuat* v);
CarpQuat carp_math_add_q_q(const CarpQuat* a, const CarpQuat* b);
CarpQuat carp_math_sub_q_q(const CarpQuat* a, const CarpQuat* b);

float carp_math_dot_q(const CarpQuat* q1, const CarpQuat* q2);
CarpQuat carp_math_mul_q_q(const CarpQuat* a, const CarpQuat* b);
CarpQuat carp_math_normalize_q(const CarpQuat* q);
CarpQuat carp_math_conjugate_q(const CarpQuat* q);
CarpV3A carp_math_rotate_V3_q(const CarpV3A* v, const CarpQuat* q);
CarpQuat carp_math_mul_q_f32(const CarpQuat* q, float t);
CarpQuat carp_math_mul_f32_q(float t, const CarpQuat* q);
CarpQuat carp_math_lerp_q(CarpQuat const* q1, CarpQuat const* q2, float t);
CarpQuat carp_math_slerp_q(CarpQuat const* q1, CarpQuat const* q2, float t);

void carp_math_getAxisFromQuat(const CarpQuat* quat, CarpV3A* right, CarpV3A* up, CarpV3A* forward);
CarpQuat carp_math_getQuatFromAxisAngle(const CarpV3A* v, float angle);
CarpQuat carp_math_getQuatFromNormalizedVectors(const CarpV3A* from, const CarpV3A* toVector);
void carp_math_getDirectionsFromPitchYawRoll(
    float pitch, float yaw, float roll, CarpV3A* rightDir, CarpV3A* upDir, CarpV3A* forwardDir);





// mat
CarpMat3x4 carp_math_m34_getFromQuaternion(const CarpQuat* quat);
CarpMat3x4 carp_math_m34_getFromScale(const CarpV3A* scale);
CarpMat3x4 carp_math_m34_getFromTranslation(const CarpV3A* pos);

CarpMat4x4 carp_math_m44_createOrtho(float width, float height, float nearPlane, float farPlane);
CarpMat4x4 carp_math_m44_createPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
CarpMat4x4 carp_math_m44_fromLookAt(const CarpV3A* pos, const CarpV3A* target, const CarpV3A* up);

CarpMat4x4 carp_math_m44_transpose(const CarpMat4x4* m);
CarpMat4x4 carp_math_m44_inverse(const CarpMat4x4* m);

bool carp_math_eq_m44(const CarpMat4x4* a, const CarpMat4x4* b);
bool carp_math_isIdentity_m44(const CarpMat4x4* m);

CarpV3A carp_math_m44__mul_v3(const CarpMat4x4* m, const CarpV3A* v);
CarpV3A carp_math_mul_v3_m44(const CarpV3A* v, const CarpMat4x4* m);

CarpV3A carp_math_mul_m34_v3(const CarpMat3x4* m, const CarpV3A* v);
CarpV3A carp_math_mul_v3_m34(const CarpV3A* v, const CarpMat3x4* m);

CarpMat4x4 carp_math_mul_m44_m44(const CarpMat4x4* a, const CarpMat4x4* b);
CarpMat4x4 carp_math_mul_m44_m34(const CarpMat4x4* a, const CarpMat3x4* b);
CarpMat4x4 carp_math_mul_m34_m44(const CarpMat3x4* a, const CarpMat4x4* b);
CarpMat3x4 carp_math_mul_m34_m34(const CarpMat3x4* a, const CarpMat3x4* b);













// helpers
void carp_math_print_v2(const CarpV2* v, const char* name);
void carp_math_print_v3a(const CarpV3A* v, const char* name);



#ifdef __cplusplus
}
#endif


#elif // defined(_M_X64) || defined(_M_AMD64)
#error "Needs x64"
#endif // defined(_M_X64) || defined(_M_AMD64)

#endif // CARP_MATH_HH
