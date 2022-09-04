#pragma once
#include "text.h"
#include "button.h"
#include "checkbox.h"
#include "slider.h"
#include "combo.h"
class c_tab;

extern bool is_button_down(int code);
class c_window : public c_element {
private:
	string title;
	Vector2D pos, size, cursor;
	float transparency = 100.f;
	bool open = true;
	bool is_pressed, is_pressed_left;
	bool holding, holding_left;
	bool needs_reset;
	bool dont_click;
	bool holding_menu;
	Vector2D drag;
	bool b_once;
	int active_tab_index;
	std::vector<c_tab*> tabs;
	bool keystates[256];
	bool oldstates[256];
	float last_time_updated = -1.f;
	float animation228;
public:
	bool decorative;
	std::vector<c_element*> elements;
	void special_render() {};
	c_window() {
		this->type = c_elementtype::window;
		g_active_element = nullptr;
	}
	bool is_holding_menu();
	void change_pointer(void* ptr) {};
	c_element* active_element_parent;
	c_element* get_parent();
	void set_title(string title);
	void set_position(Vector2D pos);
	void set_size(Vector2D size);
	void lock_bounds();
	void unlock_bounds();
	void set_transparency(float transparency);
	void increase_transparency(float add);
	void decrease_transparency(float subtract);
	string get_title();
	std::vector<c_tab*> get_tabs();
	Vector2D get_position();
	Vector2D get_size();
	void set_cursor_position(Vector2D cursor);
	Vector2D get_cursor_position();
	void set_active_tab_index(int tab);
	int get_active_tab_index();
	bool is_click();
	bool left_click();
	bool reset_mouse();
	bool is_holding();
	float get_transparency();
	void apply_element(int offset);
	void add_element(c_element* e);
	void add_tab(c_tab* t);
	bool get_clicked();
	bool hovered();
	bool key_updated(int key);
	void update_keystates();
	bool update();
	void update_animation();
	void render_tabs();
	void update_elements();
	void render_elements();
	void render();
	int get_total_offset() { return -1; };
	c_element* g_hovered_element;
	c_element* g_active_element;
	Vector2D g_active_element_pos;
};