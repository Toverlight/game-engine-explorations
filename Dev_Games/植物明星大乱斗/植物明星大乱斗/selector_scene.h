#ifndef _SELECTOR_SCENE_H_
#define _SELECTOR_SCENE_H_

#include "scene.h"
#include "animation.h"
#include "player_id.h"
#include "scene_manager.h"
#include "peashooter_player.h"
#include "sunflower_player.h"

extern Player* player_1;
extern Player* player_2;

extern IMAGE img_VS;												//VS艺术字图片
extern IMAGE img_1P;												//1P文本图片
extern IMAGE img_2P;												//2P文本图片
extern IMAGE img_1P_desc;											//1P键位描述图片
extern IMAGE img_2P_desc;											//2P键位描述图片
extern IMAGE img_gravestone_left;									//朝向左的墓碑图片
extern IMAGE img_gravestone_right;									//朝向右的墓碑图片
extern IMAGE img_selector_tip;										//选角界面显示信息文本的图片
extern IMAGE img_selector_background;								//选角界面背景图
extern IMAGE img_1P_selector_btn_idle_left;							//1P向左选择按钮默认状态图片
extern IMAGE img_1P_selector_btn_idle_right;						//1P向右选择按钮默认状态图片
extern IMAGE img_1P_selector_btn_down_left;							//1P向左选择按钮按下状态图片
extern IMAGE img_1P_selector_btn_down_right;						//1P向右选择按钮按下状态图片
extern IMAGE img_2P_selector_btn_idle_left;							//2P向左选择按钮默认状态图片
extern IMAGE img_2P_selector_btn_idle_right;						//2P向右选择按钮默认状态图片
extern IMAGE img_2P_selector_btn_down_left;							//2P向左选择按钮按下状态图片
extern IMAGE img_2P_selector_btn_down_right;						//2P向右选择按钮按下状态图片
extern IMAGE img_peashooter_selector_background_left;				//选角界面朝向左的婉逗射手背景图片
extern IMAGE img_peashooter_selector_background_right;				//选角界面朝向右的婉逗射手背景图片
extern IMAGE img_sunflower_selector_background_left;				//选角界面朝向左的龙日葵背景图片
extern IMAGE img_sunflower_selector_background_right;				//选角界面朝向右的龙日葵背景图片

extern IMAGE img_sky;												//天空图片
extern IMAGE img_hills;												//山脉图片
extern IMAGE img_platform_large;									//大型平台图片
extern IMAGE img_platform_small;									//小型平台图片

extern IMAGE img_1P_cursor;											//1P指示光标图片
extern IMAGE img_2P_cursor;											//2P指示光标图片

extern Atlas atlas_peashooter_idle_left;							//婉逗射手朝向左的默认动画图集
extern Atlas atlas_peashooter_idle_right;							//婉逗射手朝向右的默认动画图集
extern Atlas atlas_peashooter_run_left;								//婉逗射手朝向左的奔跑动画图集
extern Atlas atlas_peashooter_run_right;							//婉逗射手朝向右的奔跑动画图集
extern Atlas atlas_peashooter_attack_ex_left;						//婉逗射手朝向左的特殊攻击动画图集
extern Atlas atlas_peashooter_attack_ex_right;						//婉逗射手朝向右的特殊攻击动画图集
extern Atlas atlas_peashooter_die_left;								//婉逗射手朝向左的死亡动画图集
extern Atlas atlas_peashooter_die_right;							//婉逗射手朝向右的死亡动画图集

extern Atlas atlas_sunflower_idle_left;								//龙日葵朝向左的默认动画图集
extern Atlas atlas_sunflower_idle_right;							//龙日葵朝向右的默认动画图集
extern Atlas atlas_sunflower_run_left;								//龙日葵朝向左的奔跑动画图集
extern Atlas atlas_sunflower_run_right;								//龙日葵朝向右的奔跑动画图集
extern Atlas atlas_sunflower_attack_ex_left;						//龙日葵朝向左的特殊攻击动画图集
extern Atlas atlas_sunflower_attack_ex_right;						//龙日葵朝向右的特殊攻击动画图集
extern Atlas atlas_sunflower_die_left;								//龙日葵朝向左的死亡动画图集
extern Atlas atlas_sunflower_die_right;								//龙日葵朝向右的死亡动画图集

