#include "component.h"
#include "var_g.h"

extern GVar* gv_p;
extern Log* log_p;

void Component::process_inputing()
{
	if (!component_list.empty())
	{
		for (auto it = component_list.rbegin(); it != component_list.rend(); ++it)
			(*it)->process_inputing();
	}

	if (check_effective()) on_input();
}

void Component::process_updating()
{
	if (check_updating()) on_update();

	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->process_updating();
	}
}

void Component::process_drawing()
{
	if (check_visible()) on_draw();

	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->process_drawing();
	}
}

void Component::process_deleting()
{
	if (!component_list.empty())
	{
		for (Component* component_p : component_list)
			component_p->process_deleting();
	}

	self_delete();
}

void Component::batch_invalid_clear()
{
	if (is_valid)
	{
		if (!component_list.empty())
		{
			for (Component* component_p : component_list)
				component_p->batch_invalid_clear();
		}

		return;
	}
	process_deleting();
}

// 设置父对象为

void Component::set_parent_object(Object* par_p)
{
	if (parent_component_p)
	{
		parent_component_p->deregister_component(this);
		parent_component_p = nullptr;
	}
	if (parent_object_p)
	{
		parent_object_p->deregister_component(this);
	}
	if (!(parent_object_p = par_p)) return;
	parent_object_p->register_component(this);
}


// 设置父组件为

void Component::set_parent_component(Component* par_p)
{
	if (parent_object_p)
	{
		parent_object_p->deregister_component(this);
		parent_object_p = nullptr;
	}
	if (parent_component_p)
	{
		parent_component_p->deregister_component(this);
	}
	if (!(parent_component_p = par_p)) return;
	parent_component_p->register_component(this);
}

Vector2 Component::get_abs_coordinates() const
{
	if (parent_object_p) return parent_object_p->get_abs_coordinates() + coordinates;
	if (parent_component_p) return parent_component_p->get_abs_coordinates() + coordinates;

	return coordinates;
}

// 获取父挂载的尺寸。若无父挂载，则返回零向量

Vector2 Component::get_if_parent_size() const
{
	if (parent_component_p)
	{
		return parent_component_p->get_size();
	}
	else if (parent_object_p)
	{
		return parent_object_p->get_size();
	}
	return { 0,0 };
}

void Component::batch_set_visible(bool flag)
{
	set_visible(flag);
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->batch_set_visible(flag);
	}
}

void Component::batch_set_effective(bool flag)
{
	set_effective(flag);
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->batch_set_effective(flag);
	}
}

void Component::batch_set_updating(bool flag)
{
	set_updating(flag);
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->batch_set_updating(flag);
	}
}

void Component::batch_on()
{
	batch_set_visible(true);
	batch_set_effective(true);
	batch_set_updating(true);
}

void Component::batch_off()
{
	batch_set_visible(false);
	batch_set_effective(false);
	batch_set_updating(false);
}

bool Component::check_if_mousemove_in_self()
{
	const ExMessage& mousemove_msg = gv_p->get_mousemove_msg();
	const Vector2& abs_coordinates = get_abs_coordinates();
	return (mousemove_msg.x >= abs_coordinates.x && mousemove_msg.x <= abs_coordinates.x + get_size().x
		&&  mousemove_msg.y >= abs_coordinates.y && mousemove_msg.y <= abs_coordinates.y + get_size().y);
}

bool Component::check_if_mouseclick_in_self()
{
	const ExMessage& mouseclick_msg = gv_p->get_mouseclick_msg();
	const Vector2& abs_coordinates = get_abs_coordinates();
	return (mouseclick_msg.x >= abs_coordinates.x && mouseclick_msg.x <= abs_coordinates.x + get_size().x
		&& mouseclick_msg.y >= abs_coordinates.y && mouseclick_msg.y <= abs_coordinates.y + get_size().y);
}

int Component::deregister_component(Component* component_p)
{
	for (auto it = component_list.begin(); it != component_list.end(); ++it)
		if ((*it) == component_p)
		{
			component_list.erase(it);
			if (gv_p->check_debug())
			{
				log_p->setTarget(LogTarget::Console);
				log_p->debug("Component::", __func__, ": Component deregistered.");
			}
			break;
			return 1;
		}
	return 0;
}

void Component::self_delete()
{
	if (gv_p->check_debug())
	{
		log_p->setTarget(LogTarget::Console);
		log_p->debug("Component::", __func__, ": Component deleted.");
	}
	if (parent_object_p)
		parent_object_p->deregister_component(this);
	if (parent_component_p)
		parent_component_p->deregister_component(this);

	delete this;
}
