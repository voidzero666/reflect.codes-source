#include "dynamic_text.h"
#include "child.h"

bool c_dynamictext::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	auto pos = c->get_cursor_position();
	auto size = ImGui::CalcTextSize((*(string*)value).c_str());
	return g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y;
}
bool c_dynamictext::update()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (c->get_active_tab_index() != this->tab && c->get_tabs().size() > 0)
		return false;
	auto c_size = ImGui::CalcTextSize((*(string*)value).c_str());
	this->size = Vector2D(c_size.x, c_size.y);
	return false;
}
void c_dynamictext::render() {
	if (should_render)
		if (!should_render())
			return;
	c_child* c = (c_child*)child;
	if (c->get_active_tab_index() != this->tab && c->get_tabs().size() > 0)
		return;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd)
		return;
	auto pos = c->get_cursor_position();
	auto alpha = (int)(wnd->get_transparency() * 2.55f) * (1.f - showing_animation);
	g_Render->DrawString(pos.x, pos.y, color.manage_alpha(alpha), render::none, fonts::menu_desc, (*(string*)value).c_str());
}