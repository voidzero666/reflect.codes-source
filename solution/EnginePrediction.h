#pragma once

#include "predictioncopy.h"




struct viewmodel_data_t
{
	IBaseCombatWeapon* weapon = nullptr;
	int viewmodel_index = 0;
	int sequence = 0;
	int animation_parity = 0;

	float cycle = 0.0f;
	float animation_time = 0.0f;
};

class c_engine_prediction
{
public:
	struct
	{
		float m_flFrametime;
		float m_flCurtime;
		int m_fFlags;
		int m_iTickCount;
		float m_flVelocityModifier;
		int m_iMoveType;

		Vector m_vecVelocity;
		Vector m_vecOrigin;
	} m_store;

	struct
	{
		int m_iRunCommandTickbase;
		bool m_bOverrideModifier;
	} m_other;


	float predicted_curtime;


	int* m_nPredictionRandomSeed = nullptr;
	int* m_pPredictionPlayer = nullptr;
	//uint32_t* m_nPredictionRandomSeed;
	//c_entity** m_pPredictionPlayer;
	CMoveData				m_move_data;
private:
	bool old_in_prediction;
	bool old_first_prediction;


	using unknown_function_think = char(__thiscall*)(IBasePlayer*, char);
	using PostThinkVPhysics_t = bool(__thiscall*)(IBasePlayer*);
	using SimulatePlayerSimulatedEntities_t = void(__thiscall*)(IBasePlayer*);
	using RestoreEntityToPredictedFrame_t = void(__stdcall*)(int, int);

	unknown_function_think sub_1019B010;
	PostThinkVPhysics_t PostThinkVPhysics;
	SimulatePlayerSimulatedEntities_t SimulatePlayerSimulatedEntities;

	CUserCmd* save_m_pCurrentCommand;
	int save_m_nPredictionRandomSeed;
	int save_m_flInnacuracy;
	int save_m_flRecoilIndex;
	
	

public:
	RestoreEntityToPredictedFrame_t RestoreEntityToPredictedFrame;
	void* ReinitPredictables;
	int m_hConstraintEntity;
	c_engine_prediction()
	{
		ZeroMemory(this, sizeof c_engine_prediction);
	}

