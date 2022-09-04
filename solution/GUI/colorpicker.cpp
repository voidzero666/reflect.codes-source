#include "colorpicker.h"
#include "element.h"
#include "child.h"
#include "../colorpicker_bg.h"
#include "../alpha_slider_bg.h"

color_t hue_colors[7] = {
	color_t(255, 0, 0, 255),
	color_t(255, 255, 0, 255),
	color_t(0, 255, 0, 255),
	color_t(0, 255, 255, 255),
	color_t(0, 0, 255, 255),
	color_t(255, 0, 255, 255),
	color_t(255, 0, 0, 255)
};

void to_clipboard(const char* text)
{
	if (OpenClipboard(0))
	{
		EmptyClipboard();
		char* clip_data = (char*)(GlobalAlloc(GMEM_FIXED, MAX_PATH));
		lstrcpy(clip_data, text);
		SetClipboardData(CF_TEXT, (HANDLE)(clip_data));
		LCID* lcid = (DWORD*)(GlobalAlloc(GMEM_FIXED, sizeof(DWORD)));
		*lcid = MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL), SORT_DEFAULT);
		SetClipboardData(CF_LOCALE, (HANDLE)(lcid));
		CloseClipboard();
	}
}

string from_clipboard() {
	string fromClipboard;//в эту переменную сохраним текст из буфера обмена
	if (OpenClipboard(0))//открываем буфер обмена
	{
		HANDLE hData = GetClipboardData(CF_TEXT);//извлекаем текст из буфера обмена
		char* chBuffer = (char*)GlobalLock(hData);//блокируем память
		fromClipboard = chBuffer;
		GlobalUnlock(hData);//разблокируем память
		CloseClipboard();//закрываем буфер обмена
	}
	return fromClipboard;
}

void c_colorpicker::render_alphabg(Vector2D pos, float alpha)
{
	static LPDIRECT3DTEXTURE9 img;
	
	if (img == NULL)
		D3DXCreateTextureFromFileInMemory(g_Render->GetDevice(), &colorpicker_bg, sizeof(colorpicker_bg), &img);

	if (img)
		g_Render->_drawList->AddImage((void*)img, ImVec2(pos.x, pos.y), ImVec2(pos.x + 20, pos.y + 10), ImVec2(0, 0), ImVec2(1, 1),
			color_t(255, 255, 255, alpha).u32());
}

