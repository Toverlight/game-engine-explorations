#include "scene.h"

void Scene::background_draw()
{
	if (img_background_p)
		putimage_alpha(offset.x, offset.y, img_background_p, using_camera_bk);
}
