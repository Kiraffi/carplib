#include "carptime.h"


#include <sys/time.h>

f64 carp_time_getDifference(const CarpTime* start, const CarpTime* end)
{
    f64 result;
    #if CARP_LINUX
    struct timeval* a = (struct timeval*)start;
    struct timeval* b = (struct timeval*)end;
    result = b->tv_sec - a->tv_sec;
    result += (b->tv_usec - a->tv_usec) * 1.0e-6f;
    #else
    #error "Get current time not implemented"
    #endif
    return result;
}
void carp_time_getCurrentTime(CarpTime* outTime)
{
    #if CARP_LINUX
    //clock_gettime not part of clang time header?
    gettimeofday((struct timeval*)outTime, NULL);
    #else
    #error "Get current time not implemented"
    #endif
}

