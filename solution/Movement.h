#pragma once
#include "Hooks.h"
#include "EnginePrediction.h"
#include "hitsounds.h"


class Movement : public Singleton<Movement>
{
public:
	void Run(CUserCmd* cmd);

	void JumpBug(CUserCmd* cmd);
	void EdgeBug(CUserCmd* cmd);
	void EdgeJump(CUserCmd* cmd);
	void LongJump(CUserCmd* cmd);
	void MiniJump(CUserCmd* cmd);
	void JumpStats(CUserCmd* cmd);
	void JumpStatsBhop(CUserCmd* cmd);
	void EBDetect(CUserCmd* cmd);
	void Slidewalk(CUserCmd* cmd);
	void FastDuck(CUserCmd* cmd);
	void AutoDuck(CUserCmd* cmd);
	void LadderGlide(CUserCmd* cmd);
	void CheckpointCMOVE(CUserCmd* cmd);
	void CheckpointENDSCENE();
	void CheckpointDraw();
	void AutoPixelSurf(CUserCmd* cmd);
	void PixelSurfAlign(CUserCmd* cmd);
};