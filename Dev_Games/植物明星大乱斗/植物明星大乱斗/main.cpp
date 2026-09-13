#include "util.h"
#include "atlas.h"
#include "scene.h"
#include "bullet.h"
#include "player.h"
#include "platform.h"
#include "menu_scene.h"
#include "game_scene.h"
#include "selector_scene.h"
#include "scene_manager.h"

#include <graphics.h>

#pragma comment(lib,"Winmm.lib")

bool is_debug = false;										//是否开启调试模式

IMAGE img_menu_background;									//主菜单背景图片

void flip_atlas(Atlas& src, Atlas& dst)
{
	dst.clear();
	for (int i = 0; i < src.get_size(); ++i)
	{
		IMAGE img_flipped;
		flip_image(src.get_image(i), &img_flipped);
		dst.add_image(img_flipped);
	}
}


IMAGE img_VS;												//VS艺术字图片
IMAGE img_1P;												//1P文本图片
IMAGE img_2P;												//2P文本图片
IMAGE img_1P_desc;											//1P键位描述图片
IMAGE img_2P_desc;											//2P键位描述图片
IMAGE img_gravestone_left;									//朝向左的墓碑图片
IMAGE img_gravestone_right;									//朝向右的墓碑图片
IMAGE img_selector_tip;										//选角界面显示信息文本的图片
IMAGE img_selector_background;								//选角界面背景图
IMAGE img_1P_selector_btn_idle_left;						//1P向左选择按钮默认状态图片
IMAGE img_1P_selector_btn_idle_right;						//1P向右选择按钮默认状态图片
IMAGE img_1P_selector_btn_down_left;						//1P向左选择按钮按下状态图片
IMAGE img_1P_selector_btn_down_right;						//1P向右选择按钮按下状态图片
IMAGE img_2P_selector_btn_idle_left;						//2P向左选择按钮默认状态图片
IMAGE img_2P_selector_btn_idle_right;						//2P向右选择按钮默认状态图片
IMAGE img_2P_selector_btn_down_left;						//2P向左选择按钮按下状态图片
IMAGE img_2P_selector_btn_down_right;						//2P向右选择按钮按下状态图片
IMAGE img_peashooter_selector_background_left;				//选角界面朝向左的婉逗射手背景图片
IMAGE img_peashooter_selector_background_right;				//选角界面朝向右的婉逗射手背景图片
IMAGE img_sunflower_selector_background_left;				//选角界面朝向左的龙日葵背景图片
IMAGE img_sunflower_selector_background_right;				//选角界面朝向右的龙日葵背景图片

IMAGE img_sky;												//天空图片
IMAGE img_hills;											//山脉图片
IMAGE img_platform_large;									//大型平台图片
IMAGE img_platform_small;									//小型平台图片

IMAGE img_1P_cursor;										//1P指示光标图片
IMAGE img_2P_cursor;										//2P指示光标图片

Atlas atlas_peashooter_idle_left;							//婉逗射手朝向左的默认动画图集
Atlas atlas_peashooter_idle_right;							//婉逗射手朝向右的默认动画图集
Atlas atlas_peashooter_run_left;							//婉逗射手朝向左的奔跑动画图集
Atlas atlas_peashooter_run_right;							//婉逗射手朝向右的奔跑动画图集
Atlas atlas_peashooter_attack_ex_left;						//婉逗射手朝向左的特殊攻击动画图集
Atlas atlas_peashooter_attack_ex_right;						//婉逗射手朝向右的特殊攻击动画图集
Atlas atlas_peashooter_die_left;							//婉逗射手朝向左的死亡动画图集
Atlas atlas_peashooter_die_right;							//婉逗射手朝向右的死亡动画图集

Atlas atlas_sunflower_idle_left;							//龙日葵朝向左的默认动画图集
Atlas atlas_sunflower_idle_right;							//龙日葵朝向右的默认动画图集
Atlas atlas_sunflower_run_left;								//龙日葵朝向左的奔跑动画图集
Atlas atlas_sunflower_run_right;							//龙日葵朝向右的奔跑动画图集
Atlas atlas_sunflower_attack_ex_left;						//龙日葵朝向左的特殊攻击动画图集
Atlas atlas_sunflower_attack_ex_right;						//龙日葵朝向右的特殊攻击动画图集
Atlas atlas_sunflower_die_left;								//龙日葵朝向左的死亡动画图集
Atlas atlas_sunflower_die_right;							//龙日葵朝向右的死亡动画图集

IMAGE img_pea;												//豌豆图片
Atlas atlas_pea_break;										//豌豆破碎动画图集
Atlas atlas_sun;											//日光动画图集
Atlas atlas_sun_explode;									//日光爆炸动画图集
Atlas atlas_sun_ex;											//特殊日光动画图集
Atlas atlas_sun_ex_explode;									//特殊日光爆炸动画图集
Atlas atlas_sun_text;										//“日”文本动画图集

