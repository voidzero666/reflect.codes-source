#include "slider.h"
#include "window.h"
#include "child.h"

bool c_slider::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c->hovered())
		return false;
	auto pos = c->get_cursor_position();

	pos.y += after_text_offset;
	auto this_size = Vector2D(g_size, 15);
	return g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + this_size.x && g_mouse.y < pos.y + this_size.y;
}

bool c_slider::update()
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
	auto pos = c->get_cursor_position();
	auto alpha = (int)(c->get_transparency() * 2.55f);
	auto size = Vector2D(g_size, 15);

	bool h = hovered() && c->hovered();
	if (h) wnd->g_hovered_element = this;
	pos.y += after_text_offset;

	if (wnd->is_holding() && h) {
		dragging = true;
	}
	else {
		if (!wnd->is_holding())
			dragging = false;
		else if (wnd->is_holding() && !h && dragging)
			dragging = true;
		else
			dragging = false;
	}
	if (h) {
		float newpos = g_mouse.x - pos.x;
		if (newpos < 0)
			newpos = 0;
		if (newpos > size.x)
			newpos = size.x;
		float ratio = newpos / size.x;
		if (round)
			pseudo_value = (int)(min + (max - min) * ratio);
		else
			pseudo_value = min + (max - min) * ratio;
	}
	if (dragging) {
		float newpos = g_mouse.x - pos.x;
		if (newpos < 0)
			newpos = 0;
		if (newpos > size.x)
			newpos = size.x;
		float ratio = newpos / size.x;
		if (round)
			*(int*)value = (int)(min + (max - min) * ratio);
		else
			*(float*)value = min + (max - min) * ratio;
		wnd->g_active_element = this;
		drop_animation = 1.f;
	} else {
		if (drop_animation >= 0.f)
			drop_animation -= animation_speed * 0.75f;
		wnd->g_active_element = nullptr;
	}
	if (round) {
		if (*(int*)value > max)
			*(int*)value = max;
		else if (*(int*)value < min)
			*(int*)value = min;
	}
	else
	{
		if (*(float*)value > max)
			*(float*)value = max;
		else if (*(float*)value < min)
			*(float*)value = min;
	}

	// bullshit animations
	if (h || dragging) {
		if (animation < 1.f) animation += animation_speed;
	}
	else {
		if (animation > 0.f) animation -= animation_speed;
	}
	animation = clamp(animation, 0.f, 1.f);
	return dragging;
}

void c_slider::render() {
	if (should_render)
		if (!should_render())
			return;
	c_child* c = (c_child*)child;
	auto pos = c->get_cursor_position();
	
	auto size = Vector2D(g_size, 15);
	bool h = hovered() && c->hovered();
	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return;
	auto alpha = (int)(wnd->get_transparency() * 2.55f) * (1.f - showing_animation);

	auto clr = menu_colors::main_color.manage_alpha(alpha);
	auto clr2 = clr.increase(-75);

	if (!label.empty()) {
		g_Render->DrawString(pos.x, pos.y + 5 * sin(showing_animation), color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::none, fonts::menu_desc, label.c_str());
	}
	pos.y += after_text_offset;

	auto calc_text_size = [](ImFont* font, const char* message, ...) -> float {
		char output[4096] = {};
		va_list args;
		va_start(args, message);
		vsprintf(output, message, args);
		va_end(args);
		ImGui::PushFont(fonts::menu_desc);
		auto ret = ImGui::CalcTextSize(output).x;
		ImGui::PopFont();
		return ret;
	};

	float val = round ? *(int*)value : *(float*)value;
	float ratio = ((float)(val - this->min) / (float)(this->max - this->min));
	ratio = clamp(ratio - sin(showing_animation) * 0.1f, 0.f, 1.f);

	if (dmgSlider && val > 100) {

		std:string formatOverride = "HP+1";

		g_Render->DrawString(
			pos.x + (size.x - calc_text_size(fonts::menu_desc, format.c_str(), val)) * (!label.empty()),
			pos.y - after_text_offset + 5 * sin(showing_animation),
			color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::none, fonts::menu_desc, formatOverride.c_str(), val);
	}
	else {
		g_Render->DrawString(
			pos.x + (size.x - calc_text_size(fonts::menu_desc, format.c_str(), val)) * (!label.empty()),
			pos.y - after_text_offset + 5 * sin(showing_animation),
			color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha),
			render::none, fonts::menu_desc, format.c_str(), val);
	}

	g_Render->FilledRect(pos.x, pos.y + size.y / 2 - 2, size.x, 4, color_t(60, 70, 90, alpha));
	g_Render->FilledRect(pos.x, pos.y + size.y / 2 - 2, ratio * size.x, 4, menu_colors::main_color.manage_alpha(alpha));
	g_Render->CircleFilled(pos.x + ratio * size.x, pos.y + size.y / 2, 
		(size.y / 2 - 2) * (1.f - sin(showing_animation)), 
		menu_colors::main_color.manage_alpha(alpha * (1.f - sin(showing_animation))), 20);
	
	g_Render->CircleFilled(pos.x + ratio * size.x, pos.y + size.y / 2, size.y / 2 * 2,
		menu_colors::main_color.manage_alpha(((alpha * sin(drop_animation)) * 0.40f) + 35 * animation), 20);
}