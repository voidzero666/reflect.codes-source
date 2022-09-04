#pragma once
#include "element.h"

class c_input_text : public c_element {
private:
	std::string label;
	float animation, pulsating;
	bool active, b_switch;
	bool(*should_render)();
	bool password;
	void* value;
	bool search;
public:
	c_input_text(string label, void* value, bool password, bool(*should_render)() = nullptr, bool search = false) {
		this->label = label;
		this->value = value;
		this->password = password;
		this->b_switch = false;
		this->active = false;
		this->type = input_text;
		this->animation = 0.f;
		this->should_render = should_render;
		this->search = true;
	}
	string get_format() {
		
		if (password)
		{
			string str;
			for (auto c : *(string*)value)
				str += '*';
			return str;
		}
		else
			return *(string*)value;
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
		return element_height + 12 + (label.size() > 0) * 15;
	};
};