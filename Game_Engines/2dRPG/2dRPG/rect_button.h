#ifndef _RECT_BUTTON_H_
#define _RECT_BUTTON_H_

#include "text.h"
#include "button.h"

class RectButton : public Button
{
public:
	RectButton()
	{
		fill_color = RGB(30, 30, 30);
		fill_touched_color = RGB(219, 210, 131);
		fill_down_color = RGB(97, 85, 38);
		line_color = RGB(150, 150, 150);
		text.set_parent_component(this);
		text.set_u_format(DT_CENTER);
		text.set_center(CenterType::Rect);
		text.set_using_camera(false);
	}
	~RectButton() = default;

	void on_draw();

	Text& get_text() { return text; }

	void set_fill_color(COLORREF color) { fill_color = color; }
	COLORREF get_fill_color() const { return fill_color; }
	void set_fill_touched_color(COLORREF color) { fill_touched_color = color; }
	COLORREF get_fill_touched_color() const { return fill_touched_color; }
	void set_fill_down_color(COLORREF color) { fill_down_color = color; }
	COLORREF get_fill_down_color() const { return fill_down_color; }

	void set_line_color(COLORREF color) { line_color = color; }
	COLORREF get_line_color() const { return line_color; }

private:
	COLORREF fill_color;			// 填充颜色
	COLORREF fill_touched_color;	// 触碰时填充颜色
	COLORREF fill_down_color;		// 按下时填充颜色
	COLORREF line_color;			// 线条颜色

	Text text;						// 文本

};

#endif // !_RECT_BUTTON_H_

