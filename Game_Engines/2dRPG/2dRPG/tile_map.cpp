#include "tile_map.h"

int TileMap::block_length = 64;
std::vector<IMAGE> TileMap::img_tile_list;
std::map<int, CollisionType> TileMap::id2collision_map;
std::map<int, float> TileMap::id2mju_map;
std::map<int, float> TileMap::id2e_map;
std::map<int, float> TileMap::id2ro_map;

const Vector2 TileMap::gravity = { 0, 0.001 }; // 9.8e-3f * PIX_PER_M

TileMap::TileMap(size_t x_num, size_t y_num, int block_length)
{
	this->block_length = block_length;
	name = "";

	map_data_mat.resize(y_num);
	for (auto& row : map_data_mat)
	{
		row.resize(x_num);
	}

	img_tile_list.reserve(100);
}

// 直接指定
void TileMap::assign_map_data(const std::vector<std::vector<int>>& map_data_mat)
{
	bool is_valid = true;
	size_t err_i = -1;
	if (this->map_data_mat.size() == map_data_mat.size())
	{
		for (size_t i = 0; i < this->map_data_mat.size(); ++i)
		{
			if (this->map_data_mat[i].size() != map_data_mat[i].size())
			{
				is_valid = false;
				err_i = i;
				break;
			}
			for (size_t j = 0; j < map_data_mat[i].size(); ++j)
			{
				if (map_data_mat[i][j] < 0 || map_data_mat[i][j] > img_tile_list.size())
				{
					log_p->setTarget(LogTarget::Console);
					log_p->warn("\"", __func__, "\" map_data_mat[", i, "]", "[", j, "]", " valued ",
						map_data_mat[i][j], " is out of bounds of TileMap::img_tile_a.size()-->", img_tile_list.size(),
						", so this block won\'t be drawn.");
				}
			}
		}
	}
	else
	{
		is_valid = false;
	}

	if (!is_valid)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->error("\"", __func__, "\": map_data_mat failed to be assigned currectly!\nYou initialized the size ( ",
			this->map_data_mat[0].size(), ", ", this->map_data_mat.size(),
			" ),but trying to assign it by the data matrix of the size ( ", 
			map_data_mat[(err_i == -1 ? 0 : err_i)].size(), ", ", map_data_mat.size(), " ).");
	}

	this->map_data_mat = map_data_mat;
}

void TileMap::on_draw()
{
	for (int y = 0; y < map_data_mat.size(); ++y)
	{
		for (int x = 0; x < map_data_mat[0].size(); ++x)
		{
			if (!map_data_mat[y][x]) continue;				// 0 代表空块，不画
			if (map_data_mat[y][x] < 0 || map_data_mat[y][x] > img_tile_list.size()) continue;// 没加载贴图的不画
			auto it = id2collision_map.find(map_data_mat[y][x]);
			if (it == id2collision_map.end()) continue;
			if (it->second == CollisionType::Water) continue;	// 水的图层在场景大多数物体之上，以呈现半透明
			putimage_alpha(block_length * x, block_length * y, &img_tile_list[map_data_mat[y][x] - 1],
				gv_p->get_camera()->get_scale());

			if (gv_p->check_debug())						// 调试模式下
			{
				if (it->second == CollisionType::Wall)
				{
					setlinecolor(RED);
					rectangle(block_length * x, block_length * y, block_length * x + block_length, 
						block_length * y + block_length, gv_p->get_camera()->get_scale(),
						true);
					line(block_length * x, block_length * y + block_length, block_length * x + block_length, 
						block_length * y, gv_p->get_camera()->get_scale(), true);
				}
				if (it->second == CollisionType::Platform)
				{
					setlinecolor(RGB(244, 219, 155));
					line(block_length * x, block_length * y, block_length * x + block_length, block_length * y,
						gv_p->get_camera()->get_scale(), true);
				}
			}
		}
	}
}

