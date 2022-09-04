#include "bar.h"
#include "child.h"


bool c_bar::hovered()
{
	return false;
}

bool c_bar::update()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c) return false;
	auto pos = c->get_cursor_position();
	auto alpha = (int)(c->get_transparency() * 2.55f);
	auto size = Vector2D(g_size, 12);

	bool h = hovered();

	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return false;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return false;
	if (wnd->g_active_element != this && wnd->g_active_element != nullptr) return false;
	if (h) wnd->g_hovered_element = this;
	if (label.size() > 0)
		pos.y += after_text_offset;
	if (*(float*)value > max)
		*(float*)value = max;
	else if (*(float*)value < min)
		*(float*)value = min;
	return false;
}

void c_bar::render() {
	if (should_render)
		if (!should_render())
			return;
	c_child* c = (c_child*)child;
	auto pos = c->get_cursor_position();
	auto alpha = (int)(c->get_transparency() * 2.55f);
	auto size = Vector2D(g_size, 12);
	bool h = hovered();

	auto clr = menu_colors::main_color.manage_alpha(alpha)/*color_t(255, 150, 35.f, alpha)*/;
	auto clr2 = clr.manage_alpha(clamp(alpha - 100.f, 0.f, 255.f))/*color_t(255, 150, 35.f, clamp(alpha - 100.f, 0.f, 255.f))*/;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return;
	float val = *(float*)value;
	float ratio = (float)(val - this->min) / (float)(this->max - this->min);
	float location = ratio * size.x;
	if (label.size() > 0) {
		g_Render->DrawString(pos.x, pos.y, color_t(255, 255, 255, alpha),
			render::none, fonts::menu_desc, "%s : %0.f", label.c_str(), *(float*)value);
		pos.y += after_text_offset;
	}

	g_Render->FilledRect(pos.x, pos.y, size.x, size.y - 2, color_t(45, 45, 45, alpha));
	g_Render->filled_rect_gradient(pos.x, pos.y, location, size.y - 2, clr, clr, clr2, clr2);
	g_Render->Rect(pos.x - 1, pos.y - 1, size.x + 2, size.y, color_t(0, 0, 0, alpha));
}