#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "co/stl.h"
#include "sched.h"

#ifndef _WIN32
#ifdef __linux__
#include <sys/syscall.h>  // for SYS_xxx definitions
#include <time.h>         // for clock_gettime
#include <unistd.h>       // for syscall()

#else
#include <sys/time.h>  // for gettimeofday
#endif
#endif

namespace co {
namespace xx {

// typedef std::mutex mutex_t;
// typedef std::condition_variable cv_t;
// typedef std::unique_lock<std::mutex> mutex_guard_t;
#ifndef _WIN32
#ifdef __linux__
#ifndef SYS_gettid
#define SYS_gettid __NR_gettid
#endif
uint32_t thread_id() { return syscall(SYS_gettid); }

#else /* for mac, bsd.. */
uint32_t thread_id() {
    uint64_t x;
    pthread_threadid_np(0, &x);
    return (uint32_t)x;
}
#endif
#endif
class mutex_impl {
  public:
    struct queue {
        static constexpr int N = 13;
        struct _memb : co::clink {
            uint8_t rx{0};
            uint8_t wx{0};
            void* q[N];
        };
        static_assert(sizeof(_memb) == 128, "_memb size should be 128");
        inline queue() noexcept : _q(), _size(0) {}

        ~queue() {
            auto h = _q.front();
            while (h) {
                const auto m = (_memb*)h;
                h = h->next;
                ::free(m);
            }
        }

        inline size_t size() const noexcept { return _size; }
        inline bool empty() const noexcept { return this->size() == 0; }

        void push_back(void* x) {
            _memb* m = (_memb*)_q.back();
            if (!m || m->wx == N) {
                m = new _memb;
                _q.push_back(m);
            }
            m->q[m->wx++] = x;
            ++_size;
        }

        void* pop_front() {
            void* x = nullptr;

            auto m = (_memb*)_q.front();
            if (m && m->rx < m->wx) {
                x = m->q[m->rx++];
                --_size;
                if (m->rx == m->wx) {
                    m->rx = m->wx = 0;
                    if (_q.back() != m) {
                        _q.pop_front();
                        delete m;
                    }
                }
            }
            return x;
        }
        co::clist _q;
        size_t _size;
    };

    inline mutex_impl() noexcept : _m(), _cv(), _refn(1), _lock(0) {}
    ~mutex_impl() = default;

    void lock();
    void unlock();
    inline bool try_lock() noexcept;

    void ref() noexcept { _refn.fetch_add(1, std::memory_order_relaxed); }
    uint32_t unref() noexcept { return --_refn; }

  private:
    std::mutex _m;
    std::condition_variable _cv;
    queue _wq;
    std::atomic_uint32_t _refn;
    uint8_t _lock;  // 0: unlocked, 1: locked, 2:notify other thread
};

inline bool mutex_impl::try_lock() noexcept {
    std::unique_lock<std::mutex> g(_m);
    return _lock ? false : (_lock = 1);
}

void mutex_impl::lock() {
    const auto sched = xx::current_sched();  // xx::gSched;
    if (sched) {                             /* in coroutine */
        _m.lock();
        if (!_lock) {
            _lock = 1;
            _m.unlock();
        } else {
            Coroutine* const co = sched->running();
            _wq.push_back(co);
            _m.unlock();
            sched->yield();
        }
    } else { /* non-coroutine */
        std::unique_lock<std::mutex> g(_m);
        if (!_lock) {
            _lock = 1;
        } else {
            _wq.push_back(nullptr);
            for (;;) {
                _cv.wait(g);
                if (_lock == 2) {
                    _lock = 1;
                    break;
                }
            }
        }
    }
}

void mutex_impl::unlock() {
    _m.lock();
    if (_wq.empty()) {
        _lock = 0;
        _m.unlock();
    } else {
        Coroutine* const co = (Coroutine*)_wq.pop_front();
        if (co) {
            _m.unlock();
            co->sched->add_ready_task(co);
        } else {
            _lock = 2;
            _m.unlock();
            _cv.notify_one();
        }
    }
}

class event_impl {
  public:
    explicit inline event_impl(bool m, bool s, uint32_t wg = 0) noexcept
        : _m(), _cv(), _wt(0), _sn(0), _refn(1), _wg(wg), _signaled(s), _manual_reset(m) {}
    ~event_impl() = default;

    bool wait(uint32_t ms);
    void signal();
    void reset();

    inline void ref() noexcept { _refn.fetch_add(1, std::memory_order_relaxed); }
    inline uint32_t unref() noexcept { return --_refn; }
    inline std::atomic_uint32_t& wg() noexcept { return _wg; }

