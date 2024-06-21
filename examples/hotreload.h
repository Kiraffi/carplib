#ifndef HOTRELOAD_HH
#define HOTRELOAD_HH

#include "carplib/carptype.h"

typedef int (*mainFunctionPtr)(int);
typedef struct MainLoad
{
    mainFunctionPtr mainFn;
    void* hotReloadMemory;
} MainLoad;



typedef int (*hotReloadTestFnPtr)(MainLoad mainLoad, int, int);

#endif //HOTRELOAD_HH
