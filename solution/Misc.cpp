#include "Misc.h"
#include "Ragebot.h"
#include "Autopeek.h"
#include "EnginePrediction.h"

template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
	return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

void Misc::PreserveKillFeed(bool roundStart)
{

	static auto nextUpdate = 0.0f;

	if (roundStart || !vars.visuals.preverse_killfeed) {
		nextUpdate = interfaces.global_vars->realtime + 10.0f;
		return;
	}

	if (nextUpdate > interfaces.global_vars->realtime)
		return;

	nextUpdate = interfaces.global_vars->realtime + 2.0f;

	const auto deathNotice = std::uintptr_t(H::findHudElement(H::hud, crypt_str("CCSGO_HudDeathNotice")));
	if (!deathNotice)
		return;

	const auto deathNoticePanel = (*(UIPanel**)(*reinterpret_cast<std::uintptr_t*>(deathNotice - 20 + 88) + sizeof(std::uintptr_t)));

	const auto childPanelCount = deathNoticePanel->GetChildCount();

	for (int i = 0; i < childPanelCount; ++i) {
		const auto child = deathNoticePanel->GetChild(i);
		if (!child)
			continue;

		if (child->HasClass(crypt_str("DeathNotice_Killer")))
			child->SetAttributeFloat(crypt_str("SpawnTime"), interfaces.global_vars->curtime);
	}
}

void Misc::FixMovement(CUserCmd* cmd, Vector& ang)
{
	if (!csgo->local)
		return;

	Vector  move, dir;
	float   delta, len;
	Vector   move_angle;

	if (!(csgo->local->GetFlags() & FL_ONGROUND) && cmd->viewangles.z != 0 && cmd->buttons & IN_ATTACK)
		cmd->sidemove = 0;

	move = { cmd->forwardmove, cmd->sidemove, 0 };

	len = move.NormalizeMovement();

	if (!len)
		return;

	Math::VectorAngles(move, move_angle);

	delta = (cmd->viewangles.y - ang.y);

	move_angle.y += delta;

	Math::AngleVectors(move_angle, &dir);

	dir *= len;

	if (csgo->local->GetMoveType() == MOVETYPE_LADDER) {
		if (cmd->viewangles.x >= 45 && ang.x < 45 && std::abs(delta) <= 65)
			dir.x = -dir.x;

		cmd->forwardmove = dir.x;
		cmd->sidemove = dir.y;

		if (cmd->forwardmove > 200)
			cmd->buttons |= IN_FORWARD;

		else if (cmd->forwardmove < -200)
			cmd->buttons |= IN_BACK;

		if (cmd->sidemove > 200)
			cmd->buttons |= IN_MOVERIGHT;

		else if (cmd->sidemove < -200)
			cmd->buttons |= IN_MOVELEFT;
	}
	else {
		if (cmd->viewangles.x < -90 || cmd->viewangles.x > 90)
			dir.x = -dir.x;

		cmd->forwardmove = dir.x;
		cmd->sidemove = dir.y;
	}

	cmd->forwardmove = clamp(cmd->forwardmove, -450.f, 450.f);
	cmd->sidemove = clamp(cmd->sidemove, -450.f, 450.f);
	cmd->upmove = clamp(cmd->upmove, -320.f, 320.f);

	if (csgo->local->GetMoveType() != MOVETYPE_LADDER)
		cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);
}

bool Misc::IsChatOpened() {
	if (!csgo->local)
		return false;
	/*
	static auto hud_ptr = *(DWORD**)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);

	static auto find_hud_element =
		reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			crypt_str("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));\
			*/

	if (!H::findHudElement || !H::hud)
		return false;

	auto chat = H::findHudElement(H::hud, crypt_str("CCSGO_HudChat"));
	if (!chat)
		return false;

	bool is_in_chat = *(bool*)((DWORD)chat + 0xD);

	if (is_in_chat)
		return true;

	return false;
}

