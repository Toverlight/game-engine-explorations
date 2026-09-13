#include "object.h"
#include "tile_map.h"
#include "var_g.h"

extern GVar* gv_p;
extern Log* log_p;

Object::Object()
{
	object_list.reserve(6);
	component_list.reserve(6);

	parent_p = nullptr;

	coordinates = { 0,0 };
	last_coordinates = { 0,0 };
	orientation = { 1,0 };
	idle_orientation = { 1,0 };
	scale = { 1,1 };
	velocity = { 0,0 };
	size = { 0,0 };
	force = { 0,0 };

	mass = 1.0f;
	radius = 0;
	e = 1.0f;
	mju = 0.1f;

	is_valid = true;
	is_visible = false;
	is_effective = false;
	is_updating = true;
	is_orientation_depending = false;
	is_coordinates_depending = false;
	is_on_objects = false;

	has_radius = false;
	has_physics = true;
	has_obj_collision_resolution = true;
	has_tile_collision_resolution = true;
	has_camera_scaling = false;

	cur_collision_normal = { 0,0 };
	cur_collision_normal_object = { 0,0 };
	cur_collision_t = CollisionType::Through;
}

void Object::process_inputing()
{
	if (!component_list.empty()) 
	{
		for (auto com_it = component_list.rbegin(); com_it != component_list.rend(); ++com_it)
			(*com_it)->process_inputing();
	}
	if (!object_list.empty()) 
	{
		for (auto obj_it = object_list.rbegin(); obj_it != object_list.rend(); ++obj_it)
			(*obj_it)->process_inputing();
	}

	if (check_effective()) on_input();
}

void Object::process_updating()
{
	if (check_updating()) on_update();

	if (!object_list.empty()) 
	{
		for (Object* obj_p : object_list)
			obj_p->process_updating();
	}
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->process_updating();
	}
}

void Object::process_drawing()
{
	if (check_visible()) on_draw();

	if (!object_list.empty())
	{
		for (Object* obj_p : object_list)
			obj_p->process_drawing();
	}
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->process_drawing();
	}
}

void Object::process_deleting()
{
	if (!object_list.empty())
	{
		for (Object* object_p : object_list)
			object_p->process_deleting();
	}
	if (!component_list.empty())
	{
		for (Component* component_p : component_list)
			component_p->process_deleting();
	}

	self_delete();
}

void Object::batch_invalid_clear()
{
	if (is_valid)
	{
		if (!object_list.empty())
		{
			for (Object* object_p : object_list)
				object_p->batch_invalid_clear();
		}
		if (!component_list.empty())
		{
			for (Component* component_p : component_list)
				component_p->batch_invalid_clear();
		}

		return;
	}
	process_deleting();
}

void Object::set_parent(Object* par_p)
{
	if (parent_p)
	{
		parent_p->deregister_object(this);
	}
	if (!(parent_p = par_p)) return;
	parent_p->register_object(this);
}

Vector2 Object::get_abs_coordinates() const
{
	if (!parent_p) return coordinates;
	return parent_p->get_abs_coordinates() + coordinates;
}

Vector2 Object::get_abs_last_coordinates() const
{
	if (!parent_p) return last_coordinates;
	return parent_p->get_abs_last_coordinates() + last_coordinates;
}

void Object::move()
{
	if (has_physics)
	{
		Vector2 acceleration = force * (1.0f / mass);		// a = F / m
		velocity += acceleration * gv_p->get_delta();
		force = { 0,0 };									// F 清零，为了下一次从零叠加
	}
	last_coordinates = coordinates;
	if (velocity.x || velocity.y) {
		coordinates += velocity * gv_p->get_delta();
	}
}

void Object::set_orientation(const Vector2& orientation)
{
	const Vector2& new_orientation = orientation.normalize();
	if (!object_list.empty())
	{
		float alpha = this->orientation.angle_between(new_orientation);
		const Vector2& temp_orientation = this->orientation.rotate(90);

		float delta_alpha = (temp_orientation * new_orientation >= 0 ? alpha : -alpha);
		for (Object* obj_p : object_list)
		{
			if (obj_p->check_orientation_depending())
				obj_p->rotate(delta_alpha);

			if (obj_p->check_coordinates_depending())
				obj_p->set_coordinates(obj_p->get_rel_coordinates().rotate(delta_alpha));

		}
	}
	this->orientation = new_orientation;
}

// 旋转以角度

