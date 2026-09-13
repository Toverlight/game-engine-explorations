#include "scene_1.h"

Scene_1::Scene_1()
{
	loadimage(&img_bk, _T("res/images/background/moon_mountain_city.png"));
	set_img_background(&img_bk);
	set_using_camera_bk(false);

	set_offset(0, 0);

	tile_map_p = dbm_p->loadTileMapData("scene_1_tilemap");
	gv_p->set_tile_map(tile_map_p);

	is_panel_open = false;

	is_left_moving = false;
	is_right_moving = false;
	is_jumping = false;
	is_down = false;

	log_p->setTarget(LogTarget::Console);
	log_p->info("Scene_1 created.");
}

void Scene_1::on_enter()
{
	log_p->setTarget(LogTarget::Console);
	log_p->info("Scene_1 entering...");

	camera.set_coordinates(-400, -300);
	camera.set_parent(&ball_red);
	gv_p->set_camera(&camera);

	ball_red.set_coordinates(528, 128);
	ball_red.set_velocity(-0.08f, -0.5f);
	ball_red.set_fill_color(RGB(243, 86, 97));
	ball_red.set_line_color(WHITE);
	ball_red.set_radius(30);
	ball_red.set_e(0);
	ball_red.set_mju(0.5);
	ball_red.set_mass(0.5);
	ball_red.set_visible(true);
	ball_red.use_camera_scaling(true);

	gv_p->register_object(&ball_red);

	ball_yellow.set_coordinates(388, 128);
	ball_yellow.set_velocity(0.08f, 0);
	ball_yellow.set_fill_color(RGB(255, 205, 67));
	ball_yellow.set_line_color(WHITE);
	ball_yellow.set_radius(25);
	ball_yellow.set_e(0.8);
	ball_yellow.set_mju(0.01);
	ball_yellow.set_mass(0.5);
	ball_yellow.set_visible(true);

	gv_p->register_object(&ball_yellow);

	block_gray.set_coordinates(450, 300);
	block_gray.set_velocity(0.06f, -0.8f);
	block_gray.set_fill_color(RGB(56, 56, 56));
	block_gray.set_line_color(RGB(164, 96, 56));
	block_gray.set_size(90, 60);
	block_gray.set_e(0.9);
	block_gray.set_mju(0.02);
	block_gray.set_mass(0.8);
	block_gray.set_visible(true);

	gv_p->register_object(&block_gray);

	block_blue.set_coordinates(390, 100);
	block_blue.set_velocity(0.3f, -0.8f);
	block_blue.set_fill_color(BLUE);
	block_blue.set_line_color(0xEE0000);
	block_blue.set_size(70, 60);
	block_blue.set_e(0.9);
	block_blue.set_mju(0.02);
	block_blue.set_mass(0.8);
	block_blue.set_visible(true);

	gv_p->register_object(&block_blue);

	addv_right_btn.set_line_color(RGB(108, 130, 138));
	addv_right_btn.set_fill_color(RGB(30, 30, 30));
	addv_right_btn.set_coordinates(720, 60);
	addv_right_btn.set_size(70, 40);
	addv_right_btn.set_callback([&]()
		{
			ball_red.add_velocity(0.1f, 0);
		});
	addv_right_btn.get_text().set_text_color(RGB(85, 177, 85));
	addv_right_btn.get_text().set_bk_mode(TRANSPARENT);
	addv_right_btn.get_text().set_text_style(30, 16, _T("宋体"));
	addv_right_btn.get_text().set_content(_T("->"));
	addv_right_btn.get_text().set_center(CenterType::Rect);
	addv_right_btn.batch_on();

	gv_p->register_component(&addv_right_btn);

	addv_left_btn.set_line_color(RGB(108, 130, 138));
	addv_left_btn.set_fill_color(RGB(30, 30, 30));
	addv_left_btn.set_coordinates(640, 60);
	addv_left_btn.set_size(70, 40);
	addv_left_btn.set_callback([&]()
		{
			ball_red.add_velocity(-0.1f, 0);
		});
	addv_left_btn.get_text().set_text_color(RGB(85, 177, 85));
	addv_left_btn.get_text().set_bk_mode(TRANSPARENT);
	addv_left_btn.get_text().set_text_style(30, 16, _T("宋体"));
	addv_left_btn.get_text().set_content(_T("<-"));
	addv_left_btn.get_text().set_center(CenterType::Rect);
	addv_left_btn.batch_on();

	gv_p->register_component(&addv_left_btn);

	addv_up_btn.set_line_color(RGB(108, 130, 138));
	addv_up_btn.set_fill_color(RGB(30, 30, 30));
	addv_up_btn.set_coordinates(680, 10);
	addv_up_btn.set_size(70, 40);
	addv_up_btn.set_callback([&]()
		{
			ball_red.add_velocity(0, -0.8f);
		});
	addv_up_btn.get_text().set_text_color(RGB(85, 177, 85));
	addv_up_btn.get_text().set_bk_mode(TRANSPARENT);
	addv_up_btn.get_text().set_text_style(30, 16, _T("宋体"));
	addv_up_btn.get_text().set_content(_T("^"));
	addv_up_btn.get_text().set_center(CenterType::Rect);
	addv_up_btn.batch_on();

	gv_p->register_component(&addv_up_btn);

	panel.set_coordinates(100, 100);
	panel.set_size(600, 400);
	panel.set_layout_t(LayoutType::LinearVertical);
	panel.set_bk_t(BkType::PureColor);
	panel.set_color(RGB(51, 85, 51));

	rect_button1.set_line_color(RGB(108, 130, 138));
	rect_button1.set_fill_color(RGB(30, 30, 30));
	rect_button1.set_callback([]() {});
	rect_button1.get_text().set_text_color(RGB(85, 177, 85));
	rect_button1.get_text().set_bk_mode(TRANSPARENT);
	rect_button1.get_text().set_text_style(30, 15, _T("宋体"));
	rect_button1.get_text().set_content(_T("按钮1"));
	rect_button1.get_text().set_center(CenterType::Rect);

	rect_button2.set_line_color(RGB(108, 130, 138));
	rect_button2.set_fill_color(RGB(30, 30, 30));
	rect_button2.set_callback([]() {});
	rect_button2.get_text().set_text_color(RGB(85, 177, 85));
	rect_button2.get_text().set_bk_mode(TRANSPARENT);
	rect_button2.get_text().set_text_style(30, 15, _T("宋体"));
	rect_button2.get_text().set_content(_T("按钮2"));
	rect_button2.get_text().set_center(CenterType::Rect);

	rect_button3.set_line_color(RGB(108, 130, 138));
	rect_button3.set_fill_color(RGB(30, 30, 30));
	rect_button3.set_callback([]() {});
	rect_button3.get_text().set_text_color(RGB(85, 177, 85));
	rect_button3.get_text().set_bk_mode(TRANSPARENT);
	rect_button3.get_text().set_text_style(60, 30, _T("宋体"));
	rect_button3.get_text().set_content(_T("按钮3"));
	rect_button3.get_text().set_center(CenterType::Rect);

	panel.add_elem(&rect_button1);
	panel.add_elem(&rect_button2);
	panel.add_elem(&rect_button3);

	panel.update_layout();

	gv_p->register_component(&panel);

	log_p->setTarget(LogTarget::Console);
	log_p->info("Scene_1 entered.");
}

