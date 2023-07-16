//#define WIN32_LEAN_AND_MEAN 1
//#include <windows.h>

#include <stdlib.h>

#include "mymemory.h"


MyMemory* s_myMemory = NULL;


b8 myMemory_init()
{
    if(s_myMemory != NULL)
        return false;

    s_myMemory = calloc(1, sizeof(MyMemory));
    return s_myMemory != NULL;
}

void myMemory_deinit()
{
    if(s_myMemory != NULL)
    {
        free(s_myMemory);
    }
    s_myMemory = NULL;
}

MyMemory* myMemory_get()
{
    return s_myMemory;
}