extern IMAGE img_pea;												//豌豆图片
extern Atlas atlas_pea_break;										//豌豆破碎动画图集
extern Atlas atlas_sun;												//日光动画图集
extern Atlas atlas_sun_explode;										//日光爆炸动画图集
extern Atlas atlas_sun_ex;											//特殊日光动画图集
extern Atlas atlas_sun_ex_explode;									//特殊日光爆炸动画图集
extern Atlas atlas_sun_text;										//“日”文本动画图集

extern Atlas atlas_run_effect;										//奔跑特效动画图集
extern Atlas atlas_jump_effect;										//跳跃特效动画图集
extern Atlas atlas_land_effect;										//落地特效动画图集

extern IMAGE img_1P_winner;											//1P获胜文本图片
extern IMAGE img_2P_winner;											//2P获胜文本图片
extern IMAGE img_winner_bar;										//获胜玩家文本背景图片

extern IMAGE img_avatar_peashooter;									//婉逗射手头像图片
extern IMAGE img_avatar_sunflower;									//龙日葵头像图片

extern IMAGE* img_player_1_avatar;
extern IMAGE* img_player_2_avatar;

extern SceneManager scene_manager;

class SelectorScene : public Scene
{
public:
	SelectorScene() = default;
	~SelectorScene() = default;

	void on_enter()
	{
		animation_peashooter.set_atlas(&atlas_peashooter_idle_right);
		animation_sunflower.set_atlas(&atlas_sunflower_idle_right);
		animation_peashooter.set_interval(100);
		animation_sunflower.set_interval(100);

		static const int OFFSET_X = 50;

		pos_img_VS.x = (getwidth() - img_VS.getwidth()) / 2;
		pos_img_VS.y = (getheight() - img_VS.getheight()) / 2;
		pos_img_tip.x = (getwidth() - img_selector_tip.getwidth()) / 2;
		pos_img_tip.y = getheight() - 125;
		pos_img_1P.x = (getwidth() / 2 - img_1P.getwidth()) / 2 - OFFSET_X;
		pos_img_1P.y = 35;
		pos_img_2P.x = getwidth() / 2 + (getwidth() / 2 - img_2P.getwidth()) / 2 + OFFSET_X;
		pos_img_2P.y = pos_img_1P.y;
		pos_img_1P_desc.x = (getwidth() / 2 - img_1P_desc.getwidth()) / 2 - OFFSET_X;
		pos_img_1P_desc.y = getheight() - 150;
		pos_img_2P_desc.x = getwidth() / 2 + (getwidth() / 2 - img_2P_desc.getwidth()) / 2 + OFFSET_X;
		pos_img_2P_desc.y = pos_img_1P_desc.y;
		pos_img_1P_gravestone.x = (getwidth() / 2 - img_gravestone_right.getwidth()) / 2 - OFFSET_X;
		pos_img_1P_gravestone.y = pos_img_1P.y + img_1P.getheight() + 35;
		pos_img_2P_gravestone.x = getwidth() / 2 + (getwidth() / 2 - img_gravestone_left.getwidth()) / 2 + OFFSET_X;
		pos_img_2P_gravestone.y = pos_img_1P_gravestone.y;
		pos_animation_1P.x = (getwidth() / 2 - atlas_peashooter_idle_right.get_image(0)->getwidth()) / 2 - OFFSET_X;
		pos_animation_1P.y = pos_img_1P_gravestone.y + 80;
		pos_animation_2P.x = getwidth() / 2 + (getwidth() / 2 - atlas_peashooter_idle_right.get_image(0)->getwidth()) / 2 + OFFSET_X;
		pos_animation_2P.y = pos_animation_1P.y;
		pos_img_1P_name.y = pos_animation_1P.y + 155;
		pos_img_2P_name.y = pos_img_1P_name.y;
		pos_1P_selector_btn_left.x = pos_img_1P_gravestone.x - img_1P_selector_btn_idle_left.getwidth();
		pos_1P_selector_btn_left.y = pos_img_1P_gravestone.y + (img_gravestone_right.getheight() - img_1P_selector_btn_idle_left.getheight()) / 2;
		pos_1P_selector_btn_right.x = pos_img_1P_gravestone.x + img_gravestone_right.getwidth();
		pos_1P_selector_btn_right.y = pos_1P_selector_btn_left.y;
		pos_2P_selector_btn_left.x = pos_img_2P_gravestone.x - img_2P_selector_btn_idle_left.getwidth();
		pos_2P_selector_btn_left.y = pos_1P_selector_btn_left.y;
		pos_2P_selector_btn_right.x = pos_img_2P_gravestone.x + img_gravestone_left.getwidth();
		pos_2P_selector_btn_right.y = pos_1P_selector_btn_left.y;

	}

