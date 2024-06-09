#include "carplib/carpmath.c"

#include <stdio.h>

void testf32()
{
    printf("f32 begin\n");
    f32 minv = carp_math_min_f_f(-4.0f, 23.0f);
    f32 maxv = carp_math_max_f_f(-4.0f, 23.0f);

    printf("min: %f\n", minv);
    printf("max: %f\n", maxv);

    printf("f32 end\n\n");

}

void testv2()
{
    printf("V2 begin\n");
    CarpV2 v2a = {-7.0f, 2.0f };
    CarpV2 v2b = {-4.0f, 1.0f };

    CarpV2 add_v2_v2 = carp_math_add_v2_v2(&v2a, &v2b);
    CarpV2 add_v2_f = carp_math_add_v2_f(&v2a, 3.7f);
    CarpV2 add_f_v2 = carp_math_add_f_v2(-1.8f, &v2b);

    CarpV2 sub_v2_v2 = carp_math_sub_v2_v2(&v2a, &v2b);
    CarpV2 sub_v2_f = carp_math_sub_v2_f(&v2a, 3.7f);

    CarpV2 mul_v2_v2 = carp_math_mul_v2_v2(&v2a, &v2b);
    CarpV2 mul_v2_f = carp_math_mul_v2_f(&v2a, 3.7f);
    CarpV2 mul_f_v2 = carp_math_mul_f_v2(-1.8f, &v2b);

    CarpV2 div_v2_v2 = carp_math_div_v2_v2(&v2a, &v2b);
    CarpV2 div_v2_f = carp_math_div_v2_f(&v2a, 3.7f);

    CarpV2 neg_v2 = carp_math_neg_v2(&v2a);
    CarpV2 normalized = carp_math_normalize_v2(&v2a);

    CarpV2 lerp_v2 = carp_math_lerp_v2(&v2a, &v2b, 0.25f);

    CarpV2 broadcast = carp_math_broadcast_v2(7.5f);

    CarpV2 min_v2_v2 = carp_math_min_v2_v2(&v2a, &v2b);
    CarpV2 max_v2_v2 = carp_math_max_v2_v2(&v2a, &v2b);

    f32 min_v2 = carp_math_min_v2(&v2a);
    f32 max_v2 = carp_math_max_v2(&v2a);

    f32 dotResult = carp_math_dot_v2(&v2a, &v2b);
    f32 dotNormalizedResult = carp_math_dot_v2(&normalized, &normalized);


    carp_math_print_v2(&v2a, "v2a");
    carp_math_print_v2(&v2b, "v2b");
    carp_math_print_v2(&neg_v2, "neg v2a");
    carp_math_print_v2(&normalized, "normalized v2a");

    carp_math_print_v2(&add_v2_v2, "add_v2_v2");
    carp_math_print_v2(&add_v2_f, "add_v2_f");
    carp_math_print_v2(&add_f_v2, "add_f_v2");

    carp_math_print_v2(&sub_v2_v2, "sub_v2_v2");
    carp_math_print_v2(&sub_v2_f, "sub_v2_f");

    carp_math_print_v2(&mul_v2_v2, "mul_v2_v2");
    carp_math_print_v2(&mul_v2_f, "mul_v2_f");
    carp_math_print_v2(&mul_f_v2, "mul_f_v2");

    carp_math_print_v2(&div_v2_v2, "div_v2_v2");
    carp_math_print_v2(&div_v2_f, "div_v2_f");



    carp_math_print_v2(&broadcast, "broadcast 7.5f");
    carp_math_print_v2(&lerp_v2, "lerp v2a, v2b, 0.25f");


    carp_math_print_v2(&min_v2_v2, "min_v2_v2");
    carp_math_print_v2(&max_v2_v2, "max_v2_v2");
    printf("min_v2: %f\n", min_v2);
    printf("max_v2: %f\n", max_v2);

    printf("dot: %f\n", dotResult);
    printf("dot normalized: %f\n", dotNormalizedResult);


    printf("\nSizeof v2a: %i\n", (int)sizeof(CarpV2));
    printf("offset of x: %i, y: %i\n",
        (int)offsetof(CarpV2, x),
        (int)offsetof(CarpV2, y)
    );




    printf("V2 end\n\n");

}



