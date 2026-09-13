#include "debug_scene.h"
#include "perlin_noise.h"

using namespace PerlinNoise;

void DebugScene::on_enter()
{
	mmNC = { 0,0 };

	gv_p->use_key_state();

	camera.set_coordinates({ 0,0 });
	camera.set_velocity({ 0,0 });
	gv_p->set_camera(&camera);

	gradient = generateGradient(gridNumX * 8 + 1,gridNumY * 8 + 1);

	float min = 100000, max = -100000;

	int elevationNumY = gridNumY * gridLen / blockLen;
	int elevationNumX = gridNumX * gridLen / blockLen;
	elevations.resize(elevationNumY);
	colors.resize(elevationNumY);
	for (int i = 0; i < elevationNumY; i++)
	{
		elevations[i].resize(elevationNumX);
		colors[i].resize(elevationNumX);
	}

	for (int i = 0; i < elevationNumY; i++)
	{
		for (int j = 0; j < elevationNumX; j++)
		{
			float a = (octavePerlinNoise((j + 0.5) * blockLen, (i + 0.5) * blockLen, gridLen, gradient, 4, 0.5) + 1) / 2 * 1900 - 800;
			elevations[i][j] = a;
			if (a < min) min = a;
			if (a > max) max = a;
		}
	}
	std::cout << "PerlinNoise: min = " << min << " m, max = " << max << " m." << std::endl;

	erodeTerrain(elevations, 4, 0.2f, 100, 700);

	transformElevationsToRGB(colors, elevations);
	drawColorfulBlocksToImage(img_terrain_p, colors, blockLen);
}

void DebugScene::on_input()
{
	bool* const keyState = gv_p->get_keyState();
	if (keyState['E'])	// 侵蚀一遍
	{
		erodeTerrain(elevations, 100, 0.3f, 100, 700);
		transformElevationsToRGB(colors, elevations);
		drawColorfulBlocksToImage(img_terrain_p, colors, blockLen);
	}
	if (keyState['P'])	// 重新生成
	{
		gradient = generateGradient(gridNumX * 32 + 1, gridNumY * 32 + 1);
		float min = 10000, max = -10000;
		for (int i = 0; i < elevations.size(); i++)
		{
			for (int j = 0; j < elevations[0].size(); j++)
			{
				float a =
					(octavePerlinNoise((j + 0.5) * blockLen, (i + 0.5) * blockLen, gridLen, gradient,
						6, 0.5) + 1) / 2 * 2100 - 900;
				elevations[i][j] = a;
				if (a < min) min = a;
				if (a > max) max = a;
			}
		}
		std::cout << "PerlinNoise: min = " << min << " m, max = " << max << " m." << std::endl;
		transformElevationsToRGB(colors, elevations);
		drawColorfulBlocksToImage(img_terrain_p, colors, blockLen);
	}
	// 相机控制
	const Vector2& cam_v = camera.get_velocity();
	if (keyState['W'])
	{
		if (!cam_v.y)
			camera.set_velocity(cam_v.x, -1.0f);
	}
	else
	{
		if (cam_v.y < 0)
			camera.set_velocity(cam_v.x, 0);
	}
	if (keyState['S'])
	{
		if (!cam_v.y)
			camera.set_velocity(cam_v.x, 1.0f);
	}
	else
	{
		if (cam_v.y > 0)
			camera.set_velocity(cam_v.x, 0);
	}
	if (keyState['A'])
	{
		if (!cam_v.x)
			camera.set_velocity(-1.0f, cam_v.y);
	}
	else
	{
		if (cam_v.x < 0)
			camera.set_velocity(0, cam_v.y);
	}
	if (keyState['D'])
	{
		if (!cam_v.x)
			camera.set_velocity(1.0f, cam_v.y);
	}
	else
	{
		if (cam_v.x > 0)
			camera.set_velocity(0, cam_v.y);
	}
	// 鼠标检测
	const ExMessage& mousemove_msg = gv_p->get_mousemove_msg();
	const Vector2& camC = camera.get_abs_coordinates();
	Vector2 mmC(mousemove_msg.x, mousemove_msg.y);
	Vector2 absC(mmC + camC);
	if (absC.x >= 0 && absC.x <= gridLen * gridNumX &&
		absC.y >= 0 && absC.y <= gridLen * gridNumY)
	{
		mmNC.x = (int)absC.x / blockLen;
		mmNC.y = (int)absC.y / blockLen;
	}
	
	if (gv_p->check_if_mouseclick_msg())
	{
		const ExMessage& mouseclick_msg = gv_p->get_mouseclick_msg();
		if (mouseclick_msg.message == WM_LBUTTONDOWN)
		{
			// 打开对应块的状态栏
		}
		if (mouseclick_msg.message == WM_RBUTTONDOWN)
		{
			// 关闭对应块的状态栏
		}
	}
	
}

void DebugScene::on_update()
{
	gv_p->batch_update();
	// 相机坐标校正
	const Vector2& abs_coordinates = camera.get_abs_coordinates();
	if (abs_coordinates.x < 0)
	{
		camera.set_coordinates(0, abs_coordinates.y);
	}
	else if (abs_coordinates.x > gridLen * gridNumX - 800)
	{
		camera.set_coordinates(gridLen * gridNumX - 800, abs_coordinates.y);
	}
	if (abs_coordinates.y < 0)
	{
		camera.set_coordinates(abs_coordinates.x, 0);
	}
	else if (abs_coordinates.y > gridLen * gridNumY - 600)
	{
		camera.set_coordinates(abs_coordinates.x, gridLen * gridNumY - 600);
	}

}

void DebugScene::on_draw()
{
	if (img_terrain_p)
		putimage(0, 0, img_terrain_p, true);
	gv_p->batch_draw();
	setlinecolor(BLACK);
	rectangle(mmNC.x * blockLen, mmNC.y * blockLen, (mmNC.x + 1) * blockLen, (mmNC.y + 1) * blockLen,
		true);
}
