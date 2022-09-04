#include "Glow.h"
#include "Visuals.h"
#include "Features.h"

void GlowESP::Draw()
{
	for (auto i = 0; i < interfaces.glow_manager->size; i++)
	{
		auto glow_object = &interfaces.glow_manager->m_GlowObjectDefinitions[i];

		IBasePlayer* m_entity = glow_object->m_pEntity;

		if (!glow_object->m_pEntity || glow_object->IsUnused()
			|| m_entity->GetClientClass()->m_ClassID != ClassId->CCSPlayer)
			continue;
		if (!m_entity->isAlive() || m_entity->IsDormant())
			continue;

		if (m_entity == csgo->local)
		{
			if (vars.visuals.local_glow && interfaces.input->m_fCameraInThirdPerson)
			{
				glow_object->m_vGlowcolor_t = Vector(
					vars.visuals.local_glow_clr[0] / 255.f,
					vars.visuals.local_glow_clr[1] / 255.f,
					vars.visuals.local_glow_clr[2] / 255.f
				);
				if (vars.visuals.glowtype == 1)
					glow_object->m_bPulsatingChams = true;
				glow_object->m_flGlowAlpha = vars.visuals.local_glow_clr[3] / 255.f;
				glow_object->m_bRenderWhenOccluded = true;
				glow_object->m_bRenderWhenUnoccluded = false;
				glow_object->m_bFullBloomRender = false;
			}
			else
				continue;
		}

		if (!m_entity->isEnemy(csgo->local))
			continue;

		color_t& pclr = features->Visuals->player_info[m_entity->index()].visible ? vars.visuals.glow_color : vars.visuals.glow_color_occluded;

		if (vars.visuals.glow) {
			glow_object->m_vGlowcolor_t = Vector(
				pclr[0] / 255.f,
				pclr[1] / 255.f,
				pclr[2] / 255.f
			);
			if (vars.visuals.glowtype == 1)
				glow_object->m_bPulsatingChams = true;
			glow_object->m_flGlowAlpha = pclr[3] / 255.f;
			glow_object->m_bRenderWhenOccluded = true;
			glow_object->m_bRenderWhenUnoccluded = false;
			glow_object->m_bFullBloomRender = false;
			continue;
		}
	}
}

GlowESP* glow = new GlowESP();