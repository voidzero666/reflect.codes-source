#include "child.h"
#include "window.h"
#include "../menu.h"

c_element* c_child::get_parent() {
	return this->child;
}
bool c_child::hovered() {
	auto pos = ((c_window*)child)->get_cursor_position() + get_position();
	auto size = this->get_size();
	bool m = scroll.value < total_elements_size - size.y + 20.f;
	return g_mouse.x > pos.x && g_mouse.y > pos.y + (scroll.value > 0) * 20.f
		&& g_mouse.x < pos.x + size.x&& g_mouse.y < pos.y + size.y - m * 20.f;
}

void c_child::initialize_elements() {
	for (auto &e : elements) {
		e->child = this;
		e->tab = this->tab;
	}
}

void c_child::lock_bounds() {
	auto pos = ((c_window*)child)->get_cursor_position() + get_position();
	auto size = this->get_size();
	auto m = Vector2D(pos.x + size.x, pos.y + size.y);
	g_Render->_drawList->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(m.x, m.y), true);
}

void c_child::unlock_bounds() {
	g_Render->_drawList->PopClipRect();
}

void c_child::render() {
	c_window* wnd = (c_window*)child;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return;
	auto pos = wnd->get_cursor_position() + get_position();
	auto size = get_size();
	int alpha = (int)(this->get_transparency() * 2.55f) * (1.f - showing_animation);
	
	//g_Render->FilledRect(pos.x, pos.y, size.x, size.y, menu_colors::background2.manage_alpha( alpha));

	Vector2D backup_cursor_pos = Vector2D(pos.x + 45, pos.y + 15);
	
	if (should_draw_scroll)
		backup_cursor_pos.y -= scroll.value;
	
	set_cursor_position(backup_cursor_pos);
	if (!scroll.dragging)
		update_elements();

	set_cursor_position(backup_cursor_pos);
	lock_bounds();
	render_elements();
	unlock_bounds();
	int max_scroll_value = total_elements_size - size.y + 20.f;

	if (should_draw_scroll) {

		//g_Render->FilledRect(pos.x + size.x - 8, pos.y + 1, 8, size.y - 2, menu_colors::background1.manage_alpha(alpha));
		if (scroll.value > 0) {
			g_Render->filled_rect_gradient(pos.x, pos.y - 1, size.x, 20,
				menu_colors::background1.manage_alpha(alpha), menu_colors::background1.manage_alpha(alpha),
				menu_colors::background1.manage_alpha(0), menu_colors::background1.manage_alpha(0));

			/*Vector2D base = Vector2D(pos.x + size.x - 13, pos.y + 6);
			auto new_alpha = alpha * (scroll.value / max_scroll_value);
			g_Render->_drawList->AddTriangleFilled(
				ImVec2(base.x, base.y - 3), ImVec2(base.x - 3, base.y + 3.5f), ImVec2(base.x + 3, base.y + 3.5f),
				color_t(60, 70, 90, new_alpha).u32());*/
		}
		if (scroll.value < max_scroll_value) {
			g_Render->filled_rect_gradient(pos.x, pos.y + size.y - 20, size.x, 20,
				menu_colors::background1.manage_alpha(0), menu_colors::background1.manage_alpha(0),
				menu_colors::background1.manage_alpha(alpha), menu_colors::background1.manage_alpha(alpha));

			/*Vector2D base = Vector2D(pos.x + size.x - 13, pos.y + size.y - 6);
			auto new_alpha = alpha * (1.f - (scroll.value / max_scroll_value));
			g_Render->_drawList->AddTriangleFilled(
				ImVec2(base.x, base.y + 3.5f), ImVec2(base.x - 3.5f, base.y - 3.5f), ImVec2(base.x + 3.5f, base.y - 3.5f),
				color_t(60, 70, 90, new_alpha).u32());*/
		}

		auto scroll_pos = scroll.value / total_elements_size * (size.y - 40.f);
		auto scroll_pos_max = max_scroll_value / (float)total_elements_size * (size.y - 40.f);

		g_Render->FilledRect(pos.x + size.x - 4, pos.y + scroll_pos, 3, size.y - scroll_pos_max,
			menu_colors::main_color.manage_alpha(alpha - 100.f + 100.f * scroll.animation));
	}
	ImGui::PushFont(fonts::menu_main);
	float text_size = ImGui::CalcTextSize(get_title().c_str()).x + 16;
	ImGui::PopFont();

	g_Render->filled_rect_gradient(pos.x, pos.y - 1, size.x / 2 - text_size / 2, 3,
		menu_colors::main_color.manage_alpha(0), menu_colors::main_color.manage_alpha(alpha), menu_colors::main_color.manage_alpha(alpha), menu_colors::main_color.manage_alpha(0));

	g_Render->filled_rect_gradient(pos.x + size.x / 2 + text_size / 2, pos.y - 1, size.x / 2 - text_size / 2, 3,
		menu_colors::main_color.manage_alpha(alpha), menu_colors::main_color.manage_alpha(0), menu_colors::main_color.manage_alpha(0), menu_colors::main_color.manage_alpha(alpha));


	g_Render->DrawString(pos.x + size.x / 2, pos.y, color_t(255, 255, 255, alpha),
		render::centered_y | render::centered_x, fonts::menu_main, get_title().c_str(), total_elements_size, scroll);
}
int c_child::get_total_offset() { return (int)get_size().y + 10; }
int c_child::get_upper_offset() {
	return 40;
}
bool c_child::update()
{
	c_window* wnd = (c_window*)child;
	if (g_active_element) {
		if (g_active_element != this)
			return false;
	}
	auto pos = wnd->get_cursor_position() + get_position();
	auto size = get_size();
	bool hvrd = g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x&& g_mouse.y < pos.y + size.y;
	should_draw_scroll = total_elements_size > size.y - 20.f;
	bool hovering_listbox = wnd->g_hovered_element && wnd->g_hovered_element->type == c_elementtype::listbox;

	if (hvrd && !wnd->g_active_element && should_draw_scroll && !hovering_listbox) {
		while (csgo->scroll_amount > 0) {
			scroll.value -= 20;
			csgo->scroll_amount--;
		}
		while (csgo->scroll_amount < 0) {
			scroll.value += 20;
			csgo->scroll_amount++;
		}
	}

	int max_scroll_value = clamp(total_elements_size - size.y + 20.f, 0.f, FLT_MAX);

	scroll.value = clamp(scroll.value, 0.f, (float)max_scroll_value);

	auto scroll_pos = scroll.value / total_elements_size * (size.y - 40.f);
	auto scroll_pos_max = max_scroll_value / (float)total_elements_size * (size.y - 40.f);

	bool h_scroll = g_mouse.x > pos.x + size.x - 5 && g_mouse.y > pos.y + scroll_pos
		&& g_mouse.x < pos.x + size.x - 2 && g_mouse.y < pos.y + scroll_pos + size.y - scroll_pos_max;

	bool wnd_holding = wnd->is_holding();

	if (should_draw_scroll && !hovering_listbox) {

		
		if (h_scroll && !wnd->is_holding_menu() && (!wnd->g_active_element || wnd->g_active_element == this)) {
			if (scroll.animation < 1.f)
				scroll.animation += animation_speed;
			wnd->g_hovered_element = this;
			scroll.dragging = wnd_holding;			
		}
		else {
			if (wnd_holding && scroll.dragging) {
				scroll.dragging = true;
			}
			else {
				if (!wnd_holding)
					scroll.dragging = false;
				if (wnd->g_hovered_element == this)
					wnd->g_hovered_element = nullptr;
				if (scroll.animation > 0.f)
					scroll.animation -= animation_speed;
			}
		}
		if (scroll.dragging) {
			scroll.value += ImGui::GetIO().MouseDelta.y * (total_elements_size / (size.y - 40.f));
			wnd->g_active_element = this;
		}
		else {
			if (wnd->g_active_element == this)
				wnd->g_active_element = nullptr;
		}
		
		scroll.value = clamp(scroll.value, 0.f, clamp(total_elements_size - size.y + 20.f, 0.f, FLT_MAX));
	}
	else {
		scroll.value = 0.f;
		scroll.dragging = false;
	}
	scroll.animation = clamp(scroll.animation, 0.f, 1.f);
	set_transparency(wnd->get_transparency());
	return false;
}
void c_child::update_elements()
{
	auto wnd = (c_window*)child;
	total_elements_size = 0;
	bool b = false;
	
	for (auto e : elements) {
		if (e->tab != wnd->get_active_tab_index()) {
			e->showing_animation = 1.f;
			continue;
		}
		if (e->showing_animation > 0.f)
			e->showing_animation -= animation_speed * 0.5f;
		if (e->showing_animation < 0.f)
			e->showing_animation = 0.f;

		if (!b)
			e->update();

		int offset = e->get_total_offset();
		if (offset > 0) {
			apply_element(offset + 3);
			total_elements_size += offset + 3;
		}

		if (e == wnd->g_active_element)
			b = true;
	}
	
}

void c_child::render_elements()
{
	auto wnd = (c_window*)child;
	for (auto e : elements) {
		if (e->tab != wnd->get_active_tab_index())
			continue;
		if (e == wnd->g_active_element) {
			wnd->g_active_element_pos = get_cursor_position();
			wnd->active_element_parent = this;
			int offset = e->get_total_offset();
			if (offset > 0)
				apply_element(offset + 3);
		}
		else {
			auto cursor = get_cursor_position();
			auto chld_pos = wnd->get_cursor_position() + get_position();
			auto size_y = e->get_total_offset();
			if (cursor.y >= chld_pos.y - size_y
				&& cursor.y <= chld_pos.y + get_size().y)
				e->render();
			int offset = e->get_total_offset();
			if (offset > 0)
				apply_element(offset + 3);
		}
	}
};