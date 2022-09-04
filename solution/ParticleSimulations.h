#pragma once
#include "Hooks.h"
#include <array>
#include <string>
#include <xhash>
#include "ParticleCollection.h"

std::array<size_t,9> mollyNames =
{
	std::hash<std::string>{}(crypt_str("explosion_molotov_air")),
	std::hash<std::string>{}(crypt_str("extinguish_fire")),
	std::hash<std::string>{}(crypt_str("molotov_groundfire")),
	std::hash<std::string>{}(crypt_str("molotov_groundfire_fallback")),
	std::hash<std::string>{}(crypt_str("molotov_groundfire_fallback2")),
	std::hash<std::string>{}(crypt_str("molotov_explosion")),
	std::hash<std::string>{}(crypt_str("weapon_molotov_held")),
	std::hash<std::string>{}(crypt_str("weapon_molotov_fp")),
	std::hash<std::string>{}(crypt_str("weapon_molotov_thrown"))
};

std::array<size_t, 2> smokenames =
{
	std::hash<std::string>{}(crypt_str("explosion_smokegrenade")),
	std::hash<std::string>{}(crypt_str("explosion_smokegrenade_fallback"))
};

std::array<size_t, 4> bloodnames =
{
	std::hash<std::string>{}(crypt_str("blood_impact_light")),
	std::hash<std::string>{}(crypt_str("blood_impact_medium")),
	std::hash<std::string>{}(crypt_str("blood_impact_heavy")),
	std::hash<std::string>{}(crypt_str("blood_impact_light_headshot"))
};


void __fastcall Hooked_ParticleSimulations(CParticleCollection* thisptr, void* edx)
{
	// call original
	auto original = dtParticleSimulations.getOriginal<void>();

	original(thisptr);

	if (!vars.visuals.editparticles)
		return;

	CParticleCollection* root = thisptr;

	while (root->m_parent)
		root = root->m_parent;

	std::string namering = root->m_def.m_obj->m_name.m_buffer;
	size_t name = std::hash<std::string>{}(namering);

	
	if (!vars.visuals.remove[1])
	{
		if (auto itr = std::find(smokenames.cbegin(), smokenames.cend(), name); itr != smokenames.cend())
		{
			for (size_t i = 0; i < thisptr->m_activeParticles; i++)
			{
				thisptr->m_particleAttributes.modulateAlpha(vars.visuals.smoke_alpha, i);
			}
		}
	}
	

	if (auto itr = std::find(bloodnames.cbegin(), bloodnames.cend(), name); itr != bloodnames.cend())
	{
		for (size_t i = 0; i < thisptr->m_activeParticles; i++)
		{
			thisptr->m_particleAttributes.modulateColor(vars.visuals.blood_particle_color, i);
		}
	}

	if (auto itr = std::find(mollyNames.cbegin(), mollyNames.cend(), name); itr != mollyNames.cend())
	{
		for (size_t i = 0; i < thisptr->m_activeParticles; i++)
		{
			thisptr->m_particleAttributes.modulateColor(vars.visuals.molotov_particle_color, i);
		}
	}

}