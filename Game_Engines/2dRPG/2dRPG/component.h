#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "object.h"

class Object;
class GVar;
class Log;

// 组件
class Component
{
public:
	Component()
	{
		component_list.reserve(6);
		it = component_list.end();

		parent_component_p = nullptr;
		parent_object_p = nullptr;

		coordinates = { 0,0 };
		orientation = { 1,0 };
		idle_orientation = { 1,0 };
		scale = { 1,1 };
		size = { 0,0 };

		is_valid = true;
		is_visible = false;
		is_effective = false;
		is_updating = false;

		has_camera_scaling = false;

		weight = 1;
	}

	virtual void on_input() { }

	virtual void on_update() { }

	virtual void on_draw() { }

	// 递归输入处理（子先父后）
	virtual void process_inputing();
	// 递归更新挂载（父先子后）
	virtual void process_updating();
	// 递归绘制挂载（父先子后）
	virtual void process_drawing();

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

	// 设置父对象为（与父组件互斥）
	void set_parent_object(Object* par_p);

	// 设置父组件为（与父对象互斥）
	void set_parent_component(Component* par_p);

	virtual void set_coordinates(const Vector2& coordinates)
	{
		this->coordinates = coordinates;
	}
	virtual void set_coordinates(float x, float y)
	{
		this->coordinates.x = x;
		this->coordinates.y = y;
	}

	const Vector2& get_rel_coordinates() const { return coordinates; }
	Vector2 get_abs_coordinates() const;

	Vector2 get_parent_abs_coordinates() const
	{
		if (!parent_component_p && !parent_object_p) return get_rel_coordinates();
		return get_abs_coordinates() - get_rel_coordinates();
	}

	virtual void set_size(const Vector2& size)
	{
		this->size = size;
	}
	virtual void set_size(int w, int h)
	{
		size.x = w;
		size.y = h;
	}
	const Vector2& get_size() const { return size; }
	// 获取父挂载的尺寸。若无父挂载，则返回零向量
	Vector2 get_if_parent_size() const;

	void set_orientation(Vector2& orientation)
	{
		this->orientation = orientation.normalize();
	}
	// 设定默认方向
	void set_idle_orientation(const Vector2& vec)
	{
		orientation = idle_orientation = vec.normalize();
	}
	const Vector2& get_idle_orientation() const { return idle_orientation; }

	const Vector2& get_orientation() const { return orientation; }
	// 旋转以角度
	void rotate(float angle)
	{
		orientation.rotate(angle);
	}
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

	void use_camera_scaling(bool flag) { has_camera_scaling = flag; }
	bool if_has_camera_scaling() const { return has_camera_scaling; }
	// ----迭代器
	
	// 迭代器重置开头
	void iterator_begin() { it = component_list.begin(); }
	// 返回迭代器，并将迭代器移至下一位就绪
	std::vector<Component*>::iterator iterator_next() 
	{ 
		return (it == component_list.end() ? it : it++);
	}
	// 将迭代器置于末尾，结束迭代
	void iterator_end() { it = component_list.end(); }
	// 检查迭代器是否在末尾，即是否结束迭代
	bool check_if_end() { return it == component_list.end(); }
	// ----反向迭代器
	
	// 反向迭代器重置开头
	void iterator_rbegin() { rit = component_list.rbegin(); }
	// 返回反向迭代器，并将反向迭代器移至下一位就绪
	std::vector<Component*>::reverse_iterator iterator_rnext() 
	{ 
		return (rit == component_list.rend() ? rit : rit++);
	}
	// 将反向迭代器置于末尾，结束迭代
	void iterator_rend() { rit = component_list.rend(); }
	// 检查反向迭代器是否在末尾，即是否结束迭代
	bool check_if_rend() { return rit == component_list.rend(); }
	// 获取元素数量
	size_t get_elem_num() { return component_list.size(); }
	// 获取第一个元素，没有返回nullptr
	Component* get_first_one() 
	{
		return (component_list.size() > 0 ? component_list[0] : nullptr);
	}
	// 获取最后一个元素，没有返回nullptr
	Component* get_last_one()
	{
		return (component_list.size() > 0 ? *(component_list.rbegin()) : nullptr);
	}
	
	void last_elem_pop()
	{
		if (!component_list.empty())
		{
			(*component_list.rbegin())->set_parent_component(nullptr);
		}
	}
	void first_elem_pop()
	{
		if (!component_list.empty())
		{
			(*component_list.begin())->set_parent_component(nullptr);
		}
	}
	void insert_to_first(Component* com_p)
	{
		if (com_p)
		{
			com_p->set_parent_component(this);
			component_list.pop_back();
			component_list.insert(component_list.begin(), com_p);
		}
	}

	// 设置布局权重（ N* ）
	void set_weight(int wt) 
	{
		if (wt < 1) return;
		weight = wt;
	}
	// 获取布局权重
	int get_weight() const { return weight; }

	// 检测当前鼠标移动坐标是否在组件内部
	bool check_if_mousemove_in_self();
	// 检测当前鼠标点击坐标是否在组件内部
	bool check_if_mouseclick_in_self();

public:
	// 挂载组件
	void register_component(Component* component_p)
	{
		if (component_p)
			component_list.push_back(component_p);
	}
	// 注销组件
	int deregister_component(Component* component_p);

	virtual ~Component() = default;

private:

	std::vector<Component*> component_list;					// 子组件列表
	std::vector<Component*>::iterator it;					// 迭代器
	std::vector<Component*>::reverse_iterator rit;			// 反向迭代器

	Object* parent_object_p;								// 父对象指针（与父组件指针互斥）
	Component* parent_component_p;							// 父组件指针（与父对象指针互斥）

	Vector2 coordinates;									// 坐标（相对于上一级）
	Vector2 orientation;									// 方向（标准化）
	Vector2 idle_orientation;								// 默认方向（标准化）
	Vector2 scale;											// 缩放比 默认为 {1,1}
	Vector2 size;											// 大小(长、高）

	bool is_valid;											// 是否合法
	bool is_visible;										// 是否可见 ->on_draw
	bool is_effective;										// 是否有效 ->on_input
	bool is_updating;										// 是否更新 ->on_update

	bool has_camera_scaling;								// 是否使用相机的缩放功能

	int weight;												// 布局权重

	// 自删除
	virtual void self_delete();								

};

#endif // !_COMPONENT_H_