void TileMap::on_draw_water()
{
	for (int y = 0; y < map_data_mat.size(); ++y)
	{
		for (int x = 0; x < map_data_mat[0].size(); ++x)
		{
			if (!map_data_mat[y][x]) continue;				// 0 代表空块，不画
			if (map_data_mat[y][x] < 0 || map_data_mat[y][x] > img_tile_list.size()) continue;// 没加载贴图的不画
			auto it = id2collision_map.find(map_data_mat[y][x]);
			if (it == id2collision_map.end()) continue;
			if (it->second == CollisionType::Water)	// 水的图层在场景大多数物体之上，以呈现半透明
			{
				putimage_alpha(block_length * x, block_length * y, &img_tile_list[map_data_mat[y][x] - 1],
					gv_p->get_camera()->get_scale());
				if (gv_p->check_debug())
				{
					setlinecolor(RGB(49, 151, 203));
					rectangle(block_length * x, block_length * y, block_length * x + block_length, 
						block_length * y + block_length, gv_p->get_camera()->get_scale(), true);
				}
			}
		}
	}
}

CollisionReturn TileMap::check_collision(const Object* obj_p, int tile_x, int tile_y)
{
	CollisionReturn cr;
	// 物体是圆形的情况
	if (obj_p->if_has_radius())
	{
		const Vector2& circle_center = obj_p->get_rel_coordinates();
		Vector2 tile_min(tile_x * block_length, tile_y * block_length);
		Vector2 tile_max((tile_x + 1) * block_length, (tile_y + 1) * block_length);

		if (circle_center.x >= tile_min.x && circle_center.x <= tile_max.x &&
			circle_center.y >= tile_min.y && circle_center.y <= tile_max.y)
		{
			cr.is_inner = true;
		}
		else
		{
			cr.is_inner = false;
		}

		float nearest_x = max(tile_min.x, min(circle_center.x, tile_max.x));
		float nearest_y = max(tile_min.y, min(circle_center.y, tile_max.y));
		Vector2 nearest_point(nearest_x, nearest_y);

		float dis = distance(nearest_point, circle_center);

		if (dis <= obj_p->get_radius())
		{
			cr.overlap = obj_p->get_radius() - dis;
			if (dis == 0)
			{
				if (nearest_point.y == tile_min.y)
				{
					cr.collision_normal = { 0,-1 };
				}
				else if (nearest_point.y == tile_max.y)
				{
					cr.collision_normal = { 0,1 };
				}
				else if (nearest_point.x == tile_min.x)
				{
					cr.collision_normal = { -1,0 };
				}
				else
				{
					cr.collision_normal = { 1,0 };
				}
			}
			else
			{
				cr.collision_normal = (circle_center - nearest_point).normalize();
			}
			cr.tangential_normal = cr.collision_normal.rotate(90);
			cr.is_collided = true;
		}
		else
		{
			cr.is_collided = false;
		}
	}
	// 物体是矩形的情况
	else
	{
		const Vector2& rect_coordinates = obj_p->get_rel_coordinates();
		const Vector2& rect_size = obj_p->get_size();

		float overlap_x = min((tile_x + 1) * block_length, rect_coordinates.x + rect_size.x) -
			max(tile_x * block_length, rect_coordinates.x);
		float overlap_y = min((tile_y + 1) * block_length, rect_coordinates.y + rect_size.y) -
			max(tile_y * block_length, rect_coordinates.y);

		if (overlap_x < 0 || overlap_y < 0) 
		{
			cr.is_collided = false;
		}
		else if (overlap_x < overlap_y)
		{
			cr.overlap = overlap_x;
			// 互相的中轴检测
			if (rect_coordinates.x + rect_size.x / 2.0f > (tile_x + 0.5f) * block_length)
			{
				cr.collision_normal = { -1,0 };
				cr.tangential_normal = { 0,1 };
			}
			else
			{
				cr.collision_normal = { 1,0 };
				cr.tangential_normal = { 0,-1 };
			}
			cr.is_collided = true;
		}
		else
		{
			cr.overlap = overlap_y;
			// 互相的中轴检测
			if (rect_coordinates.y + rect_size.y / 2.0f > (tile_y + 0.5f) * block_length)
			{
				cr.collision_normal = { 0,-1 };
				cr.tangential_normal = { -1,0 };
			}
			else
			{
				cr.collision_normal = { 0,1 };
				cr.tangential_normal = { 1,0 };
			}
			cr.is_collided = true;
		}

	}
	
	return cr;
}

