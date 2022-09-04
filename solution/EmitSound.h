#pragma once
#include "Hooks.h"


static float accepttime = 0.f;
static bool toaccept = false;
int __stdcall EmitSound_Hooked(void* filter, int entIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const Vector& origin, const Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams)
{
	
	if (vars.misc.autoaccept && !strcmp(soundEntry, crypt_str("UIPanorama.popup_accept_match_beep")) && !toaccept)
	{
		/*
		UtlMap<short, PanoramaEventRegistration>* panmap = (UtlMap<short, PanoramaEventRegistration>*)csgo->registeredPanoramaEvents;
		int idx = panmap->find(csgo->makePanoramaSymbol(crypt_str("MatchAssistedAccept")));
		if (idx != -1) {
			void* eventPtr = panmap->memory[idx].value.makeEvent(nullptr);
			if (eventPtr)
			{
				interfaces.panorama_ui_engine->accessUIEngine()->dispatchEvent(eventPtr);
			}
				
		}
		*/
		accepttime = interfaces.global_vars->realtime + vars.misc.autoacceptdelay;
		toaccept = true;
		
	}


	if ((accepttime < interfaces.global_vars->realtime) && toaccept)
	{
		static auto acceptFn = reinterpret_cast<bool(__stdcall*)(const char*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(crypt_str("client.dll")), crypt_str("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12")));

		if (acceptFn)
		{
			acceptFn("");
			auto window = FindWindowW(crypt_strw(L"Valve001"), NULL);
			FLASHWINFO flash{ sizeof(FLASHWINFO), window, FLASHW_TRAY | FLASHW_TIMERNOFG, 0, 0 };
			FlashWindowEx(&flash);
			ShowWindow(window, SW_RESTORE);
			toaccept = false;
		}
		
	}
	

	if (!csgo->in_prediction)
	{
		//H::EmitSound = g_pEngineRevert "C++"SoundHook->GetOriginal<EmitSoundFn>(g_HookIndices[fnva1(hs::Hooked_EmitSound.s().c_str())]);
		return g_pEngineSoundHook->callOriginal<int, 5>(filter, entIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, std::cref(origin), std::cref(direction), utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
	}
}