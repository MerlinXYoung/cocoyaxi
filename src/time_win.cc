#ifdef _WIN32

#include <time.h>

#include "co/time.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace co {
namespace now {
namespace xx {

inline int64_t _query_counts() {
    LARGE_INTEGER x;
    QueryPerformanceCounter(&x);
    return x.QuadPart;
}
static int64_t g_freq = _query_counts();

inline int64_t ns() {
    const int64_t count = _query_counts();
    return (int64_t)(static_cast<double>(count) * 1000000000 / g_freq);
}

inline int64_t us() {
    const int64_t count = _query_counts();
    return (int64_t)(static_cast<double>(count) * 1000000 / g_freq);
}

inline int64_t ms() {
    const int64_t count = _query_counts();
    return (int64_t)(static_cast<double>(count) * 1000 / g_freq);
}

}  // namespace xx

int64_t ns() { return xx::ns(); }

int64_t us() { return xx::us(); }

int64_t ms() { return xx::ms(); }

fastring str(const char* fm) {
    int64_t x = time(0);
    struct tm t;
    _localtime64_s(&t, &x);

    char buf[256];
    const size_t r = strftime(buf, sizeof(buf), fm, &t);
    return fastring(buf, r);
}

}  // namespace now

namespace epoch {

inline int64_t filetime() {
    FILETIME ft;
    LARGE_INTEGER x;
    GetSystemTimeAsFileTime(&ft);
    x.LowPart = ft.dwLowDateTime;
    x.HighPart = ft.dwHighDateTime;
    return x.QuadPart - 116444736000000000ULL;
}

int64_t ms() { return filetime() / 10000; }

int64_t us() { return filetime() / 10; }

}  // namespace epoch
}  // namespace co

namespace _xx { namespace sleep {

void ms(uint32_t n) { ::Sleep(n); }

void sec(uint32_t n) { ::Sleep(n * 1000); }

}}  // namespace _xx::sleep

#endif
