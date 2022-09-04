#pragma once
#include "element.h"

class c_bar : public c_element {
private:
	std::string label;
	bool(*should_render)();
	bool(*inactive)();
	float pulsating;
	bool b_switch;
	float min, max;
	void* value;
public:
	c_bar(string str, void* value, float min, float max, bool(*should_render)() = nullptr, bool(*inactive)() = nullptr) {
		this->label = str;
		this->value = value;
		this->min = min;
		this->max = max;
		this->type = bar;
		this->inactive = inactive;
		this->should_render = should_render;
	}
	void change_pointer(void* ptr) { };
	bool update();
	bool hovered();
	void render();
	void special_render() {};
	int get_total_offset() {
		if (should_render)
			if (!should_render())
				return 0;
		return 33;
	};
};