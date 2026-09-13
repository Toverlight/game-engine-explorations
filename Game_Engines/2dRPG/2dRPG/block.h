#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "log.h"
#include "util.h"
#include "object.h"

#include <graphics.h>

extern Log* log_p;

class Block : public Object
{
public:
	Block()
	{
		set_size(20,20);
		use_radius(false);
		fill_color = RGB(87, 87, 87);
		line_color = RGB(30, 30, 30);

		log_p->setTarget(LogTarget::Console);
		log_p->info("Block created.");
	}
	~Block()
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

#endif // !_BLOCK_H_

