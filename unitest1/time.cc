#define CATCH_CONFIG_MAIN
#include "co/time.h"

#include "co/def.h"
#include "co/str.h"
#include "helper.h"


namespace test {

TEST_CASE("mono", "[time]") {
    int64 us = now::us();
    int64 ms = now::ms();
    EXPECT_GT(us, 0);
    EXPECT_GT(ms, 0);

    int64 x = now::us();
    int64 y = now::us();
    EXPECT_LE(x, y);
}

TEST_CASE("str", "[time]") {
    fastring ymdhms = now::str("%Y%m%d%H%M%S");
    fastring ymd = now::str("%Y%m%d");
    EXPECT(ymdhms.starts_with(ymd));
}

TEST_CASE("sleep", "[time]") {
    int64 beg = now::ms();
    sleep::ms(1);
    int64 end = now::ms();
    EXPECT_GE(end - beg, 1);
}

TEST_CASE("timer", "[time]") {
    co::Timer timer;
    sleep::ms(1);
    int64 t = timer.us();
    EXPECT_GE(t, 1000);
}

}  // namespace test
