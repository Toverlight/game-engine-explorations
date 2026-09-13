#include "block.h"

void Block::on_update()
{
}

void Block::on_draw()
{
	setlinecolor(line_color);
	setfillcolor(fill_color);
	const Vector2& abs_coordinates = get_abs_coordinates();
	fillrectangle(abs_coordinates.x, abs_coordinates.y, abs_coordinates.x + get_size().x, 
		abs_coordinates.y + get_size().y, gv_p->get_camera()->get_scale(), true);
	if (gv_p->check_debug())
	{
		const Vector2& cur_collision_normal = get_cur_collision_normal();
		const Vector2& cur_collision_normal_object = get_cur_collision_normal_object();
		const Vector2& center_point = abs_coordinates + (get_size() * 0.5f);
		if (!(cur_collision_normal == Vector2(0, 0)))
		{
			Vector2 dest;
			if (cur_collision_normal.x == 0)
			{
				dest = center_point + cur_collision_normal * (get_size().y / 2);
			}
			else
			{
				dest = center_point + cur_collision_normal * (get_size().x / 2);
			}
			setlinecolor(GREEN);
			line(center_point.x, center_point.y, dest.x, dest.y, gv_p->get_camera()->get_scale(), true);
		}

		if (!(cur_collision_normal_object == Vector2(0, 0)))
		{
			Vector2 dest_obj;
			if (cur_collision_normal_object.x == 0)
			{
				dest_obj = center_point + cur_collision_normal_object * (get_size().y / 2);
			}
			else if (cur_collision_normal_object.y == 0)
			{
				dest_obj = center_point + cur_collision_normal_object * (get_size().x / 2);
			}
			else
			{
				bool to_left = false;
				bool to_top = false;
				to_left = (cur_collision_normal_object.x < 0 ? true : false);
				to_top = (cur_collision_normal_object.y < 0 ? true : false);

				if (to_left)
					dest_obj.x = abs_coordinates.x;
				else
					dest_obj.x = abs_coordinates.x + get_size().x;

				if (to_top)
					dest_obj.y = abs_coordinates.y;
				else
					dest_obj.y = abs_coordinates.y + get_size().y;
			}
			setlinecolor(RGB(204, 151, 249));
			line(center_point.x, center_point.y, dest_obj.x, dest_obj.y, gv_p->get_camera()->get_scale(),
				true);

			set_cur_collision_normal_object(Vector2(0, 0));
		}

	}
}
