#pragma once

#include <ostream>

#include "co/fastream.h"

namespace co {
namespace color {
enum class Color : int {
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

struct Text {
    constexpr Text(const char* s, size_t n, Color c) noexcept : s(s), n(n), c(c) {}
    const char* s;
    size_t n;
    Color c;
};
template <typename StringView>
inline Text red(const StringView& s) noexcept {
    return Text(s.data(), s.size(), Color::red);
}
template <typename StringView>
inline Text green(const StringView& s) noexcept {
    return Text(s.data(), s.size(), Color::green);
}
template <typename StringView>
inline Text blue(const StringView& s) noexcept {
    return Text(s.data(), s.size(), Color::blue);
}
template <typename StringView>
inline Text yellow(const StringView& s) noexcept {
    return Text(s.data(), s.size(), Color::yellow);
}
template <typename StringView>
inline Text magenta(const StringView& s) noexcept {
    return Text(s.data(), s.size(), Color::magenta);
}
template <typename StringView>
inline Text cyan(const StringView& s) noexcept {
    return Text(s.data(), s.size(), Color::cyan);
}

inline Text red(const char* s) noexcept { return Text(s, ::strlen(s), Color::red); }

inline Text green(const char* s) noexcept { return Text(s, ::strlen(s), Color::green); }

inline Text blue(const char* s) noexcept { return Text(s, ::strlen(s), Color::blue); }

inline Text yellow(const char* s) noexcept { return Text(s, ::strlen(s), Color::yellow); }

inline Text magenta(const char* s) noexcept { return Text(s, ::strlen(s), Color::magenta); }

inline Text cyan(const char* s) noexcept { return Text(s, ::strlen(s), Color::cyan); }

struct Bold {
    constexpr Bold(const char* s, size_t n) noexcept : s(s), n(n), c(Color::bold) {}
    inline Bold& red() noexcept {
        i |= static_cast<int>(Color::red);
        return *this;
    }
    inline Bold& green() noexcept {
        i |= static_cast<int>(Color::green);
        return *this;
    }
    inline Bold& blue() noexcept {
        i |= static_cast<int>(Color::blue);
        return *this;
    }
    inline Bold& yellow() noexcept {
        i |= static_cast<int>(Color::yellow);
        return *this;
    }
    inline Bold& magenta() noexcept {
        i |= static_cast<int>(Color::magenta);
        return *this;
    }
    inline Bold& cyan() noexcept {
        i |= static_cast<int>(Color::cyan);
        return *this;
    }
    const char* s;
    size_t n;
    union {
        int i;
        Color c;
    };
};
template <typename StringView>
inline Bold bold(const StringView& s) noexcept {
    return Bold(s.data(), s.size());
}

inline Bold bold(const char* s) noexcept { return Bold(s, ::strlen(s)); }

}  // namespace color
using namespace co::color;
}  // namespace co

__coapi std::ostream& operator<<(std::ostream&, co::color::Color);
__coapi fastream& operator<<(fastream&, co::color::Color);

inline std::ostream& operator<<(std::ostream& os, const co::color::Text& x) {
    return (os << x.c).write(x.s, x.n) << co::color::Color::deflt;
}

inline std::ostream& operator<<(std::ostream& os, const co::color::Bold& x) {
    return (os << x.c).write(x.s, x.n) << co::color::Color::deflt;
}

inline fastream& operator<<(fastream& os, const co::color::Text& x) {
    return (os << x.c).append(x.s, x.n) << co::color::Color::deflt;
}

inline fastream& operator<<(fastream& os, const co::color::Bold& x) {
    return (os << x.c).append(x.s, x.n) << co::color::Color::deflt;
}
