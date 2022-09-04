#pragma once
#include "Vector.h"
#include "i_base_player.h"
#include "c_usercmd.h"
#include <rpcndr.h>
class IMoveHelper;
class CMoveData
{
public:
	bool    m_bFirstRunOfFunctions : 1;
	bool    m_bGameCodeMovedPlayer : 1;
	int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
	int     m_nImpulseCommand;      // Impulse command issued.
	Vector  m_vecViewAngles;        // Command view angles (local space)
	Vector  m_vecAbsViewAngles;     // Command view angles (world space)
	int     m_nButtons;             // Attack buttons.
	int     m_nOldButtons;          // From host_client->oldbuttons;
	float   m_flForwardMove;
	float   m_flSideMove;
	float   m_flUpMove;
	float   m_flMaxSpeed;
	float   m_flClientMaxSpeed;
	Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
	Vector  m_vecAngles;            // edict::angles
	Vector  m_vecOldAngles;
	float   m_outStepHeight;        // how much you climbed this move
	Vector  m_outWishVel;           // This is where you tried 
	Vector  m_outJumpVel;           // This is your jump velocity
	Vector  m_vecConstraintCenter;
	float   m_flConstraintRadius;
	float   m_flConstraintWidth;
	float   m_flConstraintSpeedFactor;
	float   m_flUnknown[5];
	Vector  m_vecAbsOrigin;
};
class CPrediction
{
public:
	std::byte	pad0[0x4];
	std::uintptr_t hLastGround;
	bool		InPrediction;		
	bool		IsFirstTimePredicted;
	bool		EnginePaused;		
	bool			bOldCLPredictValue;				// 0x000B
	int				iPreviousStartFrame;			// 0x000C
	int				nIncomingPacketNumber;			// 0x0010
	float			flLastServerWorldTimeStamp;		// 0x0014

	struct Split_t
	{
		bool		bIsFirstTimePredicted;			// 0x0018
		std::byte	pad0[0x3];						// 0x0019
		int			nCommandsPredicted;				// 0x001C
		int			nServerCommandsAcknowledged;	// 0x0020
		int			iPreviousAckHadErrors;			// 0x0024
		float		flIdealPitch;					// 0x0028
		int			iLastCommandAcknowledged;		// 0x002C
		bool		bPreviousAckErrorTriggersFullLatchReset; // 0x0030
		CUtlVector<std::uintptr_t> vecEntitiesWithPredictionErrorsInLastAck; // 0x0031
		bool		bPerformedTickShift;			// 0x0045
	};

	Split_t			Split[1];						// 0x0018

	void Update(int start_frame, bool valid_frame, int inc_ack, int out_cmd)
	{
		typedef void(__thiscall* oUpdate)(void*, int, bool, int, int);
		return getvfunc<oUpdate>(this, 3)(this, start_frame, valid_frame, inc_ack, out_cmd);
	}

	void CheckMovingGround(IBasePlayer* player, double frametime)
	{
		typedef void(__thiscall* oCheckMovingGround)(void*, IBasePlayer*, double);
		return getvfunc<oCheckMovingGround>(this, 18)(this, player, frametime);
	}

	void RunCommand(IBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
	{
		typedef void(__thiscall* oRunCommand)(void*, IBasePlayer*, CUserCmd*, IMoveHelper*);
		return getvfunc<oRunCommand>(this, 19)(this, player, ucmd, moveHelper);
	}

	void SetupMove(IBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper, void* pMoveData)
	{
		typedef void(__thiscall* oSetupMove)(void*, IBasePlayer*, CUserCmd*, IMoveHelper*, void*);
		return getvfunc<oSetupMove>(this, 20)(this, player, ucmd, moveHelper, pMoveData);
	}

	void FinishMove(IBasePlayer* player, CUserCmd* ucmd, void* pMoveData)
	{
		typedef void(__thiscall* oFinishMove)(void*, IBasePlayer*, CUserCmd*, void*);
		return getvfunc<oFinishMove>(this, 21)(this, player, ucmd, pMoveData);
	}

	void SetLocalViewAngles(Vector& Angles)
	{
		typedef void(__thiscall* Fn)(void*, Vector&);
		return getvfunc<Fn>(this, 13)(this, Angles);
	}

};
