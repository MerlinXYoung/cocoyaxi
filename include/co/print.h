#pragma once
#include "co/fastream.h"
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