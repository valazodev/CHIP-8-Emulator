#ifndef TIMER_H
#define TIMER_H

#include <chrono>

template <typename unit>
class Timer
{
public:
    Timer();
    long long getTime();
    void start();
    void stop();

private:
    using clock = std::chrono::steady_clock;
    clock::time_point last_time;
    clock::duration delta_time;
    bool isRunning = false;
};

// Template declaration
template class Timer <std::chrono::nanoseconds>;
template class Timer <std::chrono::microseconds>;
template class Timer <std::chrono::milliseconds>;
template class Timer <std::chrono::seconds>;
template class Timer <std::chrono::minutes>;
template class Timer <std::chrono::hours>;

#endif // TIMER_H
