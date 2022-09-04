#include "Hooks.h"
#include "Glow.h"

void __fastcall Hooked_SceneEnd(void* thisptr, void* edx)
{
	//static auto SceneEnd = g_pRenderViewHook->GetOriginal<SceneEndFn>(g_HookIndices[fnva1(hs::Hooked_SceneEnd.s().c_str())]);

	g_pRenderViewHook->callOriginal<void, 9>();


/*if (csgo->local && csgo->animstate &&  csgo->local->isAlive())
	{
		static IMaterial* Regular = interfaces.material_system->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		static IMaterial* Flat = interfaces.material_system->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		static IMaterial* Metallic = interfaces.material_system->FindMaterial("regular_ref", TEXTURE_GROUP_MODEL);
		static IMaterial* Glow = interfaces.material_system->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_MODEL);
		Regular->IncrementReferenceCount();
		Flat->IncrementReferenceCount();
		Metallic->IncrementReferenceCount();
		Glow->IncrementReferenceCount();

		if (vars.visuals.misc_chams[desync].enable)
		{
			static IMaterial* OverridedMaterial = nullptr;

			if (OverridedMaterial != nullptr)
				OverridedMaterial->IncrementReferenceCount();

			switch (vars.visuals.misc_chams[desync].material)
			{
			case 0: OverridedMaterial = Regular; break;
			case 1: OverridedMaterial = Flat; break;
			case 2: OverridedMaterial = Metallic; break;
			}

			if (vars.visuals.misc_chams[desync].material == 2)
			{
				auto phong = OverridedMaterial->FindVar("$phongexponent", nullptr);
				phong->SetVecValue(vars.visuals.misc_chams[desync].phong_exponent);
				auto phong2 = OverridedMaterial->FindVar("$phongboost", nullptr);
				phong2->SetVecValue(vars.visuals.misc_chams[desync].phong_boost);

				auto rim = OverridedMaterial->FindVar("$rimlightexponent", nullptr);
				rim->SetVecValue(vars.visuals.misc_chams[desync].rim);
				auto rim2 = OverridedMaterial->FindVar("$rimlightboost", nullptr);
				rim2->SetVecValue(vars.visuals.misc_chams[desync].rim);
				auto var = OverridedMaterial->FindVar("$envmaptint", nullptr);
				var->SetVecValue(vars.visuals.misc_chams[desync].metallic_clr[0], vars.visuals.misc_chams[desync].metallic_clr[1], vars.visuals.misc_chams[desync].metallic_clr[2]);

				auto var2 = OverridedMaterial->FindVar("$phongtint", nullptr);
				var2->SetVecValue(vars.visuals.misc_chams[desync].metallic_clr2[0], vars.visuals.misc_chams[desync].metallic_clr2[1], vars.visuals.misc_chams[desync].metallic_clr2[2]);
			}

			csgo->local->get_last_bone_setup_time() = -FLT_MAX; // invalidate bone cache for proper matrix
			csgo->local->get_most_recent_model_bone_counter() = 0;// invalidate bone cache for proper matrix
			csgo->local->SetupBones(nullptr, -1, 0x7FF00, interfaces.global_vars->curtime);
			csgo->local->SetAbsAngles(Vector(0, csgo->animstate->m_flGoalFeetYaw, 0));
			if (vars.visuals.misc_chams[desync].enable)
			{
				OverridedMaterial->color_tModulate(vars.visuals.misc_chams[desync].clr[0], vars.visuals.misc_chams[desync].clr[1], vars.visuals.misc_chams[desync].clr[2]);
				OverridedMaterial->AlphaModulate(vars.visuals.misc_chams[desync].clr[3]);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				float clr[3] = {
					vars.visuals.misc_chams[desync].clr[0] * vars.visuals.misc_chams[desync].chams_brightness,
					vars.visuals.misc_chams[desync].clr[1] * vars.visuals.misc_chams[desync].chams_brightness,
					vars.visuals.misc_chams[desync].clr[2] * vars.visuals.misc_chams[desync].chams_brightness
				};
				float backup_clr[3];
				interfaces.render_view->Getcolor_tModulation(backup_clr);
				interfaces.render_view->Setcolor_tModulation(clr);

				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				csgo->local->DrawModel(0x1, 255);

				auto pVar = Glow->FindVar("$envmaptint", nullptr);
				pVar->SetVecValue(vars.visuals.misc_chams[desync].glow_clr[0], vars.visuals.misc_chams[desync].glow_clr[1], vars.visuals.misc_chams[desync].glow_clr[2]);

				Glow->AlphaModulate(vars.visuals.misc_chams[desync].glow_clr[3]);
				Glow->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

				interfaces.models.model_render->ForcedMaterialOverride(Glow);
				csgo->local->DrawModel(0x1, 255);
				interfaces.render_view->Setcolor_tModulation(backup_clr);
			}

			interfaces.models.model_render->ForcedMaterialOverride(nullptr);
			csgo->local->SetAbsAngles(csgo->local->GetAbsAngles());
		}
	}*/
}