	void on_input(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
				// 'A'
			case 0x41:
				is_btn_1P_left_down = true;
				break;
				// 'D'
			case 0x44:
				is_btn_1P_right_down = true;
				break;
				// '←'
			case VK_LEFT:
				is_btn_2P_left_down = true;
				break;
				// '→'
			case VK_RIGHT:
				is_btn_2P_right_down = true;
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.vkcode)
			{
				// 'A'
			case 0x41:
				is_btn_1P_left_down = false;
				player_type_1 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_1 - 1) % (int)PlayerType::Invalid);
				mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);
				break;
				// 'D'
			case 0x44:
				is_btn_1P_right_down = false;
				player_type_1 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_1 + 1) % (int)PlayerType::Invalid);
				mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);
				break;
				// '←'
			case VK_LEFT:
				is_btn_2P_left_down = false;
				player_type_2 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_2 - 1) % (int)PlayerType::Invalid);
				mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);
				break;
				// '→'
			case VK_RIGHT:
				is_btn_2P_right_down = false;
				player_type_2 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_2 + 1) % (int)PlayerType::Invalid);
				mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);
				break;
			case VK_RETURN:
				scene_manager.switch_to(SceneManager::SceneType::Game);
				mciSendString(_T("play ui_confirm from 0"), NULL, 0, NULL);
			}
			break;
		default:
			break;
		}
	}

	void on_update(int delta)
	{
		animation_peashooter.on_update(delta);
		animation_sunflower.on_update(delta);

		selector_background_scroll_offset_x += 5;
		if (selector_background_scroll_offset_x >= img_peashooter_selector_background_left.getwidth())
			selector_background_scroll_offset_x = 0;
	}

	void on_draw(const Camera& camera)
	{
		IMAGE* img_p1_selector_background = nullptr;
		IMAGE* img_p2_selector_background = nullptr;

		switch (player_type_2)
		{
		case PlayerType::Peashooter:
			img_p1_selector_background = &img_peashooter_selector_background_right;
			break;
		case PlayerType::Sunflower:
			img_p1_selector_background = &img_sunflower_selector_background_right;
			break;
		default:
			img_p1_selector_background = &img_peashooter_selector_background_right;
			break;
		}

		switch (player_type_1)
		{
		case PlayerType::Peashooter:
			img_p2_selector_background = &img_peashooter_selector_background_left;
			break;
		case PlayerType::Sunflower:
			img_p2_selector_background = &img_sunflower_selector_background_left;
			break;
		default:
			img_p2_selector_background = &img_peashooter_selector_background_left;
			break;
		}

		putimage(0, 0, &img_selector_background);

		putimage_alpha(selector_background_scroll_offset_x - img_p1_selector_background->getwidth(), 0, img_p1_selector_background);
		putimage_alpha(selector_background_scroll_offset_x, 0,
			img_p1_selector_background->getwidth() - selector_background_scroll_offset_x, 0, img_p1_selector_background, 0, 0);
		putimage_alpha(getwidth() - img_p2_selector_background->getwidth(), 0, img_p2_selector_background->getwidth() - 
			selector_background_scroll_offset_x, 0, img_p2_selector_background, selector_background_scroll_offset_x, 0);
		putimage_alpha(getwidth() - selector_background_scroll_offset_x, 0, img_p2_selector_background);

		putimage_alpha(pos_img_VS.x, pos_img_VS.y, &img_VS);

		putimage_alpha(pos_img_1P.x, pos_img_1P.y, &img_1P);
		putimage_alpha(pos_img_2P.x, pos_img_2P.y, &img_2P);
		putimage_alpha(pos_img_1P_gravestone.x, pos_img_1P_gravestone.y, &img_gravestone_right);
		putimage_alpha(pos_img_2P_gravestone.x, pos_img_2P_gravestone.y, &img_gravestone_left);

		switch (player_type_1)
		{
		case PlayerType::Peashooter:
			animation_peashooter.on_draw(camera, pos_animation_1P.x, pos_animation_1P.y);
			pos_img_1P_name.x = pos_img_1P_gravestone.x + 
				(img_gravestone_right.getwidth() - textwidth(str_peashooter_name)) / 2;
			outtextxy_shaded(pos_img_1P_name.x, pos_img_1P_name.y, str_peashooter_name);
			break;
		case PlayerType::Sunflower:
			animation_sunflower.on_draw(camera, pos_animation_1P.x, pos_animation_1P.y);
			pos_img_1P_name.x = pos_img_1P_gravestone.x +
				(img_gravestone_right.getwidth() - textwidth(str_sunflower_name)) / 2;
			outtextxy_shaded(pos_img_1P_name.x, pos_img_1P_name.y, str_sunflower_name);
			break;
		}

		switch (player_type_2)
		{
		case PlayerType::Peashooter:
			animation_peashooter.on_draw(camera, pos_animation_2P.x, pos_animation_2P.y);
			pos_img_2P_name.x = pos_img_2P_gravestone.x +
				(img_gravestone_left.getwidth() - textwidth(str_peashooter_name)) / 2;
			outtextxy_shaded(pos_img_2P_name.x, pos_img_2P_name.y, str_peashooter_name);
			break;
		case PlayerType::Sunflower:
			animation_sunflower.on_draw(camera, pos_animation_2P.x, pos_animation_2P.y);
			pos_img_2P_name.x = pos_img_2P_gravestone.x +
				(img_gravestone_left.getwidth() - textwidth(str_sunflower_name)) / 2;
			outtextxy_shaded(pos_img_2P_name.x, pos_img_2P_name.y, str_sunflower_name);
			break;
		}

		putimage_alpha(pos_1P_selector_btn_left.x, pos_1P_selector_btn_left.y,
			is_btn_1P_left_down ? &img_1P_selector_btn_down_left : &img_1P_selector_btn_idle_left);
		putimage_alpha(pos_1P_selector_btn_right.x, pos_1P_selector_btn_right.y,
			is_btn_1P_right_down ? &img_1P_selector_btn_down_right : &img_1P_selector_btn_idle_right);
		putimage_alpha(pos_2P_selector_btn_left.x, pos_2P_selector_btn_left.y,
			is_btn_2P_left_down ? &img_2P_selector_btn_down_left : &img_2P_selector_btn_idle_left);
		putimage_alpha(pos_2P_selector_btn_right.x, pos_2P_selector_btn_right.y,
			is_btn_2P_right_down ? &img_2P_selector_btn_down_right : &img_2P_selector_btn_idle_right);

		putimage_alpha(pos_img_1P_desc.x, pos_img_1P_desc.y, &img_1P_desc);
		putimage_alpha(pos_img_2P_desc.x, pos_img_2P_desc.y, &img_2P_desc);

		putimage_alpha(pos_img_tip.x, pos_img_tip.y, &img_selector_tip);
	}

	void on_exit()
	{
		switch (player_type_1)
		{
		case PlayerType::Peashooter:
			player_1 = new PeashooterPlayer();
			img_player_1_avatar = &img_avatar_peashooter;
			break;
		case PlayerType::Sunflower:
			player_1 = new SunflowerPlayer();
			img_player_1_avatar = &img_avatar_sunflower;
			break;
		}
		player_1->set_id(PlayerID::P1);

		switch (player_type_2)
		{
		case PlayerType::Peashooter:
			player_2 = new PeashooterPlayer();
			img_player_2_avatar = &img_avatar_peashooter;
			break;
		case PlayerType::Sunflower:
			player_2 = new SunflowerPlayer();
			img_player_2_avatar = &img_avatar_sunflower;
			break;
		}
		player_2->set_id(PlayerID::P2);

	}

