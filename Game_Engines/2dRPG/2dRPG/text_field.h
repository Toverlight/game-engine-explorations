#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#ifndef TF_TSTR_LEN_MAX
#define TF_TSTR_LEN_MAX 256
#endif // !TF_TSTR_LEN_MAX

#include "component.h"
#include "timer.h"
#include "text.h"
#include "layout.h"		// TODO: 文本框的“框”

// 单行文本框
class TextField : public Component
{
public:
	TextField();
	TextField(int width);
	~TextField();

	void on_input() override;

	void on_update() override;

	void on_draw() override;

	// 是否聚焦当前文本框，输入就绪
	void set_focused(bool flag) { is_focused = flag; }
	// 获取聚焦状态
	bool get_focused() const { return is_focused; }

	// 获取文本引用
	const Text& get_text() const { return text; }

	// 计算当前文本加上光标符号的宽度
	int get_text_width() const
	{
		return (offset + 1) * text.get_text_info(TextInfo::W).nWidth;
	}

	// 重置字符缓冲区
	void reset_tcBuf() { tcBuf = _T('\0'); }

	// 设置文本框背景颜色
	void set_bkcolor(COLORREF color) { bk_color = color; }
	// 获取文本框背景颜色
	COLORREF get_bkcolor() const { return bk_color; }

	// 设置文本框所容文字宽度
	void set_fieldwidth(int width)
	{
		if (width < 1) return;
		field_width = width;
	}

private:
	Text text;								// 当前显示文本
	TCHAR* tstr;							// 已输入的字符串的指针
	int offset;								// 字符串指针偏移量
	Timer timer_cursor_flash;				// 光标闪烁状态更新定时器

	int field_width;						// 文本框宽度

	bool is_cursor_showing;					// 光标当前时刻是否显示
	bool is_focused;						// 文本框是否被选中

	TCHAR tcBuf;							// 待输入字符缓冲区
	bool is_capslock_on;					// 大写锁定是否开启
	bool is_shift_down;						// shift 键是否按下
	bool is_backspacing;					// 是否正在回退

	COLORREF bk_color;						// 文本框背景颜色

};

#endif // !_TEXT_FIELD_H_

