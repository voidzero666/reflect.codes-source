#include "Hooks.h"
#include "AnimationFix.h"
#include "RageBackTracking.h"
#include "Resolver.h"
#include "Ragebot.h"
#include "AntiAims.h"

CMAnimationFix* g_Animfix = new CMAnimationFix();
CResolver* resolver = new CResolver();

static bool is_dormant[65]{};

float calculate_lerpm()
{
	static auto interp = interfaces.cvars->FindVar(crypt_str("cl_interp"));
	static auto interpRatio = interfaces.cvars->FindVar(crypt_str("cl_interp_ratio"));
	static auto minInterpRatio = interfaces.cvars->FindVar(crypt_str("sv_client_min_interp_ratio"));
	static auto maxInterpRatio = interfaces.cvars->FindVar(crypt_str("sv_client_max_interp_ratio"));
	static auto updateRate = interfaces.cvars->FindVar(crypt_str("cl_updaterate"));
	static auto maxUpdateRate = interfaces.cvars->FindVar(crypt_str("sv_maxupdaterate"));

	auto ratio = std::clamp(interpRatio->GetFloat(), minInterpRatio->GetFloat(), maxInterpRatio->GetFloat());
	return max(interp->GetFloat(), (ratio / ((maxUpdateRate) ? maxUpdateRate->GetFloat() : updateRate->GetFloat())));
}

float calculate_lerp()
{
	static auto cl_interp = interfaces.cvars->FindVar(hs::cl_interp.s().c_str());
	static auto cl_updaterate = interfaces.cvars->FindVar(hs::cl_updaterate.s().c_str());
	const auto update_rate = cl_updaterate->GetInt();
	const auto interp_ratio = cl_interp->GetFloat();

	auto lerp = interp_ratio / update_rate;

	if (lerp <= interp_ratio)
		lerp = interp_ratio;

	return lerp;
}

void Extrapolate(IBasePlayer* player, Vector& origin, Vector& velocity, int& flags, bool on_ground)
{
	static const auto sv_gravity = interfaces.cvars->FindVar(hs::sv_gravity.s().c_str());
	static const auto sv_jump_impulse = interfaces.cvars->FindVar(hs::sv_jump_impulse.s().c_str());

	if (!(flags & FL_ONGROUND))
		velocity.z -= TICKS_TO_TIME(sv_gravity->GetFloat());
	else if (player->GetFlags() & FL_ONGROUND && !on_ground)
		velocity.z = sv_jump_impulse->GetFloat();

	const auto& src = origin;
	auto end = src + velocity * interfaces.global_vars->interval_per_tick;

	Ray_t r;
	r.Init(src, end, player->GetMins(), player->GetMaxs());

	trace_t t;
	CTraceFilter filter;
	filter.pSkip = player;

	interfaces.trace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

	if (t.fraction != 1.f)
	{
		for (auto i = 0; i < 2; i++)
		{
			velocity -= t.plane.normal * velocity.Dot(t.plane.normal);

			const auto dot = velocity.Dot(t.plane.normal);
			if (dot < 0.f)
				velocity -= Vector(dot * t.plane.normal.x,
					dot * t.plane.normal.y, dot * t.plane.normal.z);

			end = t.endpos + velocity * TICKS_TO_TIME(1.f - t.fraction);

			r.Init(t.endpos, end, player->GetMins(), player->GetMaxs());
			interfaces.trace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

			if (t.fraction == 1.f)
				break;
		}
	}

	origin = end = t.endpos;
	end.z -= 2.f;

	r.Init(origin, end, player->GetMins(), player->GetMaxs());
	interfaces.trace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

	flags &= ~FL_ONGROUND;

	if (t.DidHit() && t.plane.normal.z > .7f)
		flags |= FL_ONGROUND;
}
/*
bool animation::is_validn(float range = .2f, float max_unlag = .2f)
{
	auto network = interfaces.engine->GetNetChannelInfo();
	if (!network || !valid)
		return false;

	auto unlagLimit = interfaces.cvars->FindVar(crypt_str("sv_maxunlag"));

	auto deadTime = static_cast<int>(interfaces.global_vars->curtime - unlagLimit->GetFloat());
	if (sim_time < deadTime)
		return false;

	auto delta = std::clamp(network->GetLatency(0) + network->GetLatency(1) + calculate_lerp(), 0.f, unlagLimit->GetFloat()) - (interfaces.global_vars->curtime - sim_time);
	return std::fabsf(delta) <= 0.2f;

}
*/

bool animation::is_valid(float range = .2f, float max_unlag = .2f)
{
	if (!interfaces.engine->GetNetChannelInfo() || !valid)
		return false;

	const auto correct = std::clamp(interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING)
		+ interfaces.engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING)
		+ calculate_lerp(), 0.f, max_unlag);

	float curtime = csgo->local->isAlive() ? TICKS_TO_TIME(csgo->fixed_tickbase) : interfaces.global_vars->curtime;

	//if (CanDT() && csgo->dt_charged && !CMAntiAim::Get().did_shot)
	//	range += TICKS_TO_TIME(6.5f);
	return fabsf(correct - (curtime - sim_time)) < range && correct < 1.f;
}

animation::animation(IBasePlayer* player)
{
	const auto weapon = player->GetWeapon();
	safepoints = false;
	this->player = player;
	index = player->GetIndex();
	dormant = player->IsDormant();
	velocity = player->GetVelocity();
	origin = player->GetOrigin();
	abs_origin = player->GetAbsOrigin();
	obb_mins = player->GetMins();
	obb_maxs = player->GetMaxs();
	std::memcpy(layers, player->GetAnimOverlays(), sizeof(CAnimationLayer) * 13);
	poses = player->m_flPoseParameter();
	anim_state = player->GetPlayerAnimState();
	sim_time = player->GetSimulationTime();
	interp_time = 0.f;
	last_shot_time = weapon ? weapon->GetLastShotTime() : 0.f;
	duck = player->GetDuckAmount();
	lby = player->GetLBY();
	eye_angles = player->GetEyeAngles();
	abs_ang = player->GetAbsAngles();
	flags = player->GetFlags();
	eflags = player->GetEFlags();
	effects = player->GetEffects();

	lag = TIME_TO_TICKS(player->GetSimulationTime() - player->GetOldSimulationTime());

	valid = lag >= 0 && lag <= 17;

	lag = std::clamp(lag, 0, 17);
}

animation::animation(IBasePlayer* player, Vector last_reliable_angle) : animation(player)
{
	safepoints = false;
	this->last_reliable_angle = last_reliable_angle;
}

void animation::restore(IBasePlayer* player) const
{
	player->GetVelocity() = velocity;
	player->GetFlagsPtr() = flags;
	player->GetEFlags() = eflags;
	player->GetDuckAmount() = duck;
	std::memcpy(player->GetAnimOverlays(), layers, sizeof(CAnimationLayer) * 13);
	player->GetLBY() = lby;
	player->GetOrigin() = origin;
	player->SetAbsOrigin(abs_origin);
}

