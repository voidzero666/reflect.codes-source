#pragma once
#include "element.h"

class c_slider : public c_element {
private:
	string label, format;
	void* value;
	float pseudo_value;
	float animation, drop_animation;
	float max, min;
	bool dragging;
	int decimal;
	bool round;
	bool(*should_render)();
	bool dmgSlider;
public:
	c_slider(string label, void* val, float min, float max, string format, bool(*should_render)() = nullptr, bool dmgSlider = false) {
		this->label = label;
		this->format = format;
		this->value = val;
		this->min = min;
		this->max = max;
		this->should_render = should_render;
		this->dmgSlider = dmgSlider;
		dragging = false;
		//assert(format.find("%") != -1, "ti daun ili kak?");

		if (format.find("%") != -1) {
			round = format.find("%.0f") != -1;
			if (!round) {
				string temp;
				temp.push_back(format[format.find("%") + 1]);
				decimal = atoi(temp.c_str());
			}
		}
		this->drop_animation = 0.f;
		this->animation = 0.f;
		this->type = c_elementtype::slider;
	}
	void change_pointer(void* ptr) { value = ptr; };
	bool hovered();
	bool update();
	void render();
	void special_render() {};
	int get_total_offset() { 
		if (should_render)
			if (!should_render())
				return 0;
		return 18 + after_text_offset; };
};