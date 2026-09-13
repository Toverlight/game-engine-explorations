#ifndef _VAR_G_H_
#define _VAR_G_H_

#include "log.h"
#include "component.h"
#include "scene_manager.h"

#include <graphics.h>

class Object;
class Camera;
class Component;
class SceneManager;
class TileMap;
class Layout;

extern Log* log_p;
// 键盘消息处理方案
enum class KeyPlan
{
	Single,				// 单例存取
	List,				// 多例存取
	Bool				// 状态存取
};
// 全局变量管理器
class GVar
{
public:
	static GVar* get_instance();
	// 获取消息队列里的所有消息（将各类消息缓存更新至最新）
	void peek_msg_with_while()
	{
		while (peekmessage(&msg_buffer))
		{
			switch (msg_buffer.message)
			{
			case WM_MOUSEMOVE:
			if (mouse_msg_move.x != msg_buffer.x || mouse_msg_move.y != msg_buffer.y)
				mouse_msg_move = msg_buffer;
				break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
				mouse_msg_click = msg_buffer;
				break;
			case WM_KEYDOWN:
			case WM_KEYUP:
				switch (kp)
				{
				case KeyPlan::Single:
					key_msg = msg_buffer;
					break;
				case KeyPlan::List:
					key_msg_list.push_back(msg_buffer);
					break;
				case KeyPlan::Bool:
					if (msg_buffer.message == WM_KEYDOWN)
						keyState[msg_buffer.vkcode] = true;
					else
						keyState[msg_buffer.vkcode] = false;
					break;
				default:
					break;
				}
				break;
			case WM_MOUSEWHEEL:
				mouse_msg_wheel = msg_buffer;
				break;
			}
		}
	}
	void clear_msg_without_mousemove()
	{
		mouse_msg_click = { 0 };
		mouse_msg_wheel = { 0 };
		if (kp == KeyPlan::Single)
		{
			key_msg = { 0 };
		}
		else if (kp == KeyPlan::List)
		{
			key_msg_list.clear();
			head = 0;
		}
		else
		{
			//...
		}
	}
	void clear_mouseclick_msg() { mouse_msg_click = { 0 }; }
	void clear_mousewheel_msg() { mouse_msg_wheel = { 0 }; }
	void clear_key_msg(){ key_msg = { 0 }; }
	void clear_key_msg_list() { key_msg_list.clear(); head = 0; }
	void clear_key_msg_head() 
	{
		if (check_if_key_msg_list())
			key_msg_list[head] = { 0 };
	}
	bool check_if_key_msg()
	{
		if (key_msg.message == WM_KEYDOWN || key_msg.message == WM_KEYUP)
			return true;
		return false;
	}
	const ExMessage& get_key_msg() const
	{
		return key_msg;
	}
	bool check_if_mouseclick_msg()
	{
		if (mouse_msg_click.message == WM_LBUTTONDOWN || mouse_msg_click.message == WM_LBUTTONUP)
			return true;
		return false;
	}
	const ExMessage& get_mouseclick_msg() const
	{
		return mouse_msg_click;
	}
	bool check_if_mousemove_msg_in_window() const
	{
		return (mouse_msg_move.x >= 0 && mouse_msg_move.x <= getwidth() &&
			mouse_msg_move.y >= 0 && mouse_msg_move.y <= getheight());
	}
	const ExMessage& get_mousemove_msg() const
	{
		return mouse_msg_move;
	}
	bool check_if_key_msg_list() const
	{
		return head < key_msg_list.size();
	}
	const ExMessage& get_key_msg_head()
	{
		return key_msg_list[head];
	}
	bool* const get_keyState()
	{
		return keyState;
	}
	void key_msg_head_toStart() { head = 0; }
	void key_msg_head_toNext() { ++head; }
	// 将鼠标消息置外，使之失效
	void out_mousemove_msg()
	{
		mouse_msg_move.x = -1, mouse_msg_move.y = -1;
	}
	bool check_if_mousewheel_msg()
	{
		if (mouse_msg_wheel.message == WM_MOUSEWHEEL)
			return true;
		return false;
	}
	// 鼠标滚轮滚动格数（+为向上，-为向下）
	int get_mousewheel_unitNum() const
	{
		short mParam = mouse_msg_wheel.wheel;
		int unitNum = (int)mParam / 120;

		return unitNum;
	}

	void use_key_msg()
	{
		clear_key_msg();
		kp = KeyPlan::Single;
	}
	void use_key_msg_list()
	{
		clear_key_msg_list();
		kp = KeyPlan::List;
	}
	void use_key_state()
	{
		kp = KeyPlan::Bool;
	}
	
	void set_delta(int delta) { this->delta = delta; }
	int get_delta() const { return delta; }

	void set_camera(Camera* camera_p) { this->camera_p = camera_p; }
	Camera* get_camera() const { return camera_p; }

	SceneManager* get_scene_manager() { return scene_manager_p; }

	void set_tile_map(TileMap* tile_map_p) { this->tile_map_p = tile_map_p; }
	TileMap* get_tile_map() { return tile_map_p; }

