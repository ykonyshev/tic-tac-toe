#pragma once

#include <cassert>
#include <cstdint>
#include <ctime>

#ifdef __unix__
#include <unistd.h>
#endif

int64_t operator-(const struct timespec& lhs, const struct timespec& rhs);

struct Timer {
public:
    Timer() = default;

    void start();
    double end() const;

private:
    bool m_wasStarted = false;

#ifdef _POSIX_CPUTIME
    timespec m_startedAt = {};
#else
    clock_t m_startedAt = 0;
#endif
};
