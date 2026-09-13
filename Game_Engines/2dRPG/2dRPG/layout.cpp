#include "layout.h"

void Layout::on_input()
{
	if (is_covering)
	{
		if (check_if_mouseclick_in_self())
			gv_p->clear_mouseclick_msg();
	}
	
}

void Layout::on_draw()
{
	const Vector2& abs_coordinates = get_abs_coordinates();
	switch (bk_t)
	{
	case BkType::Transparent:
		break;
	case BkType::PureColor:
		setfillcolor(color);
		solidrectangle(abs_coordinates.x, abs_coordinates.y, abs_coordinates.x + get_size().x,
			abs_coordinates.y + get_size().y, false);
		break;
	case BkType::Image:
		if (img_p)
			putimage_alpha(abs_coordinates.x, abs_coordinates.y, img_p, false);
		break;
	case BkType::Animation:
		if (animation_p)
		{
			animation_p->set_using_camera(false);			// 子挂载Animation类的set_using_camera需独立设置
			animation_p->process_drawing();
		}
		break;
	default:
		break;
	}

}

void Layout::add_elem(Component* com_p)
{
	if (!com_p)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->warn("You tried adding a layout_elem but NULLPTR,it failed to be added.");
		return;
	}

	com_p->set_parent_component(this);

}

void Layout::update_layout()
{
	// 本体内容
	size_t num = get_elem_num();
	if (!num) return;
	int w = get_size().x;
	int h = get_size().y;
	int i = 0;
	int new_w;
	int new_c_x = 0;
	int new_h;
	int new_c_y = 0;
	int unit_w;
	int unit_h;
	int total_wt;
	switch (layout_t)
	{
	case LayoutType::Absolute:
		break;
	case LayoutType::LinearHorizontal:
		total_wt = get_total_weight();
		iterator_begin();
		while (!check_if_end())
		{
			auto it = iterator_next();
			new_w = roundf(w * ((float)(*it)->get_weight() / total_wt));
			(*it)->set_coordinates(new_c_x, 0);
			(*it)->set_size(new_w, h);
			new_c_x += new_w;
		}
		break;
	case LayoutType::LinearVertical:
		total_wt = get_total_weight();
		iterator_begin();
		while (!check_if_end())
		{
			auto it = iterator_next();
			new_h = roundf(h * ((float)((*it)->get_weight()) / total_wt));
			(*it)->set_coordinates(0, new_c_y);
			(*it)->set_size(w, new_h);
			new_c_y += new_h;
		}
		break;
	case LayoutType::Grid:
		unit_w = w * (1.0f / (float)tuple.x);
		unit_h = h * (1.0f / (float)tuple.y);
		iterator_begin();
		while (i < tuple.x * tuple.y && !check_if_end())
		{
			auto it = iterator_next();
			(*it)->set_coordinates((i) % (int)tuple.x * unit_w, (i) / (int)tuple.x * unit_h);
			(*it)->set_size(unit_w, unit_h);
			++i;
			std::cout << "*it = " << *it <<
				"coordinates: (" << (*it)->get_rel_coordinates().x <<
				", " << (*it)->get_rel_coordinates().y << ") , size: (" <<
				(*it)->get_size().x << ", " << (*it)->get_size().y << ")" << std::endl;
		}
		iterator_end();
		break;
	case LayoutType::Flow:
		break;
	case LayoutType::Border:
		break;
	default:
		break;
	}
	// 递归调用子布局器的update_layout方法
	iterator_begin();
	while (!check_if_end())
	{
		auto it = iterator_next();
		Layout* cast = dynamic_cast<Layout*>(*it);
		if (cast) cast->update_layout();
	}
}

int Layout::get_total_weight()
{
	int sum = 0;
	iterator_begin();
	while (!check_if_end())
	{
		auto it = iterator_next();
		sum += (*it)->get_weight();
	}
	if (!sum) return 1;
	return sum;
}
