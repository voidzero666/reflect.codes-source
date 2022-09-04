#pragma once
#include "Hooks.h"
#include "RageBacktracking.h"
#include "Misc.h"
#include "Resolver.h"
#include "AnimationFix.h"
#include "SetupAnimation.h"
#include <intrin.h>
#include <random>
#include "Ragebot.h"
#include "nSkinz.hpp"
#include "scripting.h"

void UpdateViewmodel(IBasePlayer* player)
{
	static auto update_all_viewmodel_addons = reinterpret_cast<int(__fastcall*)(void*)>(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 83 E4 ? 83 EC ? 53 8B D9 56 57 8B 03 FF 90 ? ? ? ? 8B F8 89 7C 24 ? 85 FF 0F 84 ? ? ? ? 8B 17 8B CF")));

	static auto get_viewmodel = reinterpret_cast<void* (__thiscall*)(void*, bool)>(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 8B 45 08 53 8B D9 56 8B 84 83 ? ? ? ? 83 F8 FF 74 18 0F")));

	auto v10 = getvfunc<int(__thiscall*)(IBasePlayer*)>(player, 158)(player);
	if (!v10) return;

	auto v11 = getvfunc<int(__thiscall*)(IBasePlayer*)>(player, 268)(player);
	if (!v11) return;
	//fixed 
	{
		if (player->GetViewModel())
			update_all_viewmodel_addons(player->GetViewModel());
		return;
	}
	//fixed 
	//auto viewmodel = get_viewmodel(player, *(bool*)(v11 + 0x32F8)); //was 0x3224 ,its m_hViewModel 
	//if (!viewmodel) return;
	//
	//update_all_viewmodel_addons(viewmodel);
}

static bool __fastcall Hooked_GetBool(void* _this) noexcept
{
	static auto CAM_THINK = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::retn_camera.s().c_str());
	if (_ReturnAddress() == CAM_THINK)
		return true;

	return g_pGetBoolHook->getOriginal<bool, 13>()(_this);
}


bool __fastcall Hooked_GetBool1(void* thisp, void* edx)
{
	//static auto SvCheatsGetBool = g_pGetBoolHook->GetOriginal< SvCheatsGetBoolFn >(g_HookIndices[fnva1(hs::Hooked_GetBool.s().c_str())]);
	//static auto SvCheatsGetBool = g_pGetBoolHook->GetOriginal< SvCheatsGetBoolFn >(g_HookIndices[fnva1(hs::Hooked_GetBool.s().c_str())]);
	//typedef bool(__thiscall* SvCheatsGetBoolFn)(void*);



	//if (csgo->DoUnload)
		//return g_pGetBoolHook->callOriginal<bool, 13>();

	static auto CAM_THINK = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::retn_camera.s().c_str());
	if (_ReturnAddress() == CAM_THINK)
		return true;

	return g_pGetBoolHook->callOriginal<bool, 13>();
}

static bool enabledtp = false, check = false;

__forceinline void UpdateCSGOKeyBinds()
{
	if (vars.misc.thirdperson.state)
	{
		if (!check)
			enabledtp = !enabledtp;
		check = true;
	}
	else
		check = false;


	csgo->SwitchAA = vars.antiaim.inverter.state;
}

struct ClientHitVerify_t {
	Vector pos;
	float time;
	float expires;
};

static auto lagcompcmd_set = false;


