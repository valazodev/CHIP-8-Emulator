#include "CHIP-8/timer.h"
#include <typeinfo>

using namespace std;

template <typename unit>
Timer<unit>::Timer()
{
    last_time = clock::now();
    delta_time = clock::duration(0);
}

template <typename unit>
void Timer<unit>::start()
{
    last_time = clock::now();
    delta_time = clock::duration(0);
    isRunning = true;
}

template <typename unit>
void Timer<unit>::stop()
{
    delta_time = clock::now() - last_time;
    isRunning = false;
}

template <typename unit>
long long Timer<unit>::getTime()
{
    using namespace chrono;
    long long conversion = 1;

    #define SWITCH(expr)  auto& switched = typeid(expr);
    #define CASE(val)     if (typeid(val) == switched)
    #define BREAK         else

    SWITCH(unit)
    {
        CASE(nanoseconds)  conversion = 1;      BREAK
        CASE(microseconds) conversion = 1e3;    BREAK
        CASE(milliseconds) conversion = 1e6;    BREAK
        CASE(seconds)      conversion = 1e9;    BREAK
        CASE(minutes)      conversion = 60e9;   BREAK
        CASE(hours)        conversion = 3600e9;
    }
    #undef SWITCH
    #undef CASE
    #undef BREAK

    if (isRunning)
        delta_time = clock::now() - last_time;

    return delta_time.count() / conversion;
}