  private:
    std::mutex _m;
    std::condition_variable _cv;
    co::clist _wc;
    uint32_t _wt;
    uint32_t _sn;
    std::atomic_uint32_t _refn;
    std::atomic_uint32_t _wg;  // for wait group
    bool _signaled;
    const bool _manual_reset;
};

bool event_impl::wait(uint32_t ms) {
    const auto sched = xx::current_sched();  //  gSched;
    if (sched) {                             /* in coroutine */
        Coroutine* co = sched->running();
        {
            std::unique_lock<std::mutex> g(_m);
            if (_signaled) {
                if (!_manual_reset) _signaled = false;
                return true;
            }
            if (ms == 0) return false;

            waitx_t* x = 0;
            while (!_wc.empty()) {
                waitx_t* const w = (waitx_t*)_wc.front();
                if (w->state != st_timeout) break;
                _wc.pop_front();
                if (!x)
                    x = w;
                else
                    ::free(w);
            }
            if (x)
                x->state = st_wait;
            else
                x = make_waitx(co);
            co->waitx = x;
            _wc.push_back(x);
        }

        if (ms != (uint32_t)-1) sched->add_timer(ms);
        sched->yield();
        if (!sched->timeout()) ::free(co->waitx);
        co->waitx = nullptr;
        return !sched->timeout();
    } else { /* not in coroutine */
        std::unique_lock<std::mutex> g(_m);
        if (_signaled) {
            if (!_manual_reset) _signaled = false;
            return true;
        }
        if (ms == 0) return false;

        const uint32_t sn = _sn;
        ++_wt;
        if (ms != (uint32_t)-1) {
            const auto r =
                _cv.wait_for(g, std::chrono::milliseconds(ms)) == std::cv_status::no_timeout;
            if (!r && sn == _sn) {
                assert(_wt > 0);
                --_wt;
            }
            return r;
        } else {
            _cv.wait(g);
            return true;
        }
    }
}

void event_impl::signal() {
    co::clink* h = 0;
    {
        bool has_wt = false, has_wc = false;

        std::unique_lock<std::mutex> g(_m);
        if (_wt > 0) {
            _wt = 0;
            has_wt = true;
        }

        if (!_wc.empty()) {
            h = _wc.front();
            _wc.clear();
            if (!has_wt) {
                do {
                    waitx_t* const w = (waitx_t*)h;
                    h = h->next;
                    decltype(w->state)::value_type state(st_wait);
                    if (w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                                         std::memory_order_relaxed)) {
                        has_wc = true;
                        w->co->sched->add_ready_task(w->co);
                        break;
                    } else { /* timeout */
                        ::free(w);
                    }
                } while (h);
            }
        }

        if (has_wt || has_wc) {
            if (_signaled && !_manual_reset) _signaled = false;
            if (has_wt) {
                ++_sn;
                _cv.notify_all();
            }
        } else {
            if (!_signaled) _signaled = true;
        }
    }

    while (h) {
        waitx_t* const w = (waitx_t*)h;
        h = h->next;
        decltype(w->state)::value_type state(st_wait);
        if (w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                             std::memory_order_relaxed)) {
            w->co->sched->add_ready_task(w->co);
        } else { /* timeout */
            ::free(w);
        }
    }
}

inline void event_impl::reset() {
    std::unique_lock<std::mutex> g(_m);
    _signaled = false;
}

class sync_event_impl {
  public:
    explicit sync_event_impl(bool m, bool s) : _wt(0), _sn(0), _signaled(s), _manual_reset(m) {}

    ~sync_event_impl() = default;

    void wait() {
        std::unique_lock<std::mutex> g(_m);
        if (_signaled) {
            if (!_manual_reset) _signaled = false;
            return;
        }
        ++_wt;
        _cv.wait(g);
    }

    bool wait(uint32_t ms) {
        std::unique_lock<std::mutex> g(_m);
        if (_signaled) {
            if (!_manual_reset) _signaled = false;
            return true;
        }
        if (ms == 0) return false;

        const uint32_t sn = _sn;
        ++_wt;
        const bool r = _cv.wait_for(g, std::chrono::milliseconds(ms)) == std::cv_status::no_timeout;
        if (!r && sn == _sn) {
            assert(_wt > 0);
            --_wt;
        }
        return r;
    }

    void signal() {
        std::unique_lock<std::mutex> g(_m);
        if (_wt > 0) {
            _wt = 0;
            if (_signaled && !_manual_reset) _signaled = false;
            ++_sn;
            _cv.notify_all();
        } else {
            if (!_signaled) _signaled = true;
        }
    }

    inline void reset() noexcept {
        std::unique_lock<std::mutex> g(_m);
        _signaled = false;
    }

  private:
    std::mutex _m;
    std::condition_variable _cv;
    uint32_t _wt;
    uint32_t _sn;
    bool _signaled;
    const bool _manual_reset;
};

// static thread_local bool g_done = false;

