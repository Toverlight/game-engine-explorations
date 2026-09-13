#include "image_button.h"

void ImageButton::on_draw()
{
	IMAGE* img_p = img_idle;
	if (check_touched())
		img_p = img_touched;
	if (check_down())
		img_p = img_down;

	if (img_p)
	{
		const Vector2& abs_coordinates = get_abs_coordinates();
		putimage_alpha(abs_coordinates.x, abs_coordinates.y, img_p);
	}

}
