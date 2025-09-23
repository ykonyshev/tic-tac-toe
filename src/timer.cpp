#include "timer.hpp"

#include <cstdint>
#include <stdexcept>

// Compute the difference between two `timespec`s in nanoseconds
int64_t operator-(const struct timespec& lhs, const struct timespec& rhs) {
    return (int64_t(1'000'000'000) * (lhs.tv_sec - rhs.tv_sec)) +
           (lhs.tv_nsec - rhs.tv_nsec);
}

void Timer::start() {
#ifdef _POSIX_CPUTIME
    assert(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_startedAt) == 0);
#else
    m_startedAt = clock();
#endif
    m_wasStarted = true;
}

double Timer::end() const {
    if (!m_wasStarted) {
        throw std::runtime_error("The timer was never started, nothing to end");
    }

#ifdef _POSIX_CPUTIME
    timespec endedAt = {};
    assert(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endedAt) == 0);

    double elapsedMs = 1e-6 * double(endedAt - m_startedAt);
#else
    clock_t endedAt = clock();
    double elapsedMs = double(endedAt - m_startedAt) / CLOCKS_PER_SEC * 1000.0;
#endif
    return elapsedMs;
}
