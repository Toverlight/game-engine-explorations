#include "tile_map_editor.h"

TileMapEditor::TileMapEditor()
{
	is_initialized = false;
	pTileMap = nullptr;
}

TileMapEditor::~TileMapEditor()
{
	delete pTileMap;
}

void TileMapEditor::on_enter()
{
	if (!is_initialized)
	{
		camera.set_coordinates(0, 0);
		gv_p->set_camera(&camera);
		// -----------------------------headBox-------------------------------
		headBox.set_layout_t(LayoutType::Absolute);
		headBox.set_bk_t(BkType::PureColor);
		headBox.set_color(RGB(255, 227, 158));
		headBox.set_size(800, 50);
		headBox.set_coordinates(0, 0);
		// -----------------------------headTitleBox-----------------------------
		headBox.add_elem(&headTitleBox);

		headTitleBox.set_layout_t(LayoutType::Absolute);
		headTitleBox.set_bk_t(BkType::Transparent);
		headTitleBox.set_size(150, headTitleBox.get_if_parent_size().y);
		headTitleBox.set_coordinates(0, 0);

		headTitleBox.add_elem(&textTitle);

		textTitle.set_center(CenterType::Rect);
		textTitle.set_bk_mode(TRANSPARENT);
		textTitle.set_text_color(RGB(168, 168, 168));
		textTitle.set_text_style(30, 12, _T("Arial Rounded MT Bold"));
		textTitle.set_content(_T("TTEditor~"));
		textTitle.set_using_camera(false);
		// ------------------------------headMenuBar---------------------------
		headBox.add_elem(&headMenuBar);

		headMenuBar.set_layout_t(LayoutType::LinearVertical);
		headMenuBar.set_bk_t(BkType::PureColor);
		headMenuBar.set_color(RGB(79, 155, 230));
		headMenuBar.set_size(800 - 150, headMenuBar.get_if_parent_size().y);
		headMenuBar.set_coordinates(150, 0);
		// ------------------------------headSfTop-----------------------------
		headMenuBar.add_elem(&headSfTop);

		headSfTop.set_weight(1);
		// -----------------------------headMenuButtonBar---------------------
		headMenuBar.add_elem(&headMenuButtonBar);

		headMenuButtonBar.set_layout_t(LayoutType::LinearHorizontal);
		headMenuButtonBar.set_bk_t(BkType::PureColor);
		headMenuButtonBar.set_color(RGB(125, 221, 171));
		headMenuButtonBar.set_weight(3);
		// ----------------------------headButtonNew---------------------------
		headMenuButtonBar.add_elem(&headButtonNew);

		headButtonNew.set_weight(1);
		headButtonNew.set_line_color(RGB(31, 31, 31));
		headButtonNew.set_fill_color(RGB(31, 31, 31));
		headButtonNew.set_fill_touched_color(RGB(61, 61, 61));
		headButtonNew.set_fill_down_color(RGB(113, 96, 232));
		headButtonNew.get_text().set_center(CenterType::Rect);
		headButtonNew.get_text().set_text_color(RGB(181, 181, 181));
		headButtonNew.get_text().set_text_style(28, 14, _T("Consolas"));
		headButtonNew.get_text().set_content(_T("新建(N)"));
		headButtonNew.set_callback([&]()
			{

			});
		// -----------------------------headButtonLoad-------------------------
		headMenuButtonBar.add_elem(&headButtonLoad);

		headButtonLoad.set_weight(1);
		headButtonLoad.set_line_color(RGB(31, 31, 31));
		headButtonLoad.set_fill_color(RGB(31, 31, 31));
		headButtonLoad.set_fill_touched_color(RGB(61, 61, 61));
		headButtonLoad.set_fill_down_color(RGB(113, 96, 232));
		headButtonLoad.get_text().set_center(CenterType::Rect);
		headButtonLoad.get_text().set_text_color(RGB(181, 181, 181));
		headButtonLoad.get_text().set_text_style(28, 14, _T("Consolas"));
		headButtonLoad.get_text().set_content(_T("加载(L)"));
		headButtonLoad.set_callback([&]()
			{

			});
		// ----------------------------headButtonSave--------------------------
		headMenuButtonBar.add_elem(&headButtonSave);

		headButtonSave.set_weight(1);
		headButtonSave.set_line_color(RGB(31, 31, 31));
		headButtonSave.set_fill_color(RGB(31, 31, 31));
		headButtonSave.set_fill_touched_color(RGB(61, 61, 61));
		headButtonSave.set_fill_down_color(RGB(113, 96, 232));
		headButtonSave.get_text().set_center(CenterType::Rect);
		headButtonSave.get_text().set_text_color(RGB(181, 181, 181));
		headButtonSave.get_text().set_text_style(28, 14, _T("Consolas"));
		headButtonSave.get_text().set_content(_T("保存(S)"));
		headButtonSave.set_callback([&]()
			{
				std::cout << "save!" << std::endl;
			});
		// ---------------------------headButtonSaveAs------------------------
		headMenuButtonBar.add_elem(&headButtonSaveAs);

		headButtonSaveAs.set_weight(1);
		headButtonSaveAs.set_line_color(RGB(31, 31, 31));
		headButtonSaveAs.set_fill_color(RGB(31, 31, 31));
		headButtonSaveAs.set_fill_touched_color(RGB(61, 61, 61));
		headButtonSaveAs.set_fill_down_color(RGB(113, 96, 232));
		headButtonSaveAs.get_text().set_center(CenterType::Rect);
		headButtonSaveAs.get_text().set_u_format(DT_CENTER);
		headButtonSaveAs.get_text().set_text_color(RGB(181, 181, 181));
		headButtonSaveAs.get_text().set_text_style(28, 14, _T("Consolas"));
		headButtonSaveAs.get_text().set_content(_T("另存(K)"));
		headButtonSaveAs.set_callback([&]()
			{
				if (boardSaveAs.check_visible())
					boardSaveAs.batch_off();
				else
					boardSaveAs.batch_on();

				//std::cout 
				//	<< "shot: nWidth = " << headButtonSaveAs.get_text().get_text_info(TextInfo::W).nWidth
				//	<< ", nHeight = " << headButtonSaveAs.get_text().get_text_info(TextInfo::H).nHeight
				//	<< ", get_textwidth = " << headButtonSaveAs.get_text().get_textwidth() 
				//	<< ", tchar_num = " << headButtonSaveAs.get_text().get_tchar_num()
				//	<< ", textwidth = " << textwidth(headButtonSaveAs.get_text().get_content())
				//	<< ", size = { "
				//	<< headButtonSaveAs.get_text().get_size().x << ", "
				//	<< headButtonSaveAs.get_text().get_size().y << " };" 
				//	<< std::endl;
			});
		// ---------------------------boardSaveAs----------------------------
		boardSaveAs.set_layout_t(LayoutType::LinearVertical);
		boardSaveAs.set_bk_t(BkType::PureColor);
		boardSaveAs.set_color(RGB(150, 150, 150));
		boardSaveAs.set_size(200, 180);
		// ---------------------------boardTip------------------------------
		boardSaveAs.add_elem(&boardTip);

		boardTip.set_weight(2);
		boardTip.set_layout_t(LayoutType::Absolute);
		boardTip.set_bk_t(BkType::PureColor);
		boardTip.set_color(RGB(123, 234, 12));
		// ----------------------------textSaveAsTip-------------------------
		boardTip.add_elem(&textSaveAsTip);

		textSaveAsTip.set_center(CenterType::Rect);
		textSaveAsTip.set_u_format(DT_CENTER);
		textSaveAsTip.set_bk_mode(TRANSPARENT);
		textSaveAsTip.set_text_color(BLACK);
		textSaveAsTip.set_text_style(16, 8, _T("华文琥珀 常规"));
		textSaveAsTip.set_content(_T("请输入要保存的地图名称："));
		// ---------------------------boardSaveAsTf-------------------------
		boardSaveAs.add_elem(&boardSaveAsTf);

		boardSaveAsTf.set_weight(3);
		boardSaveAsTf.set_layout_t(LayoutType::Absolute);
		boardSaveAsTf.set_bk_t(BkType::PureColor);
		boardSaveAsTf.set_color(RGB(65, 170, 38));
		// ----------------------------tfSaveAs------------------------------
		boardSaveAsTf.add_elem(&tfSaveAs);

		tfSaveAs.set_bkcolor(WHITE);
		tfSaveAs.set_fieldwidth(180);
		tfSaveAs.set_size(180, 20);
		tfSaveAs.set_coordinates(10, 10);
		// ----------------------------boardSaveAsButton---------------------
		boardSaveAs.add_elem(&boardSaveAsButton);

		boardSaveAsButton.set_weight(2);
		boardSaveAsButton.set_layout_t(LayoutType::Absolute);
		boardSaveAsButton.set_bk_t(BkType::PureColor);
		boardSaveAsButton.set_color(RGB(214, 142, 224));
		// ----------------------------buttonSaveAs--------------------------
		boardSaveAsButton.add_elem(&buttonSaveAs);

		buttonSaveAs.set_line_color(RGB(191, 191, 191));
		buttonSaveAs.set_fill_color(RGB(191, 191, 191));
		buttonSaveAs.set_fill_touched_color(RGB(222, 222, 222));
		buttonSaveAs.set_fill_down_color(RGB(79, 79, 79));
		buttonSaveAs.get_text().set_center(CenterType::Rect);
		buttonSaveAs.get_text().set_u_format(DT_CENTER);
		buttonSaveAs.get_text().set_text_color(BLACK);
		buttonSaveAs.get_text().set_text_style(26, 12, _T("Consolas"));
		buttonSaveAs.get_text().set_content(_T("提交"));
		buttonSaveAs.set_size(100, 30);
		buttonSaveAs.set_coordinates(50, 10);
		buttonSaveAs.set_callback([&]()
			{
				std::cout << "submit!" << std::endl;
			});
		// -----------------------------headButtonEdit-----------------------
		headMenuButtonBar.add_elem(&headButtonEdit);

		headButtonEdit.set_weight(1);
		headButtonEdit.set_line_color(RGB(31, 31, 31));
		headButtonEdit.set_fill_color(RGB(31, 31, 31));
		headButtonEdit.set_fill_touched_color(RGB(61, 61, 61));
		headButtonEdit.set_fill_down_color(RGB(113, 96, 232));
		headButtonEdit.get_text().set_center(CenterType::Rect);
		headButtonEdit.get_text().set_text_color(RGB(181, 181, 181));
		headButtonEdit.get_text().set_text_style(28, 14, _T("Consolas"));
		headButtonEdit.get_text().set_content(_T("编辑(E)"));
		headButtonEdit.set_callback([&]()
			{

			});
		// -----------------------------headButtonConfig---------------------
		headMenuButtonBar.add_elem(&headButtonConfig);

		headButtonConfig.set_weight(1);
		headButtonConfig.set_line_color(RGB(31, 31, 31));
		headButtonConfig.set_fill_color(RGB(31, 31, 31));
		headButtonConfig.set_fill_touched_color(RGB(61, 61, 61));
		headButtonConfig.set_fill_down_color(RGB(113, 96, 232));
		headButtonConfig.get_text().set_center(CenterType::Rect);
		headButtonConfig.get_text().set_text_color(RGB(181, 181, 181));
		headButtonConfig.get_text().set_text_style(28, 14, _T("Consolas"));
		headButtonConfig.get_text().set_content(_T("配置(C)"));
		headButtonConfig.set_callback([&]()
			{

			});
		// -----------------------------headSfBottom-------------------------
		headMenuBar.add_elem(&headSfBottom);

		headSfBottom.set_weight(1);
		// headBox upd---------------------------------------------------------------
		headBox.update_layout();
		headBox.batch_on();
		gv_p->register_component(&headBox);

		boardSaveAs.set_parent_component(&headButtonSaveAs);
		boardSaveAs.set_coordinates(-50, headButtonSaveAs.get_size().y);

		boardSaveAs.update_layout();

		// --------------------------bottomBax----------------------------
		bottomBox.set_layout_t(LayoutType::LinearHorizontal);
		bottomBox.set_bk_t(BkType::PureColor);
		bottomBox.set_color(RGB(25, 227, 15));
		bottomBox.set_size(getwidth(), 40);
		bottomBox.set_coordinates(0, getheight() - 40);
		// -------------------------bottomNameBox-------------------------
		bottomBox.add_elem(&bottomNameBox);

		bottomNameBox.set_layout_t(LayoutType::Absolute);
		bottomNameBox.set_bk_t(BkType::PureColor);
		bottomNameBox.set_color(RGB(35, 56, 29));
		bottomNameBox.set_weight(3);
		// -------------------------bottomMassageBox----------------------
		bottomBox.add_elem(&bottomMassageBox);

		bottomMassageBox.set_layout_t(LayoutType::Absolute);
		bottomMassageBox.set_bk_t(BkType::PureColor);
		bottomMassageBox.set_color(RGB(89, 100, 142));
		bottomMassageBox.set_weight(5);
		// -------------------------bottomRowColBox-----------------------
		bottomBox.add_elem(&bottomRowColBox);

		bottomRowColBox.set_layout_t(LayoutType::LinearHorizontal);
		bottomRowColBox.set_bk_t(BkType::PureColor);
		bottomRowColBox.set_color(RGB(22, 85, 149));
		bottomRowColBox.set_weight(2);
		// --------------------------leftBox------------------------------
		leftBox.set_layout_t(LayoutType::Absolute);
		leftBox.set_bk_t(BkType::PureColor);
		leftBox.set_color(RGB(255, 227, 15));
		leftBox.set_size(80, getheight() - headBox.get_size().y - bottomBox.get_size().y);
		leftBox.set_coordinates(0, headBox.get_size().y);


		// leftBox upd---------------------------------------------------------------
		leftBox.update_layout();
		leftBox.batch_on();
		gv_p->register_component(&leftBox);
		// --------------------------rightBox-----------------------------
		rightBox.set_layout_t(LayoutType::Absolute);
		rightBox.set_bk_t(BkType::PureColor);
		rightBox.set_color(RGB(255, 27, 15));
		rightBox.set_size(40, getheight() - headBox.get_size().y - bottomBox.get_size().y);
		rightBox.set_coordinates(getwidth() - 40, headBox.get_size().y);
		// rightBox upd---------------------------------------------------------------
		rightBox.update_layout();
		rightBox.batch_on();
		gv_p->register_component(&rightBox);
		// bottomBox upd---------------------------------------------------------------
		bottomBox.update_layout();
		bottomBox.batch_on();
		gv_p->register_component(&bottomBox);

		is_initialized = true;
	}

}

void TileMapEditor::on_input()
{
	gv_p->batch_input();
}

void TileMapEditor::on_update()
{
	gv_p->batch_update();
}

void TileMapEditor::on_draw()
{
	gv_p->batch_draw();
}
