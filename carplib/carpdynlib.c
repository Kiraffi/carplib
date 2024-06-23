#include "carpdynlib.h"
#include "carplog.h"

#if _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif // WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else // LINUX
    #include <dlfcn.h>
#endif // _WIN32

bool carp_dynLib_load(const char* fileName, CarpDynLib* outLib)
{
    if(fileName == NULL)
    {
        CARP_LOGERROR("Filename was null for dynlib load\n");
        return false;
    }
    if(outLib == NULL)
    {
        CARP_LOGERROR("Outlib was null pointer\n");
        return false;
    }

    outLib->loadedLib = NULL;
    outLib->fileName = fileName;

    #if _WIN32
        outLib->loadedLib = (const void*)LoadLibraryA(fileName);
    #else
        outLib->loadedLib = dlopen(fileName, RTLD_NOW); //1);
    #endif //_WIN32

    return outLib->loadedLib != NULL;
}

bool carp_dynLib_unload(CarpDynLib* loadedLib)
{
    if(loadedLib == NULL)
    {
        CARP_LOGERROR("Lib pointer to unload was null\n");
        return false;
    }
    if(loadedLib->loadedLib == NULL)
    {
        CARP_LOGERROR("Lib in lib pointer to unload was null\n");
        return false;
    }
    loadedLib->fileName = NULL;
    bool success = false;
    #if _WIN32
        success = FreeLibrary((HMODULE)loadedLib->loadedLib);
    #else
        success = dlclose((void*)loadedLib->loadedLib) == 0;
    #endif //_WIN32
    loadedLib->loadedLib = NULL;

    return true;
}

bool carp_dynLib_loadFn(const CarpDynLib* loadedLib, const char* functionName, CarpDynLibFn* outFn)
{
    if(loadedLib == NULL)
    {
        CARP_LOGERROR("Lib pointer to load function was null\n");
        return false;
    }
    if(loadedLib->loadedLib == NULL)
    {
        CARP_LOGERROR("Lib in lib pointer to load function was null\n");
        return false;
    }
    if(functionName == NULL)
    {
        CARP_LOGERROR("Function name to load function was null\n");
        return false;
    }

    if(outFn == NULL)
    {
        CARP_LOGERROR("OutFn was null to load function into\n");
        return false;
    }
    outFn->lib = loadedLib;
    outFn->functionName = functionName;

    #if _WIN32
        outFn->loadedFn = (void*)GetProcAddress((HMODULE)loadedLib->loadedLib, functionName);
    #else
        outFn->loadedFn = (void*)dlsym((void*)loadedLib->loadedLib, functionName);
    #endif

    return outFn->loadedFn != NULL;
}
