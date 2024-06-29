#ifndef CARP_DYNLIB_HH
#define CARP_DYNLIB_HH

#include "carptype.h"

typedef struct CarpDynLib
{
    const void* loadedLib;
    const char* fileName;
} CarpDynLib;


typedef struct CarpDynLibFn
{
    const CarpDynLib* lib;
    void* loadedFn;
    const char* functionName;
} CarpDynLibFn;



CARP_FN bool carp_dynLib_load(const char* fileName, CarpDynLib* outLib);
CARP_FN bool carp_dynLib_unload(CarpDynLib* loadedLib);
CARP_FN bool carp_dynLib_loadFn(const  CarpDynLib* loadedLib, const char* functionName, CarpDynLibFn* outFn);

#endif // CARP_DYNLIB_HH
