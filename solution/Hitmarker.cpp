#include "Hitmarker.h"
CHitMarker* g_Hitmarker = new CHitMarker();

void CHitMarker::Paint()
{
	if (!vars.visuals.hitmarker)
		return;
	if (hitmarkers.empty())
		return;

	if (!csgo->is_connected)
		return;

	const int size = 8;
	const int gap = 2;
	const int thickness = 1;

	// draw
	for (const auto& hitmarker : hitmarkers)
	{
		Vector pos3D = Vector(hitmarker.position.x, hitmarker.position.y, hitmarker.position.z), pos2D;
		if (!Math::WorldToScreen(pos3D, pos2D))
			continue;

		auto r = 255,
			g = 255,
			b = 255;

		g_Render->DrawLine(pos2D.x - (size + 1), pos2D.y - (size + 1), pos2D.x - (gap - 1), pos2D.y - (gap - 1), color_t(0, 0, 0, hitmarker.alpha), thickness + 1);
		g_Render->DrawLine(pos2D.x - (size + 1), pos2D.y + (size + 1), pos2D.x - (gap - 1), pos2D.y + (gap - 1), color_t(0, 0, 0, hitmarker.alpha), thickness + 1);
		g_Render->DrawLine(pos2D.x + (size + 1), pos2D.y + (size + 1), pos2D.x + (gap - 1), pos2D.y + (gap - 1), color_t(0, 0, 0, hitmarker.alpha), thickness + 1);
		g_Render->DrawLine(pos2D.x + (size + 1), pos2D.y - (size + 1), pos2D.x + (gap - 1), pos2D.y - (gap - 1), color_t(0, 0, 0, hitmarker.alpha), thickness + 1);

		g_Render->DrawLine(pos2D.x - size, pos2D.y - size, pos2D.x - gap, pos2D.y - gap, color_t(r, g, b, hitmarker.alpha), thickness);
		g_Render->DrawLine(pos2D.x - size, pos2D.y + size, pos2D.x - gap, pos2D.y + gap, color_t(r, g, b, hitmarker.alpha), thickness);
		g_Render->DrawLine(pos2D.x + size, pos2D.y + size, pos2D.x + gap, pos2D.y + gap, color_t(r, g, b, hitmarker.alpha), thickness);
		g_Render->DrawLine(pos2D.x + size, pos2D.y - size, pos2D.x + gap, pos2D.y - gap, color_t(r, g, b, hitmarker.alpha), thickness);
		

		if (vars.visuals.visualize_damage)
		{
			const auto& modifier = std::clamp(fabsf(csgo->get_absolute_time() - hitmarker.time), 0.f, 1.f);
			auto clr = vars.visuals.hitmarker_color;
			g_Render->DrawString(pos2D.x - 1 * modifier, pos2D.y - 25.f - 50 * modifier, 
				hitmarker.hs ? color_t(clr.get_red(), clr.get_green(), clr.get_blue(), (1.f - modifier) * 255) : color_t(255, 255, 255, (1.f - modifier) * 255),
				render::centered_x | render::centered_y | render::outline, fonts::hitmarker, hitmarker.hp.c_str());
		}
	}
	// proceeed
	for (int i = 0; i < hitmarkers.size(); i++) {
		if (vars.visuals.hitmarker == 1)
			hitmarkers[i].alpha -= 1;
		else if (vars.visuals.hitmarker == 2) {
			if (hitmarkers[i].time + 4.25f <= csgo->get_absolute_time()) {
				hitmarkers[i].alpha -= 1;
			}
		}
		if (hitmarkers[i].alpha <= 0)
			hitmarkers.erase(hitmarkers.begin() + i);
	}
}
void CHitMarker::Add(Vector hitpos, bool headshot, std::string damage)
{
	if (!vars.visuals.hitmarker)
		return;
	
	static int counter = 0;
	counter++;
	hitmarker_info info;
	info.position = hitpos;
	info.alpha = 255;
	info.time = csgo->get_absolute_time();
	info.hp = damage;
	info.hs = headshot;
	info.seed = counter;
	hitmarkers.push_back(info);
	if (counter > 3)
		counter = 0;
}


void CHitMarker::BulletImpact(IGameEvent* event)
{
	if (!vars.visuals.hitmarker)
		return;

	if (!event || !csgo->local)
		return;

	if (interfaces.engine->GetPlayerForUserID(event->GetInt(crypt_str("userid"))) != interfaces.engine->GetLocalPlayer())
		return;

	bulletimpact impact;
	impact.pos.x = event->GetFloat(crypt_str("x"));
	impact.pos.y = event->GetFloat(crypt_str("y"));
	impact.pos.z = event->GetFloat(crypt_str("z"));
	impact.time = csgo->get_absolute_time();
	impacts.push_back(impact);
}

void CHitMarker::PlayerHurt(IGameEvent* event)
{
	if (!vars.visuals.hitmarker)
		return;

	if (!event || !csgo->local)
		return;

	auto victim = interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(event->GetInt(crypt_str("userid"))));
	if (!victim)
		return;

	Vector enemypos = victim->GetOrigin();
	bulletimpact best_impact;
	float best_impact_distance = -1.f;
	float time = csgo->get_absolute_time();

	std::vector<bulletimpact>::iterator iter;

	for (iter = impacts.begin(); iter != impacts.end();) {
		if (time > iter->time + 0.025)
		{
			iter = impacts.erase(iter);
			continue;
		}

		Vector position = iter->pos;
		float distance = position.DistTo(enemypos);
		if ((distance < best_impact_distance) || best_impact_distance == -1.f)
		{
			best_impact_distance = distance;
			best_impact = *iter;
		}

		++iter;
	}

	if (best_impact_distance == -1.f)
		return;

	Add(best_impact.pos, event->GetInt(crypt_str("hitgroup")) == 1, std::to_string(event->GetInt(crypt_str("dmg_health"))));
}