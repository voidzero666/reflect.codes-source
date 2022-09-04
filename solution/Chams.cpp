#include "Chams.h"
#include "Hooks.h"
#include "AnimationFix.h"
#include "Ragebot.h"
#include "KeyValues.h"


constexpr auto rainbowColor(float time, float speed) noexcept
{
	return std::make_tuple(std::sin(speed * time) * 0.5f + 0.5f,
		std::sin(speed * time + static_cast<float>(2 * 3.14159f / 3)) * 0.5f + 0.5f,
		std::sin(speed * time + static_cast<float>(4 * 3.14159f / 3)) * 0.5f + 0.5f);
}

void matrix_set_origin(Vector pos, matrix& m)
{
	m[0][3] = pos.x;
	m[1][3] = pos.y;
	m[2][3] = pos.z;
}

Vector matrix_get_origin(const matrix& src)
{
	return { src[0][3], src[1][3], src[2][3] };
}

bool GetBacktrackMaxtrix(IBasePlayer* player, matrix* out)
{
	if (vars.visuals.interpolated_bt)
	{
		auto New = g_Animfix->get_latest_animation(player);
		auto Old = g_Animfix->get_oldest_animation(player);

		if (!New
			|| !Old)
			return false;

		const auto& first_invalid = Old;
		const auto& last_valid = New;

		if ((first_invalid->origin - player->GetAbsOrigin()).Length2D() < 7.5f)
			return false;

		if (first_invalid->dormant)
			return false;

		if (last_valid->sim_time - first_invalid->sim_time > 0.5f)
			return false;

		const auto next = last_valid->origin;
		const auto curtime = interfaces.global_vars->curtime;

		auto delta = 1.f - (curtime - last_valid->interp_time) / (last_valid->sim_time - first_invalid->sim_time);
		if (delta < 0.f || delta > 1.f)
			last_valid->interp_time = curtime;

		delta = 1.f - (curtime - last_valid->interp_time) / (last_valid->sim_time - first_invalid->sim_time);

		const auto lerp = [](const Vector from, const Vector to, const float percent) {
			return to * percent + from * (1.f - percent);
		}(next, first_invalid->origin, std::clamp<float>(delta, 0.f, 1.f));

		matrix ret[128];
		memcpy(ret, first_invalid->bones, sizeof(matrix[128]));

		for (size_t i{}; i < 128; ++i)
		{
			const auto matrix_delta = matrix_get_origin(first_invalid->bones[i]) - first_invalid->origin;
			matrix_set_origin(matrix_delta + lerp, ret[i]);
		}

		memcpy(out, ret, sizeof(matrix[128]));
		return true;
	}
	else
	{
		const auto last = g_Animfix->get_oldest_animation(player);

		if (!last)
			return false;

		memcpy(out, last->bones, sizeof(matrix[128]));
		return true;
	}

}


