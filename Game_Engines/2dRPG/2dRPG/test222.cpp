#include "test222.h"

Test222::Test222()
{
	pSl = nullptr;
	is_initialized = false;
}

void Test222::on_enter()
{
	if (!is_initialized)
	{
		pSl = new ScrollList(ScrollType::Vertical, 4, 280, 300);
		pSl->set_coordinates(50, 50);

		Layout* layout2 = nullptr;
		for (int i = 0; i < 10; i++)
		{
			Layout* layout = new Layout();
			layout->set_bk_t(BkType::PureColor);
			layout->set_color(RGB(30 + i * 20, i * 10, 10 + i * 20));
			layout->batch_on();

			pSl->add_elem(layout);
			if (i == 2) layout2 = layout;
		}

		pSl->update_layout();
		pSl->batch_on();
		gv_p->register_component(pSl);

		is_initialized = true;
	}
}

void Test222::on_input()
{
	gv_p->batch_input();
}

void Test222::on_update()
{
	gv_p->batch_update();
}

void Test222::on_draw()
{
	gv_p->batch_draw();
}
