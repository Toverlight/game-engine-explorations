#ifndef TIMER_PRECISION_GUARD_H
#define TIMER_PRECISION_GUARD_H

#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")  //  ÷∂Ø¡¥Ω” winmm.lib

class TimerPrecisionGuard {
public:
    TimerPrecisionGuard() {
        timeBeginPeriod(1);
    }

    ~TimerPrecisionGuard() {
        timeEndPeriod(1);
    }
};

#endif // !TIMER_PRECISION_GUARD_H

