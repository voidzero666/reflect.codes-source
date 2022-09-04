#pragma once
#include "Hooks.h"


static uintptr_t __stdcall Hooked_FindIndex(const char* text)
{

	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && csgo->game_rules->IsValveDS())
	{
		if (strstr(text, crypt_str("PermanentCooldown")) || strstr(text, crypt_str("AbandonedCooldown")))
		{
			csgo->cooldowncounter++;
			if (vars.visuals.teamdmglist && vars.visuals.cooldownsay)
			{
				std::string mesag = std::string(crypt_str("say \"") + std::string(vars.visuals.cooldownsaytext) );
				if (vars.visuals.addcounter)
					mesag += crypt_str(" ") + std::to_string(csgo->cooldowncounter);
				mesag += crypt_str("\"");
				interfaces.engine->ClientCmd_Unrestricted(mesag.c_str()); // Like (:thumbsup:)
			}
		}
	}
	
	return g_pLocalizeHook->callOriginal<uintptr_t, 11>(text);
}