void animation::apply(IBasePlayer* player) const
{
	*player->GetEyeAnglesPointer() = eye_angles;
	player->SetPoseParameter(poses);
	player->GetVelocity() = velocity;
	player->GetFlagsPtr() = flags;
	player->GetEFlags() = eflags;
	player->GetDuckAmount() = duck;
	std::memcpy(player->GetAnimOverlays(), layers, sizeof(CAnimationLayer) * 13);
	player->GetLBY() = lby;
	player->GetOrigin() = origin;
	player->SetAbsOrigin(abs_origin);
	if (player->GetPlayerAnimState())
		player->SetAnimState(anim_state);
}
/*
void animation::build_inversed_bones(IBasePlayer* player) {
	auto idx = player->GetIndex();
	static float spawntime[65] = { 0.f };
	static CBaseHandle* selfhandle[65] = { nullptr };

	if (spawntime[idx] == 0.f)
		spawntime[idx] = player->GetSpawnTime();


	auto alloc = g_Animfix->IS_Animstate[idx] == nullptr;
	auto change = !alloc && selfhandle[idx] != &player->GetRefEHandle();
	auto reset = !alloc && !change && player->GetSpawnTime() != spawntime[idx];

	if (change) {
		memset(&g_Animfix->IS_Animstate[idx], 0, sizeof(g_Animfix->IS_Animstate[idx]));
		selfhandle[idx] = (CBaseHandle*)&player->GetRefEHandle();
	}
	if (reset) {
		player->ResetAnimationState(g_Animfix->IS_Animstate[idx]);
		spawntime[idx] = player->GetSpawnTime();
	}

	if (alloc || change) {
		g_Animfix->IS_Animstate[idx] = reinterpret_cast<CCSGOPlayerAnimState*>(interfaces.memalloc->Alloc(sizeof(CCSGOPlayerAnimState)));
		if (g_Animfix->IS_Animstate[idx])
			player->CreateAnimationState(g_Animfix->IS_Animstate[idx]);
	}

	if (!g_Animfix->IS_Animstate[idx])
		return;

	if (player->GetFlags() & FL_ONGROUND) {
		g_Animfix->IS_Animstate[idx]->m_bOnGround = true;
		g_Animfix->IS_Animstate[idx]->m_bLanding = false;
	}
	g_Animfix->IS_Animstate[idx]->time_since_in_air() = 0.f;

	float desync_angle = player->GetDSYDelta();
	bool resolver_disabled = ResolverMode[idx] == crypt_str("OFF");

	if (ResolverMode[idx].find('LD') != -1) // low delta
		desync_angle *= .5f; // half of max desync delta

	if (resolver_disabled)
		g_Animfix->IS_Animstate[idx]->m_flFootYaw = Math::NormalizeYaw(player->GetEyeAngles().y - desync_angle); // setup inversed side
	else
		g_Animfix->IS_Animstate[idx]->m_flFootYaw = Math::NormalizeYaw(resolver->LastAppliedAngle[idx] - desync_angle * 2.f);

	player->UpdateAnimationState(g_Animfix->IS_Animstate[idx], player->GetEyeAngles());
	player->InvalidateBoneCache();
	player->SetupBones(opposite_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
}
*/

void animation::build_server_bones(IBasePlayer* player)
{
	float_t flCurTime = interfaces.global_vars->curtime;
	float_t flRealTime = interfaces.global_vars->realtime;
	float_t flFrameTime = interfaces.global_vars->framecount;
	float_t flAbsFrameTime = interfaces.global_vars->absoluteframetime;
	int32_t iFrameCount = interfaces.global_vars->framecount;
	int32_t iTickCount = interfaces.global_vars->tickcount;
	float_t flInterpolation = interfaces.global_vars->interpolation_amount;

	float stime = player->GetSimulationTime();
	float time = stime;
	int ticks = TIME_TO_TICKS(time);

	interfaces.global_vars->curtime = time;
	interfaces.global_vars->realtime = time;
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->absoluteframetime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->framecount = ticks;
	interfaces.global_vars->tickcount = ticks;
	interfaces.global_vars->interpolation_amount = 0.f;

	int32_t nClientEffects = player->GetEffects();
	int32_t nLastSkipFramecount = player->LastSkipFramecount();
	int32_t nOcclusionMask = player->GetOcclusionFlags();
	int32_t nOcclusionFrame = player->GetOcclusionFramecount();
	int32_t iEffects = player->GetEffects();
	bool bMaintainSequenceTransition = player->MaintainSequenceTransition();
	Vector vecAbsOrigin = player->GetAbsOriginVec();
	int32_t iMask = 0x0007FF00; //fin matrix mask

	player->InvalidateBoneCache();
	player->GetBoneAccessor()->m_ReadableBones = NULL;
	player->GetBoneAccessor()->m_WritableBones = NULL;

	if (player->GetPlayerAnimState())
		player->GetPlayerAnimState()->m_pWeaponLast = player->GetPlayerAnimState()->m_pWeapon;

	player->GetOcclusionFramecount() = 0;
	player->GetOcclusionFlags() = 0;
	player->LastSkipFramecount() = 0;
	player->SetAbsOrigin(player->origin());
	player->GetEffects() |= NO_INTERP;
	player->GetClientEffects() |= 2;
	player->MaintainSequenceTransition() = false;
	player->GetAnimOverlays()[ANIMATION_LAYER_LEAN].m_flWeight = 0.0f;
	player->SetupBones(nullptr, 128, iMask, player->GetSimulationTime());
	player->MaintainSequenceTransition() = bMaintainSequenceTransition;
	player->GetClientEffects() = nClientEffects;
	player->GetEffects() = iEffects;
	player->LastSkipFramecount() = nLastSkipFramecount;
	player->GetOcclusionFramecount() = nOcclusionFrame;
	player->GetOcclusionFlags() = nOcclusionMask;
	player->SetAbsOrigin(vecAbsOrigin);

	interfaces.global_vars->curtime = flCurTime;
	interfaces.global_vars->realtime = flRealTime;
	interfaces.global_vars->framecount = flFrameTime;
	interfaces.global_vars->absoluteframetime = flAbsFrameTime;
	interfaces.global_vars->framecount = iFrameCount;
	interfaces.global_vars->tickcount = iTickCount;
	interfaces.global_vars->interpolation_amount = flInterpolation;

	memcpy(bones, player->GetBoneCache().Base(), sizeof(matrix) * player->GetBoneCache().Count());

	/*
	player->GetBoneAccessor()->m_WritableBones = player->GetBoneAccessor()->m_ReadableBones = 0;

	*(int*)(uintptr_t(player) + 0x2924) = 0xFF7FFFFF;// m_flLastBoneSetupTime
	*(int*)(uintptr_t(player) + 0x2690) = 0;//int(bone_counter) - 1;

	const auto clientsideanim = player->GetClientSideAnims();

	float bk = FLT_MAX;

	auto state = player->GetPlayerAnimState();
	if (!state)
		return;

	const auto backupOcclusionFlags = *(int*)(uintptr_t(player) + 0xA28);
	const auto backupOcclusionFramecount = *(int*)(uintptr_t(player) + 0xA30);
	const auto v22 = *(uint8_t*)(uintptr_t(player) + 0x68);

	const auto effects = player->GetEffects();
	player->GetEffects() |= 8u;

	*(int*)(uintptr_t(player) + 0xA68) = 0;
	*(int*)(uintptr_t(player) + 0xA28) &= ~10u;
	*(int*)(uintptr_t(player) + 0xA30) = 0;

	*(unsigned short*)(uintptr_t(player) + 0x68) |= 2; // m_ClientEntEffects // entindex() + 0x4

	auto realtime_backup = interfaces.global_vars->realtime;
	auto curtime = interfaces.global_vars->curtime;
	auto frametime = interfaces.global_vars->frametime;
	auto absoluteframetime = interfaces.global_vars->absoluteframetime;
	auto framecount = interfaces.global_vars->framecount;
	auto tickcount = interfaces.global_vars->tickcount;
	auto interpolation_amount = interfaces.global_vars->interpolation_amount;

	float time = sim_time;
	int ticks = TIME_TO_TICKS(time);

	interfaces.global_vars->curtime = time;
	interfaces.global_vars->realtime = time;
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->absoluteframetime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->framecount = ticks;
	interfaces.global_vars->tickcount = ticks;
	interfaces.global_vars->interpolation_amount = 0.f;

	const auto vecForce = *(int*)(uintptr_t(player) + 0x2670);

	*(int*)(uintptr_t(player) + 0x2670) = 0;

	player->GetClientSideAnims() = false;
	player->SetupBones(nullptr, -1, 0x0000FF00, time);
	player->GetClientSideAnims() = clientsideanim;

	*(int*)(uintptr_t(player) + 0x2670) = vecForce;
	*(int*)(uintptr_t(player) + 0xA28) = backupOcclusionFlags; //occlusionflag
	*(int*)(uintptr_t(player) + 0xA30) = backupOcclusionFramecount;// occlusionframecount
	*(unsigned short*)(uintptr_t(player) + 0x68) = v22;
	player->GetEffects() = effects;

	interfaces.global_vars->realtime = realtime_backup;
	interfaces.global_vars->curtime = curtime;
	interfaces.global_vars->frametime = frametime;
	interfaces.global_vars->absoluteframetime = absoluteframetime;
	interfaces.global_vars->framecount = framecount;
	interfaces.global_vars->tickcount = tickcount;
	interfaces.global_vars->interpolation_amount = interpolation_amount;

	memcpy(bones, player->GetBoneCache().Base(), sizeof(matrix) * player->GetBoneCache().Count());
	*/
}

