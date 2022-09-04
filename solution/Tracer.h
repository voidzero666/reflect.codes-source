#pragma once
#include <deque>
#include "Hooks.h"

class CBulletTracer
{
public:
	void Log(IGameEvent* pEvent);
	void Proceed();
	void Draw();
private:
	struct TracerData_t {
		IBasePlayer* player;
		Vector src, dst;
		color_t color;
	};
	void Add(IBasePlayer* player, Vector src, Vector dst, color_t color = vars.visuals.bullet_tracer_color) {
		TracerData_t new_data;
		new_data.player = player;
		new_data.src = src;
		new_data.dst = dst;
		new_data.color = color;
		tracers.push_back(new_data);
	}

	std::vector<TracerData_t> tracers;
	std::vector<Vector> data[64];
};