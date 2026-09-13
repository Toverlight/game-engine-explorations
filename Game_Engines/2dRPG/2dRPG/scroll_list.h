#ifndef _SCROLL_LIST_H_
#define _SCROLL_LIST_H_

#include "layout.h"
#include "slider.h"
#include "rect_button.h"
#include "space_filler.h"

class GVar;
class Log;
extern GVar* gv_p;
extern Log* log_p;

// 滚动列表
class ScrollList : public Layout
{
	class Container : public Layout
	{
	public:
		Container() :pSlider(nullptr) {}
		~Container() = default;

		void on_input() override;

		Slider* pSlider;
	};

public:
	ScrollList(ScrollType st, int elemShowNum_, int len, int width);
	~ScrollList();

	void add_elem(Component* com_p) override;
	void del_elem(Component* com_p);

	void on_update() override;
	// 测试用
	void output_container_elems()
	{
		container.iterator_begin();
		while (!container.check_if_end())
		{
			auto it = container.iterator_next();
			std::cout << *it << std::endl;
		}
		container.iterator_end();
	}

private:
	int elemShowNum;					// 最多同时显示的元素个数
	std::vector<Component*> elem_list;	// 实际的元素存在这里
	size_t first_loc;					// 当前显示的第一个元素在elem_list中的下标

	SpaceFiller sfTop;					// 顶部填充

	Layout midBox;						// 中间容器
	SpaceFiller sfLeft;					// 左部填充
	Container container;				// 元素展示容器，只存放当前要显示的元素
	Slider* pSlider;					// 滑轨滑块

	SpaceFiller sfBottom;				// 底部填充

private:
	static const int MARGIN_SF = 5;		// 填充间隔

};

#endif // !_SCROLL_LIST_H_
