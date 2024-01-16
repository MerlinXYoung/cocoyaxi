#include <stdexcept>

#include "co/co.h"
#include "co/color.h"
#include "co/defer.h"
#include "co/print.h"
#include "co/stl.h"

co::wait_group g_wg;
co::mutex g_m;
co::map<int, int> g_c;

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    g_wg.add();
    go([] {
        defer(g_wg.done());
        throw std::runtime_error("test");
    });
    g_wg.wait();
    return 0;
}