void Misc::CopyCommand(CUserCmd* cmd, int tickbase_shift)
{
	if (vars.ragebot.dt_teleport)
	{
		/*
		Vector vMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float flSpeed = sqrt(vMove.x * vMove.x + vMove.y * vMove.y), flYaw;
		Vector vMove2;
		Math::VectorAngles(vMove, vMove2);
		vMove2.Normalize();
		flYaw = DEG2RAD(cmd->viewangles.y - csgo->original.y + vMove2.y);
		cmd->forwardmove = cos(flYaw) * flSpeed;
		cmd->sidemove = sin(flYaw) * flSpeed;
		*/
		/**/
		if (abs(cmd->sidemove) > 10.f)
			cmd->sidemove = copysignf(450.f, cmd->sidemove);
		else
			cmd->sidemove = 0.f;

		if (abs(cmd->forwardmove) > 10.f)
			cmd->forwardmove = copysignf(450.f, cmd->forwardmove);
		else
			cmd->forwardmove = 0.f;

	}
	else {
		cmd->forwardmove = 0.0f;
		cmd->sidemove = 0.0f;
	}

	auto commands_to_add = 0;

	csgo->shift_cmd = cmd->command_number;
	/**/
	do
	{
		auto sequence_number = commands_to_add + cmd->command_number;

		auto command = interfaces.input->GetUserCmd(sequence_number);
		auto verified_command = interfaces.input->GetVerifiedUserCmd(sequence_number);

		memcpy(command, cmd, sizeof(CUserCmd)); //-V598

		if (command->tick_count != INT_MAX && csgo->client_state->iDeltaTick > 0)
			interfaces.prediction->Update(
				csgo->client_state->iDeltaTick, true,
				csgo->client_state->nLastCommandAck,
				csgo->client_state->nLastOutgoingCommand + csgo->client_state->iChokedCommands
			);

		command->command_number = sequence_number;
		command->hasbeenpredicted = command->tick_count != INT_MAX;

		++csgo->client_state->iChokedCommands; //-V807

		if (csgo->client_state->pNetChannel)
		{
			++csgo->client_state->pNetChannel->iChokedPackets;
			++csgo->client_state->pNetChannel->iOutSequenceNr;
		}

		command->viewangles = Math::normalize(command->viewangles);

		memcpy(&verified_command->m_cmd, command, sizeof(CUserCmd)); //-V598
		verified_command->m_crc = command->GetChecksum();

		++commands_to_add;
	} while (commands_to_add != tickbase_shift);

	*(bool*)((uintptr_t)interfaces.prediction + 0x24) = true;
	*(int*)((uintptr_t)interfaces.prediction + 0x1C) = 0;
}

void Misc::CalculateVelocityModifierValue()
{
	return;
}

