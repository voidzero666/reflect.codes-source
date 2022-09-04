#include "Hooks.h"
#include "Autopeek.h"
#include "Bunnyhop.h"
#include "Legitbot.h"
#include "Ragebot.h"
#include "AntiAims.h"
#include "checksum_md5.h"
#include "Resolver.h"
#include "Knifebot.h"
#include "Misc.h"
#include "Visuals.h"
#include "EnginePrediction.h"
#include "NetChannelhook.h"
#include "WriteUsercmdDeltaToBuffer.h"
#include "RunCommand.h"
#include "Movement.h"
#include "GUI/window.h"
#include "Menu.h"
#include "Recorder.h"
#include "scripting.h"
#include "Features.h"

#define shift_ticks 14

bool __stdcall Hooked_CreateMove(float inputSampleTime, CUserCmd* cmd, bool& bSendPacket) {
	//static auto CreateMove = g_pClientModeHook->GetOriginal< CreateMoveFn >(24);

	//g_pClientModeHook->callOriginal<bool, 24>(interfaces.client_mode, a, cmd);
	//csgo->updatelocalplayer();



	//if (!cmd || !cmd->command_number || !csgo->local || csgo->DoUnload)
		//return g_pClientModeHook->callOriginal<bool, 24>(inputSampleTime, cmd);

	csgo->in_cm = true;
	csgo->cmd = cmd;
	csgo->original = cmd->viewangles;



	//uintptr_t* pebp;
	//__asm mov pebp, ebp;

	csgo->send_packet = true;
	//csgo->forceLbyChoke = false;
	csgo->m_nTickbaseShift = 0;

	Misc::Get().Clantag();
	Misc::Get().RevealRanks(cmd);
	features->Visuals->RecoilCrosshair();
	Misc::Get().MouseDelta();
	features->Visuals->Graph_CollectInfo();
	Misc::Get().ProcessMissedShots();

	INetChannel* pNetChannel = (INetChannel*)csgo->client_state->pNetChannel;
	//static auto mat_dof_enabled = interfaces.cvars->FindVar(crypt_str("mat_dof_enabled"));
	//static auto mat_dof_override = interfaces.cvars->FindVar(crypt_str("mat_dof_override"));
	//mat_dof_enabled->SetValue(vars.visuals.motion_blur);
	//mat_dof_override->SetValue(vars.visuals.motion_blur);
	features->Visuals->DrawDlights();

	// Unpredicted Createmove Scripting yes?
	if (csgo->script_loaded)
		for (auto current : CLua::Get().hooks.get_hooks(crypt_str("createmove")))
			current.func(cmd);


	if (csgo->local->isAlive()) {

		if (csgo->dt_charged) {
			[&]() {
				cmd->tick_count = INT_MAX;
				cmd->forwardmove = 0.0f;
				cmd->sidemove = 0.0f;
				cmd->upmove = 0.0f;
				cmd->buttons &= ~IN_ATTACK;
				cmd->buttons &= ~IN_ATTACK2;

				if (++csgo->skip_ticks >= shift_ticks)
				{
					csgo->dt_charged = false;
					bSendPacket = true; //sendpacket true
				}
				else
					bSendPacket = false; //sendpacket false
			}();
			return false;
		}

		[&]() {
			static bool once = false;
			if (vars.ragebot.double_tap.state || vars.ragebot.hideShots.state) {
				if (!once) {
					Misc::Get().double_tap_key = true;
					once = true;
				}
			}
			else
				once = false;
			if (csgo->skip_ticks < shift_ticks && (Misc::Get().double_tap_enabled && Misc::Get().double_tap_key))
				csgo->dt_charged = true;
		}();

		[]() {
			if (csgo->cmd->buttons & IN_FORWARD && csgo->cmd->buttons & IN_BACK)
			{
				csgo->cmd->buttons &= ~IN_FORWARD;
				csgo->cmd->buttons &= ~IN_BACK;
			}

			if (csgo->cmd->buttons & IN_MOVELEFT && csgo->cmd->buttons & IN_MOVERIGHT)
			{
				csgo->cmd->buttons &= ~IN_MOVELEFT;
				csgo->cmd->buttons &= ~IN_MOVERIGHT;
			}
		}();


		[&]() {
			csgo->fixed_tickbase = csgo->local->GetTickBase();
			interfaces.global_vars->curtime = TICKS_TO_TIME(csgo->local->GetTickBase());
			csgo->tick_rate = 1.f / interfaces.global_vars->interval_per_tick;
			csgo->vecUnpredictedVel = csgo->local->GetVelocity();
			csgo->unpdred_tick = interfaces.global_vars->tickcount;
			csgo->weapon = csgo->local->GetWeapon();
			csgo->origin = csgo->local->GetOrigin();
			csgo->unpred_eyepos = csgo->local->GetEyePosition();
			csgo->duck_amount = csgo->local->GetDuckAmount();
			csgo->unpred_inaccuracy = csgo->weapon->GetInaccuracy();
			csgo->flags = csgo->local->GetFlags();
			csgo->weapon_range = []() {
				if (const auto& info = csgo->weapon->GetCSWpnData(); info != nullptr)
					return info->m_flRange;
				return -1.f;
			}();
			Ragebot::Get().DropTarget();
		}();
		features->Bunnyhop->Run(csgo->original);
		Movement::Get().FastDuck(cmd);
		Movement::Get().EBDetect(cmd);
		Movement::Get().JumpStats(cmd);
		Movement::Get().JumpStatsBhop(cmd);



		[&]() {
			if (csgo->weapon)
			{
				csgo->exploits = vars.ragebot.double_tap.state && Misc::Get().double_tap_key;

				[&]() {
					csgo->original_forwardmove = cmd->forwardmove;
					csgo->original_sidemove = cmd->sidemove;

					engine_prediction->update();
					Misc::Get().FakeDuck();
					//Ragebot::Get().DoQuickStop();
					engine_prediction->update();
					engine_prediction->start(csgo->local, cmd);
					//engine_prediction->saveprediction();
				}();

				// Predicted Createmove Scripting yes?
				if (csgo->script_loaded)
					for (auto current : CLua::Get().hooks.get_hooks(crypt_str("pred_createmove")))
						current.func(cmd);



				[&]() {
					g_NetData->RecordViewmodelValues();

					CMAntiAim::Get().Fakelag();

					csgo->weapon_data = csgo->weapon->GetCSWpnData();
					if (csgo->weapon->IsGun())
					{
						csgo->spread = csgo->weapon->GetSpread();
						csgo->innacuracy = csgo->weapon->GetInaccuracy();
						csgo->weaponspread = (csgo->spread + csgo->innacuracy) * 1000.f;
					}
					else
						csgo->weaponspread = 0.f;

					if (vars.misc.knifebot)
						CKnifebot::Get().Run();

					bool rageTick = false;

					if (vars.ragebot.enable) {
						if (!vars.ragebot.onkey) {
							rageTick = true;
						}
						else if (vars.ragebot.onkey && (vars.ragebot.key.properstate() || (vars.ragebot.key.key == 1 && cmd->buttons & IN_ATTACK))) {
							rageTick = true;
						}
					}

					csgo->ragetick = rageTick;
					//g_AutoWall->CacheWeaponData();

					if (rageTick) {
						Ragebot::Get().in_ragebot = true;
						Ragebot::Get().Run();
						Ragebot::Get().in_ragebot = false;
					}



					csgo->eyepos = csgo->local->GetEyePosition();

					if (vars.legitbot.backtrack)
						Legitbot::Get().Backtrack();

					if (vars.legitbot.aimbot)
						Legitbot::Get().Run(cmd);

					Legitbot::Get().Triggerbot(cmd);

					//if (vars.antiaim.enable)
					//{
					CMAntiAim::Get().Run();
					if (!CAutopeek::Get().has_shot)
						CMAntiAim::Get().Sidemove();
					//}
					CAutopeek::Get().Run();

					Movement::Get().Run(cmd);

					Misc::Get().Blockbot(cmd);

					Misc::Get().Doubletap();



					if (vars.legitbot.autopistol)
						Legitbot::Get().AutoPistol();

					//Desync on shot
					if (vars.antiaim.shotDesync) {
						if (cmd->buttons & IN_ATTACK && !csgo->fake_duck) {
							if (!csgo->send_packet && csgo->client_state->iChokedCommands != 0) {
								if ((cmd->command_number - csgo->m_shot_command_number) == 1) { //1 command before shot gets sent
									cmd->viewangles.y -= vars.antiaim.inverter.state * 58.f; // B-))
								}
							}
						}
					}

					//force packet when fakelagging when exploits not ran
					if (F::Shooting() && !vars.ragebot.double_tap.state && !vars.ragebot.hideShots.state) {
						csgo->m_shot_command_number = cmd->command_number;
						if (!csgo->fake_duck)
							csgo->send_packet = true;
						//csgo->forceLbyChoke = false;
					}

					csgo->forcing_shot = false;

					[&]() {

						if (cmd->command_number >= csgo->m_shot_command_number
							&& csgo->m_shot_command_number >= cmd->command_number - csgo->client_state->iChokedCommands) {
							csgo->forcing_shot = true;
							if (csgo->send_packet)
								csgo->FakeAngle = interfaces.input->m_pCommands[csgo->m_shot_command_number % 150].viewangles;
							else
								csgo->VisualAngle = interfaces.input->m_pCommands[csgo->m_shot_command_number % 150].viewangles;

							csgo->CurAngle = interfaces.input->m_pCommands[csgo->m_shot_command_number % 150].viewangles;
						}
						else {
							if (csgo->send_packet)
								csgo->FakeAngle = csgo->cmd->viewangles;
							else
								csgo->VisualAngle = csgo->cmd->viewangles;


							csgo->CurAngle = csgo->cmd->viewangles;
						}
					}();

					if (!csgo->isInLby) {
						csgo->lbylessAng = csgo->CurAngle;
					}

					if (vars.misc.restrict_type != 2) {
						//g_Animfix->UpdateFakeState();
						//g_Animfix->UpdateRealState();
					}

					engine_prediction->end(csgo->local);


					if (vars.misc.antiuntrusted)
						csgo->cmd->viewangles = Math::normalize(csgo->cmd->viewangles);

					if (vars.antiaim.simtimeStopper) {
						cmd->viewangles.y = csgo->local->GetLBY();
					}

					Misc::Get().FixMovement(cmd, csgo->original);
				}();

				static auto previous_ticks_allowed = csgo->skip_ticks;

				if (csgo->send_packet && csgo->client_state->pNetChannel)
				{
					auto choked_packets = csgo->client_state->pNetChannel->iChokedPackets;

					if (choked_packets >= 0)
					{
						auto ticks_allowed = csgo->skip_ticks;
						auto command_number = cmd->command_number - choked_packets;

						do
						{
							auto command = &interfaces.input->m_pCommands[cmd->command_number - 150 * (command_number / 150) - choked_packets];

							if (!command || command->tick_count > interfaces.global_vars->tickcount * 2)
							{
								if (--ticks_allowed < 0)
									ticks_allowed = 0;

								csgo->skip_ticks = ticks_allowed;
							}

							++command_number;
							--choked_packets;
						} while (choked_packets >= 0);
					}
				}

				if (csgo->skip_ticks > 17)
					csgo->skip_ticks = clamp(csgo->skip_ticks - 1, 0, 17);

				int tick_to_choke = 0;
				if (previous_ticks_allowed && !csgo->skip_ticks)
					tick_to_choke = 16;

				previous_ticks_allowed = csgo->skip_ticks;

				auto& correct = csgo->c_data.emplace_front();

				correct.command_number = csgo->cmd->command_number;
				correct.choked_commands = csgo->client_state->iChokedCommands + 1;
				correct.tickcount = interfaces.global_vars->tickcount;

				if (csgo->send_packet)
					csgo->choked_number.clear();
				else
					csgo->choked_number.emplace_back(correct.command_number);

				while (csgo->c_data.size() > (int)(2.0f / interfaces.global_vars->interval_per_tick))
					csgo->c_data.pop_back();

				auto& out = csgo->packets.emplace_back();

				out.is_outgoing = csgo->send_packet;
				out.is_used = false;
				out.cmd_number = csgo->cmd->command_number;
				out.previous_command_number = 0;

				while (csgo->packets.size() > (int)(1.0f / interfaces.global_vars->interval_per_tick))
					csgo->packets.pop_front();

				if (!csgo->send_packet && !csgo->game_rules->IsValveDS())
				{
					auto net_channel = csgo->client_state->pNetChannel;

					if (net_channel->iChokedPackets > 0 && !(net_channel->iChokedPackets % 4))
					{
						auto backup_choke = net_channel->iChokedPackets;
						net_channel->iChokedPackets = 0;

						net_channel->SendDatagram();
						--net_channel->iOutSequenceNr;

						net_channel->iChokedPackets = backup_choke;
					}
				}

				CGrenadePrediction::Get().Tick(csgo->cmd->buttons);
			}
		}();
		//resolver->StoreAntifreestand();

		Misc::Get().UpdateDormantTime();
		Misc::Get().StoreTaserRange();

		//Movement::Get().AutoDuck(cmd);

		Movement::Get().LongJump(cmd);
		Movement::Get().JumpBug(cmd);
		Movement::Get().AutoPixelSurf(cmd);
		Movement::Get().PixelSurfAlign(cmd);

		int svbt = cmd->buttons;
		Movement::Get().EdgeBug(cmd);
		if (interfaces.global_vars->tickcount < csgo->detectdata.detecttick || interfaces.global_vars->tickcount > csgo->detectdata.edgebugtick)
			cmd->buttons = svbt;
		if (interfaces.prediction->Split->nCommandsPredicted > 1)
			engine_prediction->restoreprediction(0, interfaces.prediction->Split->nCommandsPredicted - 1);

		MoveRecorder->Hook(cmd, csgo->origin);

		static INetChannel* old_net_chan = nullptr;

		if (csgo->is_connected) {
			csgo->ping = interfaces.engine->GetNetChannelInfo()->GetAvgLatency(FLOW_OUTGOING);
			csgo->ping *= 1000.f;
		}
	}
	else {
		csgo->ForceOffAA = false;
		csgo->should_draw_taser_range = false;
	}
	/*
	[&]() {
		if (csgo->client_state != nullptr
			&& pNetChannel != nullptr)
		{
			csgo->g_pNetChannelHook = std::make_unique<MinHook>();
			csgo->g_pNetChannelHook->init(pNetChannel);
			csgo->g_pNetChannelHook->hookAt(46, Hooked_SendDatagram);
		}
	}();
	*/

	if (vars.misc.antiuntrusted) {
		cmd->viewangles.Normalize();
		cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.f, 89.f);
		cmd->viewangles.y = std::clamp(cmd->viewangles.y, -179.9f, 179.9f);
		if (vars.misc.restrict_type == 1) {
			cmd->viewangles.z = std::clamp(cmd->viewangles.z, -44.f, 44.f);
		}
		else {
			cmd->viewangles.z = 0.f;
		}
		cmd->forwardmove = std::clamp(cmd->forwardmove, -450.f, 450.f);
		cmd->sidemove = std::clamp(cmd->sidemove, -450.f, 450.f);
		cmd->upmove = std::clamp(cmd->upmove, -450.f, 450.f);
	}


	csgo->lasttickyaw = cmd->viewangles.y;
	csgo->last_sendpacket = csgo->send_packet;
	csgo->in_cm = false;
	//if (csgo->forceLbyChoke) {
		//*(bool*)(*pebp - 0x1C) = false;
	//}
	//else {
	bSendPacket = csgo->send_packet;
	//}
	return false;
}

void __stdcall CHLCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
{
	g_pClientHook->callOriginal<void, 22>(sequence_number, input_sample_frametime, active);
	csgo->updatelocalplayer();

	CUserCmd* cmd = interfaces.input->GetUserCmd(0, sequence_number);

	if (!cmd || !cmd->command_number || !csgo->local || csgo->DoUnload || csgo->in_prediction)
		return;

	CVerifiedUserCmd* pVerified = interfaces.input->GetVerifiedUserCmd(sequence_number);

	if (!pVerified)
		return;

	bool cmoveactive = Hooked_CreateMove(input_sample_frametime, cmd, bSendPacket);

	pVerified->m_cmd = *cmd;
	pVerified->m_crc = cmd->GetChecksum();
}

#pragma warning(disable : 4409)
__declspec(naked) void __stdcall hkCreateMoveProxy(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		PUSH	EBP
		MOV		EBP, ESP
		PUSH	EBX
		LEA		ECX, [ESP]
		PUSH	ECX
		PUSH	active
		PUSH	input_sample_frametime
		PUSH	sequence_number
		CALL	CHLCreateMove
		POP		EBX
		POP		EBP
		RETN	0xC
	}
}


