#pragma once
#include "element.h"


class c_listbox : public c_element {
private:
	std::string label;
	float animation, press_animation;
	float height;
	bool(*should_render)();
	void* value;
	float scroll;
	float* animations;
	std::vector<std::string> elements;
	bool should_draw_scroll;
	float total_elements_size;
	bool autosize;
public:
	c_listbox(std::string str, void* value, std::vector<std::string> elements, float height, bool autosize = true, bool(*should_render)() = nullptr) {
		label = str;
		this->type = listbox;
		this->height = height;
		this->value = value;
		this->elements = elements;
		this->animation = 0.f;
		this->press_animation = 0.f;
		this->scroll = 0.f;
		this->animations = new float[elements.size()];
		for (int i = 0; i < elements.size(); i++)
			animations[i] = 0.f;
		this->should_render = should_render;
		this->should_draw_scroll = false;
		this->total_elements_size = elements.size() * element_height;
		this->autosize = autosize;
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
		if (this->autosize)
			return total_elements_size + 5 + (label.size() > 0) * after_text_offset;
		return height + 5 + (label.size() > 0) * after_text_offset;
	};
};
