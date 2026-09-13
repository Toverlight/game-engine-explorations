#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifndef MINIMIZE_RATE
#define MINIMIZE_RATE (1.0f / 3.0f)			// 最小化重叠面积的比率
#endif // !MINIMIZE_RATE

#include <vector>
#include <iostream>

#include "vector2.h"
#include "component.h"

class Component;
enum class CollisionType;

// 对象
class Object
{
public:
	Object();

	virtual void on_input() { }

	virtual void on_update() { }

	virtual void on_draw() { }

	// 递归输入处理（子先父后）
	void process_inputing();
	// 递归更新挂载（父先子后）
	void process_updating();
	// 递归绘制挂载（父先子后）
	void process_drawing();

	// 检查合法性
	bool check_valid() { return is_valid; }				

	// 非法化
	void invalid()										
	{
		is_valid = false;
	}

	// 递归删除自己及所有挂载
	virtual void process_deleting();					

	// 批量清除非法挂载
	void batch_invalid_clear();															

	// 设置父对象为。（设定父对象后，该对象不受物理引擎支配）
	void set_parent(Object* par_p);

	virtual void set_coordinates(const Vector2& coordinates)
	{
		this->coordinates = coordinates;
	}
	virtual void set_coordinates(float x, float y)
	{
		this->coordinates.x = x;
		this->coordinates.y = y;
	}
	// 将当前坐标记录在last_coordinates
	void record_coordinates() { last_coordinates = coordinates; }

	const Vector2& get_rel_coordinates() const { return coordinates; }
	Vector2 get_abs_coordinates() const;
	const Vector2& get_rel_last_coordinates() const { return last_coordinates; }
	Vector2 get_abs_last_coordinates() const;

	Vector2 get_parent_abs_coordinates() const
	{
		if (!parent_p) return get_rel_coordinates();
		return get_abs_coordinates() - get_rel_coordinates();
	}
	// 设定速度[pix / ms]
	void set_velocity(const Vector2& velocity)
	{
		this->velocity = velocity;
	}
	// 设定速度[pix / ms]
	void set_velocity(float x, float y)
	{
		this->velocity.x = x;
		this->velocity.y = y;
	}

	void add_velocity(const Vector2& delta_velocity)
	{
		velocity += delta_velocity;
	}
	void add_velocity(float delta_x, float delta_y)
	{
		velocity.x += delta_x;
		velocity.y += delta_y;
	}

	const Vector2& get_velocity() const { return velocity; }
	// 根据力更新当前速度并根据当前速度更新物体坐标
	void move();

	void set_size(const Vector2& size)
	{
		this->size = size;
	}
	void set_size(int w, int h)
	{
		size.x = w;
		size.y = h;
	}
	const Vector2& get_size() const { return size; }
	// 获取父挂载的尺寸。若无父挂载，则返回零向量
	Vector2 get_if_parent_size() const
	{
		if (parent_p)
		{
			return parent_p->get_size();
		}
		return { 0,0 };
	}

	// 设定方向
	void set_orientation(const Vector2& orientation);
	const Vector2& get_orientation() const { return orientation; }
	// 设定默认方向
	void set_idle_orientation(const Vector2& vec)
	{
		orientation = idle_orientation = vec.normalize();
	}
	const Vector2& get_idle_orientation() const { return idle_orientation; }

	// 旋转以角度
	void rotate(float angle);
	// 设定缩放倍率（随方向）
	void set_scale(float orienting, float vertical)
	{
		scale.x = orienting;
		scale.y = vertical;
	}

	virtual void set_visible(bool flag) { is_visible = flag; }
	bool check_visible() const { return is_visible; }
	virtual void set_effective(bool flag) { is_effective = flag; }
	bool check_effective() const { return is_effective; }
	virtual void set_updating(bool flag) { is_updating = flag; }
	bool check_updating() const { return is_updating; }

	void batch_set_visible(bool flag);
	void batch_set_effective(bool flag);
	void batch_set_updating(bool flag);

	// 批量开启自己及所有挂载（visible,effective,updating）
	void batch_on();
	// 批量关闭自己及所有挂载（visible,effective,updating）
	void batch_off();

	// 设定方向对方向依赖
	void set_orientaion_depending(bool flag)
	{
		is_orientation_depending = flag;
	}
	// 设定相对坐标对方向依赖
	void set_coordinates_depending(bool flag)
	{
		is_coordinates_depending = flag;
	}
	bool check_orientation_depending() const { return is_orientation_depending; }
	bool check_coordinates_depending() const { return is_coordinates_depending; }

	void set_on_objects(bool flag) { is_on_objects = flag; }
	bool check_on_objects() const { return is_on_objects; }
	// 设定质量（默认为1.0）[kg]
	void set_mass(float mass) 
	{
		if (mass <= 0) return;
		this->mass = mass;
	}
	float get_mass() const { return mass; }
	// 设定当前受力[pix * kg / ms^2]
	void set_force(const Vector2& force) { this->force = force; }
	// 设定当前受力[pix * kg / ms^2]
	void set_force(float x, float y) { force.x = x, force.y = y; }
	// 获取当前受力
	const Vector2& get_force() const { return force; }
	// 设定圆形碰撞体半径（默认为0）
	void set_radius(float radius) { this->radius = radius; }
	float get_radius() const { return radius; }
	// 设定恢复系数（范围0~1）（默认为1.0，完全弹性碰撞）
	void set_e(float e) 
	{ 
		if (e >= 0.0f && e <= 1.0f)
			this->e = e;
	}
	float get_e() const { return e; }
	// 设定粗糙程度（范围0~1）（默认为0.1）
	void set_mju(float mju)
	{
		if (mju >= 0.0f && mju <= 1.0f)
			this->mju = mju;
	}
	float get_mju() const { return mju; }