void build_safe_matrix(IBasePlayer* player, matrix* aMatrix)
{
	float_t flCurTime = interfaces.global_vars->curtime;
	float_t flRealTime = interfaces.global_vars->realtime;
	float_t flFrameTime = interfaces.global_vars->framecount;
	float_t flAbsFrameTime = interfaces.global_vars->absoluteframetime;
	int32_t iFrameCount = interfaces.global_vars->framecount;
	int32_t iTickCount = interfaces.global_vars->tickcount;
	float_t flInterpolation = interfaces.global_vars->interpolation_amount;

	float stime = player->GetSimulationTime();
	float time = stime;
	int ticks = TIME_TO_TICKS(time);

	interfaces.global_vars->curtime = time;
	interfaces.global_vars->realtime = time;
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->absoluteframetime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->framecount = ticks;
	interfaces.global_vars->tickcount = ticks;
	interfaces.global_vars->interpolation_amount = 0.f;

	int32_t nClientEffects = player->GetEffects();
	int32_t nLastSkipFramecount = player->LastSkipFramecount();
	int32_t nOcclusionMask = player->GetOcclusionFlags();
	int32_t nOcclusionFrame = player->GetOcclusionFramecount();
	int32_t iEffects = player->GetEffects();
	bool bMaintainSequenceTransition = player->MaintainSequenceTransition();
	Vector vecAbsOrigin = player->GetAbsOriginVec();

	int32_t iMask = 0x00000100; //bsafematrix mask

	player->InvalidateBoneCache();
	player->GetBoneAccessor()->m_ReadableBones = NULL;
	player->GetBoneAccessor()->m_WritableBones = NULL;

	if (player->GetPlayerAnimState())
		player->GetPlayerAnimState()->m_pWeaponLast = player->GetPlayerAnimState()->m_pWeapon;

	player->GetOcclusionFramecount() = 0;
	player->GetOcclusionFlags() = 0;
	player->LastSkipFramecount() = 0;
	player->SetAbsOrigin(player->origin());
	player->GetEffects() |= NO_INTERP;
	player->GetClientEffects() |= 2;
	player->MaintainSequenceTransition() = false;
	player->GetAnimOverlays()[ANIMATION_LAYER_LEAN].m_flWeight = 0.0f;
	player->GetAnimOverlays()[ANIMATION_LAYER_ADJUST].m_pOwner = NULL;
	player->SetupBones(aMatrix, 128, iMask, player->GetSimulationTime());
	player->MaintainSequenceTransition() = bMaintainSequenceTransition;
	player->GetClientEffects() = nClientEffects;
	player->GetEffects() = iEffects;
	player->LastSkipFramecount() = nLastSkipFramecount;
	player->GetOcclusionFramecount() = nOcclusionFrame;
	player->GetOcclusionFlags() = nOcclusionMask;
	player->SetAbsOrigin(vecAbsOrigin);
	interfaces.global_vars->curtime = flCurTime;
	interfaces.global_vars->realtime = flRealTime;
	interfaces.global_vars->framecount = flFrameTime;
	interfaces.global_vars->absoluteframetime = flAbsFrameTime;
	interfaces.global_vars->framecount = iFrameCount;
	interfaces.global_vars->tickcount = iTickCount;
	interfaces.global_vars->interpolation_amount = flInterpolation;
}

template < class T >
__forceinline T Interpolate(const T& flCurrent, const T& flTarget, const int iProgress, const int iMaximum)
{
	return flCurrent + ((flTarget - flCurrent) / iMaximum) * iProgress;
}
inline float stdnorm(const float& f) //normalize without choking 20% of the  cpu like a retard (p10000000000000)
{
	return std::remainderf(f, 360.f);
}

