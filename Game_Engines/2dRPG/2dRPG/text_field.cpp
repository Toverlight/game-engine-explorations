#include "text_field.h"

TextField::TextField()
{
	is_cursor_showing = true;

	offset = 0;								// 偏移量始终指向光标位置
	tstr = new TCHAR[TF_TSTR_LEN_MAX];
	*tstr = _T('|');						// 光标符号（算入宽度）
	*(tstr + 1) = _T('\0');

	text.set_parent_component(this);
	text.set_bk_mode(TRANSPARENT);
	text.set_text_color(BLACK);
	text.set_text_style(16, 8, _T("Consolas"));
	text.set_center(CenterType::VerticalOnly);
	text.set_u_format(DT_LEFT | DT_WORD_ELLIPSIS);
	text.set_coordinates(5, 0);
	text.set_content(tstr);
	
	field_width = 100;

	bk_color = WHITE;

	is_focused = false;

	timer_cursor_flash.set_one_shot(false);
	timer_cursor_flash.set_wait_time(500);
	timer_cursor_flash.set_callback([&]()
		{
			if (is_cursor_showing = !is_cursor_showing)
				*(tstr + offset) = _T('|');
			else
				*(tstr + offset) = _T(' ');
		});

	tcBuf = _T('\0');

	is_capslock_on = false;
	is_shift_down = false;
	is_backspacing = false;
}

TextField::TextField(int width)
{
	is_cursor_showing = true;

	offset = 0;								// 偏移量始终指向光标位置
	tstr = new TCHAR[TF_TSTR_LEN_MAX];
	*tstr = _T('|');						// 光标符号（算入宽度）
	*(tstr + 1) = _T('\0');

	text.set_parent_component(this);
	text.set_bk_mode(TRANSPARENT);
	text.set_text_color(BLACK);
	text.set_text_style(16, 8, _T("Consolas"));
	text.set_center(CenterType::VerticalOnly);
	text.set_u_format(DT_LEFT | DT_WORD_ELLIPSIS);
	text.set_coordinates(5, 0);
	text.set_content(tstr);

	field_width = width;

	bk_color = WHITE;

	is_focused = false;

	timer_cursor_flash.set_one_shot(false);
	timer_cursor_flash.set_wait_time(500);
	timer_cursor_flash.set_callback([&]()
		{
			if (is_cursor_showing = !is_cursor_showing)
				*(tstr + offset) = _T('|');
			else
				*(tstr + offset) = _T(' ');
		});

	tcBuf = _T('\0');

	is_capslock_on = false;
	is_shift_down = false;
	is_backspacing = false;

}

TextField::~TextField()
{
	delete[] tstr;
}

void TextField::on_input()
{

	if (gv_p->check_if_mouseclick_msg())
	{
		const Vector2& abs_coordinates = get_abs_coordinates();
		const ExMessage& mouseclick_msg = gv_p->get_mouseclick_msg();
		if (mouseclick_msg.message == WM_LBUTTONDOWN)
		{
			if (mouseclick_msg.x >= abs_coordinates.x &&
				mouseclick_msg.x <= abs_coordinates.x + get_size().x &&
				mouseclick_msg.y >= abs_coordinates.y &&
				mouseclick_msg.y <= abs_coordinates.y + get_size().y)
			{
				is_focused = true;
				gv_p->clear_mouseclick_msg();
			}
			else
			{
				is_focused = false;
			}
		}
	}

	if (gv_p->check_if_key_msg())
	{
		const ExMessage& key_msg = gv_p->get_key_msg();
		if (key_msg.vkcode == VK_ESCAPE)
		{
			if (is_focused)
			{
				is_focused = false;
				gv_p->clear_key_msg();
			}
		}
		if (key_msg.vkcode == VK_SHIFT)
		{
			if (!is_shift_down) 
			{
				is_shift_down = true;
				gv_p->clear_key_msg();
			}
		}
		is_capslock_on = isCapsLockOn();

		if (is_focused)
		{
			if (key_msg.message == WM_KEYDOWN)
			{
				if (key_msg.vkcode >= 'A' && key_msg.vkcode <= 'Z')
				{
					if (is_capslock_on && !is_shift_down || !is_capslock_on && is_shift_down)
						tcBuf = key_msg.vkcode;
					else
						tcBuf = key_msg.vkcode + 32;
					gv_p->clear_key_msg();
				}
				else if (key_msg.vkcode >= '0' && key_msg.vkcode <= '9')
				{
					if (!is_shift_down)
					{
						tcBuf = key_msg.vkcode;
						gv_p->clear_key_msg();
					}
					else
					{
						switch (key_msg.vkcode)
						{
						case '3':
							tcBuf = _T('#');
							gv_p->clear_key_msg();
							break;
						case '9':
							tcBuf = _T('(');
							gv_p->clear_key_msg();
							break;
						case '0':
							tcBuf = _T(')');
							gv_p->clear_key_msg();
							break;
						}
					}
					
				}
				else if (key_msg.vkcode == ' ')
				{
					tcBuf = key_msg.vkcode;
					gv_p->clear_key_msg();
				}
				else if (key_msg.vkcode == VK_OEM_MINUS)
				{
					if (!is_shift_down)
						tcBuf = _T('-');
					else
						tcBuf = _T('_');
					gv_p->clear_key_msg();
				}
				else if (key_msg.vkcode == VK_BACK)
				{
					is_backspacing = true;
					gv_p->clear_key_msg();
				}
			}
			else if (key_msg.message == WM_KEYUP)
			{
				if (key_msg.vkcode == VK_BACK)
				{
					is_backspacing = false;
					gv_p->clear_key_msg();
				}
				if (key_msg.vkcode == VK_SHIFT)
				{
					if (is_shift_down) 
					{
						is_shift_down = false;
						gv_p->clear_key_msg();
					}
				}
			}
		}
	}
}

void TextField::on_update()
{
	int length = get_text_width();
	if (tcBuf)
	{
		if (length + text.get_text_info(TextInfo::W).nWidth <= field_width)
		{
			if (offset + 2 < TF_TSTR_LEN_MAX)
			{
				*(tstr + offset + 2) = _T('\0');
				*(tstr + offset + 1) = *(tstr + offset);
				*(tstr + offset) = tcBuf;
				++offset;
			}
		}
		reset_tcBuf();
	}
	if (is_backspacing)
	{
		if (offset > 0)
		{
			--offset;
			*(tstr + offset) = *(tstr + offset + 1);
			*(tstr + offset + 1) = _T('\0');
		}
		is_backspacing = false;
	}

	if (is_focused)
	{
		timer_cursor_flash.on_update();
	}
	else
	{
		if (*(tstr + offset) == _T('|'))
			*(tstr + offset) = _T(' ');
	}
}

void TextField::on_draw()
{
	const Vector2& abs_coordinates = get_abs_coordinates();
	const Vector2& size = get_size();
	setfillcolor(bk_color);
	solidrectangle(abs_coordinates.x, abs_coordinates.y, abs_coordinates.x + size.x,
		abs_coordinates.y + size.y, false);
	if (gv_p->check_debug())					// 在调试模式下
	{
		setlinecolor(RED);
		rectangle(abs_coordinates.x, abs_coordinates.y, abs_coordinates.x + size.x,
			abs_coordinates.y + size.y, false);
	}
}
