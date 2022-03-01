#include "Timer.h"

#include <windows.h>
#include <cassert>

using namespace std;

namespace ToyMan {

Timer::Timer()
{
    QueryPerformanceFrequency( &performance_frequency );
    one_over_frequency = 1.0/((double)performance_frequency.QuadPart);
    time0 = elapsed = 0;
    running = 0;
}




double Timer::GetTime()
{
    QueryPerformanceCounter( &performance_counter );
    return (double) performance_counter.QuadPart * one_over_frequency;
}



void Timer::Start()
{
    assert( !running );
    running = 1;
    time0 = GetTime();
}



void Timer::Stop()
{
    assert( running );
    running = 0;

    elapsed += GetTime() - time0;
}



void Timer::Reset()
{
    running = 0;
    elapsed = 0;
}



double Timer::Time()
{
    if (running) {
        Stop();
        Start();
    }
    return elapsed;
}

} //namespace ToyMan