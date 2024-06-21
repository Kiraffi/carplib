#ifndef CARPLOG_HH
#define CARPLOG_HH

#include <stdio.h>

#define CARP_USE_LOG 1
#if CARP_USE_LOG == 1

//#define CARP_HELPER_LOG(LOG_TEXT_STRING, ...) \
//                do { \
//                    fprintf(stdout, LOG_TEXT_STRING, __VA_ARGS__); \
//                } while(0)

#define CARP_HELPER_LOG(...) \
                do { \
                    fprintf(__VA_ARGS__); \
                } while(0)
#else
//#define HELPER_LOG(LOG_TEXT_STRING, ...)
#define HELPER_LOG(...)
#endif // LOGDEBUG

//#define CARP_LOG(LOG_TEXT_STRING, ...) CARP_HELPER_LOG(LOG_TEXT_STRING, ##__VA_ARGS__)
//#define CARP_LOGINFO(LOG_TEXT_STRING, ...) CARP_HELPER_LOG("Info: " LOG_TEXT_STRING , ##__VA_ARGS__)
//#define CARP_LOGDEBUG(LOG_TEXT_STRING, ...) CARP_HELPER_LOG("Debug: " LOG_TEXT_STRING, ##__VA_ARGS__)
//#define CARP_LOGERROR(LOG_TEXT_STRING, ...) CARP_HELPER_LOG("Error: " LOG_TEXT_STRING, ##__VA_ARGS__)

#define CARP_LOG(...) CARP_HELPER_LOG(stdout, __VA_ARGS__)
#define CARP_LOGINFO(...) CARP_HELPER_LOG(stdout, "Info: " __VA_ARGS__)
#define CARP_LOGDEBUG(...) CARP_HELPER_LOG(stdout, "Debug: " __VA_ARGS__)
#define CARP_LOGERROR(...) CARP_HELPER_LOG(stderr, "Error: " __VA_ARGS__)

#endif //CARPLOG_HH
