#ifndef CARP_TYPE_HH
#define CARP_TYPE_HH

#include <inttypes.h> // PRIi64
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef int8_t b8;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#if COMPILE_SINGLE_SOURCE
#define CARP_FN static
#else
#define CARP_FN
#endif

#if _WIN32
#define CARP_WIN32 1
#elif defined(__linux__) || defined(__unix__)
#define CARP_LINUX 1
#else
#error "Not supported system"
#endif // _WIN32

#if __TINYC__
#define CARP_TCC 1
#endif //__TINYC__

#if CARP_WIN32
#ifndef CARP_LIBEXPORT
#define CARP_LIBEXPORT __declspec(dllexport)
#endif //CARP_LIBEXPORT
#elif CARP_LINUX
#ifndef CARP_LIBEXPORT
#define CARP_LIBEXPORT /* */
#endif //CARP_LIBEXPORT
#else
#error "Unsupported operating system!"
#endif // CARP_WIN32


#endif // CARP_TYPE_HH
