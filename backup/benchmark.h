#pragma once

#include <iostream>

#include "co/time.h"
#include "co/vector.h"
#include "color.h"

namespace bm {

namespace xx {

struct Result {
    Result(const char* bm, double ns) noexcept : bm(bm), ns(ns) {}
    const char* bm;
    double ns;
};

struct Group {
    Group(const char* name, void (*f)(Group&)) noexcept : name(name), f(f) {}
    const char* name;
    const char* bm;
    void (*f)(Group&);
    int iters;
    int64_t ns;
    co::Timer timer;
    co::vector<Result> res;
};

int calc_iters(int64_t ns) {
    if (ns <= 1000) return 100 * 1000;
    if (ns <= 10000) return 10 * 1000;
    if (ns <= 100000) return 1000;
    if (ns <= 1000000) return 100;
    if (ns <= 10000000) return 10;
    return 1;
}

inline co::vector<Group>& groups() {
    static co::vector<Group> _g;
    return _g;
}

inline bool add_group(const char* name, void (*f)(Group&)) {
    groups().push_back(Group(name, f));
    return true;
}

// do nothing, just fool the compiler
inline void use(void*, int) {}

struct Num {
    constexpr Num(double v) noexcept : v(v) {}

    fastring str() const {
        fastring s(16);
        if (v < 0.01) {
            s = "< 0.01";
        } else if (v < 1000.0) {
            s << dp::_2(v);
        } else if (v < 1000000.0) {
            s << dp::_2(v / 1000) << 'K';
        } else if (v < 1000000000.0) {
            s << dp::_2(v / 1000000) << 'M';
        } else {
            const double x = v / 1000000000;
            if (x <= 1000.0) {
                s << dp::_2(x) << 'G';
            } else {
                s << "> 1000G";
            }
        }
        return s;
    }

    double v;
};

// |  group  |  ns/iter  |  iters/s  |  speedup  |
// | ------- | --------- | --------- | --------- |
// |  bm 0   |  50.0     |  20.0M    |  1.0      |
// |  bm 1   |  10.0     |  100.0M   |  5.0      |
inline void print_results(Group& g) {
    size_t grplen = ::strlen(g.name);
    size_t maxlen = grplen;
    for (auto& r : g.res) {
        const size_t x = ::strlen(r.bm);
        if (maxlen < x) maxlen = x;
    }

    std::cout << "|  " << co::color::bold(g.name).blue() << fastring(maxlen - grplen + 2, ' ')
              << "|  " << co::color::bold("ns/iter  ").blue() << "|  "
              << co::color::bold("iters/s  ").blue() << "|  " << co::color::bold("speedup  ").blue()
              << "|\n";

    std::cout << "| " << fastring(maxlen + 2, '-') << ' ' << "| " << fastring(9, '-') << ' ' << "| "
              << fastring(9, '-') << ' ' << "| " << fastring(9, '-') << ' ' << "|\n";

    for (size_t i = 0; i < g.res.size(); ++i) {
        auto& r = g.res[i];
        const size_t bmlen = ::strlen(r.bm);
        fastring t = Num(r.ns).str();
        size_t p = t.size() <= 7 ? 9 - t.size() : 2;

        std::cout << "|  " << co::color::green(r.bm) << fastring(maxlen - bmlen + 2, ' ') << "|  "
                  << co::color::red(t) << fastring(p, ' ');

        double x = r.ns > 0 ? 1000000000.0 / r.ns : 1.2e12;
        t = Num(x).str();
        p = t.size() <= 7 ? 9 - t.size() : 2;
        std::cout << "|  " << co::color::red(t) << fastring(p, ' ');

        if (i == 0) {
            t = "1.0";
        } else {
            auto _ = g.res[0].ns;
            x = r.ns > 0 ? _ / r.ns : (_ > 0 ? 1.2e12 : 1.0);
            t = Num(x).str();
        }

        p = t.size() <= 7 ? 9 - t.size() : 2;
        std::cout << "|  " << co::color::yellow(t) << fastring(p, ' ') << "|\n";
    }
}

}  // namespace xx

inline void run_benchmarks() {
    auto& groups = xx::groups();
    for (size_t i = 0; i < groups.size(); ++i) {
        if (i != 0) std::cout << '\n';
        auto& g = groups[i];
        g.f(g);
        xx::print_results(g);
    }
}

}  // namespace bm

// define a benchmark group
#define BM_group(_name_)                                                               \
    void _co_bm_group_##_name_(bm::xx::Group&);                                        \
    static bool _co_bm_v_##_name_ = bm::xx::add_group(#_name_, _co_bm_group_##_name_); \
    void _co_bm_group_##_name_(bm::xx::Group& _g_)

// add a benchmark, it must be inside BM_group
#define BM_add(_name_) \
    _g_.bm = #_name_;  \
    _BM_add

#define _BM_add(e)                                                           \
    {                                                                        \
        auto _f_ = [&]() { e; };                                             \
        _g_.timer.restart();                                                 \
        _f_();                                                               \
        _g_.ns = _g_.timer.ns();                                             \
        _g_.iters = bm::xx::calc_iters(_g_.ns);                              \
        if (_g_.iters > 1) {                                                 \
            _g_.timer.restart();                                             \
            for (int _i_ = 0; _i_ < _g_.iters; ++_i_) {                      \
                _f_();                                                       \
            }                                                                \
            _g_.ns = _g_.timer.ns();                                         \
        }                                                                    \
        _g_.res.push_back(bm::xx::Result(_g_.bm, _g_.ns * 1.0 / _g_.iters)); \
    }

// tell the compiler do not optimize this away
#define BM_use(v) bm::xx::use(&v, sizeof(v))
