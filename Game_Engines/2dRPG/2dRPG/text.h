#ifndef _TEXT_H_
#define _TEXT_H_

#include "util.h"
#include "component.h"

#include <iostream>
#include <graphics.h>
// 中心对齐方式
enum class CenterType
{
	None,					// 手动
	Point,					// 调整自身中心至上一级的点
	Rect,					// 调整自身中心至上一级矩形中心
	HorizontalOnly,			// 仅横向调整使自身位于上一级横向中心
	VerticalOnly			// 仅纵向调整使自身位于上一级纵向中心
};
// 字体信息
struct TextInfo
{
	int nHeight;
	int nWidth;
	LPCTSTR lpszFace;
	COLORREF color;
	int mode;
	// 字体信息类型
	enum InfoType
	{
		H = 1 << 0,			// 获取字体高度
		W = 1 << 1,			// 获取字体宽度
		L = 1 << 2,			// 获取字体类型
		C = 1 << 3,			// 获取字体颜色
		M = 1 << 4			// 获取字体背景透明模式
	};

	TextInfo()
	{
		nHeight = 0;
		nWidth = 0;
		lpszFace = nullptr;
		color = BLACK;
		mode = TRANSPARENT;
	}
};

// 单行文本
class Text : public Component
{
public:
	Text()
	{
		content = _T("");
		u_format = DT_CENTER;
		const Vector2& abs_coordicates = get_abs_coordinates();
		rect.left = rect.right = abs_coordicates.x;
		rect.top = rect.bottom = abs_coordicates.y;
		center_t = CenterType::Point;
		using_camera = true;
		set_text_style(30, 15, _T("宋体"));
		set_text_color(BLACK);
		set_bk_mode(TRANSPARENT);

		recorded_point = { 0,0 };
		temp_center = CenterType::None;
	}
	Text(LPCTSTR lpctstr)
	{
		content = lpctstr;
		u_format = DT_CENTER;
		const Vector2& abs_coordinates = get_abs_coordinates();
		rect.left = abs_coordinates.x;
		rect.top = abs_coordinates.y;
		rect.right = rect.left + textwidth(content);
		rect.bottom = rect.top + textheight(content);
		center_t = CenterType::Point;
		using_camera = true;
		set_text_style(30, 15, _T("宋体"));
		set_text_color(BLACK);
		set_bk_mode(TRANSPARENT);

		recorded_point = { 0,0 };
		temp_center = CenterType::None;
	}
	Text(LPCTSTR lpctstr, int nHeight, int nWidth, LPCTSTR lpszFace, COLORREF color, int mode)
	{
		content = lpctstr;
		u_format = DT_CENTER;
		const Vector2& abs_coordinates = get_abs_coordinates();
		rect.left = abs_coordinates.x;
		rect.top = abs_coordinates.y;
		rect.right = rect.left + textwidth(content);
		rect.bottom = rect.top + textheight(content);
		center_t = CenterType::Point;
		using_camera = true;
		set_text_style(nHeight, nWidth, lpszFace);
		set_text_color(color);
		set_bk_mode(mode);

		recorded_point = { 0,0 };
		temp_center = CenterType::None;
	}
	~Text() = default;

	// 设置文本内容
	void set_content(LPCTSTR lpctstr)
	{
		content = lpctstr;
	}

	LPCTSTR get_content() const { return content; }
	// 设置格式化正文方法
	void set_u_format(UINT u_format)
	{
		this->u_format = u_format;
	}

	void on_update();

	void on_draw();

	void set_center(CenterType center_t) { this->center_t = center_t; }

	void set_using_camera(bool flag) { using_camera = flag; }
	// 获取文本字体信息
	TextInfo get_text_info(TextInfo::InfoType info_t) const
	{
		TextInfo ret;
		if (info_t & TextInfo::H) ret.nHeight = nHeight;
		if (info_t & TextInfo::W) ret.nWidth = nWidth;
		if (info_t & TextInfo::L) ret.lpszFace = lpszFace;
		if (info_t & TextInfo::C) ret.color = color;
		if (info_t & TextInfo::M) ret.mode = mode;
		return ret;
	}

	// 获取字符个数
	int get_tchar_num() const;

	// 获取文本总宽
	int get_textwidth() const
	{
		settextstyle(nHeight, nWidth, lpszFace);
		settextcolor(color);
		setbkmode(mode);
		return textwidth(content);
	}
	// 获取文本总高
	int get_textheight() const
	{
		settextstyle(nHeight, nWidth, lpszFace);
		settextcolor(color);
		setbkmode(mode);
		return textheight(content);
	}

public:
	// 设置字体样式（指定高度、字符的平均宽度、字体名称）
	void set_text_style(int nHeight, int nWidth, LPCTSTR lpszFace)
	{
		this->nHeight = nHeight;
		this->nWidth = nWidth;
		this->lpszFace = lpszFace;
	}
	// 设置字体颜色
	void set_text_color(COLORREF color)
	{
		this->color = color;
	}
	// 设置背景模式（OPAQUE不透明、TRANSPARENT透明）
	void set_bk_mode(int mode)
	{
		this->mode = mode;
	}


private:
	LPCTSTR content;				// 文本内容
	RECT rect;						// 文本所在的矩形框

	int nHeight;					// 字符高度
	int nWidth;						// 字符宽度
	LPCTSTR lpszFace;				// 字体名称
	COLORREF color;					// 字体颜色
	int mode;						// 背景模式
	UINT u_format;					// 格式化正文方法（宏赋值）（框内对齐方式）

	CenterType center_t;			// 将文本框中心与父挂载坐标对齐的方式
	bool using_camera;				// 是否使用相机

	Vector2 recorded_point;			// 上一次更新的位置
	CenterType temp_center;			// 上一次更新的中心对齐方式
};


#endif // !_TEXT_H_