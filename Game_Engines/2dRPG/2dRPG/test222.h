#pragma once

#include "scene.h"
#include "scroll_list.h"
#include "var_g.h"

extern GVar* gv_p;

class Test222 : public Scene
{
public:
	Test222();
	~Test222() = default;

	void on_enter() override;

	void on_input() override;

	void on_update() override;

	void on_draw() override;

private:
	bool is_initialized;

	ScrollList* pSl;

};
