#pragma once
#include "VMT_Manager.h"
#include "cMainStruct.h"

class IBasePlayer;

class CDormantEsp : public Singleton<CDormantEsp>
{
public:
	void Start();

	bool AdjustSound(IBasePlayer* player);
	void StartAdjust(IBasePlayer* player, SndInfo_t& sound);
	bool IsValidSound(SndInfo_t& sound);

	struct SoundPlayer
	{
		void reset(bool store_data = false, const Vector& origin = Vector(), int flags = 0)
		{
			if (store_data)
			{
				m_iReceiveTime = interfaces.global_vars->realtime;
				m_vecOrigin = origin;
				m_nFlags = flags;
			}
			else
			{
				m_iReceiveTime = 0.0f;
				m_vecOrigin.Zero();
				m_nFlags = 0;
			}
		}

		void Override(SndInfo_t& sound)
		{
			m_iReceiveTime = interfaces.global_vars->realtime;
			m_vecOrigin = *sound.m_pOrigin;
		}

		float m_iReceiveTime = 0.0f;
		Vector m_vecOrigin = Vector();
		int m_nFlags = 0;
	} m_cSoundPlayers[65];

	CUtlVector <SndInfo_t> m_utlvecSoundBuffer;
	CUtlVector <SndInfo_t> m_utlCurSoundList;
};