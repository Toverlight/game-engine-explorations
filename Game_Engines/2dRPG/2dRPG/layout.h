#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include "log.h"
#include "util.h"
#include "animation.h"
#include "component.h"

#include <vector>
#include <graphics.h>

extern Log* log_p;

// 布局类型
enum class LayoutType
{
	Absolute,			// 绝对布局
	LinearHorizontal,	// 线性布局（横）
	LinearVertical,		// 线性布局（竖）
	Grid,				// 网格布局
	Flow,				// 流式布局
	Border				// 边界布局
};
// 背景样式
enum class BkType
{
	Transparent,		// 透明
	PureColor,			// 纯色
	Image,				// 图片
	Animation			// 动画
};
// 布局管理器
class Layout : public Component
{
public:
	Layout()
	{
		layout_t = LayoutType::Absolute;
		bk_t = BkType::Transparent;

		tuple = { 1,1 };
		color = RGB(72, 141, 72);
		img_p = nullptr;
		animation_p = nullptr;

		is_covering = true;
	}
	virtual ~Layout() = default;

	virtual void on_input() override;

	virtual void on_draw() override;
	// 设置布局模式
	void set_layout_t(LayoutType layout_t)
	{
		this->layout_t = layout_t;
	}
	LayoutType get_layout_t() const { return layout_t; }
	// 设置背景模式
	void set_bk_t(BkType bk_t)
	{
		this->bk_t = bk_t;
	}
	BkType get_bk_t() const { return bk_t; }

	void set_tuple(const Vector2& vec)
	{
		if (vec.x >= 1) tuple.x = (int)vec.x;
		if (vec.y >= 1) tuple.y = (int)vec.y;
	}
	void set_tuple(int x_num, int y_num)
	{
		if (x_num >= 1) tuple.x = x_num;
		if (y_num >= 1) tuple.y = y_num;
	}
	const Vector2& get_tuple() const { return tuple; }

	void set_color(COLORREF color)
	{
		this->color = color;
	}
	void set_img_p(IMAGE* img_p)
	{
		if (!(this->img_p = img_p))
		{
			log_p->setTarget(LogTarget::Console);
			log_p->warn("Layout::", __func__, ": You set a nullptr for \"img_p\".");
		}
	}
	void set_animation_p(Animation* animation_p)
	{
		if (!(this->animation_p = animation_p))
		{
			log_p->setTarget(LogTarget::Console);
			log_p->warn("Layout::", __func__, ": You set a nullptr for \"animation_p\".");
		}
	}

	// 设置覆盖下方探测
	void set_covering(bool flag) { is_covering = flag; }

	// 添加元素
	virtual void add_elem(Component* com_p);
	// 更新布局
	void update_layout();

	// 获取组件列表中所有元素的权重和
	int get_total_weight();

private:
	LayoutType layout_t;						// 布局类型
	BkType bk_t;								// 背景样式

	Vector2 tuple;								// Grid 二维数据
	COLORREF color;								// 颜色
	IMAGE* img_p;								// 图片
	Animation* animation_p;						// 动画

	bool is_covering;							// 覆盖下方的探测
};

#endif // !_LAYOUT_H_

