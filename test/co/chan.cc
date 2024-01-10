#include "co/co.h"
#include "co/co/wait_group.h"
#include "co/color.h"
#include "co/flag.h"
#include "co/print.h"
DEF_int32(n, 1000, "n providers");
struct Message {
    bool close;
    const void* data;
    size_t size;
    std::function<void(void*)> D;
};
co::chan<Message> g_chan(2);

void z() {
    co::chan<int> ch(0);
    go([ch]() {
        LOG << "begin << -1";
        ch << -1;
        LOG << "end << -1";
        LOG << "begin << -2";
        ch << -2;
        LOG << "end << -2";
        LOG << "begin << -3";
        ch << -3;
        LOG << "end << -3";
    });
    int v = 0;
    LOG << "begin >> V";
    ch >> v;
    LOG << "end >> V";
    co::print("v: ", v);
    LOG << "begin >> V";
    ch >> v;
    LOG << "end >> V";
    co::print("v:", v);
    LOG << "begin >> V";
    ch >> v;
    LOG << "end >> V";
    co::print("v:", v);
}

void f() {
    co::chan<int> ch;
    go([ch]() { ch << 7; });
    int v = 0;
    ch >> v;
    co::print("v: ", v);
}

void g() {
    co::chan<int> ch(32, 500);
    go([ch]() {
        ch << 7;
        if (!ch.done()) co::print("write to channel timeout..");
    });

    int v = 0;
    ch >> v;
    if (ch.done()) co::print("v: ", v);
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    z();
    f();
    g();
    co::wait_group wg(0);
    for (int i = 0; i < 3; ++i) {
        go([i, &wg] {
            while (true) {
                Message msg;
                g_chan >> msg;
                if (!g_chan.done()) break;
                if (msg.close) {
                    g_chan.close();
                    break;
                }
                LOG << "consume(" << i << "):" << msg.data << " size:" << msg.size;
                msg.D((void*)msg.data);
            };
            LOG << "send finish";
            wg.done();
        });
        wg.add();
    }
    for (int i = 0; i < FLG_n; ++i) {
        go([i, &wg] {
            auto str = new fastring("provider");
            *str << " " << i;

            g_chan << Message{false, str->data(), str->size(), [str](void* p) {
                                  LOG << "std:" << (void*)str->data() << " [" << *str << "]"
                                      << " p:" << p;
                                  delete str;
                              }};
            wg.done();
        });
        wg.add();
    }
    co::sleep(1000);
    go([] { g_chan << Message{true, nullptr, 0, nullptr}; });
    wg.wait();
    return 0;
}
