#include "tab.h"
#include "child.h"
#include "../Menu.h"

float animation_speed = 0.f;
void c_window::set_title(string title) {
	this->title = title;
}
void c_window::set_position(Vector2D pos) {
	this->pos = pos;
}
void c_window::set_size(Vector2D size) {
	this->size = size;
}
string c_window::get_title() {
	return this->title;
}
std::vector<c_tab*> c_window::get_tabs() {
	return this->tabs;
}
Vector2D c_window::get_position() {
	return this->pos;
}
Vector2D c_window::get_size() {
	return this->size;
}
void c_window::add_element(c_element* e) {
	this->elements.push_back(e);
}
void c_window::add_tab(c_tab* t) {
	this->tabs.push_back(t);
}
void c_window::set_cursor_position(Vector2D cursor) {
	this->cursor = cursor;
}
Vector2D c_window::get_cursor_position() {
	return this->cursor;
}
void c_window::apply_element(int offset) {
	this->cursor.y += offset;
}
float c_window::get_transparency() {
	transparency = clamp(transparency, 0.f, 100.f);
	return transparency;
}
void c_window::set_transparency(float transparency) {
	this->transparency = clamp(transparency, 0.f, 100.f);
}
void c_window::increase_transparency(float add) {
	transparency += add;
	transparency = clamp(transparency, 0.f, 100.f);
}
void c_window::decrease_transparency(float subtract)
{
	transparency -= subtract;
	transparency = clamp(transparency, 0.f, 100.f);
}
bool c_window::is_click() {
	return is_pressed;
}
bool c_window::left_click() {
	return is_pressed_left;
}
bool c_window::reset_mouse() {
	is_pressed = false;
	holding = false;
	needs_reset = true;
	return true;
}
void c_window::set_active_tab_index(int tab) {
	this->active_tab_index = tab;
}
int c_window::get_active_tab_index() {
	return this->active_tab_index;
}
bool c_window::is_holding() {
	return holding;
}
bool c_window::get_clicked()
{
	if (hovered()) {
		if (!holding) {
			drag.x = g_mouse.x - pos.x;
			drag.y = g_mouse.y - pos.y;
		}
		return true;
	}
	return false;
}
void c_window::lock_bounds() {
	auto pos = this->get_position();
	auto size = this->get_size();
	auto m = Vector2D(pos.x + size.x, pos.y + size.y);
	g_Render->_drawList->PushClipRect(ImVec2(pos.x, pos.y), ImVec2(m.x, m.y), true);
}
void c_window::unlock_bounds() {
	g_Render->_drawList->PopClipRect();
}
bool c_window::hovered() {
	return g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y;
}
bool c_window::key_updated(int key)
{
	return (keystates[key] && !oldstates[key]);
}
bool c_window::update() {
	if (ImGui::GetIO().MouseDownDuration[0] >= 0.f && needs_reset)
		return false;
	else if (needs_reset)
		needs_reset = false;

	is_pressed = ImGui::GetIO().MouseDownDuration[0] == 0.f;
	holding = ImGui::GetIO().MouseDownDuration[0] > 0.f;

	is_pressed_left = ImGui::GetIO().MouseDownDuration[1] == 0.f;
	holding_left = ImGui::GetIO().MouseDownDuration[1] > 0.f;

	g_hovered_element = nullptr;
	return false;
}
bool c_window::is_holding_menu()
{
	return holding_menu && is_holding();
}
c_element* c_window::get_parent() {
	return nullptr;
}
void c_window::render_tabs() {
	constexpr auto tabs_size = 70;
	g_Render->FilledRect(pos.x, pos.y, size.x, tabs_size, menu_colors::background2.manage_alpha((int)(transparency * 2.55f)));
	g_Render->DrawLine(pos.x, pos.y + tabs_size, pos.x + 8, pos.y + tabs_size, color_t(127, 127, 127, (int)(transparency * 2.55f)));
	g_Render->DrawLine(pos.x + size.x - 8, pos.y + tabs_size, pos.x + size.x, pos.y + tabs_size, color_t(127, 127, 127, (int)(transparency * 2.55f)));
	//	g_Render->DrawLine(pos.x + 8, pos.y + tabs_size, pos.x + 8, pos.y + size.y - 8, color_t(127, 127, 127, (int)(transparency * 2.55f)));
	//	g_Render->DrawLine(pos.x + size.x - 8, pos.y + tabs_size, pos.x + size.x - 8, pos.y + size.y - 8, color_t(127, 127, 127, (int)(transparency * 2.55f)));
	//	g_Render->DrawLine(pos.x + 8, pos.y + size.y - 8, pos.x + size.x - 8, pos.y + size.y - 8, color_t(127, 127, 127, (int)(transparency * 2.55f)));

	Vector2D tab_size = Vector2D((size.x - 16.f) / tabs.size(), tabs_size - 8);
	Vector2D pos_render = Vector2D(pos.x + 8.f, pos.y + 8.f);
	auto alpha = (int)(get_transparency() * 2.55f);

	for (auto& t : tabs) {
		t->draw(pos_render, tab_size);
		pos_render.x += tab_size.x;
	}

	bool g_hovered = !g_active_element && g_mouse.x > pos.x + 15.f && g_mouse.y > pos.y + 15.f
		&& g_mouse.x < pos.x + 8.f && g_mouse.y < pos.y + 55.f;
	if (g_hovered) {
		if (is_click())
			active_tab_index = tab_t::undefined;
	}

	static LPDIRECT3DTEXTURE9 img;

	Vector2D base_pos = Vector2D(pos.x + 15.f, pos.y + size.y - 50.f);

	//g_Render->filled_rect_gradient(pos.x + 15.f, pos.y + 105.f, size.x - 30.f, 5.f,
	//	menu_colors::main_color.manage_alpha(alpha).increase(-100), menu_colors::main_color.manage_alpha(alpha),
	//	menu_colors::main_color.manage_alpha(alpha), menu_colors::main_color.manage_alpha(alpha).increase(-100));
}
void c_window::update_animation() {
	if (last_time_updated == -1.f)
		last_time_updated = csgo->get_absolute_time();
	animation_speed = fabsf(last_time_updated - csgo->get_absolute_time()) * 5.f;
	last_time_updated = csgo->get_absolute_time();
	
}

