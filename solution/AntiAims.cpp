#include "Hooks.h"
#include "AntiAims.h"
#include "Ragebot.h"
#include "Knifebot.h"
#include "Misc.h"

bool CanDT() {
	return csgo->skip_ticks >= 14 && !csgo->dt_charged;
}

bool CanHS() {
	return false;
}

void CMAntiAim::Fakelag()
{
	//if (!vars.antiaim.enable)
		//return;

	if (vars.antiaim.simtimeStopper)
		return;

	if (vars.antiaim.fakelag != 3) {
		if (vars.antiaim.fakelagfactor == 0)
			return;
	}

	if (csgo->game_rules->IsFreezeTime()
		|| csgo->local->GetFlags() & FL_FROZEN)
		return;

	if (csgo->fake_duck && csgo->local->GetFlags() & FL_ONGROUND && !(csgo->cmd->buttons & IN_JUMP))
	{
		if (csgo->local->GetFlags() & FL_ONGROUND)
			return;
	}

	bool disable_fakelag_on_exploit = []() {
		if (vars.antiaim.fakelag_when_exploits)
			return CanDT();
		else
			return (vars.ragebot.double_tap.state || vars.ragebot.hideShots.state);
	}();

	if (vars.antiaim.fakehead.state)
	{
		csgo->send_packet = csgo->client_state->iChokedCommands >= 2;
		csgo->max_fakelag_choke = 2;
		return;
	}

	if (disable_fakelag_on_exploit || (csgo->local->GetVelocity().Length2D() < 10.f && !vars.antiaim.fakelag_when_standing))
	{
		csgo->send_packet = csgo->client_state->iChokedCommands >= 1;
		csgo->max_fakelag_choke = 1;
		return;
	}

	int tick_to_choke = 1;

	csgo->max_fakelag_choke = 14;

	switch (vars.antiaim.fakelag)
	{
	case 0:
		tick_to_choke = 1;
		break;
	case 1:
		tick_to_choke = vars.antiaim.fakelagfactor;
		break;
	case 2:
	{
		int factor = vars.antiaim.fakelagvariance;
		if (factor == 0)
			factor = 15;
		else if (factor > 100)
			factor = 100;

		if (csgo->cmd->command_number % factor < vars.antiaim.fakelagfactor)
			tick_to_choke = min(vars.antiaim.fakelagfactor, csgo->max_fakelag_choke);
		else
			tick_to_choke = 1;
		break;
	}
	case 3: // breakLC
	{
		auto velocity = csgo->local->GetVelocity();
		auto currentSpeed = velocity.Length2D();
		auto extrapolatePerTick = currentSpeed * interfaces.global_vars->interval_per_tick;
		int chokeReq = std::ceilf(64.f / extrapolatePerTick);

		if (chokeReq < 14) {
			tick_to_choke = chokeReq;
		}
		else {
			tick_to_choke = 14;
		}
		tick_to_choke = min(tick_to_choke, vars.antiaim.fakelagfactor);
		break;
	}
	default:
		tick_to_choke = 1;
		break;
	}

	tick_to_choke = clamp(tick_to_choke, 1, csgo->max_fakelag_choke);

	if (vars.misc.restrict_type == 0 && tick_to_choke > 8)
		tick_to_choke = 8;

	if (vars.antiaim.enable && 1 > tick_to_choke) {
		tick_to_choke = 1;
	}
		
	csgo->send_packet = csgo->client_state->iChokedCommands >= tick_to_choke;
}

void CMAntiAim::Pitch(bool legit_aa)
{
	if (legit_aa)
		return;

	//if (vars.antiaim.pitch == 0) {
		//disable
	//}
	if (vars.antiaim.pitch == 1) {
		//down
		csgo->cmd->viewangles.x = 89.f;
	}
	else if (vars.antiaim.pitch == 2) {
		//up
		csgo->cmd->viewangles.x = -89.f;
	}

//	if (vars.misc.antiuntrusted)
		//csgo->cmd->viewangles.x = 89.f;
	//else
	//	csgo->cmd->viewangles.x = 179.98f; //emotion pitch
}

void CMAntiAim::Sidemove() {
	if (csgo->weapon->GetItemDefinitionIndex() != weapon_revolver) {
		if ((vars.ragebot.double_tap.state || vars.ragebot.hideShots.state) && csgo->skip_ticks > 0 && csgo->cmd->buttons & IN_ATTACK || csgo->game_rules->IsFreezeTime())
			return;
	}
	if (csgo->local->GetMoveType() == MoveType_t::MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MoveType_t::MOVETYPE_LADDER)
		return;
	if (!csgo->should_sidemove)
		return;

	const float& sideAmount = csgo->cmd->buttons & IN_DUCK || csgo->fake_duck ? 3.25f : 1.1f;
	if (csgo->local->GetVelocity().Length2D() <= 0.f || std::fabs(csgo->local->GetVelocity().z) <= 100.f)
		csgo->cmd->forwardmove += csgo->cmd->command_number % 2 ? sideAmount : -sideAmount;
}

