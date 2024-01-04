#pragma once
#include <iostream>

#include "co/fastring.h"
#include "co/flag.h"
#include "co/time.h"
#include "co/vector.h"
#include "color.h"

namespace unitest {

namespace xx {

struct Failed {
    Failed(const char* c, const char* file, int line, fastring&& msg)
        : c(c), file(file), line(line), msg(std::move(msg)) {}
    const char* c;  // case name
    const char* file;
    int line;
    fastring msg;
};

struct Test {
    Test(const char* name, bool& e, void (*f)(Test&)) noexcept
        : name(name), c("default"), enabled(e), f(f) {}
    const char* name;  // test name
    const char* c;     // current case name
    bool& enabled;     // if this test unit is enabled
    void (*f)(Test&);
    co::vector<Failed> failed;
};

inline co::vector<Test>& tests() {
    static co::vector<Test> _t;
    return _t;
}

inline bool add_test(const char* name, bool& e, void (*f)(Test&)) {
    tests().push_back(Test(name, e, f));
    return true;
}

}  // namespace xx

// return number of failed test cases
inline int run_tests() {
    // n: number of tests to do
    // ft: number of failed tests
    // fc: number of failed cases
    int n = 0, ft = 0, fc = 0;
    co::Timer timer;
    auto& tests = xx::tests();

    co::vector<xx::Test*> enabled(32);
    for (auto& t : tests)
        if (t.enabled) enabled.push_back(&t);

    if (enabled.empty()) { /* run all tests by default */
        n = tests.size();
        for (auto& t : tests) {
            std::cout << "> begin test: " << t.name << std::endl;
            timer.restart();
            t.f(t);
            if (!t.failed.empty()) {
                ++ft;
                fc += t.failed.size();
            }
            std::cout << "< test " << t.name << " done in " << timer.us() << " us" << std::endl;
        }

    } else {
        n = enabled.size();
        for (auto& t : enabled) {
            std::cout << "> begin test: " << t->name << std::endl;
            timer.restart();
            t->f(*t);
            if (!t->failed.empty()) {
                ++ft;
                fc += t->failed.size();
            }
            std::cout << "< test " << t->name << " done in " << timer.us() << " us" << std::endl;
        }
    }

    if (fc == 0) {
        if (n > 0) {
            std::cout << color::green << "\nCongratulations! All tests passed!" << color::deflt
                      << std::endl;
        } else {
            std::cout << "No test found. Done nothing." << std::endl;
        }

    } else {
        std::cout << color::red << "\nAha! " << fc << " case" << (fc > 1 ? "s" : "");
        std::cout << " from " << ft << " test" << (ft > 1 ? "s" : "");
        std::cout << " failed. See details below:\n" << color::deflt << std::endl;

        const char* last_case = "";
        for (auto& t : tests) {
            if (!t.failed.empty()) {
                std::cout << color::red << "In test " << t.name << ":\n" << color::deflt;
                for (auto& f : t.failed) {
                    if (strcmp(last_case, f.c) != 0) {
                        last_case = f.c;
                        std::cout << color::red << " case " << f.c << ":\n" << color::deflt;
                    }
                    std::cout << color::yellow << "  " << f.file << ':' << f.line << "] "
                              << color::deflt << f.msg << '\n';
                }
                std::cout.flush();
            }
        }

        std::cout << color::deflt;
        std::cout.flush();
    }

    return fc;
}

// deprecated, use run_tests() instead
inline int run_all_tests() { return run_tests(); }

}  // namespace unitest

// define a test unit
#define DEF_test(_name_)                                                                           \
    DEF_bool(_name_, false, "enable this test if true");                                           \
    void _co_ut_##_name_(unitest::xx::Test&);                                                      \
    static bool _co_ut_v_##_name_ = unitest::xx::add_test(#_name_, FLG_##_name_, _co_ut_##_name_); \
    void _co_ut_##_name_(unitest::xx::Test& _t_)

// define a test case in the current unit
#define DEF_case(name) \
    _t_.c = #name;     \
    std::cout << " case " << #name << ':' << std::endl;

#define EXPECT(x)                                                                                  \
    {                                                                                              \
        if (x) {                                                                                   \
            std::cout << color::green << "  EXPECT(" << #x << ") passed" << color::deflt           \
                      << std::endl;                                                                \
        } else {                                                                                   \
            fastring _U_s(32);                                                                     \
            _U_s << "EXPECT(" << #x << ") failed";                                                 \
            std::cout << color::red << "  " << _U_s << color::deflt << std::endl;                  \
            _t_.failed.push_back(unitest::xx::Failed(_t_.c, __FILE__, __LINE__, std::move(_U_s))); \
        }                                                                                          \
    }

#define EXPECT_OP(x, y, op, opname)                                                                \
    {                                                                                              \
        auto _U_x = (x);                                                                           \
        auto _U_y = (y);                                                                           \
        if (_U_x op _U_y) {                                                                        \
            std::cout << color::green << "  EXPECT_" << opname << "(" << #x << ", " << #y          \
                      << ") passed";                                                               \
            if (strcmp("==", #op) != 0) std::cout << ": " << _U_x << " vs " << _U_y;               \
            std::cout << color::deflt << std::endl;                                                \
        } else {                                                                                   \
            fastring _U_s(128);                                                                    \
            _U_s << "EXPECT_" << opname << "(" << #x << ", " << #y << ") failed: " << _U_x         \
                 << " vs " << _U_y;                                                                \
            std::cout << color::red << "  " << _U_s << color::deflt << std::endl;                  \
            _t_.failed.push_back(unitest::xx::Failed(_t_.c, __FILE__, __LINE__, std::move(_U_s))); \
        }                                                                                          \
    }

#define EXPECT_EQ(x, y) EXPECT_OP(x, y, ==, "EQ")
#define EXPECT_NE(x, y) EXPECT_OP(x, y, !=, "NE")
#define EXPECT_GE(x, y) EXPECT_OP(x, y, >=, "GE")
#define EXPECT_LE(x, y) EXPECT_OP(x, y, <=, "LE")
#define EXPECT_GT(x, y) EXPECT_OP(x, y, >, "GT")
#define EXPECT_LT(x, y) EXPECT_OP(x, y, <, "LT")
