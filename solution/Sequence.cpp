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
#include <functional>
#include "Hooks.h"
#include "nSkinz.hpp"
#include "config_.hpp"
#include "DataTable.hpp"
#include "recvprophook.hpp"
#include "vmtsmarthook.hpp"

static auto random_sequence(const int low, const int high) -> int
{
	return rand() % (high - low + 1) + low;
}

// This only fixes if the original knife was a default knife.
// The best would be having a function that converts original knife's sequence
// into some generic enum, then another function that generates a sequence
// from the sequences of the new knife. I won't write that.
static auto get_new_animation(std::string model, const int sequence) -> int
{
	enum ESequence
	{
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,

		SEQUENCE_BUTTERFLY_DRAW = 0,
		SEQUENCE_BUTTERFLY_DRAW2 = 1,
		SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
		SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

		SEQUENCE_FALCHION_IDLE1 = 1,
		SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
		SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
		SEQUENCE_FALCHION_LOOKAT01 = 12,
		SEQUENCE_FALCHION_LOOKAT02 = 13,

		SEQUENCE_CSS_LOOKAT01 = 14, // this is stupid
		SEQUENCE_CSS_LOOKAT02 = 15,

		SEQUENCE_DAGGERS_IDLE1 = 1,
		SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
		SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
		SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
		SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

		SEQUENCE_BOWIE_IDLE1 = 1,
	};

	auto model_hash = std::hash<std::string>{}(model);

	// Hashes for best performance.
	
	
	if (model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_butterfly.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	}
	else if(model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_falchion_advanced.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}
	else if(model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_css.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			if (random_sequence(1, 2) == 1)
				return sequence;
			else
				return SEQUENCE_CSS_LOOKAT02;
		default:
			return sequence;
		}
	}
	else if(model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_push.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}
	else if(model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_survival_bowie.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}
	else if(model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_skeleton.mdl")) || model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_ursus.mdl")) || model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_outdoor.mdl")) || model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_cord.mdl")) || model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_canis.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	else if(model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_stiletto.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(12, 13);
		default:
			return sequence;
		}
	}
	else if(model_hash == std::hash<std::string>{}(crypt_str("models/weapons/v_knife_widowmaker.mdl")))
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(14, 15);
		default:
			return sequence;
		}
	}
	else 
	{
		return sequence;
	}
		
	
}

static auto do_sequence_remapping(CRecvProxyData* data, IBaseViewModel* entity) -> void
{


	if (!csgo->local)
		return;

	if (!csgo->local->isAlive() || csgo->hudUpdateRequired)
		return;



	auto owner = get_entity_from_handle<IBasePlayer>(entity->GetOwner());

	if (owner != csgo->local)
		return;

	auto view_model_weapon = get_entity_from_handle<IBaseCombatWeapon>(entity->GetViewmodelWeapon());

	if (!view_model_weapon)
		return;



	auto weapon_info = game_data::get_weapon_info(view_model_weapon->GetItemDefinitionIndex());

	if (!weapon_info)
		return;

	int sequence = data->m_Value.m_Int;
	/*
	int originalActivity = view_model_weapon->GetSequenceActivity(sequence);

	int newActivity = 1;
	int num = 0;
	for (size_t i = 0; i < 20; i++) // compare activity with original activity and save new activity // havent seen a sequence above 14 but using 20 if theres is :D
	{
		int tempActitivity = view_model_weapon->GetSequenceActivity(i);
		if (originalActivity != -1 && originalActivity == tempActitivity || originalActivity == -1 && tempActitivity == 213)
		{
			newActivity = i;
			for (size_t t = 0; t < 4; t++)
				if (view_model_weapon->GetSequenceActivity(i + t) == tempActitivity)
					num++;
			break;
		}
	}
	if (originalActivity == -1 && newActivity == 1)
		newActivity = sequence;
	if (!num)
		sequence = newActivity;
	else
		sequence = random_sequence(newActivity, newActivity + num - 1);
		*/
	sequence = get_new_animation(weapon_info->model, sequence);
	data->m_Value.m_Int = sequence;

	//
}



// Replacement function that will be called when the view model animation sequence changes.
void __cdecl sequence_proxy_fn(const CRecvProxyData* proxy_data_const, void* entity, void* output)
{
	// Ensure our other dynamic object hooks are in place.
	// Must do this from a game thread.
	//ensure_dynamic_hooks();

	static auto original_fn = SequenceHook->get_original_function();
	//static auto original_fn = g_sequence_hook->get_original_function();

	// Remove the constness from the proxy data allowing us to make changes.
	auto proxy_data = const_cast<CRecvProxyData*>(proxy_data_const);

	auto view_model = static_cast<IBaseViewModel*>(entity);

	if (!(vars.ragebot.double_tap.properstate()))
		do_sequence_remapping(proxy_data, view_model);

	original_fn(proxy_data_const, entity, output);
	// Call the original function with our edited data.
	//original_fn(proxy_data_const, entity, output);
}