void CMAnimationFix::animation_info::update_animations(animation* record, animation* previous)
{
	auto animstate = player->GetPlayerAnimState();
	int idx = player->GetIndex();

	float serverFootYaw = player->GetPlayerAnimState()->m_flFootYaw;

	float distance = csgo->local->GetAbsOrigin().DistTo(player->GetAbsOrigin());

	record->resolved = false;

	if (distance > 8192.f) {
		record->velocity = player->GetVelocity();
		record->didshot = false;
		record->safepoints = false;
		record->came_from_dormant = -1;
		record->apply(player);
		return;
	}

	if (!previous) //no previous record found so possibly came from dormancy
	{
		record->velocity = player->GetVelocity();

		record->didshot = false;
		record->safepoints = true;
		record->came_from_dormant = -1;
		record->apply(player);

		//animstate->m_flGoalFeetYaw = resolver->ResolvedAngle[idx];

		//no last record so we can calc rotations and resolve
		g_Animfix->rotationResolve(player, record, nullptr);
		return;
	}

	record->came_from_dormant++;

	// did the player shoot?
	const float& came_from_dormant_time = record->player->CameFromDormantTime();

	if (previous && !record->dormant && !previous->dormant && record->last_shot_time != came_from_dormant_time) {
		if (record->last_shot_time > previous->sim_time && record->last_shot_time <= record->sim_time) {
				record->didshot = true;
				//Msg("DIDSHOT", color_t(255, 255, 255, 255));
		}
	}
	/*
	if (!previous->didshot)
		record->didshot = record->last_shot_time > previous->sim_time && record->last_shot_time <= record->sim_time
		&& record->last_shot_time != came_from_dormant_time && previous->last_shot_time != came_from_dormant_time;
		*/

	bool bHasPreviousRecord = false;


	//we do not need to simulate fakelag
	if (record->updateDelay <= 1) {
		player->GetVelocity() = record->velocity;
		player->GetAbsVelocity() = record->velocity;

		record->apply(player);

		g_Animfix->rotationResolve(player, record, previous);
		return;
	}

	else {
		float land_time = 0.0f;
		bool is_landed = false;
		bool land_in_cycle = false;

		// check if landed in choke cycle
		if (record->layers[4].m_flCycle < 0.5f && (!(record->flags & FL_ONGROUND) || !(previous->flags & FL_ONGROUND)))
		{
			land_time = record->sim_time - (record->layers[4].m_flPlaybackRate * record->layers[4].m_flCycle);
			land_in_cycle = land_time >= previous->sim_time;
		}

		bool on_ground = record->flags & FL_ONGROUND;
		////////////////////////////
	   //SETUP RESOLVER ROTATIONS//
	  ////////////////////////////
		static auto& enable_bone_cache_invalidation = **reinterpret_cast<bool**>(csgo->ptrboneCacheInvalidation);

		//// make a backup of globals
		const auto backup_frametime = interfaces.global_vars->frametime;
		const auto backup_curtime = interfaces.global_vars->curtime;
		const auto old_flags = player->GetFlagsPtr();

		// get player anim state
		auto state = player->GetPlayerAnimState();

		// allow re-animate player in this tick
		if (state->m_nLastUpdateFrame == interfaces.global_vars->framecount)
			state->m_nLastUpdateFrame -= 1.f;

		if (state->m_flLastUpdateTime == interfaces.global_vars->curtime)
			state->m_flLastUpdateTime = interfaces.global_vars->curtime - 1;


		// fixes for networked players
		interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
		interfaces.global_vars->curtime = player->GetSimulationTime();

		player->GetEFlags() &= ~0x1000;
		player->GetAbsVelocity() = record->velocity;

		//player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);

		// make sure we keep track of the original invalidation state
		const auto old_invalidation = enable_bone_cache_invalidation;

		// notify the other hooks to instruct animations and pvs fix

		auto animstate = player->GetPlayerAnimState();
		CCSGOPlayerAnimState backupState;

		//copy the server layers before reanimating the player
		memcpy(&backupState, animstate, sizeof(CCSGOPlayerAnimState));
		std::memcpy(&record->layer6_b, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));

		//setup cycle rates to server out
		player->GetPlayerAnimState()->m_flPrimaryCycle = record->layer6_b.m_flCycle;
		player->GetPlayerAnimState()->m_flMoveWeight = record->layer6_b.m_flWeight;

		auto delta = player->GetDSYDelta();
		auto lowDelta = delta * 0.5f;

		//reanimate player to negative side
		csgo->EnableBones = player->GetClientSideAnims() = true;
		state->m_flFootYaw = stdnorm(player->EyeAngles()->y - delta);
		player->UpdateClientSideAnimation();
		csgo->EnableBones = player->GetClientSideAnims() = false;
		//player->SetupBones(record->negative_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);

		build_safe_matrix(player, record->negative_matrix);

		std::memcpy(&record->layer6_n, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
		memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));

		//LOW DELTA NEGRO
		if (record->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight > 0.f && vars.ragebot.resolver) {
			csgo->EnableBones = player->GetClientSideAnims() = true;
			state->m_flFootYaw = stdnorm(player->EyeAngles()->y - lowDelta);
			player->UpdateClientSideAnimation();
			csgo->EnableBones = player->GetClientSideAnims() = false;
			//player->SetupBones(record->n_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
			std::memcpy(&record->layer6_nl, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
			memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));
		}

		//reanimate player to positive side
		csgo->EnableBones = player->GetClientSideAnims() = true;
		state->m_flFootYaw = stdnorm(player->EyeAngles()->y + delta);
		player->UpdateClientSideAnimation();
		csgo->EnableBones = player->GetClientSideAnims() = false;
		//player->SetupBones(record->positive_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
		build_safe_matrix(player, record->positive_matrix);

		std::memcpy(&record->layer6_p, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
		memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));

		//reanimate player to positive side
		if (record->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight > 0.f && vars.ragebot.resolver) {
			csgo->EnableBones = player->GetClientSideAnims() = true;
			state->m_flFootYaw = stdnorm(player->EyeAngles()->y + lowDelta);
			player->UpdateClientSideAnimation();
			csgo->EnableBones = player->GetClientSideAnims() = false;
			//player->SetupBones(record->n_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
			std::memcpy(&record->layer6_pl, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
			memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));
		}

		//CENTRE
		if (record->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight > 0.f && vars.ragebot.resolver) {
			csgo->EnableBones = player->GetClientSideAnims() = true;
			state->m_flFootYaw = stdnorm(player->EyeAngles()->y);
			player->UpdateClientSideAnimation();
			csgo->EnableBones = player->GetClientSideAnims() = false;
			//player->SetupBones(record->n_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
			std::memcpy(&record->layer6_c, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
		}

		//BACK UP AGAIN BEFORE WE DO FUNNY
		memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));
		//float serverFootYaw = player->GetPlayerAnimState()->m_flFootYaw;

		// -- ROTATIONS DONE GO TO RESOLVE ROUTINE -- //
		// -- DO RESOLVE DATA -- //
		std::optional<float> o_flFootYawResolved = resolver->Do(player, record, previous, serverFootYaw); //Determine m_flFootYaw
		player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);

		// we don't want to enable cache invalidation by accident
		enable_bone_cache_invalidation = old_invalidation;

		// restore globals
		interfaces.global_vars->curtime = backup_curtime;
		interfaces.global_vars->frametime = backup_frametime;

		player->GetFlagsPtr() = old_flags;

		//----------------- SIMULATE DELAYED TICKS  -----------------//
		for (auto i = 1; i <= record->updateDelay; i++)
		{
			const auto simulated_time = previous->sim_time + TICKS_TO_TIME(i);

			player->GetDuckAmount() = Interpolate(previous->duck, record->duck, i, record->updateDelay);
			player->GetVelocity() = Interpolate(previous->velocity, record->velocity, i, record->updateDelay);
			player->GetAbsVelocity() = Interpolate(previous->velocity, record->velocity, i, record->updateDelay);

			const auto ct = interfaces.global_vars->curtime;
			interfaces.global_vars->curtime = simulated_time;

			if (land_in_cycle && !is_landed)
			{
				if (land_time <= simulated_time)
				{
					is_landed = true;
					on_ground = true;
				}
				else
					on_ground = previous->flags & FL_ONGROUND;
			}

			if (on_ground)
				player->GetFlagsPtr() |= FL_ONGROUND;
			else
				player->GetFlagsPtr() &= ~FL_ONGROUND;

			// backup simtime.
			const auto backup_simtime = player->GetSimulationTime();

			// set new simtime.
			player->GetSimulationTime() = simulated_time;

			//----------------- APPLY RESOLVER ON FINAL -----------------//
			if (i == record->updateDelay) {
				static auto& enable_bone_cache_invalidation = **reinterpret_cast<bool**>(csgo->ptrboneCacheInvalidation);

				//// make a backup of globals
				const auto backup_frametime = interfaces.global_vars->frametime;
				const auto backup_curtime = interfaces.global_vars->curtime;
				const auto old_flags = player->GetFlagsPtr();

				// get player anim state
				auto state = player->GetPlayerAnimState();

				// allow re-animate player in this tick
				if (state->m_nLastUpdateFrame == interfaces.global_vars->framecount)
					state->m_nLastUpdateFrame -= 1.f;

				if (state->m_flLastUpdateTime == interfaces.global_vars->curtime)
					state->m_nLastUpdateFrame -= 1.f;

				// fixes for networked players
				interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
				interfaces.global_vars->curtime = player->GetSimulationTime();

				//player->GetEFlags() &= ~0x1000;
				//player->GetAbsVelocity() = record->velocity;

				//player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);

				// make sure we keep track of the original invalidation state
				const auto old_invalidation = enable_bone_cache_invalidation;

				// notify the other hooks to instruct animations and pvs fix

				//auto animstate = player->GetPlayerAnimState();
				//CCSGOPlayerAnimState backupState;

				float flFootYawResolved = 0.f;

				// -- GENERATE SAFE MATRIX -- //

				csgo->EnableBones = player->GetClientSideAnims() = true;
				auto idx = player->GetIndex();
				float desync_angle = player->GetDSYDelta();

				bool resolver_disabled = false;

				//if option doesnt contain value resolver is effectively off
				if (o_flFootYawResolved.has_value()) {
					flFootYawResolved = o_flFootYawResolved.value();
				}
				else {
					resolver_disabled = true;
				}

				//APPLY RESOLVED ANGLES
				animstate = player->GetPlayerAnimState();
				//apply resolved angle and update
				csgo->EnableBones = player->GetClientSideAnims() = true;
				if (!resolver_disabled) {
					player->GetPlayerAnimState()->m_flFootYaw = stdnorm(flFootYawResolved); //apply resolver footYaw
				}
				//else {
					//player->GetPlayerAnimState()->m_flFootYaw = stdnorm(serverFootYaw);
				//}
				player->UpdateClientSideAnimation();
				csgo->EnableBones = player->GetClientSideAnims() = false;


				//player->InvalidatePhysicsRecursive(ANGLES_CHANGED);
				player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
				//player->InvalidatePhysicsRecursive(SEQUENCE_CHANGED);
				//player->InvalidatePhysicsRecursive(VELOCITY_CHANGED);

				// we don't want to enable cache invalidation by accident
				enable_bone_cache_invalidation = old_invalidation;

				// restore globals
				interfaces.global_vars->curtime = backup_curtime;
				interfaces.global_vars->frametime = backup_frametime;

				player->GetFlagsPtr() = old_flags;
			}
			else {
				//Update the Step of the fakelag prediction
				g_Animfix->simStepUpdate(player);
			}

			// restore old simtime.
			player->GetSimulationTime() = backup_simtime;

			interfaces.global_vars->curtime = ct;
		}
	}
}

