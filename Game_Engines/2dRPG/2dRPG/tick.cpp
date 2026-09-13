#include "tick.h"

extern GVar* gv_p;

void Tick::on_update()
{
	tpf = GetTickCount() - last_tick;
	gv_p->set_delta(tpf);

	if (tpf < tpf_limit)
		Sleep(tpf_limit - tpf - 1);

	last_tick = GetTickCount();

}
