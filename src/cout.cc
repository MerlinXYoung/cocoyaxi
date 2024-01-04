#include "co/cout.h"

#include <mutex>

static const char* fg[16] = {
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

#ifdef _WIN32
#include "co/os.h"

#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace co { namespace color {

std::once_flag g_h_flag;
static HANDLE g_h;

inline HANDLE cout_handle() {
    std::call_once(g_h_flag, []() { g_h = GetStdHandle(STD_OUTPUT_HANDLE); });
    return g_h;
}

static bool has_vterm() {
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

std::once_flag g_vterm_flag;
static int g_vterm;

inline bool ansi_esc_seq_enabled() {
    if (g_vterm == 0) {
        std::call_once(g_vterm_flag, []() { g_vterm = has_vterm() ? 1 : -1; });
    }
    return g_vterm > 0;
}

inline int get_default_color() {
    CONSOLE_SCREEN_BUFFER_INFO buf;
    auto h = cout_handle();
    if (h && GetConsoleScreenBufferInfo(h, &buf)) {
        return buf.wAttributes & 0x0f;
    }
    return 0;
}

}}  // namespace co::color

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

std::ostream& operator<<(std::ostream& os, color::Color c) {
    if (color::ansi_esc_seq_enabled()) return os << fg[c];
    os.flush();
    auto h = color::cout_handle();
    if (h) SetConsoleTextAttribute(h, (WORD)fgi[c]);
    return os;
}

// ANSI color sequence may be not supported on windows
fastream& operator<<(fastream& s, color::Color c) {
    if (color::ansi_esc_seq_enabled()) s << fg[c];
    return s;
}

#else
std::ostream& operator<<(std::ostream& os, color::Color c) { return os << fg[c]; }

fastream& operator<<(fastream& s, color::Color c) { return s << fg[c]; }
#endif

namespace co { namespace xx {

inline std::mutex& cmutex() {
    static std::mutex _mtx;
    return _mtx;
}

inline fastream& cstream() {
    static thread_local fastream _s(256);
    return _s;
}

Cout::Cout() : s(cstream()) { n = s.size(); }

Cout::~Cout() {
    s << '\n';
    {
        std::lock_guard<std::mutex> m(cmutex());
        ::fwrite(s.data() + n, 1, s.size() - n, stdout);
        s.resize(n);
    }
}

}}  // namespace co::xx
