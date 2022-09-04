/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Hooks.h"
#include "itemdefinitions.hpp"
#include "nSkinz.hpp"
#include "config_.hpp"
#include "sticker_changer.hpp"
#include "i_base_player.h"



static auto erase_override_if_exists_by_index(const int definition_index) -> void
{
	// We have info about the item not needed to be overridden
	if (const auto original_item = game_data::get_weapon_info(definition_index))
	{
		auto& icon_override_map = g_config.get_icon_override_map();

		if (!original_item->icon)
			return;

		const auto override_entry = icon_override_map.find(original_item->icon);

		// We are overriding its icon when not needed
		if (override_entry != end(icon_override_map))
			icon_override_map.erase(override_entry); // Remove the leftover override
	}
}

static auto apply_config_on_attributable_item(IBaseAttributableItem* item, const item_setting* config,
	const unsigned xuid_low) -> void
{
	// Force fallback values to be used.
	item->GetItemIDHigh() = -1;

	// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
	item->GetAccountID() = xuid_low;

	if (config->entity_quality_index)
		item->GetEntityQuality() = config->entity_quality_index;

	if (config->custom_name[0])
		strcpy_s(item->GetCustomName(), 32, config->custom_name);

	if (config->paint_kit_index)
		item->GetFallbackPaintKit() = config->paint_kit_index;

	if (config->seed)
		item->GetFallbackSeed() = config->seed;

	if (config->stat_trak)
		item->GetFallBackStatTrak() = config->stat_trak;

	item->GetFallbackWear() = config->wear;

	auto& definition_index = item->GetItemDefinitionIndex();

	auto& icon_override_map = g_config.get_icon_override_map();

	if (config->definition_override_index // We need to override defindex
		&& (config->definition_override_index != definition_index)) // It is not yet overridden
	{
		// We have info about what we gonna override it to
		if (auto replacement_item = game_data::get_weapon_info(config->definition_override_index))
		{
			auto old_definition_index = definition_index;

			definition_index = config->definition_override_index;

			// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
			//item->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(config->definition_override_index).model);
			item->SetModelIndex(interfaces.models.model_info->GetModelIndex(replacement_item->model));
			//item->GetModelIndex() = interfaces.models.model_info->GetModelIndex(replacement_item->model);
			item->GetClientNetworkable()->PreDataUpdate(0);

			// We didn't override 0, but some actual weapon, that we have data for
			if (old_definition_index)
			{
				if (auto original_item = game_data::get_weapon_info(old_definition_index))
				{
					if (original_item->icon && replacement_item->icon)
						icon_override_map[original_item->icon] = replacement_item->icon;
				}
			}
		}
	}
	else
	{
		erase_override_if_exists_by_index(definition_index);
	}

	//apply_sticker_changer(item);
}

static auto get_wearable_create_fn()
{
	auto clazz = interfaces.client->GetAllClasses();

	// Please, if you gonna paste it into a cheat use classids here. I use names because they
	// won't change in the foreseeable future and i dont need high speed, but chances are
	// you already have classids, so use them instead, they are faster.
	if (clazz)
	{
		while (clazz->m_ClassID != ClassId->CEconWearable)
			clazz = clazz->m_pNext;

		return clazz->m_pCreateFn;
	}
	else
		return reinterpret_cast<void*>(0);
}

using CreateClientClassFn = IClientNetworkable * (*)(int, int);

static auto make_glove(int entry, int serial) -> IBaseAttributableItem*
{
	static auto create_wearable_fn = (CreateClientClassFn)get_wearable_create_fn();

	create_wearable_fn(entry, serial);

	auto glove = static_cast<IBaseAttributableItem*>(interfaces.ent_list->GetClientEntity(entry));
	assert(glove);

	static auto Fn = csgo->Utils.FindPatternIDA(GetModuleHandleA(crypt_str("client.dll")), crypt_str("55 8B EC 83 E4 F8 51 53 56 57 8B F1"));
	static auto set_abs_origin = reinterpret_cast <void(__thiscall*)(void*, const Vector&)> (Fn);

	set_abs_origin(glove, Vector(16384.0f, 16384.0f, 16384.0f));

	return glove;
}

