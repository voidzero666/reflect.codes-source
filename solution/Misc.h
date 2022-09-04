#pragma once
#include "Hooks.h"
class Misc : public Singleton<Misc> {
public:
	bool in_doubletap = false;
	bool double_tap_enabled = false;
	bool recharging_double_tap = false;
	bool double_tap_key = false;

	int dt_bullets = 0;
	bool IsChatOpened();
	void CopyCommand(CUserCmd* cmd, int tickbase_shift);
	void CalculateVelocityModifierValue();
	void Doubletap();
	void PreserveKillFeed(bool roundStart);
	void FixMovement(CUserCmd* cmd, Vector& ang);
	void UpdateDormantTime();
	void Clantag();
	void ProcessMissedShots();
	void FakeDuck();
	void MouseDelta();
	void StoreTaserRange();
	void Ragdolls();
	void Blockbot(CUserCmd* cmd);
	void RevealRanks(CUserCmd* cmd);
	void votestart(const void* data, int size);
	void modelchanger(ClientFrameStage_t stage);
};