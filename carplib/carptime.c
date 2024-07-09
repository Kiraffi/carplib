#include "carptime.h"


#if CARP_LINUX
    #include <sys/time.h>
#elif CARP_WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif // WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else 
    #error "Not supported system"
#endif // CARP_LINUX

f64 carp_time_getDifference(const CarpTime* start, const CarpTime* end)
{
    f64 result;
    #if CARP_LINUX
    struct timeval* a = (struct timeval*)start;
    struct timeval* b = (struct timeval*)end;
    result = b->tv_sec - a->tv_sec;
    result += (b->tv_usec - a->tv_usec) * 1.0e-6f;
    #elif CARP_WIN32

    int64_t* startTime = (int64_t*)start; 
    int64_t* endTime = (int64_t*)end; 

    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    result = ((f64)(*endTime - *startTime)) / ((f64)Frequency.QuadPart);

    #else
        #error "Get current time not implemented"
    #endif // CARP_LINUX
    return result;
}
void carp_time_getCurrentTime(CarpTime* outTime)
{
    #if CARP_LINUX
    //clock_gettime not part of clang time header?
    gettimeofday((struct timeval*)outTime, NULL);
    #elif CARP_WIN32
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks))
    {
    }
    *((int64_t*)(outTime)) = ticks.QuadPart; 
    #else
    #error "Get current time not implemented"
    #endif
}

