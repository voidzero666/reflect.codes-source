#include "checkbox.h"
#include "window.h"
#include "child.h"

bool c_checkbox::update()
{
	if (should_render)
		if (!should_render())
			return false;

	c_child* c = (c_child*)child;
	if (!c) return false;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return false;
	if (wnd->g_active_element != this && wnd->g_active_element != nullptr) return true;
	auto pos = c->get_cursor_position();
	bool h = hovered();

	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return false;
	if (h) wnd->g_hovered_element = this;

	// bullshit animations
	if (h) {
		if (animation < 1.f) animation += animation_speed;
	}
	else {
		if (animation > 0.f) animation -= animation_speed;
	}
	animation = clamp(animation, 0.f, 1.f);

	if (h && c->hovered() && wnd->is_click()) {
		*(bool*)value = !(*(bool*)value);
		wnd->reset_mouse();
		return true;
	}

	if (*(bool*)value == true) {
		if (press_animation < 1.f)
			press_animation += animation_speed;
	}
	else {
		if (press_animation > 0.f)
			press_animation -= animation_speed;
	}
	
	press_animation = clamp(press_animation, 0.f, 1.f);
	return true;
}

bool c_checkbox::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c->hovered())
		return false;
	auto pos = c->get_cursor_position();
	ImGui::PushFont(fonts::menu_desc);
	auto size = ImGui::CalcTextSize(label.c_str());
	ImGui::PopFont();
	return g_mouse.x > pos.x - 30 && g_mouse.y > pos.y - 2
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y + 2;
}

void c_checkbox::render() {
	if (should_render)
		if (!should_render())
			return;
	c_child* c = (c_child*)child;
	auto pos = c->get_cursor_position();

	auto wnd = (c_window*)c->get_parent();
	auto size = ImGui::CalcTextSize(label.c_str());
	if (!wnd) return;
	if (wnd->get_active_tab_index() != this->tab 
		&& wnd->get_tabs().size() > 0) return;
	auto alpha = (int)(c->get_transparency() * 2.55f) * (1.f - showing_animation);
	float final_animation = press_animation * (sin((1.f - showing_animation) * pi2));
	g_Render->DrawString(pos.x, pos.y + 5 * sin(showing_animation), color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
		render::none, fonts::menu_desc, label.c_str());
	g_Render->Rect(pos.x - 30, pos.y + 4, 20, 12, color_t(60 + animation * 25.f, 70 + animation * 25.f, 90 + animation * 25.f, alpha), 10.f);
	g_Render->CircleFilled(pos.x - 25.f + 10.f * final_animation, pos.y + 10, 4,
		menu_colors::main_color.transition(color_t(0, 0, 0), final_animation).manage_alpha(alpha - 25.f * animation), 15);
	//g_Render->FilledRect(pos.x - 19, pos.y + 3, 14, 14, menu_colors::main_color.manage_alpha(alpha * press_animation));
}