class pipe_impl {
  public:
    explicit inline pipe_impl(uint32_t buf_size, uint32_t blk_size, uint32_t ms, pipe::C&& c,
                              pipe::D&& d)
        : _buf_size(buf_size),
          _blk_size(blk_size),
          _ms(ms),
          _c(std::move(c)),
          _d(std::move(d)),
          _m(),
          _cv(),
          _rx(0),
          _wx(0),
          _refn(1),
          _full(0),
          _closed(0) {
        _buf = (char*)::malloc(_buf_size);
    }

    inline ~pipe_impl() { ::free(_buf); }

    void read(void* p);
    void write(void* p, int v);
    bool done() const noexcept { return _done; }
    void close();
    inline bool is_closed() const noexcept { return _closed.load(std::memory_order_relaxed); }

    inline void ref() noexcept { _refn.fetch_add(1, std::memory_order_relaxed); }
    inline uint32_t unref() noexcept { return --_refn; }

    struct waitx : co::clink {
        explicit inline waitx(Coroutine* _co, void* _buf) : co(_co), state(st_wait), buf(_buf) {
            x.done = 0;
        }
        ~waitx() = delete;
        Coroutine* co;
        union {
            std::atomic_uint8_t state;
            struct {
                std::atomic_uint8_t state;
                uint8_t done;  // 1: ok, 2: channel closed
                uint8_t v;     // 0: cp, 1: mv, 2: need destruct the object in buf
            } x;
            void* dummy;
        };
        void* buf;
    };

    inline waitx* create_waitx(Coroutine* co, void* buf) {
        if (co && xx::current_sched()->on_stack(buf)) {
            auto p = ::malloc(sizeof(waitx) + _blk_size);
            assert(p);
            return new (p) waitx(co, (char*)p + sizeof(waitx));
        } else {
            auto p = ::malloc(sizeof(waitx));
            assert(p);
            return new (p) waitx(co, buf);
        }
    }

  private:
    void _read_block(void* p);
    void _write_block(void* p, int v);

  private:
    char* _buf;          // buffer
    uint32_t _buf_size;  // buffer size
    uint32_t _blk_size;  // block size
    uint32_t _ms;        // timeout in milliseconds
    xx::pipe::C _c;
    xx::pipe::D _d;

    std::mutex _m;
    std::condition_variable _cv;
    co::clist _wq;
    uint32_t _rx;  // read pos
    uint32_t _wx;  // write pos
    std::atomic_uint32_t _refn;
    uint8_t _full;
    std::atomic_uint8_t _closed;

  private:
    static thread_local bool _done;
};
thread_local bool pipe_impl::_done{false};

inline void pipe_impl::_read_block(void* p) {
    _d(p);
    _c(p, _buf + _rx, 1);
    _d(_buf + _rx);
    _rx += _blk_size;
    if (_rx == _buf_size) _rx = 0;
}

inline void pipe_impl::_write_block(void* p, int v) {
    _c(_buf + _wx, p, v);
    _wx += _blk_size;
    if (_wx == _buf_size) _wx = 0;
}

void pipe_impl::read(void* p) {
    auto sched = xx::current_sched();  // gSched;
    _m.lock();

    // buffer is neither empty nor full
    if (_rx != _wx) {
        this->_read_block(p);
        _m.unlock();
        goto done;
    }

    // buffer is full
    if (_full) {
        this->_read_block(p);

        while (!_wq.empty()) {
            waitx* w = (waitx*)_wq.pop_front();  // wait for write
            decltype(w->state)::value_type state(st_wait);
            if (_ms == (uint32_t)-1 ||
                w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                                 std::memory_order_relaxed)) {
                this->_write_block(w->buf, w->x.v & 1);
                if (w->x.v & 2) _d(w->buf);
                w->x.done = 1;
                if (w->co) {
                    _m.unlock();
                    w->co->sched->add_ready_task(w->co);
                } else {
                    _cv.notify_all();
                    _m.unlock();
                }
                goto done;

            } else { /* timeout */
                if (w->x.v & 2) _d(w->buf);
                ::free(w);
            }
        }

        _full = 0;
        _m.unlock();
        goto done;
    }

    // buffer is empty
    if (this->is_closed()) {
        _m.unlock();
        goto enod;
    }
    if (sched) {
        auto co = sched->running();
        waitx* w = this->create_waitx(co, p);
        w->x.v = (w->buf != p ? 0 : 2);
        _wq.push_back(w);
        _m.unlock();

        co->waitx = (waitx_t*)w;
        if (_ms != (uint32_t)-1) sched->add_timer(_ms);
        sched->yield();

        co->waitx = 0;
        if (!sched->timeout()) {
            if (w->x.done == 1) {
                if (w->buf != p) {
                    _d(p);
                    _c(p, w->buf, 1);  // mv
                    _d(w->buf);
                }
                ::free(w);
                goto done;
            }

            assert(w->x.done == 2);  // channel closed
            ::free(w);
            goto enod;
        }
        goto enod;

    } else {
        bool r = true;
        waitx* w = this->create_waitx(nullptr, p);
        _wq.push_back(w);

        std::unique_lock<std::mutex> g(_m, std::adopt_lock);
        for (;;) {
            if (_ms == (uint32_t)-1) {
                _cv.wait(g);
            } else {
                r = _cv.wait_for(g, std::chrono::milliseconds(_ms)) == std::cv_status::no_timeout;
            }
            decltype(w->state)::value_type state(st_wait);
            if (r || !w->state.compare_exchange_strong(state, st_timeout, std::memory_order_relaxed,
                                                       std::memory_order_relaxed)) {
                const auto x = w->x.done;
                if (x) {
                    g.unlock();
                    g.release();
                    ::free(w);
                    if (x == 1) goto done;
                    goto enod;  // x == 2, channel closed
                }
            } else {
                g.unlock();
                g.release();
                goto enod;
            }
        }
    }

