#include <stdexcept>

#include "co/co.h"
#include "co/color.h"
#include "co/print.h"
#include "co/stl.h"


DEF_int32(n, 32, "n coroutines");

co::wait_group g_wg;
co::mutex g_m;
co::map<int, int> g_c;

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    g_wg.add();
    go([]() { throw std::runtime_error("test"); });

    g_wg.wait();
    co::print(g_c);

    return 0;
}
