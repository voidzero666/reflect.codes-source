#pragma once
#include "Hooks.h"
#include <intrin.h>

// disable model occlusion
#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -MAX_COORD_FLOAT )

void ClearSmokeEffect()
{
	static auto smokeCount = *reinterpret_cast<uint32_t**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::smoke_effect.s().c_str()) + 1);
	*(int*)smokeCount = 0;
}

//typedef void(__thiscall* GetColorModulationFn)(void*, float*, float*, float*);
void __fastcall Hooked_GetColorModulation(void* ecx, void* edx, float* r, float* g, float* b) {

	static auto original = dtColorModulation.getOriginal<void>(r, g, b);

	original(ecx, r, g, b);

	static auto load_named_sky = reinterpret_cast<void(__fastcall*)(const char*)>(
		csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
			hs::load_named_sky.s().c_str()));

	static bool reset[2] = { false, false, };

	static auto sv_skyname = interfaces.cvars->FindVar(hs::sv_skyname.s().c_str());
	sv_skyname->m_fnChangeCallbacks.m_Size = 0;

	static auto r_3dsky = interfaces.cvars->FindVar(hs::r_3dsky.s().c_str());
	r_3dsky->m_fnChangeCallbacks.m_Size = 0;
	r_3dsky->SetValue(0);

	static std::string sky_dust = hs::sky_dust.s().c_str();
	static std::string sky_csgo_night02 = hs::sky_csgo_night02.s().c_str();
	if (!vars.visuals.nightmode || csgo->DoUnload) {
		if (reset[0]) {
			load_named_sky(sky_dust.c_str());
			reset[0] = false;
		}
		reset[1] = true;
		return original(ecx, r, g, b);
	}
	else reset[0] = true;

	const auto material = reinterpret_cast<IMaterial*>(ecx);

	if (!material || material->IsErrorMaterial())
		return original(ecx, r, g, b);

	if (reset[1] || csgo->mapChanged) {
		load_named_sky(sky_csgo_night02.c_str());
		reset[1] = false;
	}

	auto name = material->GetTextureGroupName();

	if (strstr(name, crypt_str("World"))) {
		if (vars.visuals.customize_color) {
			*r *= vars.visuals.nightmode_color[0] / 255.f;
			*g *= vars.visuals.nightmode_color[1] / 255.f;
			*b *= vars.visuals.nightmode_color[2] / 255.f;
		}
		else {
			*r *= vars.visuals.nightmode_amount * 0.01f;
			*g *= vars.visuals.nightmode_amount * 0.01f;
			*b *= vars.visuals.nightmode_amount * 0.01f;
		}
	}
	else if (strstr(name, crypt_str("StaticProp"))) {
		if (vars.visuals.customize_color) {
			*r *= vars.visuals.nightmode_prop_color[0] / 255.f;
			*g *= vars.visuals.nightmode_prop_color[1] / 255.f;
			*b *= vars.visuals.nightmode_prop_color[2] / 255.f;
		}
		else {
			*r *= vars.visuals.nightmode_amount * 0.01f;
			*g *= vars.visuals.nightmode_amount * 0.01f;
			*b *= vars.visuals.nightmode_amount * 0.01f;
		}
	}
	else if (strstr(name, crypt_str("SkyBox"))) {
		if (vars.visuals.customize_color) {
			*r *= vars.visuals.nightmode_skybox_color[0] / 255.f;
			*g *= vars.visuals.nightmode_skybox_color[1] / 255.f;
			*b *= vars.visuals.nightmode_skybox_color[2] / 255.f;
		}
	}
}

bool __fastcall Hooked_IsUsingStaticPropDebugModes(void* ecx, void* edx)
{
	return vars.visuals.nightmode && vars.visuals.customize_color ? true : vars.visuals.nightmode_amount == 100;
}


