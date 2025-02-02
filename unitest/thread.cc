#include "co/co/thread.h"

#include <memory>

#include "co/unitest.h"

namespace test {

DEF_test(thread) {
    DEF_case(sync_event) {
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

    DEF_case(gettid) { EXPECT_NE(co::thread_id(), -1); }

    DEF_case(tls) {
        co::tls<int> pi;
        EXPECT(!pi);
        EXPECT(pi == NULL);
        auto _p = std::make_shared<int>(7);
        int* p = _p.get();
        pi.set(p);
        EXPECT(pi == p);
        EXPECT_EQ(pi.get(), p);
        EXPECT_EQ(*pi, 7);
        EXPECT_EQ(*pi.get(), 7);
    }
}

}  // namespace test
