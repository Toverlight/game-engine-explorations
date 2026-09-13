#ifndef DEBUG_SCENE_H
#define DEBUG_SCENE_H

#include "util.h"
#include "scene.h"

class DebugScene : public Scene
{
public:
	DebugScene()
	{
		img_terrain_p = new IMAGE;
	}
	~DebugScene()
	{
		delete img_terrain_p;
	}

	void on_enter() override;

	void on_input() override;

	void on_update() override;

	void on_draw() override;

private:
	const int blockLen = 20;
	const int gridLen = 400;

	int gridNumX = 4;
	int gridNumY = 3;
	
	std::vector<std::vector<Vector2>> gradient;
	std::vector<std::vector<float>> elevations;
	std::vector<std::vector<RGB_T>> colors;
	
	Vector2 mmNC;							// 鼠标位置在elevations索引上的映射

	Camera camera;

	IMAGE* img_terrain_p;
};

#endif // !DEBUG_SCENE_H