void CMAnimationFix::simStepUpdate(IBasePlayer* player)
{
	// make a backup of globals
	const auto backup_frametime = interfaces.global_vars->frametime;
	const auto backup_curtime = interfaces.global_vars->curtime;
	const auto old_flags = player->GetFlagsPtr();

	// get player anim state
	auto state = player->GetPlayerAnimState();

	// allow re-animate player in this tick
	if (state->m_nLastUpdateFrame == interfaces.global_vars->framecount)
		state->m_nLastUpdateFrame -= 1.f;

	if (state->m_flLastUpdateTime == interfaces.global_vars->curtime)
		state->m_nLastUpdateFrame -= 1.f;

	// fixes for networked players
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->curtime = player->GetSimulationTime();
	player->GetEFlags() &= ~(EFL_DIRTY_ABSVELOCITY | EFL_DIRTY_ABSTRANSFORM);

	// notify the other hooks to instruct animations and pvs fix
	csgo->EnableBones = player->GetClientSideAnims() = true;
	player->UpdateClientSideAnimation();
	csgo->EnableBones = player->GetClientSideAnims() = false;

	player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);

	// restore globals
	interfaces.global_vars->curtime = backup_curtime;
	interfaces.global_vars->frametime = backup_frametime;
}

void CMAnimationFix::UpdatePlayers()
{
	if (!interfaces.engine->IsInGame())
		return;

	const auto local = csgo->local;

	if (!local) {
		if (!animation_infos.empty()) {
			animation_infos.clear();
		}
		return;
	}

	// erase outdated entries
	for (auto it = animation_infos.begin(); it != animation_infos.end();) {
		auto player = reinterpret_cast<IBasePlayer*>(interfaces.ent_list->GetClientEntityFromHandle(it->first));

		if (!player || player != it->second->player || !player->isAlive()
			|| !local)
		{
			if (player)
				player->GetClientSideAnims() = true;
			it = animation_infos.erase(it);
		}
		else
			it = next(it);
	}

	for (auto i = 1; i <= interfaces.engine->GetMaxClients(); ++i) {
		const auto entity = interfaces.ent_list->GetClientEntity(i);
		if (!entity || !entity->IsPlayer())
			continue;
		if (!entity->isAlive() /*|| player->IsDormant()*/)
			continue;

		if (entity->IsDormant())
			continue;

		if (entity == local)
			continue;

		if (entity != local && !entity->isEnemy(csgo->local)) {
			csgo->EnableBones = entity->GetClientSideAnims() = true;
			continue;
		}

		if (animation_infos.find(entity->GetRefEHandle()) == animation_infos.end())
			animation_infos.insert_or_assign(entity->GetRefEHandle(), new animation_info(entity, {}));
	}

	// run post update
	for (auto& info : animation_infos)
	{
		auto& _animation = info.second;
		const auto player = _animation->player;

		// erase frames out-of-range
		for (auto i = 0; i < _animation->frames.size(); i++) {
			if (!_animation->frames[i].is_valid(0.2f + TICKS_TO_TIME(17))) {
				_animation->frames.erase(_animation->frames.begin() + i, _animation->frames.begin() + _animation->frames.size() - 1);
				break;
			}
		}

		// have we already seen this update?
		if (player->GetSimulationTime() != player->CameFromDormantTime()) {
			if (player->GetSimulationTime() == player->GetOldSimulationTime())
				continue;
		}

		// reset animstate
		if (_animation->last_spawn_time != player->GetSpawnTime())
		{
			const auto state = player->GetPlayerAnimState();
			if (state)
				player->ResetAnimationState(state);

			_animation->last_spawn_time = player->GetSpawnTime();
		}

		// grab weapon
		const auto weapon = player->GetWeapon();

		//// make a full backup of the player
		animation* backup = new animation(player);
		backup->apply(player);

		// grab previous
		animation* previous = nullptr;

		if (!_animation->frames.empty() && !_animation->frames.front().dormant
			&& TIME_TO_TICKS(player->GetSimulationTime() - _animation->frames.front().sim_time) <= 17)
			previous = &_animation->frames.front();

		// update shot info
		if (!weapon || weapon->GetLastShotTime() != player->GetSimulationTime())
			info.second->last_reliable_angle = player->GetEyeAngles();


		// store server record
		auto& record = _animation->frames.emplace_front(player, info.second->last_reliable_angle);

		_animation->update_animations(&record, previous);


		if (previous) {
			if ((record.origin - previous->origin).LengthSqr() > 4096.f) {
				//lagcomp breaker
				_animation->frames.erase(_animation->frames.begin() + 1, _animation->frames.begin() + _animation->frames.size() - 1);
			}
		}


		//restore backed up layers
		memcpy(player->GetAnimOverlays(), backup->layers, sizeof(CAnimationLayer) * player->GetNumAnimOverlays());

		// use uninterpolated data to generate our bone matrix
		record.build_server_bones(player);

		// restore correctly synced values
		backup->restore(player);

		delete backup;
	}
}

