#pragma once
#include "Hooks.h"

void __fastcall Hooked_BeginFrame(void* thisptr)
{
	//static auto BeginFrame = g_pStudioRenderHook->GetOriginal< BeginFrameFn > (g_HookIndices[fnva1(hs::Hooked_BeginFrame.s().c_str())]);



	features->BulletTracer->Draw();
	g_pStudioRenderHook->callOriginal<void, 9>();
}