#include "scene_manager.h"

SceneManager::SceneManager()
{
	cur_scene_p = nullptr;
}

void SceneManager::set_cur_scene(Scene* scene_p)
{
	if (cur_scene_p)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->info("SceneManager::", __func__, " begins to exit cur_scene...");
		cur_scene_p->on_exit();
		log_p->setTarget(LogTarget::Console);
		log_p->info("Exited cur_scene.");
	}

	if (cur_scene_p = scene_p)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->info("SceneManager::", __func__, " begins to enter cur_scene...");
		cur_scene_p->on_enter();
		log_p->setTarget(LogTarget::Console);
		log_p->info("Entered cur_scene.");
	}
	else
	{
		log_p->setTarget(LogTarget::Console);
		log_p->warn("SceneManager::cur_scene_p failed to be set!");
	}
}

void SceneManager::on_input()
{
	if (cur_scene_p)
		cur_scene_p->on_input();

}

void SceneManager::on_update()
{
	if (cur_scene_p)
		cur_scene_p->on_update();
}

void SceneManager::on_draw()
{
	if (cur_scene_p)
	{
		cur_scene_p->background_draw();
		cur_scene_p->on_draw();
	}
}
