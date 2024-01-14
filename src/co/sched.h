#pragma once

#include <atomic>
#include <type_traits>
#ifdef _MSC_VER
#pragma warning(disable : 4127)
#endif

#include "co/closure.h"
#include "co/co.h"
#include "co/fastream.h"
#include "co/flag.h"
#include "co/god.h"
#include "co/log.h"
#include "co/stl.h"
#include "co/time.h"
#include "context/context.h"

#if defined(_WIN32)
#include "epoll/iocp.h"
#elif defined(__linux__)
#include "epoll/epoll.h"
#else
#include "epoll/kqueue.h"
#endif

DEC_uint16(co_sched_num);
DEC_uint32(co_stack_num);
DEC_uint32(co_stack_size);
DEC_bool(co_sched_log);

#define SCHEDLOG TLOG_IF(FLG_co_sched_log)

namespace co { namespace xx {

class Sched;
struct Coroutine;
typedef co::multimap<int64_t, Coroutine*>::iterator timer_id_t;

enum state_t : uint8_t {
    st_wait = 0,     // wait for an event, do not modify
    st_ready = 1,    // ready to resume
    st_timeout = 2,  // timeout
};

// waiting context
struct waitx_t : co::clink {
    explicit inline waitx_t(Coroutine* _co) : co(_co), state(st_wait) {}
    ~waitx_t() = delete;
    Coroutine* co;
    union {
        std::atomic_uint8_t state;
        void* dummy;
    };
};

inline waitx_t* make_waitx(void* co, size_t n = sizeof(waitx_t)) {
    auto p = ::malloc(n);
    assert(p);
    return new (p) waitx_t((Coroutine*)co);
}

struct Stack {
    char* p;        // stack pointer
    char* top;      // stack top
    Coroutine* co;  // coroutine owns this stack
};

struct Buffer {
    struct H {
        uint32_t cap;
        uint32_t size;
        char p[];
    };

    Buffer() = delete;
    ~Buffer() = delete;

    inline const char* data() const noexcept { return _h ? _h->p : 0; }
    inline uint32_t size() const noexcept { return _h ? _h->size : 0; }
    inline uint32_t capacity() const noexcept { return _h ? _h->cap : 0; }
    inline void clear() noexcept {
        if (_h) _h->size = 0;
    }

    inline void reset() {
        if (_h) {
            ::free(_h);
            _h = 0;
        }
    }

    inline void append(const void* p, size_t size) {
        const uint32_t n = (uint32_t)size;
        if (!_h) {
            _h = (H*)::malloc(size + 8);
            assert(_h);
            _h->cap = n;
            _h->size = 0;
            goto lable;
        }

        if (_h->cap < _h->size + n) {
            const uint32_t o = _h->cap;
            _h->cap += (o >> 1) + n;
            _h = (H*)::realloc(_h, _h->cap + 8);
            assert(_h);
            goto lable;
        }

    lable:
        memcpy(_h->p + _h->size, p, n);
        _h->size += n;
    }

    H* _h;
};

struct Coroutine {
    Coroutine() = delete;
    ~Coroutine() = delete;
    union {
        struct {
            uint16_t use_count;
            uint16_t sched_id;
            uint32_t idx;  // coroutine idx for CoroutinePool
        };
        uint64_t id;
    };
    tb_context_t ctx;  // coroutine context, points to the stack bottom
    Closure* cb;       // coroutine function
    Sched* sched;      // scheduler this coroutine runs in
    Stack* stack;      // stack this coroutine runs on
    union {
        Buffer buf;  // for saving stack data of this coroutine
        void* pbuf;
    };
    waitx_t* waitx;  // waiting context
    union {
        timer_id_t it;
        char _dummy2[sizeof(timer_id_t)];
    };
};

class CoroutinePool {
  public:
    static const int E = 12;
    static const int N = 1 << E;  // max coroutines per block
    static const int M = 256;

    inline CoroutinePool() : _c(0), _o(0), _v(M), _use_count(M) {
        _v.resize(M);
        _use_count.resize(M);
    }

    inline ~CoroutinePool() {
        for (size_t i = 0; i < _v.size(); ++i) {
            if (_v[i]) ::free(_v[i]);
        }
        _v.clear();
    }

    Coroutine* pop() {
        int id = 0;
        if (!_v0.empty()) {
            id = _v0.pop_back();
            goto reuse;
        }
        if (!_vc.empty()) {
            id = _vc.pop_back();
            goto reuse;
        }
        if (_o < N) goto newco;
        _c = !_blks.empty() ? *_blks.begin() : _c + 1;
        if (!_blks.empty()) _blks.erase(_blks.begin());
        _o = 0;

    newco : {
        if (_c < (int)_v.size()) {
            if (!_v[_c]) _v[_c] = (Coroutine*)::calloc(N, sizeof(Coroutine));
        } else {
            const int c = god::align_up<M>(_c + 1);
            _v.resize(c);
            _use_count.resize(c);
            _v[_c] = (Coroutine*)::calloc(N, sizeof(Coroutine));
        }

        auto& co = _v[_c][_o];
        co.idx = (_c << E) + _o++;
        _use_count[_c]++;
        return &co;
    }

    reuse : {
        const int q = id >> E;
        const int r = id & (N - 1);
        auto& co = _v[q][r];
        co.ctx = 0;
        _use_count[q]++;
        return &co;
    }
    }

