#include "co/log.h"

#include <algorithm>
#include <memory>
#include <thread>
#include <vector>

#include "co/color.h"
#include "co/os.h"
#include "co/time.h"

DEF_bool(perf, false, "performance testing");

bool static_log() {
    DLOG << "hello static";
    LOG << "hello again, static";
    return true;
}

bool __ = static_log();

int nested_log() {
    DLOG << ">>>> nested log..";
    return 123;
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);

    if (FLG_perf) {
        // test performance by writting 100W logs
        co::print("print 100W logs, every log is about 50 bytes");

        co::Timer t;
        for (int k = 0; k < 1000000; k++) {
            LOG << "hello world " << 3;
        }
        int64_t write_to_cache = t.us();

        log::exit();
        int64_t write_to_file = t.us();

        co::print("All logs written to cache in ", write_to_cache, " us");
        co::print("All logs written to file in ", write_to_file, " us");

    } else {
        // usage of other logs
        DLOG << "This is DLOG (debug).. " << 23;
        LOG << "This is LOG  (info).. " << 23;
        WLOG << "This is WLOG (warning).. " << 23;
        ELOG << "This is ELOG (error).. " << 23;
        // FLOG << "This is FLOG (fatal).. " << 23;
        LOG << "hello " << nested_log() << "  " << nested_log();
        TLOG("co") << "hello co";
        TLOG("bob") << "hello bob";
    }
    std::vector<std::shared_ptr<std::thread>> threads;
    for (int i = 0; i < os::cpunum(); ++i) {
        threads.emplace_back(new std::thread([] {
            for (int j = 0; j < 10000; ++j) {
                DLOG << "This is DLOG (debug).. " << j;
                LOG << "This is LOG  (info).. " << j;
                WLOG << "This is WLOG (warning).. " << j;
                ELOG << "This is ELOG (error).. " << j;
            }
        }));
    }
    for (auto& p : threads) {
        p->join();
    }

    return 0;
}
