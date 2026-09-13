#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "atlas.h"
#include "timer.h"
#include "component.h"

#include <functional>

class Animation : public Component
{
public:
	Animation();
	~Animation() = default;
	// 暂停
	void pause()
	{ 
		timer_frame.pause();
		timer_all.pause();
	}
	// 继续
	void resume()
	{
		timer_frame.resume();
		timer_all.resume();
	}
	// 重新播放
	void restart()
	{
		timer_frame.restart();
		timer_all.restart();
	}

	// 显示
	void show() { set_visible(true); }
	// 隐藏
	void hide() { set_visible(false); }

	void on_update();

	void on_draw();

	// 设置图集
	void set_atlas(Atlas* atlas_p) { this->atlas_p = atlas_p; }

	// 设置帧间隔。若在不循环状态设置，则自动设置到动画播放完最后一帧时结束（先设置图集）
	void set_interval(int ms)
	{
		if (!atlas_p) return;
		interval = ms;
		timer_frame.set_wait_time(interval);
		if (!is_loop)
			timer_all.set_wait_time(atlas_p->get_frame_num() * interval);
	}
	// 设置是否循环播放。若不循环，则自动设置到动画播放完最后一帧时结束（先设置图集）
	void set_loop(bool flag)
	{ 
		if (!atlas_p) return;
		if (!(is_loop = flag))
			timer_all.set_wait_time(atlas_p->get_frame_num() * interval);
	}

	// 设置动画持续时间（仅先设置循环后有效）
	void set_duration(int ms)
	{
		if (is_loop)
		{
			timer_all.set_wait_time(ms);
		}
	}
	// 设置动画结束回调函数
	void set_callback(std::function<void()> callback) 
	{ 
		timer_all.set_callback(callback);
	}

	void set_using_camera(bool flag) { using_camera = flag; }

private:

	int interval;						// 帧间隔(ms)
	int idx_frame;						// 帧索引

	bool is_loop;						// 是否循环

	Atlas* atlas_p;						// 图集指针
	Timer timer_frame;					// 帧定时器
	Timer timer_all;					// 总定时器

	bool using_camera;					// 使用相机

};

#endif // !_ANIMATION_H_
