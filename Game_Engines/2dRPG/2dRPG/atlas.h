#ifndef _ATLAS_H_
#define _ATLAS_H_

#include <vector>
#include <graphics.h>

// 图集
class Atlas
{
public:
	Atlas()
	{

	}
	~Atlas() = default;
	// 根据含一个 %d 的格式字符串从路径中加载连续 num 张图片
	void load_from_file(LPCTSTR path_template, int num)
	{
		img_list.clear();
		img_list.resize(num);

		TCHAR path_file[256];
		for (int i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path_template, i + 1);
			loadimage(&img_list[i], path_file);
		}
	}
	
	// 获取总帧数
	size_t get_frame_num() { return img_list.size(); }

	// 清空图像列表
	void clear() 
	{ 
		img_list.clear();
	}

	// 获取帧
	IMAGE* get_frame_at(int idx_frame) { return &img_list[idx_frame]; }

private:
	std::vector<IMAGE> img_list;			// 图像列表

};

#endif // !_ATLAS_H_

