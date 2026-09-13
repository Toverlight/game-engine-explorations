#include "util.h"

void processAndFreeTcharArray(LPCTSTR tchar_ptr) {
	// 假设 tchar_ptr 是一个指向堆区分配的 TCHAR 数组的指针

	// 确保指针确实指向堆区分配的内存
	// 因为 LPCTSTR 是 const 指针，我们需要假设调用者已经正确管理内存
	delete[] const_cast<TCHAR*>(tchar_ptr); // 删除动态分配的内存
}

void putimage(int dst_x, int dst_y, IMAGE* img, bool using_camera)
{
	int w = img->getwidth();
	int h = img->getheight();
	if (w <= 0 || h <= 0) return;
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempX = (int)(dst_x - abs_coordinates.x);
		int tempY = (int)(dst_y - abs_coordinates.y);
		if (tempX > getwidth() || tempY > getheight() || tempX + w < 0 || tempY + h < 0)
			return;
		int srcX = 0, srcY = 0;
		if (tempX < 0)
		{
			srcX = -tempX;
			w -= srcX;
			tempX = 0;
			if (w > getwidth()) w = getwidth();
		}
		if (tempY < 0)
		{
			srcY = -tempY;
			h -= srcY;
			tempY = 0;
			if (h > getheight()) h = getheight();
		}

		putimage(tempX, tempY, w, h, img, srcX, srcY);
	}
	else
	{
		if (dst_x > getwidth() || dst_y > getheight() || dst_x + w < 0 || dst_y + h < 0)
			return;
		int srcX = 0, srcY = 0;
		if (dst_x < 0)
		{
			srcX = -dst_x;
			w -= srcX;
			dst_x = 0;
			if (w > getwidth()) w = getwidth();
		}
		if (dst_y < 0)
		{
			srcY = -dst_y;
			h -= srcY;
			dst_y = 0;
			if (h > getheight()) h = getheight();
		}
		putimage(dst_x, dst_y, w, h, img, srcX, srcY);
	}
}

void putimage_alpha(int dst_x, int dst_y, IMAGE* img, bool using_camera)
{
	int w = img->getwidth();
	int h = img->getheight();
	if (w <= 0 || h <= 0) return;
	bool success = false;
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempX = (int)(dst_x - abs_coordinates.x);
		int tempY = (int)(dst_y - abs_coordinates.y);
		if (tempX > getwidth() || tempY > getheight() || tempX + w < 0 || tempY + h < 0)
			return;
		int srcX = 0, srcY = 0;
		if (tempX < 0)
		{
			srcX = -tempX;
			w -= srcX;
			tempX = 0;
			if (w > getwidth()) w = getwidth();
		}
		if (tempY < 0)
		{
			srcY = -tempY;
			h -= srcY;
			tempY = 0;
			if (h > getheight()) h = getheight();
		}
		success = AlphaBlend(GetImageHDC(GetWorkingImage()), tempX, tempY, w, h,
			GetImageHDC(img), srcX, srcY, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });

		//std::cout << "camera at (" << abs_coordinates.x << ", " << abs_coordinates.y << ")" << '\n';
		//std::cout << "tempX = " << tempX << ", tempY = " << tempY << '\n';
		//std::cout << "w = " << w << ", h = " << h << '\n';
		//std::cout << "srcX = " << srcX << ", srcY = " << srcY << '\n';
		//std::cout << "success = " << (success ? "true" : "false") << '\n';
	}
	else
	{
		if (dst_x > getwidth() || dst_y > getheight() || dst_x + w < 0 || dst_y + h < 0)
			return;
		int srcX = 0, srcY = 0;
		if (dst_x < 0)
		{
			srcX = -dst_x;
			w -= srcX;
			dst_x = 0;
			if (w > getwidth()) w = getwidth();
		}
		if (dst_y < 0)
		{
			srcY = -dst_y;
			h -= srcY;
			dst_y = 0;
			if (h > getheight()) h = getheight();
		}
		success = AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, w, h,
			GetImageHDC(img), srcX, srcY, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
	}
	if (!success) {
		log_p->error("AlphaBlend failed (camera): ", GetLastError());
		system("pause");
	}

	/*system("pause");*/
}