bool isLbyUpdating()
{
	static float Update = 0.f;
	if (!(csgo->local->GetFlags() & 1) || !csgo->local->GetPlayerAnimState())
		return false;
	float serverTime = TICKS_TO_TIME(csgo->local->GetTickBase());
	if (csgo->local->GetVelocity().Length2D() > 0.1f || fabsf(csgo->local->GetPlayerAnimState()->m_flVelocityLengthZ) > 100.f)
	{
		Update = serverTime + 0.22f;
	}
	if (Update < serverTime)
	{
		Update = serverTime + 1.1f;
		return true;
	}
	return false;
}

bool should_break_lby()
{
	static float lby_timer = 0;
	float correct_servertime = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;

	auto animstate = csgo->local->GetPlayerAnimState();
	if (!animstate)
		return false;

	if (animstate->m_flVelocityLengthXY > 1.1f || fabs(animstate->m_flVelocityLengthZ) > 100.f)
		lby_timer = correct_servertime + 0.22f;
	else if (lby_timer < correct_servertime)
	{
		lby_timer = correct_servertime + 0.22f;
		return true;
	}
	else if (correct_servertime + TICKS_TO_TIME(1) > lby_timer)
		return true;
	return false;
}

static float sent = 0.f;

void CMAntiAim::Yaw(bool legit_aa)
{
	bool check = vars.antiaim.fakehead.state || vars.antiaim.ignore_attarget && 
		(vars.antiaim.manual_back.state
		|| vars.antiaim.manual_right.state
		|| vars.antiaim.manual_left.state
		|| vars.antiaim.manual_forward.state);

	csgo->isInLby = isLbyUpdating();

	if (vars.antiaim.attarget && !check)
	{
		int w, h, y, x;
		interfaces.engine->GetScreenSize(w, h);
		y = h / 2;
		x = w / 2;
		Vector crosshair = Vector(x, y, 0);
		IBasePlayer* best_ent = nullptr;
		float best_dist = FLT_MAX;
		for (int i = 1; i < 65; i++)
		{
			auto ent = interfaces.ent_list->GetClientEntity(i);
			if (!ent)
				continue;
			if (
				!ent->isAlive()
				|| !ent->IsPlayer()
				|| ent == csgo->local
				|| !ent->isEnemy(csgo->local)
				|| ent->DormantWrapped())
				continue;
			
			Vector origin_2d;
			if (!Math::WorldToScreen(ent->GetOrigin(), origin_2d) && vars.antiaim.attarget_off_when_offsreen)
				continue;

			float dist = crosshair.DistTo(origin_2d);
			if (dist < best_dist)
			{
				best_ent = ent;
				best_dist = dist;
			}
		}

		if (best_ent)
			csgo->cmd->viewangles.y = Math::CalculateAngle(csgo->local->GetOrigin(), best_ent->GetOrigin()).y;
	}

	//int side = csgo->SwitchAA ? 1 : -1;
	int side = vars.antiaim.inverter.state ? 1 : -1;
	//vars.antiaim.inverter.active

	//base angle
	//csgo->cmd->viewangles.y += 180.f;

	//if (csgo->send_packet) {
		//csgo->cmd->viewangles.y -= 180.f;
		//sent = csgo->cmd->viewangles.y;
	//}

	//if (vars.antiaim.base_yaw == 1)
		//side *= -1;

	static bool sw = false;
	static bool avoid_overlap_side = false;
	static float last_angle = 0.f;

	float desync_amount = legit_aa ? 60.f : vars.antiaim.static_delta ? 60.f : 60.f * (vars.antiaim.desync_amount / 100.f);

	static constexpr float desync_lean = 120.f;
	static float lbyRange = 119.95f;

	bool extend = false;

	if (vars.antiaim.extendlby /*  && !vars.ragebot.double_tap.state */) {
		extend = true;
		csgo->should_sidemove = false;
	}
	else {
		extend = false;
		csgo->should_sidemove = true;
	}

	if (csgo->isInLby && extend) {
	//	csgo->cmd->viewangles.y += ((desync_amount * 2) * -side);

		desync_amount *= 2.f;

		if (vars.antiaim.base_yaw == 1) {
			if (side > 0) {
				csgo->cmd->viewangles.y -= 119.95f;
			}
			else {
				csgo->cmd->viewangles.y += 119.95f;
			}
		}
		else {


			if (side > 0) {
				csgo->cmd->viewangles.y += 119.95f;
			}
			else {
				csgo->cmd->viewangles.y -= 119.95f;
			}
		}

	 //csgo->cmd->viewangles.y += (119.95f * -side); //Set our broken LBY angle

		//zlean memes
	 if (vars.misc.restrict_type == 1 && vars.antiaim.zleanenable) {

		 if (csgo->local->GetVelocity().Length2D() <= 5.f) {
			 csgo->cmd->viewangles.z = -INT_MAX;
		 }
	 }

		//side ? csgo->cmd->viewangles.y = sent - lbyRange : csgo->cmd->viewangles.y = sent + lbyRange;
		csgo->send_packet = false;
		//csgo->forceLbyChoke = true;
		//csgo->force_next_sendpacket = false;
		//side ^= 1;
		return;
	}

	//if (std::abs(csgo->desync_angle) < 10.f && csgo->send_packet)
		//csgo->cmd->viewangles.y += 25.f;
	/*
	if (vars.antiaim.avoid_overlap && !vars.antiaim.static_delta && csgo->send_packet) {

		static int ticks_delay = 0;
		if (ticks_delay == 0
			&& csgo->desync_angle <= 1.f) {
			avoid_overlap_side = !avoid_overlap_side;
			ticks_delay = 1;
		}
		if (ticks_delay > 0)
			ticks_delay--;
		side *= avoid_overlap_side ? 1 : -1;
	}
	*/


	if (vars.antiaim.fakehead.state)
	{
		int meme = interfaces.global_vars->tickcount % 17;
		bool flick = meme <= 2;

		if (vars.antiaim.fakehead_mode == 0)
		{
			flick = meme == 15;
		}

		if (flick)
		{
			csgo->cmd->viewangles.y += 90 * side;
		}
	}

	if(!csgo->send_packet) {

		if (side > 0) {
			csgo->cmd->viewangles.y += 180.f + desync_amount;
		}
		else {
			csgo->cmd->viewangles.y += 180.f - desync_amount;
		}
	}
	else {
		csgo->cmd->viewangles.y += 180.f;
	}

	csgo->cmd->viewangles.y += vars.antiaim.jitter_angle * (sw ? 1 : -1);

	if (!legit_aa) {

		if (vars.antiaim.base_yaw == 1 && !vars.antiaim.manual_antiaim) {
			csgo->cmd->viewangles.y -= 180.f;
		}

		if (vars.antiaim.manual_antiaim) {
			if (vars.antiaim.manual_forward.state)
				csgo->cmd->viewangles.y -= 180.f;
			if (vars.antiaim.manual_left.state)
				csgo->cmd->viewangles.y -= 90.f;
			if (vars.antiaim.manual_right.state)
				csgo->cmd->viewangles.y += 90.f;
		}
	}
	else {
		csgo->cmd->viewangles.y -= 180.f;
	}

	if (csgo->send_packet)
		sw = !sw;

	//else {
		//last_angle = csgo->cmd->viewangles.y;
	//}
}

