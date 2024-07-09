#include "hotreload.h"
#include "hotreloadfn.h"

#include "carplib/carplog.h"

s32 CARP_LIBEXPORT hotReloadTestFn(MainLoad mainLoad, s32 a, s32 b)
{
    CARP_LOG("Hello from hotreloaded fn again\n");
    CARP_LOG("The parameters are %i, %i\n", a, b);
    s32 result = a * b + 19;
    if(mainLoad.mainFn)
    {
        CARP_LOG("Called main: %i\n", mainLoad.mainFn(result * 3));
        CARP_LOG("Called main imported: %i\n", mainLoad.mainFn(result * 5));
    }
    return result;
}