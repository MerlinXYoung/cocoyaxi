#include <atomic>

#include "benchmark.h"
#include "co/all.h"

BM_group(atomic) {
    int i = 0;

    // BM_add(++)(::atomic_fetch_add((::atomic_int*)&i, 1););

    // BM_add(--)(::atomic_fetch_sub((::atomic_int*)&i, 1););

    BM_add(++)(reinterpret_cast<std::atomic_int*>(&i)->fetch_add(1););

    BM_add(--)(reinterpret_cast<std::atomic_int*>(&i)->fetch_sub(1););
}

BM_group(rand) {
    int x;
    x = ::rand();
    x = co::rand();

    BM_add(::rand)(x = ::rand(););
    BM_use(x);

    BM_add(co::rand)(x = co::rand(););
    BM_use(x);

    uint32_t seed = co::rand();
    BM_add(co::rand(seed))(x = co::rand(seed););
    BM_use(x);
}

BM_group(malloc) {
    void* p;

    BM_add(::malloc)(p = ::malloc(32););
    BM_use(p);

    // BM_add(co::alloc)(
    //     p = co::alloc(32);
    // );
    // BM_use(p);

    // BM_add(co::alloc_with_align)(
    //     p = co::alloc(32, 64);
    // );
    BM_use(p);
}

BM_group(malloc_free) {
    void* p;

    BM_add(malloc + free)(p = ::malloc(32); ::free(p););
    BM_use(p);

    // BM_add(co::alloc+free)(
    //     p = co::alloc(32);
    //     co::free(p, 32);
    // );
}

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    bm::run_benchmarks();
    return 0;
}
