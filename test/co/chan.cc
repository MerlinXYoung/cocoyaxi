#include "co/co.h"
#include "co/color.h"
#include "co/print.h"

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
    return 0;
}
