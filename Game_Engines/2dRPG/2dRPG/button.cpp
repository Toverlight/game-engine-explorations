#include "button.h"

void Button::on_input()
{
	const ExMessage& mousemove_msg = gv_p->get_mousemove_msg();
	if (mousemove_msg.x < 0 || mousemove_msg.x > getwidth() ||
		mousemove_msg.y < 0 || mousemove_msg.y > getheight())
		return;
	const Vector2& abs_coordinates = get_abs_coordinates();
	if (abs_coordinates.x > getwidth() || abs_coordinates.x + get_size().x < 0 ||
		abs_coordinates.y > getheight() || abs_coordinates.y + get_size().y < 0)
		return;
	const ExMessage& mouseclick_msg = gv_p->get_mouseclick_msg();
	if (mousemove_msg.x >= abs_coordinates.x &&
		mousemove_msg.x <= abs_coordinates.x + monitor_square.x &&
		mousemove_msg.y >= abs_coordinates.y &&
		mousemove_msg.y <= abs_coordinates.y + monitor_square.y)
	{
		if (!check_touched())
			set_touched(true);
		if (mouseclick_msg.message == WM_LBUTTONDOWN)
		{
			set_down(true);
			gv_p->clear_mouseclick_msg();
		}
		else if (mouseclick_msg.message == WM_LBUTTONUP && check_down())
		{
			set_down(false);
			gv_p->clear_mouseclick_msg();

			//if (gv_p->check_debug())
			//{
			//	log_p->setTarget(LogTarget::Console);
			//	log_p->debug("Button shot!");
			//}

			if (callback) callback();
			else
			{
				log_p->setTarget(LogTarget::Console);
				log_p->warn("None callback for this button, is it your wanted?");
			}
		}

	}
	else
	{
		if (check_touched())
			set_touched(false);
		if (mouseclick_msg.message == WM_LBUTTONUP && check_down())
			set_down(false);
	}

	
}
