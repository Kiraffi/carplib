#include "carpmemory.h"

#include "carpbuffer.h"

static CarpMemory* s_carpMemory = NULL;


CARP_FN b8 carp_memory_init(void)
{
    if(s_carpMemory != NULL)
        return false;

    s_carpMemory = (CarpMemory*) carp_lib_calloc(1, sizeof(CarpMemory));
    return s_carpMemory != NULL;
}

CARP_FN void carp_memory_destroy(void)
{
    if(s_carpMemory != NULL)
    {
        carp_lib_free(s_carpMemory);
    }
    s_carpMemory = NULL;
}

CARP_FN CarpMemory* carp_memory_get(void)
{
    return s_carpMemory;
}
