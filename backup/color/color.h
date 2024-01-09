#pragma once

#include <ostream>

#include "co/fastream.h"

namespace co {
namespace color {

enum Color {
    deflt = 0,
    red = 1,
    green = 2,
    yellow = 3,  // red | green
    blue = 4,
    magenta = 5,  // blue | red
    cyan = 6,     // blue | green
    // white = 7,  // red| blue | green
    bold = 8,
};

}  // namespace color

namespace text {

struct Text {
    constexpr Text(const char* s, size_t n, color::Color c) noexcept : s(s), n(n), c(c) {}
    const char* s;
    size_t n;
    color::Color c;
};
template <typename StringView>
inline Text red(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::red);
}
template <typename StringView>
inline Text green(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::green);
}
template <typename StringView>
inline Text blue(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::blue);
}
template <typename StringView>
inline Text yellow(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::yellow);
}
template <typename StringView>
inline Text magenta(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::magenta);
}
template <typename StringView>
inline Text cyan(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::cyan);
}

inline Text red(const char* s) noexcept { return Text(s, ::strlen(s), color::red); }

inline Text green(const char* s) noexcept { return Text(s, ::strlen(s), color::green); }

inline Text blue(const char* s) noexcept { return Text(s, ::strlen(s), color::blue); }

inline Text yellow(const char* s) noexcept { return Text(s, ::strlen(s), color::yellow); }

inline Text magenta(const char* s) noexcept { return Text(s, ::strlen(s), color::magenta); }

inline Text cyan(const char* s) noexcept { return Text(s, ::strlen(s), color::cyan); }

struct Bold {
    constexpr Bold(const char* s, size_t n) noexcept : s(s), n(n), c(color::bold) {}
    inline Bold& red() noexcept {
        i |= color::red;
        return *this;
    }
    inline Bold& green() noexcept {
        i |= color::green;
        return *this;
    }
    inline Bold& blue() noexcept {
        i |= color::blue;
        return *this;
    }
    inline Bold& yellow() noexcept {
        i |= color::yellow;
        return *this;
    }
    inline Bold& magenta() noexcept {
        i |= color::magenta;
        return *this;
    }
    inline Bold& cyan() noexcept {
        i |= color::cyan;
        return *this;
    }
    const char* s;
    size_t n;
    union {
        int i;
        color::Color c;
    };
};
template <typename StringView>
inline Bold bold(const StringView& s) noexcept {
    return Bold(s.data(), s.size());
}

inline Bold bold(const char* s) noexcept { return Bold(s, ::strlen(s)); }

}  // namespace text
}  // namespace co

namespace color = co::color;
namespace text = co::text;

__coapi std::ostream& operator<<(std::ostream&, color::Color);
__coapi fastream& operator<<(fastream&, color::Color);

inline std::ostream& operator<<(std::ostream& os, const co::color::Text& x) {
    return (os << x.c).write(x.s, x.n) << color::deflt;
}

inline std::ostream& operator<<(std::ostream& os, const co::color::Bold& x) {
    return (os << x.c).write(x.s, x.n) << color::deflt;
}

inline fastream& operator<<(fastream& os, const co::color::Text& x) {
    return (os << x.c).append(x.s, x.n) << color::deflt;
}

inline fastream& operator<<(fastream& os, const co::color::Bold& x) {
    return (os << x.c).append(x.s, x.n) << color::deflt;
}

namespace co {
namespace xx {

struct __coapi Printer {
    Printer();
    ~Printer();
    fastream& s;
    size_t n;
};

}  // namespace xx

// print to stdout with newline (thread-safe)
//   - co::print("hello", co::color::green(" xxx "), 23);
template <typename... X>
inline void print(X&&... x) {
    xx::Printer().s.cat(std::forward<X>(x)...);
}

}  // namespace co
