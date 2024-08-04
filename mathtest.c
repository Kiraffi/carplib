
#include "carplib/carpbuffer.c"
#include "carplib/carplib.c"
#include "carplib/carpmath.c"

#include "carplib/carplog.h"

#include <stdalign.h>


void testf32(void)
{
    CARP_LOG("f32 begin\n");
    f32 minv = carp_math_min_f_f(-4.0f, 23.0f);
    f32 maxv = carp_math_max_f_f(-4.0f, 23.0f);

    CARP_LOG("min: %f\n", minv);
    CARP_LOG("max: %f\n", maxv);

    CARP_LOG("f32 end\n\n");

}

void testv2(void)
{
    CARP_LOG("V2 begin\n");
    CarpV2 v2a = {-7.0f, 2.0f };
    CarpV2 v2b = {-4.0f, 1.0f };
    carp_math_print_v2(&v2a, "v2a at begin");
    carp_math_print_v2(&v2b, "v2b at begin");

    CarpV2 zero_v2; carp_math_zero_v2(&zero_v2);
    CarpV2 neg_v2; carp_math_neg_v2(&v2a, &neg_v2);
    CarpV2 normalized; carp_math_normalize_v2(&v2a, &normalized);
    CarpV2 set_v2; carp_math_set_v2(&v2a, &set_v2);

    CarpV2 add_v2_v2; carp_math_add_v2_v2(&v2a, &v2b, &add_v2_v2);
    CarpV2 add_v2_f; carp_math_add_v2_f(&v2a, 3.7f, &add_v2_f);

    CarpV2 sub_v2_v2; carp_math_sub_v2_v2(&v2a, &v2b, &sub_v2_v2);
    CarpV2 sub_v2_f; carp_math_sub_v2_f(&v2a, 3.7f, &sub_v2_f);

    CarpV2 mul_v2_v2; carp_math_mul_v2_v2(&v2a, &v2b, &mul_v2_v2);
    CarpV2 mul_v2_f; carp_math_mul_v2_f(&v2a, 3.7f, &mul_v2_f);

    CarpV2 div_v2_v2; carp_math_div_v2_v2(&v2a, &v2b, &div_v2_v2);
    CarpV2 div_v2_f; carp_math_div_v2_f(&v2a, 3.7f, &div_v2_f);

    CarpV2 broadcast; carp_math_broadcast_v2(7.5f, &broadcast);
    CarpV2 lerp_v2; carp_math_lerp_v2(&v2a, &v2b, 0.25f, &lerp_v2);

    CarpV2 min_v2_v2; carp_math_min_v2_v2(&v2a, &v2b, &min_v2_v2);
    CarpV2 max_v2_v2; carp_math_max_v2_v2(&v2a, &v2b, &max_v2_v2);

    f32 min_v2 = carp_math_min_v2(&v2a);
    f32 max_v2 = carp_math_max_v2(&v2a);

    f32 dotResult = carp_math_dot_v2(&v2a, &v2b);
    f32 dotNormalizedResult = carp_math_dot_v2(&normalized, &normalized);

    carp_math_print_v2(&zero_v2, "zero v2");
    carp_math_print_v2(&neg_v2, "neg v2a");
    carp_math_print_v2(&normalized, "normalized v2a");
    carp_math_print_v2(&set_v2, "set v2, should be same as v2a");

    carp_math_print_v2(&add_v2_v2, "add_v2_v2");
    carp_math_print_v2(&add_v2_f, "add_v2_f");

    carp_math_print_v2(&sub_v2_v2, "sub_v2_v2");
    carp_math_print_v2(&sub_v2_f, "sub_v2_f");

    carp_math_print_v2(&mul_v2_v2, "mul_v2_v2");
    carp_math_print_v2(&mul_v2_f, "mul_v2_f");

    carp_math_print_v2(&div_v2_v2, "div_v2_v2");
    carp_math_print_v2(&div_v2_f, "div_v2_f");

    carp_math_print_v2(&broadcast, "broadcast 7.5f");
    carp_math_print_v2(&lerp_v2, "lerp v2a, v2b, 0.25f");

    carp_math_print_v2(&min_v2_v2, "min_v2_v2");
    carp_math_print_v2(&max_v2_v2, "max_v2_v2");
    CARP_LOG("min_v2: %f\n", min_v2);
    CARP_LOG("max_v2: %f\n", max_v2);

    CARP_LOG("dot: %f\n", dotResult);
    CARP_LOG("dot normalized: %f\n", dotNormalizedResult);

    CARP_LOG("\nSizeof v2a: %i\n", (int)sizeof(CarpV2));
    CARP_LOG("Align of v2: %i\n", (int)alignof(CarpV2));

    CARP_LOG("offset of x: %i, y: %i\n",
        (int)offsetof(CarpV2, x),
        (int)offsetof(CarpV2, y)
    );

    carp_math_print_v2(&v2a, "v2a at end");
    carp_math_print_v2(&v2b, "v2b at end");

    CARP_LOG("V2 end\n\n");

}



