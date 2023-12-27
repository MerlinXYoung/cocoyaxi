#pragma once
#include <cstdint>
#ifdef CO_USE_STD_ATOMIC
#include <atomic>

namespace co {
using memory_order_t = std::memory_order;
#define mo_relaxed std::memory_order_relaxed
#define mo_consume std::memory_order_consume
#define mo_acquire std::memory_order_acquire
#define mo_release std::memory_order_release
#define mo_acq_rel std::memory_order_acq_rel
#define mo_seq_cst std::memory_order_seq_cst

template <class T>
using atomic = std::atomic<T>;
using atomic_bool = std::atomic<bool>;
using atomic_char = std::atomic<char>;
using atomic_schar = std::atomic<signed char>;
using atomic_uchar = std::atomic<unsigned char>;
using atomic_short = std::atomic<short>;
using atomic_ushort = std::atomic<unsigned short>;
using atomic_int = std::atomic<int>;
using atomic_uint = std::atomic<unsigned int>;
using atomic_long = std::atomic<long>;
using atomic_ulong = std::atomic<unsigned long>;
using atomic_llong = std::atomic<long long>;
using atomic_ullong = std::atomic<unsigned long long>;

using atomic_char16_t = std::atomic<char16_t>;
using atomic_char32_t = std::atomic<char32_t>;
using atomic_wchar_t = std::atomic<wchar_t>;
using atomic_int8_t = std::atomic<std::int8_t>;
using atomic_uint8_t = std::atomic<std::uint8_t>;
using atomic_int16_t = std::atomic<std::int16_t>;
using atomic_uint16_t = std::atomic<std::uint16_t>;
using atomic_int32_t = std::atomic<std::int32_t>;
using atomic_uint32_t = std::atomic<std::uint32_t>;
using atomic_int64_t = std::atomic<std::int64_t>;
using atomic_uint64_t = std::atomic<std::uint64_t>;
using atomic_int_least8_t = std::atomic<std::int_least8_t>;
using atomic_uint_least8_t = std::atomic<std::uint_least8_t>;
using atomic_int_least16_t = std::atomic<std::int_least16_t>;
using atomic_uint_least16_t = std::atomic<std::uint_least16_t>;
using atomic_int_least32_t = std::atomic<std::int_least32_t>;
using atomic_uint_least32_t = std::atomic<std::uint_least32_t>;
using atomic_int_least64_t = std::atomic<std::int_least64_t>;
using atomic_uint_least64_t = std::atomic<std::uint_least64_t>;
using atomic_int_fast8_t = std::atomic<std::int_fast8_t>;
using atomic_uint_fast8_t = std::atomic<std::uint_fast8_t>;
using atomic_int_fast16_t = std::atomic<std::int_fast16_t>;
using atomic_uint_fast16_t = std::atomic<std::uint_fast16_t>;
using atomic_int_fast32_t = std::atomic<std::int_fast32_t>;
using atomic_uint_fast32_t = std::atomic<std::uint_fast32_t>;
using atomic_int_fast64_t = std::atomic<std::int_fast64_t>;
using atomic_uint_fast64_t = std::atomic<std::uint_fast64_t>;
using atomic_intptr_t = std::atomic<std::intptr_t>;
using atomic_uintptr_t = std::atomic<std::uintptr_t>;
using atomic_size_t = std::atomic<std::size_t>;
using atomic_ptrdiff_t = std::atomic<std::ptrdiff_t>;
using atomic_intmax_t = std::atomic<std::intmax_t>;
using atomic_uintmax_t = std::atomic<std::uintmax_t>;

// mo: mo_relaxed, mo_consume, mo_acquire, mo_seq_cst
template <typename T>
inline T atomic_load(const std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->load(mo_seq_cst);
}
template <typename T>
inline T atomic_load(const volatile std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->load(mo_seq_cst);
}

// mo: mo_relaxed, mo_release, mo_seq_cst
template <typename T, typename V>
inline void atomic_store(std::atomic<T>* obj, V desired, memory_order_t mo = mo_seq_cst) noexcept {
    obj->store(desired, mo);
}
template <typename T, typename V>
inline void atomic_store(volatile std::atomic<T>* obj, V desired,
                         memory_order_t mo = mo_seq_cst) noexcept {
    obj->store(desired, mo);
}

// mo: all
template <typename T, typename V>
inline T atomic_swap(std::atomic<T>* obj, V desired, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->exchange(desired, mo);
}

template <typename T, typename V>
inline T atomic_swap(volatile std::atomic<T>* obj, V desired,
                     memory_order_t mo = mo_seq_cst) noexcept {
    return obj->exchange(desired, mo);
}

// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename E, typename D>
inline T atomic_compare_swap(std::atomic<T>* obj, E expected, D desired,
                             memory_order_t smo = mo_seq_cst,
                             memory_order_t fmo = mo_seq_cst) noexcept {
    obj->compare_exchange_strong(expected, desired, smo, fmo);
    return expected;
}
template <typename T, typename E, typename D>
inline T atomic_compare_swap(volatile std::atomic<T>* obj, E expected, D desired,
                             memory_order_t smo = mo_seq_cst,
                             memory_order_t fmo = mo_seq_cst) noexcept {
    obj->compare_exchange_strong(expected, desired, smo, fmo);
    return expected;
}

// the same as atomic_compare_swap
// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename E, typename D>
inline T atomic_cas(std::atomic<T>* obj, E expected, D desired, memory_order_t smo = mo_seq_cst,
                    memory_order_t fmo = mo_seq_cst) noexcept {
    return atomic_compare_swap(obj, expected, desired, smo, fmo);
}
template <typename T, typename E, typename D>
inline T atomic_cas(volatile std::atomic<T>* obj, E expected, D desired,
                    memory_order_t smo = mo_seq_cst, memory_order_t fmo = mo_seq_cst) noexcept {
    return atomic_compare_swap(obj, expected, desired, smo, fmo);
}

// like the atomic_cas, but return true if the swap operation is successful
// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename E, typename D>
inline bool atomic_bool_cas(std::atomic<T>* obj, E expected, D desired,
                            memory_order_t smo = mo_seq_cst,
                            memory_order_t fmo = mo_seq_cst) noexcept {
    return obj->compare_exchange_strong(expected, desired, smo, fmo);
}
template <typename T, typename E, typename D>
inline bool atomic_bool_cas(volatile std::atomic<T>* obj, E expected, D desired,
                            memory_order_t smo = mo_seq_cst,
                            memory_order_t fmo = mo_seq_cst) noexcept {
    return obj->compare_exchange_strong(expected, desired, smo, fmo);
}

template <typename T, typename V>
inline T atomic_add(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_add(arg, mo) + arg;
}
template <typename T, typename V>
inline T atomic_add(volatile std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_add(arg, mo) + arg;
}

template <typename T, typename V>
inline T atomic_sub(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_sub(arg, mo) - arg;
}
template <typename T, typename V>
inline T atomic_sub(volatile std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_sub(arg, mo) - arg;
}

template <typename T>
inline T atomic_inc(std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_add(obj, 1, mo);
}
template <typename T>
inline T atomic_inc(volatile std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_add(obj, 1, mo);
}

template <typename T>
inline T atomic_dec(std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_sub(obj, (typename std::atomic<T>::difference_type)1, mo);
}
template <typename T>
inline T atomic_dec(volatile std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_sub(obj, (typename std::atomic<T>::difference_type)1, mo);
}

template <typename T, typename V>
inline T atomic_fetch_add(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_add(arg, mo);
}
template <typename T, typename V>
inline T atomic_fetch_add(volatile std::atomic<T>* obj, V arg,
                          memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_add(arg, mo);
}

template <typename T, typename V>
inline T atomic_fetch_sub(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_sub(arg, mo);
}
template <typename T, typename V>
inline T atomic_fetch_sub(volatile std::atomic<T>* obj, V arg,
                          memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_sub(arg, mo);
}

template <typename T>
inline T atomic_fetch_inc(std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_fetch_add(obj, 1, mo);
}
template <typename T>
inline T atomic_fetch_inc(volatile std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_fetch_add(obj, 1, mo);
}

template <typename T>
inline T atomic_fetch_dec(std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_fetch_sub(obj, 1, mo);
}
template <typename T>
inline T atomic_fetch_dec(volatile std::atomic<T>* obj, memory_order_t mo = mo_seq_cst) noexcept {
    return atomic_fetch_sub(obj, 1, mo);
}

template <typename T, typename V>
inline T atomic_or(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_or(arg, mo) | arg;
}

template <typename T, typename V>
inline T atomic_or(volatile std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_or(arg, mo) | arg;
}

template <typename T, typename V>
inline T atomic_and(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_and(arg, mo) & arg;
}

template <typename T, typename V>
inline T atomic_and(volatile std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_and(arg, mo) & arg;
}
template <typename T, typename V>
inline T atomic_xor(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_xor(arg, mo) ^ arg;
}
template <typename T, typename V>
inline T atomic_xor(volatile std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_xor(arg, mo) ^ arg;
}

template <typename T, typename V>
inline T atomic_fetch_or(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_or(arg, mo);
}
template <typename T, typename V>
inline T atomic_fetch_or(volatile std::atomic<T>* obj, V arg,
                         memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_or(arg, mo);
}

template <typename T, typename V>
inline T atomic_fetch_and(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_and(arg, mo);
}
template <typename T, typename V>
inline T atomic_fetch_and(volatile std::atomic<T>* obj, V arg,
                          memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_and(arg, mo);
}
template <typename T, typename V>
inline T atomic_fetch_xor(std::atomic<T>* obj, V arg, memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_xor(arg, mo);
}

template <typename T, typename V>
inline T atomic_fetch_xor(volatile std::atomic<T>* obj, V arg,
                          memory_order_t mo = mo_seq_cst) noexcept {
    return obj->fetch_xor(arg, mo);
}

}  // namespace co

