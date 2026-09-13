#ifndef _ANIMATION_BUTTON_H_
#define _ANIMATION_BUTTON_H_

#include "button.h"
#include "animation.h"

class AnimationButton : public Button
{
public:
	AnimationButton()
	{
		animation_idle.set_parent_component(this);
		animation_touched.set_parent_component(this);
		animation_down.set_parent_component(this);
	}
	~AnimationButton() = default;

	void on_draw();
	// 设置默认动画
	void set_animation_idle(Atlas* atlas_p, int interval)
	{
		animation_idle.set_atlas(atlas_p);
		animation_idle.set_loop(false);
		animation_idle.set_interval(interval);
		animation_idle.set_callback([&]()
			{
				animation_idle.restart();
			});
		animation_idle.set_visible(true);
	}
	// 设置触碰动画
	void set_animation_touch(Atlas* atlas_p, int interval)
	{
		animation_touched.set_atlas(atlas_p);
		animation_touched.set_loop(false);
		animation_touched.set_interval(interval);
		animation_touched.set_callback([&]()
			{
				animation_touched.restart();
			});
		animation_touched.set_visible(true);
	}
	// 设置按下动画
	void set_animation_down(Atlas* atlas_p, int interval)
	{
		animation_down.set_atlas(atlas_p);
		animation_down.set_loop(false);
		animation_down.set_interval(interval);
		animation_down.set_callback([&]()
			{
				animation_down.restart();
			});
		animation_down.set_visible(true);
	}


private:
	Animation animation_idle;			// 默认动画
	Animation animation_touched;		// 触碰动画
	Animation animation_down;			// 按下动画


};

#endif // !_ANIMATION_BUTTON_H_

