#pragma once
#include "window.h"

class c_child : public c_window {
private:
	struct {
		float value, animation;
		bool dragging;
	} scroll;
	struct {
		bool dragging;
	} resizer;
	Vector2D active_element_position;
public:
	c_element* get_parent();
	c_child(string label, int tab, c_window* wnd) {
		this->set_title(label);
		this->child = wnd;
		this->tab = tab;
		this->type = c_elementtype::child;
		this->horizontal_offset = true;
		this->scroll.value = 0.f;
		this->scroll.animation = 0.f;
		this->scroll.dragging = false;
	}

	bool should_draw_scroll;
	int total_elements_size;
	bool horizontal_offset;
	void lock_bounds();
	void unlock_bounds();
	int get_total_offset();
	int get_upper_offset();
	bool update();
	void update_elements();
	void render_elements();
	void render();
	void special_render() {};
	bool hovered();
	void initialize_elements();
};