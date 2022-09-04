#pragma once
#include <intrin.h>
#include "EnginePrediction.h"
/*
void __fastcall Hooked_RunCommand(void* ecx, void* edx, IBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
{
	static auto RunCommand = g_pPredictHook->GetOriginal< RunCommandFn >(g_HookIndices[fnva1(hs::Hooked_RunCommand.s().c_str())]);

	if (csgo->DoUnload || player == nullptr || csgo->local == nullptr || !csgo->local->isAlive())
		return RunCommand(ecx, player, ucmd, moveHelper);

	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
		return RunCommand(ecx, player, ucmd, moveHelper);

	if (ucmd->tick_count >= (interfaces.global_vars->tickcount + int(1 / TICK_INTERVAL) + 8)) {
		ucmd->hasbeenpredicted = true;
		player->SetAbsOrigin(player->GetOrigin());
		if (interfaces.global_vars->frametime > 0.0f && !interfaces.prediction->EnginePaused)
			++player->GetTickBasePtr();
		return;
	}



	// backup variables.
	int backup_tickbase = player->GetTickBase();
	float backup_curtime = interfaces.global_vars->curtime;

	// fix tickbase when shifting. csgo->local->GetTickBase()

	if (ucmd->command_number == csgo->m_shift_command_number) {
		player->GetTickBasePtr() = (csgo->local->GetTickBase() - csgo->m_current_shift + 1);
		//++player->GetTickBasePtr();

		interfaces.global_vars->curtime = TICKS_TO_TIME(player->GetTickBasePtr());
	}


	float m_flVelModBackup = player->GetVelocityModifier();
	if (csgo->in_cm && ucmd->command_number == csgo->client_state->m_command_ack + 1)
		player->GetVelocityModifier() = csgo->velocity_modifier;

	RunCommand(ecx, player, ucmd, moveHelper);


	// restore tickbase and curtime.
	if (ucmd->command_number == csgo->m_shift_command_number) {
		player->GetTickBasePtr() = backup_tickbase;

		interfaces.global_vars->curtime = backup_curtime;
	}


	g_NetData->Store();

	if (!csgo->in_cm)
		player->GetVelocityModifier() = m_flVelModBackup;
}
*/

void __fastcall Hooked_RunCommand(void* ecx, void* edx, IBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
{
	//static auto RunCommand = g_pPredictHook->GetOriginal< RunCommandFn >(g_HookIndices[fnva1(hs::Hooked_RunCommand.s().c_str())]);


	if (csgo->DoUnload || player == nullptr || csgo->local == nullptr || !csgo->local->isAlive())
		return g_pPredictHook->callOriginal<void, 19>(player, ucmd, moveHelper);

	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
		return g_pPredictHook->callOriginal<void, 19>(player, ucmd, moveHelper);

	if (ucmd->tick_count > interfaces.global_vars->tickcount * 2) //-V807
	{
		ucmd->hasbeenpredicted = true;
		player->SetAbsOrigin(player->GetOrigin());
		if (interfaces.global_vars->frametime > 0.0f && !interfaces.prediction->EnginePaused)
			++player->GetTickBasePtr();
		return;
	}

	float m_flVelModBackup = player->GetVelocityModifier();
	if (csgo->in_cm && ucmd->command_number == csgo->client_state->m_command_ack + 1)
		player->GetVelocityModifier() = csgo->velocity_modifier;

	g_pPredictHook->callOriginal<void, 19>(player, ucmd, moveHelper);

	g_NetData->Store();

	if (!csgo->in_cm)
		player->GetVelocityModifier() = m_flVelModBackup;
}

class CommandContext
{
public:
	bool needsProcessing;
	CUserCmd cmd;
	int commandNumber;
};

static void __fastcall physicsSimulateHook(void* thisPointer, void* edx) noexcept
{
	static auto original = physSim.getOriginal<void>();

	const auto entity = reinterpret_cast<IBasePlayer*>(thisPointer);
	if (!csgo->local || !csgo->local->isAlive() || entity != csgo->local)
		return original(thisPointer);

	const int simulationTick = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(thisPointer) + 0x2AC);
	if (simulationTick == interfaces.global_vars->tickcount)
		return;

	CommandContext* commandContext = reinterpret_cast<CommandContext*>(reinterpret_cast<uintptr_t>(csgo->local) + 0x350C);

	if (!commandContext || !commandContext->needsProcessing)
		return;

	int tickBase = csgo->local->GetTickBase();

	if (commandContext->cmd.command_number == csgo->shift_cmd)
		tickBase = tickBase - csgo->m_current_shift;
	else if (commandContext->cmd.command_number == csgo->shift_cmd + 1)
		tickBase = tickBase + csgo->m_current_shift;

	csgo->local->GetTickBasePtr() = tickBase;

	original(thisPointer);

	// save netvar data
	g_NetData->Store();
}


bool __fastcall Hooked_InPrediction(CPrediction* prediction)
{
	//static auto InPrediction = g_pPredictHook->GetOriginal< InPredictionFn >(g_HookIndices[fnva1(hs::Hooked_InPrediction.s().c_str())]);

	if (csgo->DoUnload)
		return g_pPredictHook->callOriginal<bool, 14>();

	static const auto return_to_maintain_sequence_transitions = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::seq_transition.s().c_str());

	static const auto return_to_setup_bones = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::setup_bones_timing.s().c_str());

	if (_ReturnAddress() == (uint32_t*)return_to_maintain_sequence_transitions)
		return false;

	if (_ReturnAddress() == (uint32_t*)return_to_setup_bones)
		return false;

	return g_pPredictHook->callOriginal<bool, 14>();
}

void __fastcall Hooked_ProcessMovement(void* ecx, void* edx, IBasePlayer* ent, CMoveData* move_data) {
	//static auto ProcessMovement = g_pMovementHook->GetOriginal< void(__thiscall*)(void*, IBasePlayer*, CMoveData*) >(g_HookIndices[fnva1(hs::Hooked_ProcessMovement.s().c_str())]);


	move_data->m_bGameCodeMovedPlayer = false; // fix shit when you jump and movement are fucked up (credits to l3d451r7)
	g_pMovementHook->callOriginal<void, 1>(ent, move_data);
}

void __fastcall Hooked_SetupMove(void* ecx, void* edx, IBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper, void* pMoveData)
{
	//static auto SetupMove = g_pPredictHook->GetOriginal< void(__thiscall*)(void*, IBasePlayer*, CUserCmd*, IMoveHelper*, void*) >(20);
	//SetupMove(ecx, player, ucmd, moveHelper, pMoveData);
	g_pPredictHook->callOriginal<void, 20>(player, ucmd, moveHelper, pMoveData);
}