void c_colorpicker::render_sliderbg(Vector2D pos, float alpha)
{
	static LPDIRECT3DTEXTURE9 img;

	if (img == NULL)
		D3DXCreateTextureFromFileInMemory(g_Render->GetDevice(), &alpha_slider_bg, sizeof(alpha_slider_bg), &img);

	if (img)
		g_Render->_drawList->AddImage((void*)img, ImVec2(pos.x, pos.y), ImVec2(pos.x + 170, pos.y + 10), ImVec2(0, 0), ImVec2(1, 1),
			color_t(255, 255, 255, alpha).u32());
}
bool c_colorpicker::update()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c) return false;

	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return false;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return false;
	if (wnd->g_active_element != this && wnd->g_active_element != nullptr) return false;
	bool h = hovered();
	auto pos = c->get_cursor_position() + Vector2D(0, 5);
	pos.x += g_size + 15;
	auto size = Vector2D(20, 10);
	
	// bullshit animations
	if (h) {
		if (animation < 1.f) animation += animation_speed;
	}
	else {
		if (animation > 0.f) animation -= animation_speed;
	}
	animation = clamp(animation, 0.f, 1.f);
	
	if (h) {
		wnd->g_hovered_element = this;
		if (wnd->is_click()) {
			if (open) {
				wnd->g_active_element = nullptr;
				open = false;
			}
			else if (wnd->g_active_element == nullptr) {
				open = true;
				wnd->g_active_element = this;
			}
			update_colors();
			return true;
		}
		else if (wnd->left_click() && !open) {
			if (wnd->g_active_element == nullptr && !open) {
				dialogue.open = true;
				wnd->g_active_element = this;
				update_colors();
				return true;
			}
			
		}
		
	}
	if (dialogue.open) {
		pos.y += size.y + 2;
		size = Vector2D(80, 20);
		h = g_mouse.x >= pos.x && g_mouse.y >= pos.y
			&& g_mouse.x <= pos.x + size.x && g_mouse.y < pos.y + size.y * dialogue.elements.size();
		if (wnd->is_click() && !h) {
			dialogue.open = false;
			wnd->g_active_element = nullptr;
			wnd->reset_mouse();
			update_colors();
			return true;
		}
		for (size_t i = 0; i < dialogue.elements.size(); i++) {
			h = g_mouse.x >= pos.x && g_mouse.y >= pos.y
				&& g_mouse.x <= pos.x + size.x && g_mouse.y < pos.y + size.y;
			
			if (h) {
				wnd->g_hovered_element = this;
				if (dialogue.animations[i] < 1.f) dialogue.animations[i] += animation_speed;
			}
			else
			{
				if (dialogue.animations[i] > 0.f) dialogue.animations[i] -= animation_speed;
			}
			if (dialogue.animations[i] > 1.f) dialogue.animations[i] = 1.f;
			else if (dialogue.animations[i] < 0.f) dialogue.animations[i] = 0.f;
			if (wnd->is_click() && h) {
				if (i == 0) { // copy
					to_clipboard(std::to_string((*(color_t*)value).u32()).c_str());
				}
				else if (i == 1) { // paste
					(*(color_t*)value).set_u32(atoll(from_clipboard().c_str()));
					//update_colors();
				}
				dialogue.open = false;
				wnd->g_active_element = nullptr;
				wnd->reset_mouse();
				update_colors();
				return true;
			}
			pos.y += 20;
		}
	}
	else if (open) {	
		pos.y += size.y;
		size.x = 160 * 1.25f;
		bool full_hovered = h || (g_mouse.x > pos.x && g_mouse.y > pos.y
			&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + 165);


		if (!full_hovered && wnd->is_click()) {
			wnd->g_active_element = nullptr;
			open = false;
			wnd->reset_mouse();
			update_colors();
			return true;
		}
		bool s1 = g_mouse.x > pos.x + 15 && g_mouse.y > pos.y + 130
			&& g_mouse.x < pos.x + size.x - 15 && g_mouse.y < pos.y + 130 + size.y;

		bool s2 = g_mouse.x > pos.x + 15 && g_mouse.y > pos.y + 145
			&& g_mouse.x < pos.x + size.x - 15 && g_mouse.y < pos.y + 145 + size.y;
	
		bool hvrd = g_mouse.x > pos.x + 15 && g_mouse.y > pos.y + 15
			&& g_mouse.x < pos.x + size.x - 15 && g_mouse.y < pos.y + 120;
		if (s1 || s2 || hvrd)
			wnd->g_hovered_element = this;
		if (wnd->is_click() && g_mouse.x > pos.x + size.x - 12 && g_mouse.y > pos.y
			&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + 12) {
			wnd->g_active_element = nullptr;
			open = false;
			update_colors();
			wnd->reset_mouse();
			return true;
		}

		if (s1) {
			if (animations[0] < 1.f) animations[0] += animation_speed;
		}
		else {
			if (animations[0] > 0.f) animations[0] -= animation_speed;
		}
		if (s2) {
			if (animations[1] < 1.f) animations[1] += animation_speed;
		}
		else {
			if (animations[1] > 0.f) animations[1] -= animation_speed;
		}
		if (animations[0] > 1.f) animations[0] = 1.f;
		else if (animations[0] < 0.f) animations[0] = 0.f;
		if (animations[1] > 1.f) animations[1] = 1.f;
		else if (animations[1] < 0.f) animations[1] = 0.f;

		if (wnd->is_holding()) {
			if (s1)
				dragging_slider = 1;
			else if (s2)
				dragging_slider = 2;
			else if (hvrd)
				dragging_slider = 3;
		}
		else {
			if (!wnd->is_holding())
				dragging_slider = 0;
			else if (wnd->is_holding() && !s1 && dragging_slider == 1)
				dragging_slider = 1;
			else if (wnd->is_holding() && !s2 && dragging_slider == 2)
				dragging_slider = 2;
			else if (wnd->is_holding() && !hvrd && dragging_slider == 3)
				dragging_slider = 3;
			else
				dragging_slider = 0;
		}
		
		if (dragging_slider > 0) {
			float newpos_x = g_mouse.x - (pos.x + 15);
			float newpos_y = g_mouse.y - (pos.y + 15);

			if (newpos_x < 0) newpos_x = 0;
			if (newpos_x > (size.x - 30.f)) newpos_x = (size.x - 30.f);

			if (newpos_y < 0) newpos_y = 0;
			if (newpos_y > 105.f) newpos_y = 105.f;

			if (dragging_slider == 1)
				hue = clamp(newpos_x / (size.x - 30.f), 0.f, 1.f);
			else if (dragging_slider == 2)
				val_alpha = clamp(newpos_x / (size.x - 30.f), 0.f, 1.f);
			else if (dragging_slider == 3) {
				saturation = newpos_x / (size.x - 30.f);
				brightness = 1.f - newpos_y / 105.f;
			}
			clamp_colors();
			*(color_t*)value = color_t::hsb(hue, saturation, brightness);
			//if (val_alpha * 255.f <= 2.f)
			//	val_alpha = 0.f;
			(*(color_t*)value).set_alpha(val_alpha * 255.f);
		}
	}
	return false;
}

bool c_colorpicker::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	auto c = (c_child*)child;
	if (!c->hovered())
		return false;
	auto pos = c->get_cursor_position() + Vector2D(0, 5);
	pos.x += g_size + 15;
	auto size = Vector2D(20, 10);
	return g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y;
}

