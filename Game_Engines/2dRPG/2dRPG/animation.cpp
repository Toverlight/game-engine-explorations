#include "util.h"
#include "animation.h"

Animation::Animation()
{
	interval = 500;
	is_loop = false;
	atlas_p = nullptr;
	idx_frame = 0;

	timer_frame.set_wait_time(interval);
	timer_frame.set_one_shot(false);
	timer_frame.set_callback([&]()
		{
			if (idx_frame < atlas_p->get_frame_num() - 1) {
				idx_frame++;
			}
			else if (is_loop)
			{
				idx_frame = 0;
			}
		}
	);

	timer_all.set_wait_time(2000);
	timer_all.set_one_shot(true);

	using_camera = true;

}

void Animation::on_update()
{
	timer_frame.on_update();
	timer_all.on_update();
}

void Animation::on_draw()
{
	const Vector2& abs_coordinates = get_abs_coordinates();
	putimage_alpha(abs_coordinates.x, abs_coordinates.y, atlas_p->get_frame_at(idx_frame), using_camera);
}
