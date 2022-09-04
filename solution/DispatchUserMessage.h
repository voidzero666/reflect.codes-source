#pragma once
#include "Hooks.h"

bool __stdcall Hooked_DispatchUserMessage(UserMessageType type, int passthroughFlags, int size, const void* data)
{
	
	if (type == UserMessageType::CS_UM_VoteStart)
	{
		Misc::Get().votestart(data, size);
	}

	if (type == UserMessageType::CS_UM_TextMsg || type == UserMessageType::CS_UM_HudMsg || type == UserMessageType::CS_UM_SayText)
	{
		if (vars.misc.ublockOrigin && !csgo->game_rules->IsValveDS())
			return true;
	}

	return g_pClientHook->callOriginal<bool, 38>(type, passthroughFlags, size, data);
}