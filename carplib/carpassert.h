#ifndef CARPASSERT_HH
#define CARPASSERT_HH

#include "carplog.h"

#if _MSC_VER
#define CARP_DEBUG_BREAK() __debugbreak()
#elif __TINYC__ && (defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__))
#define CARP_DEBUG_BREAK() do { __asm__ volatile("int $0x03"); } while(0)
#else
#include <signal.h>
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
