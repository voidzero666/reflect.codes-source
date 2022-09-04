#pragma once
#include "element.h"

class c_window;
class c_colorpicker : public c_element {
private:
	float animation;
	float animations[2];
	void* value;
	bool open;
	int dragging_slider;
	float val_alpha;
	float hue, saturation, brightness;
	struct {
		float animations[2];
		vector<string> elements = { "Copy", "Paste" };
		bool open = false;
	} dialogue;
	bool(*should_render)();
	void render_sliderbg(Vector2D pos, float alpha);
	void render_alphabg(Vector2D pos, float alpha);
public:
	void update_colors() {
		hue = (*(color_t*)value).get_hue();
		saturation = (*(color_t*)value).get_saturation();
		brightness = (*(color_t*)value).get_brightness();
		val_alpha = (*(color_t*)value).get_alpha() / 255.f;
	}
	void clamp_colors() {
		hue = clamp(hue, 0.f, 1.f);
		saturation = clamp(saturation, 0.f, 1.f);
		brightness = clamp(brightness, 0.f, 1.f);
	}
	c_colorpicker(void* val, bool(*should_render)() = nullptr) {
		value = val;
		this->type = c_elementtype::colorpicker;
		dragging_slider = 0;
		update_colors();
		this->should_render = should_render;
		this->open = false;
		dialogue.open = false;
		animation = 0.f;
		dialogue.animations[0] = 0.f;
		dialogue.animations[1] = 0.f;
		animations[0] = 0.f;
		animations[1] = 0.f;
	}
	void change_pointer(void* ptr) { value = ptr; };
	bool update();
	bool hovered();
	void render();
	void special_render() {};
	int get_total_offset() { return 0; };
};

