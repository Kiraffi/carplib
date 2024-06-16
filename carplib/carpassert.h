#ifndef CARPASSERT_HH
#define CARPASSERT_HH

#include "carplog.h"

#if _MSC_VER
#define CARP_DEBUG_BREAK() __debugbreak()
#else
#include <signal.h>
//#define CARP_DEBUG_BREAK() __builtin_trap()
#define CARP_DEBUG_BREAK() raise(SIGTRAP)
#endif

#define CARP_ASSERTS 1
#if CARP_ASSERTS

#define CARP_ASSERT_STRING(STUFF, STUFFSTRING) \
do \
{ \
    if (STUFF) {} \
    else  \
    {  \
        CARP_LOG("Assertion: %s\n", STUFFSTRING); \
        CARP_DEBUG_BREAK(); \
    } \
} while (0)

#define CARP_ASSERT_STRING_RETURN(STUFF, STUFFSTRING, RETURN_VALUE) \
do \
{ \
    if (STUFF) {} \
    else  \
    {  \
        CARP_LOG("Assertion: %s\n", STUFFSTRING); \
        CARP_DEBUG_BREAK(); \
        return RETURN_VALUE; \
    } \
} while (0)

#define CARP_ASSERT(STUFF) CARP_ASSERT_STRING(STUFF, #STUFF)
#define CARP_ASSERT_RETURN(STUFF, RETURN_VALUE) CARP_ASSERT_STRING_RETURN(STUFF, #STUFF, RETURN_VALUE)

#else 
#define CARP_ASSERT(STUFF) CARP_ASSERT_STRING(STUFF, #STUFF)
#define CARP_ASSERT_RETURN(STUFF, RETURN_VALUE) \
do \
{ \
    if(STUFF) {}\
    else { CARP_DEBUG_BREAK(); RETURN_VALUE; } \
} while(0)

#endif /* CARPASSERTS */

#endif /* CARPASSERT_HH */
