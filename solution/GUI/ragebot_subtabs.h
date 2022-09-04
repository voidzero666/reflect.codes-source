#pragma once
#include "element.h"

class c_window;

class c_icon {
public:
	c_icon(void* texture, float width, string name) {
		this->texture = texture;
		this->width = width;
		this->name = name;
	}
	void* texture;
	float width;
	string name;
};

class c_ragebot_subtabs : public c_element {
private:
	string label;
	void* value;
	float open_animation;
	float animation;
	float* animations;
	bool open;
	
	std::vector<c_icon*> elements;
	Vector2D pos;
	bool(*should_render)();
	void(*on_change_value)(int new_index);
public:

	c_ragebot_subtabs(string label, void* val, std::vector<c_icon*> el, c_window* wnd,
		int tab, bool(*should_render)() = nullptr, void(*on_change_value)(int new_index) = nullptr) {
		this->child = (c_element*)wnd;
		this->tab = tab;
		this->label = label;
		this->value = val;
		this->elements = el;
		this->type = c_elementtype::icons_combobox;
		this->animations = new float[elements.size()];
		for (int i = 0; i < elements.size(); i++)
			this->animations[i] = 0.f;
		this->should_render = should_render;
		this->on_change_value = on_change_value;
		this->open = false;
		this->animation = 0.f;
		this->open_animation = 0.f;
	}
	//void add_icon(void* texture, Vector2D size);
	void change_pointer(void* ptr) { value = ptr; };
	bool hovered();
	bool update();
	void render();
	int get_total_offset();
};