void __stdcall  Hooked_DrawModelExecute(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix* pCustomBoneToWorld)
{
	//static auto DrawModelExecute = g_pModelRenderHook->GetOriginal< DrawModelExecuteFn >(g_HookIndices[fnva1(hs::Hooked_DrawModelExecute.s().c_str())]);


	if (csgo->DoUnload || interfaces.studio_render->isForcedMaterialOverride())
		return g_pModelRenderHook->callOriginal<void, 21>(ctx, std::cref(state), std::cref(info), pCustomBoneToWorld);

	static bool fl = false;
	if (!fl)
	{
		std::ofstream(crypt_str("csgo\\materials\\regular_glow.vmt")) << crypt_str(R"#("VertexLitGeneric" {
			"$additive" "1"
			"$envmap" "models/effects/cube_white"
			"$envmaptint" "[1 1 1]"
			"$envmapfresnel" "1"
			"$envmapfresnelminmaxexp" "[0 1 2]"
			"$alpha" "1"
		})#");

		std::ofstream(crypt_str("csgo\\materials\\regular_ref.vmt")) << crypt_str(R"#("VertexLitGeneric" {
			"$basetexture"				    "vgui/white"
			"$envmap"						"env_cubemap"
			"$envmaptint"                   "[.10 .10 .10]"
			"$pearlescent"					"0"
			"$phong"						"1"
			"$phongexponent"				"10"
			"$phongboost"					"1.0"
			"$rimlight"					    "1"
			"$rimlightexponent"		        "1"
			"$rimlightboost"		        "1"
			"$model"						"1"
			"$nocull"						"0"
			"$halflambert"				    "1"
			"$lightwarptexture"             "metalic"
		})#");
		fl = true;
	}

	if (interfaces.engine->IsInGame() && csgo->local && info.pModel)
	{
		const auto mN = info.pModel->name;

		static std::string contats_shadow = crypt_str("player/contactshadow");
		static std::string prop = crypt_str("props");

		if ((vars.visuals.remove[8]) && strstr(mN, contats_shadow.c_str()) != nullptr)
			return;
		static Chams chams;

		if (!chams.render(ctx, (void*)&state, info, pCustomBoneToWorld))
			g_pModelRenderHook->callOriginal<void, 21>(ctx, std::cref(state), std::cref(info), pCustomBoneToWorld);

		if (strstr(mN, prop.c_str()) != nullptr)
			interfaces.render_view->SetBlend(vars.visuals.nightmode_prop_color.get_alpha() / 255.f);

		static IMaterial* mat1 = interfaces.material_system->FindMaterial(crypt_str("particle/vistasmokev1/vistasmokev1_smokegrenade"), crypt_str("Other textures"));
		static IMaterial* mat2 = interfaces.material_system->FindMaterial(crypt_str("particle/vistasmokev1/vistasmokev1_emods"), crypt_str("Other textures"));
		static IMaterial* mat3 = interfaces.material_system->FindMaterial(crypt_str("particle/vistasmokev1/vistasmokev1_emods_impactdust"), crypt_str("Other textures"));
		static IMaterial* mat4 = interfaces.material_system->FindMaterial(crypt_str("particle/vistasmokev1/vistasmokev1_fire"), crypt_str("Other textures"));

		mat1->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, vars.visuals.remove[1]);
		mat2->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, vars.visuals.remove[1]);
		mat3->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, vars.visuals.remove[1]);
		mat4->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, vars.visuals.remove[1]);

		if (vars.visuals.remove[1])
			ClearSmokeEffect();
	}

	g_pModelRenderHook->callOriginal<void, 21>(ctx, std::cref(state), std::cref(info), pCustomBoneToWorld);
	interfaces.models.model_render->ForcedMaterialOverride(nullptr);
}

int __stdcall Hooked_ListLeavesInBox(Vector& mins, Vector& maxs, unsigned short* pList, int listMax) 
{
	//static auto ListLeavesInBox = g_pQueryHook->GetOriginal< ListLeavesInBoxFn >(g_HookIndices[fnva1(hs::Hooked_ListLeavesInBox.s().c_str())]);

	// occulusion getting updated on player movement/angle change,
	// in RecomputeRenderableLeaves ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L674 );
	// check for return in CClientLeafSystem::InsertIntoTree

	static auto list_leaves = (void*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("56 52 FF 50 18")) + 5);

	if (_ReturnAddress() != list_leaves)
		return g_pQueryHook->callOriginal<int, 6>(std::cref(mins), std::cref(maxs), pList, listMax);

	// get current renderable info from stack ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470 )
	auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() - sizeof(std::uintptr_t) + 0x18);
	if (!info || !info->m_pRenderable)
		return g_pQueryHook->callOriginal<int, 6>(std::cref(mins), std::cref(maxs), pList, listMax);

	// check if disabling occulusion for players ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1491 )
	auto base_entity = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity(); // idx 7 virtual func call mens
	if (!base_entity || !base_entity->IsPlayer())
		return g_pQueryHook->callOriginal<int, 6>(std::cref(mins), std::cref(maxs), pList, listMax);

	// fix render order, force translucent group ( https://www.unknowncheats.me/forum/2429206-post15.html )
	// AddRenderablesToRenderLists: https://i.imgur.com/hcg0NB5.png ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473 )
	info->m_Flags &= ~0x100;
	info->m_Flags2 |= 0xC0;

	// extend world space bounds to maximum ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707 )
	static const Vector map_min = Vector(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
	static const Vector map_max = Vector(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
	auto count = g_pQueryHook->callOriginal<int, 6>(std::cref(map_min), std::cref(map_max), pList, listMax);
	return count;
}