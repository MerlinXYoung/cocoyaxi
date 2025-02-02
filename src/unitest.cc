#include "co/unitest.h"

#include "co/time.h"

namespace unitest {
namespace xx {

inline std::vector<Test>& tests() {
    static std::vector<Test> _t;
    return _t;
}

bool add_test(const char* name, bool& e, void (*f)(Test&)) {
    tests().push_back(Test(name, e, f));
    return true;
}

}  // namespace xx

int run_tests() {
    // n: number of tests to do
    // ft: number of failed tests
    // fc: number of failed cases
    int n = 0, ft = 0, fc = 0;
    co::Timer timer;
    auto& tests = xx::tests();

    std::vector<xx::Test*> enabled;
    enabled.reserve(32);
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
            std::cout << co::Color::green << "\nCongratulations! All tests passed!" << co::Color::deflt
                      << std::endl;
        } else {
            std::cout << "No test found. Done nothing." << std::endl;
        }

    } else {
        std::cout << co::Color::red << "\nAha! " << fc << " case" << (fc > 1 ? "s" : "");
        std::cout << " from " << ft << " test" << (ft > 1 ? "s" : "");
        std::cout << " failed. See details below:\n" << co::Color::deflt << std::endl;

        const char* last_case = "";
        for (auto& t : tests) {
            if (!t.failed.empty()) {
                std::cout << co::Color::red << "In test " << t.name << ":\n" << co::Color::deflt;
                for (auto& f : t.failed) {
                    if (strcmp(last_case, f.c) != 0) {
                        last_case = f.c;
                        std::cout << co::Color::red << " case " << f.c << ":\n" << co::Color::deflt;
                    }
                    std::cout << co::Color::yellow << "  " << f.file << ':' << f.line << "] "
                              << co::Color::deflt << f.msg << '\n';
                }
                std::cout.flush();
            }
        }

        std::cout << co::Color::deflt;
        std::cout.flush();
    }

    return fc;
}

}  // namespace unitest
