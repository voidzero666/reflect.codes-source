#pragma once
#include "element.h"

class c_button : public c_element {
private:
	std::string label; // название отображаемое в меню
	float animation, press_animation; // анимация при наведении, анимация при нажатии
	bool(*should_render)(); // функция которая отвечает за то, рендерится элемент или нет
	bool(*active)(); // функция которая отвечает за то, можно ли нажать на кнопку или нет
public:
	c_button(string str, void(*_call)(), bool(*active)() = nullptr, bool(*should_render)() = nullptr) {
		label = str;
		fn = _call;
		this->type = button;
		press_animation = 0.f;
		animation = 0.f;
		this->active = active;
		this->should_render = should_render;
	}
	void(*fn)();
	string get_label();
	void set_label(string label);
	void change_pointer(void* ptr) { };
	bool update();
	bool hovered();
	void render();
	void special_render() {};
	int get_total_offset() {
		if (should_render)
			if (!should_render())
				return 0;
		return element_height + 3;
	};
};