void Scene_1::on_input()
{
	if (gv_p->check_if_mousewheel_msg())
	{
		int unitNum = gv_p->get_mousewheel_unitNum();
		if (unitNum > 0)
			camera.set_scale(camera.get_scale() * 2.0f);
		else if (unitNum < 0)
			camera.set_scale(camera.get_scale() / 2.0f);
		gv_p->clear_mousewheel_msg();
	}
	if (gv_p->get_key_msg().message == WM_KEYUP)
	{
		if (gv_p->get_key_msg().vkcode == 'E')
		{
			if (is_panel_open = !is_panel_open)
			{
				panel.batch_on();
				gv_p->suspend_except(&panel);
				gv_p->focus_on(&panel);

				log_p->setTarget(LogTarget::Console);
				log_p->info("Scene1::panel opened.");
			}
			else
			{
				panel.batch_off();
				gv_p->release_except(&panel);
				gv_p->focus_off();

				log_p->setTarget(LogTarget::Console);
				log_p->info("Scene1::panel closed.");
			}
			gv_p->clear_key_msg();
		}
		else
		{
			switch (gv_p->get_key_msg().vkcode)
			{
			case 'W':
			case 'S':
				is_jumping = false;
				is_down = false;
				gv_p->clear_key_msg();
				break;
			case 'A':
			case 'D':
				is_left_moving = false;
				is_right_moving = false;
				gv_p->clear_key_msg();
				break;
			}
		}
	}
	else if (gv_p->get_key_msg().message == WM_KEYDOWN)
	{
		switch (gv_p->get_key_msg().vkcode)
		{
		case 'W':
			if (ball_red.get_cur_collision_normal() == Vector2(0, -1) ||
				ball_red.get_cur_collision_t() == CollisionType::Water ||
				ball_red.check_on_objects())
			{
				is_jumping = true;
			}
			gv_p->clear_key_msg();
			break;
		case 'A':
			if (ball_red.get_cur_collision_normal() == Vector2(0, -1) ||
				ball_red.get_cur_collision_t() == CollisionType::Water ||
				ball_red.check_on_objects())
			{
				is_left_moving = true;
			}
			gv_p->clear_key_msg();
			break;
		case 'S':
			if (ball_red.get_cur_collision_normal() == Vector2(0, -1) ||
				ball_red.get_cur_collision_t() == CollisionType::Water)
			{
				is_down = true;
			}
			gv_p->clear_key_msg();
			break;
		case 'D':
			if (ball_red.get_cur_collision_normal() == Vector2(0, -1) ||
				ball_red.get_cur_collision_t() == CollisionType::Water ||
				ball_red.check_on_objects())
			{
				is_right_moving = true;
			}
			gv_p->clear_key_msg();
			break;
		}
	}
	gv_p->batch_input();
}

