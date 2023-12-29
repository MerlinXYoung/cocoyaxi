#pragma once

#include <cstdint>
namespace co {
typedef bool atomic_bool;
typedef char atomic_char;
typedef signed char atomic_schar;
typedef unsigned char atomic_uchar;
typedef short atomic_short;
typedef unsigned short atomic_ushort;
typedef int atomic_int;
typedef unsigned int atomic_uint;
typedef long atomic_long;
typedef unsigned long atomic_ulong;
typedef long long atomic_llong;
typedef unsigned long long atomic_ullong;

typedef int8_t atomic_int8_t;
typedef uint8_t atomic_uint8_t;
typedef int16_t atomic_int16_t;
typedef uint16_t atomic_uint16_t;
typedef int32_t atomic_int32_t;
typedef uint32_t atomic_uint32_t;
typedef int64_t atomic_int64_t;
typedef uint64_t atomic_uint64_t;

typedef char16_t atomic_char16_t;
typedef char32_t atomic_char32_t;
typedef wchar_t atomic_wchar_t;
typedef std::int_least8_t atomic_int_least8_t;
typedef std::uint_least8_t atomic_uint_least8_t;
typedef std::int_least16_t atomic_int_least16_t;
typedef std::uint_least16_t atomic_uint_least16_t;
typedef std::int_least32_t atomic_int_least32_t;
typedef std::uint_least32_t atomic_uint_least32_t;
typedef std::int_least64_t atomic_int_least64_t;
typedef std::uint_least64_t atomic_uint_least64_t;
typedef std::int_fast8_t atomic_int_fast8_t;
typedef std::uint_fast8_t atomic_uint_fast8_t;
typedef std::int_fast16_t atomic_int_fast16_t;
typedef std::uint_fast16_t atomic_uint_fast16_t;
typedef std::int_fast32_t atomic_int_fast32_t;
typedef std::uint_fast32_t atomic_uint_fast32_t;
typedef std::int_fast64_t atomic_int_fast64_t;
typedef std::uint_fast64_t atomic_uint_fast64_t;
typedef std::intptr_t atomic_intptr_t;
typedef std::uintptr_t atomic_uintptr_t;
typedef std::size_t atomic_size_t;
typedef std::ptrdiff_t atomic_ptrdiff_t;
typedef std::intmax_t atomic_intmax_t;
typedef std::uintmax_t atomic_uintmax_t;
}  // namespace co
#define COOST_USED_STD_ATOMIC
#if defined(_MSC_VER) || defined(COOST_USED_STD_ATOMIC)
#include <atomic>
namespace co {
// using memory_order_t = std::memory_order;
// #define mo_relaxed std::memory_order_relaxed
// #define mo_consume std::memory_order_consume
// #define mo_acquire std::memory_order_acquire
// #define mo_release std::memory_order_release
// #define mo_acq_rel std::memory_order_acq_rel
// #define mo_seq_cst std::memory_order_seq_cst
enum memory_order_t {
    mo_relaxed = std::memory_order_relaxed,
    mo_consume = std::memory_order_consume,
    mo_acquire = std::memory_order_acquire,
    mo_release = std::memory_order_release,
    mo_acq_rel = std::memory_order_acq_rel,
    mo_seq_cst = std::memory_order_seq_cst,
};

// mo: mo_relaxed, mo_consume, mo_acquire, mo_seq_cst
template <typename T>
inline T atomic_load(const T* p, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->load((std::memory_order)mo);
}

// mo: mo_relaxed, mo_release, mo_seq_cst
template <typename T, typename V>
inline void atomic_store(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    ((std::atomic<T>*)p)->store((T)v, (std::memory_order)mo);
}

// mo: all
template <typename T, typename V>
inline T atomic_swap(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->exchange((T)v, (std::memory_order)mo);
}

// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename O, typename V>
inline T atomic_compare_swap(T* p, O o, V v, memory_order_t smo = mo_seq_cst,
                             memory_order_t fmo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    T x = (T)o;
    ((std::atomic<T>*)p)->compare_exchange_strong(x, (T)v, (std::memory_order)smo, (std::memory_order)fmo);
    return x;
}

template <typename T, typename V>
inline T atomic_add(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_add(v, (std::memory_order)mo) + v;
}

template <typename T, typename V>
inline T atomic_sub(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_sub(v, (std::memory_order)mo) - v;
}

template <typename T>
inline T atomic_inc(T* p, memory_order_t mo = mo_seq_cst) {
    return atomic_add(p, 1, mo);
}

template <typename T>
inline T atomic_dec(T* p, memory_order_t mo = mo_seq_cst) {
    return atomic_sub(p, 1, mo);
}

template <typename T, typename V>
inline T atomic_fetch_add(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_add(v, (std::memory_order)mo);
}

template <typename T, typename V>
inline T atomic_fetch_sub(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_sub(v, (std::memory_order)mo);
}

template <typename T>
inline T atomic_fetch_inc(T* p, memory_order_t mo = mo_seq_cst) {
    return atomic_fetch_add(p, 1, mo);
}

template <typename T>
inline T atomic_fetch_dec(T* p, memory_order_t mo = mo_seq_cst) {
    return atomic_fetch_sub(p, 1, mo);
}

template <typename T, typename V>
inline T atomic_or(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_or((T)v, (std::memory_order)mo) | (T)v;
}

template <typename T, typename V>
inline T atomic_and(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_and((T)v, (std::memory_order)mo) & (T)v;
}

template <typename T, typename V>
inline T atomic_xor(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_xor((T)v, (std::memory_order)mo) ^ (T)v;
}

template <typename T, typename V>
inline T atomic_fetch_or(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_or((T)v, (std::memory_order)mo);
}

template <typename T, typename V>
inline T atomic_fetch_and(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_and((T)v, (std::memory_order)mo);
}

template <typename T, typename V>
inline T atomic_fetch_xor(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_xor((T)v, (std::memory_order)mo);
}
}  // namespace co
#else /* gcc/clang */
//     |
//     v   ^
//---------|-----  release barrier
//
//---------|-----  acquire barrier
//     ^   v
//     |
namespace co {
enum memory_order_t {
    mo_relaxed = __ATOMIC_RELAXED,
    mo_consume = __ATOMIC_CONSUME,
    mo_acquire = __ATOMIC_ACQUIRE,
    mo_release = __ATOMIC_RELEASE,
    mo_acq_rel = __ATOMIC_ACQ_REL,
    mo_seq_cst = __ATOMIC_SEQ_CST,
};

// mo: mo_relaxed, mo_consume, mo_acquire, mo_seq_cst
template <typename T>
inline T atomic_load(T* p, memory_order_t mo = mo_seq_cst) {
    return __atomic_load_n(p, mo);
}

// mo: mo_relaxed, mo_release, mo_seq_cst
template <typename T, typename V>
inline void atomic_store(T* p, V v, memory_order_t mo = mo_seq_cst) {
    __atomic_store_n(p, (T)v, mo);
}

// mo: all
template <typename T, typename V>
inline T atomic_swap(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_exchange_n(p, (T)v, mo);
}

// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename O, typename V>
inline T atomic_compare_swap(T* p, O o, V v, memory_order_t smo = mo_seq_cst,
                             memory_order_t fmo = mo_seq_cst) {
    T x = (T)o;
    __atomic_compare_exchange_n(p, &x, (T)v, false, smo, fmo);
    return x;
}

template <typename T>
inline T atomic_inc(T* p, memory_order_t mo = mo_seq_cst) {
    return __atomic_add_fetch(p, 1, mo);
}

template <typename T>
inline T atomic_dec(T* p, memory_order_t mo = mo_seq_cst) {
    return __atomic_sub_fetch(p, 1,mo);
}

template <typename T, typename V>
inline T atomic_add(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_add_fetch(p, v, mo);
}

template <typename T, typename V>
inline T atomic_sub(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_sub_fetch(p, v, mo);
}

template <typename T>
inline T atomic_fetch_inc(T* p, memory_order_t mo = mo_seq_cst) {
    return __atomic_fetch_add(p, 1, mo);
}

template <typename T>
inline T atomic_fetch_dec(T* p, memory_order_t mo = mo_seq_cst) {
    return __atomic_fetch_sub(p, 1, mo);
}

template <typename T, typename V>
inline T atomic_fetch_add(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_fetch_add(p, v, mo);
}

template <typename T, typename V>
inline T atomic_fetch_sub(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_fetch_sub(p, v, mo);
}

template <typename T, typename V>
inline T atomic_or(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_or_fetch(p, v, mo);
}

template <typename T, typename V>
inline T atomic_and(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_and_fetch(p, v, mo);
}

template <typename T, typename V>
inline T atomic_xor(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_xor_fetch(p, v, mo);
}

template <typename T, typename V>
inline T atomic_fetch_or(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_fetch_or(p, v, mo);
}

template <typename T, typename V>
inline T atomic_fetch_and(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_fetch_and(p, v, mo);
}

template <typename T, typename V>
inline T atomic_fetch_xor(T* p, V v, memory_order_t mo = mo_seq_cst) {
    return __atomic_fetch_xor(p, v, mo);
}
}  // namespace co
#endif
namespace co {
// the same as atomic_compare_swap
// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename O, typename V>
inline T atomic_cas(T* p, O o, V v, memory_order_t smo = mo_seq_cst,
                    memory_order_t fmo = mo_seq_cst) {
    return atomic_compare_swap(p, o, v, smo, fmo);
}

// like the atomic_cas, but return true if the swap operation is successful
// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename O, typename V>
inline bool atomic_bool_cas(T* p, O o, V v, memory_order_t smo = mo_seq_cst,
                            memory_order_t fmo = mo_seq_cst) {
    return atomic_cas(p, o, v, smo, fmo) == (T)o;
}
}  // namespace co