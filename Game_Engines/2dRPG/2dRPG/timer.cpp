#include "timer.h"

void Timer::on_update()
{
	if (paused) return;

	pass_time += gv_p->get_delta();
	if (pass_time >= wait_time)
	{
		pass_time = 0;

		if ((!one_shot || (one_shot && !is_shotted)) && callback) callback();
		if (!is_shotted) is_shotted = true;
	}
}
