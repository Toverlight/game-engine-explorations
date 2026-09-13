#ifndef _TILE_MAP_MODEL_H_
#define _TILE_MAP_MODEL_H_

#include <string>
#include <vector>

struct TileMapModel
{
	int id;

	int width;
	int height;
	int block_length;

	std::vector<std::vector<int>> map_data_mat;

	std::string name;

};

#endif // !_TILE_MAP_MODEL_H_
