#include "co/co.h"
#include "co/color.h"
#include "co/print.h"

DEF_uint32(n, 8, "coroutine number");

DEF_main(argc, argv) {
    co::wait_group wg;
    wg.add(FLG_n);

    for (uint32_t i = 0; i < FLG_n; ++i) {
        go([wg]() {
            co::print("sched: ", co::sched_id(), " co: ", co::coroutine_id());
            wg.done();
        });
    }

    wg.wait();
    return 0;
}
