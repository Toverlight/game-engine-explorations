#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "log.h"
#include "timer.h"
#include "var_g.h"

#include <graphics.h>
#include <functional>

extern Log* log_p;
extern GVar* gv_p;

// 按钮
class Button : public Component
{
public:
	Button()
	{
		monitor_square = { 0,0 };
		is_down = false;
		is_touched = false;
		callback = nullptr;
	}
	virtual ~Button() = default;

	virtual void on_input();

	virtual void on_update() { };

	virtual void on_draw() { };

	void set_size(const Vector2& size) override
	{
		Component::set_size(size);
		monitor_square = size;
	}
	void set_size(int w, int h) override
	{
		Component::set_size(w, h);
		monitor_square.x = w;
		monitor_square.y = h;
	}

	// 设置检测范围，默认矩形，可另外重写圆形等形状的属性与判定
	virtual void set_monitor_square(int width, int height)
	{
		set_size(monitor_square);			// 按钮的大小与探测范围一致
	}
	virtual const Vector2& get_monitor_square() const { return monitor_square; }

	void set_down(bool flag) { is_down = flag; }
	bool check_down() { return is_down; }

	void set_touched(bool flag) { is_touched = flag; }
	bool check_touched() { return is_touched; }

	void set_callback(std::function<void()> callback) { this->callback = callback; }



private:
	Vector2 monitor_square;			// 监测范围

	bool is_down;					// 是否被按下
	bool is_touched;				// 是否被触摸

	std::function<void()> callback;	// 回调函数
};

#endif // !_BUTTON_H_