void Scene_1::on_update()
{
	const Vector2& v = ball_red.get_velocity();
	if (is_left_moving)
	{
		if (ball_red.get_cur_collision_t() == CollisionType::Water)
		{
			ball_red.set_velocity(-0.1f, v.y);
		}
		else
		{
			ball_red.set_velocity(-0.3f, v.y);
		}
	}
	if (is_right_moving)
	{
		if (ball_red.get_cur_collision_t() == CollisionType::Water)
		{
			ball_red.set_velocity(0.1f, v.y);
		}
		else
		{
			ball_red.set_velocity(0.3f, v.y);
		}
	}

	if (is_jumping)
	{
		if (ball_red.get_cur_collision_t() == CollisionType::Water)
		{
			ball_red.set_velocity(v.x, -0.1f);
		}
		else
		{
			ball_red.set_velocity(v.x, -0.8f);
			is_jumping = false;
		}
	}
	if (is_down)
	{
		if (ball_red.get_cur_collision_t() == CollisionType::Water)
		{
			ball_red.set_velocity(v.x, 0.1f);
		}
		else if (ball_red.get_cur_collision_t() == CollisionType::Platform)
		{
			const Vector2& c_l = ball_red.get_rel_last_coordinates();
			ball_red.set_last_coordinates(c_l.x, c_l.y + 1);
			is_down = false;
		}
	}

	gv_p->batch_update();
	gv_p->batch_model();
}

void Scene_1::on_draw()
{
	gv_p->tile_draw();
	gv_p->batch_draw_object();
	gv_p->tile_draw_water();
	gv_p->batch_draw_component();
}