void putimage_alpha(int dst_x, int dst_y, IMAGE* img, float scale, bool using_camera)
{
	int w = img->getwidth();
	int h = img->getheight();
	if (w <= 0 || h <= 0) return;
	int dst_w; 
	int dst_h; 
	if (abs(scale - 0.5f) < 0.05f || abs(scale - 1.0f) < 0.05f || abs(scale - 2.0f) < 0.05f)
	{
		dst_w = static_cast<int>(w * scale);
		dst_h = static_cast<int>(h * scale);
	}
	else
	{
		dst_w = static_cast<int>(w * scale + 1.0f);
		dst_h = static_cast<int>(h * scale + 1.0f);
	}
	bool success = false;
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempX = static_cast<int>(roundf((dst_x - focus_point.x) * scale + focus_offset.x));
		int tempY = static_cast<int>(roundf((dst_y - focus_point.y) * scale + focus_offset.y));
		if (tempX > getwidth() || tempY > getheight() || tempX + dst_w < 0 || tempY + dst_h < 0)
			return;
		success = AlphaBlend(GetImageHDC(GetWorkingImage()), tempX, tempY, dst_w, dst_h,
			GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
	}
	else
	{
		if (dst_x > getwidth() || dst_y > getheight() || dst_x + dst_w < 0 || dst_y + dst_h < 0)
			return;
		success = AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, dst_w, dst_h,
			GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
	}
	if (!success) {
		log_p->error("AlphaBlend failed (camera): ", GetLastError());
		system("pause");
	}
}

// 遍历 IMAGE 变量的所有像素，并打印出每个像素的 RGB 值
// !!! 慎用，性能开销很大很大
void printImageRGBValues(IMAGE* img_p)
{
	int width = img_p->getwidth();
	int height = img_p->getheight();

	SetWorkingImage(img_p);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// 获取像素的 RGB 值
			int color = getpixel(x, y);
			int r = GetRValue(color); // 获取红色分量
			int g = GetGValue(color); // 获取绿色分量
			int b = GetBValue(color); // 获取蓝色分量

			// 输出 RGB 值
			std::cout << "Pixel (" << x << ", " << y << "): "
				<< "R=" << r << ", G=" << g << ", B=" << b << '\n';
		}
	}

	SetWorkingImage(NULL);
}

void line(int left, int top, int right, int bottom, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempLeft = (int)(left - abs_coordinates.x);
		int tempTop = (int)(top - abs_coordinates.y);
		int tempRight = (int)(right - abs_coordinates.x);
		int tempBottom = (int)(bottom - abs_coordinates.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		line(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		if ((left > getwidth() && right > getwidth()) ||
			(top > getheight() && bottom > getheight()) ||
			(right < 0 && left < 0) || (bottom < 0 && top < 0))
			return;
		line(left, top, right, bottom);
	}
}

void circle(int x, int y, int radius, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempX = (int)(x - abs_coordinates.x);
		int tempY = (int)(y - abs_coordinates.y);
		if (tempX - radius > getwidth() || tempY - radius > getheight() ||
			tempX + radius < 0 || tempY + radius < 0)
			return;
		circle(tempX, tempY, radius);
	}
	else
	{
		if (x - radius > getwidth() || y - radius > getheight() ||
			x + radius < 0 || y + radius < 0)
			return;
		circle(x, y, radius);
	}
}

void fillcircle(int x, int y, int radius, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempX = (int)(x - abs_coordinates.x);
		int tempY = (int)(y - abs_coordinates.y);
		if (tempX - radius > getwidth() || tempY - radius > getheight() ||
			tempX + radius < 0 || tempY + radius < 0)
			return;
		fillcircle(tempX, tempY, radius);
	}
	else
	{
		if (x - radius > getwidth() || y - radius > getheight() ||
			x + radius < 0 || y + radius < 0)
			return;
		fillcircle(x, y, radius);
	}
}

void solidcircle(int x, int y, int radius, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempX = (int)(x - abs_coordinates.x);
		int tempY = (int)(y - abs_coordinates.y);
		if (tempX - radius > getwidth() || tempY - radius > getheight() ||
			tempX + radius < 0 || tempY + radius < 0)
			return;
		solidcircle(tempX, tempY, radius);
	}
	else
	{
		if (x - radius > getwidth() || y - radius > getheight() ||
			x + radius < 0 || y + radius < 0)
			return;
		solidcircle(x, y, radius);
	}
}

