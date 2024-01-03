#ifdef _WIN32

#include "iocp.h"

namespace co {

Iocp::Iocp(int32_t sched_id) : _signaled(), _sched_id(sched_id) {
    _iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);
    CHECK(_iocp != 0) << "create iocp failed..";
    _ev = (OVERLAPPED_ENTRY*)::calloc(1024, sizeof(OVERLAPPED_ENTRY));
}

Iocp::~Iocp() {
    if (_iocp) {
        CloseHandle(_iocp);
        _iocp = 0;
    }
    ::free(_ev);
    _ev = nullptr;
}

}  // namespace co

#endif