	void use_radius(bool flag) { has_radius = flag; }
	bool if_has_radius() const { return has_radius; }
	void use_physics(bool flag) { has_physics = flag; }
	bool if_has_physics() const { return has_physics; }
	void use_obj_collision_resolution(bool flag) { has_obj_collision_resolution = flag; }
	bool if_has_obj_collision_resolution() { return has_obj_collision_resolution; }
	void use_tile_collision_resolution(bool flag) { has_tile_collision_resolution = flag; }
	bool if_has_tile_collision_resolution() { return has_tile_collision_resolution; }
	void use_camera_scaling(bool flag) { has_camera_scaling = flag; }
	bool if_has_camera_scaling() const { return has_camera_scaling; }

	// 力作用于物体，叠加作用（物理引擎开启生效）
	void apply_force(const Vector2& force) { this->force += force; }
	// 力作用于物体，叠加作用（物理引擎开启生效）
	void apply_force(float x, float y) { force.x += x, force.y += y; }

	// 碰撞检测
	bool check_collision(Object* other) const;

	// 碰撞响应：最小化重叠面积与e碰撞（对开启物理引擎的物体生效）
	void resolve_collision(Object* other);

	bool has_parent() const { return parent_p ? true : false; }

	void iterator_begin() { it = object_list.begin(); }
	std::vector<Object*>::iterator iterator_next() { return (it == object_list.end() ? it : it++); }
	void iterator_end() { it = object_list.end(); }
	bool check_if_end() { return it == object_list.end(); }
	size_t get_elem_num() { return object_list.size(); }

	void set_cur_collision_normal(const Vector2& vec) { cur_collision_normal = vec; }
	const Vector2& get_cur_collision_normal() const { return cur_collision_normal; }
	void set_cur_collision_normal_object(const Vector2& vec) { cur_collision_normal_object = vec; }
	const Vector2& get_cur_collision_normal_object() const { return cur_collision_normal_object; }
	void set_cur_collision_t(CollisionType c_t);
	CollisionType get_cur_collision_t() const;

public:						// 不推荐直接使用的
	// 挂载组件
	void register_component(Component* component_p)				
	{
		if (component_p)
			component_list.push_back(component_p);
	}
	// 挂载对象
	void register_object(Object* object_p)						
	{
		if (object_p)
			object_list.push_back(object_p);
	}
	// 注销对象
	void deregister_object(Object* object_p);
	// 注销组件
	void deregister_component(Component* component_p);

	// 直接指定上一个位置为（慎用，仅特殊用途）
	void set_last_coordinates(float x, float y)
	{
		last_coordinates.x = x;
		last_coordinates.y = y;
	}
	// 直接指定上一个位置为（慎用，仅特殊用途）
	void set_last_coordinates(Vector2 vec)
	{
		last_coordinates = vec;
	}

protected:
	virtual ~Object() = default;

private:
	std::vector<Object*> object_list;						// 子对象列表
	std::vector<Component*> component_list;					// 挂载组件列表

	std::vector<Object*>::iterator it;						// 子对象列表迭代器
	
	Object* parent_p;										// 父对象指针

	Vector2 coordinates;									// 坐标（相对于上一级）
	Vector2 last_coordinates;								// 最近一次调用move或坐标修正前的坐标（相对上一级）
	Vector2 orientation;									// 方向（标准化）
	Vector2 idle_orientation;								// 默认方向（标准化）
	Vector2 scale;											// 缩放比 默认为 {1,1}
	Vector2 velocity;										// 速度
	Vector2 size;											// 大小(长、高）、矩形碰撞体尺寸
	Vector2 force;											// 当前作用力

	float mass;												// 质量
	float radius;											// 球形碰撞体半径
	float e;												// 弹性系数
	float mju;												// 粗糙程度

	bool is_valid;											// 是否合法
	bool is_visible;										// 是否可见 ->on_draw
	bool is_effective;										// 是否有效 ->on_input
	bool is_updating;										// 是否更新 ->on_update
	bool is_orientation_depending;							// 方向对方向依赖
	bool is_coordinates_depending;							// 坐标对方向依赖
	bool is_on_objects;										// 当前是否踩踏在其它物体上

	bool has_radius;										// 是否应用球形碰撞体而非矩形
	bool has_physics;										// 是否受物理引擎支配（不受物理引擎支配者不受力的控制）
	bool has_obj_collision_resolution;						// 是否与物体碰撞响应resolve
	bool has_tile_collision_resolution;						// 是否与瓦片图块碰撞响应resolve
	bool has_camera_scaling;								// 是否使用相机的缩放功能

	Vector2 cur_collision_normal;							// 当前碰撞单位向量
	Vector2 cur_collision_normal_object;					// 当前与其他物体的碰撞单位向量
	CollisionType cur_collision_t;							// 当前碰撞的图块类型

	virtual void self_delete();								// 自删除				
};

#endif // !_OBJECT_H_