void c_window::update_keystates() {
	for (size_t l = 0; l < 256; l++)
		oldstates[l] = keystates[l];
	for (auto i = 0; i < 256; i++)
		keystates[i] = csgo->key_pressed[i];
}
void c_window::render() {
	if (transparency <= 0.f)
		return;
	
	int a = clamp((int)(transparency * 2.55f - 50.f), 0, 255);
	g_Render->FilledRect(0, 0, csgo->w, csgo->h, color_t(0, 0, 0, 100 * transparency / 100.f));

	g_Render->filled_rect_gradient(pos.x, pos.y, size.x + 30, size.y + 30,
		color_t(0, 0, 0, (int)(transparency * 2.55f)), color_t(0, 0, 0, 0), 
		color_t(0, 0, 0, 0), color_t(0, 0, 0, 0));

	g_Render->filled_rect_gradient(pos.x - 30, pos.y - 30, size.x + 30, size.y + 30,
		color_t(0, 0, 0, 0), color_t(0, 0, 0, 0),
		color_t(0, 0, 0, (int)(transparency * 2.55f)), color_t(0, 0, 0, 0));

	g_Render->FilledRect(pos.x, pos.y, size.x, size.y, menu_colors::background1.manage_alpha((int)(transparency * 2.55f)));
	g_Render->filled_rect_gradient(pos.x, pos.y + size.y / 2, size.x, size.y / 2,
		menu_colors::background1.manage_alpha((int)(transparency * 2.55f)), menu_colors::background1.manage_alpha((int)(transparency * 2.55f)),
		menu_colors::background2.manage_alpha( (int)(transparency * 2.55f)), menu_colors::background2.manage_alpha( (int)(transparency * 2.55f)));
	g_Render->Rect(pos.x - 1, pos.y - 1, size.x + 2, size.y + 2, color_t(127, 127, 127, (int)(transparency * 2.55f)));

	//// left border
	//g_Render->FilledRect(pos.x, pos.y, 8, size.y, menu_colors::background2.manage_alpha( (int)(transparency * 2.55f)));
	//// right border
	//g_Render->FilledRect(pos.x + size.x - 8, pos.y, 8, size.y, menu_colors::background2.manage_alpha( (int)(transparency * 2.55f)));
	//// bottom border
	g_Render->FilledRect(pos.x, pos.y + size.y - 8, size.x, 8, menu_colors::background2.manage_alpha( (int)(transparency * 2.55f)));


	update();

	if (tabs.size() > 0) {
		render_tabs();
		
		cursor = pos + Vector2D(15, 90);
	}
	else
		cursor = pos + Vector2D(15, 25);
	
	Vector2D backup_cursor_pos = cursor;
	update_elements();
	cursor = backup_cursor_pos;
	render_elements();

	if (g_active_element) {
		if (g_active_element->type != c_elementtype::child)
			((c_child*)active_element_parent)->set_cursor_position(g_active_element_pos);
		g_active_element->render();
	}

	if (!holding)
		holding_menu = !g_hovered_element && get_clicked();
	
	if (holding_menu)
		this->pos = Vector2D(g_mouse.x - drag.x, g_mouse.y - drag.y);
}

void c_window::update_elements()
{
	for (auto e : elements) {
		if (e->tab != active_tab_index) {
			e->showing_animation = 1.f;
			if (e->type == c_elementtype::child)
				for (const auto& el : ((c_child*)e)->elements)
					el->showing_animation = 1.f;
			continue;
		}

		if (e->showing_animation > 0.f)
			e->showing_animation -= animation_speed * 0.5f;
		if (e->showing_animation < 0.f)
			e->showing_animation = 0.f;

		e->update();
	}
}

void c_window::render_elements()
{
	for (auto e : elements) {
		if (e->tab != active_tab_index)
			continue;
		e->render();
		if (e->type != c_elementtype::child) {
			if (e == g_active_element) {
				g_active_element_pos = get_cursor_position();
				active_element_parent = this;
			}
		}
	}
};

const vector<string> KeyStrings = {
	"no key", "left mouse", "right mouse", "control+break", "middle mouse", "mouse 4", "mouse 5",
	"unk", "backspace", "tab", "unk", "unk", "unk", "enter", "unk", "unk", "shift", "ctrl", "alt", "pause",
	"caps lock", "unk", "unk", "unk", "unk", "unk", "unk", "esc", "unk", "unk", "unk", "unk", "spacebar",
	"pg up", "pg down", "end", "home", "left", "up", "right", "down", "unk", "print", "unk", "print screen", "insert",
	"delete", "unk", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", "left windows", "right windows", "unk", "unk", "unk", "num 0", "num 1", "num 2", "num 3", "num 4", "num 5", "num 6",
	"num 7", "num 8", "num 9", "*", "+", "_", "-", ".", "/", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12",
	"f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "num lock", "scroll lock", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "lshift", "rshift", "lcontrol", "rcontrol", "lmenu", "rmenu", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "next track", "previous track", "stop", "play/pause", "unk", "unk",
	"unk", "unk", "unk", "unk", ";", "+", ",", "-", ".", "/?", "~", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "[{", "\\|", "}]", "'\"", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk",
	"unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk", "unk","unk", "unk", "unk"
};