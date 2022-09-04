#pragma once
#include "Hooks.h"
#include <intrin.h>

bool __stdcall Hooked_IsConnected()
{
	if (vars.misc.unlockinventoryaccess && _ReturnAddress() == csgo->dwUnlockInventory)
		return false;

	return g_pEngineHook->callOriginal<bool, 27>();
}

bool __fastcall Hooked_IsPaused(void* ecx, void* edx) {
	//static auto IsPaused = g_pEngineHook->GetOriginal< IsPausedFn >(g_HookIndices[fnva1(hs::Hooked_IsPaused.s().c_str())]);
	//g_pEngineHook->callOriginal<bool, 90>(ecx);



	static DWORD* return_to_extrapolation = (DWORD*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("FF D0 A1 ?? ?? ?? ?? B9 ?? ?? ?? ?? D9 1D ?? ?? ?? ?? FF 50 34 85 C0 74 22 8B 0D ?? ?? ?? ??")) + 0x29);

	if (_ReturnAddress() == (void*)return_to_extrapolation)
		return true;

	return g_pEngineHook->callOriginal<bool, 90>();
}

int32_t __fastcall Hooked_IsBoxVisible(IEngineClient* engine_client, uint32_t, Vector& min, Vector& max)
{
	//static auto BoxVisible = g_pEngineHook->GetOriginal< BoxVisibleFn >(g_HookIndices[fnva1(hs::Hooked_IsBoxVisible.s().c_str())]);

	//g_pEngineHook->callOriginal<int32_t, 32>(engine_client, min, max);

	const auto ret = hs::ret_engine.s().c_str();

	if (!memcmp(_ReturnAddress(), ret, 10))
		return 1;

	return g_pEngineHook->callOriginal<int32_t, 32>(std::cref(min), std::cref(max));
}

bool __fastcall Hooked_IsHLTV(IEngineClient* inst_IEngineClient)
{
	//static auto IsHLTV = g_pEngineHook->GetOriginal< IsHLTVFn >(g_HookIndices[fnva1(hs::Hooked_IsHLTV.s().c_str())]);

	//g_pEngineHook->callOriginal<bool, 93>(IEngineClient);
	/*
	static const auto return_to_accumulate_layers = (csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::accum_layers.s().c_str()));

	static const auto return_to_setup_vel = (csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::setup_vel.s().c_str()));
		*/

	if (_ReturnAddress() == (uint32_t*)(csgo->return_to_accumulate_layers)
		|| _ReturnAddress() == (uint32_t*)(csgo->return_to_setup_vel))
		return true;

	return g_pEngineHook->callOriginal<bool, 93>();
}

void __fastcall Hooked_ClipRayColliedable(void* ecx, void* edx, const Ray_t& ray, uint32_t fMask, ICollideable* pCollide, CGameTrace* pTrace) {
	//static auto ClipRayColliedable = g_pEngineTraceHook->GetOriginal< ClipRayCollideableFn >(g_HookIndices[fnva1(hs::Hooked_ClipRayColliedable.s().c_str())]);

	//g_pEngineTraceHook->callOriginal<void, 4>(ecx, ray, fMask, pCollide, pTrace);

	if (pCollide) {
		const auto old_max = pCollide->OBBMaxs().z;

		pCollide->OBBMaxs().z += 5;
		g_pEngineTraceHook->callOriginal<void, 4>(ecx, ray, fMask, pCollide, pTrace);
		pCollide->OBBMaxs().z = old_max;
	}
	else
		return g_pEngineTraceHook->callOriginal<void, 4>(ecx, ray, fMask, pCollide, pTrace);
}

void __fastcall Hooked_ModifyEyePos(CCSGOPlayerAnimState* state, void* edx, const Vector& vec) {

	static auto original = dtModifyEyePos.getOriginal<void>(std::cref(vec));

	if (csgo->DoUnload)
		return original(state, std::cref(vec));

	IBasePlayer* ent = (IBasePlayer*)state->m_pBaseEntity;
	if (!ent)
		return original(state, std::cref(vec));

	if (csgo->local && ent != csgo->local)
		original(state, std::cref(vec));
	else
		return;
}

typedef void(__cdecl* MsgFn)(char const* pMsg, va_list);

const int hasher = std::hash<std::string>{}(crypt_str("%d:  Reinitialized %i predictable entities"));

void __cdecl Hooked_Tier0_Msg(const char* msg, ...)
{

	if (csgo->blocktier0_msg)
		return;

	char buf[989];   // max is 989: https://developer.valvesoftware.com/wiki/Developer_Console_Control
	va_list vlist;
	va_start(vlist, msg);
	_vsnprintf(buf, sizeof(buf), msg, vlist);
	va_end(vlist);

	MsgFn pOrgFn = (MsgFn)dtTier0_Msg.original;
	pOrgFn(buf, nullptr);
}

typedef int(__stdcall* ReinFn)();

int __stdcall Hooked_ReinitPredictables()
{
	ReinFn pOrgFn = (ReinFn)dtReinitPredictables.original;
	csgo->blocktier0_msg = true;
	return pOrgFn();
	csgo->blocktier0_msg = false;
}