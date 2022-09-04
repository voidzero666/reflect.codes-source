#pragma once
#include "element.h"

class c_text : public c_element {
private:
	string label;
	bool(*should_render)();
	Vector2D size;
	color_t color;
public:
	c_text(string str, bool(*should_render)() = nullptr, color_t clr = color_t(200, 200, 200)) {
		this->label = str;
		this->type = c_elementtype::text;
		this->should_render = should_render;
		this->color = clr;
	}
	string get_label();
	void set_label(string label);
	void change_pointer(void* ptr) {};
	bool hovered();
	bool update();
	void render();
	void special_render() {};
	int get_total_offset() {
		if (should_render)
			if (!should_render())
				return 0; 
		return size.y + 6;
	};
};