enod:
    _done = false;
    return;
done:
    _done = true;
}

void pipe_impl::write(void* p, int v) {
    auto sched = xx::current_sched();  // gSched;
    _m.lock();
    if (this->is_closed()) {
        _m.unlock();
        goto enod;
    }

    // buffer is neither empty nor full
    if (_rx != _wx) {
        this->_write_block(p, v);
        if (_rx == _wx) _full = 1;
        _m.unlock();
        goto done;
    }

    // buffer is empty
    if (!_full) {
        while (!_wq.empty()) {
            waitx* w = (waitx*)_wq.pop_front();  // wait for read
            decltype(w->state)::value_type state(st_wait);
            if (_ms == (uint32_t)-1 ||
                w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                                 std::memory_order_relaxed)) {
                w->x.done = 1;
                if (w->co) {
                    if (w->x.v & 2) _d(w->buf);
                    _c(w->buf, p, v);
                    _m.unlock();
                    w->co->sched->add_ready_task(w->co);
                } else {
                    _d(w->buf);
                    _c(w->buf, p, v);
                    _cv.notify_all();
                    _m.unlock();
                }
                goto done;

            } else { /* timeout */
                ::free(w);
            }
        }

        this->_write_block(p, v);
        if (_rx == _wx) _full = 1;
        _m.unlock();
        goto done;
    }

    // buffer is full
    if (sched) {
        auto co = sched->running();
        waitx* w = this->create_waitx(co, p);
        if (w->buf != p) { /* p is on the coroutine stack */
            _c(w->buf, p, v);
            w->x.v = 1 | 2;
        } else {
            w->x.v = (uint8_t)v;
        }
        _wq.push_back(w);
        _m.unlock();

        co->waitx = (waitx_t*)w;
        if (_ms != (uint32_t)-1) sched->add_timer(_ms);
        sched->yield();

        co->waitx = nullptr;
        if (!sched->timeout()) {
            ::free(w);
            goto done;
        }
        goto enod;  // timeout

    } else {
        bool r = true;
        waitx* w = this->create_waitx(nullptr, p);
        w->x.v = (uint8_t)v;
        _wq.push_back(w);
        std::unique_lock<std::mutex> g(_m, std::adopt_lock);
        for (;;) {
            if (_ms == (uint32_t)-1) {
                _cv.wait(g);
            } else {
                r = _cv.wait_for(g, std::chrono::milliseconds(_ms)) == std::cv_status::no_timeout;
            }
            decltype(w->state)::value_type state(st_wait);
            if (r || !w->state.compare_exchange_strong(state, st_timeout, std::memory_order_relaxed,
                                                       std::memory_order_relaxed)) {
                if (w->x.done) {
                    assert(w->x.done == 1);
                    g.unlock();
                    g.release();
                    ::free(w);
                    goto done;
                }
            } else {
                g.unlock();
                g.release();
                goto enod;
            }
        }
    }

enod:
    _done = false;
    return;
done:
    _done = true;
}

void pipe_impl::close() {
    decltype(_closed)::value_type closed{0};
    _closed.compare_exchange_strong(closed, 1, std::memory_order_relaxed,
                                    std::memory_order_relaxed);
    if (closed == 0) {
        std::unique_lock<std::mutex> g(_m);
        if (_rx == _wx && !_full) { /* empty */
            while (!_wq.empty()) {
                waitx* w = (waitx*)_wq.pop_front();  // wait for read
                decltype(w->state)::value_type state(st_wait);
                if (w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                                     std::memory_order_relaxed)) {
                    w->x.done = 2;  // channel closed
                    if (w->co) {
                        w->co->sched->add_ready_task(w->co);
                    } else {
                        _cv.notify_all();
                    }
                } else {
                    ::free(w);
                }
            }
        }
        _closed.store(2, std::memory_order_relaxed);

    } else if (closed == 1) {
        while (_closed.load(std::memory_order_relaxed) != 2) co::sleep(1);
    }
}
pipe::pipe(uint32_t buf_size, uint32_t blk_size, uint32_t ms, pipe::C&& c, pipe::D&& d)
    : _p(new pipe_impl(buf_size, blk_size, ms, std::move(c), std::move(d))) {}

