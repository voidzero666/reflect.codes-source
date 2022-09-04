#pragma once
#include "Hooks.h"

static char __fastcall Hooked_NewFunctionClientBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
    return 1;
}

static char __fastcall Hooked_NewFunctionEngineBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
    return 1;
}

static char __fastcall Hooked_NewFunctionStudioRenderBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
    return 1;
}

static char __fastcall Hooked_NewFunctionMaterialSystemBypass(void* thisPointer, void* edx, const char* moduleName) noexcept
{
    return 1;
}