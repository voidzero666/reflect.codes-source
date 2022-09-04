#include "ragebot_subtabs.h"
#include "child.h"

#define icons_scale 0.5f

bool c_ragebot_subtabs::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	auto wnd = (c_child*)child;
	auto pos = wnd->get_cursor_position();
	if (!wnd->hovered())
		return false;
	auto size = Vector2D(g_size + 90, 42 * icons_scale * 2);
	if (label.size() > 0)
		pos.y += after_text_offset;
	return g_mouse.x >= pos.x && g_mouse.y >= pos.y
		&& g_mouse.x <= pos.x + size.x && g_mouse.y <= pos.y + size.y;
}
bool c_ragebot_subtabs::update()
{
	if (should_render)
		if (!should_render())
			return false;

	auto wnd = (c_window*)child;
	if (!wnd) return false;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return false;
	if (wnd->g_active_element != this && wnd->g_active_element != nullptr) return false;
	auto pos = wnd->get_cursor_position();
	auto size = Vector2D(g_size + 90, 42 * icons_scale * 2);

	bool h = hovered();
	if (h) wnd->g_hovered_element = this;
	if (label.size() > 0)
		pos.y += after_text_offset;
	if (h && wnd->hovered() && wnd->is_click()) {
		open = !open;
		wnd->g_active_element = this;
		return true;
	}
	if (open) {
		if (open_animation < 1.f)
			open_animation += animation_speed;
		h = g_mouse.x >= pos.x && g_mouse.y >= pos.y
			&& g_mouse.x <= pos.x + size.x && g_mouse.y < pos.y + size.y * (elements.size() + 1);
		if (wnd->is_click() && !h) {
			open = false;
			wnd->g_active_element = nullptr;
			wnd->reset_mouse();
			return true;
		}
		if (open_animation == 1.f) {
			pos.y += 5;
			for (size_t i = 0; i < elements.size(); i++) {
				pos.y += size.y;
				h = g_mouse.x >= pos.x && g_mouse.y >= pos.y
					&& g_mouse.x <= pos.x + size.x && g_mouse.y < pos.y + size.y;
				if (h) {
					if (h) wnd->g_hovered_element = this;
					if (animations[i] < 1.f) animations[i] += animation_speed;
				}
				else
				{
					if (animations[i] > 0.f) animations[i] -= animation_speed;
				}
				if (animations[i] > 1.f) animations[i] = 1.f;
				else if (animations[i] < 0.f) animations[i] = 0.f;
				if (wnd->is_click() && h) {
					*(int*)value = i;
					open = false;
					wnd->g_active_element = nullptr;
					wnd->reset_mouse();
					if (on_change_value)
						on_change_value(i);
					return true;
				}
			}
		}
		wnd->g_active_element = this;
	}
	else {
		if (wnd->g_active_element == this)
			wnd->g_active_element = nullptr;
		if (open_animation > 0.f)
			open_animation -= animation_speed;

	}
	// bullshit animations
	if (h || open) {
		if (animation < 1.f) animation += animation_speed;
	}
	else {
		if (animation > 0.f) animation -= animation_speed;
	}
	if (open_animation > 1.f) open_animation = 1.f;
	else if (open_animation < 0.f) open_animation = 0.f;
	animation = clamp(animation, 0.f, 1.f);
	*(int*)value = clamp(*(int*)value, 0, (int)elements.size() - 1);
	return false;
}
void c_ragebot_subtabs::render() {
	if (should_render)
		if (!should_render())
			return;
	auto wnd = (c_window*)child;
	if (!wnd)
		return;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return;
	auto pos = wnd->get_cursor_position();
	auto alpha = (int)(wnd->get_transparency() * 2.55f) * (1.f - showing_animation);
	auto size = Vector2D(g_size + 90, 42 * icons_scale * 2);

	bool h = hovered();
	/*

		35, 39, 50

		27, 31, 40,

		60, 95, 156

		60, 75, 130,

		40 + animation * 10.f, 40 + animation * 10.f, 40 + animation * 10.f
	*/
	auto clr = menu_colors::background2.manage_alpha( alpha - 15.f * animation);
	if (label.size() > 0) {
		if (this == wnd->g_active_element)
			wnd->lock_bounds();
		g_Render->DrawString(pos.x, pos.y, color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::none, fonts::menu_desc, label.c_str());
		pos.y += after_text_offset;
		if (this == wnd->g_active_element)
			wnd->unlock_bounds();
	}
	auto selected_item = elements[*(int*)value];
	if (open) {
		auto new_alpha = clamp(alpha * open_animation, 0.f, 255.f);
		g_Render->Rect(pos.x - 1, pos.y - 1, size.x + 2, size.y + 2, color_t(40, 49, 70, new_alpha));
		g_Render->FilledRect(pos.x, pos.y, size.x, size.y,
			menu_colors::background1.manage_alpha(new_alpha));
		auto base = ImVec2(pos.x + size.x - 10, pos.y + size.y / 2);
		g_Render->_drawList->AddTriangleFilled(
			ImVec2(base.x - 3, base.y + 2), ImVec2(base.x + 3, base.y + 2), ImVec2(base.x, base.y - 4), color_t(255, 255, 255, new_alpha).u32());
		ImGui::PushFont(fonts::menu_main);
		auto txt_size = ImGui::CalcTextSize(selected_item->name.c_str());
		ImGui::PopFont();

		g_Render->DrawString(pos.x + size.x / 2, pos.y + size.y / 2 + 10,
			color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::centered_y | render::centered_x, fonts::menu_main, selected_item->name.c_str());
		auto base_pos = ImVec2(pos.x + size.x / 2 - (selected_item->width * icons_scale) / 2, pos.y + 3);
		g_Render->_drawList->AddImage(selected_item->texture,
			base_pos, ImVec2(base_pos.x + selected_item->width * icons_scale, base_pos.y + 40 * icons_scale),
			ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), color_t(255, 255, 255, new_alpha).u32());

		pos.y += 5;
		g_Render->Rect(pos.x - 1, pos.y - 1 + size.y, size.x + 2, size.y * elements.size() + 2, color_t(40, 49, 70, new_alpha));
		for (size_t i = 0; i < elements.size(); i++) {
			pos.y += size.y;
			g_Render->FilledRect(pos.x, pos.y, size.x, size.y,
				color_t(27 + 15.f * animations[i], 31 + 15.f * animations[i], 40 + 15.f * animations[i], new_alpha));
			auto current_item = elements[i];
			auto clr2 = color_t(200 + animations[i] * 55.f, 200 + animations[i] * 55.f, 200 + animations[i] * 55.f, new_alpha);

			base_pos = ImVec2(pos.x + size.x / 2 - (current_item->width * icons_scale) / 2, pos.y + 3);

			g_Render->_drawList->AddImage(current_item->texture,
				base_pos, ImVec2(base_pos.x + current_item->width * icons_scale, base_pos.y + 40 * icons_scale),
				ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), *(int*)value == i ?
				menu_colors::main_color.manage_alpha(new_alpha).u32() : 
				clr2.u32());

			ImGui::PushFont(fonts::menu_main);
			txt_size = ImGui::CalcTextSize(current_item->name.c_str());
			ImGui::PopFont();
			g_Render->DrawString(pos.x + size.x / 2, pos.y + size.y / 2 + 10, *(int*)value == i
				? menu_colors::main_color.manage_alpha(new_alpha - 50.f * (1.f - animations[i]))
				: clr2.manage_alpha(new_alpha - 50.f * (1.f - animations[i])), render::centered_y | render::centered_x,
				fonts::menu_main, current_item->name.c_str());
		}

	}
	else {
		auto new_alpha = clamp(alpha * (1.f - open_animation), 0.f, 255.f);
		auto clr2 = menu_colors::background1.manage_alpha(new_alpha - 15.f * animation);
		g_Render->filled_rect_gradient(pos.x, pos.y, size.x, size.y,
			clr2, clr2,
			clr, clr);
		g_Render->Rect(pos.x - 1, pos.y - 1, size.x + 2, size.y + 2, color_t(40, 49, 70, new_alpha));


		auto base_pos = ImVec2(pos.x + size.x / 2 - (selected_item->width * icons_scale) / 2, pos.y + 3);

		g_Render->_drawList->AddImage(selected_item->texture,
			base_pos, ImVec2(base_pos.x + selected_item->width * icons_scale, base_pos.y + 40 * icons_scale),/*pos.x + 10 + current_item->width * icons_scale, pos.y + 41 * icons_scale),*/
			ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha).u32());

		/*g_Render->_drawList->AddImage(selected_item->texture,
			ImVec2(pos.x + 10, pos.y + 1), ImVec2(pos.x + 10 + selected_item->width * icons_scale, pos.y + 41 * icons_scale),
			ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha).u32());*/

		ImGui::PushFont(fonts::menu_main);
		auto txt_size = ImGui::CalcTextSize(selected_item->name.c_str());
		ImGui::PopFont();

		g_Render->DrawString(pos.x + size.x / 2, pos.y + size.y / 2 + 10, 
			color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::centered_y | render::centered_x, fonts::menu_main, elements[*(int*)value]->name.c_str());

	

		auto base = ImVec2(pos.x + size.x - 10, pos.y + size.y / 2);
		g_Render->_drawList->AddTriangleFilled(
			ImVec2(base.x - 4, base.y - 2), ImVec2(base.x + 4, base.y - 2), ImVec2(base.x, base.y + 5), color_t(255, 255, 255, new_alpha).u32());
	}
}

int c_ragebot_subtabs::get_total_offset() {
	if (should_render)
		if (!should_render())
			return 0;
	return 42 * icons_scale + 3 + (label.size() > 0) * after_text_offset;
};