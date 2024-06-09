#include "carplib/carpmath.c"

#include <stdio.h>





void testv3()
{
    printf("V3 begin\n");
    CarpV3A v3a = {-7.0f, 2.0f, 3.0f, 0.0f};
    CarpV3A v3b = {-4.0f, 1.0f, 3.0f, 0.0f};

    CarpV3A sum_v3_v3 = carp_math_add_v3_v3(&v3a, &v3b);
    CarpV3A sum_v3_f = carp_math_add_v3_f(&v3a, 3.7f);
    CarpV3A sum_f_v3 = carp_math_add_f_v3(-1.8f, &v3b);

    CarpV3A sub_v3_v3 = carp_math_sub_v3_v3(&v3a, &v3b);
    CarpV3A sub_v3_f = carp_math_sub_v3_f(&v3a, 3.7f);

    CarpV3A mul_v3_v3 = carp_math_mul_v3_v3(&v3a, &v3b);
    CarpV3A mul_v3_f = carp_math_mul_v3_f(&v3a, 3.7f);
    CarpV3A mul_f_v3 = carp_math_mul_f_v3(-1.8f, &v3b);

    CarpV3A div_v3_v3 = carp_math_div_v3_v3(&v3a, &v3b);
    CarpV3A div_v3_f = carp_math_div_v3_f(&v3a, 3.7f);

    CarpV3A neg = carp_math_neg_v3(&v3a);
    CarpV3A broadcast = carp_math_broadcast_f32_v3(7.5f);

    CarpV3A min_v3_v3 = carp_math_min_v3_v3(&v3a, &v3b);
    CarpV3A max_v3_v3 = carp_math_max_v3_v3(&v3a, &v3b);

    f32 dotResult = carp_math_dot_v3(&v3a, &v3b);

    carp_math_print_v3a(&v3a, "v3a");
    carp_math_print_v3a(&v3b, "v3b");
    carp_math_print_v3a(&neg, "neg v3a");

    carp_math_print_v3a(&sum_v3_v3, "sum_v3_v3");
    carp_math_print_v3a(&sum_v3_f, "sum_v3_f");
    carp_math_print_v3a(&sum_f_v3, "sum_f_v3");

    carp_math_print_v3a(&sub_v3_v3, "sub_v3_v3");
    carp_math_print_v3a(&sub_v3_f, "sub_v3_f");

    carp_math_print_v3a(&mul_v3_v3, "mul_v3_v3");
    carp_math_print_v3a(&mul_v3_f, "mul_v3_f");
    carp_math_print_v3a(&mul_f_v3, "mul_f_v3");

    carp_math_print_v3a(&div_v3_v3, "div_v3_v3");
    carp_math_print_v3a(&div_v3_f, "div_v3_f");
    carp_math_print_v3a(&broadcast, "broadcast 7.5f");


    carp_math_print_v3a(&min_v3_v3, "min_v3_v3");
    carp_math_print_v3a(&max_v3_v3, "max_v3_v3");


    printf("dot: %f\n", dotResult);


    printf("\nSizeof v3a: %i\n", (int)sizeof(CarpV3A));
    printf("offset of x: %i, y: %i, z: %i, w: %i\n",
        (int)offsetof(CarpV3A, x),
        (int)offsetof(CarpV3A, y),
        (int)offsetof(CarpV3A, z),
        (int)offsetof(CarpV3A, w)
    );

    printf("V3 end\n\n");
}


int main()
{
    testv3();
    return 0;
}
