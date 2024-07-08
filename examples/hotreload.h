#ifndef HOTRELOAD_HH
#define HOTRELOAD_HH

#include "carplib/carptype.h"

typedef s32 (*mainFunctionPtr)(s32);
typedef struct MainLoad
{
    mainFunctionPtr mainFn;
    void* hotReloadMemory;
} MainLoad;



typedef s32 (*hotReloadTestFnPtr)(MainLoad mainLoad, int, int);

#endif //HOTRELOAD_HH
