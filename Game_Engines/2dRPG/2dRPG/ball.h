#ifndef _BALL_H_
#define _BALL_H_

#include "log.h"
#include "util.h"
#include "timer.h"
#include "object.h"

#include <iostream>

extern Log* log_p;

class Ball : public Object
{
public:
	Ball()
	{
		set_radius(10);
		use_radius(true);
		fill_color = RGB(138, 226, 138);
		line_color = RGB(30, 30, 30);

		log_p->setTarget(LogTarget::Console);
		log_p->info("Ball created.");
	}
	~Ball()
	{
		log_p->setTarget(LogTarget::Console);
		log_p->info("Ball deleted.");
	}

	void on_update();

	void on_draw();

	void set_fill_color(COLORREF color) { fill_color = color; }
	COLORREF get_fill_color() const { return fill_color; }

	void set_line_color(COLORREF color) { line_color = color; }
	COLORREF get_line_color() const { return line_color; }

private:
	COLORREF fill_color;		// 填充颜色
	COLORREF line_color;		// 线条颜色
};

#endif // !_BALL_H_

