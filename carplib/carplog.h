#ifndef CARPLOG_HH
#define CARPLOG_HH

#include <stdio.h>

#define CARP_LOGDEBUG 1
#if CARP_LOGDEBUG == 1

#define CARP_HELPER_LOG(LOG_TEXT_STRING, ...) \
                do { \
                    printf(LOG_TEXT_STRING, ##__VA_ARGS__); \
                } while(0)
#else
#define HELPER_LOG(LOG_TEXT_STRING, ...)
#endif // LOGDEBUG

#define CARP_LOG(LOG_TEXT_STRING, ...) CARP_HELPER_LOG(LOG_TEXT_STRING, ##__VA_ARGS__)
#define CARP_LOGINFO(LOG_TEXT_STRING, ...) CARP_HELPER_LOG("Info: " LOG_TEXT_STRING, ##__VA_ARGS__)
#define CARP_LOGERROR(LOG_TEXT_STRING, ...) CARP_HELPER_LOG("Error: " LOG_TEXT_STRING, ##__VA_ARGS__)

#endif //CARPLOG_HH
