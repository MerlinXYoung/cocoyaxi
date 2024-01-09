#pragma once

#include <experimental/type_traits>
#include <type_traits>
#include <utility>

namespace co { namespace xx {

template <typename F>
struct Defer {
    Defer(F&& f) noexcept : _f(std::forward<F>(f)) {}
    ~Defer() { _f(); }
    typename std::remove_reference<F>::type _f;
};

template <typename F>
inline Defer<F> make_defer(F&& f) noexcept {
    return Defer<F>(std::forward<F>(f));
}

struct DeferOp {
    template <typename Func>
    Defer<Func> operator-(Func&& func) {
        return Defer<Func>(std::forward<Func>(func));
    }
};

}}  // namespace co::xx
#define _co_defer_concat(x, n) x##n
#define _co_defer_make_name(x, n) _co_defer_concat(x, n)
#define _co_defer_name _co_defer_make_name(_co_defer_, __LINE__)

#define defer(e) auto _co_defer_name = co::xx::make_defer([&]() { e; })

#define _CO_DEFER auto _co_defer_name = ::co::xx::DeferOp() -
#define DEFER _CO_DEFER[&]