void CMAnimationFix::rotationResolve(IBasePlayer* player, animation* record, animation* previous)
{
	static auto& enable_bone_cache_invalidation = **reinterpret_cast<bool**>(csgo->ptrboneCacheInvalidation);

	//// make a backup of globals
	const auto backup_frametime = interfaces.global_vars->frametime;
	const auto backup_curtime = interfaces.global_vars->curtime;
	const auto old_flags = player->GetFlagsPtr();

	// get player anim state
	auto state = player->GetPlayerAnimState();

	float serverFootYaw = player->GetPlayerAnimState()->m_flFootYaw;

	// allow re-animate player in this tick
	if (state->m_nLastUpdateFrame == interfaces.global_vars->framecount)
		state->m_nLastUpdateFrame -= 1.f;

	// fixes for networked players
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->curtime = player->GetSimulationTime();

	player->GetEFlags() &= ~0x1000;
	player->GetAbsVelocity() = record->velocity;

	//player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);

	// make sure we keep track of the original invalidation state
	const auto old_invalidation = enable_bone_cache_invalidation;

	// notify the other hooks to instruct animations and pvs fix

	auto animstate = player->GetPlayerAnimState();
	CCSGOPlayerAnimState backupState;

	//copy the server layers before reanimating the player
	memcpy(&backupState, animstate, sizeof(CCSGOPlayerAnimState));
	std::memcpy(&record->layer6_b, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));

	//setup cycle rates to server out
	player->GetPlayerAnimState()->m_flPrimaryCycle = record->layer6_b.m_flCycle;
	player->GetPlayerAnimState()->m_flMoveWeight = record->layer6_b.m_flWeight;

	auto delta = player->GetDSYDelta();
	//Msg(std::to_string(delta), vars.visuals.eventlog_color);
	auto lowDelta = delta * 0.5f;

	//reanimate player to negative side
	csgo->EnableBones = player->GetClientSideAnims() = true;
	state->m_flFootYaw = stdnorm(player->EyeAngles()->y - delta);
	player->UpdateClientSideAnimation();
	csgo->EnableBones = player->GetClientSideAnims() = false;
	//player->SetupBones(record->negative_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
	build_safe_matrix(player, record->negative_matrix);

	std::memcpy(&record->layer6_n, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
	memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));

	if (record->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight > 0.f && vars.ragebot.resolver) {
		//LOW DELTA NEGRO
		csgo->EnableBones = player->GetClientSideAnims() = true;
		state->m_flFootYaw = stdnorm(player->EyeAngles()->y - lowDelta);
		player->UpdateClientSideAnimation();
		csgo->EnableBones = player->GetClientSideAnims() = false;
		//player->SetupBones(record->n_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
		std::memcpy(&record->layer6_nl, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
		memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));
	}

	//reanimate player to positive side
	csgo->EnableBones = player->GetClientSideAnims() = true;
	state->m_flFootYaw = stdnorm(player->EyeAngles()->y + delta);
	player->UpdateClientSideAnimation();
	csgo->EnableBones = player->GetClientSideAnims() = false;
	build_safe_matrix(player, record->positive_matrix);
	std::memcpy(&record->layer6_p, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
	memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));

	//reanimate player to positive side
	if (record->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight > 0.f && vars.ragebot.resolver) {
		csgo->EnableBones = player->GetClientSideAnims() = true;
		state->m_flFootYaw = stdnorm(player->EyeAngles()->y + lowDelta);
		player->UpdateClientSideAnimation();
		csgo->EnableBones = player->GetClientSideAnims() = false;
		//player->SetupBones(record->n_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
		std::memcpy(&record->layer6_pl, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
		memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));
	}

	//CENTRE
	if (record->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight > 0.f && vars.ragebot.resolver) {
		csgo->EnableBones = player->GetClientSideAnims() = true;
		state->m_flFootYaw = stdnorm(player->EyeAngles()->y);
		player->UpdateClientSideAnimation();
		csgo->EnableBones = player->GetClientSideAnims() = false;
		//player->SetupBones(record->n_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
		std::memcpy(&record->layer6_c, &player->GetAnimOverlays()[6], sizeof(CAnimationLayer));
	}

	// -- ROTATIONS DONE GO TO RESOLVE ROUTINE -- //

	// -- DO RESOLVE DATA -- //
	std::optional<float> o_flFootYawResolved = resolver->Do(player, record, previous, serverFootYaw); //Determine m_flFootYaw
	float flFootYawResolved = 0.f;

	// -- GENERATE SAFE MATRIX -- //

	csgo->EnableBones = player->GetClientSideAnims() = true;
	auto idx = player->GetIndex();
	float desync_angle = player->GetDSYDelta();

	bool resolver_disabled = false;

	//if option doesnt contain value resolver is effectively off
	if (o_flFootYawResolved.has_value()) {
		flFootYawResolved = o_flFootYawResolved.value();
	}
	else {
		resolver_disabled = true;
	}

	/*
	if (resolver_disabled)
		animstate->m_flFootYaw = stdnorm(player->GetEyeAngles().y - desync_angle); // setup a matrix that could be the desync angle incase user disabled resolver but still wants safepoint (not ideal since we only have 1 side matrix)
	else
		animstate->m_flFootYaw = stdnorm(serverFootYaw - (desync_angle * -(resolver->resolvedForward[idx] ? -resolver->FreestandSide[idx] : resolver->FreestandSide[idx]))); // setup inversed side from resolved angle

	//reanimate player for safepoint matrix
	player->UpdateClientSideAnimation();
	player->SetupBones(record->opposite_matrix, 128, 0x7FF00, interfaces.global_vars->curtime);
	csgo->EnableBones = player->GetClientSideAnims() = false;
	memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));
	*/

	//APPLY RESOLVED ANGLES

	animstate = player->GetPlayerAnimState();
	//apply resolved angle and update
	csgo->EnableBones = player->GetClientSideAnims() = true;
	if (!resolver_disabled) {
		player->GetPlayerAnimState()->m_flFootYaw = stdnorm(flFootYawResolved); //apply resolver footYaw
	}
	else {
		memcpy(player->GetPlayerAnimState(), &backupState, sizeof(CCSGOPlayerAnimState));
	}

	// fix feet spin? ~need proper fix soon 
	record->anim_state->m_flMoveWeight = 0.f;

	player->UpdateClientSideAnimation();
	csgo->EnableBones = player->GetClientSideAnims() = false;

	//player->InvalidatePhysicsRecursive(ANGLES_CHANGED);
	player->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
	//player->InvalidatePhysicsRecursive(SEQUENCE_CHANGED);
	//player->InvalidatePhysicsRecursive(VELOCITY_CHANGED);

	// we don't want to enable cache invalidation by accident
	enable_bone_cache_invalidation = old_invalidation;

	// restore globals
	interfaces.global_vars->curtime = backup_curtime;
	interfaces.global_vars->frametime = backup_frametime;

	player->GetFlagsPtr() = old_flags;
}

