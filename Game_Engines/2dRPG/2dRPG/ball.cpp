#include "ball.h"

void Ball::on_update()
{
}

void Ball::on_draw()
{
	setlinecolor(line_color);
	setfillcolor(fill_color);
	const Vector2& abs_coordinates = get_abs_coordinates();
	fillcircle(abs_coordinates.x, abs_coordinates.y, get_radius(),gv_p->get_camera()->get_scale(), true);
	if (gv_p->check_debug())					// 调试模式下
	{
		if (!(get_cur_collision_normal() == Vector2(0, 0)))
		{
			Vector2 dest = abs_coordinates - get_cur_collision_normal() * get_radius();
			setlinecolor(GREEN);
			line(abs_coordinates.x, abs_coordinates.y, dest.x, dest.y, gv_p->get_camera()->get_scale(),
				true);
		}

		if (!(get_cur_collision_normal_object() == Vector2(0, 0)))
		{
			Vector2 dest_obj = abs_coordinates - get_cur_collision_normal_object() * get_radius();
			setlinecolor(RGB(204, 151, 249));
			line(abs_coordinates.x, abs_coordinates.y, dest_obj.x, dest_obj.y,
				gv_p->get_camera()->get_scale(), true);

			set_cur_collision_normal_object(Vector2(0, 0));
		}
		
	}
}
