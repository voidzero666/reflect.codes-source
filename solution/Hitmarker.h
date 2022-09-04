#pragma once
#include "Hooks.h"


class CHitMarker
{
public:
	struct bulletimpact
	{
		Vector pos;
		float time;
	};

	struct hitmarker_info
	{
		Vector position;
		int alpha;
		float time;
		std::string hp;
		bool hs;
		int seed;
	};
	std::vector<hitmarker_info> hitmarkers;
	std::vector<bulletimpact> impacts;
	void BulletImpact(IGameEvent* event);
	void PlayerHurt(IGameEvent* event);
	void Paint();
	void Add(Vector hitpos, bool headshot, std::string damage);
};

extern CHitMarker* g_Hitmarker;