private:
	enum class PlayerType
	{
		Peashooter = 0,
		Sunflower,
		Invalid
	};

private:
	POINT pos_img_VS = { 0 };										//VS 艺术字图片位置
	POINT pos_img_tip = { 0 };										//提示信息文本图片位置
	POINT pos_img_1P = { 0 };										//1P 文本图片位置
	POINT pos_img_2P = { 0 };										//2P 文本图片位置
	POINT pos_img_1P_desc = { 0 };									//1P 键位描述图片位置
	POINT pos_img_2P_desc = { 0 };									//2P 键位描述图片位置
	POINT pos_img_1P_name = { 0 };									//1P 角色姓名文本位置
	POINT pos_img_2P_name = { 0 };									//2P 角色姓名文本位置
	POINT pos_animation_1P = { 0 };									//1P 角色动画位置
	POINT pos_animation_2P = { 0 };									//2P 角色动画位置
	POINT pos_img_1P_gravestone = { 0 };							//1P 墓碑图片位置
	POINT pos_img_2P_gravestone = { 0 };							//2P 墓碑图片位置
	POINT pos_1P_selector_btn_left = { 0 };							//1P 向左切换按钮位置
	POINT pos_1P_selector_btn_right = { 0 };						//1P 向右切换按钮位置
	POINT pos_2P_selector_btn_left = { 0 };							//2P 向左切换按钮位置
	POINT pos_2P_selector_btn_right = { 0 };						//2P 向右切换按钮位置

	Animation animation_peashooter;									//婉逗射手动画
	Animation animation_sunflower;									//龙日葵动画

	PlayerType player_type_1 = PlayerType::Peashooter;				//1P 角色类型
	PlayerType player_type_2 = PlayerType::Sunflower;				//2P 角色类型

	LPCTSTR str_peashooter_name = _T("婉逗射手");					// 婉逗射手角色名
	LPCTSTR str_sunflower_name = _T("龙日葵");						// 龙日葵角色名

	int selector_background_scroll_offset_x = 0;					// 背景板滚动距离

	bool is_btn_1P_left_down = false;								// 1P 向左切换按钮是否按下
	bool is_btn_1P_right_down = false;								// 1P 向右切换按钮是否按下
	bool is_btn_2P_left_down = false;								// 2P 向左切换按钮是否按下
	bool is_btn_2P_right_down = false;								// 2P 向右切换按钮是否按下

private:
	void outtextxy_shaded(int x, int y, LPCTSTR str)
	{
		settextcolor(RGB(45, 45, 45));
		outtextxy(x + 3, y + 3, str);
		settextcolor(RGB(255, 255, 255));
		outtextxy(x, y, str);
	}
};

#endif //!_SELECTOR_SCENE_H_
