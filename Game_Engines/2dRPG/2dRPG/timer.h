#ifndef _TIMER_H_
#define _TIMER_H_

#include "var_g.h"

#include <Windows.h>
#include <functional>

extern GVar* gv_p;

class Timer
{
public:
	Timer()
	{
		pass_time = 0;
		wait_time = 0;
		one_shot = true;
		is_shotted = false;
		paused = false;

		callback = nullptr;
	}
	~Timer() = default;

	void restart()						// 重置定时器
	{
		pass_time = 0;
		is_shotted = false;
		paused = false;
	}

	void resume()						// 继续
	{
		paused = false;
	}

	void pause()						// 暂停
	{
		paused = true;
	}

	void set_one_shot(bool flag)		// 设置是否单次触发
	{
		one_shot = flag;
	}

	void set_wait_time(int ms)			// 设置等待时间
	{
		wait_time = ms;
	}

	void on_update();					

	void set_callback(std::function<void()> callback) { this->callback = callback; }

private:
	int pass_time;						// 从计时开始到当前的时间(ms)
	int wait_time;						// 等待的时间(ms)
	bool one_shot;						// 是否单次出发

	bool is_shotted;					// 是否已被触发
	bool paused;						// 是否暂停
	
	std::function<void()> callback;		// 回调函数
};

#endif // !_TIMER_H_