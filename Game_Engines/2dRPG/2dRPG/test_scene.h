#ifndef _TEST_SCENE_H_
#define _TEST_SCENE_H_

#include "log.h"
#include "ball.h"
#include "scene.h"
#include "audio_set.h"
#include "rect_button.h"
#include "window_manager.h"

extern Log* log_p;

extern Camera test_scene_camera;
extern RectButton* rect_button_p;
extern Window* window_p;

extern Ball* big_ball;
extern Ball* small_ball;

extern AudioSet audioSet_test_songs;

extern Timer timer_test_song_play;
extern Timer timer_test_song_pause;
extern Timer timer_test_song_resume;
extern Timer timer_test_song_stop;
extern Timer timer_test_song_close;

class TestScene : public Scene
{
public:
	TestScene()
	{
		log_p->setTarget(LogTarget::Console);
		log_p->info("TestScene created.");
	}
	~TestScene() = default;

	void on_enter()
	{
		log_p->setTarget(LogTarget::Console);
		log_p->info("TestScene entering...");
		big_ball = new Ball();
		small_ball = new Ball();
		rect_button_p = new RectButton();

		test_scene_camera.set_coordinates(0, 0);
		gv_p->set_camera(&test_scene_camera);

		gv_p->register_object(big_ball);
		big_ball->set_radius(20);
		big_ball->set_fill_color(RGB(162, 101, 170));
		big_ball->set_coordinates(20, 20);
		big_ball->set_velocity({ 0.005,0.01 });

		small_ball->set_radius(10);
		small_ball->set_parent(big_ball);
		small_ball->set_coordinates(50, 0);
		small_ball->set_coordinates_depending(true);

		rect_button_p = new RectButton();

		gv_p->register_component(rect_button_p);
		rect_button_p->set_coordinates(750, 40);
		rect_button_p->set_monitor_square(80, 50);
		rect_button_p->set_fill_color(RGB(165, 165, 165));
		rect_button_p->set_line_color(RGB(153, 153, 153));
		rect_button_p->get_text().set_text_color(BLACK);
		rect_button_p->get_text().set_bk_mode(TRANSPARENT);
		rect_button_p->get_text().set_text_style(30, 15, _T("ËÎÌå"));
		rect_button_p->get_text().set_content(_T("ÍË³ö"));
		rect_button_p->set_callback([&]()
			{
				window_p->shutdown();
			});
		rect_button_p->set_effective(true);
		rect_button_p->set_visible(true);

		audioSet_test_songs.set_cur_path(_T("res/audio/"));
		audioSet_test_songs.load_from_file(_T("bgm_game.mp3"));

		timer_test_song_play.set_one_shot(true);
		timer_test_song_play.set_wait_time(3000);
		timer_test_song_play.set_callback([&]()
			{
				audioSet_test_songs.play_audio(_T("bgm_game"));
			}
		);

		log_p->setTarget(LogTarget::Console);
		log_p->info("TestScene entered.");
	}

	void on_input()
	{
		gv_p->batch_input();
	}

	void on_draw()
	{
		gv_p->batch_draw();
	}

	void on_update()
	{
		timer_test_song_play.on_update();
		if (big_ball) big_ball->rotate(0.09 * gv_p->get_delta());
		gv_p->batch_update();
	}

	void on_exit()
	{

	}


private:

};

#endif // !_TEST_SCENE_H_