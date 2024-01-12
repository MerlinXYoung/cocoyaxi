#include "co/benchmark.h"
#include "co/co.h"
#include "co/co/wait_group.h"
#include "co/color.h"
#include "co/flag.h"
#include "co/print.h"
co::chan<int> ch(100000);
co::chan1<int> ch1(100000);
co::chan1<int> ch0;
BM_group(write) {
    BM_add1w(ch)(ch << 777;);
    BM_add1w(ch1)(ch1 << 777;);
    BM_add(ch0)(ch0 << 777;);
}
BM_group(read) {
    int v;

    BM_add1w(ch)(ch >> v; assert(v == 777));
    BM_add1w(ch1)(ch1 >> v; assert(v == 777));
    BM_add(ch0)(ch0 >> v; assert(v == 777));
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);

    bm::run_benchmarks();
    return 0;
}