	void initalize()
	{
		sub_1019B010 = (unknown_function_think)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B C6 C1 E8")));

		PostThinkVPhysics = (PostThinkVPhysics_t)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB ? ? ? ? ? 0F 84")));

		SimulatePlayerSimulatedEntities = (SimulatePlayerSimulatedEntities_t)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74"))); //fixed

		m_nPredictionRandomSeed = *reinterpret_cast <int**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("A3 ? ? ? ? 66 0F 6E 86")) + 0x1);	

		m_pPredictionPlayer = *reinterpret_cast <int**> (csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("89 35 ? ? ? ? F3 0F 10 48")) + 0x2);
			
		RestoreEntityToPredictedFrame = (RestoreEntityToPredictedFrame_t)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 8B 4D ? 56 E8 ? ? ? ? 8B 75")));

		ReinitPredictables = csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("A1 ? ? ? ? B9 ? ? ? ? 53 56 FF 50 18"));
	}

	void update() {
		auto delta_tick = csgo->client_state->iDeltaTick;
		auto start = csgo->client_state->nLastCommandAck;
		auto stop = csgo->client_state->nLastOutgoingCommand + csgo->client_state->iChokedCommands;
		interfaces.prediction->Update(delta_tick, delta_tick > 0, start, stop);
	}

	void post_think(IBasePlayer* player)
	{
		getvfunc<void(__thiscall*)(void*)>(interfaces.model_cache, 33)(interfaces.model_cache);

		if (player->isAlive()) {
			getvfunc< void(__thiscall*)(void*) >(player, 340)(player); //UpdateCollisionBounds
			if (player->GetFlags() & FL_ONGROUND)
				player->GetFallVelocity() = 0.f;
			if (player->GetSequence() == -1)
				getvfunc< void(__thiscall*)(void*, int) >(player, 219)(player, 0); //SetSequence
			getvfunc< void(__thiscall*)(void*) >(player, 220)(player); //StudioFrameAdvance 
			PostThinkVPhysics(player);
		};

		SimulatePlayerSimulatedEntities(player);

		getvfunc<void(__thiscall*)(void*)>(interfaces.model_cache, 34)(interfaces.model_cache);
	}

	void start(IBasePlayer* player, CUserCmd* m_cmd)
	{
		if (!interfaces.engine->IsInGame() || !player)
			return;

		

		//	CPrediction::StartCommand
		{
			save_m_nPredictionRandomSeed = *m_nPredictionRandomSeed;

			*m_nPredictionRandomSeed = m_cmd->random_seed;
			*m_pPredictionPlayer = reinterpret_cast <int> (player);

			save_m_pCurrentCommand = *reinterpret_cast<CUserCmd**>(uintptr_t(player) + m_hConstraintEntity - 12);
			*reinterpret_cast<CUserCmd**>(uintptr_t(player) + m_hConstraintEntity - 12) = m_cmd; //might be player + 0x3348
			*reinterpret_cast<CUserCmd*>(uintptr_t(player) + 0x3288) = *m_cmd;
		}

		auto weapon = player->GetWeapon();

		if (weapon && !weapon->IsNade() && !weapon->IsKnife())
		{
			save_m_flInnacuracy = weapon->GetInaccuracy();
			save_m_flRecoilIndex = weapon->GetRecoilIndex();
		}

		//	backup player variables
		m_store.m_fFlags = player->GetFlags();
		m_store.m_vecVelocity = player->GetVelocity();
		m_store.m_iMoveType = player->GetMoveType();
		m_store.m_vecOrigin = player->GetOrigin();

		//	backup globals
		m_store.m_flCurtime = interfaces.global_vars->curtime;
		m_store.m_flFrametime = interfaces.global_vars->frametime;
		m_store.m_iTickCount = interfaces.global_vars->tickcount;


		//	backup
		const auto old_tickbase = player->GetTickBase();
		old_in_prediction = interfaces.prediction->InPrediction;
		old_first_prediction = interfaces.prediction->IsFirstTimePredicted;

		//	set globals correctly
		interfaces.global_vars->curtime = player->GetTickBase() * interfaces.global_vars->interval_per_tick;
		predicted_curtime = interfaces.global_vars->curtime;

		interfaces.global_vars->frametime = interfaces.prediction->EnginePaused ? 0 : interfaces.global_vars->interval_per_tick;
		interfaces.global_vars->tickcount = player->GetTickBase();


		//	setup prediction
		interfaces.prediction->IsFirstTimePredicted = false;
		interfaces.prediction->InPrediction = true;

		if (m_cmd->impulse)
			*reinterpret_cast<uint32_t*>((uintptr_t)player + 0x320C) = m_cmd->impulse; // 31fc -> 0x320C

		m_cmd->buttons |= *reinterpret_cast<int*>((uintptr_t)player + 0x3344);
		m_cmd->buttons &= ~(*reinterpret_cast<int*>((uintptr_t)player + 0x3340));

		// update button state
		const int buttons = m_cmd->buttons;
		int* player_buttons = player->GetButtons();
		const int buttons_changed = buttons ^ *player_buttons;

		// synchronize m_afButtonLast
		player->GetButtonLast() = *player_buttons;

		// synchronize m_nButtons
		*player->GetButtons() = buttons;

		// synchronize m_afButtonPressed
		player->GetButtonPressed() = buttons & buttons_changed;

		// synchronize m_afButtonReleased
		player->GetButtonReleased() = buttons_changed & ~buttons;


		interfaces.prediction->CheckMovingGround(player, interfaces.global_vars->frametime);

		interfaces.prediction->SetLocalViewAngles(m_cmd->viewangles);

		//	CPrediction::RunPreThink
		if (player->PhysicsRunThink(0))
		{
			getvfunc<void(__thiscall*)(void*)>(player, 318)(player); //prethink 317 -> 318
			/*player->PreThink();*/
		}

		//	CPrediction::RunThink
		{
			auto m_nNextThinkTick = *reinterpret_cast<int32_t*>((uintptr_t)player + 0xFC);
			if (m_nNextThinkTick != -1 &&
				m_nNextThinkTick > 0 &&
				m_nNextThinkTick <= player->GetTickBase())
			{
				*reinterpret_cast<int32_t*>((uintptr_t)player + 0xFC) = 01; //m_nNextThinkTick = 1

				sub_1019B010(player, 0);

				getvfunc<void(__thiscall*)(void*)>(player, 138)(player);
			}
		}


		//	set host
		interfaces.move_helper->SetHost(player);

		// start track prediction errors
		interfaces.game_movement->StartTrackPredictionErrors(player);


		//edgebug accuracy fix dumped from patokes cheat dll LOL:D
		/*
		if (vars.movement.edgebug.enabled)
		{
			if (vars.movement.edgebug.key.properstate())
			{
				if (csgo->EBcrouched)
					m_move_data.m_nButtons |= IN_DUCK;
				else
					m_move_data.m_nButtons &= ~IN_DUCK;

				
				m_move_data.m_flForwardMove = 0.f;
				m_move_data.m_flSideMove = 0.f;
				//m_move_data.m_nButtons &= ~(IN_FORWARD | IN_MOVELEFT | IN_MOVERIGHT | IN_BACK);

				if (!vars.misc.bunnyhop)
				{
					m_move_data.m_nOldButtons &= ~IN_JUMP;
					m_move_data.m_nButtons &= ~IN_JUMP;
				}
				
			}
		}
		*/


		//	setup input
		interfaces.prediction->SetupMove(player, m_cmd, interfaces.move_helper, &m_move_data);

		csgo->in_prediction = true;
		//	run movement
		interfaces.game_movement->ProcessMovement(player, &m_move_data);
		csgo->in_prediction = false;

		//	finish prediction
		interfaces.prediction->FinishMove(player, m_cmd, &m_move_data);

		//	invoke impact functions
		interfaces.move_helper->ProcessImpacts();

		//	CPrediction::RunPostThink
		{
			post_think(player);
		}

		player->GetTickBasePtr() = old_tickbase;
		player->GetAbsVelocity() = m_move_data.m_vecVelocity;
		player->SetAbsOrigin(player->GetNetworkOrigin());
		player->GetEFlags() = (player->GetEFlags() & ~(EFL_DIRTY_ABSTRANSFORM | EFL_DIRTY_ABSVELOCITY));

		interfaces.game_movement->FinishTrackPredictionErrors(player);
		interfaces.move_helper->SetHost(nullptr);

		if (weapon && !weapon->IsNade() && !weapon->IsKnife())
			weapon->UpdateAccuracyPenalty();
	}

	void end(IBasePlayer* player)
	{
		interfaces.prediction->IsFirstTimePredicted = old_first_prediction;
		interfaces.prediction->InPrediction = old_in_prediction;

		interfaces.global_vars->curtime = m_store.m_flCurtime;
		interfaces.global_vars->frametime = m_store.m_flFrametime;
		interfaces.global_vars->tickcount = m_store.m_iTickCount;

		//	CPrediction::FinishCommand
		{
			*reinterpret_cast<CUserCmd**>(uintptr_t(player) + m_hConstraintEntity - 12) = save_m_pCurrentCommand;
			*m_nPredictionRandomSeed = save_m_nPredictionRandomSeed;
			*m_pPredictionPlayer = 0;
		}

		interfaces.game_movement->Reset();

		/*if (!csgo::prediction->m_engine_paused && csgo::vars->frametime > 0)
			player->m_nTickBase()++;*/

		auto weapon = player->GetWeapon();
		if (weapon && !weapon->IsNade() && !weapon->IsKnife())
		{
			weapon->GetRecoilIndex() = save_m_flRecoilIndex;
			weapon->GetAccuracyPenalty() = save_m_flInnacuracy;
		}

		
	}

	void saveprediction()
	{
		if (!csgo->local || !csgo->local->isAlive())
			return;
		if (!csgo->storedData)
		{
			const auto allocSize = csgo->local->getintermediatedatasize();
			csgo->storedData = new uint8_t[allocSize];
		}

		if (!csgo->storedData)
			return;

		PredictionCopy helper(PC_EVERYTHING, (uint8_t*)csgo->storedData, true, (uint8_t*)csgo->local, false, PredictionCopy::TRANSFERDATA_COPYONLY, NULL);
		helper.TransferData(crypt_str("engine_prediction::saveprediction"), csgo->local->GetIndex(), csgo->local->GetPredDescMap());
	}

	void restoreprediction(int slot, int frame)
	{
		csgo->blocktier0_msg = true;
		RestoreEntityToPredictedFrame(slot, frame);
		csgo->blocktier0_msg = false;
	}

};
extern c_engine_prediction* engine_prediction;