void Object::rotate(float angle)
{
	this->orientation = orientation.rotate(angle);
	if (!object_list.empty()){
		for (auto& obj_p : object_list)
		{
			if (obj_p->check_orientation_depending())
				obj_p->rotate(angle);

			if (obj_p->check_coordinates_depending())
				obj_p->set_coordinates(obj_p->get_rel_coordinates().rotate(-angle));

		}
	}
}

void Object::batch_set_visible(bool flag)
{
	set_visible(flag);
	if (!object_list.empty())
	{
		for (Object* obj_p : object_list)
			obj_p->batch_set_visible(flag);
	}
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->batch_set_visible(flag);
	}
}

void Object::batch_set_effective(bool flag)
{
	set_effective(flag);
	if (!object_list.empty())
	{
		for (Object* obj_p : object_list)
			obj_p->batch_set_effective(flag);
	}
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->batch_set_effective(flag);
	}
}

void Object::batch_set_updating(bool flag)
{
	set_updating(flag);
	if (!object_list.empty())
	{
		for (Object* obj_p : object_list)
			obj_p->batch_set_updating(flag);
	}
	if (!component_list.empty())
	{
		for (Component* com_p : component_list)
			com_p->batch_set_updating(flag);
	}
}

void Object::batch_on()
{
	batch_set_visible(true);
	batch_set_effective(true);
	batch_set_updating(true);
}

void Object::batch_off()
{
	batch_set_visible(false);
	batch_set_effective(false);
	batch_set_updating(false);
}

bool Object::check_collision(Object* other) const
{
	// 圆形与圆形的碰撞检测
	if (has_radius && other->has_radius)
	{
		return (coordinates - other->coordinates).length() <= (radius + other->radius);
	}
	// AABB 碰撞检测
	else if (!has_radius && !other->has_radius)
	{
		return !(coordinates.x + size.x < other->coordinates.x ||
			coordinates.x > other->coordinates.x + other->size.x ||
			coordinates.y + size.y < other->coordinates.y ||
			coordinates.y > other->coordinates.y + other->size.y);
	}
	// 圆形与矩形或矩形与圆形的碰撞检测
	else
	{
		if (has_radius && !other->has_radius)
		{
			const Vector2& circle_center = coordinates;
			const Vector2& rect_min = other->coordinates; // 矩形左上角
			const Vector2& rect_max = rect_min + other->size;  // 矩形右下角

			// 找出矩形中离圆心最近的点
			float nearest_x = max(rect_min.x, min(circle_center.x, rect_max.x));
			float nearest_y = max(rect_min.y, min(circle_center.y, rect_max.y));
			Vector2 nearest_point(nearest_x, nearest_y);

			// 计算圆心和最近点的距离
			float dis = distance(nearest_point, circle_center);
			
			// 如果距离小于圆的半径，说明碰撞发生
			return dis * dis <= (radius * radius);
		}
		else
		{
			const Vector2& circle_center = other->coordinates;
			const Vector2& rect_min = coordinates;			// 矩形左上角
			const Vector2& rect_max = rect_min + size;					// 矩形右下角

			// 找出矩形中离圆心最近的点
			float nearest_x = max(rect_min.x, min(circle_center.x, rect_max.x));
			float nearest_y = max(rect_min.y, min(circle_center.y, rect_max.y));
			Vector2 nearest_point(nearest_x, nearest_y);

			// 计算圆心和最近点的距离
			float dis = distance(nearest_point, circle_center);

			// 如果距离小于圆的半径，说明碰撞发生
			return dis * dis <= (radius * radius);
		}
	}
}