void c_colorpicker::render() {
	if (should_render)
		if (!should_render())
			return;
	auto c = (c_child*)child;
	if (!c) return;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return;

	auto pos = c->get_cursor_position() + Vector2D(0, 5);
	pos.x += g_size + 15;
	auto alpha = (int)(wnd->get_transparency() * 2.55f) * (1.f - showing_animation);

	auto size = Vector2D(20, 10);

	auto clr = *(color_t*)value;

	auto clr2 = color_t(150.f * animation, 150.f * animation, 150.f * animation, alpha);
	clr.set_alpha(alpha);
	render_alphabg(pos, alpha);
	auto color_faded = *(color_t*)value;
	color_faded.set_alpha(min(color_faded.get_alpha(), alpha));
	g_Render->filled_rect_gradient(pos.x, pos.y, size.x, size.y,
		clr, clr,
		color_faded, color_faded);

	g_Render->Rect(pos.x, pos.y, size.x, size.y, clr2);
	if (dialogue.open) {
		pos.y += size.y + 2;
		size = Vector2D(80, 20);
		g_Render->Rect(pos.x - 1, pos.y - 1, size.x + 2, size.y * (dialogue.elements.size()) + 2, color_t(0, 0, 0, alpha));

		for (size_t i = 0; i < dialogue.elements.size(); i++) {
			g_Render->FilledRect(pos.x, pos.y, size.x, size.y,
				color_t(27 + 15.f * dialogue.animations[i], 31 + 15.f * dialogue.animations[i], 40 + 15.f * dialogue.animations[i], alpha));
			auto clr2 = color_t(200 + dialogue.animations[i] * 55.f, 200 + dialogue.animations[i] * 55.f, 200 + dialogue.animations[i] * 55.f, alpha);
			g_Render->DrawString(pos.x + 10, pos.y + size.y / 2, clr2, render::centered_y,
				fonts::menu_desc, dialogue.elements[i].c_str());
			pos.y += size.y;
		}
	}
	else if (open) {
		size.x = 160 * 1.25f;
		pos.y += size.y;

		// background
		g_Render->Rect(pos.x, pos.y, size.x, 165, color_t(0, 0, 0, alpha));
		g_Render->FilledRect(pos.x + 1, pos.y + 1, size.x - 2, 163, menu_colors::background2.manage_alpha( alpha));

		color_t picker = color_t::hsb(hue, 1.f, 1.f).manage_alpha(alpha);

		// color selection area
		g_Render->filled_rect_gradient(pos.x + 15, pos.y + 15, size.x - 30, 105,
			color_t(255, 255, 255, alpha), picker,
			picker, color_t(255, 255, 255, alpha));
		g_Render->filled_rect_gradient(pos.x + 15, pos.y + 15, size.x - 30, 105,
			color_t(0, 0, 0, 0), color_t(0, 0, 0, 0),
			color_t(0, 0, 0, alpha), color_t(0, 0, 0, alpha));

		// close button
		g_Render->DrawString(pos.x + size.x - 6, pos.y + 6, color_t(255, 255, 255, alpha),
			render::centered_x | render::centered_y, fonts::menu_desc, "x");

		// hue slider
		float location_hue = hue * (size.x - 30.f);

		for (auto i = 0; i < 6; i++)
			g_Render->filled_rect_gradient(pos.x + 15 + i * ((size.x - 30.f) / 6.f), pos.y + 130, (size.x - 30.f) / 6.f, 10.f,
				hue_colors[i].manage_alpha(alpha), hue_colors[i + 1].manage_alpha(alpha),
				hue_colors[i + 1].manage_alpha(alpha), hue_colors[i].manage_alpha(alpha));

		g_Render->Rect(pos.x + 15, pos.y + 130, size.x - 30.f, size.y,
			color_t(75.f * animations[0], 75.f * animations[0], 75.f * animations[0], alpha));
		g_Render->DrawLine(pos.x + 15 + location_hue, pos.y + 130, pos.x + 15 + location_hue, pos.y + 130 + size.y,
			color_t(255, 255, 255, alpha), 2);


		// alpha slider
		float location_alpha = val_alpha * (size.x - 30.f);
		render_sliderbg(Vector2D(pos.x + 15, pos.y + 145), alpha);
		g_Render->filled_rect_gradient(pos.x + 15, pos.y + 145, size.x - 30.f, 10.f,
			menu_colors::background2.manage_alpha( 0), clr, clr, menu_colors::background2.manage_alpha( 0));
		g_Render->Rect(pos.x + 15, pos.y + 145, size.x - 30.f, size.y, 
			color_t(75.f * animations[1], 75.f * animations[1], 75.f * animations[1], alpha));		
		g_Render->DrawLine(pos.x + 15 + location_alpha, pos.y + 145, pos.x + 15 + location_alpha, pos.y + 145 + size.y,
			color_t(255, 255, 255, alpha), 2);

		
		g_Render->_drawList->AddCircle(ImVec2(pos.x + 15 + (size.x - 30) * saturation, pos.y + 15 + 105.f * (1.f - brightness)), 3,
			color_t(255, 255, 255, alpha).u32());
	}
}

