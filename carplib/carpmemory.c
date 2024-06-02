//#define WIN32_LEAN_AND_MEAN 1
//#include <windows.h>

#include <stdlib.h>

#include "carpmemory.h"


CarpMemory* s_carpMemory = NULL;


b8 carp_memory_init()
{
    if(s_carpMemory != NULL)
        return false;

    s_carpMemory = calloc(1, sizeof(CarpMemory));
    return s_carpMemory != NULL;
}

void carp_memory_destroy()
{
    if(s_carpMemory != NULL)
    {
        free(s_carpMemory);
    }
    s_carpMemory = NULL;
}

CarpMemory* carp_memory_get()
{
    return s_carpMemory;
}