#include "tab.h"
#include "child.h"
#include "../Math.h"
#define icons_size 28
void c_tab::draw(Vector2D pos, Vector2D size) {
	c_child* c = (c_child*)child;
	auto alpha = (int)(c->get_transparency() * 2.55f);
	bool hovered = g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y;
	if (hovered && !c->g_active_element && !c->is_holding_menu()) {
		c->g_hovered_element = this;
		if (c->is_click()) {
			c->set_active_tab_index(this->index);
			c->reset_mouse();
		}
		
		if (animation < 1.f) animation += animation_speed / 1.5f;
	}
	else {
		if (animation > 0.f) animation -= animation_speed / 1.5f;
	}

	animation = clamp(animation, 0.f, 1.f);
	float r_animation = sin(DEG2RAD(animation * 90.f));
	auto clr = color_t(38 + 10 * animation, 41 + 10 * animation, 54 + 10 * animation, alpha);

	if (c->get_active_tab_index() == this->index) {
		g_Render->FilledRect(pos.x, pos.y, size.x, size.y, menu_colors::background1.manage_alpha(alpha));
		g_Render->DrawLine(pos.x, pos.y, pos.x, pos.y + size.y, color_t(127, 127, 127, alpha));
		g_Render->DrawLine(pos.x + size.x, pos.y, pos.x + size.x, pos.y + size.y, color_t(127, 127, 127, alpha));
		g_Render->DrawLine(pos.x, pos.y, pos.x + size.x, pos.y, color_t(127, 127, 127, alpha));
	}
	else
		g_Render->DrawLine(pos.x, pos.y + size.y, pos.x + size.x, pos.y + size.y, color_t(127, 127, 127, alpha));

	auto t_clr = c->get_active_tab_index() == this->index ? 
		menu_colors::main_color.manage_alpha(alpha - 25.f * r_animation):
		color_t(200 + 55.f * r_animation, 200 + 55.f * r_animation, 200 + 55.f * r_animation, alpha);
	auto base_pos = Vector2D(pos.x + size.x / 2, pos.y + 20);

	g_Render->_drawList->AddImage(this->texture, ImVec2(base_pos.x - icons_size / 2, base_pos.y - icons_size / 2),
		ImVec2(base_pos.x + icons_size / 2, base_pos.y + icons_size / 2), ImVec2(0, 0), ImVec2(1, 1), 
		t_clr.u32());

	g_Render->DrawString(pos.x + size.x / 2, pos.y + 45, t_clr,
		render::centered_x | render::centered_y, fonts::menu_desc, get_title().c_str());
}

void c_tab::special_render() {

}

void c_tab::render() {

}