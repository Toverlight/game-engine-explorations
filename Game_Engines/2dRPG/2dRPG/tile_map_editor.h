#ifndef _TILE_MAP_EDITOR_H_
#define _TILE_MAP_EDITOR_H_

#include "text.h"
#include "scene.h"
#include "layout.h"
#include "tile_map.h"
#include "text_field.h"
#include "rect_button.h"
#include "space_filler.h"
#include "database_manager.h"

extern GVar* gv_p;
extern Log* log_p;

// 地图编辑器
class TileMapEditor : public Scene
{
public:
	TileMapEditor();
	~TileMapEditor();

	void on_enter() override;

	void on_input() override;

	void on_update() override;

	void on_draw() override;

	void on_exit() override {};
private:
	bool is_initialized;				// 是否初始化完成

private:
	Camera camera;						// 相机

	TileMap* pTileMap;					// 地图指针

	// -----------------------布局--------------------------------

	Layout headBox;						// 顶部容器
	Layout headTitleBox;				// v>> 顶部标题容器
	Text textTitle;						// ->> ->> 标题文本
	Layout headMenuBar;					// v>> 顶部菜单栏
	SpaceFiller headSfTop;				// ->> ->> 上填充
	Layout headMenuButtonBar;			// v>> v>> 顶部按钮栏
	RectButton headButtonNew;			// ->> ->> ->> 新建按钮
	RectButton headButtonLoad;			// ->> ->> ->> 加载按钮
	RectButton headButtonSave;			// ->> ->> ->> 保存按钮
	RectButton headButtonSaveAs;		// v>> v>> v>> 另存为按钮
	Layout boardSaveAs;					// v>> v>> v>> v>> 另存功能指示板
	Layout boardTip;					// v>> v>> v>> v>> v>> 提示文字容器
	Text textSaveAsTip;					// ->> ->> ->> ->> ->> ->> 另存为提示文字标签
	Layout boardSaveAsTf;				// v>> v>> v>> v>> v>> 另存为文本框容器
	TextField tfSaveAs;					// ->> ->> ->> ->> ->> ->> 另存为文本框
	Layout boardSaveAsButton;			// v>> v>> v>> v>> v>> 另存决定按钮容器
	RectButton buttonSaveAs;			// ->> ->> ->> ->> ->> ->> 另存决定按钮
	RectButton headButtonEdit;			// ->> ->> ->> 编辑按钮
	RectButton headButtonConfig;		// ->> ->> ->> 配置按钮
	SpaceFiller headSfBottom;			// ->> ->> 下填充

	Layout leftBox;						// 左部容器

	Layout rightBox;					// 右部容器

	Layout bottomBox;					// 底部容器
	Layout bottomNameBox;				// v>> 底部地图名称容器
	Layout bottomMassageBox;			// v>> 底部消息容器
	Layout bottomRowColBox;				// v>> 底部行列消息容器	

};

#endif // !_TILE_MAP_EDITOR_H_