Atlas atlas_run_effect;										//奔跑特效动画图集
Atlas atlas_jump_effect;									//跳跃特效动画图集
Atlas atlas_land_effect;									//落地特效动画图集

IMAGE img_1P_winner;										//1P获胜文本图片
IMAGE img_2P_winner;										//2P获胜文本图片
IMAGE img_winner_bar;										//获胜玩家文本背景图片

IMAGE img_avatar_peashooter;								//婉逗射手头像图片
IMAGE img_avatar_sunflower;									//龙日葵头像图片

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void load_game_resources()
{
	AddFontResourceEx(_T("res/IPix.ttf"),FR_PRIVATE,NULL);

	loadimage(&img_menu_background, _T("res/menu_background.png"));

	loadimage(&img_VS, _T("res/VS.png"));
	loadimage(&img_1P, _T("res/1P.png"));
	loadimage(&img_2P, _T("res/2P.png"));
	loadimage(&img_1P_desc, _T("res/1P_desc.png"));
	loadimage(&img_2P_desc, _T("res/2P_desc.png"));
	loadimage(&img_gravestone_right, _T("res/gravestone.png"));
	flip_image(&img_gravestone_right, &img_gravestone_left);
	loadimage(&img_selector_tip, _T("res/selector_tip.png"));
	loadimage(&img_selector_background, _T("res/selector_background.png"));
	loadimage(&img_1P_selector_btn_idle_right, _T("res/1P_selector_btn_idle.png"));
	flip_image(&img_1P_selector_btn_idle_right, &img_1P_selector_btn_idle_left);
	loadimage(&img_1P_selector_btn_down_right, _T("res/1P_selector_btn_down.png"));
	flip_image(&img_1P_selector_btn_down_right, &img_1P_selector_btn_down_left);
	loadimage(&img_2P_selector_btn_idle_right, _T("res/2P_selector_btn_idle.png"));
	flip_image(&img_2P_selector_btn_idle_right, &img_2P_selector_btn_idle_left);
	loadimage(&img_2P_selector_btn_down_right, _T("res/2P_selector_btn_down.png"));
	flip_image(&img_2P_selector_btn_down_right, &img_2P_selector_btn_down_left);
	loadimage(&img_peashooter_selector_background_right, _T("res/peashooter_selector_background.png"));
	flip_image(&img_peashooter_selector_background_right, &img_peashooter_selector_background_left);
	loadimage(&img_sunflower_selector_background_right, _T("res/sunflower_selector_background.png"));
	flip_image(&img_sunflower_selector_background_right, &img_sunflower_selector_background_left);

	loadimage(&img_sky, _T("res/sky.png"));
	loadimage(&img_hills, _T("res/hills.png"));
	loadimage(&img_platform_large, _T("res/platform_large.png"));
	loadimage(&img_platform_small, _T("res/platform_small.png"));

	loadimage(&img_1P_cursor, _T("res/1P_cursor.png"));
	loadimage(&img_2P_cursor, _T("res/2P_cursor.png"));

	atlas_peashooter_idle_right.load_from_file(_T("res/peashooter_idle_%d.png"), 9);
	flip_atlas(atlas_peashooter_idle_right, atlas_peashooter_idle_left);
	atlas_peashooter_run_right.load_from_file(_T("res/peashooter_run_%d.png"), 5);
	flip_atlas(atlas_peashooter_run_right, atlas_peashooter_run_left);
	atlas_peashooter_attack_ex_right.load_from_file(_T("res/peashooter_attack_ex_%d.png"), 3);
	flip_atlas(atlas_peashooter_attack_ex_right, atlas_peashooter_attack_ex_left);
	atlas_peashooter_die_right.load_from_file(_T("res/peashooter_die_%d.png"), 4);
	flip_atlas(atlas_peashooter_die_right, atlas_peashooter_die_left);

	atlas_sunflower_idle_right.load_from_file(_T("res/sunflower_idle_%d.png"), 8);
	flip_atlas(atlas_sunflower_idle_right, atlas_sunflower_idle_left);
	atlas_sunflower_run_right.load_from_file(_T("res/sunflower_run_%d.png"), 5);
	flip_atlas(atlas_sunflower_run_right, atlas_sunflower_run_left);
	atlas_sunflower_attack_ex_right.load_from_file(_T("res/sunflower_attack_ex_%d.png"), 9);
	flip_atlas(atlas_sunflower_attack_ex_right, atlas_sunflower_attack_ex_left);
	atlas_sunflower_die_right.load_from_file(_T("res/sunflower_die_%d.png"), 2);
	flip_atlas(atlas_sunflower_die_right, atlas_sunflower_die_left);

	loadimage(&img_pea, _T("res/pea.png"));
	atlas_pea_break.load_from_file(_T("res/pea_break_%d.png"), 3);
	atlas_sun.load_from_file(_T("res/sun_%d.png"), 5);
	atlas_sun_explode.load_from_file(_T("res/sun_explode_%d.png"), 5);
	atlas_sun_ex.load_from_file(_T("res/sun_ex_%d.png"), 5);
	atlas_sun_ex_explode.load_from_file(_T("res/sun_ex_explode_%d.png"), 5);
	atlas_sun_text.load_from_file(_T("res/sun_text_%d.png"), 6);

	atlas_run_effect.load_from_file(_T("res/run_effect_%d.png"), 4);
	atlas_jump_effect.load_from_file(_T("res/jump_effect_%d.png"), 5);
	atlas_land_effect.load_from_file(_T("res/jump_land_%d.png"), 2);

	loadimage(&img_1P_winner, _T("res/1P_winner.png"));
	loadimage(&img_2P_winner, _T("res/2P_winner.png"));
	loadimage(&img_winner_bar, _T("res/winner_bar.png"));

	loadimage(&img_avatar_peashooter, _T("res/avatar_peashooter.png"));
	loadimage(&img_avatar_sunflower, _T("res/avatar_sunflower.png"));

	mciSendString(_T("open res/bgm_game.mp3 alias bgm_game"), NULL, 0, NULL);
	mciSendString(_T("open res/bgm_menu.mp3 alias bgm_menu"), NULL, 0, NULL);
	mciSendString(_T("open res/pea_break_1.mp3 alias pea_break_1"), NULL, 0, NULL);
	mciSendString(_T("open res/pea_break_2.mp3 alias pea_break_2"), NULL, 0, NULL);
	mciSendString(_T("open res/pea_break_3.mp3 alias pea_break_3"), NULL, 0, NULL);
	mciSendString(_T("open res/pea_shoot_1.mp3 alias pea_shoot_1"), NULL, 0, NULL);
	mciSendString(_T("open res/pea_shoot_2.mp3 alias pea_shoot_2"), NULL, 0, NULL);
	mciSendString(_T("open res/pea_shoot_ex.mp3 alias pea_shoot_ex"), NULL, 0, NULL);
	mciSendString(_T("open res/sun_explode.mp3 alias sun_explode"), NULL, 0, NULL);
	mciSendString(_T("open res/sun_explode_ex.mp3 alias sun_explode_ex"), NULL, 0, NULL);
	mciSendString(_T("open res/sun_text.mp3 alias sun_text"), NULL, 0, NULL);
	mciSendString(_T("open res/ui_confirm.wav alias ui_confirm"), NULL, 0, NULL);
	mciSendString(_T("open res/ui_switch.wav alias ui_switch"), NULL, 0, NULL);
	mciSendString(_T("open res/ui_win.wav alias ui_win"), NULL, 0, NULL);

}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

