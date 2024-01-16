#include "co/print.h"

#include <mutex>

namespace co { namespace xx {

static thread_local fastream g_s(256);
Printer::Printer() : s(g_s) { n = s.size(); }

Printer::~Printer() {
    static std::mutex _m;
    s << '\n';
    {
        std::lock_guard<std::mutex> l(_m);
        ::fwrite(s.data() + n, 1, s.size() - n, stdout);
        s.resize(n);
    }
}

}}  // namespace co::xx