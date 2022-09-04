#pragma once
#include "Hooks.h"

static void __fastcall hkPerfScreenOverlay(void* thisPointer, void* edx, int x, int y, int width, int height) noexcept
{
	static auto original = dtPerfScreenOverlay.getOriginal<void>(x, y, width, height);

	if (!vars.misc.ublockOrigin || csgo->game_rules->IsValveDS())
		return original(thisPointer, x, y, width, height);
}