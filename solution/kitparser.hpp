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
#pragma once
#include <vector>
#include <string>



struct EconItemQualityDefinition {
	int id;
	const char* name;
	unsigned weight;
	bool explicitMatchesOnly;
	bool canSupportSet;
	const char* hexColor;
};

class CEconItemDefinition {
public:


	int getWeaponId()
	{
		typedef int(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 0)(this);
	}

	const char* getItemBaseName()
	{
		typedef const char*(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 2)(this);
	}

	const char* getItemTypeName()
	{
		typedef const char*(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 3)(this);
	}

	const char* getPlayerDisplayModel()
	{
		typedef const char*(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 6)(this);
	}

	const char* getWorldDisplayModel()
	{
		typedef const char*(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 7)(this);
	}

	std::uint8_t getRarity()
	{
		typedef std::uint8_t(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 12)(this);
	}
	
	char pad_0x0000[0x8];
	int32_t m_iItemDefinitionIndex;
	char pad_0x000C[0x40];
	char* m_szHudName;
	char pad_0x0050[0x4];
	char* m_szWeaponType;
	char pad_0x0058[0x4];
	char* m_szWeaponDescription;
	char pad_0x0060[0x34];
	char* m_szViewModel;
	char pad_0x0098[0x4];
	char* m_szWorldModel;
	char* m_szWorldModelDropped;
	char pad_0x001[0x58];
	char* stickersdecalmdl;
	char pad_0x002[0x0C];
	char* stickersdecalmdl1;
	char* inventoryicon;
	char pad_0x003[0xA4];
	char* m_szWeaponName1;
	char pad_0x004[0x14];
	char* m_szWeaponName;
	char pad_0x005[0x8];
	char* m_szWeaponName3;
	char pad_0x01C4[0x27C];
};




class CCStrike15ItemSystem;

template <typename Key, typename Value>
struct Node_t
{
	int previous_id;		//0x0000
	int next_id;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	Key key;				//0x0010
	Value value;			//0x0014
};

template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory;		//0x0000
	int allocation_count;			//0x0004
	int grow_size;					//0x0008
	int start_element;				//0x000C
	int next_available;				//0x0010
	int _unknown;					//0x0014
	int last_element;				//0x0018
}; //Size=0x001C

// could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t
{
	char* buffer;	//0x0000
	int capacity;	//0x0004
	int grow_size;	//0x0008
	int length;		//0x000C
}; //Size=0x0010

struct CPaintKit
{
	int id;						//0x0000

	String_t name;				//0x0004
	String_t description;		//0x0014
	String_t item_name;			//0x0024
	String_t material_name;		//0x0034
	String_t image_inventory;	//0x0044
	String_t pad1;
	String_t sLogoMaterial;
	int bBaseDiffuseOverride;
	int rarity;
	int nStyle;
	int color1;
	int color2;
	int color3;
	int color4;
	int logoColor1;
	int logoColor2;
	int logoColor3;
	int logoColor4;
	float flWearDefault;
	float flWearRemapMin;
	float flWearRemapMax;
	char nFixedSeed;
	char uchPhongExponent;
	char uchPhongAlbedoBoost;
	char uchPhongIntensity;
	float flPatternScale;
	float flPatternOffsetXStart;
	float flPatternOffsetXEnd;
	float flPatternOffsetYStart;
	float flPatternOffsetYEnd;
	float flPatternRotateStart;
	float flPatternRotateEnd;
	float flLogoScale;
	float flLogoOffsetX;
	float flLogoOffsetY;
	float flLogoRotation;
	int bIgnoreWeaponSizeScale;
	int nViewModelExponentOverrideSize;
	int bOnlyFirstMaterial;
	float pearlescent;
	int sVmtPath[4];
	int kvVmtOverrides;
}; //Size=0x00E0

struct CStickerKit
{
	int id;

	int item_rarity;

	String_t name;
	String_t description;
	String_t item_name;
	String_t material_name;
	String_t image_inventory;

	int tournament_event_id;
	int tournament_team_id;
	int tournament_player_id;
	bool is_custom_sticker_material;

	float rotate_end;
	float rotate_start;

	float scale_min;
	float scale_max;

	float wear_min;
	float wear_max;

	String_t image_inventory2;
	String_t image_inventory_large;

	std::uint32_t pad0[4];
};

class CEconItemRarityDefinition
{
public:
	int id;
	int color;
	String_t name;
	String_t loc_key;
	String_t loc_key_weapon;
	String_t loot_list;
	String_t recycle_list;
	String_t drop_sound;
	String_t endmatchitemrevealrarity;
	String_t pointertosomething3;
	void* pointertosomething4;
	void* pointertosomething5;
	char dumb10[8];
};

