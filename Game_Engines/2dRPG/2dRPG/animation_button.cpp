#include "animation_button.h"

void AnimationButton::on_draw()
{
	if (check_down()) animation_down.on_draw();
	else if (check_touched()) animation_touched.on_draw();
	else animation_idle.on_draw();
}
