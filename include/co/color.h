#pragma once

#include <mutex>
#include <ostream>

#include "fastream.h"


#ifdef _WIN32
#include "os.h"
#include "god.h"
#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

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

inline const char* get_color_str(int c) {
    static const char* _fg[16] = {
        "\033[0m",          // default
        "\033[31m",         // red
        "\033[32m",         // green
        "\033[33m",         // yellow
        "\033[34m",         // blue
        "\033[35m",         // magenta
        "\033[36m",         // cyan
        "\033[37m",         // white
        "\033[1m",          // bold
        "\033[1m\033[91m",  // bold | red
        "\033[1m\033[32m",  // bold | green
        "\033[1m\033[33m",  // bold | yellow
        "\033[1m\033[94m",  // bold | blue
        "\033[1m\033[95m",  // bold | magenta
        "\033[1m\033[96m",  // bold | cyan
        "\033[1m\033[97m",  // bold | white
    };
    return _fg[c];
}
#ifdef _WIN32
inline HANDLE& cout_handle()noexcept {
    static HANDLE _h = GetStdHandle(STD_OUTPUT_HANDLE);
    return _h;
}

inline bool has_vterm()noexcept {
    if (!os::env("TERM").empty()) return true;
#ifdef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    auto h = cout_handle();
    DWORD mode = 0;
    if (h && GetConsoleMode(h, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (SetConsoleMode(h, mode)) return true;
    }
    return false;
#else
    return false;
#endif
}

inline bool ansi_esc_seq_enabled()noexcept {
    static int _vterm = has_vterm() ? 1 : -1;
    return _vterm > 0;
}

inline int get_default_color() noexcept{
    CONSOLE_SCREEN_BUFFER_INFO buf;
    auto h = cout_handle();
    if (h && GetConsoleScreenBufferInfo(h, &buf)) {
        return buf.wAttributes & 0x0f;
    }
    return 0;
}

inline int get_fgi(int c)noexcept {
    static const int fgi[16] = {
        color::get_default_color(),  // default
        FOREGROUND_RED,
        FOREGROUND_GREEN,
        FOREGROUND_RED | FOREGROUND_GREEN,  // yellow
        FOREGROUND_BLUE,
        FOREGROUND_BLUE | FOREGROUND_RED,                     // magenta
        FOREGROUND_BLUE | FOREGROUND_GREEN,                   // cyan
        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,  // white
        FOREGROUND_INTENSITY,
        FOREGROUND_INTENSITY | FOREGROUND_RED,
        FOREGROUND_INTENSITY | FOREGROUND_GREEN,
        FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
        FOREGROUND_INTENSITY | FOREGROUND_BLUE,
        FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED,
        FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN,
        FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
    };
    return fgi[c];
}
#endif

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
   inline Bold& red() noexcept {
        i |= color::red;
        return *this;
    }
   inline Bold& green() noexcept {
        i |= color::green;
        return *this;
    }
  inline  Bold& blue() noexcept {
        i |= color::blue;
        return *this;
    }
  inline  Bold& yellow() noexcept {
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
template <class StringView>
inline Bold bold(const StringView& s) noexcept {
    return Bold(s.data(), s.size());
}
inline Bold bold(const char* s) noexcept { return Bold(s, ::strlen(s)); }

}  // namespace text
}  // namespace co

namespace color = co::color;
namespace text = co::text;

DEF_has_method(flush);

template <class OStream>
inline OStream& operator<<(OStream& os, color::Color c) {
#ifdef _WIN32
    if (color::ansi_esc_seq_enabled()) return os << color::get_color_str(c);
        
    if constexpr (god::has_method_flush<OStream>())
        os.flush();
    auto h = color::cout_handle();
    if (h) SetConsoleTextAttribute(h, (WORD)color::get_fgi(c));
    return os;
#else
    return os << color::get_color_str(c);
#endif
}

template <class OStream>
inline OStream& operator<<(OStream& os, const text::Text& x) {
    return (os << x.c).write(x.s, x.n) << color::deflt;
}
template <class OStream>
inline OStream& operator<<(OStream& os, const text::Bold& x) {
    return (os << x.c).write(x.s, x.n) << color::deflt;
}

namespace co {
namespace xx {
inline fastream& cstream() {
    static thread_local fastream _s(256);
    return _s;
}
struct Printer {
    inline Printer() noexcept: s(cstream()) { n = s.size(); }
    inline ~Printer() {
        s << '\n';
        {
            static std::mutex _mtx;
            std::lock_guard<std::mutex> m(_mtx);
            ::fwrite(s.data() + n, 1, s.size() - n, stdout);
            s.resize(n);
        }
    }
    fastream& s;
    size_t n;

    //   private:
    // static std::mutex _mtx;
    // static thread_local fastream _s(256);
};
// std::mutex Printer::_mtx;

}  // namespace xx

// print to stdout with newline (thread-safe)
//   - co::print("hello", text::green(" xxx "), 23);
template <typename... X>
inline void print(X&&... x) {
    xx::Printer().s.cat(std::forward<X>(x)...);
}

}  // namespace co