pipe::pipe(const pipe& p) : _p(p._p) {
    if (_p) reinterpret_cast<pipe_impl*>(_p)->ref();
}

pipe::~pipe() {
    const auto p = (pipe_impl*)_p;
    if (p && p->unref() == 0) {
        delete p;
        _p = nullptr;
    }
}

void pipe::read(void* p) const { reinterpret_cast<pipe_impl*>(_p)->read(p); }

void pipe::write(void* p, int v) const { reinterpret_cast<pipe_impl*>(_p)->write(p, v); }

bool pipe::done() const noexcept { return reinterpret_cast<pipe_impl*>(_p)->done(); }

void pipe::close() const { reinterpret_cast<pipe_impl*>(_p)->close(); }

bool pipe::is_closed() const noexcept { return reinterpret_cast<pipe_impl*>(_p)->is_closed(); }

//=====================
class pipe_impl_cap {
    struct node : public clink {
        char data[0];
    };

  public:
    explicit inline pipe_impl_cap(uint32_t cap, uint32_t blk_size, uint32_t ms, pipe::C&& c,
                                  pipe::D&& d)
        : _cap(cap),
          _size(0),
          _blk_size(blk_size),
          _ms(ms),
          _c(std::move(c)),
          _d(std::move(d)),
          _m(),
          _cv(),
          _refn(1),
          _closed(0) {}

    inline ~pipe_impl_cap() {
        auto n = _list.front();
        while (n) {
            _list.pop_front();
            _d(((node*)n)->data);
            ::free(n);
        };
    }

    inline bool empty() const noexcept { return 0 == _size; }
    inline bool full() const noexcept { return _cap == 0 ? false : _size == _cap; }

    void read(void* p);
    void write(void* p, int v);
    bool done() const noexcept { return _done; }
    void close();
    inline bool is_closed() const noexcept { return _closed.load(std::memory_order_relaxed); }

    inline void ref() noexcept { _refn.fetch_add(1, std::memory_order_relaxed); }
    inline uint32_t unref() noexcept { return --_refn; }

    struct waitx : co::clink {
        explicit inline waitx(Coroutine* _co, void* _buf) : co(_co), state(st_wait), buf(_buf) {
            x.done = 0;
        }
        ~waitx() = delete;
        Coroutine* co;
        union {
            std::atomic_uint8_t state;
            struct {
                std::atomic_uint8_t state;
                uint8_t done;  // 1: ok, 2: channel closed
                uint8_t v;     // 0: cp, 1: mv, 2: need destruct the object in buf
            } x;
            void* dummy;
        };
        void* buf;
    };

    inline waitx* create_waitx(Coroutine* co, void* buf) {
        if (co && xx::current_sched()->on_stack(buf)) {
            auto p = ::malloc(sizeof(waitx) + _blk_size);
            assert(p);
            return new (p) waitx(co, (char*)p + sizeof(waitx));
        } else {
            auto p = ::malloc(sizeof(waitx));
            assert(p);
            return new (p) waitx(co, buf);
        }
    }

  private:
    void _read_block(void* p, void* src);
    void _write_block(void* dst, void* p, int v);

  private:
    co::clist _list;
    uint32_t _cap;  // capcity
    uint32_t _size;
    uint32_t _blk_size;  // block size
    uint32_t _ms;        // timeout in milliseconds
    xx::pipe::C _c;
    xx::pipe::D _d;

    std::mutex _m;
    std::condition_variable _cv;
    co::clist _wq;

    std::atomic_uint32_t _refn;
    std::atomic_uint8_t _closed;

  private:
    static thread_local bool _done;
};
thread_local bool pipe_impl_cap::_done{false};

inline void pipe_impl_cap::_read_block(void* p, void* src) {
    _d(p);
    _c(p, src, 1);
    _d(src);
}

inline void pipe_impl_cap::_write_block(void* dst, void* p, int v) { _c(dst, p, v); }

