#ifndef __TIMER_H__
#define __TIMER_H__

#include <windows.h>

namespace ToyMan {

class Timer {
public:
    Timer();
    
    void Start();
    void Stop();
    void Reset();
    
    double Time();
private:
    // Private Timer Data
    double time0, elapsed;
    bool running;
    double GetTime();

    // Private Windows Timer Data
    LARGE_INTEGER performance_counter, performance_frequency;
    double one_over_frequency;
};

} //namespace ToyMan 

#endif //#ifndef __TIMER_H__