void testv3a()
{
    printf("V3A begin\n");
    CarpV3A v3a = {-7.0f, 2.0f, 3.0f, 10.0f};
    CarpV3A v3b = {-4.0f, 1.0f, 3.0f, 20.0f};

    CarpV3A add_v3_v3 = carp_math_add_v3_v3(&v3a, &v3b);
    CarpV3A add_v3_f = carp_math_add_v3_f(&v3a, 3.7f);
    CarpV3A add_f_v3 = carp_math_add_f_v3(-1.8f, &v3b);

    CarpV3A sub_v3_v3 = carp_math_sub_v3_v3(&v3a, &v3b);
    CarpV3A sub_v3_f = carp_math_sub_v3_f(&v3a, 3.7f);

    CarpV3A mul_v3_v3 = carp_math_mul_v3_v3(&v3a, &v3b);
    CarpV3A mul_v3_f = carp_math_mul_v3_f(&v3a, 3.7f);
    CarpV3A mul_f_v3 = carp_math_mul_f_v3(-1.8f, &v3b);

    CarpV3A div_v3_v3 = carp_math_div_v3_v3(&v3a, &v3b);
    CarpV3A div_v3_f = carp_math_div_v3_f(&v3a, 3.7f);

    CarpV3A neg_v3 = carp_math_neg_v3(&v3a);
    CarpV3A normalized = carp_math_normalize_v3(&v3a);

    CarpV3A lerp_v3 = carp_math_lerp_v3(&v3a, &v3b, 0.25f);

    CarpV3A broadcast = carp_math_broadcast_v3(7.5f);

    CarpV3A min_v3_v3 = carp_math_min_v3_v3(&v3a, &v3b);
    CarpV3A max_v3_v3 = carp_math_max_v3_v3(&v3a, &v3b);

    f32 min_v3 = carp_math_min_v3(&v3a);
    f32 max_v3 = carp_math_max_v3(&v3a);

    f32 dotResult = carp_math_dot_v3(&v3a, &v3b);
    f32 dotNormalizedResult = carp_math_dot_v3(&normalized, &normalized);

    carp_math_print_v3a(&v3a, "v3a");
    carp_math_print_v3a(&v3b, "v3b");
    carp_math_print_v3a(&neg_v3, "neg v3a");
    carp_math_print_v3a(&normalized, "normalized v3a");

    carp_math_print_v3a(&add_v3_v3, "add_v3_v3");
    carp_math_print_v3a(&add_v3_f, "add_v3_f");
    carp_math_print_v3a(&add_f_v3, "add_f_v3");

    carp_math_print_v3a(&sub_v3_v3, "sub_v3_v3");
    carp_math_print_v3a(&sub_v3_f, "sub_v3_f");

    carp_math_print_v3a(&mul_v3_v3, "mul_v3_v3");
    carp_math_print_v3a(&mul_v3_f, "mul_v3_f");
    carp_math_print_v3a(&mul_f_v3, "mul_f_v3");

    carp_math_print_v3a(&div_v3_v3, "div_v3_v3");
    carp_math_print_v3a(&div_v3_f, "div_v3_f");

    carp_math_print_v3a(&broadcast, "broadcast 7.5f");
    carp_math_print_v3a(&lerp_v3, "lerp v3a, v3b, 0.25f");


    carp_math_print_v3a(&min_v3_v3, "min_v3_v3");
    carp_math_print_v3a(&max_v3_v3, "max_v3_v3");
    printf("min_v3: %f\n", min_v3);
    printf("max_v3: %f\n", max_v3);

    printf("dot: %f\n", dotResult);
    printf("dot normalized: %f\n", dotNormalizedResult);


    printf("\nSizeof v3a: %i\n", (int)sizeof(CarpV3A));
    printf("offset of x: %i, y: %i, z: %i, w: %i\n",
        (int)offsetof(CarpV3A, x),
        (int)offsetof(CarpV3A, y),
        (int)offsetof(CarpV3A, z),
        (int)offsetof(CarpV3A, w)
    );

    printf("V3A end\n\n");
}


int main()
{
    testf32();
    testv2();
    testv3a();
    return 0;
}
