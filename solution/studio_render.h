#pragma once
#include "VMT_Manager.h"
class IMaterial;

enum class OverrideType {
	Normal = 0,
	BuildShadows,
	DepthWrite,
	CustomMaterial, // weapon skins
	SsaoDepthWrite
};

class IStudioRender
{

	std::byte pad_0[592];
	IMaterial* materialOverride;
	std::byte pad_1[12];
	OverrideType overrideType;
public:


	void forcedMaterialOverride(IMaterial* material, OverrideType type = OverrideType::Normal, int index = -1)
	{
		using original_fn = void(__thiscall*)(IStudioRender*, IMaterial*, OverrideType, int);
		(*(original_fn**)this)[33](this, material, type, index);
	}


	bool isForcedMaterialOverride() noexcept
	{
		if (!materialOverride)
			return overrideType == OverrideType::DepthWrite || overrideType == OverrideType::SsaoDepthWrite; // see CStudioRenderContext::IsForcedMaterialOverride
		return std::string_view{ materialOverride->GetName()}._Starts_with(crypt_str("dev/glow"));
	}

};