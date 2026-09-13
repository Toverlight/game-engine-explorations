#ifndef _IMAGE_BUTTON_H_
#define _IMAGE_BUTTON_H_

#include "util.h"
#include "button.h"

class ImageButton : public Button
{
public:
	ImageButton() = default;
	~ImageButton() = default;

	void on_draw();

	void set_img_p(IMAGE* idle_p, IMAGE* touched_p, IMAGE* down_p)
	{
		img_idle = idle_p, img_touched = touched_p, img_down = down_p;
	}

private:
	IMAGE* img_idle;				// Ä¬ÈÏÌùÍ¼
	IMAGE* img_touched;				// ´¥ÅöÌùÍ¼
	IMAGE* img_down;				// °´ÏÂÌùÍ¼

};

#endif // !_IMAGE_BUTTON_H_

