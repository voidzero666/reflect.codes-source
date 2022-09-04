#include "text.h"

class c_dynamictext : public c_element {
private:
	void* value;
	bool(*should_render)();
	Vector2D size;
	color_t color;
public:
	c_dynamictext(void* value, bool(*should_render)() = nullptr, color_t clr = color_t(255, 255, 255)) {
		this->value = value;
		this->type = c_elementtype::dynamic_text;
		this->should_render = should_render;
		this->color = clr;
	}
	void change_pointer(void* ptr) {};
	bool hovered();
	bool update();
	void render();
	void special_render() {};
	int get_total_offset() {
		if (should_render)
			if (!should_render())
				return 0;
		return size.y + 3;
	};
};