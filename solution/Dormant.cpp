#include "Dormant.h"
#include "Hooks.h"

void CDormantEsp::Start()
{
	m_utlCurSoundList.RemoveAll();
	interfaces.engine_sound->GetActiveSounds(m_utlCurSoundList);

	if (!m_utlCurSoundList.Count())
		return;

	if (!csgo->local)
		return;

	for (auto i = 0; i < m_utlCurSoundList.Count(); i++)
	{
		auto& sound = m_utlCurSoundList[i];

		if (sound.m_nSoundSource < 1 || sound.m_nSoundSource > 64)
			continue;

		if (sound.m_pOrigin->IsZero())
			continue;

		if (!IsValidSound(sound))
			continue;

		auto player = interfaces.ent_list->GetClientEntity(sound.m_nSoundSource);

		if (!player
			|| !player->isAlive()
			|| !player->IsPlayer()
			|| player->EntIndex() == interfaces.engine->GetLocalPlayer()
			|| !player->isEnemy(csgo->local))
			continue;

		StartAdjust(player, sound);
		m_cSoundPlayers[sound.m_nSoundSource].Override(sound);
	}

	m_utlvecSoundBuffer = m_utlCurSoundList;
}

void CDormantEsp::StartAdjust(IBasePlayer* player, SndInfo_t& sound)
{
	Vector src3D, dst3D;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	src3D = *sound.m_pOrigin + Vector(0.0f, 0.0f, 1.0f);
	dst3D = src3D - Vector(0.0f, 0.0f, 100.0f);

	filter.pSkip = player;
	ray.Init(src3D, dst3D);

	interfaces.trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	if (tr.allsolid)
		m_cSoundPlayers[sound.m_nSoundSource].m_iReceiveTime = -1;

	*sound.m_pOrigin = tr.fraction <= 0.97f ? tr.endpos : *sound.m_pOrigin;

	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags = player->GetFlags();
	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags |= (tr.fraction < 0.50f ? FL_DUCKING : 0) | (tr.fraction < 1.0f ? FL_ONGROUND : 0);
	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags &= (tr.fraction >= 0.50f ? ~FL_DUCKING : 0) | (tr.fraction >= 1.0f ? ~FL_ONGROUND : 0);
}

bool CDormantEsp::AdjustSound(IBasePlayer* entity)
{
	auto i = entity->EntIndex();
	auto sound_player = m_cSoundPlayers[i];

	auto expired = false;

	if (fabs(interfaces.global_vars->realtime - sound_player.m_iReceiveTime) > 10.0f)
		expired = true;

	entity->TargetSpotted() = true;
	entity->GetFlagsPtr() = sound_player.m_nFlags;
	entity->SetAbsOrigin(sound_player.m_vecOrigin);

	csgo->LastSeenTime[i] = csgo->get_absolute_time();

	return !expired;
}

bool CDormantEsp::IsValidSound(SndInfo_t& sound)
{
	for (auto i = 0; i < m_utlvecSoundBuffer.Count(); i++)
		if (m_utlvecSoundBuffer[i].m_nGuid == sound.m_nGuid)
			return false;

	return true;
}