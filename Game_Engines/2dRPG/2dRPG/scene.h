#ifndef _SCENE_H_
#define _SCENE_H_

#include "log.h"
#include "util.h"
#include "vector2.h"

#include <string>

extern Log* log_p;

class IMAGE;

class Scene
{
public:
	Scene()
	{
		img_background_p = nullptr;
		using_camera_bk = true;

		offset = { -10,-10 };
	}
	virtual ~Scene() = default;

	virtual void on_enter() { }

	virtual void on_input() { }

	virtual void on_update() { }

	void background_draw();
	virtual void on_draw() { }

	virtual void on_exit() { }
	// …Ë÷√±≥æ∞Õº∆¨
	void set_img_background(IMAGE* img_p)
	{
		img_background_p = img_p;
	}
	// …Ë÷√±≥æ∞Õº∆¨∆´“∆¡ø
	void set_offset(int delta_x, int delta_y)
	{
		offset.x = delta_x;
		offset.y = delta_y;
	}

	// ±≥æ∞÷∏’Î «∑Ò…Ë÷√
	bool is_imgbkp_set() const { return img_background_p != nullptr; }
	// …Ë÷√±≥æ∞Õº∆¨ «∑Ò π”√œ‡ª˙
	void set_using_camera_bk(bool flag) { using_camera_bk = flag; }

private:
	IMAGE* img_background_p;					// ±≥æ∞Õº∆¨÷∏’Î

	Vector2 offset;								// ±≥æ∞Õº∆¨∆´“∆¡ø

	bool using_camera_bk;						// ±≥æ∞Õº∆¨ «∑Ò π”√œ‡ª˙
};


#endif // !_SCENE_H_