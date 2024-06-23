#ifndef CARPTIME_HH
#define CARPTIME_HH

#include "carptype.h"

typedef struct CarpTime
{
    // u64 should be enough, but just in case
    u64 storage[4];
} CarpTime;

f64 carp_time_getDifference(const CarpTime* start, const CarpTime* end);
void carp_time_getCurrentTime(CarpTime* outTime);



#endif //CARPTIME_HH
