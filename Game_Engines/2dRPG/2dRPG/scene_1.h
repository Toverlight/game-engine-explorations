#ifndef _SCENE_1_H_
#define _SCENE_1_H_

#include "scene.h"
#include "ball.h"
#include "block.h"
#include "layout.h"
#include "rect_button.h"
#include "tile_map.h"

#include "database_manager.h"

extern DatabaseManager* dbm_p;

class Scene_1 : public Scene
{
public:
	Scene_1();
	~Scene_1()
	{
		delete tile_map_p;
	}

	void on_enter();

	void on_input();

	void on_update();

	void on_draw();

private:
	TileMap* tile_map_p;					// 地图数据

	IMAGE img_bk;							// 背景图片

	Camera camera;							// 场景相机

	Layout panel;							// 面板
	RectButton rect_button1;				// 按钮1
	RectButton rect_button2;				// 按钮2
	RectButton rect_button3;				// 按钮3

	RectButton addv_right_btn;				// 加右速度
	RectButton addv_left_btn;				// 加左速度
	RectButton addv_up_btn;					// 加上速度

	bool is_panel_open;						// 是否打开面板

	Ball ball_red;							// 红球
	Ball ball_yellow;						// 黄球

	Block block_gray;						// 灰色矩形
	Block block_blue;						// 蓝色矩形

	bool is_left_moving;					// 向左移动，向左游
	bool is_right_moving;					// 向右移动，向右游
	bool is_jumping;						// 起跳，上游
	bool is_down;							// 下平台，潜水
};

#endif // !_SCENE_1_H_