    void push(Coroutine* co) {
        const int idx = co->idx;
        const int q = idx >> E;
        if (q == 0) {
            if (_v0.capacity() == 0) _v0.reserve(N);
            _v0.push_back(idx);
            goto end;
        }
        if (q == _c) {
            if (_vc.capacity() == 0) _vc.reserve(N);
            _vc.push_back(idx);
            goto end;
        }

    end:
        if (--_use_count[q] == 0) {
            ::free(_v[q]);
            _v[q] = 0;
            if (q != _c) {
                _blks.insert(q);
            } else {
                _vc.clear();
                _o = 0;
                if (!_blks.empty() && *_blks.begin() < _c) {
                    _blks.insert(_c);
                    _c = *_blks.begin();
                    _blks.erase(_blks.begin());
                }
            }
        }
    }

    Coroutine& operator[](int i) const {
        const int q = i >> E;
        const int r = i & (N - 1);
        return _v[q][r];
    }

  private:
    int _c;  // current block
    int _o;  // offset in the current block [0, S)
    co::vector<Coroutine*> _v;
    co::vector<int> _use_count;
    co::vector<int> _v0;  // id of coroutine in _v[0]
    co::vector<int> _vc;  // id of coroutine in _v[_c]
    co::set<int> _blks;   // blocks available
};

// Task may be added from any thread. We need a mutex here.
class TaskManager {
  public:
    TaskManager() : _mtx(), _new_tasks(512), _ready_tasks(512) {}
    ~TaskManager() = default;

    void add_new_task(Closure* cb) {
        std::lock_guard<std::mutex> g(_mtx);
        _new_tasks.push_back(cb);
    }

    void add_ready_task(Coroutine* co) {
        std::lock_guard<std::mutex> g(_mtx);
        _ready_tasks.push_back(co);
    }

    void get_all_tasks(co::vector<Closure*>& new_tasks, co::vector<Coroutine*>& ready_tasks) {
        std::lock_guard<std::mutex> g(_mtx);
        if (!_new_tasks.empty()) _new_tasks.swap(new_tasks);
        if (!_ready_tasks.empty()) _ready_tasks.swap(ready_tasks);
    }

  private:
    std::mutex _mtx;
    co::vector<Closure*> _new_tasks;
    co::vector<Coroutine*> _ready_tasks;
};

inline fastream& operator<<(fastream& fs, const timer_id_t& id) { return fs << *(void**)(&id); }

// Timer must be added in the scheduler thread. We need no lock here.
class TimerManager {
    using timer_type = co::multimap<int64_t, Coroutine*>;

  public:
    inline TimerManager() : _timer(), _it(_timer.end()) {}
    ~TimerManager() = default;

    inline timer_id_t add_timer(uint32_t ms, Coroutine* co) {
        return _it = _timer.emplace_hint(_it, now::ms() + ms, co);
    }

    inline void del_timer(const timer_id_t& it) {
        if (_it == it) ++_it;
        _timer.erase(it);
    }

    inline timer_id_t end() { return _timer.end(); }

    // get timedout coroutines, return time(ms) to wait for the next timeout
    uint32_t check_timeout(co::vector<Coroutine*>& res);

  private:
    timer_type _timer;                  // timed-wait tasks: <time_ms, co>
    typename timer_type::iterator _it;  // make insert faster with this hint
};

// coroutine scheduler, loop in a single thread
class Sched {
  public:
    Sched(uint32_t id, uint32_t sched_num, uint32_t stack_num, uint32_t stack_size);
    ~Sched();

    // id of this scheduler
    inline uint32_t id() const noexcept { return _id; }

    // the current running coroutine
    inline Coroutine* running() const noexcept { return _running; }

    // id of the current running coroutine
    inline uint64_t coroutine_id() const noexcept {
        return _running->id /*_sched_num * (_running->idx - 1) + _id*/;
    }

    // check if the memory @p points to is on the stack of the coroutine
    inline bool on_stack(const void* p) const {
        Stack* const s = _running->stack;
        return (s->p <= (char*)p) && ((char*)p < s->top);
    }

    // resume a coroutine
    void resume(Coroutine* co);

    // suspend the current coroutine
    inline void yield() { tb_context_jump(_main_co->ctx, _running); }

    // add a new task to run as a coroutine later (thread-safe)
    inline void add_new_task(Closure* cb) {
        _task_mgr.add_new_task(cb);
        _epoll->signal();
    }

    // add a coroutine ready to resume (thread-safe)
    inline void add_ready_task(Coroutine* co) {
        _task_mgr.add_ready_task(co);
        _epoll->signal();
    }

