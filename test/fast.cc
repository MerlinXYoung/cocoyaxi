
#include <inttypes.h>

#include <sstream>
#include <string>

#include "co/benchmark.h"
#include "co/color.h"
#include "co/flag.h"
#include "co/time.h"

DEF_uint64(beg, 1000, "beg");
DEF_uint64(end, 9999, "end");
DEF_double(f, 3.14159, "double");

char buf[32] = {0};

BM_group(uint64_to_string) {
    BM_add(snprintf)(for (uint64_t i = FLG_beg; i < FLG_end;
                          i++) { snprintf(buf, 32, "%" PRIu64, i); }) BM_use(buf);

    BM_add(u64toa)(for (uint64_t i = FLG_beg; i < FLG_end; i++) { fast::u64toa(i, buf); })
        BM_use(buf);

    BM_add(to_string)(for (uint64_t i = FLG_beg; i < FLG_end; i++) { (void)std::to_string(i); })
        BM_use(buf);

    BM_add(ostringstream)(for (uint64_t i = FLG_beg; i < FLG_end; i++) {
        std::ostringstream os;
        os << i;
        os.str();
    }) BM_use(buf);

    // BM_use(buf);
}

BM_group(uint64_to_hex) {
    BM_add(snprintf)(for (uint64_t i = FLG_beg; i < FLG_end;
                          i++) { snprintf(buf, 32, "0x%" PRIx64, i); }) BM_use(buf);

    BM_add(u64toh)(for (uint64_t i = FLG_beg; i < FLG_end; i++) { fast::u64toh(i, buf); })
        BM_use(buf);
}

BM_group(double_to_string) {
    int r;
    BM_add(snprintf)(r = snprintf(buf, 32, "%.7g", FLG_f);) BM_use(r);
    BM_use(buf);

    BM_add(dtoa)(r = fast::dtoa(FLG_f, buf);) BM_use(r);
    BM_use(buf);

    BM_add(to_string)({ (void)std::to_string(FLG_f); }) BM_use(buf);

    BM_add(ostringstream)({
        std::ostringstream os;
        os << FLG_f;
        os.str();
    }) BM_use(buf);
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    bm::run_benchmarks();
    return 0;
}
