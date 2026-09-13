#ifndef _SLIDER_H_
#define _SLIDER_H_

#include "layout.h"
#include "rect_button.h"

class GVar;
extern GVar* gv_p;

// 滚动类型
enum class ScrollType
{
	Horizontal,					// 水平滚动
	Vertical					// 竖直滚动
};
// 滑块控制信息
enum class SliderCtrlInfo
{
	None,						// 无
	AddOne,						// 加一
	SubOne						// 减一
};

// 滑轨滑块
class Slider : public Layout
{
	class SlidingBlock : public Component
	{
	public:
		SlidingBlock()
		{
			slidingBlockColor = RGB(180, 180, 180);
			slidingBlockTouchedColor = RGB(155, 155, 155);

			is_touched = false;
			is_down = false;
		}
		~SlidingBlock() = default;

		void on_draw() override;
		
		COLORREF slidingBlockColor;			// 滑块颜色
		COLORREF slidingBlockTouchedColor;	// 滑块触碰颜色	

		bool is_touched;
		bool is_down;
	};
public:
	Slider(ScrollType s_t, int len)
	{
		st = s_t;

		sci = SliderCtrlInfo::None;
		
		recorded_XorY = -1;

		is_inArea = false;

		slidingRailWidth = 20;
		slidingBlockLength = len - 40;
		containNum = 1;
		containSum = 0;

		slidingRail.set_layout_t(LayoutType::Absolute);
		slidingRail.set_bk_t(BkType::PureColor);
		slidingRail.set_color(RGB(220, 220, 220));
		slidingRail.set_covering(false);

		add_elem(&btnUpOrLeft);
		btnUpOrLeft.set_coordinates(0, 0);
		btnUpOrLeft.set_line_color(RGB(46, 46, 46));
		btnUpOrLeft.set_fill_color(RGB(46, 46, 46));
		btnUpOrLeft.set_fill_touched_color(RGB(133, 133, 133));
		btnUpOrLeft.set_fill_down_color(RGB(121,121,121));
		btnUpOrLeft.get_text().set_text_color(RGB(127, 127, 127));
		btnUpOrLeft.get_text().set_text_style(18, 9, _T("Consolas"));
		btnUpOrLeft.set_callback([&]()
			{
				sci = SliderCtrlInfo::AddOne;
				std::cout << "containSum = " << containSum <<
					", containNum = " << containNum << std::endl;
			});

		btnDownOrRight.set_line_color(RGB(46, 46, 46));
		btnDownOrRight.set_fill_color(RGB(46, 46, 46));
		btnDownOrRight.set_fill_touched_color(RGB(153, 153, 153));
		btnDownOrRight.set_fill_down_color(RGB(121, 121, 121));
		btnDownOrRight.get_text().set_text_color(RGB(127, 127, 127));
		btnDownOrRight.get_text().set_text_style(18, 9, _T("Consolas"));
		btnDownOrRight.set_callback([&]()
			{
				sci = SliderCtrlInfo::SubOne;
				std::cout << "containSum = " << containSum <<
					", containNum = " << containNum << std::endl;
			});
		if (s_t == ScrollType::Vertical)
		{
			btnUpOrLeft.get_text().set_content(_T("▲"));
			btnDownOrRight.get_text().set_content(_T("▼"));
			set_size(slidingRailWidth, len);
			btnUpOrLeft.set_size(slidingRailWidth, 20);
			slidingRail.set_size(slidingRailWidth, len - 40);
			slidingRail.set_coordinates(0, 20);
			slidingBlock.set_size(slidingRailWidth - 6, slidingBlockLength);
			slidingBlock.set_coordinates(3, 0);
			btnDownOrRight.set_size(slidingRailWidth, 20);
			btnDownOrRight.set_coordinates(0, len - 20);
		}
		else
		{
			btnUpOrLeft.get_text().set_content(_T("◀"));
			btnDownOrRight.get_text().set_content(_T("▶"));
			set_size(len, slidingRailWidth);
			btnUpOrLeft.set_size(20, slidingRailWidth);
			slidingRail.set_size(len - 40, slidingRailWidth);
			slidingRail.set_coordinates(20, 0);
			slidingBlock.set_size(slidingBlockLength, slidingRailWidth - 6);
			slidingBlock.set_coordinates(0, 3);
			btnDownOrRight.set_size(20, slidingRailWidth);
			btnDownOrRight.set_coordinates(len - 20, 0);
		}

		slidingRail.add_elem(&slidingBlock);
		add_elem(&slidingRail);
		add_elem(&btnDownOrRight);
		
	}

	~Slider() = default;

	void on_input() override;

	void on_update() override;

	// 获取滑块控制信息
	SliderCtrlInfo get_SCI() const { return sci; }
	// 发送滑块控制信息
	void send_SCI(SliderCtrlInfo sci) { this->sci = sci; }
	// 重置滑块控制信息
	void reset_SCI() { sci = SliderCtrlInfo::None; }
	
	void set_containSum(int sum)
	{
		containSum = sum;
	}

	void set_containNum(int num)
	{
		if (num < 1 || num > 10000) return;
		containNum = num;
	}
	// 更新滑块尺寸（在containNum或containSum更新时调用）
	void update_slidingBlockSize()
	{
		if (st == ScrollType::Vertical)
		{
			slidingBlockLength = slidingRail.get_size().y * ((float)containNum / containSum);
			slidingBlock.set_size(slidingRailWidth - 6, slidingBlockLength);
		}
		else
		{
			slidingBlockLength = slidingRail.get_size().x * ((float)containNum / containSum);
			slidingBlock.set_size(slidingBlockLength, slidingRailWidth - 6);
		}
	}
	// 置底
	void move_to_bottom()
	{
		if (st == ScrollType::Vertical)
		{
			slidingBlock.set_coordinates(3, slidingRail.get_size().y - slidingBlockLength);
		}
		else
		{
			slidingBlock.set_coordinates(slidingRail.get_size().x - slidingBlockLength, 3);
		}
	}
	// 置顶
	void move_to_top()
	{
		if (st == ScrollType::Vertical)
		{
			slidingBlock.set_coordinates(3, 0);
		}
		else
		{
			slidingBlock.set_coordinates(0, 3);
		}
	}

	int get_slidingRailWidth() const { return slidingRailWidth; }
	int get_slidingBlockLength() const { return slidingBlockLength; }

	ScrollType get_scrollType() const { return st; }

private:
	ScrollType st;						// 滚动类型

	RectButton btnUpOrLeft;				// 上

	Layout slidingRail;					// 滑轨
	SlidingBlock slidingBlock;			// 滑块

	RectButton btnDownOrRight;			// 下

	int slidingRailWidth;				// 滑轨宽度
	int slidingBlockLength;				// 滑块当前长度

	int containNum;						// 最多一起显示的元素数量
	int containSum;						// 关联容器当前容纳的元素总量

	SliderCtrlInfo sci;					// 滑块控制信息

	bool is_inArea;						// 在slider大区域内

	int recorded_XorY;					// 被按下时的x或y坐标

};

#endif // !_SLIDER_H_

