#include <stdexcept>

#define AKANE_DISABLE_COPY(KLASS)
#define AKANE_DISABLE_COPY_AND_MOVE(KLASS)

#define AKANE_ASSERT(condition) assert(condition)
#define AKANE_REQUIRE(condition) assert(condition)

#define AKANE_NO_IMPL() (throw std::runtime_error{"no impl"})