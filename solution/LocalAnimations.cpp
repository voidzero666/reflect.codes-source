#include "LocalAnimations.hpp"

// hello team is my atempt on animfix thank you

void build_da_matrix(IBasePlayer* pPlayer, matrix* aMatrix, bool bSafeMatrix)
{
	std::array < CAnimationLayer, 13 > aAnimationLayers;

	float_t flCurTime = interfaces.global_vars->curtime;
	float_t flRealTime = interfaces.global_vars->realtime;
	float_t flFrameTime = interfaces.global_vars->framecount;
	float_t flAbsFrameTime = interfaces.global_vars->absoluteframetime;
	int32_t iFrameCount = interfaces.global_vars->framecount;
	int32_t iTickCount = interfaces.global_vars->tickcount;
	float_t flInterpolation = interfaces.global_vars->interpolation_amount;

	interfaces.global_vars->curtime = pPlayer->GetSimulationTime();
	interfaces.global_vars->realtime = pPlayer->GetSimulationTime();
	interfaces.global_vars->framecount = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->absoluteframetime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->framecount = INT_MAX;
	interfaces.global_vars->tickcount = TIME_TO_TICKS(pPlayer->GetSimulationTime());
	interfaces.global_vars->interpolation_amount = 0.0f;

	int32_t nClientEffects = pPlayer->GetEffects();
	int32_t nLastSkipFramecount = pPlayer->LastSkipFramecount();
	int32_t nOcclusionMask = pPlayer->GetOcclusionFlags();
	int32_t nOcclusionFrame = pPlayer->GetOcclusionFramecount();
	int32_t iEffects = pPlayer->GetEffects();
	bool bMaintainSequenceTransition = pPlayer->MaintainSequenceTransition();
	Vector vecAbsOrigin = pPlayer->GetAbsOriginVec();

	int32_t iMask = 0x0007FF00;
	if (bSafeMatrix)
		iMask = 0x00000100;

	std::memcpy(aAnimationLayers.data(), pPlayer->GetAnimOverlays(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);

	pPlayer->InvalidateBoneCache();
	pPlayer->GetBoneAccessor()->m_ReadableBones = NULL;
	pPlayer->GetBoneAccessor()->m_WritableBones = NULL;

	if (pPlayer->GetPlayerAnimState())
		pPlayer->GetPlayerAnimState()->m_pWeaponLast = pPlayer->GetPlayerAnimState()->m_pWeapon;

	pPlayer->GetOcclusionFramecount() = 0;
	pPlayer->GetOcclusionFlags() = 0;
	pPlayer->LastSkipFramecount() = 0;
	if (pPlayer != csgo->local)
		pPlayer->SetAbsOrigin(pPlayer->origin());

	pPlayer->GetEffects() |= NO_INTERP;
	pPlayer->GetClientEffects() |= 2;
	pPlayer->MaintainSequenceTransition() = false;

	pPlayer->GetAnimOverlays()[ANIMATION_LAYER_LEAN].m_flWeight = 0.0f;
	if (bSafeMatrix)
		pPlayer->GetAnimOverlays()[ANIMATION_LAYER_ADJUST].m_pOwner = NULL;
	else if (pPlayer == csgo->local)
	{
		if (pPlayer->GetSequenceActivity(pPlayer->GetAnimOverlays()[ANIMATION_LAYER_ADJUST].m_nSequence) == ACT_CSGO_IDLE_TURN_BALANCEADJUST)
		{
			pPlayer->GetAnimOverlays()[ANIMATION_LAYER_ADJUST].m_flCycle = 0.0f;
			pPlayer->GetAnimOverlays()[ANIMATION_LAYER_ADJUST].m_flWeight = 0.0f;
		}
	}

	csgo->setup_da_bones = true;
	pPlayer->SetupBones(aMatrix, 128, iMask, pPlayer->GetSimulationTime());
	csgo->setup_da_bones = false;

	pPlayer->MaintainSequenceTransition() = bMaintainSequenceTransition;
	pPlayer->GetClientEffects() = nClientEffects;
	pPlayer->GetEffects() = iEffects;
	pPlayer->LastSkipFramecount() = nLastSkipFramecount;
	pPlayer->GetOcclusionFramecount() = nOcclusionFrame;
	pPlayer->GetOcclusionFlags() = nOcclusionMask;

	if (pPlayer != csgo->local)
		pPlayer->SetAbsOrigin(vecAbsOrigin);

	std::memcpy(pPlayer->GetAnimOverlays(), aAnimationLayers.data(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);

	interfaces.global_vars->curtime = flCurTime;
	interfaces.global_vars->realtime = flRealTime;
	interfaces.global_vars->framecount = flFrameTime;
	interfaces.global_vars->absoluteframetime = flAbsFrameTime;
	interfaces.global_vars->framecount = iFrameCount;
	interfaces.global_vars->tickcount = iTickCount;
	interfaces.global_vars->interpolation_amount = flInterpolation;
}

// epoic fuynction no 100000000123221435435345% cpu fixerd by VOIDZERO_HVH_LEGENDE
void normalize_da_angle(float& angle)
{
	float rot;

	// bad number.
	if (!std::isfinite(angle))
	{
		angle = 0.f;
		return;
	}

	// no need to normalize this angle.
	if (angle >= -180.f && angle <= 180.f)
		return;

	// get amount of rotations needed.
	rot = std::round(std::abs(angle / 360.f));

	// normalize.
	angle = (angle < 0.f) ? angle + (360.f * rot) : angle - (360.f * rot);
}

void LocalAnimations::Instance()
{
	float_t flCurtime = interfaces.global_vars->curtime;
	float_t flRealTime = interfaces.global_vars->realtime;
	float_t flAbsFrameTime = interfaces.global_vars->absoluteframetime;
	float_t flFrameTime = interfaces.global_vars->frametime;
	float_t flInterpolationAmount = interfaces.global_vars->interpolation_amount;
	float_t iTickCount = interfaces.global_vars->tickcount;
	float_t iFrameCount = interfaces.global_vars->framecount;

	if (csgo->local->GetSpawnTime() != LocalData.m_flSpawnTime)
	{
		LocalData.m_iFlags[0] = LocalData.m_iFlags[1] = csgo->local->GetFlags();
		LocalData.m_iMoveType[0] = LocalData.m_iMoveType[1] = csgo->local->GetMoveType();
		LocalData.m_flSpawnTime = csgo->local->GetSpawnTime();

		std::memcpy(&LocalData.m_FakeAnimationState, csgo->local->GetPlayerAnimState(), sizeof(CCSGOPlayerAnimState));
		std::memcpy(LocalData.m_FakeAnimationLayers.data(), csgo->local->GetAnimOverlays(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
		std::memcpy(LocalData.m_FakePoseParameters.data(), csgo->local->m_flPoseParameter().data(), sizeof(float_t) * 24);
	}

	int32_t iFlags = csgo->local->GetFlags();
	float_t flLowerBodyYaw = csgo->local->GetLBY();
	float_t flDuckSpeed = csgo->local->GetDuckSpeed();
	float_t flDuckAmount = csgo->local->GetDuckAmount();
	Vector angVisualAngles = csgo->local->GetVisualAngles();

	interfaces.global_vars->curtime = TICKS_TO_TIME(csgo->cmd->tick_count);
	interfaces.global_vars->realtime = TICKS_TO_TIME(csgo->cmd->tick_count);
	interfaces.global_vars->absoluteframetime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->tickcount = csgo->cmd->tick_count;
	interfaces.global_vars->framecount = csgo->cmd->tick_count;
	interfaces.global_vars->interpolation_amount = 0.0f;

	csgo->local->GetAbsVelocity() = csgo->local->GetVelocity();
	csgo->local->GetVisualAngles() = csgo->cmd->viewangles;

	csgo->local->GetThirdpersonRecoil() = csgo->local->GetAimPunchAngle().x * interfaces.cvars->FindVar(str("weapon_recoil_scale"))->GetFloat();

	if (LocalData.m_bDidShotAtChokeCycle)
		if (csgo->send_packet)
			csgo->local->GetVisualAngles() = LocalData.m_angShotChokedAngle;

	//Vector negro = Vector(csgo->local->GetVisualAngles().x, csgo->local->GetVisualAngles().y, 0.0f);
	//csgo->local->GetVisualAngles() = negro;

	csgo->local->GetLBY() = LocalData.m_flLowerBodyYaw;
	if (csgo->local->GetFlags() & FL_FROZEN || csgo->game_rules->IsFreezeTime())
		csgo->local->GetLBY() = flLowerBodyYaw;

	if (csgo->local->GetPlayerAnimState()->m_nLastUpdateFrame > interfaces.global_vars->framecount - 1)
		csgo->local->GetPlayerAnimState()->m_nLastUpdateFrame = interfaces.global_vars->framecount - 1;

	this->DoAnimationEvent(0);
	for (int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++)
		csgo->local->GetAnimOverlays()[iLayer].m_pOwner = csgo->local;

	bool bClientSideAnimation = csgo->local->GetClientSideAnims();
	csgo->local->GetClientSideAnims() = true;

	csgo->ShouldUpdate = true;
	csgo->local->UpdateClientSideAnimation();
	csgo->ShouldUpdate = false;

	csgo->local->GetClientSideAnims() = bClientSideAnimation;

	std::memcpy(LocalData.m_PoseParameters.data(), csgo->local->m_flPoseParameter().data(), sizeof(float_t) * 24);
	std::memcpy(LocalData.GetAnimOverlays.data(), csgo->local->GetAnimOverlays(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);

	if (csgo->local->GetPlayerAnimState()->m_flVelocityLengthXY > 0.1f || fabs(csgo->local->GetPlayerAnimState()->m_flVelocityLengthZ) > 100.0f)
	{
		LocalData.m_flNextLowerBodyYawUpdateTime = flCurtime + 0.22f;
		if (LocalData.m_flLowerBodyYaw != Math::NormalizeYaw(csgo->cmd->viewangles.y))
		{
			auto nigger = csgo->local->GetPlayerAnimState()->m_flEyeYaw;
			normalize_da_angle(nigger);
			LocalData.m_flLowerBodyYaw = csgo->local->GetLBY() = nigger;
		}
	}
	else if (flCurtime > LocalData.m_flNextLowerBodyYawUpdateTime)
	{
		auto jew = csgo->local->GetPlayerAnimState()->m_flFootYaw;
		normalize_da_angle(jew);

		auto george_floyd_gaming = csgo->cmd->viewangles.y;
		normalize_da_angle(george_floyd_gaming);

		float_t flAngleDifference = Math::AngleDiff(jew, george_floyd_gaming);

		if (fabsf(flAngleDifference) > 35.0f)
		{
			LocalData.m_flNextLowerBodyYawUpdateTime = flCurtime + 1.1f;
			auto fuckingnigger = csgo->cmd->viewangles.y;
			normalize_da_angle(fuckingnigger);
			if (LocalData.m_flLowerBodyYaw != fuckingnigger)
				LocalData.m_flLowerBodyYaw = csgo->local->GetLBY() = fuckingnigger;
		}
	}

	csgo->local->GetFlagsPtr() = iFlags;
	csgo->local->GetDuckAmount() = flDuckAmount;
	csgo->local->GetDuckSpeed() = flDuckSpeed;
	csgo->local->GetLBY() = flLowerBodyYaw;
	csgo->local->GetVisualAngles() = angVisualAngles;

	if (csgo->send_packet)
	{
		CCSGOPlayerAnimState AnimationState;
		std::memcpy(&AnimationState, csgo->local->GetPlayerAnimState(), sizeof(CCSGOPlayerAnimState));

		bool bShouldSetupMatrix = true;

		if (bShouldSetupMatrix)
			build_da_matrix(csgo->local, LocalData.m_aMainBones.data(), false);

		std::memcpy(csgo->local->GetAnimOverlays(), GetFakeAnimationLayers().data(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
		std::memcpy(csgo->local->GetPlayerAnimState(), &LocalData.m_FakeAnimationState, sizeof(CCSGOPlayerAnimState));
		std::memcpy(csgo->local->m_flPoseParameter().data(), LocalData.m_FakePoseParameters.data(), sizeof(float_t) * 24);

		int32_t iSimulationTicks = csgo->client_state->iChokedCommands + 1;
		for (int32_t iSimulationTick = 1; iSimulationTick <= iSimulationTicks; iSimulationTick++)
		{
			int32_t iTickCount = csgo->cmd->tick_count - (iSimulationTicks - iSimulationTick);
			interfaces.global_vars->curtime = TICKS_TO_TIME(iTickCount);
			interfaces.global_vars->realtime = TICKS_TO_TIME(iTickCount);
			interfaces.global_vars->absoluteframetime = interfaces.global_vars->interval_per_tick;
			interfaces.global_vars->framecount = interfaces.global_vars->interval_per_tick;
			interfaces.global_vars->tickcount = iTickCount;
			interfaces.global_vars->framecount = iTickCount;

			csgo->local->GetAbsVelocity() = csgo->local->GetVelocity();
			csgo->local->GetThirdpersonRecoil() = csgo->local->GetAimPunchAngle().x * interfaces.cvars->FindVar(str("weapon_recoil_scale"))->GetFloat();

			csgo->local->GetVisualAngles() = csgo->FakeAngle;
			if ((iSimulationTicks - iSimulationTick) < 1)
			{
				if (LocalData.m_bDidShotAtChokeCycle)
					csgo->local->GetVisualAngles() = LocalData.m_angShotChokedAngle;

				//Vector negro2 = Vector(csgo->local->GetVisualAngles().x, csgo->local->GetVisualAngles().y, 0.0f);
				//csgo->local->GetVisualAngles() = negro2;
			}

			if (csgo->local->GetPlayerAnimState()->m_nLastUpdateFrame == interfaces.global_vars->framecount)
				csgo->local->GetPlayerAnimState()->m_nLastUpdateFrame = interfaces.global_vars->framecount - 1;

			this->DoAnimationEvent(1);
			for (int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++)
				csgo->local->GetAnimOverlays()[iLayer].m_pOwner = csgo->local;

			bool bClientSideAnimation = csgo->local->GetClientSideAnims();
			csgo->local->GetClientSideAnims() = true;

			csgo->ShouldUpdate = true;
			csgo->local->UpdateClientSideAnimation();
			csgo->ShouldUpdate = false;

			csgo->local->GetClientSideAnims() = bClientSideAnimation;
		}

		// build desync matrix
		build_da_matrix(csgo->local, LocalData.m_aDesyncBones.data(), false);

		// copy lag matrix
		std::memcpy(LocalData.m_aLagBones.data(), LocalData.m_aDesyncBones.data(), sizeof(matrix) * 128);

		std::memcpy(&LocalData.m_FakeAnimationState, csgo->local->GetPlayerAnimState(), sizeof(CCSGOPlayerAnimState));
		std::memcpy(LocalData.m_FakeAnimationLayers.data(), csgo->local->GetAnimOverlays(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
		std::memcpy(LocalData.m_FakePoseParameters.data(), csgo->local->m_flPoseParameter().data(), sizeof(float_t) * 24);

		std::memcpy(csgo->local->GetAnimOverlays(), GetAnimationLayers().data(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
		std::memcpy(csgo->local->GetPlayerAnimState(), &AnimationState, sizeof(CCSGOPlayerAnimState));
		std::memcpy(csgo->local->m_flPoseParameter().data(), LocalData.m_PoseParameters.data(), sizeof(float_t) * 24);

		for (int i = 0; i < 128; i++)
			LocalData.m_vecBoneOrigins[i] = csgo->local->GetAbsOrigin() - LocalData.m_aMainBones[i].GetOrigin();

		for (int i = 0; i < 128; i++)
			LocalData.m_vecFakeBoneOrigins[i] = csgo->local->GetAbsOrigin() - LocalData.m_aDesyncBones[i].GetOrigin();

		LocalData.m_bDidShotAtChokeCycle = false;
		LocalData.m_angShotChokedAngle = Vector(0, 0, 0);
	}

	csgo->local->GetFlagsPtr() = iFlags;
	csgo->local->GetDuckAmount() = flDuckAmount;
	csgo->local->GetDuckSpeed() = flDuckSpeed;
	csgo->local->GetLBY() = flLowerBodyYaw;
	csgo->local->GetVisualAngles() = angVisualAngles;

	interfaces.global_vars->curtime = flCurtime;
	interfaces.global_vars->realtime = flRealTime;
	interfaces.global_vars->absoluteframetime = flAbsFrameTime;
	interfaces.global_vars->framecount = flFrameTime;
	interfaces.global_vars->tickcount = iTickCount;
	interfaces.global_vars->framecount = iFrameCount;
	interfaces.global_vars->interpolation_amount = flInterpolationAmount;
}

void LocalAnimations::SetupShootPosition()
{
	std::memcpy(csgo->local->GetAnimOverlays(), g_LocalAnimations->GetAnimationLayers().data(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
	std::memcpy(csgo->local->m_flPoseParameter().data(), LocalData.m_PoseParameters.data(), sizeof(float_t) * 24);

	//float flOldBodyPitch = csgo->local->m_flPoseParameter()[12];
	Vector vecOldOrigin = csgo->local->GetAbsOrigin();

	csgo->local->SetAbsAngles(Vector(0.0f, csgo->local->GetPlayerAnimState()->m_flFootYaw, 0.0f));
	csgo->local->SetAbsAngles(csgo->local->origin());

	matrix aMatrix[128];

//	csgo->local->m_flPoseParameter()[12] = (csgo->local->GetEyeAngles().x + 89.0f) / 178.0f;
	build_da_matrix(csgo->local, aMatrix, true);
	//csgo->local->m_flPoseParameter()[12] = flOldBodyPitch;

	csgo->local->SetAbsAngles(vecOldOrigin);
	std::memcpy(csgo->local->GetBoneCache().Base(), aMatrix, sizeof(matrix) * csgo->local->GetBoneCache().Count());

	csgo->local->ForceBoneCache();
	LocalData.m_vecShootPosition = csgo->local->GetShootPosition();
}

bool LocalAnimations::GetCachedMatrix(matrix* aMatrix)
{
	std::memcpy(aMatrix, LocalData.m_aMainBones.data(), sizeof(matrix) * csgo->local->GetBoneCache().Count());
	return true;
}

std::array < matrix, 128 > LocalAnimations::GetDesyncMatrix()
{
	return LocalData.m_aDesyncBones;
}

std::array < matrix, 128 > LocalAnimations::GetLagMatrix()
{
	return LocalData.m_aLagBones;
}

void LocalAnimations::DoAnimationEvent(int type)
{
	if (csgo->game_rules->IsFreezeTime() || (csgo->local->GetFlags() & FL_FROZEN))
	{
		LocalData.m_iMoveType[type] = MOVETYPE_NONE;
		LocalData.m_iFlags[type] = FL_ONGROUND;
	}

	CAnimationLayer* pLandOrClimbLayer = &csgo->local->GetAnimOverlays()[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];
	if (!pLandOrClimbLayer)
		return;

	CAnimationLayer* pJumpOrFallLayer = &csgo->local->GetAnimOverlays()[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
	if (!pJumpOrFallLayer)
		return;

	if (LocalData.m_iMoveType[type] != MOVETYPE_LADDER && csgo->local->GetMoveType() == MOVETYPE_LADDER)
		csgo->local->GetPlayerAnimState()->SetLayerSequence(pLandOrClimbLayer, ACT_CSGO_CLIMB_LADDER);
	else if (LocalData.m_iMoveType[type] == MOVETYPE_LADDER && csgo->local->GetMoveType() != MOVETYPE_LADDER)
		csgo->local->GetPlayerAnimState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_FALL);
	else
	{
		if (csgo->local->GetFlags() & FL_ONGROUND)
		{
			if (!(LocalData.m_iFlags[type] & FL_ONGROUND))
				csgo->local->GetPlayerAnimState()->SetLayerSequence(pLandOrClimbLayer, csgo->local->GetPlayerAnimState()->m_flDurationInAir > 1.0f && type == 0 ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT);
		}
		else if (LocalData.m_iFlags[type] & FL_ONGROUND)
		{
			if (csgo->local->GetVelocity().z > 0.0f)
				csgo->local->GetPlayerAnimState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_JUMP);
			else
				csgo->local->GetPlayerAnimState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_FALL);
		}
	}

	LocalData.m_iMoveType[type] = csgo->local->GetMoveType();
	LocalData.m_iFlags[type] = csgo->local->GetFlags();
}

void LocalAnimations::OnUpdateClientSideAnimation()
{
	for (int i = 0; i < 128; i++)
		LocalData.m_aMainBones[i].SetOrigin(csgo->local->GetAbsOrigin() - LocalData.m_vecBoneOrigins[i]);

	for (int i = 0; i < 128; i++)
		LocalData.m_aDesyncBones[i].SetOrigin(csgo->local->GetAbsOrigin() - LocalData.m_vecFakeBoneOrigins[i]);

	std::memcpy(csgo->local->GetBoneCache().Base(), LocalData.m_aMainBones.data(), sizeof(matrix) * csgo->local->GetBoneCache().Count());
	std::memcpy(csgo->local->GetBoneAccessor()->get_bone_array_for_write(), LocalData.m_aMainBones.data(), sizeof(matrix) * csgo->local->GetBoneCache().Count());

	return csgo->local->AttachmentHelper();
}

std::array< CAnimationLayer, 13 > LocalAnimations::GetAnimationLayers()
{
	std::array< CAnimationLayer, 13 > aOutput;

	std::memcpy(aOutput.data(), csgo->local->GetAnimOverlays(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
	std::memcpy(&aOutput.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), &LocalData.GetAnimOverlays.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), sizeof(CAnimationLayer));
	std::memcpy(&aOutput.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), &LocalData.GetAnimOverlays.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), sizeof(CAnimationLayer));
	std::memcpy(&aOutput.at(ANIMATION_LAYER_ALIVELOOP), &LocalData.GetAnimOverlays.at(ANIMATION_LAYER_ALIVELOOP), sizeof(CAnimationLayer));
	std::memcpy(&aOutput.at(ANIMATION_LAYER_LEAN), &LocalData.GetAnimOverlays.at(ANIMATION_LAYER_LEAN), sizeof(CAnimationLayer));

	return aOutput;
}

std::array< CAnimationLayer, 13 > LocalAnimations::GetFakeAnimationLayers()
{
	std::array< CAnimationLayer, 13 > aOutput;

	std::memcpy(aOutput.data(), csgo->local->GetAnimOverlays(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
	std::memcpy(&aOutput.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), &LocalData.m_FakeAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), sizeof(CAnimationLayer));
	std::memcpy(&aOutput.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), &LocalData.m_FakeAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), sizeof(CAnimationLayer));
	std::memcpy(&aOutput.at(ANIMATION_LAYER_ALIVELOOP), &LocalData.m_FakeAnimationLayers.at(ANIMATION_LAYER_ALIVELOOP), sizeof(CAnimationLayer));
	std::memcpy(&aOutput.at(ANIMATION_LAYER_LEAN), &LocalData.m_FakeAnimationLayers.at(ANIMATION_LAYER_LEAN), sizeof(CAnimationLayer));

	return aOutput;
}

void LocalAnimations::ResetData()
{
	LocalData.m_aDesyncBones = { };
	LocalData.m_aMainBones = { };

	LocalData.m_vecNetworkedOrigin = Vector(0, 0, 0);
	LocalData.m_angShotChokedAngle = Vector(0, 0, 0);
	LocalData.m_vecBoneOrigins.fill(Vector(0, 0, 0));
	LocalData.m_vecFakeBoneOrigins.fill(Vector(0, 0, 0));

	LocalData.m_bDidShotAtChokeCycle = false;

	LocalData.GetAnimOverlays.fill(CAnimationLayer());
	LocalData.m_FakeAnimationLayers.fill(CAnimationLayer());

	LocalData.m_PoseParameters.fill(0.0f);
	LocalData.m_FakePoseParameters.fill(0.0f);

	LocalData.m_flShotTime = 0.0f;
	LocalData.m_angForcedAngles = Vector(0, 0, 0);

	LocalData.m_flLowerBodyYaw = 0.0f;
	LocalData.m_flNextLowerBodyYawUpdateTime = 0.0f;
	LocalData.m_flSpawnTime = 0.0f;

	LocalData.m_iFlags[0] = LocalData.m_iFlags[0] = 0;
	LocalData.m_iMoveType[0] = LocalData.m_iMoveType[1] = 0;
}