void CMAntiAim::Run()
{
	if (vars.antiaim.slowwalk.state || csgo->should_stop_slide)
	{

		//csgo->cmd->buttons &= ~IN_SPEED;

		const auto weapon = csgo->weapon;
		if (weapon) {
			const auto info = csgo->weapon->GetCSWpnData();
			float speed = (csgo->local->IsScoped() ? info->m_flMaxSpeedAlt : info->m_flMaxSpeed) * (std::clamp(vars.antiaim.slowwalkspeed, 0, 100) / 100.f);
			if (info) {
				speed /= 3.4f;
				float min_speed = (float)(sqrt(pow(csgo->cmd->forwardmove, 2) + pow(csgo->cmd->sidemove, 2) + pow(csgo->cmd->upmove, 2)));

				if (min_speed > speed && min_speed > 0.f)
				{
					float ratio = speed / min_speed;
					csgo->cmd->forwardmove *= ratio;
					csgo->cmd->sidemove *= ratio;
					csgo->cmd->upmove *= ratio;
				}
			}
		}
		csgo->should_stop_slide = false;
	}

	should_aa = true;

	if (!vars.antiaim.enable) {
		should_aa = false;
		return;
	}

	if (csgo->game_rules->IsFreezeTime()
		|| csgo->local->GetFlags() & FL_FROZEN) {
		should_aa = false;
		return;
	}

	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP
		|| csgo->local->GetMoveType() == MOVETYPE_LADDER)
	{
		should_aa = false;
		return;
	}

	bool use_aa_on_e = !csgo->local->IsDefusing() && vars.antiaim.aa_on_use && csgo->cmd->buttons & IN_USE;

	if (csgo->cmd->buttons & IN_USE && !use_aa_on_e)
	{
		should_aa = false;
		return;
	}

	if (F::Shooting())
	{
		should_aa = false;
		return;
	}

	if (should_aa)
	{
		Pitch(use_aa_on_e);
		Yaw(use_aa_on_e);

		if (vars.misc.restrict_type == 1 && vars.antiaim.zleanenable) {

			if (csgo->local->GetVelocity().Length2D() <= 10.f) {
				csgo->cmd->viewangles.z = vars.antiaim.zlean;
			}
		}
	}
}