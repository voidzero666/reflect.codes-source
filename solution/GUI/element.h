#pragma once
#include "../DLL_MAIN.h"
#include "../render.h"

#define element_height 26
#define after_text_offset 23
#define g_size 220


extern float animation_speed;

namespace menu_colors {
	extern color_t main_color;
	extern color_t background1;
	extern color_t background2;
}

extern Vector2D g_mouse;

enum c_elementtype : unsigned short {
	window,
	child,
	text,
	dynamic_text,
	button,
	checkbox,
	slider,
	combobox,
	icons_combobox,
	multi_combobox,
	colorpicker,
	keybind,
	input_text,
	listbox,
	bar,
};

class c_element {
public:
	int order = 0;
	int tab;
	c_elementtype type;
	c_element* child;
	float showing_animation = 1.f;
	virtual int get_total_offset() = 0;
	virtual void render() = 0;
	virtual bool update() = 0;
	virtual bool hovered() = 0;
};
extern const vector<string> KeyStrings;