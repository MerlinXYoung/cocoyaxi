#include "co/str.h"

#include <math.h>

#include <algorithm>

namespace str {

fastring replace(const char* s, size_t n, const char* sub, size_t m, const char* to, size_t l,
                 size_t t) {
    if (unlikely(m == 0)) return fastring(s, n);

    const char* p;
    const char* const end = s + n;
    fastring x(n);

    while ((p = str::memmem(s, end - s, sub, m))) {
        x.append(s, p - s).append(to, l);
        s = p + m;
        if (t && --t == 0) break;
    }

    if (s < end) x.append(s, end - s);
    return x;
}

co::vector<fastring> split(const char* s, size_t n, char c, size_t t) {
    co::vector<fastring> v(8);
    const char* p;
    const char* const end = s + n;

    while ((p = (const char*)::memchr(s, c, end - s))) {
        v.emplace_back(s, p - s);
        s = p + 1;
        if (v.size() == t) break;
    }

    if (s < end) v.emplace_back(s, end - s);
    return v;
}

co::vector<fastring> split(const char* s, size_t n, const char* c, size_t m, size_t t) {
    co::vector<fastring> v;
    if (unlikely(m == 0)) return v;
    v.reserve(8);

    const char* p;
    const char* const end = s + n;

    while ((p = str::memmem(s, end - s, c, m))) {
        v.emplace_back(s, p - s);
        s = p + m;
        if (v.size() == t) break;
    }

    if (s < end) v.emplace_back(s, end - s);
    return v;
}

// co::error() is equal to errno on linux/mac, that's not the fact on windows.
#ifdef _WIN32
#define _co_set_error(e) co::error(e)
#define _co_reset_error() \
    do {                  \
        errno = 0;        \
        co::error(0);     \
    } while (0)
#else
#define _co_set_error(e)
#define _co_reset_error() errno = 0
#endif

bool to_bool(const char* s) {
    co::error(0);
    if (strcmp(s, "false") == 0 || strcmp(s, "0") == 0) return false;
    if (strcmp(s, "true") == 0 || strcmp(s, "1") == 0) return true;
    co::error(EINVAL);
    return false;
}

int32_t to_int32(const char* s) {
    int64_t x = to_int64(s);
    if (unlikely(x > INT32_MAX || x < INT32_MIN)) {
        co::error(ERANGE);
        return 0;
    }
    return (int32_t)x;
}

uint32_t to_uint32(const char* s) {
    int64_t x = (int64_t)to_uint64(s);
    int64_t absx = x < 0 ? -x : x;
    if (unlikely(absx > UINT32_MAX)) {
        co::error(ERANGE);
        return 0;
    }
    return (uint32_t)x;
}

inline int _Shift(char c) {
    switch (c) {
        case 'k':
        case 'K':
            return 10;
        case 'm':
        case 'M':
            return 20;
        case 'g':
        case 'G':
            return 30;
        case 't':
        case 'T':
            return 40;
        case 'p':
        case 'P':
            return 50;
        default:
            return 0;
    }
}

int64_t to_int64(const char* s) {
    _co_reset_error();
    if (!*s) return 0;

    char* end = 0;
    int64_t x = strtoll(s, &end, 0);
    if (errno != 0) {
        _co_set_error(errno);
        return 0;
    }

    size_t n = strlen(s);
    if (end == s + n) return x;

    if (end == s + n - 1) {
        int shift = _Shift(s[n - 1]);
        if (shift != 0) {
            if (x == 0) return 0;
            if (x < (INT64_MIN >> shift) || x > (INT64_MAX >> shift)) {
                co::error(ERANGE);
                return 0;
            }
            return x << shift;
        }
    }

    co::error(EINVAL);
    return 0;
}

uint64_t to_uint64(const char* s) {
    _co_reset_error();
    if (!*s) return 0;

    char* end = 0;
    uint64_t x = strtoull(s, &end, 0);
    if (errno != 0) {
        _co_set_error(errno);
        return 0;
    }

    size_t n = strlen(s);
    if (end == s + n) return x;

    if (end == s + n - 1) {
        int shift = _Shift(s[n - 1]);
        if (shift != 0) {
            if (x == 0) return 0;
            int64_t absx = (int64_t)x;
            if (absx < 0) absx = -absx;
            if (absx > static_cast<int64_t>(UINT64_MAX >> shift)) {
                co::error(ERANGE);
                return 0;
            }
            return x << shift;
        }
    }

    co::error(EINVAL);
    return 0;
}

double to_double(const char* s) {
    _co_reset_error();
    char* end = 0;
    double x = strtod(s, &end);
    if (errno != 0) {
        _co_set_error(errno);
        return 0;
    }

    if (end == s + strlen(s)) return x;
    co::error(EINVAL);
    return 0;
}

#undef _co_set_error
#undef _co_reset_error

}  // namespace str
