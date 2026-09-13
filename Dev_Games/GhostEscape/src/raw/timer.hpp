#ifndef TIMER_HPP
#define TIMER_HPP

#include "../core/object.hpp"

class Timer : public Object // 创建的时候，默认为!active
{
protected:
    float timer_ = 0.0f; // 定时器
    float interval_ = 3.0f; // 定时器间隔
    bool time_out_ = false; // 是否超时
public:
    static Timer* addTimerChild(Object* parent, float interval = 3.0f);

    virtual void update(float dt) override;
    void start() { is_active_ = true; }
    void stop() { is_active_ = false; }
    bool timeOut();
    float getProgress() const { return timer_ / interval_; } // 获取进度，0.0f - 1.0f

    // getters and setters
    float getInterval() const { return interval_; } // 获取定时器间隔
    void setInterval(float interval) { interval_ = interval; } // 设置定时器间隔
    float getTimer() const { return timer_; } // 获取定时器
    void setTimer(float timer) { timer_ = timer; } // 设置定时器
};


#endif // TIMER_HPP