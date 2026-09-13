#include "slider.h"
#include "var_g.h"

void Slider::on_input()
{
	const ExMessage& mousemove_msg = gv_p->get_mousemove_msg();
	const Vector2& abs_coordinates = slidingBlock.get_abs_coordinates();
	const Vector2& size = slidingBlock.get_size();
	const Vector2& abs_coordinates_total = get_abs_coordinates();
	const Vector2& size_total = get_size();
	// ´¥Åö×´Ì¬
	if (mousemove_msg.x >= abs_coordinates_total.x &&
		mousemove_msg.x <= abs_coordinates_total.x + size_total.x &&
		mousemove_msg.y >= abs_coordinates_total.y &&
		mousemove_msg.y <= abs_coordinates_total.y + size_total.y)
	{
		is_inArea = true;
		if (mousemove_msg.x >= abs_coordinates.x &&
			mousemove_msg.x <= abs_coordinates.x + size.x &&
			mousemove_msg.y >= abs_coordinates.y &&
			mousemove_msg.y <= abs_coordinates.y + size.y)
		{
			slidingBlock.is_touched = true;
		}
		else
		{
			slidingBlock.is_touched = false;
		}
	}
	else
	{
		is_inArea = false;
		slidingBlock.is_touched = false;
	}
	// °´ÏÂ×´Ì¬
	if (gv_p->check_if_mouseclick_msg())
	{
		const ExMessage& mouseclick_msg = gv_p->get_mouseclick_msg();
		if (mouseclick_msg.message == WM_LBUTTONDOWN)
		{
			if (mouseclick_msg.x >= abs_coordinates.x &&
				mouseclick_msg.x <= abs_coordinates.x + size.x &&
				mouseclick_msg.y >= abs_coordinates.y &&
				mouseclick_msg.y <= abs_coordinates.y + size.y)
			{
				slidingBlock.is_down = true;
				if (st == ScrollType::Vertical)
					recorded_XorY = mouseclick_msg.y;
				else
					recorded_XorY = mouseclick_msg.x;
				gv_p->clear_mouseclick_msg();
			}
		}
		else if (mouseclick_msg.message == WM_LBUTTONUP)
		{
			slidingBlock.is_down = false;
		}
	}

	if (containSum > 0)
	{
		// Êó±êÍÏ×§
		if (slidingBlock.is_down)
		{
			if (st == ScrollType::Vertical)
			{
				if (abs(mousemove_msg.y - recorded_XorY) >= slidingRail.get_size().y *
					(1.0f / (float)containSum))
				{
					if (mousemove_msg.y - recorded_XorY >= 0)
					{
						sci = SliderCtrlInfo::SubOne;
					}
					else
					{
						sci = SliderCtrlInfo::AddOne;
					}
					recorded_XorY = mousemove_msg.y;
				}
			}
			else
			{
				if (abs(mousemove_msg.x - recorded_XorY) >= slidingRail.get_size().x *
					(1.0f / (float)containSum))
				{
					if (mousemove_msg.x - recorded_XorY >= 0)
					{
						sci = SliderCtrlInfo::SubOne;
					}
					else
					{
						sci = SliderCtrlInfo::AddOne;
					}
					recorded_XorY = mousemove_msg.x;
				}
			}
		}
		// ¹öÂÖ
		if (is_inArea || slidingBlock.is_down)
		{
			if (gv_p->check_if_mousewheel_msg())
			{
				int unit_num = gv_p->get_mousewheel_unitNum();

				if (unit_num)
				{
					if (unit_num > 0)
					{
						sci = SliderCtrlInfo::AddOne;
					}
					else
					{
						sci = SliderCtrlInfo::SubOne;
					}
				}
				gv_p->clear_mousewheel_msg();
			}
		}
	}

	Layout::on_input();
}

void Slider::on_update()
{
	if (sci != SliderCtrlInfo::None)
	{
		if (st == ScrollType::Vertical)
		{
			if (sci == SliderCtrlInfo::AddOne)
			{
				int temp_y = slidingBlock.get_rel_coordinates().y - slidingRail.get_size().y *
					(1.0f / containSum);
				if (temp_y >= 0)
				{
					slidingBlock.set_coordinates(3, temp_y);
				}
				else
				{
					slidingBlock.set_coordinates(3, 0);
				}
			}
			else if (sci == SliderCtrlInfo::SubOne)
			{
				int temp_y = slidingBlock.get_rel_coordinates().y + slidingRail.get_size().y *
					(1.0f / containSum);
				if (temp_y + slidingBlock.get_size().y <= slidingRail.get_size().y)
				{
					slidingBlock.set_coordinates(3, temp_y);
				}
				else
				{
					slidingBlock.set_coordinates(3, slidingRail.get_size().y -
						slidingBlock.get_size().y);
				}
			}
		}
		else
		{
			if (sci == SliderCtrlInfo::AddOne)
			{
				int temp_x = slidingBlock.get_rel_coordinates().x - slidingRail.get_size().x *
					(1.0f / containSum);
				if (temp_x >= 0)
				{
					slidingBlock.set_coordinates(temp_x, 3);
				}
				else
				{
					slidingBlock.set_coordinates(0, 3);
				}
			}
			else if (sci == SliderCtrlInfo::SubOne)
			{
				int temp_x = slidingBlock.get_rel_coordinates().x + slidingRail.get_size().x *
					(1.0f / containSum);
				if (temp_x + slidingBlock.get_size().x <= slidingRail.get_size().x)
				{
					slidingBlock.set_coordinates(temp_x, 3);
				}
				else
				{
					slidingBlock.set_coordinates(slidingRail.get_size().x - slidingBlock.get_size().x,
						3);
				}
			}
		}
	}
	reset_SCI();
}

void Slider::SlidingBlock::on_draw()
{
	if (is_touched || is_down)
	{
		setfillcolor(slidingBlockTouchedColor);
	}
	else
	{
		setfillcolor(slidingBlockColor);
	}
	const Vector2& abs_coordinates = get_abs_coordinates();
	solidrectangle(abs_coordinates.x, abs_coordinates.y, abs_coordinates.x + get_size().x,
		abs_coordinates.y + get_size().y, false);
}
