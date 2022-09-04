#include "input_text.h"
#include "window.h"
#include "child.h"


bool c_input_text::update()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c) return false;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd)
		return false;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return false;
	if (wnd->g_active_element != this && wnd->g_active_element != nullptr) return false;
	auto pos = c->get_cursor_position();
	auto size = Vector2D(g_size, element_height);
	if (label.size() > 0)
		pos.y += after_text_offset;
	bool h = hovered();
	if (active) {
		ImGui::GetIO().WantTextInput = true;

		if (ImGui::GetIO().InputQueueCharacters.Size > 0) {
			for (auto c : ImGui::GetIO().InputQueueCharacters) {
				if (c == VK_ESCAPE || c == VK_RETURN) {
					wnd->g_active_element = nullptr;
					active = false;
					return true;
				}
				else if (c == VK_BACK)
					*(string*)value = (*(string*)value).substr(0, (*(string*)value).size() - 1);
				else if (c != VK_TAB) {
					if ((*(string*)value).size() < 64)
						*(string*)value += (unsigned char)c;
				}
			}
		}
	
		if (b_switch) {
			if (pulsating >= 0.f)
				pulsating += animation_speed / 4.f;
			if (pulsating >= 1.f)
				b_switch = false;
		}
		else {
			if (pulsating <= 1.f)
				pulsating -= animation_speed / 4.f;
			if (pulsating <= 0.f)
				b_switch = true;
		}
		wnd->g_active_element = this;
	}
	else {
		if (wnd->g_active_element == this)
			wnd->g_active_element = nullptr;
		pulsating = 1.f;
		b_switch = false;
	}

	if (h) {
		wnd->g_hovered_element = this;

		if (animation < 1.f) animation += animation_speed;
		
		if (c->hovered() && wnd->is_click() && !wnd->g_active_element) {
			active = true;
			wnd->g_active_element = this;
			b_switch = false;
			return true;
		}

	}
	else {
		if (animation > 0.f) animation -= animation_speed;
		if (wnd->is_click() && active && wnd->g_active_element == this) {
			active = false;
			wnd->g_active_element = nullptr;
			pulsating = 1.f;
			b_switch = false;
			return true;
		}
	}
	animation = clamp(animation, 0.f, 1.f);
	pulsating = clamp(pulsating, 0.f, 1.f);

	return false;
}
bool c_input_text::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c->hovered())
		return false;
	auto pos = c->get_cursor_position();
	auto size = Vector2D(g_size, element_height);
	if (label.size() > 0)
		pos.y += after_text_offset;
	return g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y;
}
void c_input_text::render() {
	if (should_render)
		if (!should_render())
			return;
	c_child* c = (c_child*)child;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return;
	auto pos = c->get_cursor_position();
	auto alpha = (int)(wnd->get_transparency() * 2.55f) * (1.f - showing_animation);
	auto size = Vector2D(g_size, element_height);
	auto format = get_format();
	ImGui::PushFont(fonts::menu_desc);
	auto text_width = ImGui::CalcTextSize(format.c_str()).x;
	ImGui::PopFont();
	auto clr = menu_colors::background2.manage_alpha( alpha - 15.f * animation);
	auto clr2 = menu_colors::background1.manage_alpha(alpha - 15.f * animation);

	if (label.size() > 0) {
		g_Render->DrawString(pos.x, pos.y, color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::none, fonts::menu_desc, label.c_str());
		pos.y += after_text_offset;
	}
	//g_Render->filled_rect_gradient(pos.x, pos.y, size.x, size.y,
	//	clr2, clr2,
	//	clr, clr);

	g_Render->Rect(pos.x - 1, pos.y - 1, size.x + 2, size.y + 2,
		color_t(40 + 10 * (1.f - pulsating), 49 + 10 * (1.f - pulsating), 70 + 10 * (1.f - pulsating), alpha));

	int offset = 0;
	if (text_width + 20.f > size.x) {
		offset = size.x - text_width - 20.f;
	}
	g_Render->_drawList->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y), true);
	if (format.size() > 0) {
		g_Render->DrawString(pos.x + 10 + offset, pos.y + size.y / 2, color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::centered_y, fonts::menu_desc, format.c_str());
	}
	else if (!active)
	{
		g_Render->DrawString(pos.x + 10 + offset, pos.y + size.y / 2, color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::centered_y, fonts::menu_desc, "...");
	}
	g_Render->DrawLine(pos.x + 10 + text_width + offset, pos.y + 2, pos.x + 10 + text_width + offset, pos.y + size.y - 4,
		color_t(255, 255, 255, alpha * (1.f - pulsating)));
	g_Render->_drawList->PopClipRect();
}