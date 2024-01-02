#ifndef _WIN32

#include "co/time.h"

#include <sys/time.h>
#include <time.h>

namespace co {
namespace now {
namespace xx {

#ifdef CLOCK_MONOTONIC

inline int64_t ns() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return static_cast<int64_t>(t.tv_sec) * 1000000000 + t.tv_nsec;
}

inline int64_t us() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return static_cast<int64_t>(t.tv_sec) * 1000000 + t.tv_nsec / 1000;
}

inline int64_t ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return static_cast<int64_t>(t.tv_sec) * 1000 + t.tv_nsec / 1000000;
}

#else

// WARNING:
//   If you are from the year 2262 or later, DO NOT use this,
//   as nanoseconds since epoth (1970/1/1) may overflow then.
inline int64_t ns() { return epoch::us() * 1000; }

inline int64_t us() { return epoch::us(); }

inline int64_t ms() { return epoch::ms(); }

#endif

}  // namespace xx

int64_t ns() { return xx::ns(); }

int64_t us() { return xx::us(); }

int64_t ms() { return xx::ms(); }

fastring str(const char* fm) {
    time_t x = time(0);
    struct tm t;
    localtime_r(&x, &t);

    char buf[256];
    const size_t r = strftime(buf, sizeof(buf), fm, &t);
    return fastring(buf, r);
}

}  // namespace now

namespace epoch {

int64_t us() {
    struct timeval t;
    gettimeofday(&t, 0);
    return static_cast<int64_t>(t.tv_sec) * 1000000 + t.tv_usec;
}

int64_t ms() {
    struct timeval t;
    gettimeofday(&t, 0);
    return static_cast<int64_t>(t.tv_sec) * 1000 + t.tv_usec / 1000;
}

}  // namespace epoch
}  // namespace co

namespace _xx { namespace sleep {

void ms(uint32_t n) {
    struct timespec ts;
    ts.tv_sec = n / 1000;
    ts.tv_nsec = n % 1000 * 1000000;
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR)
        ;
}

void sec(uint32_t n) {
    struct timespec ts;
    ts.tv_sec = n;
    ts.tv_nsec = 0;
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR)
        ;
}

}}  // namespace _xx::sleep

#endif
