//#define WIN32_LEAN_AND_MEAN 1
//#include <windows.h>

#include <stdlib.h>

#include "carpmemory.h"


CarpMemory* s_carpMyMemory = NULL;


b8 carpMemory_init()
{
    if(s_carpMyMemory != NULL)
        return false;

    s_carpMyMemory = calloc(1, sizeof(CarpMemory));
    return s_carpMyMemory != NULL;
}

void carpMemory_deinit()
{
    if(s_carpMyMemory != NULL)
    {
        free(s_carpMyMemory);
    }
    s_carpMyMemory = NULL;
}

CarpMemory* carpMemory_get()
{
    return s_carpMyMemory;
}