void Misc::Doubletap()
{
	static int last_doubletap = 0;
	double_tap_enabled = true;
	static auto recharge_double_tap = false;

	if (recharge_double_tap)
	{
		recharge_double_tap = false;
		recharging_double_tap = true;
		csgo->skip_ticks = 0;
		return;
	}

	if (recharging_double_tap)
	{
		if (fabs(csgo->weapon->GetLastShotTime() - TICKS_TO_TIME(csgo->fixed_tickbase)) > TICKS_TO_TIME(21) && !csgo->should_stop_slide) {
			recharging_double_tap = false;
			double_tap_key = true;
			dt_bullets = 0;
		}
		else if (csgo->cmd->buttons & IN_ATTACK) {
			dt_bullets++;
		}
	}

	if (!vars.ragebot.enable)
	{
		double_tap_enabled = false;
		double_tap_key = false;
		csgo->skip_ticks = 0;
		return;
	}
	if (!vars.ragebot.double_tap.state && !vars.ragebot.hideShots.state)
	{
		double_tap_enabled = false;
		double_tap_key = false;
		csgo->skip_ticks = 0;
		return;
	}
	if (!double_tap_key)
	{
		double_tap_enabled = false;
		csgo->skip_ticks = 0;
		//g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (csgo->game_rules->IsFreezeTime() || csgo->local->HasGunGameImmunity() || csgo->local->GetFlags() & FL_FROZEN) //-V807
	{
		double_tap_enabled = false;
		csgo->skip_ticks = 0;
		return;
	}
	/*
	if (csgo->game_rules->IsValveDS())
	{
		//dont disable lol
		double_tap_enabled = true;
		csgo->skip_ticks = 0;
		return;
	}
	*/

	if (csgo->fake_duck)
	{
		double_tap_enabled = false;
		csgo->skip_ticks = 0;
		return;
	}

	auto max_tickbase_shift = csgo->weapon->GetMaxTickbaseShift();
	bool can_dt =
		!csgo->weapon->IsMiscWeapon()
		&& csgo->weapon->IsGun()
		&& csgo->weapon->GetItemDefinitionIndex() != WEAPON_TASER
		&& csgo->weapon->GetItemDefinitionIndex() != WEAPON_REVOLVER;



	if (csgo->game_rules->IsValveDS() || vars.ragebot.hideShots.state) {
		max_tickbase_shift = min(max_tickbase_shift, 6);
	}

	float latency = interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
	if (latency >= 150.f && !vars.ragebot.hideshot) {
		max_tickbase_shift = min(max_tickbase_shift, 12);
	}

	bool is_firing = csgo->cmd->buttons & IN_ATTACK;

	auto wep = csgo->local->GetWeapon();
	if (wep) {
		if (csgo->local->GetShootsFired() > 0 && !csgo->local->GetWeapon()->IsAuto()) {
			can_dt = false;
		}

	}

	if (can_dt && is_firing)
	{
		auto next_command_number = csgo->cmd->command_number + 1;
		auto user_cmd = interfaces.input->GetUserCmd(next_command_number);
		csgo->m_current_shift = max_tickbase_shift;
		csgo->m_shift_command_number = csgo->cmd->command_number;

		memcpy(user_cmd, csgo->cmd, sizeof(CUserCmd));
		user_cmd->command_number = next_command_number;

		CopyCommand(user_cmd, max_tickbase_shift);

		recharge_double_tap = true;
		double_tap_enabled = false;
		double_tap_key = false;
		//csgo->send_packet = true;
		last_doubletap = csgo->fixed_tickbase;
		dt_bullets++;
	}
	else if (can_dt && !recharging_double_tap)
		csgo->m_nTickbaseShift = max_tickbase_shift;
}

void Misc::UpdateDormantTime() {
	for (int i = 1; i < 65; ++i)
	{
		auto ent = interfaces.ent_list->GetClientEntity(i);
		if (!ent)
			continue;
		if (ent->IsDormant() || !ent->isAlive() || !ent->IsPlayer()
			|| ent->EntIndex() == interfaces.engine->GetLocalPlayer()
			|| !ent->isEnemy(csgo->local)) {
			csgo->dormant_time[ent->GetIndex()] = -1.f;
			continue;
		}
		if (csgo->dormant_time[ent->GetIndex()] == -1.f)
			csgo->dormant_time[ent->GetIndex()] = ent->GetSimulationTime();
	}
}

void Misc::Clantag()
{
	auto SetClanTag = [](const char* tag, const char* name)
	{
		csgo->pSetClantag(tag, name);
	};

	auto Marquee = [](std::string& clantag)
	{
		std::string temp = clantag;
		clantag.erase(0, 1);
		clantag += temp[0];
	};

	static bool Reset = true;
	static auto lastTime = 0.0f;
	static std::string torotate{ vars.visuals.clantagcustomname };
	static std::string rotating{ vars.visuals.clantagcustomname };


	static float oldTime;
	float flCurTime = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
	static float flNextTimeUpdate = 0;
	static int iLastTime;


	if (csgo->local->GetFlags() & FL_FROZEN
		|| csgo->local->HasGunGameImmunity()
		//|| csgo->game_rules->IsFreezeTime()
		)
		return;

	if (vars.visuals.clantagspammer)
	{
		if (vars.visuals.clantagtype == 0)// static
		{
			if (interfaces.global_vars->realtime - lastTime < 1.0f)
				return;

			SetClanTag(crypt_str("Reflect.codes"), crypt_str("Reflect.codes"));
			lastTime = interfaces.global_vars->realtime;
		}
		else if (vars.visuals.clantagtype == 1) // animated
		{



			float latency = interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
			if (int(interfaces.global_vars->curtime * 2 + latency) != iLastTime)
			{
				if (flNextTimeUpdate <= flCurTime || flNextTimeUpdate - flCurTime > 1.f)
				{
					switch (int(interfaces.global_vars->curtime * 3.2) % 13) {
					case 0: SetClanTag(hs::clantag1.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 1: SetClanTag(hs::clantag2.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 2: SetClanTag(hs::clantag3.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 3: SetClanTag(hs::clantag4.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 4: SetClanTag(hs::clantag5.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 5: SetClanTag(hs::clantag6.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 6: SetClanTag(hs::clantag7.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 7: SetClanTag(hs::clantag8.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 8: SetClanTag(hs::clantag9.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 9: SetClanTag(hs::clantag10.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 10: SetClanTag(hs::clantag11.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 11: SetClanTag(hs::clantag12.s().c_str(), hs::ecstasy.s().c_str()); break;
					case 12: SetClanTag(hs::clantag13.s().c_str(), hs::ecstasy.s().c_str()); break;
					}
				}
				iLastTime = int(interfaces.global_vars->curtime * 2 + latency);
			}
			else
			{
				if (!Reset)
				{
					static int iLastTime;

					float latency = interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
					if (int(interfaces.global_vars->curtime * 2 + latency) != iLastTime)
					{
						SetClanTag(crypt_str(""), crypt_str(""));
						iLastTime = int(interfaces.global_vars->curtime * 2 + latency);
					}
					Reset = true;
				}
			}
		}
		else if (vars.visuals.clantagtype == 2) // custom
		{
			if (vars.visuals.clantagrotating)
			{
				if (interfaces.global_vars->realtime - lastTime < vars.visuals.clantagspeed)
					return;

				if (torotate != std::string(vars.visuals.clantagcustomname))
				{
					torotate = vars.visuals.clantagcustomname;
					rotating = vars.visuals.clantagcustomname;
				}

				if (!rotating.empty())
				{
					std::rotate(rotating.begin(), rotating.begin() + (rotating.size() - 1), rotating.end());
					SetClanTag(rotating.c_str(), torotate.c_str());
				}


				lastTime = interfaces.global_vars->realtime;
			}
			else
			{
				if (interfaces.global_vars->realtime - lastTime < 1.0f)
					return;

				SetClanTag(vars.visuals.clantagcustomname, vars.visuals.clantagcustomname);
				lastTime = interfaces.global_vars->realtime;
			}
		}
		else if (vars.visuals.clantagtype == 3) // clock
		{
			if (interfaces.global_vars->realtime - lastTime < 1.0f)
				return;

			const auto time = std::time(nullptr);
			const auto localTime = std::localtime(&time);
			char s[11];
			s[0] = '\0';
			snprintf(s, sizeof(s), crypt_str("[%02d:%02d:%02d]"), localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
			lastTime = interfaces.global_vars->realtime;
			SetClanTag(s, s);
		}

	}
}

void Misc::ProcessMissedShots()
{
	if (shot_snapshots.size() == 0)
		return;
	auto& snapshot = shot_snapshots.front();
	const auto& time = csgo->local->isAlive() ? csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick : interfaces.global_vars->curtime;
	if (fabs(time - snapshot.time) > 1.f)
	{
		if (snapshot.weapon_fire) {
			if (snapshot.bullet_impact)
				Msg(crypt_str("Shot rejected due to ping"), color_t(255, 0, 0));
			else
				Msg(crypt_str("Shot rejected by server"), color_t(255, 0, 0));
		}
		shot_snapshots.erase(shot_snapshots.begin());
		return;
	}

	if (snapshot.first_processed_time != -1.f) {
		if (snapshot.damage == -1 && snapshot.weapon_fire && snapshot.bullet_impact && snapshot.record.player) {
			string msg;
			msg += crypt_str("Missed shot to ") + snapshot.entity->GetName();
			bool resolver = false;
			bool spread = false;
			bool sp_spread = false;
			bool enemy_death = false;
			bool local_death = false;
			bool occlusion = false;

			if (snapshot.record.player) {
				const auto studio_model = interfaces.models.model_info->GetStudioModel(snapshot.record.player->GetModel());

				if (studio_model)
				{
					const auto angle = Math::CalculateAngle(snapshot.start, snapshot.impact);
					Vector forward;
					Math::AngleVectors(angle, forward);
					const auto end = snapshot.impact + forward * 2000.f;

					if (CanHitHitbox(snapshot.start, end, &snapshot.record, snapshot.hitbox, snapshot.record.bones))
					{
						resolver = true;
					}
					else {

						if (!snapshot.record.player->isAlive()) {
							enemy_death = true;
						}
						else if (snapshot.start.DistTo(snapshot.impact) < snapshot.start.DistTo(snapshot.intended_position)) {
							occlusion = true;
						}
						else {
							spread = true;
						}
					}




					/*
					if (snapshot.record.safepoints) {
						if (!CanHitHitbox(snapshot.start, end, &snapshot.record, snapshot.hitbox, snapshot.record.positive_matrix)
							|| !CanHitHitbox(snapshot.start, end, &snapshot.record, snapshot.hitbox, snapshot.record.negative_matrix))
							sp_spread = true;
					}
					*/
				}
			}
			auto& new_hitlog = csgo->custom_hitlog.emplace_back();
			new_hitlog.name = snapshot.entity->GetName();
			new_hitlog.backtrack = snapshot.backtrack;
			new_hitlog.hitbox = snapshot.hitbox;
			new_hitlog.resolver = snapshot.resolver;
			new_hitlog.spread = spread || sp_spread;

			if (resolver) {
				if (!snapshot.record.didshot) {
					msg += crypt_str(" due to desync ");
					csgo->actual_misses[snapshot.entity->GetIndex()]++;
				}
				else {
					//do not increment the resolver misses when onshotting!
					msg += crypt_str(" due to onshot desync ");
					//csgo->onshot_misses[snapshot.entity->GetIndex()]++;
				}
			}
			else if (spread) {
				msg += crypt_str(" due to spread ");
			}
			else if (occlusion) {
				msg += crypt_str(" due to occlusion ");
			}
			else if (enemy_death) {
				msg += crypt_str(" due to enemy death ");
			}
			else {
				msg += crypt_str(" due to ? ");
			}

			//if (snapshot.record.resolved && resolver) {
				//msg += crypt_str(" RESOLVED RECORD");
			//}

			msg += snapshot.get_info();

			Msg(msg, vars.visuals.eventlog_color);

			shot_snapshots.erase(shot_snapshots.begin());
		}
	}
}

void Misc::FakeDuck()
{
	if (vars.misc.restrict_type == 0) {
		csgo->fake_duck = false;
		return;
	}

	if (csgo->game_rules->IsFreezeTime()
		|| csgo->local->HasGunGameImmunity()
		|| csgo->local->GetFlags() & FL_FROZEN
		|| csgo->cmd->buttons & IN_JUMP
		|| !(csgo->local->GetFlags() & FL_ONGROUND)
		|| !vars.antiaim.enable) {
		csgo->fake_duck = false;
		return;
	}

	csgo->cmd->buttons |= IN_BULLRUSH;

	int maxChoke = []() {
		int amt = csgo->game_rules->IsValveDS() ? 8 : 16; //maxproctick
		return clamp(amt, 0, 14);
	}();

	bool sendDuckCmd = csgo->client_state->iChokedCommands >= (maxChoke / 2);

	if (vars.antiaim.fakeduck.state)
	{
		csgo->fake_duck = true;

		if (sendDuckCmd)
			csgo->cmd->buttons &= ~IN_DUCK;
		else
			csgo->cmd->buttons |= IN_DUCK;

		csgo->send_packet = csgo->client_state->iChokedCommands >= maxChoke;
	}
	else
		csgo->fake_duck = false;
}

void Misc::MouseDelta()
{
	if (!csgo->local || !csgo->cmd)
		return;

	static Vector delta_viewangles{ };
	Vector delta = csgo->cmd->viewangles - delta_viewangles;
	delta.Clamp();

	static ConVar* sensitivity = interfaces.cvars->FindVar(crypt_str("sensitivity"));

	if (!sensitivity)
		return;

	if (delta.x != 0.f) {
		static ConVar* m_pitch = interfaces.cvars->FindVar(crypt_str("m_pitch"));

		if (!m_pitch)
			return;

		int final_dy = static_cast<int>((delta.x / m_pitch->GetFloat()) / sensitivity->GetFloat());
		if (final_dy <= 32767) {
			if (final_dy >= -32768) {
				if (final_dy >= 1 || final_dy < 0) {
					if (final_dy <= -1 || final_dy > 0)
						final_dy = final_dy;
					else
						final_dy = -1;
				}
				else {
					final_dy = 1;
				}
			}
			else {
				final_dy = 32768;
			}
		}
		else {
			final_dy = 32767;
		}

		csgo->cmd->mousedy = static_cast<short>(final_dy);
	}

	if (delta.y != 0.f) {
		static ConVar* m_yaw = interfaces.cvars->FindVar(crypt_str("m_yaw"));

		if (!m_yaw)
			return;

		int final_dx = static_cast<int>((delta.y / m_yaw->GetFloat()) / sensitivity->GetFloat());
		if (final_dx <= 32767) {
			if (final_dx >= -32768) {
				if (final_dx >= 1 || final_dx < 0) {
					if (final_dx <= -1 || final_dx > 0)
						final_dx = final_dx;
					else
						final_dx = -1;
				}
				else {
					final_dx = 1;
				}
			}
			else {
				final_dx = 32768;
			}
		}
		else {
			final_dx = 32767;
		}

		csgo->cmd->mousedx = static_cast<short>(final_dx);
	}

	delta_viewangles = csgo->cmd->viewangles;
}

void Misc::StoreTaserRange()
{
	Vector prev_scr_pos{ 0, 0, 0 };
	Vector scr_pos{ 0, 0, 0 };

	if (!csgo->local || !csgo->local->isAlive())
		return;

	auto local_weapon = csgo->weapon;
	if (!local_weapon || local_weapon->GetItemDefinitionIndex() != weapon_taser) {
		csgo->should_draw_taser_range = false;
		return;
	}

	csgo->should_draw_taser_range = true;

	float step = PI * 2.0f / 105.f;

	float rad = local_weapon->GetCSWpnData()->m_flRange - 15.f;

	Vector origin = csgo->local->GetRenderOrigin() + Vector(0, 0, 50);
	size_t i = 0;
	for (float rotation = 0; rotation <= (PI * 2.0); rotation += step)
	{
		Vector pos(rad * cos(rotation) + origin.x, rad * sin(rotation) + origin.y, origin.z);

		Ray_t ray;
		trace_t trace;
		CTraceFilter filter;
		filter.pSkip = csgo->local;
		ray.Init(origin, pos);

		interfaces.trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);
		csgo->taser_ranges[i++] = std::pair(trace.endpos, trace.endpos + Vector(0.f, 0.f, -10.f));
	}
}

void Misc::Ragdolls()
{
	if (!vars.visuals.ragdoll_force)
		return;

	for (auto i = 1; i <= interfaces.ent_list->GetHighestEntityIndex(); ++i)
	{
		auto e = interfaces.ent_list->GetClientEntity(i);

		if (!e)
			continue;

		if (e->IsDormant())
			continue;

		auto client_class = e->GetClientClass();

		if (!client_class)
			continue;

		if (client_class->m_ClassID != ClassId->CCSRagdoll)
			continue;

		e->GetVecForce() = Vector(800000.0f, 800000.0f, 800000.0f);
	}
}

static int blockTargetHandle = 0;

void Misc::Blockbot(CUserCmd* cmd)
{
	if (!csgo->local || !csgo->local->isAlive())
		return;

	const auto st = csgo->local->GetMoveType();

	if (st == MOVETYPE_NOCLIP || st == MOVETYPE_LADDER)
		return;

	if (!vars.misc.blockbot.properstate())
	{
		blockTargetHandle = 0;
		return;
	}

	float best = 1024.0f;
	if (!blockTargetHandle)
	{
		for (int i = 1; i <= interfaces.engine->GetMaxClients(); i++)
		{
			IBasePlayer* entity = interfaces.ent_list->GetClientEntity(i);

			if (!entity || !entity->IsPlayer() || entity == csgo->local || entity->IsDormant() || !entity->isAlive())
				continue;

			const auto distance = entity->GetAbsOrigin().DistTo(csgo->local->GetAbsOrigin());
			if (distance < best)
			{
				best = distance;
				blockTargetHandle = entity->GetRefEHandle();
			}
		}
	}

	const auto target = interfaces.ent_list->GetClientEntityFromHandle(blockTargetHandle);
	if (target && target->IsPlayer() && target != csgo->local && !target->IsDormant() && target->isAlive())
	{
		const auto targetVec = (target->GetAbsOrigin() + target->GetVelocity() * interfaces.global_vars->interval_per_tick * 1.f - csgo->local->GetAbsOrigin()) * 2.f;
		const auto z1 = target->GetAbsOrigin().z - csgo->local->GetEyePosition().z;
		const auto z2 = target->GetEyePosition().z - csgo->local->GetAbsOrigin().z;
		if (z1 >= 0.0f || z2 <= 0.0f)
		{
			Vector fwd = Vector{ cos(DEG2RAD(cmd->viewangles.y)), sin(DEG2RAD(cmd->viewangles.y)), 0.f };

			Vector side = fwd.Cross(Vector{ 0.f,0.f,1.f });
			Vector move = Vector{ fwd.x * targetVec.x + fwd.y * targetVec.y, fwd.x * side.x + fwd.y * side.y, 0.0f };
			move *= 45.0f;

			const float l = move.Length2D();
			if (l > 450.0f)
				move *= 450.0f / l;

			cmd->forwardmove = move.x;
			cmd->sidemove = move.y;
		}
		else
		{
			Vector fwd = Vector{ cos(DEG2RAD(cmd->viewangles.y)), sin(DEG2RAD(cmd->viewangles.y)), 0.f };
			Vector side = fwd.Cross(Vector{ 0.f,0.f,1.f });
			Vector tar = (targetVec / targetVec.Length2D()).Cross(Vector{ 0.f,0.f,1.f });
			tar = tar.snapTo4();
			tar *= tar.x * targetVec.x + tar.y * targetVec.y;
			Vector move = Vector{ fwd.x * tar.x + fwd.y * tar.y, side.x * tar.x + side.y * tar.y, 0.0f };
			move *= 45.0f;

			const float l = move.Length2D();
			if (l > 450.0f)
				move *= 450.0f / l;

			cmd->forwardmove = move.x;
			cmd->sidemove = move.y;
		}
	}
}

void Misc::RevealRanks(CUserCmd* cmd)
{
	if (vars.misc.revealranks && cmd->buttons & IN_SCORE)
		interfaces.client->dispatchUserMessage(UserMessageType::CS_UM_ServerRankRevealAll, 0, 0, nullptr);
}

std::string voteName(int index)
{
	switch (index) {
	case 0: return crypt_str("kick");
	case 1: return crypt_str("change level");
	case 6: return crypt_str("surrender");
	case 13: return crypt_str("timeout");
	default: return crypt_str("");
	}
}

void Misc::votestart(const void* data, int size)
{
	if (!vars.visuals.eventlog)
		return;

	const auto reader = ProtobufReader{ static_cast<const std::uint8_t*>(data), size };
	const auto entityIndex = reader.readInt32(2);

	const auto entity = interfaces.ent_list->GetClientEntity(entityIndex);
	if (!entity || !entity->IsPlayer())
		return;

	const auto voteType = reader.readInt32(3);

	std::string message = crypt_str("Vote called by ") + entity->GetName() + crypt_str(" for ") + voteName(voteType);

	Msg(message, vars.visuals.eventlog_color);
	if (vars.visuals.print_votes)
	{
		if (vars.visuals.print_votes_chat)
		{
			interfaces.engine->ClientCmd_Unrestricted(std::string(crypt_str("say \"") + message + '"').c_str());
		}
		else
		{
			interfaces.engine->ClientCmd_Unrestricted(std::string(crypt_str("say_team \"") + message + '"').c_str());
		}
	}
}

void Misc::modelchanger(ClientFrameStage_t stage)
{

	if (stage != FRAME_RENDER_START && stage != FRAME_RENDER_END)
		return;

	static int originalIdx = 0;

	if (!csgo->is_local_alive || !csgo->is_connected || !interfaces.engine->IsInGame()) {
		originalIdx = 0;
		return;
	}

	if (interfaces.engine->IsPlayingDemo()) {
		originalIdx = NULL;
		return;
	}

	constexpr auto getModel = [](int team) constexpr noexcept -> const char* {
		constexpr std::array models{
		"models/player/custom_player/legacy/ctm_fbi_variantb.mdl",
		"models/player/custom_player/legacy/ctm_fbi_variantf.mdl",
		"models/player/custom_player/legacy/ctm_fbi_variantg.mdl",
		"models/player/custom_player/legacy/ctm_fbi_varianth.mdl",
		"models/player/custom_player/legacy/ctm_sas_variantf.mdl",
		"models/player/custom_player/legacy/ctm_st6_variante.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantg.mdl",
		"models/player/custom_player/legacy/ctm_st6_varianti.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantk.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantm.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantf.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantg.mdl",
		"models/player/custom_player/legacy/tm_balkan_varianth.mdl",
		"models/player/custom_player/legacy/tm_balkan_varianti.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantj.mdl",
		"models/player/custom_player/legacy/tm_leet_variantf.mdl",
		"models/player/custom_player/legacy/tm_leet_variantg.mdl",
		"models/player/custom_player/legacy/tm_leet_varianth.mdl",
		"models/player/custom_player/legacy/tm_leet_varianti.mdl",
		"models/player/custom_player/legacy/tm_phoenix_variantf.mdl",
		"models/player/custom_player/legacy/tm_phoenix_variantg.mdl",
		"models/player/custom_player/legacy/tm_phoenix_varianth.mdl",
		"models/player/custom_player/legacy/tm_pirate.mdl",
		"models/player/custom_player/legacy/tm_pirate_varianta.mdl",
		"models/player/custom_player/legacy/tm_pirate_variantb.mdl",
		"models/player/custom_player/legacy/tm_pirate_variantc.mdl",
		"models/player/custom_player/legacy/tm_pirate_variantd.mdl",
		"models/player/custom_player/legacy/tm_anarchist.mdl",
		"models/player/custom_player/legacy/tm_anarchist_varianta.mdl",
		"models/player/custom_player/legacy/tm_anarchist_variantb.mdl",
		"models/player/custom_player/legacy/tm_anarchist_variantc.mdl",
		"models/player/custom_player/legacy/tm_anarchist_variantd.mdl",
		"models/player/custom_player/legacy/tm_balkan_varianta.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantb.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantc.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantd.mdl",
		"models/player/custom_player/legacy/tm_balkan_variante.mdl",
		"models/player/custom_player/legacy/tm_jumpsuit_varianta.mdl",
		"models/player/custom_player/legacy/tm_jumpsuit_variantb.mdl",
		"models/player/custom_player/legacy/tm_jumpsuit_variantc.mdl",
		"models/player/custom_player/legacy/tm_phoenix_varianti.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantj.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantl.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantk.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantl.mdl",
		"models/player/custom_player/legacy/ctm_swat_variante.mdl",
		"models/player/custom_player/legacy/ctm_swat_variantf.mdl",
		"models/player/custom_player/legacy/ctm_swat_variantg.mdl",
		"models/player/custom_player/legacy/ctm_swat_varianth.mdl",
		"models/player/custom_player/legacy/ctm_swat_varianti.mdl",
		"models/player/custom_player/legacy/ctm_swat_variantj.mdl",
		"models/player/custom_player/legacy/tm_professional_varf.mdl",
		"models/player/custom_player/legacy/tm_professional_varf1.mdl",
		"models/player/custom_player/legacy/tm_professional_varf2.mdl",
		"models/player/custom_player/legacy/tm_professional_varf3.mdl",
		"models/player/custom_player/legacy/tm_professional_varf4.mdl",
		"models/player/custom_player/legacy/tm_professional_varg.mdl",
		"models/player/custom_player/legacy/tm_professional_varh.mdl",
		"models/player/custom_player/legacy/tm_professional_vari.mdl",
		"models/player/custom_player/legacy/tm_professional_varj.mdl"
		};
		switch (team) {
		case 2: return static_cast<std::size_t>(vars.visuals.playermodelt - 1) < models.size() ? models[vars.visuals.playermodelt - 1] : nullptr;
		case 3: return static_cast<std::size_t>(vars.visuals.playermodelct - 1) < models.size() ? models[vars.visuals.playermodelct - 1] : nullptr;
		default: return nullptr;
		}
	};

	if (const auto model = getModel(csgo->local->GetTeam())) {
		if (stage == FRAME_RENDER_START) {
			originalIdx = csgo->local->GetModelIndex();
			if (const auto modelprecache = interfaces.network_string_table_container->find_table(crypt_str("modelprecache"))) {
				modelprecache->add_string(false, model);
				const auto viewmodelArmConfig = csgo->getPlayerViewmodelArmConfigForPlayerModel(model);
				modelprecache->add_string(false, viewmodelArmConfig[2]);
				modelprecache->add_string(false, viewmodelArmConfig[3]);
			}
		}

		if (stage == FRAME_RENDER_END && originalIdx && interfaces.engine->IsPlayingDemo())
			csgo->local->SetModelIndex(originalIdx);
		else
			csgo->local->SetModelIndex(interfaces.models.model_info->GetModelIndex(model));

		if (const auto ragdoll = static_cast<IBasePlayer*> (interfaces.ent_list->GetClientEntityFromHandle(csgo->local->ragdoll() && interfaces.engine->IsPlayingDemo()))) {

			if (stage == FRAME_RENDER_END && originalIdx)
				ragdoll->SetModelIndex(originalIdx);
			else
				ragdoll->SetModelIndex(interfaces.models.model_info->GetModelIndex(model));
		}
	}

}
