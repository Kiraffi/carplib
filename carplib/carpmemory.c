
#include <malloc.h> // malloc, otherwise tcc will coredump
#include <stdlib.h> //free

#include "carpmemory.h"


static CarpMemory* s_carpMemory = NULL;


CARP_FN b8 carp_memory_init(void)
{
    if(s_carpMemory != NULL)
        return false;

    s_carpMemory = (CarpMemory*) calloc(1, sizeof(CarpMemory));
    return s_carpMemory != NULL;
}

CARP_FN void carp_memory_destroy(void)
{
    if(s_carpMemory != NULL)
    {
        free(s_carpMemory);
    }
    s_carpMemory = NULL;
}

CARP_FN CarpMemory* carp_memory_get(void)
{
    return s_carpMemory;
}
