#ifndef _DATABASE_MANAGER_H_
#define _DATABASE_MANAGER_H_

#include "log.h"
#include "object.h"
#include "tile_map.h"

#include <sqlite3.h>

extern Log* log_p;

// 游戏数据库管理器
class DatabaseManager
{
public:
	static DatabaseManager* getInstance();
	// 保存地图数据到数据库
	void saveTileMapData(const TileMap* pTileMap);
	// 根据名称从数据库加载地图数据
	TileMap* loadTileMapData(const std::string& name);
	// 从数据库加载id映射和图片
	void loadTileMapId2WhatAndImages();

private:
	DatabaseManager();
	~DatabaseManager() = default;
	// 打开数据库
	void openDb();
	// 关闭数据库
	void closeDb();

private:
	sqlite3* db;						// 数据库指针

};

#endif // !_DATABASE_MANAGER_H_