//class CEnginePrediction : public Singleton<CEnginePrediction>
//{
//public:
//	void Start(CUserCmd* cmd, IBasePlayer* local);
//	void Finish(IBasePlayer* local);
//	viewmodel_data_t viewmodel_data;
//private:
//	//CMoveData data;
//	struct {
//		Vector velocity, origin;
//		float curtime, frametime;
//		int tickcount, tickbase;
//		bool in_pred, was_pred;
//		int* prediction_random_seed = nullptr;
//		int* prediction_player = nullptr;
//	} old_vars;
//};

class CNetData {
private:
	class StoredData_t {
	public:
		int    m_tickbase;
		Vector  m_punch;
		Vector  m_punch_vel;
		Vector  m_view_punch;
		Vector m_view_offset;
		Vector m_origin;
		Vector m_velocity;
		float  m_velocity_modifier;
		float m_duck_amount;
		float m_thirdperson_recoil;
		float m_duck_speed;
		float m_fall_velocity;
	public:
		StoredData_t() {
			m_tickbase = -1;
			m_punch.Zero();
			m_punch_vel.Zero();
			m_view_punch.Zero();
			m_view_offset.Zero();
			m_velocity.Zero();
			m_origin.Zero();
			m_velocity_modifier = -1.f;
			m_duck_amount = -1.f;
			m_thirdperson_recoil = -1.f;
			m_duck_speed = -1.f;
			m_fall_velocity = -1.f;
		}
	};

	std::array< StoredData_t, 150 > m_data;

public:
	struct ViewModelData_t {
		CBaseHandle m_hWeapon = 0;

		int m_nViewModelIndex = 0;
		int m_nAnimationParity = 0;
		int m_nSequence = 0;
		float networkedCycle = 0.0f;
		float animationTime = 0.0f;
	}viewModelData;

	void RecordViewmodelValues();
	void ApplyViewmodelValues();

	void Store();
	void Apply();
	void Reset();
};

extern CNetData* g_NetData;