#pragma once
#include "window.h"

class c_tab : public c_window {
private:
	int index;
	float animation;
	void* texture;
public:
	c_tab(string name, int index, void* tex, c_window* child) {
		this->child = child;
		this->index = index;
		this->texture = tex;
		set_title(name);
		animation = 0.f;
	}
	void draw(Vector2D pos, Vector2D size);
	void render();
	void special_render();
};