void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	csgo->updatelocalplayer();
	static auto last_count = 0;
	auto s = vars.visuals.impacts_size;
	//static auto FrameStageNotify = g_pClientHook->GetOriginal< FrameStageNotifyFn >(g_HookIndices[fnva1(hs::Hooked_FrameStageNotify.s().c_str())]);
	// 
	// 
	// 
	// hooks->client.callOriginal<void, 37>(stage);
	//std::size_t idx = g_HookIndices[fnva1(hs::Hooked_FrameStageNotify.s().c_str())];


	//static auto FrameStageNotify = g_pClientHook->getOriginal<void>(curStage);

	if (csgo->script_loaded)
		for (auto current : CLua::Get().hooks.get_hooks(crypt_str("frame_stage")))
			current.func(curStage);

	static bool didOnce = false;
	/*
	if (curStage == FRAME_START) {
		if (vars.antiaim.simtimeStopper) {
			didOnce = true;
			auto address = csgo->Utils.FindPatternIDA(
				GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
				hs::choke_limit.s().c_str()) + 0x1;

			DWORD oldProtect = 0;
			VirtualProtect((void*)address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &oldProtect);
			if (*(uint32_t*)address != 0) {
				*(uint32_t*)address = 0;
			}
			VirtualProtect((void*)address, sizeof(uint32_t), oldProtect, &oldProtect);
		}
		else {
			if (didOnce) {
				auto address = csgo->Utils.FindPatternIDA(
					GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
					hs::choke_limit.s().c_str()) + 0x1;

				DWORD oldProtect = 0;
				VirtualProtect((void*)address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &oldProtect);
				if (*(uint32_t*)address != 17) {
					*(uint32_t*)address = 17;
				}
				VirtualProtect((void*)address, sizeof(uint32_t), oldProtect, &oldProtect);
				didOnce = false;
			}
		}
	}

	*/
	


	if (lagcompcmd_set != vars.ragebot.antiexploit) {

		std::string cmd;
		bool rj = false;
		int rteam = 1;
		if (interfaces.engine->IsInGame() && csgo->local) {
			rteam = csgo->local->GetTeam();
			interfaces.engine->ClientCmd_Unrestricted(crypt_str("kill"), 0);
			interfaces.engine->ClientCmd_Unrestricted(crypt_str("jointeam 1"), 0);

			static auto clextrapolate = interfaces.cvars->FindVar(crypt_str("cl_lagcompensation"));
			clextrapolate->m_fnChangeCallbacks.m_Size = 0;
			if (clextrapolate->GetInt() == vars.ragebot.antiexploit)
				clextrapolate->SetValue(!vars.ragebot.antiexploit);

			rj = true;
		}
		lagcompcmd_set = vars.ragebot.antiexploit;
	}


	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame()) {
		csgo->mapChanged = true;
		csgo->is_connected = false;
	}
	else csgo->is_connected = true;
	/*
	static auto hud_ptr = *(DWORD**)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);




	static auto find_hud_element =
		reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			crypt_str("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
			*/

	//if (!find_hud_element || !hud_ptr)
		//return g_pClientHook->callOriginal<void, 37>(curStage);

	Misc::Get().modelchanger(curStage);
	
	[&]() {

		/*
		if (csgo->client_state != nullptr) {
			if (csgo->g_pNetChannelHook) {
				if (csgo->client_state->pNetChannel != nullptr) {
					uintptr_t* vtable = *(uintptr_t**)csgo->client_state->pNetChannel;

					if (vtable != csgo->g_pNetChannelHook->shadow_vtable) {
						csgo->g_pNetChannelHook.reset();
					}
				}
				else
					csgo->g_pNetChannelHook.reset();
			}
		}
		*/

		if (curStage == FRAME_START) {
			features->Visuals->StoreOtherInfo();
			features->Visuals->RecordInfo();
			features->Visuals->sunsetmode();
			if (g_Menu->window)
				g_Menu->window->update_keystates();
			g_Menu->update_binds();

			if (interfaces.global_vars->tickcount >= csgo->detectdata.detecttick && interfaces.global_vars->tickcount <= csgo->detectdata.edgebugtick && csgo->detectdata.strafing)
			{
				Vector edgebugva = Vector{csgo->original.x, csgo->detectdata.startingyaw , csgo->original.z };
				
				float to_eb_time = TICKS_TO_TIME(csgo->detectdata.edgebugtick) - TICKS_TO_TIME(csgo->detectdata.detecttick); // whole time from detection to eb
				float from_detect_time = interfaces.global_vars->curtime - TICKS_TO_TIME(csgo->detectdata.detecttick); // time from detection to rn
				int tickamount = csgo->detectdata.edgebugtick - csgo->detectdata.detecttick; // whole time from detection to eb

				float addedyaw = Math::NormalizeYaw(csgo->detectdata.yawdelta * (tickamount * (from_detect_time / to_eb_time))); // -> from_detect_time / to_eb_time - percentage of time to edgebug
				edgebugva.y += addedyaw;

				interfaces.engine->SetViewAngles(edgebugva);

			}
			
		}
	}();

	if (curStage == FRAME_RENDER_START) {
		Misc::Get().PreserveKillFeed(csgo->round_start);

		if (csgo->round_start)
			csgo->round_start = false;

		static auto postProcess = (*(DWORD*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), 
			crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85")) + 0x2));

		bool shouldPostProcess = vars.visuals.remove[5];

		if (*(bool*)postProcess != shouldPostProcess)
			*(bool*)postProcess = shouldPostProcess;

		
	}

	if (!csgo->local || !csgo->local->isAlive()) {
		csgo->ForceOffAA = false;
		if (curStage == FRAME_NET_UPDATE_END) {
			for (int i = 1; i < 65; i++) {
				auto entity = interfaces.ent_list->GetClientEntity(i);
				if (entity != nullptr && entity->IsPlayer() && entity != csgo->local) {
					entity->GetClientSideAnims() = csgo->EnableBones = true;
				}
			}
			csgo->disable_dt = false;
		}
		return g_pClientHook->callOriginal<void, 37>(curStage);
	}

	[&]() {
		if (curStage == FRAME_RENDER_START)
		{
			if (csgo->game_rules == nullptr || csgo->mapChanged) {
				csgo->game_rules = **reinterpret_cast<CCSGameRules***>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
					g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
					hs::game_rules.s().c_str()) + 0x1);
				for (int i = 0; i < 64; i++)
					features->Visuals->player_info[i].Reset();

			}
		}
	}();

	if (curStage == FRAME_RENDER_START) {
		if (csgo->mapChanged) csgo->mapChanged = false;
		Misc::Get().Ragdolls();
	}

	UpdateCSGOKeyBinds();

	if (interfaces.engine->IsConnected() && interfaces.engine->IsInGame())
	{
		Misc::Get().CalculateVelocityModifierValue();

		if (curStage == FRAME_NET_UPDATE_END) {
			[&]()
			{
				/*
				static auto cl_threaded_bone_setup = interfaces.cvars->FindVar(crypt_str("cl_threaded_bone_setup"));
				if (cl_threaded_bone_setup->GetInt() <= 0)
					cl_threaded_bone_setup->SetValue(1);*/

				static auto r_jiggle_bones = interfaces.cvars->FindVar(crypt_str("r_jiggle_bones"));
				if (r_jiggle_bones->GetInt() >= 1)
					r_jiggle_bones->SetValue(0);

				static auto clextrapolate = interfaces.cvars->FindVar(crypt_str("cl_extrapolate"));
				if (clextrapolate->GetInt() >= 1)
					clextrapolate->SetValue(0);
					

				g_Animfix->UpdatePlayers();
				
				g_Animfix->UpdateFakeState();
			}();
		}


		if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {

			if (csgo->local) {
				if (csgo->local->isAlive())
				{
					g_NetData->ApplyViewmodelValues();
				}
				
			}
			Skinchanger::Get().postDataUpdateSkins(csgo->local);
		}

		if (curStage == FRAME_RENDER_START)
		{
			g_Animfix->FixPvs();

			if (csgo->local->isAlive()) {

				

				bool fr = csgo->game_rules->IsFreezeTime()
					|| csgo->local->HasGunGameImmunity()
					|| csgo->local->GetFlags() & FL_FROZEN;

				if (!fr) {
					
					g_Animfix->UpdateRealState();
					//csgo->local->GetPlayerAnimState()->m_flFootYaw = csgo->last_gfy;
					//csgo->local->SetAbsAngles(Vector(0, csgo->local->GetPlayerAnimState()->m_flFootYaw, 0));
				}

				UpdateViewmodel(csgo->local);
				if (vars.visuals.remove[2])
					csgo->local->GetFlashDuration() = 0.f;
				csgo->scoped = csgo->weapon && csgo->weapon->isSniper() && csgo->local->IsScoped();
				csgo->zoom_level = csgo->weapon->GetZoomLevel() * csgo->scoped;
			}
			interfaces.input->m_fCameraInThirdPerson = false;

			features->BulletTracer->Proceed();
		}

		csgo->disable_dt = false;
		csgo->ForceOffAA = false;
	}
	else {
		csgo->disable_dt = false;
		csgo->mapChanged = true;
		csgo->dt_charged = false;
		csgo->skip_ticks = 0;
	}

	g_pClientHook->callOriginal<void, 37>(curStage);

	if (curStage == FRAME_NET_UPDATE_END) {
		g_NetData->Apply();
	}
}

bool __fastcall Hooked_ShouldDrawFog(void* ecx, void* edx) {
	return !(vars.visuals.remove[6]);
}

bool __fastcall Hooked_ShouldDrawShadow(void*, uint32_t) {
	return !(vars.visuals.remove[7]);
}
