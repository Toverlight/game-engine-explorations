#ifndef _TILE_MAP_H_
#define _TILE_MAP_H_

#ifndef PIX_PER_M
#define PIX_PER_M 64
#endif // !PIX_PER_M

#include "log.h"
#include "util.h"
#include "var_g.h"
#include "object.h"

#include <map>
#include <vector>
#include <graphics.h>

extern GVar* gv_p;
extern Log* log_p;

// 碰撞类型
enum class CollisionType
{
	Through,
	Platform,
	Wall,
	Water
};
// 矩形的边
enum class RectSide
{
	None = 0,
	Left = 1,
	Top = 1 << 1,
	Right = 1 << 2,
	Bottom = 1 << 3
};
// 碰撞检测返回类型
struct CollisionReturn
{
	float overlap;						// 重叠长度
	bool is_inner;						// （坐标）是否在对方里面
	Vector2 collision_normal;			// 碰撞方向单位向量
	Vector2 tangential_normal;			// 切向方向单位向量
	bool is_collided;					// 是否发生碰撞

	CollisionReturn()
	{
		overlap = 0;
		is_inner = false;
		collision_normal = { 1,0 };
		tangential_normal = { 0,-1 };
		is_collided = false;
	}
};

// 平铺图类
class TileMap
{
public:
	TileMap(size_t x_num, size_t y_num, int block_length = 64);
	~TileMap() = default;

	// 直接指定
	void assign_map_data(const std::vector<std::vector<int>>& map_data_mat);

	// 插入映射和加载图片
	static void insert_id2what_loadimage(CollisionType c_t, float e, float mju, float ro,
		LPCTSTR path_name)
	{
		size_t curId = img_tile_list.size() + 1;
		id2collision_map.emplace(curId, c_t);
		id2e_map.emplace(curId, e);
		id2mju_map.emplace(curId, mju);
		id2ro_map.emplace(curId, ro);
		load_img(path_name);

	}

	// 绘制地图
	void on_draw();
	// 绘制水
	void on_draw_water();

	// 碰撞检测
	CollisionReturn check_collision(const Object* obj_p, int tile_x, int tile_y);

	// 碰撞解决函数
	void resolve_collision(Object* obj_p);

	// 指定地图名称
	void set_name(const std::string& str)
	{
		name = str;
	}

	// 获取地图名称
	std::string get_name() const
	{
		return name;
	}

	// 获取地图的高度
	int get_height() const
	{
		return map_data_mat.size();
	}
	// 获取地图的宽度
	int get_width() const
	{
		return map_data_mat[0].size();
	}
	// 获取地图块的边长
	int get_block_length() const
	{
		return block_length;
	}

	// 序列化地图数据
	std::string serialize_map_data() const {
		std::ostringstream oss;
		oss << '\'';
		for (const auto& row : map_data_mat) {
			for (const auto& val : row) {
				oss << val << ',';
			}
			oss << ';';  // 用分号分隔行
		}
		oss << '\'';
		return oss.str();
	}
	// 反序列化地图数据
	void externalize_map_data(const unsigned char* pData)
	{
		int offset = 0;
		int xNum = 0, yNum = 0;
		std::string curNumS = "";
		while (*(pData + offset) != '\0')
		{
			if (*(pData + offset) >= '0' && *(pData + offset) <= '9')
			{
				curNumS += *(pData + offset);
			}
			else if (*(pData + offset) == ',')
			{
				map_data_mat[yNum][xNum++] = atoi(curNumS.c_str());
				curNumS = "";
			}
			else if (*(pData + offset) == ';')
			{
				yNum++;
				xNum = 0;
			}
			offset++;
		}
	}
private:
	// 加载图片到平铺块图片列表（传入nullptr则压入零大小的空图片）
	static void load_img(LPCTSTR file_name)
	{
		IMAGE temp_img;
		if (file_name)
			loadimage(&temp_img, file_name);
		img_tile_list.push_back(temp_img);
	}

private:
	std::vector<std::vector<int>> map_data_mat;					// 地图数据
	std::string name;											// 地图名称

	static int block_length;									// 地图块边长
	static std::map<int, CollisionType> id2collision_map;		// 碰撞映射
	static std::map<int, float> id2mju_map;						// 粗糙程度映射
	static std::map<int, float> id2e_map;						// 恢复系数映射
	static std::map<int, float> id2ro_map;						// 液体密度映射
	static std::vector<IMAGE> img_tile_list;					// 平铺块图片列表

public:
	static const Vector2 gravity;								// 重力
};

#endif // !_TILE_MAP_H_

