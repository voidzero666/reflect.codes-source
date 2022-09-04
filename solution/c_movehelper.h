#pragma once
class IMoveHelper
{
public:
	virtual void _vpad() = 0;
	virtual void SetHost(IBasePlayer* host) = 0;
private:
	virtual void pad00() = 0;
	virtual void pad01() = 0;
public:
	virtual void ProcessImpacts() = 0;
};

class CMoveData;
class CGameMovement
{
public:

	virtual			~CGameMovement(void) {}

	virtual void	ProcessMovement(IBasePlayer *pPlayer, CMoveData *pMove) = 0;
	virtual void	Reset(void) = 0;
	virtual void	StartTrackPredictionErrors(IBasePlayer *pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(IBasePlayer *pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	virtual Vector const&	GetPlayerMins(bool ducked) const = 0;
	virtual Vector const&	GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;

	virtual bool			IsMovingPlayerStuck(void) const = 0;
	virtual IBasePlayer*	GetMovingPlayer(void) const = 0;
	virtual void			UnblockPusher(IBasePlayer* pPlayer, IBasePlayer *pPusher) = 0;

	virtual void    SetupMovementBounds(CMoveData *pMove) = 0;

	//virtual			~CGameMovement(void) {}

	//virtual void	ProcessMovement(IBasePlayer *pPlayer, CMoveData *pMove) = 0;
	//virtual void	Reset(void) = 0;
	//virtual void	StartTrackPredictionErrors(IBasePlayer *pPlayer) = 0;
	//virtual void	FinishTrackPredictionErrors(IBasePlayer *pPlayer) = 0;
	//virtual void	DiffPrint(char const *fmt, ...) = 0;

	//virtual Vector const&	GetPlayerMins(bool ducked) const = 0;
	//virtual Vector const&	GetPlayerMaxs(bool ducked) const = 0;
	//virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;

	//virtual bool			IsMovingPlayerStuck(void) const = 0;
	//virtual IBasePlayer*	GetMovingPlayer(void) const = 0;
	//virtual void			UnblockPusher(IBasePlayer* pPlayer, IBasePlayer *pPusher) = 0;

	//virtual void    SetupMovementBounds(CMoveData *pMove) = 0;
};
