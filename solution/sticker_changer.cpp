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
#include <cstring>
#include "Hooks.h"
#include "netvar_manager.h"
#include "config_.hpp"

config g_config;

enum class EStickerAttributeType
{
	Index,
	Wear,
	Scale,
	Rotation
};

static auto s_econ_item_interface_wrapper_offset = std::uint16_t(0);

struct GetStickerAttributeBySlotIndexFloat
{
	static auto __fastcall hooked(void* thisptr, void*, const int slot,
		const EStickerAttributeType attribute, const float unknown) -> float
	{
		auto item = reinterpret_cast<IBaseAttributableItem*>(std::uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

		const auto defindex = item->GetItemDefinitionIndex();

		auto config = g_config.get_by_definition_index(defindex);

		if (config)
		{
			switch (attribute)
			{
			case EStickerAttributeType::Wear:
				return config->stickers.at(slot).wear;
			case EStickerAttributeType::Scale:
				return config->stickers.at(slot).scale;
			case EStickerAttributeType::Rotation:
				return config->stickers.at(slot).rotation;
			default:
				break;
			}
		}

		return m_original(thisptr, nullptr, slot, attribute, unknown);
	}

	static decltype(&hooked) m_original;
};

decltype(GetStickerAttributeBySlotIndexFloat::m_original) GetStickerAttributeBySlotIndexFloat::m_original;

struct GetStickerAttributeBySlotIndexInt
{
	static auto __fastcall hooked(void* thisptr, void*, const int slot,
		const EStickerAttributeType attribute, const int unknown) -> int
	{
		auto item = reinterpret_cast<IBaseAttributableItem*>(std::uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

		if (attribute == EStickerAttributeType::Index)
		{
			const auto defindex = item->GetItemDefinitionIndex();

			auto config = g_config.get_by_definition_index(defindex);

			if (config)
				return config->stickers.at(slot).kit;
		}

		return m_original(thisptr, nullptr, slot, attribute, unknown);
	}

	static decltype(&hooked) m_original;
};

decltype(GetStickerAttributeBySlotIndexInt::m_original) GetStickerAttributeBySlotIndexInt::m_original;

auto apply_sticker_changer(IBaseAttributableItem* item) -> void
{
	if (!s_econ_item_interface_wrapper_offset)
		s_econ_item_interface_wrapper_offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_Item")) + 0xC;	

	static MinHook hook;

	const auto econ_item_interface_wrapper = std::uintptr_t(item) + s_econ_item_interface_wrapper_offset;

	hook.init(reinterpret_cast<void*>(econ_item_interface_wrapper));
	hook.hookAt(4, GetStickerAttributeBySlotIndexFloat::hooked);
	hook.hookAt(5, GetStickerAttributeBySlotIndexInt::hooked);
	/*
	if (hook.initialize_and_hook_instance(reinterpret_cast<void*>(econ_item_interface_wrapper)))
	{
		hook.apply_hook<GetStickerAttributeBySlotIndexFloat>(4);
		hook.apply_hook<GetStickerAttributeBySlotIndexInt>(5);
	}
	*/
}