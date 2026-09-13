#include "timer.hpp"

Timer *Timer::addTimerChild(Object *parent, float interval)
{
    auto timer = new Timer();
    timer->setInterval(interval);
    if (parent) parent->addChild(timer);
    timer->setActive(false); // 默认不激活
    return timer;
}

void Timer::update(float dt)
{
    timer_ += dt;
    if (timer_ >= interval_) {
        timer_ = 0.0f;
        time_out_ = true;
    }
}

bool Timer::timeOut()
{
    if (time_out_) {
        time_out_ = false;
        return true;
    }
    return false;
}