void TileMap::resolve_collision(Object* obj_p)
{
	obj_p->set_cur_collision_normal({ 0,0 });
	if (!obj_p->if_has_tile_collision_resolution()) return;
	// 有父对象的不进行瓦片碰撞检测
	if (obj_p->has_parent()) return;

	// 矩形情况size，圆形情况则为外接正方形
	// 获取物体的边界
	// 这里获取的相对坐标就是世界坐标
	Vector2 obj_min;
	Vector2 obj_max;
	if (obj_p->if_has_radius())
	{
		int radius = obj_p->get_radius();
		obj_min.x = obj_p->get_rel_coordinates().x - radius;
		obj_min.y = obj_p->get_rel_coordinates().y - radius;
		obj_max.x = obj_p->get_rel_coordinates().x + radius;
		obj_max.y = obj_p->get_rel_coordinates().y + radius;
	}
	else
	{
		obj_min = obj_p->get_rel_coordinates();							// 左上角
		obj_max = obj_p->get_rel_coordinates() + obj_p->get_size();		// 右下角
	}

	int tile_min_x = static_cast<int>((int)obj_min.x % block_length <= 1 ? obj_min.x / block_length - 1 : obj_min.x / block_length);
	int tile_min_y = static_cast<int>((int)obj_min.y % block_length <= 1 ? obj_min.y / block_length - 1 : obj_min.y / block_length);
	int tile_max_x = static_cast<int>(obj_max.x / block_length);
	int tile_max_y = static_cast<int>(obj_max.y / block_length);

	bool switch_water_resolution = true;							// 水碰撞处理以第一块接触的水图块为准

	for (int ty = tile_min_y; ty <= tile_max_y; ++ty)
	{
		for (int tx = tile_min_x; tx <= tile_max_x; ++tx)
		{
			if (tx >= 0 && tx < map_data_mat[0].size() && ty >= 0 && ty < map_data_mat.size())
			{
				int tile_id = map_data_mat[ty][tx];
				if (!tile_id) continue;

				CollisionType collision_t;
				auto it_ct = id2collision_map.find(tile_id);
				if (it_ct != id2collision_map.end())
					collision_t = it_ct->second;
				else
					collision_t = CollisionType::Through;
				obj_p->set_cur_collision_t(collision_t);
				if (collision_t == CollisionType::Through) continue;
				if (collision_t == CollisionType::Water && !switch_water_resolution) continue;

				CollisionReturn cr = check_collision(obj_p, tx, ty);
				const Vector2& coordinates = obj_p->get_rel_coordinates();
				const Vector2& last_coordinates = obj_p->get_rel_last_coordinates();
				const Vector2& velocity = obj_p->get_velocity();
				const Vector2& force = obj_p->get_force();
				float mju;
				auto it_mju = id2mju_map.find(tile_id);
				if (it_mju != id2mju_map.end())
					mju = it_mju->second;
				else
					mju = 1;
				float new_mju = sqrt(mju * obj_p->get_mju());
				float ro = 0;												// 水的密度[kg / pix^2]
				float e = 1, new_e = 0;
				if (collision_t == CollisionType::Water)
				{
					auto it_ro = id2ro_map.find(tile_id);
					if (it_ro != id2ro_map.end())
						ro = it_ro->second;
					else
						ro = 0;
				}
				else
				{
					auto it_e = id2e_map.find(tile_id);
					if (it_e != id2e_map.end())
						e = it_e->second;
					else
						e = 0;
					new_e = sqrt(e * obj_p->get_e());
				}

				// 圆形的情况
				if (obj_p->if_has_radius() && cr.is_collided)
				{
					obj_p->set_cur_collision_normal(cr.collision_normal);
					float area = 0;					// 圆浸入水中的面积
					float alpha = 0;				// 圆浸入水中的弧对应的角度
					float h = 0;					// 圆心到水面的竖直距离
					float l = 0;					// 水面在圆内的弦长的一半
					switch (collision_t)
					{
					case CollisionType::Wall:
						// 上或下
						if (cr.collision_normal.x == 0)
						{
							// 重叠处理
							if (cr.overlap > 0)
							{
								if (cr.is_inner) {
									obj_p->set_coordinates(coordinates - cr.collision_normal * (cr.overlap +
										obj_p->get_radius()));
									cr.collision_normal = cr.collision_normal * -1;
								}
								else
								{
									obj_p->set_coordinates(coordinates + cr.collision_normal * cr.overlap);
								}
							}
							// 速度分支
							if (velocity.y == 0)
							{
								if (velocity.x != 0)
								{
									// 水平摩擦力
									if (cr.collision_normal.y > 0)	// 上面（较小）
									{
										obj_p->apply_force((velocity.x > 0 ? 1 : -1) * -(new_mju * 0.1f) *
											(obj_p->get_mass() * gravity.y), 0);
									}
									else							// 下面
									{
										obj_p->apply_force((velocity.x > 0 ? 1 : -1) * -(new_mju) *
											(obj_p->get_mass() * gravity.y), 0);
									}
								}
								// 支持力模拟
								if (force.y > 0)
									obj_p->set_force(force.x, 0);
							}
							// 碰撞处理
							else
							{
								if (velocity * cr.collision_normal < 0)
								{
									// 速度计算
									obj_p->set_velocity(cr.collision_normal * ((velocity * cr.collision_normal) *
										-new_e) + cr.tangential_normal * ((velocity * cr.tangential_normal) *
											(1 - new_mju)));
									// 速度修正（消除微小分量）
									obj_p->set_velocity((abs(velocity.x) < 0.00064f ? 0 : velocity.x),
										(abs(velocity.y) < 0.00064f ? 0 : velocity.y));
								}
							}
						}
						// 左或右
						else if (cr.collision_normal.y == 0)
						{
							// 重叠处理
							if (cr.overlap > 0)
							{
								if (cr.is_inner) {
									obj_p->set_coordinates(coordinates - cr.collision_normal * (cr.overlap +
										obj_p->get_radius()));
									cr.collision_normal = cr.collision_normal * -1;
								}
								else
								{
									obj_p->set_coordinates(coordinates + cr.collision_normal * cr.overlap);
								}
							}
							// 速度分支
							if (velocity.x == 0)
							{
								if (velocity.y != 0)
								{
									// 竖直摩擦力
									obj_p->apply_force(0, (velocity.y > 0 ? 1 : -1) * -(new_mju * 0.2f) *
										(obj_p->get_mass() * gravity.y));
								}
							}
							// 碰撞处理
							else
							{
								if (velocity * cr.collision_normal < 0)
								{
									// 速度计算
									obj_p->set_velocity(cr.collision_normal * ((velocity * cr.collision_normal) *
										-new_e) + cr.tangential_normal * ((velocity * cr.tangential_normal) *
											(1 - new_mju)));
									// 速度修正（消除微小分量）
									obj_p->set_velocity((abs(velocity.x) < 0.00064f ? 0 : velocity.x),
										(abs(velocity.y) < 0.00064f ? 0 : velocity.y));
								}
							}
						}
						// 斜
						else
						{
							int right_tile_id, bottom_tile_id;
							CollisionType right_collision_t, bottom_collision_t;

							bool check_right = false;
							bool check_bottom = false;
							if (cr.collision_normal.x > 0) { check_right = true; }
							if (cr.collision_normal.y > 0) { check_bottom = true; }

							if (tx + 1 < map_data_mat[0].size())
							{
								right_tile_id = map_data_mat[ty][tx + 1];
								right_collision_t = id2collision_map[right_tile_id];
							}
							else
							{
								check_right = false;
							}
							if (ty + 1 < map_data_mat.size())
							{
								bottom_tile_id = map_data_mat[ty + 1][tx];
								bottom_collision_t = id2collision_map[bottom_tile_id];
							}
							else
							{
								check_bottom = false;
							}

							if (check_right && !check_bottom)	// 右上
							{
								if (right_collision_t == CollisionType::Wall ||
									right_collision_t == CollisionType::Platform)
									continue;
							}
							else if (check_right && check_bottom) // 右下
							{
								if (right_collision_t == CollisionType::Wall ||
									bottom_collision_t == CollisionType::Wall)
									continue;

							}
							else if (!check_right && check_bottom) // 左下
							{
								if (bottom_collision_t == CollisionType::Wall)
									continue;
							}

							// 重叠处理
							if (cr.overlap > 0)
							{
								if (cr.is_inner) {
									obj_p->set_coordinates(coordinates - cr.collision_normal * (cr.overlap +
										obj_p->get_radius()));
									cr.collision_normal = cr.collision_normal * -1;
								}
								else
								{
									obj_p->set_coordinates(coordinates + cr.collision_normal * cr.overlap);
								}
							}
							if (velocity * cr.collision_normal < 0)
							{
								// 速度计算
								obj_p->set_velocity(cr.collision_normal * ((velocity * cr.collision_normal) *
									-new_e) + cr.tangential_normal * ((velocity * cr.tangential_normal) *
										(1 - new_mju)));
								// 速度修正（消除微小分量）
								obj_p->set_velocity((abs(velocity.x) < 0.00064f ? 0 : velocity.x),
									(abs(velocity.y) < 0.00064f ? 0 : velocity.y));
							}
						}

						break;
					case CollisionType::Platform:
						// 上侧
						if (cr.collision_normal.y < 0)
						{
							// 正上侧
							if (cr.collision_normal.x == 0)
							{
								// 上一个位置圆在上方或与上表面相切时 且 正在坠落或沿表面滑动时
								if (last_coordinates.y + obj_p->get_radius() <= ty * block_length
									&& velocity.y >= 0)
								{
									// 重叠处理
									if (cr.overlap > 0)
									{
										obj_p->set_coordinates(coordinates + cr.collision_normal * cr.overlap);
									}
									// 速度分支
									if (velocity.y > 0)
									{
										// 速度计算
										obj_p->set_velocity(cr.collision_normal* ((velocity* cr.collision_normal) *
											-new_e) + cr.tangential_normal * ((velocity * cr.tangential_normal) *
												(1 - new_mju)));
										// 速度修正（消除微小分量）
										obj_p->set_velocity((abs(velocity.x) < 0.00064f ? 0 : velocity.x),
											(abs(velocity.y) < 0.00064f ? 0 : velocity.y));
									}
									else if (velocity.y == 0)
									{
										if (velocity.x != 0)
										{
											obj_p->apply_force((velocity.x > 0 ? 1 : -1) * -(new_mju) *
												(obj_p->get_mass() * gravity.y), 0);
										}
										// 支持力模拟
										if (force.y > 0)
											obj_p->set_force(force.x, 0);
									}
								}
							}
						}
						break;
					case CollisionType::Water:
						// 圆不完全浸入水
						if (coordinates.y - obj_p->get_radius() < ty * block_length)
						{
							// 圆心在水面上方
							if (coordinates.y < ty * block_length)
							{
								h = ty * block_length - coordinates.y;
								alpha = acosf(h / obj_p->get_radius());
								l = sinf(alpha) * obj_p->get_radius();
								alpha = 2.0f * alpha;
								area = 0.5f * alpha * (obj_p->get_radius() * obj_p->get_radius()) - h * l;
							}
							// 圆心在水面下
							else
							{
								h = coordinates.y - ty * block_length;
								alpha = acosf(h / obj_p->get_radius());
								l = sinf(alpha) * obj_p->get_radius();
								alpha = 2.0f * (PI - alpha);
								area = 0.5f * alpha * (obj_p->get_radius() * obj_p->get_radius()) + h * l;
							}
						}
						// 圆完全浸入水
						else
						{
							area = PI * (obj_p->get_radius() * obj_p->get_radius());
						}
						// 施加浮力
						obj_p->apply_force(0, ro * -gravity.y * area);
						// 施加阻力
						obj_p->apply_force(velocity* (-mju * 0.01));
						// 关闭水处理开关
						switch_water_resolution = false;
						break;
					default:
						break;
					}
				}
				// 矩形的情况
				else if (!obj_p->if_has_radius() && cr.is_collided)
				{
					obj_p->set_cur_collision_normal(cr.collision_normal);
					float area = 0;					// 矩形浸入水中的面积
					switch (collision_t)
					{
					case CollisionType::Wall:
						// 上或下
						if (cr.collision_normal.x == 0)
						{
							// 重叠处理
							if (cr.overlap > 0)
							{
								obj_p->set_coordinates(coordinates - cr.collision_normal * cr.overlap);
							}
							// 速度分支
							if (velocity.y == 0)
							{
								if (velocity.x != 0)
								{
									// 水平摩擦力
									if (cr.collision_normal.y > 0)	// 上面（较小）
									{
										obj_p->apply_force((velocity.x > 0 ? 1 : -1) * -(new_mju * 0.1f) *
											(obj_p->get_mass() * gravity.y), 0);
									}
									else							// 下面
									{
										obj_p->apply_force((velocity.x > 0 ? 1 : -1) * -(new_mju) *
											(obj_p->get_mass() * gravity.y), 0);
									}
								}
								// 支持力模拟
								if (force.y > 0)
									obj_p->set_force(force.x, 0);
							}
							// 碰撞处理
							else
							{
								if (velocity * cr.collision_normal > 0)
								{
									// 速度计算
									obj_p->set_velocity(cr.collision_normal * ((velocity * cr.collision_normal) *
										-new_e) + cr.tangential_normal * ((velocity * cr.tangential_normal) *
											(1 - new_mju)));
									// 速度修正（消除微小分量）
									obj_p->set_velocity((abs(velocity.x) < 0.00064f ? 0 : velocity.x),
										(abs(velocity.y) < 0.00064f ? 0 : velocity.y));
								}
							}
						}
						// 左或右
						else if (cr.collision_normal.y == 0)
						{
							// 重叠处理
							if (cr.overlap > 0)
							{
								obj_p->set_coordinates(coordinates - cr.collision_normal * cr.overlap);
							}
							// 速度分支
							if (velocity.x == 0)
							{
								if (velocity.y != 0)
								{
									// 竖直摩擦力
									obj_p->apply_force(0, (velocity.y > 0 ? 1 : -1) * -(new_mju * 0.2f) *
										(obj_p->get_mass() * gravity.y));
								}
							}
							// 碰撞处理
							else
							{
								if (velocity * cr.collision_normal > 0)
								{
									// 速度计算
									obj_p->set_velocity(cr.collision_normal * ((velocity * cr.collision_normal) *
										-new_e) + cr.tangential_normal * ((velocity * cr.tangential_normal) *
											(1 - new_mju)));
									// 速度修正（消除微小分量）
									obj_p->set_velocity((abs(velocity.x) < 0.00064f ? 0 : velocity.x),
										(abs(velocity.y) < 0.00064f ? 0 : velocity.y));
								}
							}
						}
						break;
					case CollisionType::Platform:
						// 上一个位置矩形在上方或与上表面相切时 且 正在坠落或沿表面滑动时
						if (last_coordinates.y + obj_p->get_size().y <= ty * block_length
							&& velocity.y >= 0)
						{
							// 重叠处理
							if (coordinates.y + obj_p->get_size().y > ty * block_length)
							{
								obj_p->set_coordinates(coordinates.x, coordinates.y -
									(coordinates.y + obj_p->get_size().y - ty * block_length));
							}
							// 速度分支
							if (velocity.y > 0)
							{
								// 速度计算
								obj_p->set_velocity(Vector2(0, -1) * ((velocity * Vector2(0, -1)) *
									-new_e) + Vector2(-1, 0) * ((velocity * Vector2(-1, 0)) *
										(1 - new_mju)));
								// 速度修正（消除微小分量）
								obj_p->set_velocity((abs(velocity.x) < 0.00064f ? 0 : velocity.x),
									(abs(velocity.y) < 0.00064f ? 0 : velocity.y));
							}
							else if (velocity.y == 0)
							{
								if (velocity.x != 0)
								{
									obj_p->apply_force((velocity.x > 0 ? 1 : -1) * -(new_mju) *
										(obj_p->get_mass() * gravity.y), 0);
								}
								// 支持力模拟
								if (force.y > 0)
									obj_p->set_force(force.x, 0);
							}
						}
						break;
					case CollisionType::Water:
						// 矩形不完全浸入水中
						if (coordinates.y < ty * block_length)
						{
							area = obj_p->get_size().x * (coordinates.y + obj_p->get_size().y - ty * block_length);
						}
						// 矩形完全浸入水中
						else
						{
							area = obj_p->get_size().x * obj_p->get_size().y;
						}
						// 施加浮力
						obj_p->apply_force(0, ro * -gravity.y * area);
						// 施加阻力
						obj_p->apply_force(velocity * (-mju * 0.01));
						// 关闭水处理开关
						switch_water_resolution = false;
						break;
					default:
						break;
					}
				}
	/*
				 -------------------------------------------------

				// 圆形的情况
				if (obj_p->if_has_radius() && cr.is_collided)
				{
					// 处理碰撞
					switch (collision_t)
					{
					case CollisionType::Wall:
						// 重叠
						if (cr.overlap > 0)									
						{
							// 重叠处理
							obj_p->set_coordinates(coordinates + cr.collision_normal * cr.overlap);
							// 碰撞处理
							if (velocity.x != 0 && velocity.y != 0)							
							{
								// 速度计算
								obj_p->set_velocity(cr.collision_normal * ((velocity * cr.collision_normal) *
									-new_e) + cr.tangential_normal * ((velocity * cr.tangential_normal) *
										(1 - new_mju)));
								// 速度修正（消除微小分量）
								obj_p->set_velocity((abs(velocity.x) < 0.064e-1f ? 0 : velocity.x),
									(abs(velocity.y) < 0.064e-1f ? 0 : velocity.y));
								return;
							}
							else if (velocity.x == 0 && velocity.y == 0 && cr.collision_normal.x == 0)
							{
								// 竖直向下的力分量归零
								if (obj_p->get_force().y > 0)
								{
									obj_p->set_force(Vector2(obj_p->get_force().x, 0));
								}
								return;
							}

						}
						// 相切
						else if (cr.overlap == 0)
						{
							if (velocity.x == 0 && velocity.y != 0 && cr.collision_normal.y == 0)// 竖直滑行（影响较小）
							{
								obj_p->apply_force(Vector2(0, (velocity.y > 0 ? 1 : -1) * -(new_mju * 0.1f) *
									(obj_p->get_mass() * gravity.y)));
								return;
							}
							else if (velocity.x != 0 && velocity.y == 0 && cr.collision_normal.x == 0)// 水平滑行
							{
								obj_p->apply_force(Vector2((velocity.x > 0 ? 1 : -1) * -new_mju * (obj_p->get_mass()
									* gravity.y), 0));
								// 竖直向下的力分量归零
								if (obj_p->get_force().y > 0 && cr.collision_normal.y < 0)
								{
									obj_p->set_force(Vector2(obj_p->get_force().x, 0));
								}
								return;
							}
							else if (velocity.x == 0 && velocity.y == 0 && cr.collision_normal.x == 0
								&& cr.collision_normal.y < 0)									// 静止于上侧
							{
								// 竖直向下的力分量归零
								if (obj_p->get_force().y > 0)
								{
									obj_p->set_force(Vector2(obj_p->get_force().x, 0));
								}
								return;
							}
						}
						
						break;
					case CollisionType::Platform:
						// 圆形在水平移动 且 与顶线从上方相切
						if (velocity.y == 0 && cr.overlap == 0 && cr.collision_normal.x == 0 &&
							cr.collision_normal.y < 0)
						{
							obj_p->apply_force(Vector2((velocity.x > 0 ? 1 : -1) * -new_mju * (obj_p->get_mass()
								* gravity.y), 0));
							// 竖直向下的力分量归零
							if (obj_p->get_force().y > 0)
							{
								obj_p->set_force(Vector2(obj_p->get_force().x, 0));
							}
						}
						// 圆形在下落 且 碰撞于顶线
						if (velocity.y > 0 && cr.collision_normal.y < 0)
						{
							if (cr.overlap > 0)								// 重叠处理
								obj_p->set_coordinates(coordinates + cr.collision_normal * cr.overlap);



						}


						break;
					case CollisionType::Water:
						break;
					default:
						break;
					}
				}
				// 矩形的情况
				else if (!obj_p->if_has_radius())
				{
					// 处理碰撞
					switch (collision_t)
					{
					case CollisionType::Wall:
						switch (cr.rect_side)
						{
						case RectSide::None:
							break;
						case RectSide::Left:
							if (cr.overlap)
								obj_p->set_coordinates(coordinates + Vector2(cr.overlap, 0));
							obj_p->set_velocity((velocity.x < 0 ? 0 : velocity.x), velocity.y * (1.0f - new_mju));
							break;
						case RectSide::Top:
							if (cr.overlap)
								obj_p->set_coordinates(coordinates + Vector2(0, cr.overlap));
							obj_p->set_velocity(velocity.x * (1.0f - new_mju), (velocity.y < 0 ? 0 : velocity.y));
							break;
						case RectSide::Right:
							obj_p->set_coordinates(coordinates + Vector2(-cr.overlap, 0));
							obj_p->set_velocity((velocity.x > 0 ? 0 : velocity.x), velocity.y * (1.0f - new_mju));
							break;
						case RectSide::Bottom:
							obj_p->set_coordinates(coordinates + Vector2(0, -cr.overlap));
							obj_p->set_velocity(velocity.x * (1.0f - new_mju), (velocity.y > 0 ? 0 : velocity.y));
							break;
						}
						break;
					case CollisionType::Platform:
						if ((cr.rect_side == RectSide::Bottom) && velocity.y >= 0)
						{
							if (cr.overlap)
								obj_p->set_coordinates(coordinates + Vector2(0, -cr.overlap));
							obj_p->set_velocity(velocity.x, 0);
						}
						break;
					case CollisionType::Water:
						obj_p->apply_force(Vector2(0, (cr.overlap * obj_p->get_size().x) * -gravity.y * 1.0f));
						obj_p->apply_force(velocity.normalize() * -mju);
						break;
					default:
						break;
					}
				}
			}
		}
	}
	*/


			}
		}
	}
	if (obj_p->get_cur_collision_normal() == Vector2(0, 0))
		obj_p->set_cur_collision_t(CollisionType::Through);
}