	// 批量消息处理
	void batch_input();						
	// 批量更新
	void batch_update();
	// 批量绘制
	void batch_draw();
	// 批量绘制仅物体
	void batch_draw_object();
	// 批量绘制仅组件
	void batch_draw_component();
	// 绘制瓦片地图（除了水）
	void tile_draw();
	// 绘制瓦片地图的水
	void tile_draw_water();
	// 批量清除非法挂载
	void batch_invalid_clear();				
	// 批量物理模拟
	void batch_model();
	// 聚焦布局面板
	void focus_on(Layout* layout_p)
	{
		focus_p = layout_p;
	}
	// 解除聚焦，即恢复到对全局的输入监测
	void focus_off()
	{
		focus_p = nullptr;
	}
	// 挂起除指定布局面板外的所有对象和组件（影响update和batch_model，相当于游戏暂停）
	void suspend_except(Layout* layout_p)
	{
		if (!layout_p)
			suspend_all();
		else
		{
			for (auto it = component_list.begin(); it != component_list.end(); it++)
			{
				if ((*it) != (Component*)(layout_p))
				{
					(*it)->batch_set_updating(false);
				}
			}
			for (auto it = object_list.begin(); it != object_list.end(); it++)
			{
				(*it)->batch_set_updating(false);
			}
		}
	}
	// 挂起一切（影响update和batch_model，相当于游戏暂停）
	void suspend_all()
	{
		for (auto it = object_list.begin(); it != object_list.end(); it++)
		{
			(*it)->batch_set_updating(false);
		}
		for (auto it = component_list.begin(); it != component_list.end(); it++)
		{
			(*it)->batch_set_updating(false);
		}

	}
	// 释放除指定布局面板外的所有对象和组件（影响update和batch_model，相当于游戏继续）
	void release_except(Layout* layout_p)
	{
		if (!layout_p)
			release_all();
		else
		{
			for (auto it = object_list.begin(); it != object_list.end(); it++)
			{
				(*it)->batch_set_updating(true);
			}
			for (auto it = component_list.begin(); it != component_list.end(); it++)
			{
				if ((*it) != (Component*)(layout_p))
				{
					(*it)->batch_set_updating(true);
				}
			}
		}
	}
	// 释放一切（影响update和batch_model，相当于游戏继续）
	void release_all()
	{
		for (auto it = object_list.begin(); it != object_list.end(); it++)
		{
			(*it)->batch_set_updating(true);
		}
		for (auto it = component_list.begin(); it != component_list.end(); it++)
		{
			(*it)->batch_set_updating(true);
		}
	}

	// -----------------------图层----------------------------
	
	// 将对象上移一层
	void upper_one_object(Object* obj_p);
	// 将组件上移一层
	void upper_one_component(Component* com_p);
	// 将对象下移一层
	void lower_one_object(Object* obj_p);
	// 将组件下移一层
	void lower_one_component(Component* com_p);
	// 将对象移至最上层
	void upper_top_object(Object* obj_p);
	// 将组件移至最上层
	void upper_top_component(Component* com_p);
	// 将对象移至最下层
	void lower_bottom_object(Object* obj_p);
	// 将组件移至最下层
	void lower_bottom_component(Component* com_p);

	// 注册对象
	void register_object(Object* object_p)
	{
		object_list.push_back(object_p);
	}
	// 注册组件
	void register_component(Component* component_p)
	{
		component_list.push_back(component_p);
	}
	// 注销对象
	void deregister_object(Object* object_p)
	{
		for (auto it = object_list.begin(); it != object_list.end(); ++it)
			if ((*it) == object_p)
			{
				object_list.erase(it);
				break;
			}
	}
	// 注销组件
	void deregister_component(Component* component_p)
	{
		for (auto it = component_list.begin(); it != component_list.end(); ++it)
			if ((*it) == component_p)
			{
				component_list.erase(it);
				break;
			}
	}

	bool check_debug() const { return is_debug; }

private:
	GVar();
	~GVar();

	SceneManager* scene_manager_p;			// 场景管理器
	TileMap* tile_map_p;					// 当前瓦片地图

	int delta;								// 从上一个 tick 经过的时间(ms)
	Camera* camera_p;						// 相机

	ExMessage msg_buffer;					// 消息缓冲区
	ExMessage key_msg;						// 键盘消息				方案1 Single
	std::vector<ExMessage> key_msg_list;	// 键盘消息队列			方案2 List
	bool keyState[256];						// 键盘状态				方案3 Bool
	ExMessage mouse_msg_move;				// 鼠标移动消息（静止时保留上一刻移动消息记录的位置）
	ExMessage mouse_msg_click;				// 鼠标点击消息
	ExMessage mouse_msg_wheel;				// 鼠标滚轮消息

	std::vector<Object*> object_list;		// 对象批量操作列表
	std::vector<Component*> component_list;	// 组件批量操作列表

	bool is_debug;							// 调试状态
	KeyPlan kp;								// 键盘处理方案
	size_t head;							// 键盘消息队列头索引

	Layout* focus_p;						// 聚焦的布局面板

};

#endif // !_VAR_G_H_