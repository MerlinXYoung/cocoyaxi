#include <atomic>

#include "co/co.h"
#include "co/color.h"
#include "co/print.h"

class S {
  public:
    S() : _v(get_id()) {}
    ~S() = default;

    void run() { co::print("S: ", _v); }

  private:
    int _v;

    static int get_id() {
        static std::atomic_int g_id{0};
        return g_id.fetch_add(1) + 1;
    }
};

// use DEF_main to make code in main() also run in coroutine.
DEF_main(argc, argv) {
    co::pool p([]() { return (void*)new S(); },  // ccb
               [](void* p) { delete (S*)p; },    // dcb
               1024                              // max capacity
    );

    co::wait_group wg;

    do {
        co::print("test pop/push begin: ");
        wg.add(8);
        for (int i = 0; i < 8; ++i) {
            co::print("go: ", i);
            go([p, wg]() { /* capture p and wg by value here, as they are on stack */
                           S* s = (S*)p.pop();
                           s->run();
                           p.push(s);
                           co::print("size: ", p.size());
                           wg.done();
            });
        }
        wg.wait();
        co::print("test pop/push end.. \n");
    } while (0);

    do {
        co::print("test co::pool_guard begin: ");
        wg.add(8);
        for (int i = 0; i < 8; ++i) {
            go([p, wg]() { /* capture p and wg by value here, as they are on stack */
                           {
                               co::pool_guard<S> s(p);
                               s->run();
                           }
                           co::print("size: ", p.size());
                           wg.done();
            });
        }
        wg.wait();
        co::print("test co::pool_guard end..\n");
    } while (0);

    p.clear();
    co::print("size: ", p.size());
    return 0;
}