void pipe_impl_cap::read(void* p) {
    auto sched = xx::current_sched();  // gSched;
    _m.lock();

    // buffer is neither empty nor full
    if (!empty() && !full()) {
        auto n = _list.front();
        _list.pop_front();
        --_size;
        this->_read_block(p, ((node*)n)->data);
        ::free(n);
        _m.unlock();
        goto done;
    }

    // buffer is full
    if (full()) {
        auto n = _list.front();
        _list.pop_front();
        --_size;
        this->_read_block(p, ((node*)n)->data);
        ::free(n);

        while (!_wq.empty()) {
            waitx* w = (waitx*)_wq.pop_front();  // wait for write
            decltype(w->state)::value_type state(st_wait);
            if (_ms == (uint32_t)-1 ||
                w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                                 std::memory_order_relaxed)) {
                node* n = (node*)::malloc(sizeof(node) + _blk_size);
                n->prev = n->next = nullptr;
                this->_write_block(n->data, w->buf, w->x.v & 1);
                _list.push_back(n);
                ++_size;
                if (w->x.v & 2) _d(w->buf);
                w->x.done = 1;
                if (w->co) {
                    _m.unlock();
                    w->co->sched->add_ready_task(w->co);
                } else {
                    _cv.notify_all();
                    _m.unlock();
                }
                goto done;

            } else { /* timeout */
                if (w->x.v & 2) _d(w->buf);
                ::free(w);
            }
        }

        _m.unlock();
        goto done;
    }

    // buffer is empty
    if (this->is_closed()) {
        _m.unlock();
        goto enod;
    }
    if (sched) {
        auto co = sched->running();
        waitx* w = this->create_waitx(co, p);
        w->x.v = (w->buf != p ? 0 : 2);
        _wq.push_back(w);
        _m.unlock();

        co->waitx = (waitx_t*)w;
        if (_ms != (uint32_t)-1) sched->add_timer(_ms);
        sched->yield();

        co->waitx = 0;
        if (!sched->timeout()) {
            if (w->x.done == 1) {
                if (w->buf != p) {
                    _d(p);
                    _c(p, w->buf, 1);  // mv
                    _d(w->buf);
                }
                ::free(w);
                goto done;
            }

            assert(w->x.done == 2);  // channel closed
            ::free(w);
            goto enod;
        }
        goto enod;

    } else {
        bool r = true;
        waitx* w = this->create_waitx(nullptr, p);
        _wq.push_back(w);

        std::unique_lock<std::mutex> g(_m, std::adopt_lock);
        for (;;) {
            if (_ms == (uint32_t)-1) {
                _cv.wait(g);
            } else {
                r = _cv.wait_for(g, std::chrono::milliseconds(_ms)) == std::cv_status::no_timeout;
            }
            decltype(w->state)::value_type state(st_wait);
            if (r || !w->state.compare_exchange_strong(state, st_timeout, std::memory_order_relaxed,
                                                       std::memory_order_relaxed)) {
                const auto x = w->x.done;
                if (x) {
                    g.unlock();
                    g.release();
                    ::free(w);
                    if (x == 1) goto done;
                    goto enod;  // x == 2, channel closed
                }
            } else {
                g.unlock();
                g.release();
                goto enod;
            }
        }
    }

enod:
    _done = false;
    return;
done:
    _done = true;
}

void pipe_impl_cap::write(void* p, int v) {
    auto sched = xx::current_sched();  // gSched;
    _m.lock();
    if (this->is_closed()) {
        _m.unlock();
        goto enod;
    }

    // buffer is neither empty nor full
    if (!empty() && !full()) {
        node* n = (node*)::malloc(sizeof(node) + _blk_size);
        n->prev = n->next = nullptr;
        this->_write_block(n->data, p, v);
        _list.push_back(n);
        ++_size;
        _m.unlock();
        goto done;
    }

    // buffer is empty
    if (empty()) {
        while (!_wq.empty()) {
            waitx* w = (waitx*)_wq.pop_front();  // wait for read
            decltype(w->state)::value_type state(st_wait);
            if (_ms == (uint32_t)-1 ||
                w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                                 std::memory_order_relaxed)) {
                w->x.done = 1;
                if (w->co) {
                    if (w->x.v & 2) _d(w->buf);
                    _c(w->buf, p, v);
                    _m.unlock();
                    w->co->sched->add_ready_task(w->co);
                } else {
                    _d(w->buf);
                    _c(w->buf, p, v);
                    _cv.notify_all();
                    _m.unlock();
                }
                goto done;

            } else { /* timeout */
                ::free(w);
            }
        }
        node* n = (node*)::malloc(sizeof(node) + _blk_size);
        n->prev = n->next = nullptr;
        this->_write_block(n->data, p, v);
        _list.push_back(n);
        ++_size;
        _m.unlock();
        goto done;
    }

    // buffer is full
    if (sched) {
        auto co = sched->running();
        waitx* w = this->create_waitx(co, p);
        if (w->buf != p) { /* p is on the coroutine stack */
            _c(w->buf, p, v);
            w->x.v = 1 | 2;
        } else {
            w->x.v = (uint8_t)v;
        }
        _wq.push_back(w);
        _m.unlock();

        co->waitx = (waitx_t*)w;
        if (_ms != (uint32_t)-1) sched->add_timer(_ms);
        sched->yield();

        co->waitx = nullptr;
        if (!sched->timeout()) {
            ::free(w);
            goto done;
        }
        goto enod;  // timeout

    } else {
        bool r = true;
        waitx* w = this->create_waitx(nullptr, p);
        w->x.v = (uint8_t)v;
        _wq.push_back(w);
        std::unique_lock<std::mutex> g(_m, std::adopt_lock);
        for (;;) {
            if (_ms == (uint32_t)-1) {
                _cv.wait(g);
            } else {
                r = _cv.wait_for(g, std::chrono::milliseconds(_ms)) == std::cv_status::no_timeout;
            }
            decltype(w->state)::value_type state(st_wait);
            if (r || !w->state.compare_exchange_strong(state, st_timeout, std::memory_order_relaxed,
                                                       std::memory_order_relaxed)) {
                if (w->x.done) {
                    assert(w->x.done == 1);
                    g.unlock();
                    g.release();
                    ::free(w);
                    goto done;
                }
            } else {
                g.unlock();
                g.release();
                goto enod;
            }
        }
    }

