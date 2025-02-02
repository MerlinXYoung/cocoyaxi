#include "co/co.h"

#include "co/color.h"
#include "co/print.h"
#include "co/stl.h"

DEF_int32(n, 32, "n coroutines");

co::wait_group g_wg;
co::mutex g_m;
co::map<int, int> g_c;

void f() {
    LOG << co::coroutine_id() << " start ";
    co::sleep(32);
    {
        co::mutex_guard g(g_m);
        g_c[co::sched_id()]++;
    }
    g_wg.done();
    LOG << co::coroutine_id() << " done ";
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    g_wg.add(FLG_n * 2);
    for (int i = 0; i < FLG_n; ++i) {
        go(f);
        GO[] { f(); };
    }
    g_wg.wait();
    co::print(g_c);
    return 0;
}