static auto post_data_update_start(IBasePlayer* local) -> void
{

	if (!local)
		return;

	auto local_index = local->index();

	/*if(auto player_resource = *g_player_resource)
	{
		player_resource->GetCoins()[local_index] = 890;
		player_resource->GetMusicKits()[local_index] = 3;
		player_resource->GetRanks()[local_index] = 1;
		player_resource->GetWins()[local_index] = 1337;
	}*/

	player_info_t player_info;
	if (!interfaces.engine->GetPlayerInfo(local_index, &player_info))
		return;

	// Handle glove config
	{
		auto wearables = local->GetWearables();

		auto glove_config = g_config.get_by_definition_index(GLOVE_T_SIDE);

		static auto glove_handle = 0;

		
		auto glove = get_entity_from_handle<IBaseAttributableItem>(wearables[0]);

		if (!glove) // There is no glove
		{
			// Try to get our last created glove
			auto our_glove = get_entity_from_handle<IBaseAttributableItem>(glove_handle);

			if (our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if (!local->isAlive())
		{
			// We are dead but we have a glove, destroy it
			if (glove)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}

			return;
		}

		if (glove_config && glove_config->definition_override_index)
		{
			// We don't have a glove, but we should
			if (!glove)
			{
				auto entry = interfaces.ent_list->GetHighestEntityIndex() + 1;
				auto serial = rand() % 0x1000;

				glove = make_glove(entry, serial);

				wearables[0] = entry | serial << 16;

				// Let's store it in case we somehow lose it.
				glove_handle = wearables[0];
			}

			// Thanks, Beakers
			glove->index() = -1;

			apply_config_on_attributable_item(glove, glove_config, player_info.xuidLow);
		}
	}

	// Handle weapon configs
	{
		auto weapons = local->GetWeapons();

		for (auto wpn : weapons)
		{
			auto weapon = (IBaseAttributableItem*)wpn;
			

			auto& definition_index = weapon->GetItemDefinitionIndex();
			auto active_conf = g_config.get_by_definition_index(is_knife(definition_index) ? WEAPON_KNIFE : definition_index);
			// All knives are terrorist knives.
			if (active_conf)
				apply_config_on_attributable_item(weapon, active_conf, player_info.xuidLow);
			else
				erase_override_if_exists_by_index(definition_index);
		}
	}

	auto view_model = local->GetViewModel();

	if (!view_model)
		return;

	auto view_model_weapon = get_entity_from_handle<IBaseCombatWeapon>(view_model->GetViewmodelWeapon());

	if (!view_model_weapon)
		return;

	auto override_info = game_data::get_weapon_info(view_model_weapon->GetItemDefinitionIndex());

	if (!override_info)
		return;

	auto world_model = get_entity_from_handle<IBaseCombatWeapon>(view_model_weapon->GetWeaponWorldModel());

	if (!world_model)
		return;

	auto override_model_index = interfaces.models.model_info->GetModelIndex(override_info->model);
	view_model->GetModelIndex() = override_model_index;
	world_model->GetModelIndex() = override_model_index + 1;
}



struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};


void updateHud() noexcept
{
	if (!csgo->is_local_alive || !csgo->is_connected || !interfaces.engine->IsInGame())
		return;


	

	int element = H::findHudElement(H::hud, crypt_str("CCSGO_HudWeaponSelection"));
	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);

	if (hud_weapons == nullptr)
		return;

	if (*hud_weapons->get_weapon_count() < 1)
		return;
		
	for (int i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = csgo->clearHudWeapon((int*)hud_weapons, i);
	

	
	csgo->hudUpdateRequired = false;
}

void Skinchanger::scheduleHudUpdate() noexcept
{
	
	if(csgo->local)
		if(csgo->local->GetRefEHandle())
			csgo->hudUpdateRequired = true;

	if (csgo->client_state)
		csgo->client_state->ForceFullUpdate();
}


void Skinchanger::postDataUpdateSkins(void* thisptr)
{
	post_data_update_start((IBasePlayer*)thisptr);
	if (csgo->hudUpdateRequired && csgo->local)
		updateHud();
} 


//hooks::CCSPlayer_PostDataUpdate::Fn* hooks::CCSPlayer_PostDataUpdate::m_original;