#pragma once

#include <ostream>

#include "fastream.h"

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
    white = 7,    // red | green | blue
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
template <class StringView>
inline Text red(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::red);
}
inline Text red(const char* s) noexcept { return Text(s, ::strlen(s), color::red); }
template <class StringView>
inline Text green(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::green);
}
inline Text green(const char* s) noexcept { return Text(s, ::strlen(s), color::green); }
template <class StringView>
inline Text blue(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::blue);
}
inline Text blue(const char* s) noexcept { return Text(s, ::strlen(s), color::blue); }
template <class StringView>
inline Text yellow(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::yellow);
}
inline Text yellow(const char* s) noexcept { return Text(s, ::strlen(s), color::yellow); }
template <class StringView>
inline Text magenta(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::magenta);
}
inline Text magenta(const char* s) noexcept { return Text(s, ::strlen(s), color::magenta); }
template <class StringView>
inline Text cyan(const StringView& s) noexcept {
    return Text(s.data(), s.size(), color::cyan);
}
inline Text cyan(const char* s) noexcept { return Text(s, ::strlen(s), color::cyan); }

struct Bold {
    constexpr Bold(const char* s, size_t n) noexcept : s(s), n(n), c(color::bold) {}
    Bold& red() noexcept {
        i |= color::red;
        return *this;
    }
    Bold& green() noexcept {
        i |= color::green;
        return *this;
    }
    Bold& blue() noexcept {
        i |= color::blue;
        return *this;
    }
    Bold& yellow() noexcept {
        i |= color::yellow;
        return *this;
    }
    Bold& magenta() noexcept {
        i |= color::magenta;
        return *this;
    }
    Bold& cyan() noexcept {
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
template <class StringView>
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

template <class OStream>
__coapi inline OStream& operator<<(OStream& os, const text::Text& x) {
    return (os << x.c).write(x.s, x.n) << color::deflt;
}
template <class OStream>
__coapi inline OStream& operator<<(OStream& os, const text::Bold& x) {
    return (os << x.c).write(x.s, x.n) << color::deflt;
}

namespace co {
namespace xx {

struct __coapi Cout {
    Cout();
    ~Cout();
    fastream& s;
    size_t n;
};

}  // namespace xx

// print to stdout with newline (thread-safe)
//   - co::print("hello", text::green(" xxx "), 23);
template <typename... X>
inline void print(X&&... x) {
    xx::Cout().s.cat(std::forward<X>(x)...);
}

}  // namespace co
