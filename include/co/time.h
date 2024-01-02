#pragma once

#include "def.h"
#include "fastring.h"

namespace co {
namespace now {

#ifdef _WIN32
namespace xx {

struct __coapi Initializer {
    Initializer();
    ~Initializer() = default;
};

static Initializer g_initializer;

}  // namespace xx
#endif

// monotonic timestamp in nanoseconds
__coapi int64_t ns();

// monotonic timestamp in microseconds
__coapi int64_t us();

// monotonic timestamp in milliseconds
__coapi int64_t ms();

// "%Y-%m-%d %H:%M:%S" ==> 2023-01-07 18:01:23
__coapi fastring str(const char* fm = "%Y-%m-%d %H:%M:%S");

}  // namespace now

namespace epoch {

// microseconds since epoch
__coapi int64_t us();

// milliseconds since epoch
__coapi int64_t ms();

}  // namespace epoch

class __coapi Timer {
  public:
    Timer() { _start = now::ns(); }

    void restart() { _start = now::ns(); }

    int64_t ns() const { return now::ns() - _start; }

    int64_t us() const { return this->ns() / 1000; }

    int64_t ms() const { return this->ns() / 1000000; }

  private:
    int64_t _start;
};

}  // namespace co

namespace now = co::now;
namespace epoch = co::epoch;

namespace _xx { namespace sleep {

__coapi void ms(uint32_t n);

__coapi void sec(uint32_t n);

}}  // namespace _xx::sleep

using namespace _xx;