Chams::Chams() noexcept
{
	normal = interfaces.material_system->CreateMaterial(crypt_str("normal"), KeyValues::fromString(crypt_str("VertexLitGeneric"), nullptr));
	flat = interfaces.material_system->CreateMaterial(crypt_str("flat"), KeyValues::fromString(crypt_str("UnlitGeneric"), nullptr));
	chrome = interfaces.material_system->CreateMaterial(crypt_str("chrome"), KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$envmap env_cubemap")));
	glow = interfaces.material_system->CreateMaterial(crypt_str("glow"), KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $alpha .8")));
	pearlescent = interfaces.material_system->CreateMaterial(crypt_str("pearlescent"), KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1")));
	metallic = interfaces.material_system->CreateMaterial(crypt_str("metallic"), KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$basetexture white $ignorez 0 $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1")));

	{
		const auto kv = KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$envmap editor/cube_vertigo $envmapcontrast 1 $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } }"));
		kv->setString(crypt_str("$envmaptint"), crypt_str("[.7 .7 .7]"));
		animated = interfaces.material_system->CreateMaterial(crypt_str("animated"), kv);
	}

	{
		const auto kv = KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$baseTexture models/player/ct_fbi/ct_fbi_glass $envmap env_cubemap"));
		kv->setString(crypt_str("$envmaptint"), crypt_str("[.4 .6 .7]"));
		platinum = interfaces.material_system->CreateMaterial(crypt_str("platinum"), kv);
	}

	{
		const auto kv = KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$baseTexture detail/dt_metal1 $additive 1 $envmap editor/cube_vertigo"));
		kv->setString(crypt_str("$color"), crypt_str("[.05 .05 .05]"));
		glass = interfaces.material_system->CreateMaterial(crypt_str("glass"), kv);
	}

	{
		const auto kv = KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$baseTexture black $bumpmap effects/flat_normal $translucent 1 $envmap models/effects/crystal_cube_vertigo_hdr $envmapfresnel 0 $phong 1 $phongexponent 16 $phongboost 2"));
		kv->setString(crypt_str("$phongtint"), crypt_str("[.2 .35 .6]"));
		crystal = interfaces.material_system->CreateMaterial(crypt_str("crystal"), kv);
	}

	{
		const auto kv = KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 2 $phongexponent 8"));
		kv->setString(crypt_str("$color2"), crypt_str("[.05 .05 .05]"));
		kv->setString(crypt_str("$envmaptint"), crypt_str("[.2 .2 .2]"));
		kv->setString(crypt_str("$phongfresnelranges"), crypt_str("[.7 .8 1]"));
		kv->setString(crypt_str("$phongtint"), crypt_str("[.8 .9 1]"));
		silver = interfaces.material_system->CreateMaterial(crypt_str("silver"), kv);
	}

	{
		const auto kv = KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 6 $phongexponent 128 $phongdisablehalflambert 1"));
		kv->setString(crypt_str("$color2"), crypt_str("[.18 .15 .06]"));
		kv->setString(crypt_str("$envmaptint"), crypt_str("[.6 .5 .2]"));
		kv->setString(crypt_str("$phongfresnelranges"), crypt_str("[.7 .8 1]"));
		kv->setString(crypt_str("$phongtint"), crypt_str("[.6 .5 .2]"));
		gold = interfaces.material_system->CreateMaterial(crypt_str("gold"), kv);
	}

	{
		const auto kv = KeyValues::fromString(crypt_str("VertexLitGeneric"), crypt_str("$baseTexture black $bumpmap models/inventory_items/trophy_majors/matte_metal_normal $additive 1 $envmap editor/cube_vertigo $envmapfresnel 1 $normalmapalphaenvmapmask 1 $phong 1 $phongboost 20 $phongexponent 3000 $phongdisablehalflambert 1"));
		kv->setString(crypt_str("$phongfresnelranges"), crypt_str("[.1 .4 1]"));
		kv->setString(crypt_str("$phongtint"), crypt_str("[.8 .9 1]"));
		plastic = interfaces.material_system->CreateMaterial(crypt_str("plastic"), kv);
	}
}

bool Chams::render(void* ctx, void* state, const ModelRenderInfo_t& info, matrix* customBoneToWorld) noexcept
{
	appliedChams = false;

	this->ctx = ctx;
	this->state = state;
	this->info = &info;
	this->customBoneToWorld = customBoneToWorld;

	if (std::string_view{ info.pModel->name }._Starts_with(crypt_str("models/weapons/v_"))) {
		// info.model->name + 17 -> small optimization, skip "models/weapons/v_"
		if (std::strstr(info.pModel->name + 17, crypt_str("sleeve")))
			renderSleeves();
		else if (std::strstr(info.pModel->name + 17, crypt_str("arms")))
			renderHands();
		else if (!std::strstr(info.pModel->name + 17, crypt_str("tablet"))
			&& !std::strstr(info.pModel->name + 17, crypt_str("parachute"))
			&& !std::strstr(info.pModel->name + 17, crypt_str("fists")))
			renderWeapons();
	}
	else {
		const auto entity = interfaces.ent_list->GetClientEntity(info.entity_index);
		if (entity && !entity->IsDormant() && entity->IsPlayer())
			renderPlayer(entity);
	}

	return appliedChams;
}

void Chams::renderPlayer(IBasePlayer* player) noexcept
{
	if (!csgo->local)
		return;

	const auto health = player->GetHealth();

	if (player == csgo->local) {
		if (csgo->local->IsScoped() && vars.visuals.blend_on_scope)
			interfaces.render_view->SetBlend((100.f - vars.visuals.blend_value) / 100.f);
		applyChams(vars.chams[LOCALPLAYER].materials, false, health);
		if (csgo->fakematrix != nullptr && vars.antiaim.enable) {
			for (auto& i : csgo->fakematrix)
			{
				i[0][3] += info->origin.x;
				i[1][3] += info->origin.y;
				i[2][3] += info->origin.z;
			}
			if (!appliedChams)
				g_pModelRenderHook->callOriginal<void, 21>(ctx, state, info, customBoneToWorld);

			if (csgo->game_rules->IsFreezeTime()
				|| csgo->local->HasGunGameImmunity()
				|| csgo->local->GetFlags() & FL_FROZEN)
			{
				//no
			}
			else {
				if (csgo->local->IsScoped() && vars.visuals.blend_on_scope)
					interfaces.render_view->SetBlend((100.f - vars.visuals.blend_value) / 100.f);
				applyChams(vars.chams[DESYNC].materials, false, health, csgo->fakematrix);
				interfaces.studio_render->forcedMaterialOverride(nullptr);
				for (auto& i : csgo->fakematrix)
				{
					i[0][3] -= info->origin.x;
					i[1][3] -= info->origin.y;
					i[2][3] -= info->origin.z;
				}
			}
		}
	}
	else if (player->isEnemy(csgo->local)) {
		

		//overlap chams for debugging
		/*
		applyChams(config->chams[DESYNC].materials, false, health, Animations::data.player[player->index()].n_matrix.data());
		applyChams(config->chams[DESYNC].materials, false, health, Animations::data.player[player->index()].p_matrix.data());
		*/
#ifdef _DEBUG
		const auto last = g_Animfix->get_oldest_animation(player);

		if (last) {
			applyChams(vars.chams[DESYNC].materials, false, health, g_Animfix->get_latest_animation(player)->negative_matrix);
			applyChams(vars.chams[DESYNC].materials, false, health, g_Animfix->get_latest_animation(player)->positive_matrix);
		}
#endif

		if ((vars.legitbot.backtrack || vars.ragebot.enable) && (vars.chams[BACKTRACK].materials[0].enabled || vars.chams[BACKTRACK].materials[1].enabled)) {
			matrix record[128];
			if (GetBacktrackMaxtrix(player, record)) {
				if (!appliedChams)
					g_pModelRenderHook->callOriginal<void, 21>(ctx, state, info, customBoneToWorld);
				
				applyChams(vars.chams[BACKTACK_OCCLUDED].materials, true, health, record);
				applyChams(vars.chams[BACKTRACK].materials, false, health, record);

				interfaces.studio_render->forcedMaterialOverride(nullptr);
			}
		}

		applyChams(vars.chams[ENEMIES_OCCLUDED].materials, true, health);
		applyChams(vars.chams[ENEMIES_VISIBLE].materials, false, health);



	}
	else {
		applyChams(vars.chams[ALLIES_OCCLUDED].materials, true, health);
		applyChams(vars.chams[ALLIES_VISIBLE].materials, false, health);
	}
}

void Chams::renderWeapons() noexcept
{
	if (!csgo->local || !csgo->local->isAlive() || csgo->local->IsScoped())
		return;

	applyChams(vars.chams[WEAPONS].materials, false, csgo->local->GetHealth());
}

void Chams::renderHands() noexcept
{
	if (!csgo->local || !csgo->local->isAlive())
		return;

	applyChams(vars.chams[HANDS].materials, false, csgo->local->GetHealth());
}

void Chams::renderSleeves() noexcept
{
	if (!csgo->local || !csgo->local->isAlive())
		return;

	applyChams(vars.chams[SLEEVES].materials, false, csgo->local->GetHealth());
}

void Chams::applyChams(const std::vector<CGlobalVariables::Chams::Material>& chams, bool ignorez, int health, matrix* customMatrix) noexcept
{
	for (const auto& cham : chams) {
		if (!cham.enabled || !ignorez)
			continue;

		const auto material = dispatchMaterial(cham.material);
		if (!material)
			continue;

		if (material == glow || material == chrome || material == plastic || material == glass || material == crystal) {
			if (cham.healthBased && health) {
				material->FindVar(crypt_str("$envmaptint"), nullptr)->SetVecValue(1.0f - health / 100.0f, health / 100.0f, 0.0f);
	
			}
			else if (cham.rainbow) {
				const auto [r, g, b] { rainbowColor(interfaces.global_vars->realtime, cham.rainbowSpeed) };
				material->FindVar(crypt_str("$envmaptint"), nullptr)->SetVecValue(r, g, b);
			}
			else {
				material->FindVar(crypt_str("$envmaptint"), nullptr)->SetVecValue(cham.color[0] / 255.f, cham.color[1] / 255.f, cham.color[2] / 255.f);
			}
		}
		else {
			if (cham.healthBased && health) {
				material->ColorModulate(1.0f - health / 100.0f, health / 100.0f, 0.0f);
			}
			else if (cham.rainbow) {
				const auto [r, g, b] { rainbowColor(interfaces.global_vars->realtime, cham.rainbowSpeed) };
				material->ColorModulate(r, g, b);
			}
			else {
				material->ColorModulate(cham.color[0] / 255.f, cham.color[1] / 255.f, cham.color[2] / 255.f);
			}
		}

		const auto pulse = (cham.color[3] / 255.f) * (cham.blinking ? std::sin(interfaces.global_vars->curtime * 5) * 0.5f + 0.5f : 1.0f);

		if (material == glow)
			material->FindVar(crypt_str("$envmapfresnelminmaxexp"), nullptr)->setVecComponentValue(9.0f * (1.2f - pulse), 2);
		else
			material->AlphaModulate(pulse);

		material->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_IGNOREZ, true);
		material->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_WIREFRAME, cham.wireframe);
		interfaces.studio_render->forcedMaterialOverride(material);
		g_pModelRenderHook->callOriginal<void, 21>(ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
		interfaces.studio_render->forcedMaterialOverride(nullptr);
	}

	for (const auto& cham : chams) {
		if (!cham.enabled || ignorez)
			continue;

		const auto material = dispatchMaterial(cham.material);
		if (!material)
			continue;

		if (material == glow || material == chrome || material == plastic || material == glass || material == crystal) {
			if (cham.healthBased && health) {
				material->FindVar(crypt_str("$envmaptint"), nullptr)->SetVecValue(1.0f - health / 100.0f, health / 100.0f, 0.0f);
			}
			else if (cham.rainbow) {
				const auto [r, g, b] { rainbowColor(interfaces.global_vars->realtime, cham.rainbowSpeed) };
				material->FindVar(crypt_str("$envmaptint"), nullptr)->SetVecValue(r, g, b);
			}
			else {
				material->FindVar(crypt_str("$envmaptint"), nullptr)->SetVecValue(cham.color[0] / 255.f, cham.color[1] / 255.f, cham.color[2] / 255.f);
			}
		}
		else {
			if (cham.healthBased && health) {
				material->ColorModulate(1.0f - health / 100.0f, health / 100.0f, 0.0f);
			}
			else if (cham.rainbow) {
				const auto [r, g, b] { rainbowColor(interfaces.global_vars->realtime, cham.rainbowSpeed) };
				material->ColorModulate(r, g, b);
			}
			else {
				material->ColorModulate(cham.color[0] / 255.f, cham.color[1] / 255.f, cham.color[2] / 255.f);
			}
		}

		const auto pulse = (cham.color[3] / 255.f) * (cham.blinking ? std::sin(interfaces.global_vars->curtime * 5) * 0.5f + 0.5f : 1.0f);

		if (material == glow)
			material->FindVar(crypt_str("$envmapfresnelminmaxexp"), nullptr)->setVecComponentValue(9.0f * (1.2f - pulse), 2);
		else
			material->AlphaModulate(pulse);

		material->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_IGNOREZ, false);
		material->SetMaterialVarFlag(MaterialVarFlags_t::MATERIAL_VAR_WIREFRAME, cham.wireframe);
		interfaces.studio_render->forcedMaterialOverride(material);
		g_pModelRenderHook->callOriginal<void, 21>(ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
		appliedChams = true;
	}
}












/*
void CChams::AddHitmatrix(animation* record) {
	auto& hit = m_Hitmatrix.emplace_back();
	auto player = record->player;
	std::memcpy(hit.pBoneToWorld, record->bones, player->GetBoneCache().Count() * sizeof(matrix));
	hit.time = interfaces.global_vars->realtime + 4.f;

	int m_nSkin = FindInDataMap(player->GetPredDescMap(), crypt_str("m_nSkin"));
	int m_nBody = FindInDataMap(player->GetPredDescMap(), crypt_str("m_nBody"));

	hit.info.origin = player->GetAbsOrigin();
	hit.info.angles = player->GetAbsAngles();

	auto renderable = player->GetClientRenderable();

	if (!renderable)
		return;

	auto model = player->GetModel();

	if (!model)
		return;

	auto hdr = interfaces.models.model_info->GetStudioModel(model);
	if (!hdr)
		return;

	hit.state.m_pStudioHdr = hdr;
	hit.state.m_pStudioHWData = interfaces.model_cache->GetHardwareData(model->studio);

	hit.state.m_pRenderable = renderable;
	hit.state.m_drawFlags = 0;

	hit.info.pRenderable = renderable;
	hit.info.pModel = model;
	hit.info.pLightingOffset = nullptr;
	hit.info.pLightingOrigin = nullptr;
	hit.info.hitboxset = player->GetHitboxSet();
	hit.info.skin = (int)(uintptr_t(player) + m_nSkin);
	hit.info.body = (int)(uintptr_t(player) + m_nBody);
	hit.info.entity_index = player->GetIndex();
	hit.info.instance = getvfunc<ModelInstanceHandle_t(__thiscall*)(void*) >(renderable, 30)(renderable);
	hit.info.flags = 0x1;

	hit.info.pModelToWorld = &hit.model_to_world;
	hit.state.m_pModelToWorld = &hit.model_to_world;
	Math::AngleMatrix(hit.info.angles, hit.info.origin, hit.model_to_world);
}

void CChams::OnPostScreenEffects() {
	if (m_Hitmatrix.empty())
		return;

	auto isNullMaterial = [&] {
		if (!Regular
			|| !Flat
			|| !Metallic
			|| !Glow 
			|| !GlowFade)
			return true;

		return false;
	};

	if (isNullMaterial())
		return;

	static auto DrawModelExecute = g_pModelRenderHook->GetOriginal< DrawModelExecuteFn >(21);

	static IMaterial* OverlayMaterialShot = nullptr;
	static IMaterial* OverridedMaterialShot = nullptr;

	switch (vars.visuals.misc_chams[shot_record].overlay)
	{
	case 1: OverlayMaterialShot = Glow; break;
	case 2: OverlayMaterialShot = GlowFade; break;
	}

	if (OverlayMaterialShot)
		OverlayMaterialShot->IncrementReferenceCount();

	switch (vars.visuals.misc_chams[shot_record].material)
	{
	case 0: OverridedMaterialShot = Regular; break;
	case 1: OverridedMaterialShot = Flat; break;
	case 2: OverridedMaterialShot = Metallic; break;
	}

	if (OverridedMaterialShot)
		OverridedMaterialShot->IncrementReferenceCount();

	if (!interfaces.models.model_render)
		return;

	auto drawed_shit = false;

	auto ctx = interfaces.material_system->GetRenderContext();

	if (!ctx)
		return;

	auto it = m_Hitmatrix.begin();
	while (it != m_Hitmatrix.end()) {

		if (!it->state.m_pModelToWorld || !it->state.m_pRenderable || !it->state.m_pStudioHdr || !it->state.m_pStudioHWData ||
			!it->info.pRenderable || !it->info.pModelToWorld || !it->info.pModel) {
			++it;
			continue;
		}
		auto ent = interfaces.ent_list->GetClientEntity(it->info.entity_index);
		if (!ent) {
			it = m_Hitmatrix.erase(it);
			continue;
		}
		auto alpha = 1.0f;
		auto delta = interfaces.global_vars->realtime - it->time;
		if (delta > 0.0f) {
			alpha -= delta;
			if (delta > 1.0f) {
				it = m_Hitmatrix.erase(it);
				continue;
			}
		}

		drawed_shit = true;

		if (vars.visuals.misc_chams[shot_record].material == 2)
		{
			auto phong = OverridedMaterialShot->FindVar(hs::phongexponent.s().c_str(), nullptr);
			phong->SetVecValue((float)vars.visuals.misc_chams[shot_record].phong_exponent);
			auto phong2 = OverridedMaterialShot->FindVar(hs::phongboost.s().c_str(), nullptr);
			phong2->SetVecValue((float)vars.visuals.misc_chams[shot_record].phong_boost);

			auto rim = OverridedMaterialShot->FindVar(hs::rimlightexponent.s().c_str(), nullptr);
			rim->SetVecValue((float)vars.visuals.misc_chams[shot_record].rim);
			auto rim2 = OverridedMaterialShot->FindVar(hs::rimlightboost.s().c_str(), nullptr);
			rim2->SetVecValue((float)vars.visuals.misc_chams[shot_record].rim);
			auto pearl = OverridedMaterialShot->FindVar(hs::pearlescent.s().c_str(), nullptr);
			pearl->SetVecValue(vars.visuals.misc_chams[shot_record].pearlescent / 100.f);
			auto var = OverridedMaterialShot->FindVar(hs::envmaptint.s().c_str(), nullptr);
			var->SetVecValue(vars.visuals.misc_chams[shot_record].metallic_clr[0] / 255.f,
				vars.visuals.misc_chams[shot_record].metallic_clr[1] / 255.f, vars.visuals.misc_chams[shot_record].metallic_clr[2] / 255.f);

			auto var2 = OverridedMaterialShot->FindVar(hs::phongtint.s().c_str(), nullptr);
			var2->SetVecValue(vars.visuals.misc_chams[shot_record].metallic_clr2[0] / 255.f,
				vars.visuals.misc_chams[shot_record].metallic_clr2[1] / 255.f, vars.visuals.misc_chams[shot_record].metallic_clr2[2] / 255.f);
		}

		OverridedMaterialShot->ColorModulate(
			vars.visuals.misc_chams[shot_record].clr[0] / 255.f,
			vars.visuals.misc_chams[shot_record].clr[1] / 255.f,
			vars.visuals.misc_chams[shot_record].clr[2] / 255.f);
		OverridedMaterialShot->AlphaModulate(vars.visuals.misc_chams[shot_record].clr[3] / 255.f);
		OverridedMaterialShot->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		float clr[3] = {
			(vars.visuals.misc_chams[shot_record].clr[0] / 255.f) * vars.visuals.misc_chams[shot_record].chams_brightness / 100.f,
			(vars.visuals.misc_chams[shot_record].clr[1] / 255.f) * vars.visuals.misc_chams[shot_record].chams_brightness / 100.f,
			(vars.visuals.misc_chams[shot_record].clr[2] / 255.f) * vars.visuals.misc_chams[shot_record].chams_brightness / 100.f
		};
		float backup_clr[3];
		interfaces.render_view->GetColorModulation(backup_clr);
		interfaces.render_view->SetColorModulation(clr);

		interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterialShot);
		DrawModelExecute(interfaces.models.model_render, ctx, it->state, it->info, it->pBoneToWorld);

		if (vars.visuals.misc_chams[shot_record].overlay > 0) {
			auto pVar = OverlayMaterialShot->FindVar(hs::envmaptint.s().c_str(), nullptr);
			pVar->SetVecValue(vars.visuals.misc_chams[shot_record].glow_clr[0] / 255.f,
				vars.visuals.misc_chams[shot_record].glow_clr[1] / 255.f, vars.visuals.misc_chams[shot_record].glow_clr[2] / 255.f);

			OverlayMaterialShot->AlphaModulate(vars.visuals.misc_chams[shot_record].glow_clr[3] / 255.f);
			OverlayMaterialShot->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

			interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterialShot);
			DrawModelExecute(interfaces.models.model_render, ctx, it->state, it->info, it->pBoneToWorld);
		}
		interfaces.models.model_render->ForcedMaterialOverride(nullptr);
		interfaces.render_view->SetColorModulation(backup_clr);

		/*auto color = vars.ragebot.shot_clr;
		color[3] *= alpha;
		auto pVar = GlowFade->FindVar(hs::envmaptint.s().c_str(), nullptr);
		pVar->SetVecValue(color[0] / 255.f,
			color[1] / 255.f, color[2] / 255.f);
		GlowFade->AlphaModulate(color[3] / 255.f);
		GlowFade->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

		interfaces.models.model_render->ForcedMaterialOverride(GlowFade);
		DrawModelExecute(interfaces.models.model_render, ctx, it->state, it->info, it->pBoneToWorld);
		interfaces.models.model_render->ForcedMaterialOverride(nullptr, 0);
		++it;
	}
}

bool str_starts_with(const char* str, const char* search_str)
{
	bool ret = false;

	for (intptr_t i = 0;; ++i)
	{
		if (search_str[i] == '\0')
		{
			ret = true;
			break;
		}

		if (str[i] != search_str[i])
			goto out;

		if (str[i] == '\0')
			break;
	}

out:
	return ret;
}

void CChams::Draw(void* thisptr, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix* pCustomBoneToWorld)
{
	static auto DrawModelExecute = g_pModelRenderHook->GetOriginal< DrawModelExecuteFn >(21);

	static bool init = false;

	if (!init) {
		this->Regular = interfaces.material_system->FindMaterial(hs::debugambientcube.s().c_str(), nullptr);
		this->Flat = interfaces.material_system->FindMaterial(hs::debugdrawflat.s().c_str(), nullptr);
		this->Metallic = interfaces.material_system->FindMaterial(hs::regular_ref.s().c_str(), nullptr);
		this->Glow = interfaces.material_system->FindMaterial(hs::glow_armsrace.s().c_str(), nullptr);
		this->GlowFade = interfaces.material_system->FindMaterial(hs::regular_glow.s().c_str(), nullptr);
		init = true;
	}

	static std::string models_player = crypt_str("models/player");
	static std::string _arms = crypt_str("arms");
	static std::string v_ = crypt_str("v_");

	Regular->IncrementReferenceCount();
	Flat->IncrementReferenceCount();
	Metallic->IncrementReferenceCount();
	Glow->IncrementReferenceCount();
	GlowFade->IncrementReferenceCount();

	const auto ModelName = info.pModel->name;
	bool is_player = str_starts_with(ModelName, models_player.c_str());
	if (is_player/*strstr(ModelName, crypt_str("models/player")) != nullptr)
	{
		auto Entity = interfaces.ent_list->GetClientEntity(info.entity_index);

		if (!Entity)
			return DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);

		if (
			[&]() {
				if (Entity == nullptr)
					return false;
				if (!Entity->IsPlayer()) 
					return false;
				if (!Entity->isAlive())
					return false;
				if (Entity->HasGunGameImmunity())
					return false;
				if (Entity->IsDormant())
					return false;
				if (Entity == csgo->local)
					return false;
				if (Entity->GetTeam() == csgo->local->GetTeam())
					return false;

				return true;
			}())
		{
			static IMaterial* OverlayMaterial = nullptr;
			static IMaterial* OverlayMaterialXqz = nullptr;

			static IMaterial* OverridedMaterial = nullptr;
			static IMaterial* OverridedMaterialXqz = nullptr;

			static IMaterial* OverridedMaterialBck = nullptr;
			static IMaterial* OverlayMaterialBck = nullptr;

			switch (vars.visuals.overlay)
			{
			case 1: OverlayMaterial = Glow; break;
			case 2: OverlayMaterial = GlowFade; break;
			}

			if (OverlayMaterial)
				OverlayMaterial->IncrementReferenceCount();

			switch (vars.visuals.misc_chams[history].overlay)
			{
			case 1: OverlayMaterialBck = Glow; break;
			case 2: OverlayMaterialBck = GlowFade; break;
			}

			if (OverlayMaterialBck)
				OverlayMaterialBck->IncrementReferenceCount();

			switch (vars.visuals.overlay_xqz)
			{
			case 1: OverlayMaterialXqz = Glow; break;
			case 2: OverlayMaterialXqz = GlowFade; break;
			}

			switch (vars.visuals.chamstype)
			{
			case 0: OverridedMaterial = Regular; break;
			case 1: OverridedMaterial = Flat; break;
			case 2: OverridedMaterial = Metallic; break;
			}

			if (OverridedMaterial)
				OverridedMaterial->IncrementReferenceCount();

			OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, true);

			switch (vars.visuals.misc_chams[history].material)
			{
			case 0: OverridedMaterialBck = Regular; break;
			case 1: OverridedMaterialBck = Flat; break;
			case 2: OverridedMaterialBck = Metallic; break;
			}

			if (OverridedMaterialBck)
				OverridedMaterialBck->IncrementReferenceCount();

			matrix pMat[128];
			if (vars.visuals.misc_chams[history].enable && GetBacktrackMaxtrix(Entity, pMat)) {
				if (vars.visuals.misc_chams[history].material == 2)
				{
					auto phong = OverridedMaterialBck->FindVar(hs::phongexponent.s().c_str(), nullptr);
					phong->SetVecValue((float)vars.visuals.misc_chams[history].phong_exponent);
					auto phong2 = OverridedMaterialBck->FindVar(hs::phongboost.s().c_str(), nullptr);
					phong2->SetVecValue((float)vars.visuals.misc_chams[history].phong_boost);

					auto rim = OverridedMaterialBck->FindVar(hs::rimlightexponent.s().c_str(), nullptr);
					rim->SetVecValue((float)vars.visuals.misc_chams[history].rim);
					auto rim2 = OverridedMaterialBck->FindVar(hs::rimlightboost.s().c_str(), nullptr);
					rim2->SetVecValue((float)vars.visuals.misc_chams[history].rim);
					auto pearl = OverridedMaterialBck->FindVar(hs::pearlescent.s().c_str(), nullptr);
					pearl->SetVecValue(vars.visuals.misc_chams[history].pearlescent / 100.f);
					auto var = OverridedMaterialBck->FindVar(hs::envmaptint.s().c_str(), nullptr);
					var->SetVecValue(vars.visuals.misc_chams[history].metallic_clr[0] / 255.f,
						vars.visuals.misc_chams[history].metallic_clr[1] / 255.f, vars.visuals.misc_chams[history].metallic_clr[2] / 255.f);

					auto var2 = OverridedMaterialBck->FindVar(hs::phongtint.s().c_str(), nullptr);
					var2->SetVecValue(vars.visuals.misc_chams[history].metallic_clr2[0] / 255.f,
						vars.visuals.misc_chams[history].metallic_clr2[1] / 255.f, vars.visuals.misc_chams[history].metallic_clr2[2] / 255.f);
				}

				OverridedMaterialBck->ColorModulate(
					vars.visuals.misc_chams[history].clr[0] / 255.f,
					vars.visuals.misc_chams[history].clr[1] / 255.f,
					vars.visuals.misc_chams[history].clr[2] / 255.f);
				OverridedMaterialBck->AlphaModulate(vars.visuals.misc_chams[history].clr[3] / 255.f);
				OverridedMaterialBck->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				float clr[3] = {
					(vars.visuals.misc_chams[history].clr[0] / 255.f) * vars.visuals.misc_chams[history].chams_brightness / 100.f,
					(vars.visuals.misc_chams[history].clr[1] / 255.f) * vars.visuals.misc_chams[history].chams_brightness / 100.f,
					(vars.visuals.misc_chams[history].clr[2] / 255.f) * vars.visuals.misc_chams[history].chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);

				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterialBck);
				DrawModelExecute(interfaces.models.model_render, ctx, state, info, pMat);

				if (vars.visuals.misc_chams[history].overlay > 0 && OverlayMaterialBck) {
					auto pVar = OverlayMaterialBck->FindVar(hs::envmaptint.s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.misc_chams[history].glow_clr[0] / 255.f,
						vars.visuals.misc_chams[history].glow_clr[1] / 255.f, vars.visuals.misc_chams[history].glow_clr[2] / 255.f);

					OverlayMaterialBck->AlphaModulate(vars.visuals.misc_chams[history].glow_clr[3] / 255.f);
					OverlayMaterialBck->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterialBck);
					DrawModelExecute(interfaces.models.model_render, ctx, state, info, pMat);
				}
				interfaces.models.model_render->ForcedMaterialOverride(nullptr);
				interfaces.render_view->SetColorModulation(backup_clr);
			}
		/*	auto latest_animation = g_Animfix->get_latest_animation(Entity);

			if (latest_animation) {
				auto pizda = Flat;
				pizda->AlphaModulate(0.5f);
				pizda->ColorModulate(1.f, 0.f, 0.f);

				interfaces.models.model_render->ForcedMaterialOverride(pizda);
				DrawModelExecute(interfaces.models.model_render, ctx, state, info, latest_animation->unresolved_bones);
		

				pizda->ColorModulate(0.f, 0.f, 1.f);
				interfaces.models.model_render->ForcedMaterialOverride(pizda);
				DrawModelExecute(interfaces.models.model_render, ctx, state, info, latest_animation->inversed_bones);
			}

			if (vars.visuals.chamstype == 2)
			{
				auto phong = OverridedMaterial->FindVar(hs::phongexponent.s().c_str(), nullptr);
				phong->SetVecValue((float)vars.visuals.phong_exponent);
				auto phong2 = OverridedMaterial->FindVar(hs::phongboost.s().c_str(), nullptr);
				phong2->SetVecValue((float)vars.visuals.phong_boost);

				auto rim = OverridedMaterial->FindVar(hs::rimlightexponent.s().c_str(), nullptr);
				rim->SetVecValue((float)vars.visuals.rim);
				auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost.s().c_str(), nullptr);
				rim2->SetVecValue((float)vars.visuals.rim);
				auto pearl = OverridedMaterial->FindVar(hs::pearlescent.s().c_str(), nullptr);
				pearl->SetVecValue((float)vars.visuals.pearlescent / 100.f);
				auto var = OverridedMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
				var->SetVecValue(vars.visuals.metallic_clr[0] / 255.f, vars.visuals.metallic_clr[1] / 255.f, vars.visuals.metallic_clr[2] / 255.f);

				auto var2 = OverridedMaterial->FindVar(hs::phongtint.s().c_str(), nullptr);
				var2->SetVecValue(vars.visuals.metallic_clr2[0] / 255.f, vars.visuals.metallic_clr2[1] / 255.f, vars.visuals.metallic_clr2[2] / 255.f);
			}

			if (vars.visuals.chamsxqz)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.chamscolor_xqz[0] / 255.f, vars.visuals.chamscolor_xqz[1] / 255.f, vars.visuals.chamscolor_xqz[2] / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.chamscolor_xqz[3] / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				float clr[3] = {
				(vars.visuals.chamscolor_xqz[0] / 255.f) * vars.visuals.chams_brightness / 100.f,
				(vars.visuals.chamscolor_xqz[1] / 255.f) * vars.visuals.chams_brightness / 100.f,
				(vars.visuals.chamscolor_xqz[2] / 255.f) * vars.visuals.chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);
				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);

				if (vars.visuals.overlay_xqz > 0) {
					auto pVar = OverlayMaterialXqz->FindVar(hs::envmaptint.s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.glow_col_xqz[0] / 255.f, vars.visuals.glow_col_xqz[1] / 255.f, vars.visuals.glow_col_xqz[2] / 255.f);

					OverlayMaterialXqz->AlphaModulate(vars.visuals.glow_col_xqz[3] / 255.f);
					OverlayMaterialXqz->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterialXqz);
					DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);
					interfaces.render_view->SetColorModulation(backup_clr);
				}
			}

			if (vars.visuals.chams)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.chamscolor[0] / 255.f, vars.visuals.chamscolor[1] / 255.f, vars.visuals.chamscolor[2] / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.chamscolor[3] / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				float clr[3] = {
					(vars.visuals.chamscolor[0] / 255.f) * vars.visuals.chams_brightness / 100.f,
					(vars.visuals.chamscolor[1] / 255.f) * vars.visuals.chams_brightness / 100.f,
					(vars.visuals.chamscolor[2] / 255.f) * vars.visuals.chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);
				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);

				if (vars.visuals.overlay > 0) {
					auto pVar = OverlayMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.glow_col[0] / 255.f, vars.visuals.glow_col[1] / 255.f, vars.visuals.glow_col[2] / 255.f);

					OverlayMaterial->AlphaModulate(vars.visuals.glow_col[3] / 255.f);
					OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
					DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);
					interfaces.render_view->SetColorModulation(backup_clr);
				}
			}
		}
		else if (Entity == csgo->local && csgo->local->isAlive())
		{

			static IMaterial* OverridedMaterialLocal = nullptr;
			static IMaterial* OverlayMaterialLocal = nullptr;
			if (interfaces.input->m_fCameraInThirdPerson) {

				switch (vars.visuals.localchamstype)
				{
				case 0: OverridedMaterialLocal = Regular; break;
				case 1: OverridedMaterialLocal = Flat; break;
				case 2: OverridedMaterialLocal = Metallic; break;
				}

				if (OverridedMaterialLocal)
					OverridedMaterialLocal->IncrementReferenceCount();

				switch (vars.visuals.local_chams.overlay)
				{
				case 1: OverlayMaterialLocal = Glow; break;
				case 2: OverlayMaterialLocal = GlowFade; break;
				}

				if (OverlayMaterialLocal)
					OverlayMaterialLocal->IncrementReferenceCount();

				if (csgo->local && csgo->local->isAlive()) {
					if (vars.visuals.blend_on_scope)
					{
						if (csgo->local->IsScoped())
							interfaces.render_view->SetBlend(vars.visuals.blend_value / 100.f);
					}
				}

				bool fr = csgo->game_rules->IsFreezeTime()
					|| csgo->local->HasGunGameImmunity()
					|| csgo->local->GetFlags() & FL_FROZEN;

				if (csgo->fakematrix != nullptr && csgo->local && !fr)
				{
					if (vars.visuals.interpolated_dsy) {
						for (auto& i : csgo->fakematrix)
						{
							i[0][3] += info.origin.x;
							i[1][3] += info.origin.y;
							i[2][3] += info.origin.z;
						}
					}

					static IMaterial* OverridedMaterial = nullptr;
					static IMaterial* OverlayMaterial = nullptr;

					switch (vars.visuals.misc_chams[desync].overlay)
					{
					case 1: OverlayMaterial = Glow; break;
					case 2: OverlayMaterial = GlowFade; break;
					}

					switch (vars.visuals.misc_chams[desync].material)
					{
					case 0: OverridedMaterial = Regular; break;
					case 1: OverridedMaterial = Flat; break;
					case 2: OverridedMaterial = Metallic; break;
					}

					if (vars.visuals.misc_chams[desync].material == 2)
					{
						auto phong = OverridedMaterial->FindVar(hs::phongexponent.s().c_str(), nullptr);
						phong->SetVecValue((float)vars.visuals.misc_chams[desync].phong_exponent);
						auto phong2 = OverridedMaterial->FindVar(hs::phongboost.s().c_str(), nullptr);
						phong2->SetVecValue((float)vars.visuals.misc_chams[desync].phong_boost);

						auto rim = OverridedMaterial->FindVar(hs::rimlightexponent.s().c_str(), nullptr);
						rim->SetVecValue((float)vars.visuals.misc_chams[desync].rim);
						auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost.s().c_str(), nullptr);
						rim2->SetVecValue((float)vars.visuals.misc_chams[desync].rim);
						auto pearl = OverridedMaterial->FindVar(hs::pearlescent.s().c_str(), nullptr);
						pearl->SetVecValue(vars.visuals.misc_chams[desync].pearlescent / 100.f);
						auto var = OverridedMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
						var->SetVecValue(vars.visuals.misc_chams[desync].metallic_clr[0] / 255.f,
							vars.visuals.misc_chams[desync].metallic_clr[1] / 255.f, vars.visuals.misc_chams[desync].metallic_clr[2] / 255.f);

						auto var2 = OverridedMaterial->FindVar(hs::phongtint.s().c_str(), nullptr);
						var2->SetVecValue(vars.visuals.misc_chams[desync].metallic_clr2[0] / 255.f,
							vars.visuals.misc_chams[desync].metallic_clr2[1] / 255.f, vars.visuals.misc_chams[desync].metallic_clr2[2] / 255.f);
					}

					if (vars.visuals.misc_chams[desync].enable)
					{
						OverridedMaterial->ColorModulate(
							vars.visuals.misc_chams[desync].clr[0] / 255.f,
							vars.visuals.misc_chams[desync].clr[1] / 255.f, vars.visuals.misc_chams[desync].clr[2] / 255.f);

						OverridedMaterial->AlphaModulate(vars.visuals.misc_chams[desync].clr[3] / 255.f);
						OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						float clr[3] = {
							(vars.visuals.misc_chams[desync].clr[0] / 255.f) * vars.visuals.misc_chams[desync].chams_brightness / 100.f,
							(vars.visuals.misc_chams[desync].clr[1] / 255.f) * vars.visuals.misc_chams[desync].chams_brightness / 100.f,
							(vars.visuals.misc_chams[desync].clr[2] / 255.f) * vars.visuals.misc_chams[desync].chams_brightness / 100.f
						};

						float backup_clr[3];
						interfaces.render_view->GetColorModulation(backup_clr);
						interfaces.render_view->SetColorModulation(clr);
						interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);

						DrawModelExecute(interfaces.models.model_render, ctx, state, info, csgo->fakematrix);

						interfaces.render_view->SetColorModulation(backup_clr);

						if (vars.visuals.misc_chams[desync].overlay > 0) {
							auto pVar = OverlayMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
							pVar->SetVecValue(vars.visuals.misc_chams[desync].glow_clr[0] / 255.f,
								vars.visuals.misc_chams[desync].glow_clr[1] / 255.f, vars.visuals.misc_chams[desync].glow_clr[2] / 255.f);

							OverlayMaterial->AlphaModulate(vars.visuals.misc_chams[desync].glow_clr[3] / 255.f);
							OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

							interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
							DrawModelExecute(interfaces.models.model_render, ctx, state, info, csgo->fakematrix);
							interfaces.render_view->SetColorModulation(backup_clr);
						}
						interfaces.models.model_render->ForcedMaterialOverride(nullptr);
					}

					if (vars.visuals.interpolated_dsy) {
						for (auto& i : csgo->fakematrix)
						{
							i[0][3] -= info.origin.x;
							i[1][3] -= info.origin.y;
							i[2][3] -= info.origin.z;
						}
					}
				}

				if (vars.visuals.localchams)
				{
					OverridedMaterialLocal->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, true);

					if (vars.visuals.localchamstype == 2)
					{
						auto phong = OverridedMaterialLocal->FindVar(hs::phongexponent.s().c_str(), nullptr);
						phong->SetVecValue((float)vars.visuals.local_chams.phong_exponent);
						auto phong2 = OverridedMaterialLocal->FindVar(hs::phongboost.s().c_str(), nullptr);
						phong2->SetVecValue((float)vars.visuals.local_chams.phong_boost);

						auto rim = OverridedMaterialLocal->FindVar(hs::rimlightexponent.s().c_str(), nullptr);
						rim->SetVecValue((float)vars.visuals.local_chams.rim);
						auto rim2 = OverridedMaterialLocal->FindVar(hs::rimlightboost.s().c_str(), nullptr);
						rim2->SetVecValue((float)vars.visuals.local_chams.rim);
						auto pearl = OverridedMaterialLocal->FindVar(hs::pearlescent.s().c_str(), nullptr);
						pearl->SetVecValue(vars.visuals.local_chams.pearlescent / 100.f);
						auto var = OverridedMaterialLocal->FindVar(hs::envmaptint.s().c_str(), nullptr);
						var->SetVecValue(vars.visuals.local_chams.metallic_clr[0] / 255.f,
							vars.visuals.local_chams.metallic_clr[1] / 255.f, vars.visuals.local_chams.metallic_clr[2] / 255.f);

						auto var2 = OverridedMaterialLocal->FindVar(hs::phongtint.s().c_str(), nullptr);
						var2->SetVecValue(vars.visuals.local_chams.metallic_clr2[0] / 255.f,
							vars.visuals.local_chams.metallic_clr2[1] / 255.f, vars.visuals.local_chams.metallic_clr2[2] / 255.f);
					}
					OverridedMaterialLocal->ColorModulate(
						vars.visuals.localchams_color[0] / 255.f, vars.visuals.localchams_color[1] / 255.f, vars.visuals.localchams_color[2] / 255.f);
					float clr[3] = {
						(vars.visuals.localchams_color[0] / 255.f) * vars.visuals.local_chams_brightness / 100.f,
						(vars.visuals.localchams_color[1] / 255.f) * vars.visuals.local_chams_brightness / 100.f,
						(vars.visuals.localchams_color[2] / 255.f) * vars.visuals.local_chams_brightness / 100.f
					};
					float backup_clr[3];
					interfaces.render_view->GetColorModulation(backup_clr);
					interfaces.render_view->SetColorModulation(clr);

					OverridedMaterialLocal->AlphaModulate(vars.visuals.localchams_color[3] / 255.f);
					OverridedMaterialLocal->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterialLocal);
					DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);

					if (vars.visuals.local_chams.overlay > 0) {
						auto pVar = OverlayMaterialLocal->FindVar(hs::envmaptint.s().c_str(), nullptr);
						pVar->SetVecValue(vars.visuals.local_glow_color[0] / 255.f,
							vars.visuals.local_glow_color[1] / 255.f, vars.visuals.local_glow_color[2] / 255.f);

						OverlayMaterialLocal->AlphaModulate(vars.visuals.local_glow_color[3] / 255.f);
						OverlayMaterialLocal->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);

						interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterialLocal);
						DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);
						interfaces.render_view->SetColorModulation(backup_clr);
					}
				}
				else {
					interfaces.models.model_render->ForcedMaterialOverride(nullptr);
					DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);
				}

			}
		}
	}
	else if (strstr(ModelName, _arms.c_str()) != nullptr)
	{
		if (csgo->local && csgo->local->isAlive() && !interfaces.input->m_fCameraInThirdPerson)
		{
			static IMaterial* OverridedMaterial = nullptr;
			static IMaterial* OverlayMaterial = nullptr;

			switch (vars.visuals.misc_chams[arms].overlay)
			{
			case 1: OverlayMaterial = Glow; break;
			case 2: OverlayMaterial = GlowFade; break;
			}

			if (OverlayMaterial)
				OverlayMaterial->IncrementReferenceCount();

			switch (vars.visuals.misc_chams[arms].material)
			{
			case 0: OverridedMaterial = Regular; break;
			case 1: OverridedMaterial = Flat; break;
			case 2: OverridedMaterial = Metallic; break;
			}

			if (OverridedMaterial)
				OverridedMaterial->IncrementReferenceCount();

			if (vars.visuals.misc_chams[arms].material == 2)
			{
				auto phong = OverridedMaterial->FindVar(hs::phongexponent.s().c_str(), nullptr);
				phong->SetVecValue((float)vars.visuals.misc_chams[arms].phong_exponent);
				auto phong2 = OverridedMaterial->FindVar(hs::phongboost.s().c_str(), nullptr);
				phong2->SetVecValue((float)vars.visuals.misc_chams[arms].phong_boost);

				auto rim = OverridedMaterial->FindVar(hs::rimlightexponent.s().c_str(), nullptr);
				rim->SetVecValue((float)vars.visuals.misc_chams[arms].rim);
				auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost.s().c_str(), nullptr);
				rim2->SetVecValue((float)vars.visuals.misc_chams[arms].rim);
				auto pearl = OverridedMaterial->FindVar(hs::pearlescent.s().c_str(), nullptr);
				pearl->SetVecValue(vars.visuals.misc_chams[arms].pearlescent / 100.f);
				auto var = OverridedMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
				var->SetVecValue(vars.visuals.misc_chams[arms].metallic_clr.get_red() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr.get_green() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr.get_blue() / 255.f);

				auto var2 = OverridedMaterial->FindVar(hs::phongtint.s().c_str(), nullptr);
				var2->SetVecValue(vars.visuals.misc_chams[arms].metallic_clr2.get_red() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr2.get_green() / 255.f,
					vars.visuals.misc_chams[arms].metallic_clr2.get_blue() / 255.f);
			}

			if (vars.visuals.misc_chams[arms].enable)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.misc_chams[arms].clr.get_red() / 255.f,
					vars.visuals.misc_chams[arms].clr.get_green() / 255.f,
					vars.visuals.misc_chams[arms].clr.get_blue() / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.misc_chams[arms].clr.get_alpha() / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				float clr[3] = {
					(vars.visuals.misc_chams[arms].clr.get_red() / 255.f) * vars.visuals.misc_chams[arms].chams_brightness / 100.f,
					(vars.visuals.misc_chams[arms].clr.get_green() / 255.f) * vars.visuals.misc_chams[arms].chams_brightness / 100.f,
					(vars.visuals.misc_chams[arms].clr.get_blue() / 255.f) * vars.visuals.misc_chams[arms].chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);

				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);

				if (vars.visuals.misc_chams[arms].overlay > 0) {
					auto pVar = OverlayMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.misc_chams[arms].glow_clr[0] / 255.f,
						vars.visuals.misc_chams[arms].glow_clr[1] / 255.f, vars.visuals.misc_chams[arms].glow_clr[2] / 255.f);

					OverlayMaterial->AlphaModulate(vars.visuals.misc_chams[arms].glow_clr[3] / 255.f);
					OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
					DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);
				}
			}
		}
	}
	else if (strstr(ModelName, v_.c_str()) != nullptr)
	{
		if (csgo->local && csgo->local->isAlive() && !interfaces.input->m_fCameraInThirdPerson)
		{
			static IMaterial* OverridedMaterial = nullptr;
			static IMaterial* OverlayMaterial = nullptr;

			switch (vars.visuals.misc_chams[weapon].overlay)
			{
			case 1: OverlayMaterial = Glow; break;
			case 2: OverlayMaterial = GlowFade; break;
			}

			if (OverlayMaterial)
				OverlayMaterial->IncrementReferenceCount();

			switch (vars.visuals.misc_chams[weapon].material)
			{
			case 0: OverridedMaterial = Regular; break;
			case 1: OverridedMaterial = Flat; break;
			case 2: OverridedMaterial = Metallic; break;
			}

			if (OverridedMaterial)
				OverridedMaterial->IncrementReferenceCount();

			if (vars.visuals.misc_chams[weapon].material == 2)
			{
				auto phong = OverridedMaterial->FindVar(hs::phongexponent.s().c_str(), nullptr);
				phong->SetVecValue((float)vars.visuals.misc_chams[weapon].phong_exponent);
				auto phong2 = OverridedMaterial->FindVar(hs::phongboost.s().c_str(), nullptr);
				phong2->SetVecValue((float)vars.visuals.misc_chams[weapon].phong_boost);

				auto rim = OverridedMaterial->FindVar(hs::rimlightexponent.s().c_str(), nullptr);
				rim->SetVecValue((float)vars.visuals.misc_chams[weapon].rim);
				auto rim2 = OverridedMaterial->FindVar(hs::rimlightboost.s().c_str(), nullptr);
				rim2->SetVecValue((float)vars.visuals.misc_chams[weapon].rim);
				auto pearl = OverridedMaterial->FindVar(hs::pearlescent.s().c_str(), nullptr);
				pearl->SetVecValue(vars.visuals.misc_chams[weapon].pearlescent / 100.f);
				auto var = OverridedMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
				var->SetVecValue(vars.visuals.misc_chams[weapon].metallic_clr[0] / 255.f,
					vars.visuals.misc_chams[weapon].metallic_clr[1] / 255.f, vars.visuals.misc_chams[weapon].metallic_clr[2] / 255.f);

				auto var2 = OverridedMaterial->FindVar(hs::phongtint.s().c_str(), nullptr);
				var2->SetVecValue(vars.visuals.misc_chams[weapon].metallic_clr2[0] / 255.f,
					vars.visuals.misc_chams[weapon].metallic_clr2[1] / 255.f, vars.visuals.misc_chams[weapon].metallic_clr2[2] / 255.f);
			}

			if (vars.visuals.misc_chams[weapon].enable)
			{
				OverridedMaterial->ColorModulate(
					vars.visuals.misc_chams[weapon].clr[0] / 255.f, vars.visuals.misc_chams[weapon].clr[1] / 255.f, vars.visuals.misc_chams[weapon].clr[2] / 255.f);
				OverridedMaterial->AlphaModulate(vars.visuals.misc_chams[weapon].clr[3] / 255.f);
				OverridedMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				float clr[3] = {
					(vars.visuals.misc_chams[weapon].clr[0] / 255.f) * vars.visuals.misc_chams[weapon].chams_brightness / 100.f,
					(vars.visuals.misc_chams[weapon].clr[1] / 255.f) * vars.visuals.misc_chams[weapon].chams_brightness / 100.f,
					(vars.visuals.misc_chams[weapon].clr[2] / 255.f) * vars.visuals.misc_chams[weapon].chams_brightness / 100.f
				};
				float backup_clr[3];
				interfaces.render_view->GetColorModulation(backup_clr);
				interfaces.render_view->SetColorModulation(clr);

				interfaces.models.model_render->ForcedMaterialOverride(OverridedMaterial);
				DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);

				if (vars.visuals.misc_chams[weapon].overlay > 0) {
					auto pVar = OverlayMaterial->FindVar(hs::envmaptint.s().c_str(), nullptr);
					pVar->SetVecValue(vars.visuals.misc_chams[weapon].glow_clr[0] / 255.f,
						vars.visuals.misc_chams[weapon].glow_clr[1] / 255.f, vars.visuals.misc_chams[weapon].glow_clr[2] / 255.f);

					OverlayMaterial->AlphaModulate(vars.visuals.misc_chams[weapon].glow_clr[3] / 255.f);
					OverlayMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					interfaces.models.model_render->ForcedMaterialOverride(OverlayMaterial);
					DrawModelExecute(interfaces.models.model_render, ctx, state, info, pCustomBoneToWorld);
				}
				interfaces.render_view->SetColorModulation(backup_clr);
			}
		}
	}
}

*/