#else
namespace co {
using atomic_bool = bool;
using atomic_char = char;
using atomic_schar = signed char;
using atomic_uchar = unsigned char;
using atomic_short = short;
using atomic_ushort = unsigned short;
using atomic_int = int;
using atomic_uint = unsigned int;
using atomic_long = long;
using atomic_ulong = unsigned long;
using atomic_llong = long long;
using atomic_ullong = unsigned long long;

using atomic_char16_t = char16_t;
using atomic_char32_t = char32_t;
using atomic_wchar_t = wchar_t;
using atomic_int8_t = std::int8_t;
using atomic_uint8_t = std::uint8_t;
using atomic_int16_t = std::int16_t;
using atomic_uint16_t = std::uint16_t;
using atomic_int32_t = std::int32_t;
using atomic_uint32_t = std::uint32_t;
using atomic_int64_t = std::int64_t;
using atomic_uint64_t = std::uint64_t;
using atomic_int_least8_t = std::int_least8_t;
using atomic_uint_least8_t = std::uint_least8_t;
using atomic_int_least16_t = std::int_least16_t;
using atomic_uint_least16_t = std::uint_least16_t;
using atomic_int_least32_t = std::int_least32_t;
using atomic_uint_least32_t = std::uint_least32_t;
using atomic_int_least64_t = std::int_least64_t;
using atomic_uint_least64_t = std::uint_least64_t;
using atomic_int_fast8_t = std::int_fast8_t;
using atomic_uint_fast8_t = std::uint_fast8_t;
using atomic_int_fast16_t = std::int_fast16_t;
using atomic_uint_fast16_t = std::uint_fast16_t;
using atomic_int_fast32_t = std::int_fast32_t;
using atomic_uint_fast32_t = std::uint_fast32_t;
using atomic_int_fast64_t = std::int_fast64_t;
using atomic_uint_fast64_t = std::uint_fast64_t;
using atomic_intptr_t = std::intptr_t;
using atomic_uintptr_t = std::uintptr_t;
using atomic_size_t = std::size_t;
using atomic_ptrdiff_t = std::ptrdiff_t;
using atomic_intmax_t = std::intmax_t;
using atomic_uintmax_t = std::uintmax_t;
}  // namespace co
#ifdef _MSC_VER
#include <atomic>
namespace co {
using memory_order_t = std::memory_order;
#define mo_relaxed std::memory_order_relaxed
#define mo_consume std::memory_order_consume
#define mo_acquire std::memory_order_acquire
#define mo_release std::memory_order_release
#define mo_acq_rel std::memory_order_acq_rel
#define mo_seq_cst std::memory_order_seq_cst

// mo: mo_relaxed, mo_consume, mo_acquire, mo_seq_cst
template <typename T>
inline T atomic_load(const T* p, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->load(mo);
}

// mo: mo_relaxed, mo_release, mo_seq_cst
template <typename T, typename V>
inline void atomic_store(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    ((std::atomic<T>*)p)->store((T)v, mo);
}

// mo: all
template <typename T, typename V>
inline T atomic_swap(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->exchange((T)v, mo);
}

// smo: success memory order, all
// fmo: failure memory order, cannot be mo_release, mo_acq_rel,
//      and cannot be stronger than smo
template <typename T, typename O, typename V>
inline T atomic_compare_swap(T* p, O o, V v, memory_order_t smo = mo_seq_cst,
                             memory_order_t fmo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    T x = (T)o;
    ((std::atomic<T>*)p)->compare_exchange_strong(x, (T)v, smo, fmo);
    return x;
}

template <typename T, typename V>
inline T atomic_add(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_add(v, mo) + v;
}

template <typename T, typename V>
inline T atomic_sub(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_sub(v, mo) - v;
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
    return ((std::atomic<T>*)p)->fetch_add(v, mo);
}

template <typename T, typename V>
inline T atomic_fetch_sub(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_sub(v, mo);
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
    return ((std::atomic<T>*)p)->fetch_or((T)v, mo) | (T)v;
}

template <typename T, typename V>
inline T atomic_and(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_and((T)v, mo) & (T)v;
}

template <typename T, typename V>
inline T atomic_xor(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_xor((T)v, mo) ^ (T)v;
}

template <typename T, typename V>
inline T atomic_fetch_or(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_or((T)v, mo);
}

template <typename T, typename V>
inline T atomic_fetch_and(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_and((T)v, mo);
}

template <typename T, typename V>
inline T atomic_fetch_xor(T* p, V v, memory_order_t mo = mo_seq_cst) {
    static_assert(sizeof(T) == sizeof(std::atomic<T>), "");
    return ((std::atomic<T>*)p)->fetch_xor((T)v, mo);
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
    return __atomic_sub_fetch(p, 1, mo);
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
#endif
