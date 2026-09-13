#include "database_manager.h"

#include "id2what_model.h"
#include "tile_map_model.h"

DatabaseManager* DatabaseManager::getInstance()
{
	static DatabaseManager databaseManager;
	return &databaseManager;
}

void DatabaseManager::saveTileMapData(const TileMap* pTileMap)
{
	openDb();

	if (pTileMap->get_name().length() == 0)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->warn("You haven\'t set any name for this tilemap, saving won\'t be completed.");
	}

	std::string sql_insert_pre = R"(
		INSERT INTO tile_maps (width, height, block_length, map_data_mat, name) VALUES (
	)";

	std::string sql_insert_nxt = 
		std::to_string(pTileMap->get_width()) + ", "
		+ std::to_string(pTileMap->get_height()) + ", " + std::to_string(pTileMap->get_block_length())
		+ ", " + pTileMap->serialize_map_data() + ", \'" + pTileMap->get_name() + "\');";

	std::string sql_insert = sql_insert_pre + sql_insert_nxt;

	int rc = sqlite3_exec(db, sql_insert.c_str(), nullptr, 0, nullptr);
	if (rc != SQLITE_OK)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->error("in ", __func__, ": SQL error: ", sqlite3_errmsg(db));
	}
	else
	{
		log_p->setTarget(LogTarget::Console);
		log_p->info("Save tilemap data successfully!");
	}

	closeDb();
}

TileMap* DatabaseManager::loadTileMapData(const std::string& name)
{
	openDb();

	std::string sql_select = R"(
		SELECT width, height, block_length, map_data_mat FROM tile_maps
			WHERE name = ')" + name + "\';";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql_select.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->error("in ", __func__, ": Failed to fetch data: ", sqlite3_errmsg(db));
	}
	else
	{
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			int width = sqlite3_column_int(stmt, 0);
			int height = sqlite3_column_int(stmt, 1);
			int block_length = sqlite3_column_int(stmt, 2);
			const unsigned char* map_data_mat = sqlite3_column_text(stmt, 3);

			TileMap* pNewTileMap = new TileMap(width, height, block_length);
			if (!pNewTileMap)
			{
				log_p->setTarget(LogTarget::Console);
				log_p->error("in ", __func__, ": Failed to create Tilemap entity!");
			}
			else
			{
				pNewTileMap->externalize_map_data(map_data_mat);
				pNewTileMap->set_name(name);

				log_p->setTarget(LogTarget::Console);
				log_p->info("Load tilemap data successfully!");
			}

			sqlite3_finalize(stmt);			// ÊÍ·ÅÓï¾ä

			closeDb();

			return pNewTileMap;
		}
	}
	sqlite3_finalize(stmt);			// ÊÍ·ÅÓï¾ä

	closeDb();

	return nullptr;
}

void DatabaseManager::loadTileMapId2WhatAndImages()
{
	openDb();

	const char* sql_selectAll_id2what = "SELECT * FROM id2what;";
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql_selectAll_id2what, -1, &stmt, nullptr);
	int collision_type = (int)CollisionType::Through;
	float e = 0, mju = 0, ro = 0;
	if (rc != SQLITE_OK)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->error("in ", __func__, " \'stmt\' : Failed to fetch data: ", sqlite3_errmsg(db));
	}
	else
	{
		std::string sql_select_images_pre = "SELECT path_name from images WHERE alias = \'";
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			collision_type = sqlite3_column_int(stmt, 2);
			e = sqlite3_column_double(stmt, 3);
			mju = sqlite3_column_double(stmt, 4);
			ro = sqlite3_column_double(stmt, 5);
			const unsigned char* image_alias = sqlite3_column_text(stmt, 6);
			std::string sql_select_images_mid(reinterpret_cast<const char*>(image_alias));
			
			log_p->setTarget(LogTarget::Console);
			log_p->info("LOAD: collision_type = ", collision_type, ", e = ",
				e, ", mju = ", mju, ", ro = ", ro, ", image_alias = ", image_alias);

			std::string sql_select_images_nxt("\';");
			std::string sql_select_images = sql_select_images_pre + sql_select_images_mid +
				sql_select_images_nxt;
			sqlite3_stmt* stmt2;
			int rc2 = sqlite3_prepare_v2(db, sql_select_images.c_str(), -1, &stmt2, nullptr);
			TCHAR* tstr = nullptr;
			if (rc2 != SQLITE_OK)
			{
				log_p->setTarget(LogTarget::Console);
				log_p->error("in ", __func__, " \'stmt2\' : Failed to fetch data: ", sqlite3_errmsg(db));
			}
			else
			{
				if (sqlite3_step(stmt2) == SQLITE_ROW)
				{
					const unsigned char* path_name = sqlite3_column_text(stmt2, 0);
					log_p->setTarget(LogTarget::Console);
					log_p->info("path_name = ", path_name);
					std::string path_name_str(reinterpret_cast<const char*>(path_name));
					tstr = str_to_tchar(path_name_str);
				}
				else
				{
					log_p->setTarget(LogTarget::Console);
					log_p->warn("in ", __func__, " \'stmt2\' : No matched image path_name to fetch!",
						"So an empty image created to complete the blank of TileMap::img_tile_list.");
				}
			}
			switch (collision_type - 1)
			{
			case (int)CollisionType::Through:
				TileMap::insert_id2what_loadimage(CollisionType::Through, 0, 0, 0, tstr);
				break;
			case (int)CollisionType::Platform:
				TileMap::insert_id2what_loadimage(CollisionType::Platform, e, mju, 0, tstr);
				break;
			case (int)CollisionType::Wall:
				TileMap::insert_id2what_loadimage(CollisionType::Wall, e, mju, 0, tstr);
				break;
			case (int)CollisionType::Water:
				TileMap::insert_id2what_loadimage(CollisionType::Water, 0, mju, ro, tstr);
				break;
			}
			if (tstr)
				delete[] tstr;
			sqlite3_finalize(stmt2);			// ÊÍ·ÅÓï¾ä
		}
	}
	sqlite3_finalize(stmt);			// ÊÍ·ÅÓï¾ä

	closeDb();
}

DatabaseManager::DatabaseManager()
{
	db = nullptr;

	log_p->setTarget(LogTarget::Console);
	log_p->info("DatabaseManager initialized.");
}

void DatabaseManager::openDb()
{
	int rc = sqlite3_open("data/gamedata.db", &db);

	if (rc)
	{
		log_p->setTarget(LogTarget::Console);
		log_p->error("Can\'t open database: ", sqlite3_errmsg(db));
	}
	else
	{
		log_p->setTarget(LogTarget::Console);
		log_p->info("Opened database successfully!");
	}

}

void DatabaseManager::closeDb()
{
	sqlite3_close(db);
	db = nullptr;
}