void rectangle(int left, int top, int right, int bottom, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempLeft = (int)(left - abs_coordinates.x);
		int tempTop = (int)(top - abs_coordinates.y);
		int tempRight = (int)(right - abs_coordinates.x);
		int tempBottom = (int)(bottom - abs_coordinates.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		rectangle(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		if ((left > getwidth() && right > getwidth()) ||
			(top > getheight() && bottom > getheight()) ||
			(right < 0 && left < 0) || (bottom < 0 && top < 0))
			return;
		rectangle(left, top, right, bottom);
	}
}

void fillrectangle(int left, int top, int right, int bottom, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempLeft = (int)(left - abs_coordinates.x);
		int tempTop = (int)(top - abs_coordinates.y);
		int tempRight = (int)(right - abs_coordinates.x);
		int tempBottom = (int)(bottom - abs_coordinates.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		fillrectangle(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		if ((left > getwidth() && right > getwidth()) ||
			(top > getheight() && bottom > getheight()) ||
			(right < 0 && left < 0) || (bottom < 0 && top < 0))
			return;
		fillrectangle(left, top, right, bottom);
	}
}

void solidrectangle(int left, int top, int right, int bottom, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		int tempLeft = (int)(left - abs_coordinates.x);
		int tempTop = (int)(top - abs_coordinates.y);
		int tempRight = (int)(right - abs_coordinates.x);
		int tempBottom = (int)(bottom - abs_coordinates.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		solidrectangle(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		if ((left > getwidth() && right > getwidth()) ||
			(top > getheight() && bottom > getheight()) ||
			(right < 0 && left < 0) || (bottom < 0 && top < 0))
			return;
		solidrectangle(left, top, right, bottom);
	}
}

void line(int left, int top, int right, int bottom, float scale, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempLeft = (int)((left - focus_point.x) * scale + focus_offset.x);
		int tempTop = (int)((top - focus_point.y) * scale + focus_offset.y);
		int tempRight = (int)((right - focus_point.x) * scale + focus_offset.x);
		int tempBottom = (int)((bottom - focus_point.y) * scale + focus_offset.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		line(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		int tempRight = left + (right - left) * scale;
		int tempBottom = top + (bottom - top) * scale;
		if ((left > getwidth() && right > getwidth()) || (top > getheight() && bottom > getheight()) ||
			(tempRight < 0 && left < 0) || (tempBottom < 0 && top < 0))
			return;
		line(left, top, tempRight, tempBottom);
	}
}

void circle(int x, int y, int radius, float scale, bool using_camera)
{
	int temp_radius = (int)(radius * scale);
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempX = (int)((x - focus_point.x) * scale + focus_offset.x);
		int tempY = (int)((y - focus_point.y) * scale + focus_offset.y);
		if (tempX - temp_radius > getwidth() || tempY - temp_radius > getheight() ||
			tempX + temp_radius < 0 || tempY + temp_radius < 0)
			return;
		circle(tempX, tempY, temp_radius);
	}
	else
	{
		if (x - temp_radius > getwidth() || y - temp_radius > getheight() ||
			x + temp_radius < 0 || y + temp_radius < 0)
			return;
		circle(x, y, temp_radius);
	}
}

void fillcircle(int x, int y, int radius, float scale, bool using_camera)
{
	int temp_radius = (int)(radius * scale);
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempX = (int)((x - focus_point.x) * scale + focus_offset.x);
		int tempY = (int)((y - focus_point.y) * scale + focus_offset.y);
		if (tempX - temp_radius > getwidth() || tempY - temp_radius > getheight() ||
			tempX + temp_radius < 0 || tempY + temp_radius < 0)
			return;
		fillcircle(tempX, tempY, temp_radius);
	}
	else
	{
		if (x - temp_radius > getwidth() || y - temp_radius > getheight() ||
			x + temp_radius < 0 || y + temp_radius < 0)
			return;
		fillcircle(x, y, temp_radius);
	}
}

void solidcircle(int x, int y, int radius, float scale, bool using_camera)
{
	int temp_radius = (int)(radius * scale);
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempX = (int)((x - focus_point.x) * scale + focus_offset.x);
		int tempY = (int)((y - focus_point.y) * scale + focus_offset.y);
		if (tempX - temp_radius > getwidth() || tempY - temp_radius > getheight() ||
			tempX + temp_radius < 0 || tempY + temp_radius < 0)
			return;
		solidcircle(tempX, tempY, temp_radius);
	}
	else
	{
		if (x - temp_radius > getwidth() || y - temp_radius > getheight() ||
			x + temp_radius < 0 || y + temp_radius < 0)
			return;
		solidcircle(x, y, temp_radius);
	}
}

void rectangle(int left, int top, int right, int bottom, float scale, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempLeft = (int)((left - focus_point.x) * scale + focus_offset.x);
		int tempTop = (int)((top - focus_point.y) * scale + focus_offset.y);
		int tempRight = (int)((right - focus_point.x) * scale + focus_offset.x);
		int tempBottom = (int)((bottom - focus_point.y) * scale + focus_offset.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		rectangle(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		int tempRight = left + (right - left) * scale;
		int tempBottom = top + (bottom - top) * scale;
		if ((left > getwidth() && right > getwidth()) || (top > getheight() && bottom > getheight()) ||
			(tempRight < 0 && left < 0) || (tempBottom < 0 && top < 0))
			return;
		rectangle(left, top, tempRight, tempBottom);
	}
}

void fillrectangle(int left, int top, int right, int bottom, float scale, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempLeft = (int)((left - focus_point.x) * scale + focus_offset.x);
		int tempTop = (int)((top - focus_point.y) * scale + focus_offset.y);
		int tempRight = (int)((right - focus_point.x) * scale + focus_offset.x);
		int tempBottom = (int)((bottom - focus_point.y) * scale + focus_offset.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		fillrectangle(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		int tempRight = left + (right - left) * scale;
		int tempBottom = top + (bottom - top) * scale;
		if ((left > getwidth() && right > getwidth()) || (top > getheight() && bottom > getheight()) ||
			(tempRight < 0 && left < 0) || (tempBottom < 0 && top < 0))
			return;
		fillrectangle(left, top, tempRight, tempBottom);
	}
}

void solidrectangle(int left, int top, int right, int bottom, float scale, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		int tempLeft = (int)((left - focus_point.x) * scale + focus_offset.x);
		int tempTop = (int)((top - focus_point.y) * scale + focus_offset.y);
		int tempRight = (int)((right - focus_point.x) * scale + focus_offset.x);
		int tempBottom = (int)((bottom - focus_point.y) * scale + focus_offset.y);
		if ((tempLeft > getwidth() && tempRight > getwidth()) ||
			(tempTop > getheight() && tempBottom > getheight()) ||
			(tempRight < 0 && tempLeft < 0) || (tempBottom < 0 && tempTop < 0))
			return;
		solidrectangle(tempLeft, tempTop, tempRight, tempBottom);
	}
	else
	{
		int tempRight = left + (right - left) * scale;
		int tempBottom = top + (bottom - top) * scale;
		if ((left > getwidth() && right > getwidth()) || (top > getheight() && bottom > getheight()) ||
			(tempRight < 0 && left < 0) || (tempBottom < 0 && top < 0))
			return;
		solidrectangle(left, top, tempRight, tempBottom);
	}
}

void drawtext(LPCTSTR str, RECT* pRect, UINT uFormat, bool using_camera)
{
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		RECT tempRect;
		tempRect.left = pRect->left - abs_coordinates.x;
		tempRect.right = pRect->right - abs_coordinates.x;
		tempRect.top = pRect->top - abs_coordinates.y;
		tempRect.bottom = pRect->bottom - abs_coordinates.y;
		if (tempRect.left > getwidth() || tempRect.top > getheight() ||
			tempRect.right < 0 || tempRect.bottom < 0)
			return;
		drawtext(str, &tempRect, uFormat);
	}
	else
	{
		if (pRect->left > getwidth() || pRect->top > getheight() || pRect->right < 0 ||
			pRect->bottom < 0)
			return;
		drawtext(str, pRect, uFormat);
	}
}

void drawtext(LPCTSTR str, RECT* pRect, UINT uFormat, float scale, bool using_camera)
{
	RECT tempRect;
	if (using_camera)
	{
		const Vector2& abs_coordinates = gv_p->get_camera()->get_abs_coordinates();
		Vector2 focus_offset(getwidth() / 2, getheight() / 2);
		Vector2 focus_point = abs_coordinates + focus_offset;
		tempRect.left = (int)((pRect->left - focus_point.x) * scale + focus_offset.x);
		tempRect.right = (int)((pRect->right - focus_point.x) * scale + focus_offset.x);
		tempRect.top = (int)((pRect->top - focus_point.y) * scale + focus_offset.y);
		tempRect.bottom = (int)((pRect->bottom - focus_point.y) * scale + focus_offset.y);
		if (tempRect.left > getwidth() || tempRect.top > getheight() ||
			tempRect.right < 0 || tempRect.bottom < 0)
			return;
		drawtext(str, &tempRect, uFormat);
	}
	else
	{
		tempRect.left = pRect->left;
		tempRect.top = pRect->top;
		tempRect.right = pRect->left + (pRect->right - pRect->left) * scale;
		tempRect.bottom = pRect->top + (pRect->bottom - pRect->top) * scale;
		if (tempRect.left > getwidth() || tempRect.top > getheight() ||
			tempRect.right < 0 || tempRect.bottom < 0)
			return;
		drawtext(str, &tempRect, uFormat);
	}
}
