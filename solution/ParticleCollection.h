#pragma once
#include "colors.h"
#include "UtlString.h"
#include "UtlReference.h"

#define PARTICLE_ATTRIBUTE_TINT_RGB 6
#define PARTICLE_ATTRIBUTE_ALPHA	7

#define MAX_PARTICLE_ATTRIBUTES 24

struct CParticleAttributeAddressTable
{
	float* m_pAttributes[MAX_PARTICLE_ATTRIBUTES];
	size_t m_nFloatStrides[MAX_PARTICLE_ATTRIBUTES];

	float* floatAttribute(int attribute, int particleNum) const
	{
		int blockofs = particleNum / 4;
		return m_pAttributes[attribute] +
			m_nFloatStrides[attribute] * blockofs +
			(particleNum & 3);
	}
	// custom
	void modulateColor(const color_t& color, int num)
	{
		/*
		0 : 0.948685
		1 : 0.942262
		2 : 0.898302
		3 : 0.932009
		4 : 0.885635
		5 : 0.878489
		6 : 0.717303
		7 : 0.840896
		8 : 0.670859
		*/

		auto rgb = floatAttribute(PARTICLE_ATTRIBUTE_TINT_RGB, num);
		//auto a = floatAttribute(PARTICLE_ATTRIBUTE_ALPHA, num);


		rgb[0] = (float)color.get_red() / 255.f;
		rgb[4] = (float)color.get_green() / 255.f;
		rgb[8] = (float)color.get_blue() / 255.f;

		//*a = color.a();
	}
	void modulateAlpha(float alpha, int num)
	{
		auto a = floatAttribute(PARTICLE_ATTRIBUTE_ALPHA, num);
		*a = alpha;
	}
};

class CParticleSystemDefinition
{
public:
	char pad[308];
	CUtlString m_name;
};

class CParticleCollection
{
public:
	char pad[48];
	int m_activeParticles;
	char pad1[12];
	CUtlReference<CParticleSystemDefinition> m_def;
	char pad2[60];
	CParticleCollection* m_parent;
	char pad3[84];
	CParticleAttributeAddressTable m_particleAttributes;
};