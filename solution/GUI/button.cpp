#include "button.h"
#include "window.h"
#include "child.h"
#include <thread>

string c_button::get_label() {
	return this->label;
}
void c_button::set_label(string label) {
	this->label = label;
}
bool c_button::update()
{
	if (should_render)
		if (!should_render())
			return false;

	c_child* c = (c_child*)child; // получаем чайлд где рендерится наша кнопка
	if (!c) return false; // если его не существует, не обновляем
	
	bool inactive = active; // ??? 
	if (inactive) inactive = !active(); // очень крутой код, но думаю ты понял

	auto pos = c->get_cursor_position(); // получаем из чайда позицию рендера для нашей кнопки
	auto size = Vector2D(g_size, element_height); // размер = константа
	bool h = hovered(); // проверяем наведена ли мышка на кнопку

	auto wnd = (c_window*)c->get_parent(); // чтобы получить информацию о мышке и о активном табе получаем окно через чайлд
	if (!wnd) // если окна нету, не обновляем
		return false;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0) // если таб не тот, тоже не обновляем
		return false;

	if (wnd->g_active_element != this && wnd->g_active_element != nullptr) return false; // если есть активный элемент, и это не наша кнопка то не обновляем

	if (h) { // если мы навели мышку
		wnd->g_hovered_element = this; // говорим окну что мышка находится на кнопке
		if (c->hovered() && wnd->is_click()) { // если мышка находится в границах чайлда и мышка нажата
			if (fn && !inactive) { // если кнопка активна и существует callback (функция которая выполняется при нажатии кнопки)
				std::thread(fn).detach(); // вызываем функцию асинхронно 
				press_animation = 1.f; // переменная отвечает за анимацию
				wnd->reset_mouse(); // говорим окну чтобы сбросил инпут кнопки
			}
			return true;
		}
	}
	if (fn && !inactive) { // если кнопка активна, манипулируем с анимациями
		if (press_animation > 0.f)
			press_animation -= animation_speed;
		if (h) {
			if (animation < 1.f) animation += animation_speed;
		}
		else {
			if (animation > 0.f) animation -= animation_speed;
		}
	}
	animation = clamp(animation, 0.f, 1.f);
	press_animation = clamp(press_animation, 0.f, 1.f);
	return false;
}
bool c_button::hovered()
{
	if (should_render)
		if (!should_render())
			return false;
	c_child* c = (c_child*)child;
	if (!c->hovered())
		return false;
	auto wnd = (c_window*)c->get_parent();
	if (!wnd)
		return false;
	auto pos = c->get_cursor_position();
	auto size = Vector2D(g_size, element_height);
	return g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y && !wnd->g_active_element;
}
void c_button::render() {
	if (should_render)
		if (!should_render())
			return;
	bool inactive = active;
	if (inactive) inactive = !active();
	c_child* c = (c_child*)child;
	auto pos = c->get_cursor_position();
	
	auto size = Vector2D(g_size, element_height);

	auto wnd = (c_window*)c->get_parent();
	if (!wnd) return;
	if (wnd->get_active_tab_index() != this->tab && wnd->get_tabs().size() > 0)
		return;
	auto alpha = (int)(wnd->get_transparency() * 2.55f) * (1.f - showing_animation);

	/*
	
	35, 39, 50

	27, 31, 40,

	60, 95, 156

	60, 75, 130,

	40 + animation * 10.f, 40 + animation * 10.f, 40 + animation * 10.f
	*/
	auto clr = menu_colors::background2.manage_alpha(alpha * (1.f - press_animation) * animation);
	auto clr2 = menu_colors::main_color.manage_alpha(alpha * (1.f - press_animation) * animation);
	auto clr3 = menu_colors::main_color.increase(-50).manage_alpha(alpha * (1.f - press_animation) * (1.f - animation));

	g_Render->FilledRect(pos.x, pos.y, size.x, size.y, clr2, 3.f);
	g_Render->FilledRect(pos.x + 1, pos.y + 1, size.x - 2, size.y - 2, clr, 3.f);

	g_Render->FilledRect(pos.x, pos.y, size.x, size.y, clr3);

	g_Render->DrawString(pos.x + size.x / 2, pos.y + size.y / 2, (fn && !inactive) ?
		color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha) :
		color_t(127, 127, 127, alpha),
		render::centered_x | render::centered_y , fonts::menu_desc, label.c_str());
}