enod:
    _done = false;
    return;
done:
    _done = true;
}

void pipe_impl_cap::close() {
    decltype(_closed)::value_type closed{0};
    _closed.compare_exchange_strong(closed, 1, std::memory_order_relaxed,
                                    std::memory_order_relaxed);
    if (closed == 0) {
        std::unique_lock<std::mutex> g(_m);
        if (empty()) { /* empty */
            while (!_wq.empty()) {
                waitx* w = (waitx*)_wq.pop_front();  // wait for read
                decltype(w->state)::value_type state(st_wait);
                if (w->state.compare_exchange_strong(state, st_ready, std::memory_order_relaxed,
                                                     std::memory_order_relaxed)) {
                    w->x.done = 2;  // channel closed
                    if (w->co) {
                        w->co->sched->add_ready_task(w->co);
                    } else {
                        _cv.notify_all();
                    }
                } else {
                    ::free(w);
                }
            }
        }
        _closed.store(2, std::memory_order_relaxed);

    } else if (closed == 1) {
        while (_closed.load(std::memory_order_relaxed) != 2) co::sleep(1);
    }
}

pipe_cap::pipe_cap(uint32_t cap, uint32_t blk_size, uint32_t ms, pipe::C&& c, pipe::D&& d)
    : _p(new pipe_impl_cap(cap, blk_size, ms, std::move(c), std::move(d))) {}

pipe_cap::pipe_cap(const pipe_cap& p) : _p(p._p) {
    if (_p) reinterpret_cast<pipe_impl_cap*>(_p)->ref();
}

pipe_cap::~pipe_cap() {
    const auto p = (pipe_impl_cap*)_p;
    if (p && p->unref() == 0) {
        delete p;
        _p = nullptr;
    }
}

void pipe_cap::read(void* p) const { reinterpret_cast<pipe_impl_cap*>(_p)->read(p); }

void pipe_cap::write(void* p, int v) const { reinterpret_cast<pipe_impl_cap*>(_p)->write(p, v); }

bool pipe_cap::done() const noexcept { return reinterpret_cast<pipe_impl_cap*>(_p)->done(); }

void pipe_cap::close() const { reinterpret_cast<pipe_impl_cap*>(_p)->close(); }

bool pipe_cap::is_closed() const noexcept {
    return reinterpret_cast<pipe_impl_cap*>(_p)->is_closed();
}
//=======================

class pool_impl {
  public:
    typedef co::vector<void*> V;

    pool_impl() : _maxcap((size_t)-1), _refn(1) { this->_make_pools(); }

    pool_impl(std::function<void*()>&& ccb, std::function<void(void*)>&& dcb, size_t cap)
        : _maxcap(cap), _refn(1), _ccb(std::move(ccb)), _dcb(std::move(dcb)) {
        this->_make_pools();
    }

    ~pool_impl() {
        this->clear();
        this->_free_pools();
    }

    void* pop();
    void push(void* p);
    void clear();
    size_t size() const noexcept;

    void _make_pools() {
        _size = co::sched_num();
        _pools = (V*)::calloc(_size, sizeof(V));
    }

    void _free_pools() {
        for (size_t i = 0; i < _size; ++i) _pools[i].~V();
        ::free(_pools);
    }

    inline void ref() noexcept { _refn.fetch_add(1, std::memory_order_relaxed); }
    inline uint32_t unref() noexcept { return --_refn; }

  private:
    V* _pools;
    size_t _size;
    size_t _maxcap;
    std::atomic_uint32_t _refn;
    std::function<void*()> _ccb;
    std::function<void(void*)> _dcb;
};

inline void* pool_impl::pop() {
    auto s = xx::current_sched();  // gSched;
    CHECK(s) << "must be called in coroutine..";
    auto& v = _pools[s->id()];
    return !v.empty() ? v.pop_back() : (_ccb ? _ccb() : nullptr);
}

inline void pool_impl::push(void* p) {
    if (p) {
        auto s = xx::current_sched();  //  gSched;
        CHECK(s) << "must be called in coroutine..";
        auto& v = _pools[s->id()];
        (v.size() < _maxcap || !_dcb) ? v.push_back(p) : _dcb(p);
    }
}

