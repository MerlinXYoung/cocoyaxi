#include "co/benchmark.h"
#include "co/co.h"
#include "co/co/wait_group.h"
#include "co/color.h"
#include "co/flag.h"
#include "co/print.h"
co::chan<int> ch00(0);
co::chan<int> ch(100000);
co::chan1<int> ch1(100000);
co::chan1<int> ch10;
BM_group(write) {
    BM_add1w(ch)(ch << 777;);
    BM_add1w(ch1)(ch1 << 777;);
    // BM_add(ch00)(ch00 << 777;);
    BM_add(ch10)(ch10 << 777;);
}
BM_group(read) {
    int v;

    BM_add1w(ch)(ch >> v; assert(v == 777));
    BM_add1w(ch1)(ch1 >> v; assert(v == 777));
    // BM_add(ch00)(ch00 >> v; assert(v == 777));
    BM_add(ch10)(ch10 >> v; assert(v == 777));
}
template <class CHAN>
void test_chan(CHAN& ch, co::wait_group& wg) {
    wg.add(3);
    for (int i = 0; i < 3; ++i) {
        go([i, &ch, &wg] {
            int v;
            while (true) {
                ch >> v;
                if (!ch.done()) break;
                if (v == -1) {
                    ch.close();
                    break;
                }
            };
            LOG << i << " send finish";
            wg.done();
        });
    }
    wg.add(10000);
    for (int i = 0; i < 10000; ++i) {
        go([i, &ch, &wg] {
            ch << i;
            wg.done();
        });
    }

    GO[&ch, &wg] {
        while (wg.load() > 3) co::sleep(1);
        ch << -1;
    };
}
BM_group(co) {
    co::wait_group wg;

    BM_add(ch00)(test_chan(ch00, wg); wg.wait());
    BM_add(ch10)(test_chan(ch10, wg); wg.wait());
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);

    bm::run_benchmarks();
    return 0;
}
