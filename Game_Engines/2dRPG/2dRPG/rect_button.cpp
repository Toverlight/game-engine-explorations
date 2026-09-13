#include "rect_button.h"
#include "var_g.h"

extern GVar* gv_p;
extern Log* log_p;

void RectButton::on_draw()
{
	const Vector2& abs_coordinates = get_abs_coordinates();
	setlinecolor(line_color);
	if (check_down())
	{
		setfillcolor(fill_down_color);
	}
	else if (check_touched())
	{
		setfillcolor(fill_touched_color);
	}
	else
	{
		setfillcolor(fill_color);
	}
	fillrectangle(abs_coordinates.x, abs_coordinates.y,
		abs_coordinates.x + get_size().x, abs_coordinates.y + get_size().y, false);
}