void testv3a(void)
{
    CARP_LOG("V3A begin\n");
    CarpV3A v3a = {-7.0f, 2.0f, 3.0f, 10.0f};
    CarpV3A v3b = {-4.0f, 1.0f, 3.0f, 20.0f};
    carp_math_print_v3a(&v3a, "v3a at begin");
    carp_math_print_v3a(&v3b, "v3b at begin");

    CarpV3A zero_v3; carp_math_zero_v3(&zero_v3);
    CarpV3A neg_v3; carp_math_neg_v3(&v3a, &neg_v3);
    CarpV3A normalized; carp_math_normalize_v3(&v3a, &normalized);
    CarpV3A set_v3; carp_math_set_v3(&v3a, &set_v3);

    CarpV3A add_v3_v3; carp_math_add_v3_v3(&v3a, &v3b, &add_v3_v3);
    CarpV3A add_v3_f; carp_math_add_v3_f(&v3a, 3.9f, &add_v3_f);

    CarpV3A sub_v3_v3; carp_math_sub_v3_v3(&v3a, &v3b, &sub_v3_v3);
    CarpV3A sub_v3_f; carp_math_sub_v3_f(&v3a, 3.7f, &sub_v3_f );

    CarpV3A mul_v3_v3; carp_math_mul_v3_v3(&v3a, &v3b, &mul_v3_v3);
    CarpV3A mul_v3_f; carp_math_mul_v3_f(&v3a, 3.7f, &mul_v3_f);

    CarpV3A div_v3_v3; carp_math_div_v3_v3(&v3a, &v3b, &div_v3_v3);
    CarpV3A div_v3_f; carp_math_div_v3_f(&v3a, 3.7f, &div_v3_f);

    CarpV3A broadcast; carp_math_broadcast_v3(7.5f, &broadcast);
    CarpV3A lerp_v3; carp_math_lerp_v3(&v3a, &v3b, 0.25f, &lerp_v3);

    CarpV3A min_v3_v3; carp_math_min_v3_v3(&v3a, &v3b, &min_v3_v3);
    CarpV3A max_v3_v3; carp_math_max_v3_v3(&v3a, &v3b, &max_v3_v3);

    f32 min_v3 = carp_math_min_v3(&v3a);
    f32 max_v3 = carp_math_max_v3(&v3a);

    f32 dotResult = carp_math_dot_v3(&v3a, &v3b);
    f32 dotNormalizedResult = carp_math_dot_v3(&normalized, &normalized);


    carp_math_print_v3a(&zero_v3, "zero v3");
    carp_math_print_v3a(&neg_v3, "neg v3a");
    carp_math_print_v3a(&normalized, "normalized v3a");
    carp_math_print_v3a(&set_v3, "set v3, should be same as v3a");

    carp_math_print_v3a(&add_v3_v3, "add_v3_v3");
    carp_math_print_v3a(&add_v3_f, "add_v3_f");

    carp_math_print_v3a(&sub_v3_v3, "sub_v3_v3");
    carp_math_print_v3a(&sub_v3_f, "sub_v3_f");

    carp_math_print_v3a(&mul_v3_v3, "mul_v3_v3");
    carp_math_print_v3a(&mul_v3_f, "mul_v3_f");

    carp_math_print_v3a(&div_v3_v3, "div_v3_v3");
    carp_math_print_v3a(&div_v3_f, "div_v3_f");

    carp_math_print_v3a(&broadcast, "broadcast 7.5f");
    carp_math_print_v3a(&lerp_v3, "lerp v3a, v3b, 0.25f");

    carp_math_print_v3a(&min_v3_v3, "min_v3_v3");
    carp_math_print_v3a(&max_v3_v3, "max_v3_v3");
    CARP_LOG("min_v3: %f\n", min_v3);
    CARP_LOG("max_v3: %f\n", max_v3);

    CARP_LOG("dot: %f\n", dotResult);
    CARP_LOG("dot normalized: %f\n", dotNormalizedResult);

    CARP_LOG("\nSizeof v3a: %i\n", (int)sizeof(CarpV3A));
    CARP_LOG("Align of v3a: %i\n", (int)alignof(CarpV3A));
    CARP_LOG("offset of x: %i, y: %i, z: %i, w: %i\n",
        (int)offsetof(CarpV3A, x),
        (int)offsetof(CarpV3A, y),
        (int)offsetof(CarpV3A, z),
        (int)offsetof(CarpV3A, w)
    );
    carp_math_print_v3a(&v3a, "v3a at end");
    carp_math_print_v3a(&v3b, "v3b at end");

    CARP_LOG("V3A end\n\n");
}

void testQuat(void)
{
    CARP_LOG("Quat begin\n");

    CarpV3A right = {1.0f, 0.0f, 0.0f, 0.0f};
    CarpV3A up = {0.0f, 1.0f, 0.0f, 0.0f};
    CarpQuat a = {0.0f, 0.0f, 0.0f, 1.0f};
    CarpQuat b; carp_math_getQuatFromAxisAngle(&right, CARP_PI * 0.5f, &b);
    CarpQuat c; carp_math_mul_q_q(&a, &b, &c);

    carp_math_print_q(&b, "Quat after multiply");
    CARP_LOG("Quat end\n\n");
}

void testMat(void)
{
    CARP_LOG("Mat begin\n");

    CarpM34 m34;
    carp_math_zero_m34(&m34);

    CarpM44 m44;
    carp_math_zero_m44(&m44);

    carp_math_print_m34(&m34, "Zeroed m34");
    CARP_LOG("\n");
    carp_math_print_m44(&m44, "Zeroed m44");
    CARP_LOG("Mat end\n\n");
}


s32 main(void)
{
    CARP_LOG("Begin test\n");
    testf32();
    testv2();
    testv3a();
    testQuat();
    testMat();

    return 0;
}
