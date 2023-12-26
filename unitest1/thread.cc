#define CATCH_CONFIG_MAIN
#include "co/co/thread.h"

#include <memory>

#include "helper.h"


namespace test {

TEST_CASE("sync_event", "[thread]") {
    co::sync_event ev;
    EXPECT_EQ(ev.wait(0), false);
    ev.signal();
    EXPECT_EQ(ev.wait(0), true);
    EXPECT_EQ(ev.wait(0), false);

    co::sync_event em(true, false);  // manual-reset
    EXPECT_EQ(em.wait(0), false);
    em.signal();
    EXPECT_EQ(em.wait(0), true);
    em.reset();
    EXPECT_EQ(em.wait(1), false);
}

TEST_CASE("gettid", "[thread]") { EXPECT_NE(co::thread_id(), -1); }

TEST_CASE("tls", "[thread]") {
    co::tls<int> pi;
    EXPECT(!pi);
    EXPECT((pi == NULL));
    auto _p = std::make_shared<int>(7);
    int* p = _p.get();
    pi.set(p);
    EXPECT((pi == p));
    EXPECT_EQ(pi.get(), p);
    EXPECT_EQ(*pi, 7);
    EXPECT_EQ(*pi.get(), 7);
}

}  // namespace test