// Create n coroutines to clear all the pools, n is number of schedulers.
// clear() blocks untils all the coroutines are done.
void pool_impl::clear() {
    if (xx::is_active()) {
        auto& scheds = co::scheds();
        co::wait_group wg((uint32_t)scheds.size());
        for (auto& s : scheds) {
            s->go([this, wg]() {
                auto& v = this->_pools[xx::current_sched()->id()];
                if (this->_dcb)
                    for (auto& e : v) this->_dcb(e);
                v.clear();
                wg.done();
            });
        }
        wg.wait();
    } else {
        for (size_t i = 0; i < _size; ++i) {
            auto& v = _pools[i];
            if (this->_dcb)
                for (auto& e : v) this->_dcb(e);
            v.clear();
        }
    }
}

inline size_t pool_impl::size() const noexcept {
    auto s = xx::current_sched();  // gSched;
    CHECK(s) << "must be called in coroutine..";
    return _pools[s->id()].size();
}

}  // namespace xx

mutex::mutex() : _p(new xx::mutex_impl) {}

mutex::mutex(const mutex& m) : _p(m._p) {
    if (_p) reinterpret_cast<xx::mutex_impl*>(_p)->ref();
}

mutex::~mutex() {
    const auto p = (xx::mutex_impl*)_p;
    if (p && p->unref() == 0) {
        delete p;
        _p = nullptr;
    }
}

void mutex::lock() const { reinterpret_cast<xx::mutex_impl*>(_p)->lock(); }

void mutex::unlock() const { reinterpret_cast<xx::mutex_impl*>(_p)->unlock(); }

bool mutex::try_lock() const noexcept { return reinterpret_cast<xx::mutex_impl*>(_p)->try_lock(); }

event::event(bool manual_reset, bool signaled) : _p(new xx::event_impl(manual_reset, signaled)) {}

event::event(const event& e) : _p(e._p) {
    if (_p) reinterpret_cast<xx::event_impl*>(_p)->ref();
}

event::~event() {
    const auto p = (xx::event_impl*)_p;
    if (p && p->unref() == 0) {
        delete p;
        _p = 0;
    }
}

bool event::wait(uint32_t ms) const { return reinterpret_cast<xx::event_impl*>(_p)->wait(ms); }

void event::signal() const { reinterpret_cast<xx::event_impl*>(_p)->signal(); }

void event::reset() const { reinterpret_cast<xx::event_impl*>(_p)->reset(); }

sync_event::sync_event(bool manual_reset, bool signaled)
    : _p(new xx::sync_event_impl(manual_reset, signaled)) {}

sync_event::~sync_event() {
    if (_p) {
        delete (xx::sync_event_impl*)_p;
        _p = 0;
    }
}

void sync_event::signal() { ((xx::sync_event_impl*)_p)->signal(); }

void sync_event::reset() { ((xx::sync_event_impl*)_p)->reset(); }

void sync_event::wait() { ((xx::sync_event_impl*)_p)->wait(); }

bool sync_event::wait(uint32_t ms) { return ((xx::sync_event_impl*)_p)->wait(ms); }

wait_group::wait_group(uint32_t n) : _p(new xx::event_impl(false, false, n)) {}

wait_group::wait_group(const wait_group& wg) : _p(wg._p) {
    if (_p) reinterpret_cast<xx::event_impl*>(_p)->ref();
}

wait_group::~wait_group() {
    const auto p = (xx::event_impl*)_p;
    if (p && p->unref() == 0) {
        delete p;
        _p = nullptr;
    }
}

void wait_group::add(uint32_t n) const {
    reinterpret_cast<xx::event_impl*>(_p)->wg().fetch_add(n /*, std::memory_order_relaxed*/);
}

void wait_group::done() const {
    const auto e = reinterpret_cast<xx::event_impl*>(_p);
    const uint32_t x = --e->wg();
    CHECK(x != (uint32_t)-1);
    if (x == 0) e->signal();
}

void wait_group::wait() const { reinterpret_cast<xx::event_impl*>(_p)->wait((uint32_t)-1); }

pool::pool() : _p(new xx::pool_impl) {}

pool::pool(const pool& p) : _p(p._p) {
    if (_p) reinterpret_cast<xx::pool_impl*>(_p)->ref();
}

pool::~pool() {
    const auto p = (xx::pool_impl*)_p;
    if (p && p->unref() == 0) {
        delete p;
        _p = 0;
    }
}

pool::pool(std::function<void*()>&& ccb, std::function<void(void*)>&& dcb, size_t cap)
    : _p(new xx::pool_impl(std::move(ccb), std::move(dcb), cap)) {}

void* pool::pop() const { return reinterpret_cast<xx::pool_impl*>(_p)->pop(); }

void pool::push(void* p) const { reinterpret_cast<xx::pool_impl*>(_p)->push(p); }

void pool::clear() const { reinterpret_cast<xx::pool_impl*>(_p)->clear(); }

size_t pool::size() const noexcept { return reinterpret_cast<xx::pool_impl*>(_p)->size(); }

}  // namespace co
