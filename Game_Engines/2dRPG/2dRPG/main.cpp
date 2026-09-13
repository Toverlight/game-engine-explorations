#include "log.h"
#include "tick.h"
#include "var_g.h"
#include "scene_1.h"
#include "test_scene.h"
#include "scene_manager.h"
#include "window_manager.h"
#include "tile_map_editor.h"
#include "database_manager.h"
#include "timer_precision_guard.h"

#include "test222.h"
#include "debug_scene.h"

#include <iostream>
#include <Windows.h>
#include <graphics.h>
#include <cstdlib>
#include <ctime>

#define WINDOW_W 800
#define WINDOW_H 600

Log* log_p;								// 存放日志指针
DatabaseManager* dbm_p;					// 存放数据库管理器指针

Window* window_p;

GVar* gv_p;								// 存放全局变量
Scene* test_scene_p;
Scene* scene_1_p;
Scene* tteditor_p;

Scene* test222;							// test222
Scene* debug_scene_p;					// debug_scene

Ball* big_ball;							// 大球
Ball* small_ball;						// 小球

Timer* timer_del_small_ball_p;			// 删除小球的测试计时器
// 全局的初始化
void init_g()
{
	// log, gvar init START (Above all)
	log_p = Log::getInstance();
	gv_p = GVar::get_instance();
	// log, gvar init END

	dbm_p = DatabaseManager::getInstance();
	dbm_p->loadTileMapId2WhatAndImages();

	test_scene_p = new TestScene();
	scene_1_p = new Scene_1();
	tteditor_p = new TileMapEditor();

	test222 = new Test222();
	debug_scene_p = new DebugScene();

	big_ball = new Ball();
	small_ball = new Ball();

	log_p->setTarget(LogTarget::Console);
	log_p->info(__func__, " finished.");
}

int main()
{
	std::srand(std::time(0));
	// 帧率相关
	TimerPrecisionGuard timer_precision_guard;			// 设置系统计时器精度为1ms（默认是15ms），程序退出后自动恢复

	const int FPS = 60;									// 设定帧率

	LARGE_INTEGER frequency;							// 频率获取（单位时间计数）（精度远高于GetTickCount64()）
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER frameStart, frameEnd;					// 帧开始和帧结束的计数
	double elapsedTime = 0.0;							// 帧用时（单位：秒）

	LARGE_INTEGER last_tick_time, current_tick_time;	// tick相关计数
	QueryPerformanceCounter(&last_tick_time);			// 初始化last_tick_time

	init_g();

	// 视窗与游戏运行
	window_p = WindowManager::make_one();
	window_p->set_size(WINDOW_W, WINDOW_H);
	window_p->set_show_console(true);
	window_p->set_background_color(RGB(210, 209, 209));
	window_p->run();
	// 指定初始场景
	gv_p->get_scene_manager()->set_cur_scene(debug_scene_p);

	BeginBatchDraw();
	while (window_p->check_running())
	{
		// 帧率控制-开始部分
		QueryPerformanceCounter(&frameStart);

		// 输入
		gv_p->peek_msg_with_while();
		gv_p->get_scene_manager()->on_input();
		gv_p->clear_msg_without_mousemove();

		// 更新
		QueryPerformanceCounter(&current_tick_time);
		int delta_time = (static_cast<double>(current_tick_time.QuadPart -
			last_tick_time.QuadPart) / frequency.QuadPart) * 1000;
		gv_p->set_delta(delta_time);
		gv_p->get_scene_manager()->on_update();
		last_tick_time = current_tick_time;

		// 批量清除非法挂载
		gv_p->batch_invalid_clear();

		// 渲染
		cleardevice();

		gv_p->get_scene_manager()->on_draw();

		FlushBatchDraw();

		// 帧率控制-结束部分
		QueryPerformanceCounter(&frameEnd);
			// 将时间转换成秒
		elapsedTime = static_cast<double>(frameEnd.QuadPart - frameStart.QuadPart) / frequency.QuadPart;

		double sleepTime = (1.0 / FPS) - elapsedTime;		
		
		if (sleepTime > 0){
			Sleep(static_cast<DWORD>(sleepTime * 1000));
		}

	}

	EndBatchDraw();

	window_p->close();
	WindowManager::del_if_one();

	return 0;
}