CMAnimationFix::animation_info* CMAnimationFix::get_animation_info(IBasePlayer* player)
{
	auto info = animation_infos.find(player->GetRefEHandle());

	if (info == animation_infos.end())
		return nullptr;

	return info->second;
}

bool animation::is_valid_extended()
{
	return is_valid();
}

animation* CMAnimationFix::get_latest_animation(IBasePlayer* player)
{
	const auto info = animation_infos.find(player->GetRefEHandle());

	if (info == animation_infos.end() || info->second->frames.empty())
		return nullptr;

	for (auto it = info->second->frames.begin(); it != info->second->frames.end(); it = next(it)) {
		if ((it)->is_valid_extended()) {
			return &*it;
		}
	}

	return nullptr;
}

animation* CMAnimationFix::get_resolved_animation(IBasePlayer* player)
{
	const auto info = animation_infos.find(player->GetRefEHandle());

	if (info == animation_infos.end() || info->second->frames.empty())
		return nullptr;

	for (auto it = info->second->frames.begin(); it != info->second->frames.end(); it = next(it))
		if ((it)->is_valid_extended() && (it)->resolved)
			return &*it;

	return nullptr;
}

std::vector<animation*> CMAnimationFix::get_valid_animations(IBasePlayer* player)
{
	const auto info = animation_infos.find(player->GetRefEHandle());

	std::vector<animation*> ret = {};

	if (info == animation_infos.end() || info->second->frames.empty())
		return ret;

	for (auto it = info->second->frames.begin(); it != info->second->frames.end(); it = next(it))
		if ((it)->is_valid_extended())
			ret.emplace_back(&*it);

	return ret;
}

animation* CMAnimationFix::get_oldest_animation(IBasePlayer* player)
{
	const auto info = animation_infos.find(player->GetRefEHandle());

	if (info == animation_infos.end() || info->second->frames.empty())
		return nullptr;

	for (auto it = info->second->frames.rbegin(); it != info->second->frames.rend(); it = next(it)) {
		if (vars.legitbot.backtrack && !csgo->ragetick)
		{
			if ((it)->is_valid(vars.legitbot.btlength, 0.2f)) {
				return &*it;
			}
		}
		else
		{
			if ((it)->is_valid_extended()) {
				return &*it;
			}
		}
	}
	return nullptr;
}

animation* CMAnimationFix::get_latest_firing_animation(IBasePlayer* player)
{
	const auto info = animation_infos.find(player->GetRefEHandle());

	if (info == animation_infos.end() || info->second->frames.empty())
		return nullptr;

	for (auto it = info->second->frames.begin(); it != info->second->frames.end(); it = next(it))
		if ((it)->is_valid_extended() && (it)->didshot)
			return &*it;

	return nullptr;
}

static bool updatingFake{ false };
/*
void CMAnimationFix::UpdateFakeState()
{
	static CCSGOPlayerAnimState* fakeAnimState = nullptr;
	static bool updateFakeAnim = true;
	static bool initFakeAnim = true;
	static float spawnTime = 0.f;

	if (!csgo->local || !csgo->local->isAlive() || !csgo->local->GetPlayerAnimState())
		return;

	if (spawnTime != csgo->local->GetSpawnTime() || updateFakeAnim)
	{
		spawnTime = csgo->local->GetSpawnTime();
		initFakeAnim = false;
		updateFakeAnim = false;
	}

	if (!initFakeAnim)
	{
		fakeAnimState = static_cast<CCSGOPlayerAnimState*>(interfaces.memalloc->Alloc(sizeof(CCSGOPlayerAnimState)));
		if (fakeAnimState != nullptr)
			csgo->local->CreateAnimationState(fakeAnimState);

		initFakeAnim = true;
	}

	if (!fakeAnimState)
		return;

	if (csgo->send_packet)
	{
		updatingFake = true;

		std::array<CAnimationLayer, 13> layers;

		std::memcpy(&layers, csgo->local->GetAnimOverlays(), sizeof(CAnimationLayer) * csgo->local->GetNumAnimOverlays());

		auto backupAbs = csgo->local->GetAbsAngles();
		auto backupPoses = csgo->local->m_flPoseParameter();

		csgo->local->UpdateAnimationState(fakeAnimState, csgo->CurAngle);
		csgo->local->SetAbsAngles(Vector{ 0, fakeAnimState->m_flFootYaw, 0 });

		std::memcpy(csgo->local->GetAnimOverlays(), &layers, sizeof(CAnimationLayer) * csgo->local->GetNumAnimOverlays());


		csgo->local->GetAnimOverlays()[ANIMATION_LAYER_LEAN].m_flWeight = std::numeric_limits<float>::epsilon();


		//csgo->local->InvalidateBoneCache();
		csgo->UpdateMatrixLocal = true;
		csgo->local->SetupBones(csgo->fakematrix, 128, 0x0000FF00, interfaces.global_vars->curtime);// setup matrix
		csgo->UpdateMatrixLocal = false;



		const auto origin = csgo->local->GetRenderOrigin();
		if (csgo->fakematrix)
		{
			for (auto& i : csgo->fakematrix)
			{
				i[0][3] -= origin.x;
				i[1][3] -= origin.y;
				i[2][3] -= origin.z;
			}
		}
		std::memcpy(csgo->local->GetAnimOverlays(), &layers, sizeof(CAnimationLayer) * csgo->local->GetNumAnimOverlays());
		csgo->local->SetPoseParameter(backupPoses);
		csgo->local->SetAbsAngles(Vector{ 0, backupAbs.y, 0 });
		updatingFake = false;
	}
}
*/


