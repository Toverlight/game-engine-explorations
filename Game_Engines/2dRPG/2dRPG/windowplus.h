#ifndef _WINDOWPLUS_H_
#define _WINDOWPLUS_H_

#include "vector2.h"
#include "window_manager.h"

#include <graphics.h>

class Window
{
	friend class WindowManager;

public:
	void set_size(float width, float height) { size = { width, height }; }

	void set_pos(float width, float height) { pos = { width, height }; }

	void set_show_console(bool flag) { show_console = flag; }

	void set_background_color(COLORREF color) { background_color = color; }

	void run() 
	{
		if (!is_running){
			initgraph(size.x, size.y, show_console ? EX_SHOWCONSOLE : NULL);
			// 获取EasyX窗口句柄
			HWND hwnd = GetHWnd();

			// 使用Windows API设置窗口样式，允许最大化
			LONG style = GetWindowLong(hwnd, GWL_STYLE);
			style |= WS_MAXIMIZEBOX | WS_SIZEBOX; // 添加最大化按钮和窗口大小调整边框
			SetWindowLong(hwnd, GWL_STYLE, style);

			// 使窗口立即应用样式更改
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

			setbkcolor(background_color);
			cleardevice();

			is_running = true;
		}
	}

	bool check_running() { return is_running; }
	void shutdown() { is_running = false; }

	void close() { closegraph(); }

private:
	Window() = default;

	Window(bool flag)
	{
		show_console = flag;
		size = { 600,800 };
	}

	~Window() = default;

private:
	Vector2 size;
	Vector2 pos;
	
	bool show_console = false;
	bool is_running = false;

	COLORREF background_color = RGB(40,40,40);

};

#endif // !_WINDOWPLUS_H_