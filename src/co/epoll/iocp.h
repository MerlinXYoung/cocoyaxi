#pragma once
#ifdef _WIN32
#include <atomic>
#include <stdint.h>

#include "../hook.h"
#include "../sock_ctx.h"
#include "co/co.h"
#include "co/error.h"
#include "co/log.h"


namespace co {

class Iocp {
  public:
    Iocp(int32_t sched_id);
    ~Iocp();

    bool add_event(sock_t fd) {
        if (fd == (sock_t)-1) return false;
        auto& ctx = co::get_sock_ctx(fd);
        if (ctx.has_event()) return true;  // already exists

        if (CreateIoCompletionPort((HANDLE)fd, _iocp, fd, 0) != 0) {
            ctx.add_event();
            return true;
        } else {
            // ELOG << "iocp add socket " << fd << " error: " << co::strerror();
            // always return true here.
            return true;
        }
    }

    // for close
    void del_event(sock_t fd) {
        if (fd != (sock_t)-1) co::get_sock_ctx(fd).del_event();
    }

    // for half-shutdown read
    void del_ev_read(sock_t fd) {
        if (fd != (sock_t)-1) co::get_sock_ctx(fd).del_ev_read();
    }

    // for half-shutdown write
    void del_ev_write(sock_t fd) {
        if (fd != (sock_t)-1) co::get_sock_ctx(fd).del_ev_write();
    }

    inline int wait(int ms) {
        ULONG n = 0;
        const BOOL r = __sys_api(GetQueuedCompletionStatusEx)(_iocp, _ev, 1024, &n, ms, false);
        if (r == TRUE) return (int)n;
        const uint32_t e = ::GetLastError();
        return e == WAIT_TIMEOUT ? 0 : -1;
    }

    void signal() noexcept {
        if (!_signaled.test_and_set()) {
            const BOOL r = PostQueuedCompletionStatus(_iocp, 0, 0, 0);
            if (!r) {
                const uint32_t e = ::GetLastError();
                ELOG << "PostQueuedCompletionStatus error: " << co::strerror(e);
            }
        }
    }

    inline const OVERLAPPED_ENTRY& operator[](int i) const { return _ev[i]; }
    inline void* user_data(const OVERLAPPED_ENTRY& ev) const noexcept { return ev.lpOverlapped; }
    inline bool is_ev_pipe(const OVERLAPPED_ENTRY& ev) const noexcept {
        return ev.lpOverlapped == 0;
    }
    inline void handle_ev_pipe() { _signaled.clear(std::memory_order_release); }

  private:
    HANDLE _iocp;
    OVERLAPPED_ENTRY* _ev;
    std::atomic_flag _signaled;
    int32_t _sched_id;
};

typedef OVERLAPPED_ENTRY epoll_event;
typedef Iocp Epoll;

}  // namespace co

#endif
