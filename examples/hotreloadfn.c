#include "hotreload.h"

#include "carplib/carptype.h"
#include <stdio.h>

int CARP_LIBEXPORT hotReloadTestFn(int a, int b)
{
    printf("Hello from hotreloaded fn again\n");
    printf("The parameters are %i, %i\n", a, b);

    return a * b + 10;
}