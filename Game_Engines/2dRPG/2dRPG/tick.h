#ifndef _TICK_H_
#define _TICK_H_

#include "var_g.h"

#include <Windows.h>

class Tick
{
public:
	Tick(int fps = 60):last_tick(GetTickCount()),tpf(0)
	{
		if (fps <= 0) fps = 60;
		limit_fps(fps);
	}

	~Tick() = default;

	void on_update();

	void limit_fps(int fps)
	{
		tpf_limit = (int)(1000.0f / (float)fps);
	}

private:
	int tpf;							// 毫秒每帧
	int tpf_limit;						// 限制的 毫秒每帧
	int last_tick;						// 上一刻 tick

};


#endif // !_TICK_H_