    // sleep for milliseconds in the current coroutine
    inline void sleep(uint32_t ms) {
        if (_wait_ms > ms) _wait_ms = ms;
        (void)_timer_mgr.add_timer(ms, _running);
        SCHEDLOG << "co(" << _running << ")" << (void*)_running->id << " sleep(" << ms << " ms)";
        this->yield();
    }

    // add a timer for the current coroutine
    inline void add_timer(uint32_t ms) {
        if (_wait_ms > ms) _wait_ms = ms;
        _running->it = _timer_mgr.add_timer(ms, _running);
        SCHEDLOG << "co(" << _running << ")" << (void*)_running->id << " add timer " << _running->it
                 << " (" << ms << " ms)";
    }

    // check whether the current coroutine has timed out
    inline bool timeout() const noexcept { return _timeout; }

    // add an IO event on a socket to epoll for the current coroutine.
    inline bool add_io_event(sock_t fd, _ev_t ev) {
        SCHEDLOG << "co(" << _running << ")" << (void*)_running->id << " add io event fd: " << fd
                 << " ev: " << (int)ev;
#if defined(_WIN32)
        (void)ev;  // we do not care what the event is on windows
        return _epoll->add_event(fd);
#elif defined(__linux__)
        return ev == ev_read ? _epoll->add_ev_read(fd, _running->idx)
                             : _epoll->add_ev_write(fd, _running->idx);
#else
        return ev == ev_read ? _epoll->add_ev_read(fd, _running)
                             : _epoll->add_ev_write(fd, _running);
#endif
    }

    // delete an IO event on a socket from the epoll for the current coroutine.
    inline void del_io_event(sock_t fd, _ev_t ev) {
        SCHEDLOG << "co(" << _running << ")" << (void*)_running->id << " del io event, fd: " << fd
                 << " ev: " << (int)ev;
        ev == ev_read ? _epoll->del_ev_read(fd) : _epoll->del_ev_write(fd);
    }

    // delete all IO events on a socket from the epoll.
    inline void del_io_event(sock_t fd) {
        SCHEDLOG << "co(" << _running << ")" << (void*)_running->id << " del io event, fd: " << fd;
        _epoll->del_event(fd);
    }

    // cputime of this scheduler (us)
    inline int64_t cputime() const noexcept { return _cputime.load(std::memory_order_relaxed); }

    // start the scheduler thread
    inline void start() { std::thread(&Sched::loop, this).detach(); }

    // stop the scheduler thread
    void stop();

    // the thread function
    void loop();

  private:
    // entry function for coroutine
    static void main_func(tb_context_from_t from);

    // save stack for the coroutine
    inline void save_stack(Coroutine* co) {
        if (co) {
            SCHEDLOG << "co(" << co << ")" << (void*)co->id
                     << " save stack: " << co->stack->top - (char*)co->ctx;
            if (!co->pbuf && !_bufs.empty()) co->pbuf = _bufs.pop_back();
            co->buf.clear();
            co->buf.append(co->ctx, co->stack->top - (char*)co->ctx);
        }
    }

    // pop a Coroutine from the pool
    Coroutine* new_coroutine(Closure* cb) {
        Coroutine* co = _co_pool.pop();
        ++co->use_count;
        co->cb = cb;
        if (!co->sched) {
            co->sched = this;
            co->sched_id = this->_id;
            co->stack = &_stack[co->idx & (_stack_num - 1)];
        }
        new (&co->it) timer_id_t(_timer_mgr.end());
        return co;
    }

    void recycle(Coroutine* co) {
        co->it.~timer_id_t();
        if (co->pbuf) {
            if (co->buf.capacity() > 8192 || _bufs.size() >= 128) {
                co->buf.reset();
                goto end;
            }
            _bufs.push_back(co->pbuf);
            co->pbuf = 0;
        }

    end:
        _co_pool.push(co);
    }

  private:
    std::atomic_int64_t _cputime{0};
    co::sync_event _ev;
    Epoll* _epoll;
    std::atomic_bool _stopped{false};

    TaskManager _task_mgr;
    TimerManager _timer_mgr;
    uint32_t _wait_ms;  // time the epoll to wait for
    bool _timeout;
    co::vector<void*> _bufs;
    CoroutinePool _co_pool;
    Coroutine* _running;   // the current running coroutine
    Coroutine* _main_co;   // save the main context
    uint16_t _id;          // scheduler id
    uint16_t _sched_num;   // number of schedulers
    uint32_t _stack_num;   // number of stacks per scheduler
    uint32_t _stack_size;  // size of the stack
    Stack* _stack;         // stack array
};

class SchedManager {
  public:
    SchedManager();
    ~SchedManager();

    inline Sched* next_sched() const noexcept { return _next(_scheds); }

    inline const co::vector<Sched*>& scheds() const noexcept { return _scheds; }

    void stop();

  private:
    std::function<Sched*(const co::vector<Sched*>&)> _next;
    co::vector<Sched*> _scheds;
};

inline std::atomic_bool& is_active() noexcept {
    static std::atomic_bool _is_active;
    return _is_active;
}

inline Sched*& current_sched() noexcept {
    static thread_local Sched* _sched;
    return _sched;
}

}}  // namespace co::xx