Scene* menu_scene = nullptr;
Scene* game_scene = nullptr;
Scene* selector_scene = nullptr;

Camera main_camera;
SceneManager scene_manager;

std::vector<Bullet*> bullet_list;
std::vector<Platform> platform_list;

Player* player_1 = nullptr;
Player* player_2 = nullptr;

IMAGE* img_player_1_avatar = nullptr;
IMAGE* img_player_2_avatar = nullptr;

int main() {
	ExMessage msg;
	const int FPS = 60;

	load_game_resources();

	initgraph(1280, 720,EX_SHOWCONSOLE);

	settextstyle(28, 0, _T("IPix"));
	setbkmode(TRANSPARENT);

	BeginBatchDraw();

	menu_scene = new MenuScene();
	game_scene = new GameScene();
	selector_scene = new SelectorScene();

	scene_manager.set_current_scene(menu_scene);

	while (true) {
		DWORD frame_start_time = GetTickCount();

		while (peekmessage(&msg)) {
			scene_manager.on_input(msg);
		}

		static DWORD last_tick_time = GetTickCount();
		DWORD current_tick_time = GetTickCount();
		DWORD delta_tick = current_tick_time - last_tick_time;
		std::cout << "delta_tick = " << delta_tick << std::endl;
		scene_manager.on_update(delta_tick);
		last_tick_time = current_tick_time;

		cleardevice();
		scene_manager.on_draw(main_camera);
		FlushBatchDraw();

		DWORD frame_end_time = GetTickCount();
		DWORD frame_delta_time = frame_end_time - frame_start_time;
		if (frame_delta_time < 1000 / FPS) {
			Sleep(1000 / FPS - frame_delta_time);
		}
	}

	EndBatchDraw();


	return 0;
}