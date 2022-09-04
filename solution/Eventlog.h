#pragma once
#include "Hooks.h"

class CMessage
{
public:
	CMessage(std::string str, float time, color_t clr)
	{
		this->str = str;
		this->time = csgo->get_absolute_time();
		this->clr = clr;
		alpha = 255.f;
	}
	std::string str;
	float time;
	color_t clr;
	color_t clrb = color_t(5, 5, 5, 255);
	float alpha;
};
extern float animation_speed;
class CEventlog
{
public:
	std::vector <CMessage> messages;
	void Draw()
	{
		static constexpr float showtime = 5.f;
		static std::string prefix = "[Reflect] ";
		if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame()) {
			messages.clear();
			return;
		}

		if (!vars.visuals.eventlog)
			return;
		while (messages.size() > 6)
			messages.erase(messages.begin());
		for (int i = messages.size() - 1; i >= 0; i--)
		{
			if (messages[i].time + showtime <= csgo->get_absolute_time())
				messages[i].alpha -= 16.f;;
			ImGui::PushFont(fonts::logger);
			auto txt_size = ImGui::CalcTextSize(messages[i].str.c_str());
			ImGui::PopFont();
			//g_Render->FilledRect(15 - (255 - messages[i].alpha), 50 + (20 * i), txt_size.x, txt_size.y, color_t(15, 15, 15, 150));
			//shading
			g_Render->DrawString(14, 5 + (20 * i) + 1,
				messages[i].clrb.manage_alpha(messages[i].alpha),
				render::none, fonts::logger, messages[i].str.c_str());
			//text
			g_Render->DrawString(15, 5 + (20 * i),
				messages[i].clr.manage_alpha(messages[i].alpha),
				render::none, fonts::logger, messages[i].str.c_str());
		}

		for (int i = messages.size() - 1; i >= 0; i--) {
			if (messages[i].alpha <= 0) {
				messages.erase(messages.begin() + i);
				break;
			}
		}

	}
};