void CMAnimationFix::UpdateFakeState()
{
	if ((!interfaces.engine->IsConnected() && !interfaces.engine->IsInGame()) || !csgo->local) {
		//ShouldInitAnimstate = false;
		return;
	}

	static CBaseHandle* selfhandle = nullptr;
	static float spawntime = csgo->local->GetSpawnTime();

	auto alloc = FakeAnimstate == nullptr;
	auto change = !alloc && selfhandle != &csgo->local->GetRefEHandle();
	auto reset = !alloc && !change && csgo->local->GetSpawnTime() != spawntime;

	if (change) {
		memset(&FakeAnimstate, 0, sizeof(FakeAnimstate));
		selfhandle = (CBaseHandle*)&csgo->local->GetRefEHandle();
	}
	if (reset) {
		csgo->local->ResetAnimationState(FakeAnimstate);
		spawntime = csgo->local->GetSpawnTime();
	}

	if (alloc || change) {
		FakeAnimstate = reinterpret_cast<CCSGOPlayerAnimState*>(interfaces.memalloc->Alloc(sizeof(CCSGOPlayerAnimState)));
		if (FakeAnimstate)
			csgo->local->CreateAnimationState(FakeAnimstate);
	}

	if (!FakeAnimstate)
		return;

	if (FakeAnimstate->m_nLastUpdateFrame == interfaces.global_vars->framecount)
		FakeAnimstate->m_nLastUpdateFrame -= 1;

	CAnimationLayer layers[13];
	std::array<float, 24> poses;

	csgo->local->ParseAnimOverlays(layers);
	csgo->local->ParsePoseParameter(poses);

	if (csgo->send_packet)
	{
		if (vars.misc.restrict_type != 2)
			csgo->local->UpdateAnimationState(FakeAnimstate, csgo->FakeAngle); // update animstate

		csgo->local->InvalidateBoneCache();
		csgo->UpdateMatrixLocal = true;

		csgo->local->GetAnimOverlays()[12].m_flWeight = FLT_EPSILON;
		csgo->local->GetAnimOverlays()[3].m_flWeight = 0.f;
		csgo->local->GetAnimOverlays()[3].m_flCycle = 0.f;

		csgo->local->SetupBones(csgo->fakematrix, 128, 0x0000FF00, interfaces.global_vars->curtime);// setup matrix
		csgo->UpdateMatrixLocal = false;

		if (vars.visuals.interpolated_dsy) {
			for (auto& i : csgo->fakematrix)
			{
				i[0][3] -= csgo->local->GetRenderOrigin().x;
				i[1][3] -= csgo->local->GetRenderOrigin().y;
				i[2][3] -= csgo->local->GetRenderOrigin().z;
			}
		}
	}

	csgo->local->SetAnimOverlays(layers);
	csgo->local->SetPoseParameter(poses);

	csgo->animstate = FakeAnimstate;
}

void CMAnimationFix::UpdateRealState() {
	
	if ((!interfaces.engine->IsConnected() && !interfaces.engine->IsInGame()) || !csgo->local || vars.misc.restrict_type == 2 || interfaces.gameType->getrealgamemode() == GAMEMODE_DANGERZONE || csgo->hudUpdateRequired || csgo->ping > 500.f)
		return;

	if (!csgo->local->isAlive())
		return;

	static CBaseHandle* selfhandle = nullptr;
	static float spawntime = csgo->local->GetSpawnTime();

	auto alloc = RealAnimstate == nullptr;
	auto change = !alloc && selfhandle != &csgo->local->GetRefEHandle();
	auto reset = !alloc && !change && csgo->local->GetSpawnTime() != spawntime;

	if (change) {
		memset(&RealAnimstate, 0, sizeof(RealAnimstate));
		selfhandle = (CBaseHandle*)&csgo->local->GetRefEHandle();
	}
	if (reset) {
		csgo->local->ResetAnimationState(RealAnimstate);
		spawntime = csgo->local->GetSpawnTime();
	}

	if (alloc || change) {
		RealAnimstate = reinterpret_cast<CCSGOPlayerAnimState*>(interfaces.memalloc->Alloc(sizeof(CCSGOPlayerAnimState)));
		if (RealAnimstate)
			csgo->local->CreateAnimationState(RealAnimstate);
	}

	if (!RealAnimstate)
		return;

	if (RealAnimstate->m_nLastUpdateFrame == interfaces.global_vars->framecount)
		RealAnimstate->m_nLastUpdateFrame -= 1;

	Vector old_ang = *(Vector*)((DWORD)csgo->local + 0x31E8);

	*(Vector*)((DWORD)csgo->local + 0x31E8) = csgo->FakeAngle;

	csgo->ShouldUpdate = true;
	csgo->local->UpdateAnimationState(RealAnimstate, csgo->CurAngle);
	csgo->ShouldUpdate = false;

	if (csgo->send_packet) {
		csgo->local->ParseAnimOverlays(csgo->layers);
		csgo->local->ParsePoseParameter(csgo->poses);

		if (RealAnimstate)
			csgo->last_gfy = RealAnimstate->m_flFootYaw;
	}

	csgo->layers[12].m_flWeight = FLT_EPSILON;

	std::memcpy(csgo->local->GetAnimOverlays(), &csgo->layers, sizeof(CAnimationLayer) * csgo->local->GetNumAnimOverlays());
	csgo->local->SetPoseParameter(csgo->poses);
	*(Vector*)((DWORD)csgo->local + 0x31E8) = old_ang;
	

}

void CMAnimationFix::FixPvs() {
	[&]() {
		for (int i = 1; i <= interfaces.global_vars->maxClients; i++)
		{
			auto pCurEntity = interfaces.ent_list->GetClientEntity(i);
			if (!pCurEntity
				|| !pCurEntity->IsPlayer()
				|| pCurEntity->EntIndex() == interfaces.engine->GetLocalPlayer())
				continue;

			*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pCurEntity) + 0xA30) = interfaces.global_vars->framecount; //occlusionframecount
			*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pCurEntity) + 0xA28) = 0; //occlusionflag
		}
	}();
}