struct AlternateIconData {
	String_t simpleName;
	String_t largeSimpleName;
	String_t iconURLSmall;
	String_t iconURLLarge;
	PAD(28)
};

class CEconColorDefinition
{
public:
	int xz;
	int xz2;
};

class CEconGraffitiTintDefinition
{
public:
	int xz;
	int xz2;
};

template <typename T>
class MusicVector {
public:
	constexpr T& operator[](int i) noexcept { return memory[i]; };
	constexpr const T& operator[](int i) const noexcept { return memory[i]; };

	T* memory;
	PAD(0xC)
		int itemcount;
};

class CEconMusicDefinition
{
public:
	int id;
	char* name;
	char* desc;
	char* model;
	char* music;
};

class EconItemSetDefinition {
public:
	const char* getLocKey()
	{
		typedef const char*(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 1)(this);
	}

	int getItemCount()
	{
		typedef int(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 4)(this);
	}

	int getItemDef(int index)
	{
		typedef int(__thiscall* p_Function)(void*, int);
		return getvfunc<p_Function>(this, 5)(this, index);
	}

	int getItemPaintKit(int index)
	{
		typedef int(__thiscall* p_Function)(void*, int);
		return getvfunc<p_Function>(this, 6)(this, index);
	}
};

struct ItemListEntry {
	int itemDef;
	int paintKit;
	PAD(20)

	auto weaponId() const noexcept
	{
		return itemDef;
	}
};

class EconLootListDefinition {
public:
	const char* getName()
	{
		typedef const char*(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 0)(this);
	}

	const UtlVector<ItemListEntry>& getLootListContents()
	{
		typedef  UtlVector<ItemListEntry>& (__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 1)(this);
	}
};

class CCStrike15ItemSchema
{
public:
	std::byte pad23232223[0x64];
	UtlMap<int, CEconItemRarityDefinition> rars;
	UtlMap<int, EconItemQualityDefinition> qualities;
	std::byte pad232323[0x48];
	UtlMap<int, CEconItemDefinition*> itemsSorted;
	std::byte pad2323[0x104];
	UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
	std::byte pad2553[0x48];
	UtlMap<int, CPaintKit*> paintKits;
	UtlMap<int, CStickerKit*> stickerKits;
	UtlMap<const char*, CStickerKit*> sticker_kits_namekey;
	int32_t pad6[24];
	UtlMap<const char*, KeyValues*> prefabs;
	UtlVector<CEconColorDefinition*> colors;
	UtlVector<CEconGraffitiTintDefinition*> graffiti_tints;
	int32_t pad7[20];
	MusicVector<CEconMusicDefinition*> music_definitions;

	CEconItemDefinition* getItemDefinitionInterface(int id)
	{
		typedef CEconItemDefinition*(__thiscall* p_Function)(void*,int);
		return getvfunc<p_Function>(this, 4)(this, id);
	}

	int getItemSetCount()
	{
		typedef int(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 28)(this);
	}

	EconItemSetDefinition* getItemSet(int index)
	{
		typedef EconItemSetDefinition*(__thiscall* p_Function)(void*, int);
		return getvfunc<p_Function>(this, 29)(this, index);
	}


	EconLootListDefinition* getLootList(int index)
	{
		typedef EconLootListDefinition* (__thiscall* p_Function)(void*, int);
		return getvfunc<p_Function>(this, 32)(this, index);
	}

	int getLootListCount()
	{
		typedef int(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 34)(this);
	}

	CEconItemDefinition* getItemDefinitionByName(const char* name)
	{
		typedef CEconItemDefinition* (__thiscall* p_Function)(void*, const char*);
		return getvfunc<p_Function>(this, 42)(this, name);
	}
};

class ItemSystem {
public:
	CCStrike15ItemSchema* getItemSchema()
	{
		typedef CCStrike15ItemSchema*(__thiscall* p_Function)(void*);
		return getvfunc<p_Function>(this, 0)(this);
	}
};

namespace game_data
{
	struct paint_kit
	{
		paint_kit(int i, std::string n, CPaintKit* l)
		{
			id = i;
			name = n;
			link = l;
		}
		int id;
		std::string name;
		CPaintKit* link;
		auto operator < (const paint_kit& other) const -> bool
		{
			return name < other.name;
		}


	};

	extern std::vector<paint_kit> skin_kits;
	extern std::vector<paint_kit> glove_kits;
	extern std::vector<paint_kit> sticker_kits;

	inline std::add_pointer_t<ItemSystem* __cdecl()> itemSystem;
	extern auto initialize_kits() -> void;
}
