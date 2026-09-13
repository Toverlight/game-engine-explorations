#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "object.h"
#include "vector2.h"

#include <iostream>

// 摄像机
class Camera : public Object
{
public:
	Camera()
	{
		scale = 1.0f;
		use_physics(false);
	}
	~Camera() = default;

	void on_update() override
	{
		Object::move();
	}

	// 设置画面缩放比例，[0.5f, 2.0f]
	void set_scale(float scale) 
	{ 
		if (scale <= 0.5f) this->scale = 0.5f;
		else if (scale >= 2.0f) this->scale = 2.0f;
		else if (abs(scale - 1.0f) < 0.05f) this->scale = 1.0f;
		else
		{
			this->scale = scale;
		}
	}
	// 获取画面缩放比例
	float get_scale() const { return scale; }
	// 增加画面缩放比例，[0.5f, 2.0f]
	void add_scale(float delta_scale) 
	{ 
		if (scale + delta_scale <= 0.5f) scale = 0.5f;
		else if (scale + delta_scale >= 2.0f) scale = 2.0f;
		else if (abs(scale + delta_scale - 1.0f) < 0.05f) this->scale = 1.0f;
		else if (abs(scale + delta_scale - 0.5f) < 0.05f) this->scale = 0.5f;
		else if (abs(scale + delta_scale - 2.0f) < 0.05f) this->scale = 2.0f;
		else
		{
			scale += delta_scale;
		}
	}

private:
	float scale;								// 画面缩放比例
};

#endif // !_CAMERA_H_