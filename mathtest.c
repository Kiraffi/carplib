#include "carplib/carpmath.c"

#include <stdio.h>


int main()
{
    CarpV3A v3a = {1.0f, 2.0f, 3.0f, 0.0f};
    CarpV3A v3b = {4.0f, 5.0f, 6.0f, 0.0f};

    CarpV3A sum = carp_math_add_v3_v3(&v3a, &v3b);
    CarpV3A sub = carp_math_sub_v3_v3(&v3a, &v3b);
    CarpV3A mul = carp_math_mul_v3_v3(&v3a, &v3b);
    CarpV3A div = carp_math_div_v3_v3(&v3a, &v3b);

    CarpV3A neg = carp_math_neg_v3(&v3a);

    carp_math_print_v3a(&v3a, "v3a");
    carp_math_print_v3a(&v3b, "v3b");
    carp_math_print_v3a(&neg, "neg v3a");
    carp_math_print_v3a(&sum, "sum");
    carp_math_print_v3a(&sub, "sub");
    carp_math_print_v3a(&mul, "mul");
    carp_math_print_v3a(&div, "div");

    return 0;
}