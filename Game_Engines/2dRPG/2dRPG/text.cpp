#include "text.h"
#include "var_g.h"

extern GVar* gv_p;

void Text::on_update()
{
	const Vector2& abs_coordinates = get_abs_coordinates();
	int width = get_textwidth();
	int height = get_textheight();
	if (recorded_point.x != abs_coordinates.x || recorded_point.y != abs_coordinates.y ||
		temp_center != center_t || rect.right - rect.left != width ||
		rect.bottom - rect.top != height)
	{
		if (center_t == CenterType::Point)
		{
			const Vector2& parent_abs_coordinates = get_abs_coordinates() - get_rel_coordinates();
			rect.left = parent_abs_coordinates.x - width / 2;
			rect.top = parent_abs_coordinates.y - height / 2;
		}
		else if (center_t == CenterType::Rect)
		{
			const Vector2& parent_abs_coordinates = get_abs_coordinates() - get_rel_coordinates();
			Vector2 parent_size = get_if_parent_size();
			int w_offset = 0, h_offset = 0;
			if (parent_size.x != 0 || parent_size.y != 0)
			{
				w_offset = (parent_size.x - width) / 2;
				h_offset = (parent_size.y - height) / 2;
			}
			rect.left = parent_abs_coordinates.x + w_offset;
			rect.top = parent_abs_coordinates.y + h_offset;
		}
		else if (center_t == CenterType::HorizontalOnly)
		{
			const Vector2& parent_abs_coordinates = get_abs_coordinates() - get_rel_coordinates();
			Vector2 parent_size = get_if_parent_size();
			int w_offset = 0;
			if (parent_size.x != 0)
			{
				w_offset = (parent_size.x - width) / 2;
			}
			rect.left = parent_abs_coordinates.x + w_offset;
			rect.top = abs_coordinates.y;
		}
		else if (center_t == CenterType::VerticalOnly)
		{
			const Vector2& parent_abs_coordinates = get_abs_coordinates() - get_rel_coordinates();
			Vector2 parent_size = get_if_parent_size();
			int h_offset = 0;
			if (parent_size.x != 0)
			{
				h_offset = (parent_size.y - height) / 2;
			}
			rect.left = abs_coordinates.x;
			rect.top = parent_abs_coordinates.y + h_offset;
		}
		else
		{
			rect.left = abs_coordinates.x;
			rect.top = abs_coordinates.y;
		}
		rect.right = rect.left + width;
		rect.bottom = rect.top + height;

		recorded_point = abs_coordinates;
		temp_center = center_t;
	}
}

void Text::on_draw()
{
	if (content)
	{
		setbkmode(mode);
		settextcolor(color);
		settextstyle(nHeight, nWidth, lpszFace);

		drawtext(content, &rect, u_format, using_camera);
		if (gv_p->check_debug())						// 调试模式下
		{
			setlinecolor(RGB(85, 177, 85));
			rectangle(rect.left, rect.top, rect.right, rect.bottom, using_camera);
		}
	}
}

int Text::get_tchar_num() const
{
	const TCHAR* tchar_p = content;
	if (!content) return 0;
	int sum = 0;
	while (*tchar_p != _T('\0'))
	{
		++sum;
		++tchar_p;
	}
	return sum;
}
