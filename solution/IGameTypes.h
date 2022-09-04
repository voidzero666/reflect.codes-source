#pragma once
#include "common.h"

enum EGameType : int
{
	GAMETYPE_UNKNOWN = -1,
	GAMETYPE_CLASSIC,
	GAMETYPE_GUNGAME,
	GAMETYPE_TRAINING,
	GAMETYPE_CUSTOM,
	GAMETYPE_COOPERATIVE,
	GAMETYPE_SKIRMISH,
	GAMETYPE_FREEFORALL
};

enum EGameMode : int
{
	GAMEMODE_UNKNOWN = 0,
	GAMEMODE_CASUAL,
	GAMEMODE_COMPETITIVE,
	GAMEMODE_WINGMAN,
	GAMEMODE_ARMSRACE,
	GAMEMODE_DEMOLITION,
	GAMEMODE_DEATHMATCH,
	GAMEMODE_GUARDIAN,
	GAMEMODE_COOPSTRIKE,
	GAMEMODE_DANGERZONE
};

namespace sdk
{
	class IGameTypes
	{
	public:

		
		int GetCurrentGameType()
		{
			typedef int(__thiscall* OriginalFn)(void*);
			return getvfunc< OriginalFn >(this, 8)(this);
		}

		int GetCurrentGameMode()
		{
			typedef int(__thiscall* OriginalFn)(void*);
			return getvfunc< OriginalFn >(this, 9)(this);
		}

		const char* GetCurrentMapName()
		{
			typedef const char*(__thiscall* OriginalFn)(void*);
			return getvfunc< OriginalFn >(this, 10)(this);
		}

		const char* GetCurrentGameTypeNameID()
		{
			typedef const char* (__thiscall* OriginalFn)(void*);
			return getvfunc< OriginalFn >(this, 11)(this);
		}

		const char* GetCurrentGameModeNameID()
		{
			typedef const char* (__thiscall* OriginalFn)(void*);
			return getvfunc< OriginalFn >(this, 13)(this);
		}

		int getrealgamemode()
		{
			int game_type = GetCurrentGameType();
			int game_mode = GetCurrentGameMode();

			switch (game_type)
			{
			case 0:
				switch (game_mode)
				{
				case 0:
					return GAMEMODE_CASUAL;
				case 1:
					return GAMEMODE_COMPETITIVE;
				case 2:
					return GAMEMODE_WINGMAN;
				default:
					return GAMEMODE_UNKNOWN;
				}
				return GAMEMODE_UNKNOWN;
			case 1:
				switch (game_mode)
				{
				case 0:
					return GAMEMODE_ARMSRACE;
				case 1:
					return GAMEMODE_DEMOLITION;
				case 2:
					return GAMEMODE_DEATHMATCH;
				default:
					return GAMEMODE_UNKNOWN;
				}
				return GAMEMODE_UNKNOWN;
			case 4:
				switch (game_mode)
				{
				case 0:
					return GAMEMODE_GUARDIAN;
				case 1:
					return GAMEMODE_COOPSTRIKE;
				default:
					return GAMEMODE_UNKNOWN;
				}
				return GAMEMODE_UNKNOWN;
			case 6:
				switch (game_mode)
				{
				case 0:
					return GAMEMODE_DANGERZONE;
				default:
					return GAMEMODE_UNKNOWN;
				}
				return GAMEMODE_UNKNOWN;
			default:
				return GAMEMODE_UNKNOWN;
			}
			return GAMEMODE_UNKNOWN;
		}
	};
}