#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#include "scene.h"

#include <vector>
#include <graphics.h>

class Scene;

class SceneManager
{
public:
	SceneManager();
	~SceneManager() = default;

	void set_cur_scene(Scene* scene_p);

	void on_input();

	void on_update();

	void on_draw();


private:
	Scene* cur_scene_p;							// 当前场景指针

};

#endif // !_SCENE_MANAGER_H_