#include "scroll_list.h"
#include "log.h"

ScrollList::ScrollList(ScrollType st, int elemShowNum_, int len, int width)
{
	if (elemShowNum_ < 1) elemShowNum = 1;
	else elemShowNum = elemShowNum_;
	elem_list.reserve(10);
	first_loc = -1;

	set_layout_t(LayoutType::Absolute);
	set_bk_t(BkType::PureColor);
	set_color(RGB(240, 240, 240));

	Layout::add_elem(&sfTop);
	Layout::add_elem(&midBox);
	Layout::add_elem(&sfBottom);

	pSlider = new Slider(st, len);
	pSlider->set_containNum(elemShowNum);
	container.pSlider = this->pSlider;

	midBox.add_elem(&sfLeft);
	midBox.add_elem(&container);
	midBox.add_elem(pSlider);

	container.set_bk_t(BkType::PureColor);
	container.set_color(RGB(240, 240, 240));

	if (st == ScrollType::Vertical)
	{
		set_size(width, len);
		sfTop.set_size(width, MARGIN_SF);
		sfTop.set_coordinates(0, 0);
		midBox.set_size(width, len - MARGIN_SF * 2);
		midBox.set_coordinates(0, MARGIN_SF);
		sfBottom.set_size(width, MARGIN_SF);
		sfBottom.set_coordinates(0, len - MARGIN_SF);

		sfLeft.set_size(MARGIN_SF, len - MARGIN_SF * 2);
		sfLeft.set_coordinates(0, 0);
		container.set_size(width - MARGIN_SF - pSlider->get_slidingRailWidth(), len - MARGIN_SF * 2);
		container.set_coordinates(MARGIN_SF, 0);
		pSlider->set_coordinates(width - pSlider->get_slidingRailWidth(), 0);

		container.set_layout_t(LayoutType::Grid);
		container.set_tuple(1, elemShowNum);
	}
	else
	{
		set_size(len, width);
		sfTop.set_size(MARGIN_SF, width);
		sfTop.set_coordinates(0, 0);
		midBox.set_size(len - MARGIN_SF * 2, width);
		midBox.set_coordinates(MARGIN_SF, 0);
		sfBottom.set_size(MARGIN_SF, width);
		sfBottom.set_coordinates(len - MARGIN_SF, 0);

		sfLeft.set_size(len - MARGIN_SF * 2, MARGIN_SF);
		sfLeft.set_coordinates(0, 0);
		container.set_size(len - MARGIN_SF * 2, width - MARGIN_SF - pSlider->get_slidingRailWidth());
		container.set_coordinates(0, MARGIN_SF);
		pSlider->set_coordinates(0, width - pSlider->get_slidingRailWidth());

		container.set_layout_t(LayoutType::Grid);
		container.set_tuple(elemShowNum, 1);
	}
}

ScrollList::~ScrollList()
{
	delete pSlider;
}

void ScrollList::add_elem(Component* com_p)
{
	if (!com_p)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->warn("You tried adding a layout_elem but NULLPTR,it failed to be added.");
		return;
	}

	if (elem_list.size() == 0)
		first_loc = 0;
	elem_list.push_back(com_p);

	if (container.get_elem_num() < elemShowNum)
	{
		container.add_elem(com_p);
		container.update_layout();
	}

	pSlider->set_containSum(elem_list.size());
	pSlider->update_slidingBlockSize();
}

void ScrollList::del_elem(Component* com_p)
{
	if (!com_p)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->warn("You tried deleting a layout_elem but NULLPTR,it failed to be deleted.");
		return;
	}

	if (container.get_elem_num() == 0 || elem_list.empty())
	{
		log_p->setTarget(LogTarget::Console);
		log_p->warn("No elem to be deleted!");
		return;
	}

	bool move_fl = true;
	container.iterator_begin();
	while (!container.check_if_end())
	{
		auto it = container.iterator_next();
		if ((*it) == com_p)
		{
			com_p->set_parent_component(nullptr);
			break;
		}
	}
	container.iterator_end();

	for (auto it = elem_list.begin(); it != elem_list.end(); ++it)
	{
		if ((*it) == elem_list[first_loc]) move_fl = false;
		if ((*it) == com_p)
		{
			elem_list.erase(it);
			if (move_fl) --first_loc;
			if (first_loc + elemShowNum < elem_list.size())
			{
				container.add_elem(elem_list[first_loc + elemShowNum]);
				container.update_layout();
			}
			break;
		}
	}

	pSlider->set_containSum(elem_list.size());
	pSlider->update_slidingBlockSize();
}

void ScrollList::on_update()
{
	SliderCtrlInfo sci = pSlider->get_SCI();

	if (!elem_list.empty())
	{
		if (sci != SliderCtrlInfo::None)
		{
			if (sci == SliderCtrlInfo::AddOne)
			{
				if (first_loc > 0 && first_loc + elemShowNum <= elem_list.size())
				{
					container.last_elem_pop();
					container.insert_to_first(elem_list[--first_loc]);
					std::cout << "ScrollList: Addone! first_loc = " <<
						first_loc << ", elemShowNum = " << elemShowNum <<
						", container elemNum = " << container.get_elem_num() <<
						", elem_list.size() = " << elem_list.size() << std::endl;
					container.update_layout();
					output_container_elems();
				}
			}
			else if (sci == SliderCtrlInfo::SubOne)
			{
				if (first_loc >= 0 && first_loc + elemShowNum < elem_list.size())
				{
					container.first_elem_pop();
					container.add_elem(elem_list[first_loc++ + elemShowNum]);
					std::cout << "ScrollList: Subone! first_loc = " <<
						first_loc << ", elemShowNum = " << elemShowNum <<
						", container elemNum = " << container.get_elem_num() <<
						", elem_list.size() = " << elem_list.size() << std::endl;
					container.update_layout();
					output_container_elems();
				}
			}
		}
		if (first_loc != 0 && first_loc + elemShowNum > elem_list.size())
		{
			first_loc = elem_list.size() - elemShowNum;
			if (first_loc < 0) first_loc = 0;
			for (int i = 0; i < container.get_elem_num(); ++i)
			{
				container.last_elem_pop();
			}
			for (int k = first_loc; k < elem_list.size(); ++k)
			{
				container.add_elem(elem_list[k]);
			}
			container.update_layout();
			output_container_elems();
		}
	}
}

void ScrollList::Container::on_input()
{
	const Vector2& abs_coordinates = get_abs_coordinates();
	const Vector2& size = get_size();
	const ExMessage& mousemove_msg = gv_p->get_mousemove_msg();
	if (gv_p->check_if_mousewheel_msg())
	{
		if (mousemove_msg.x >= abs_coordinates.x &&
			mousemove_msg.x <= abs_coordinates.x + size.x &&
			mousemove_msg.y >= abs_coordinates.y &&
			mousemove_msg.y <= abs_coordinates.y + size.y)
		{
			int unitNum = gv_p->get_mousewheel_unitNum();
			if (unitNum > 0)
			{
				pSlider->send_SCI(SliderCtrlInfo::AddOne);
				std::cout << "container: Addone!" << std::endl;
			}
			else
			{
				pSlider->send_SCI(SliderCtrlInfo::SubOne);
				std::cout << "container: Subone!" << std::endl;
			}
			gv_p->clear_mousewheel_msg();
		}
	}
}
