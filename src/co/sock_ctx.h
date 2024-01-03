#pragma once

#include <atomic>

#include "co/def.h"
#include "co/table.h"

namespace co {

#if defined(_WIN32)

class SockCtx {
  public:
    SockCtx() = delete;

    inline bool has_event() const noexcept { return _x; }

    inline int add_event() noexcept {
        uint16_t tmp = 0;
        reinterpret_cast<std::atomic_uint16_t*>(&_x)->atomic_compare_exchange(
            tmp, 0x0101, std::memory_order_acq_rel, std::memory_order_acquire);
        return tmp;
    }

    inline void del_ev_read() noexcept { _s.r = 0; }
    inline void del_ev_write() noexcept { _s.w = 0; }
    inline void del_event() noexcept { _x = 0; }

    inline void set_address_family(int x) noexcept {
        assert(x < 65536);
        _x = (uint16_t)x;
    }
    inline int get_address_family() const noexcept { return _x; }

  private:
    union {
        struct {
            uint8_t r;
            uint8_t w;
        } _s;
        uint16_t _x;
    };
};

#elif defined(__linux__)

class SockCtx {
  public:
    SockCtx() = delete;

    // store id and scheduler id of the coroutine that performs read operation.
    inline void add_ev_read(int sched_id, int co_id) noexcept {
        _rev.s = sched_id;
        _rev.c = co_id;
    }

    // store id and scheduler id of the coroutine that performs write operation.
    inline void add_ev_write(int sched_id, int co_id) noexcept {
        _wev.s = sched_id;
        _wev.c = co_id;
    }

    inline void del_event() noexcept {
        _r64 = 0;
        _w64 = 0;
    }
    inline void del_ev_read() noexcept { _r64 = 0; }
    inline void del_ev_write() noexcept { _w64 = 0; }

    inline bool has_ev_read() const noexcept { return _rev.c != 0; }
    inline bool has_ev_write() const noexcept { return _wev.c != 0; }

    inline bool has_ev_read(int sched_id) const noexcept {
        return _rev.s == sched_id && _rev.c != 0;
    }

    inline bool has_ev_write(int sched_id) const noexcept {
        return _wev.s == sched_id && _wev.c != 0;
    }

    inline bool has_event() const noexcept { return this->has_ev_read() || this->has_ev_write(); }

    inline int32_t get_ev_read(int sched_id) const noexcept {
        return _rev.s == sched_id ? _rev.c : 0;
    }

    inline int32_t get_ev_write(int sched_id) const noexcept {
        return _wev.s == sched_id ? _wev.c : 0;
    }

  private:
    struct S {
        int32_t s;  // scheduler id
        int32_t c;  // coroutine id
    };
    union {
        S _rev;
        uint64_t _r64;
    };
    union {
        S _wev;
        uint64_t _w64;
    };
};

#else

class SockCtx {
  public:
    SockCtx() = delete;

    inline bool has_event() const noexcept { return _x; }
    inline bool has_ev_read() const noexcept { return _s.r; }
    inline bool has_ev_write() const noexcept { return _s.w; }

    inline void add_event() noexcept {
        uint16_t tmp = 0;
        reinterpret_cast<std::atomic_uint16_t*>(&_x)->atomic_compare_exchange(
            tmp, 0x0101, std::memory_order_acq_rel, std::memory_order_acquire);
    }

    inline void add_ev_read() noexcept { _s.r = 1; }
    inline void add_ev_write() noexcept { _s.w = 1; }
    inline void del_ev_read() noexcept { _s.r = 0; }
    inline void del_ev_write() noexcept { _s.w = 0; }
    inline void del_event() noexcept { _x = 0; }

  private:
    union {
        struct {
            uint8_t r;
            uint8_t w;
        } _s;
        uint16_t _x;
    };
};

#endif

inline SockCtx& get_sock_ctx(size_t sock) {
    static co::table<SockCtx> _tb(15, 16);
    return _tb[sock];
}

}  // namespace co
