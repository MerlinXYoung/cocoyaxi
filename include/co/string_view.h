#pragma once

#if __cplusplus >= 201703L
#include <string_view>
namespace xrpc {
using string_view = std::string_view;
}
#else
#include "__/string_view.hpp"
namespace std {
using string_view = nonstd::string_view;
}
#endif