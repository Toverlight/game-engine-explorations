#include "var_g.h"
#include "object.h"
#include "camera.h"
#include "layout.h"
#include "tile_map.h"

GVar::GVar()
{
	msg_buffer = { 0 };
	key_msg = { 0 };
	key_msg_list.reserve(20);
	mouse_msg_move = { 0 };
	mouse_msg_click = { 0 };
	mouse_msg_wheel = { 0 };
	delta = 0;
	camera_p = nullptr;
	tile_map_p = nullptr;
	scene_manager_p = new SceneManager();

	object_list.reserve(30);
	component_list.reserve(60);

	is_debug = false;
	kp = KeyPlan::Bool;
	head = 0;

	focus_p = nullptr;
}

GVar::~GVar()
{
	delete scene_manager_p;
}

GVar* GVar::get_instance()
{
	static GVar g_var;
	log_p->setTarget(LogTarget::Console);
	log_p->info("g_var created.");
	return &g_var;
}

void GVar::batch_input()
{
	if (key_msg.message == WM_KEYUP)
	{
		if (key_msg.vkcode == 'Q')				// Q 键改变调试状态
		{
			if (is_debug = !is_debug)
			{
				log_p->setTarget(LogTarget::Console);
				log_p->debug("debug mode ON.");
			}
			else
			{
				log_p->setTarget(LogTarget::Console);
				log_p->debug("debug mode OFF.");
			}
			clear_key_msg();
		}
	}
	// 无聚焦，监测全局
	if (!focus_p)
	{
		// 检测采用反向遍历
		for (auto com_it = component_list.rbegin(); com_it != component_list.rend(); ++com_it)
		{
			(*com_it)->process_inputing();
		}
		for (auto obj_it = object_list.rbegin(); obj_it != object_list.rend(); ++obj_it)
		{
			(*obj_it)->process_inputing();
		}

	}
	// 有聚焦，监测指定面板
	else
	{
		focus_p->process_inputing();
	}
}

void GVar::batch_update()
{
	if (camera_p) camera_p->on_update();
	for (Object* obj_p : object_list)
		obj_p->process_updating();
	for (Component* com_p : component_list)
		com_p->process_updating();
}

void GVar::batch_draw()
{
	for (Object* obj_p : object_list)
		obj_p->process_drawing();
	for (Component* com_p : component_list)
		com_p->process_drawing();
}

void GVar::batch_draw_object()
{
	for (Object* obj_p : object_list)
		obj_p->process_drawing();
}

void GVar::batch_draw_component()
{
	for (Component* com_p : component_list)
		com_p->process_drawing();
}

void GVar::tile_draw()
{
	if (tile_map_p)
		tile_map_p->on_draw();
}

void GVar::tile_draw_water()
{
	if (tile_map_p)
		tile_map_p->on_draw_water();
}

void GVar::batch_invalid_clear()
{
	for (Object* obj_p : object_list)
	{
		if (!obj_p->check_valid())
			deregister_object(obj_p);
		obj_p->batch_invalid_clear();
	}
	for (Component* com_p : component_list)
	{
		if (!com_p->check_valid())
			deregister_component(com_p);
		com_p->batch_invalid_clear();
	}
}

void GVar::batch_model()
{
	// 物体 is_on_objects 重置
	for (auto it = object_list.begin(); it != object_list.end(); ++it)
	{
		if ((*it)->if_has_physics() && (*it)->check_updating())
		{
			(*it)->set_on_objects(false);
		}
	}
	// 施加重力 与 物体之间的碰撞处理
	for (auto it = object_list.begin(); it != object_list.end(); ++it)
	{
		if ((*it)->if_has_physics() && (*it)->check_updating())
		{
			// 施加重力
			(*it)->apply_force(TileMap::gravity * (*it)->get_mass());
			// 物体之间的碰撞处理
			for (auto it_other = it + 1; it_other != object_list.end(); ++it_other)
			{
				if ((*it_other)->if_has_physics() && (*it_other)->check_updating())
				{
					(*it)->resolve_collision(*it_other);
				}
			}
		}
	}
	// 瓦片图块与物体间的碰撞处理
	for (auto it = object_list.begin(); it != object_list.end(); ++it)
	{
		if ((*it)->if_has_physics() && (*it)->check_updating()){
			gv_p->get_tile_map()->resolve_collision(*it);
		}
	}
	// 物体的移动
	for (auto it = object_list.begin(); it != object_list.end(); ++it)
	{
		if ((*it)->check_updating())
		{
			// 移动之前，限制最大速度
			const Vector2& velocity = (*it)->get_velocity();
			if (abs(velocity.x) > 1.0f) (*it)->set_velocity((velocity.x > 0 ? 1 : -1) * 1.0f, velocity.y);
			if (abs(velocity.y) > 1.0f) (*it)->set_velocity(velocity.x, (velocity.y > 0 ? 1 : -1) * 1.0f);
			// 移动
			(*it)->move();
		}
	}
}

void GVar::upper_one_object(Object* obj_p)
{
	auto it = object_list.begin();
	for (; it != object_list.end() - 1; ++it)
		if (obj_p == *it) break;
	if (it == object_list.end() - 1) return;
	Object* temp = *it;
	*it = *(it + 1);
	*(it + 1) = temp;
}

void GVar::upper_one_component(Component* com_p)
{
	auto it = component_list.begin();
	for (; it != component_list.end() - 1; ++it)
		if (com_p == *it) break;
	if (it == component_list.end() - 1) return;
	Component* temp = *it;
	*it = *(it + 1);
	*(it + 1) = temp;
}

void GVar::lower_one_object(Object* obj_p)
{
	auto it = object_list.rbegin();
	for (; it != object_list.rend() - 1; ++it)
		if (obj_p == *it) break;
	if (it == object_list.rend() - 1) return;
	Object* temp = *it;
	*it = *(it + 1);
	*(it + 1) = temp;
}

void GVar::lower_one_component(Component* com_p)
{
	auto it = component_list.rbegin();
	for (; it != component_list.rend() - 1; ++it)
		if (com_p == *it) break;
	if (it == component_list.rend() - 1) return;
	Component* temp = *it;
	*it = *(it + 1);
	*(it + 1) = temp;
}

void GVar::upper_top_object(Object* obj_p)
{
	auto it = object_list.begin();
	for (; it != object_list.end() - 1; ++it)
		if (obj_p == *it) break;
	if (it == object_list.end() - 1) return;
	Object* temp = *it;
	for (; it != object_list.end() - 1; ++it)
		*it = *(it + 1);
	*it = temp;
}

void GVar::upper_top_component(Component* com_p)
{
	auto it = component_list.begin();
	for (; it != component_list.end() - 1; ++it)
		if (com_p == *it) break;
	if (it == component_list.end() - 1) return;
	Component* temp = *it;
	for (; it != component_list.end() - 1; ++it)
		*it = *(it + 1);
	*it = temp;
}

void GVar::lower_bottom_object(Object* obj_p)
{
	auto it = object_list.rbegin();
	for (; it != object_list.rend() - 1; ++it)
		if (obj_p == *it) break;
	if (it == object_list.rend() - 1) return;
	Object* temp = *it;
	for (; it != object_list.rend() - 1; ++it)
		*it = *(it + 1);
	*it = temp;
}

void GVar::lower_bottom_component(Component* com_p)
{
	auto it = component_list.rbegin();
	for (; it != component_list.rend() - 1; ++it)
		if (com_p == *it) break;
	if (it == component_list.rend() - 1) return;
	Component* temp = *it;
	for (; it != component_list.rend() - 1; ++it)
		*it = *(it + 1);
	*it = temp;
}