void Object::resolve_collision(Object* other)
{
	if (!(has_physics && other->has_physics)) return;
	if (!(has_obj_collision_resolution && other->has_obj_collision_resolution)) return;
	// 圆形与圆形
	if (has_radius && other->has_radius && ((coordinates - other->coordinates).length() <= radius + other->radius))
	{

		float dis = distance(coordinates, other->coordinates);
		float overlap = radius + other->radius - dis;
		Vector2 collision_normal = (coordinates - other->coordinates).normalize();
		cur_collision_normal_object = collision_normal;
		other->cur_collision_normal_object = cur_collision_normal_object * -1;
		Vector2 tangential_normal = collision_normal.rotate(90);

		// 重叠处理
		if (overlap > 1)
		{
			coordinates += collision_normal * (overlap * MINIMIZE_RATE);
			other->coordinates += collision_normal * (overlap * MINIMIZE_RATE * -1);
		}
		else
		{
			coordinates += collision_normal * (overlap);
			other->coordinates += collision_normal * (overlap * -1);
		}
		// 两圆靠近时，处理碰撞速度
		if ((velocity - other->velocity) * collision_normal < 0)
		{
			// 法向速度
			float v1n = velocity * collision_normal;
			float v2n = other->velocity * collision_normal;
			float new_e = sqrt(e * other->e);									// 几何平均数作为新的恢复系数
			float v1n_ = ((mass - new_e * other->mass) * v1n + (1.0f + new_e) * other->mass * v2n) /
				(mass + other->mass);
			v1n_ = (abs(v1n_) < 0.00064f ? 0 : v1n_);
			float v2n_ = ((other->mass - new_e * mass) * v2n + (1.0f + new_e) * mass * v1n) /
				(mass + other->mass);
			v2n_ = (abs(v2n_) < 0.00064f ? 0 : v2n_);
			Vector2 vec_v1n_ = collision_normal * v1n_;
			Vector2 vec_v2n_ = collision_normal * v2n_;
			// 切向速度
			float v1t = velocity * tangential_normal;
			float v2t = other->velocity * tangential_normal;
			float new_mju = sqrt(mju * other->mju);								// 几何平均数作为新的粗糙程度
			float v1t_ = v1t * (1.0f - new_mju); 
			v1t_ = (abs(v1t_) < 0.00064f ? 0 : v1t_);
			float v2t_ = v2t * (1.0f - new_mju);
			v2t_ = (abs(v2t_) < 0.00064f ? 0 : v2t_);
			Vector2 vec_v1t_ = tangential_normal * v1t_;
			Vector2 vec_v2t_ = tangential_normal * v2t_;
			// 合成速度
			velocity = vec_v1n_ + vec_v1t_;
			other->velocity = vec_v2n_ + vec_v2t_;
		}
	}
	// 矩形与矩形
	else if (!has_radius && !other->has_radius && 
		!(coordinates.x + size.x < other->coordinates.x ||
		coordinates.x > other->coordinates.x + other->size.x ||
		coordinates.y + size.y < other->coordinates.y ||
		coordinates.y > other->coordinates.y + other->size.y))
	{
		float overlap_x = min(coordinates.x + size.x, other->coordinates.x + other->size.x) - 
			max(coordinates.x, other->coordinates.x);
		float overlap_y = min(coordinates.y + size.y, other->coordinates.y + other->size.y) -
			max(coordinates.y, other->coordinates.y);

		Vector2 collision_normal;
		Vector2 tangential_normal;
		// 根据最小重叠方向调整位置
		if (overlap_x < overlap_y)										// 水平调整
		{
			// 向左调整
			if (coordinates.x + size.x / 2 < other->coordinates.x + other->size.x / 2)
			{
				if (overlap_x > 1)
				{
					coordinates.x -= overlap_x * (MINIMIZE_RATE);
					other->coordinates.x += overlap_x * (MINIMIZE_RATE);
				}
				else
				{
					coordinates.x -= overlap_x;
					other->coordinates.x += overlap_x;
				}
				collision_normal = { 1,0 };
				tangential_normal = { 0,-1 };
			}
			// 向右调整
			else
			{
				if (overlap_x > 1)
				{
					coordinates.x += overlap_x * (MINIMIZE_RATE);
					other->coordinates.x -= overlap_x * (MINIMIZE_RATE);
				}
				else
				{
					coordinates.x += overlap_x;
					other->coordinates.x -= overlap_x;
				}
				collision_normal = { -1,0 };
				tangential_normal = { 0,1 };
			}
		}
		else															// 竖直调整
		{
			// 向上调整
			if (coordinates.y + size.y / 2 < other->coordinates.y + other->size.y / 2)
			{
				if (overlap_y > 1)
				{
					coordinates.y -= overlap_y * (MINIMIZE_RATE);
					other->coordinates.y += overlap_y * (MINIMIZE_RATE);
				}
				else
				{
					coordinates.y -= overlap_y;
					other->coordinates.y += overlap_y;
				}
				collision_normal = { 0,1 };
				tangential_normal = { 1,0 };
			}
			// 向下调整
			else
			{
				if (overlap_y > 1)
				{
					coordinates.y += overlap_y * (MINIMIZE_RATE);
					other->coordinates.y -= overlap_y * (MINIMIZE_RATE);
				}
				else
				{
					coordinates.y += overlap_y;
					other->coordinates.y -= overlap_y;
				}
				collision_normal = { 0,-1 };
				tangential_normal = { -1,0 };
			}
		}

		cur_collision_normal_object = collision_normal;
		other->cur_collision_normal_object = cur_collision_normal_object * -1;

		float new_mju = sqrt(mju * other->mju);								// 几何平均数作为新的粗糙程度
		// 两矩形靠近时，处理碰撞速度
		if ((velocity - other->velocity) * collision_normal > 0)
		{
			// 法向速度
			float v1n = velocity * collision_normal;
			float v2n = other->velocity * collision_normal;
			float new_e = sqrt(e * other->e);									// 几何平均数作为新的恢复系数
			float v1n_ = ((mass - new_e * other->mass) * v1n + (1.0f + new_e) * other->mass * v2n) /
				(mass + other->mass);
			v1n_ = (abs(v1n_) < 0.00064f ? 0 : v1n_);
			float v2n_ = ((other->mass - new_e * mass) * v2n + (1.0f + new_e) * mass * v1n) /
				(mass + other->mass);
			v2n_ = (abs(v2n_) < 0.00064f ? 0 : v2n_);
			Vector2 vec_v1n_ = collision_normal * v1n_;
			Vector2 vec_v2n_ = collision_normal * v2n_;
			// 切向速度
			float v1t = velocity * tangential_normal;
			float v2t = other->velocity * tangential_normal;
			float v1t_ = v1t * (1.0f - new_mju);
			v1t_ = (abs(v1t_) < 0.00064f ? 0 : v1t_);
			float v2t_ = v2t * (1.0f - new_mju);
			v2t_ = (abs(v2t_) < 0.00064f ? 0 : v2t_);
			Vector2 vec_v1t_ = tangential_normal * v1t_;
			Vector2 vec_v2t_ = tangential_normal * v2t_;
			// 合成速度
			velocity = vec_v1n_ + vec_v1t_;
			other->velocity = vec_v2n_ + vec_v2t_;
		}	
		// 竖直方向上相对静止时
		else if (velocity.y == other->velocity.y)
		{
			// 水平相对运动
			if (velocity.x != other->velocity.x)
			{
				// 施加水平摩擦力
				apply_force(((velocity.x - other->velocity.x) > 0 ? 1 : -1) * -new_mju * mass * 
					TileMap::gravity.y, 0);
				other->apply_force(((velocity.x - other->velocity.x) > 0 ? 1 : -1) * new_mju * mass *
					TileMap::gravity.y, 0);
			}
			// 支持力模拟
			if (force.y > 0)
			{
				force.y = 0;
			}
		}
		// 水平方向上相对静止时
		else if (velocity.x == other->velocity.x)
		{
			// 竖直相对运动
			if (velocity.y != other->velocity.y)
			{
				// 施加竖直摩擦力
				apply_force(0, ((velocity.y - other->velocity.y) > 0 ? 1 : -1) * (-new_mju * 0.1f) * mass*
					TileMap::gravity.y);
				other->apply_force(0, ((velocity.y - other->velocity.y) > 0 ? 1 : -1) * (new_mju * 0.1f) * mass *
					TileMap::gravity.y);
			}
		}
	}
	// 圆形与矩形 或 矩形与圆形
	else if ((has_radius && !other->has_radius) || (!has_radius && other->has_radius))
	{
		Vector2 circle_center;
		Vector2 rect_min;
		Vector2 rect_max;
		float c = 1.0f;
		if (has_radius && !other->has_radius)
		{
			circle_center = coordinates;
			rect_min = other->coordinates;			// 矩形左上角
			rect_max = rect_min + other->size;		// 矩形右下角
		}
		else
		{
			circle_center = other->coordinates;
			rect_min = coordinates;					// 矩形左上角
			rect_max = rect_min + size;				// 矩形右下角
			c = -c;
		}
		// 找出矩形中离圆心最近的点
		float nearest_x = max(rect_min.x, min(circle_center.x, rect_max.x));
		float nearest_y = max(rect_min.y, min(circle_center.y, rect_max.y));
		Vector2 nearest_point(nearest_x, nearest_y);

		// 计算圆心和最近点的距离
		float dis = distance(nearest_point, circle_center);

		if (dis <= radius)
		{
			// 碰撞发生：计算重叠部分并进行位置调整
			float overlap = radius - dis;
			Vector2 collision_normal = (circle_center - nearest_point).normalize();	// 归一化，获得碰撞方向

			cur_collision_normal_object = collision_normal * c;
			other->cur_collision_normal_object = cur_collision_normal_object * c;

			Vector2 tangential_normal = collision_normal.rotate(90);			// 逆时针旋转90度归一化，获得切向方向
			// 沿碰撞方向微调坐标，最小化重叠面积
			if (overlap > 1)
			{
				coordinates += collision_normal * (overlap * MINIMIZE_RATE * c);
				other->coordinates += collision_normal * (overlap * MINIMIZE_RATE * -c);
			}
			else
			{
				coordinates += collision_normal * (overlap * c);
				other->coordinates += collision_normal * (overlap * -c);
			}

			float new_mju = sqrt(mju * other->mju);								// 几何平均数作为新的粗糙程度
			// 两者靠近时，处理碰撞速度
			if ((velocity - other->velocity) * collision_normal * c < 0)
			{
				// 法向速度
				float v1n = velocity * collision_normal;
				float v2n = other->velocity * collision_normal;
				float new_e = sqrt(e * other->e);								// 几何平均数作为新的恢复系数
				float v1n_ = ((mass - new_e * other->mass) * v1n + (1.0f + new_e) * other->mass * v2n) /
					(mass + other->mass);
				v1n_ = (abs(v1n_) < 0.00064f ? 0 : v1n_);
				float v2n_ = ((other->mass - new_e * mass) * v2n + (1.0f + new_e) * mass * v1n) /
					(mass + other->mass);
				v2n_ = (abs(v2n_) < 0.00064f ? 0 : v2n_);
				Vector2 vec_v1n_ = collision_normal * v1n_;
				Vector2 vec_v2n_ = collision_normal * v2n_;
				// 切向速度
				float v1t = velocity * tangential_normal;
				float v2t = other->velocity * tangential_normal;
				float v1t_ = v1t * (1.0f - new_mju);
				v1t_ = (abs(v1t_) < 0.00064f ? 0 : v1t_);
				float v2t_ = v2t * (1.0f - new_mju);
				v2t_ = (abs(v2t_) < 0.00064f ? 0 : v2t_);
				Vector2 vec_v1t_ = tangential_normal * v1t_;
				Vector2 vec_v2t_ = tangential_normal * v2t_;
				// 合成速度
				velocity = vec_v1n_ + vec_v1t_;
				other->velocity = vec_v2n_ + vec_v2t_;
			}
			// 竖直方向上相对静止时
			else if (velocity.y == other->velocity.y)
			{
				// 水平相对运动
				if (velocity.x != other->velocity.x)
				{
					// 施加水平摩擦力
					apply_force(((velocity.x - other->velocity.x) > 0 ? 1 : -1) * -new_mju * mass *
						TileMap::gravity.y, 0);
					other->apply_force(((velocity.x - other->velocity.x) > 0 ? 1 : -1) * new_mju * mass *
						TileMap::gravity.y, 0);
				}
				// 支持力模拟
				if (force.y > 0)
				{
					force.y = 0;
				}
			}
			// 水平方向上相对静止时
			else if (velocity.x == other->velocity.x)
			{
				// 竖直相对运动
				if (velocity.y != other->velocity.y)
				{
					// 施加竖直摩擦力
					apply_force(0, ((velocity.y - other->velocity.y) > 0 ? 1 : -1) * (-new_mju * 0.1f) * mass *
						TileMap::gravity.y);
					other->apply_force(0, ((velocity.y - other->velocity.y) > 0 ? 1 : -1) * (new_mju * 0.1f) * 
						mass * TileMap::gravity.y);
				}
			}
		}
	}
	if (has_radius && cur_collision_normal_object.y < 0)
		is_on_objects = true;
	else if (!has_radius && cur_collision_normal_object.y > 0)
		is_on_objects = true;
	if (other->has_radius && cur_collision_normal_object.y < 0)
		other->is_on_objects = true;
	else if (!other->has_radius && cur_collision_normal_object.y > 0)
		other->is_on_objects = true;
}

void Object::set_cur_collision_t(CollisionType c_t) { cur_collision_t = c_t; }

CollisionType Object::get_cur_collision_t() const { return cur_collision_t; }

void Object::deregister_object(Object* object_p)
{
	for (auto it = object_list.begin(); it != object_list.end(); ++it)
		if ((*it) == object_p)
		{
			object_list.erase(it);
			if (gv_p->check_debug())
			{
				log_p->setTarget(LogTarget::Console);
				log_p->debug("Object::", __func__, ": Object deregistered.");
			}
			break;
		}
}

void Object::deregister_component(Component* component_p)
{
	for (auto it = component_list.begin(); it != component_list.end(); ++it)
		if ((*it) == component_p)
		{
			component_list.erase(it);
			if (gv_p->check_debug())
			{
				log_p->setTarget(LogTarget::Console);
				log_p->debug("Object::", __func__, ": Component deregistered.");
			}
			break;
		}
}

void Object::self_delete()
{
	if (gv_p->check_debug())
	{
		log_p->setTarget(LogTarget::Console);
		log_p->debug("Object::", __func__, ": Object deleted.");
	}
	if (parent_p)
	{
		parent_p->deregister_object(this);
	}

	delete this;
}
