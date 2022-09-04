#pragma once
#include "element.h"

class c_checkbox : public c_element {
private:
	string label;
	float animation, press_animation;
	void* value;
	bool (*should_render)();
public:
	c_checkbox(string label, void* val, bool (*should_render)() = nullptr) {
		this->label = label;
		this->value = val;
		this->type = c_elementtype::checkbox;
		this->should_render = should_render;
		press_animation = 0.f;
		animation = 0.f;
	}

	void* get_ptr() { return value; };
	bool update();
	bool hovered();
	void render();
	void change_pointer(void* ptr) { value = ptr; };
	void special_render() {};
	int get_total_offset() {
		if (should_render)
			if (!should_render())
				return 0;
		return 20;
	};
};
