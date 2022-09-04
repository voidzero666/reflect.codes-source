#include "Hooks.h"
#include "Resolver.h"
#include "RageBacktracking.h"
#include "Ragebot.h"
#include "AnimationFix.h"
#include "Misc.h"
#include <optional>

std::string ResolverMode[65];
std::string LastAppliedAngle2[65];
bool LastAppliedAngle3[65];
int last_ticks[65];
int IBasePlayer::GetChokedPackets() {
	auto ticks = TIME_TO_TICKS(GetSimulationTime() - GetOldSimulationTime());
	if (ticks == 0 && last_ticks[GetIndex()] > 0) {
		return last_ticks[GetIndex()] - 1;
	}
	else {
		last_ticks[GetIndex()] = ticks;
		return ticks;
	}
}

float AngleDiff(float destAngle, float srcAngle)
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle)
	{
		if (delta >= 180)
			delta -= 360;
	}
	else
	{
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

const float m_flAimYawMin = -58.f;
const float m_flAimYawMax = 58.f;

float fnormalize_yaw(float f)
{
	while (f < -180.0f)
		f += 360.0f;

	while (f > 180.0f)
		f -= 360.0f;

	return f;
}

__forceinline float AngleMod(float angle) {
	return (360.f / 65536) * ((int)(angle * (65536.f / 360.f)) & 65535);
}

void NormalizeAngle(float& angle) {
	float rot;

	// bad number.
	if (!std::isfinite(angle)) {
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

float ApproachAngle(float target, float value, float speed) {
	float delta;

	target = AngleMod(target);
	value = AngleMod(value);
	delta = target - value;

	// speed is assumed to be positive.
	speed = std::abs(speed);

	NormalizeAngle(delta);

	if (delta > speed)
		value += speed;

	else if (delta < -speed)
		value -= speed;

	else
		value = target;

	return value;
}

float CResolver::GetAngle(IBasePlayer* player) {
	return Math::NormalizeYaw(player->GetEyeAngles().y);
}

float CResolver::GetForwardYaw(IBasePlayer* player) {
	return Math::NormalizeYaw(GetBackwardYaw(player) - 180.f);
}

float CResolver::GetAwayAngle(IBasePlayer* player) {
	return Math::CalculateAngle(csgo->local->GetEyePosition(), player->GetEyePosition()).y;
}

float CResolver::GetBackwardYaw(IBasePlayer* player) {
	return Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y;
}

float CResolver::GetLeftYaw(IBasePlayer* player) {
	return Math::NormalizeYaw(Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y - 90.f);
}

float CResolver::GetRightYaw(IBasePlayer* player) {
	return Math::NormalizeYaw(Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y + 90.f);
}

void CResolver::DetectSide(IBasePlayer* player, int* side)
{
	if (!csgo->local || !csgo->local->isAlive() || !csgo->local->GetWeapon())
		return;

	Vector src3D, dst3D, forward, right, up, src, dst;
	float back_two, right_two, left_two;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	Math::AngleVectors(Vector(0, GetBackwardYaw(player), 0), &forward, &right, &up);

	filter.pSkip = player;
	src3D = player->GetEyePosition();
	dst3D = src3D + (forward * 384);

	ray.Init(src3D, dst3D);
	interfaces.trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	back_two = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);
	interfaces.trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	right_two = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);
	interfaces.trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	left_two = (tr.endpos - tr.startpos).Length();

	if (left_two > right_two) {
		*side = 1;
	}
	else if (right_two > left_two) {
		*side = -1;
	}
	else
		*side = 1;
}

Vector GetHitboxPos(IBasePlayer* player, matrix* mat, int hitbox_id)
{
	if (!player)
		return Vector();

	auto hdr = interfaces.models.model_info->GetStudioModel(player->GetModel());

	if (!hdr)
		return Vector();

	auto hitbox_set = hdr->pHitboxSet(player->GetHitboxSet());

	if (!hitbox_set)
		return Vector();

	auto hitbox = hitbox_set->pHitbox(hitbox_id);

	if (!hitbox)
		return Vector();

	Vector min, max;

	Math::VectorTransform(hitbox->bbmin, mat[hitbox->bone], min);
	Math::VectorTransform(hitbox->bbmax, mat[hitbox->bone], max);

	return (min + max) * 0.5f;
}
/*
void CResolver::StoreAntifreestand()
{
	if (!csgo->local->isAlive())
		return;

	if (!csgo->weapon->IsGun())
		return;

	if (!vars.ragebot.resolver)
		return;

	static int delay_ticks[65] = { 0 };
	for (int i = 0; i < interfaces.engine->GetMaxClients(); ++i)
	{
		auto player = interfaces.ent_list->GetClientEntity(i);

		if (!player || !player->isAlive() || player->IsDormant()
			|| player->GetTeam() == csgo->local->GetTeam() || csgo->actual_misses[player->GetIndex()] > 0) {
			UseFreestandAngle[i] = false;
			continue;
		}

		static int last_side = 0;

		auto idx = player->GetIndex();

		auto latest_animation = g_Animfix->get_latest_animation(player);
		if (!latest_animation)
			continue;

		auto point = Ragebot::Get().GetAdvancedHeadPoints(player, latest_animation->unresolved_bones);

		Ragebot::Get().BackupPlayer(latest_animation);
		Ragebot::Get().SetAnims(latest_animation, latest_animation->unresolved_bones);

		float left_damage = g_AutoWall.Think(point[0], player, HITGROUP_HEAD).m_damage;
		float right_damage = g_AutoWall.Think(point[1], player, HITGROUP_HEAD).m_damage;

		Ragebot::Get().RestorePlayer(latest_animation);

		if (left_damage > right_damage)
			FreestandSide[idx] = 1;
		else if (left_damage < right_damage)
			FreestandSide[idx] = -1;
	}
}*/

bool DoesHaveFakeAngles(IBasePlayer* player) {
	static int skip_ticks[65] = { 0 };
	int idx = player->GetIndex();
	if (player->GetSimulationTime() == player->GetOldSimulationTime())
		skip_ticks[idx]++;
	else
		skip_ticks[idx] = 0;
	return skip_ticks[idx] >= 16;
}

void AntiFreestand(IBasePlayer* player, int* fside)
{
	if (!csgo->local || !csgo->local->isAlive())
		return;

	resolver->DetectSide(player, fside);

	//autowall antifreestand might be whats murdering our performance (entities without eyeLBY or records -> autowall LOTS of times for no reason)

	/*
	if (!csgo->local || !csgo->local->isAlive() || !csgo->local->GetWeapon() || !csgo->local->GetWeapon()->GetCSWpnData())
		return;



	bool Autowalled = false, HitSide1 = false, HitSide2 = false;
	auto idx = player->GetIndex();
	float angToLocal = Math::CalculateAngle(interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer())->GetOrigin(), player->GetOrigin()).y;
	Vector ViewPoint = interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer())->GetOrigin() + Vector(0, 0, 90);
	Vector2D Side1 = { (45 * sin(DEG2RAD(angToLocal))),(45 * cos(DEG2RAD(angToLocal))) };
	Vector2D Side2 = { (45 * sin(DEG2RAD(angToLocal + 180))) ,(45 * cos(DEG2RAD(angToLocal + 180))) };

	Vector2D Side3 = { (50 * sin(DEG2RAD(angToLocal))),(50 * cos(DEG2RAD(angToLocal))) };
	Vector2D Side4 = { (50 * sin(DEG2RAD(angToLocal + 180))) ,(50 * cos(DEG2RAD(angToLocal + 180))) };

	Vector Origin = player->GetOrigin();

	Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

	Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

	for (int side = 0; side < 2; side++)
	{
		Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
		Vector ViewPointAutowall = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

		if (g_AutoWall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
		{
			if (side == 0)
			{
				HitSide1 = true;
				*fside = 1;
			}
			else if (side == 1)
			{
				HitSide2 = true;
				*fside = -1;
			}

			Autowalled = true;
		}
		else
		{
			for (int sidealternative = 0; sidealternative < 2; sidealternative++)
			{
				Vector ViewPointAutowallalternative = { Origin.x + OriginLeftRight[sidealternative].x,  Origin.y - OriginLeftRight[sidealternative].y , Origin.z + 90 };

				if (g_AutoWall.CanHitFloatingPoint(ViewPointAutowallalternative, ViewPointAutowall))
				{
					if (sidealternative == 0)
					{
						HitSide1 = true;
						*fside = 1;
					}
					else if (sidealternative == 1)
					{
						HitSide2 = true;
						*fside = -1;
					}

					Autowalled = true;
				}
			}
		}
	}
	if (!Autowalled)
		resolver->DetectSide(player, fside);
		*/
}

//-- overriden fnc since diff between slapstick and this is ~ > 0.3f while fnc crashes frequently --//
float buildServerAbsoluteYaw(IBasePlayer* m_player, float angle)
{
	return angle;
	/*
	Vector velocity = m_player->GetVelocity();
	auto anim_state = m_player->GetPlayerAnimState();
	float resolvedAngle = angle;
	float m_flGoalFeetYaw = 0.f;

	float eye_feetyaw_delta = AngleDiff(resolvedAngle, m_flGoalFeetYaw);

	static auto GetSmoothedVelocity = [](float min_delta, Vector a, Vector b) {
		Vector delta = a - b;
		float delta_length = delta.Length();

		if (delta_length <= min_delta)
		{
			Vector result;

			if (-min_delta <= delta_length)
				return a;
			else
			{
				float iradius = 1.0f / (delta_length + FLT_EPSILON);
				return b - ((delta * iradius) * min_delta);
			}
		}
		else
		{
			float iradius = 1.0f / (delta_length + FLT_EPSILON);
			return b + ((delta * iradius) * min_delta);
		}
	};

	float spd = velocity.LengthSqr();

	Vector velocity_processed;

	if (spd > std::powf(1.2f * 260.0f, 2.f))
	{
		velocity.Normalize();
		velocity_processed = velocity * (1.2f * 260.0f);
	}

	float m_flLastUpdateIncrement = anim_state->m_flLastUpdateTime;
	float duckLandingAdditive = std::clamp(m_player->GetDuckAmount() + anim_state->m_flDuckAdditional, 0.0f, 1.0f);
	float duckAmt = anim_state->m_flAnimDuckAmount;
	float choke = m_flLastUpdateIncrement * 6.0f;
	float fChokeDuck;

	// rebuilt clamping function, i hate pseudo
	if ((duckLandingAdditive - duckAmt) <= choke)
	{
		if (-choke <= (duckLandingAdditive - duckAmt))
			fChokeDuck = duckLandingAdditive;
		else
			fChokeDuck = duckAmt - choke;
	}
	else
	{
		fChokeDuck = duckAmt + choke;
	}

	float flDuckAmount = std::clamp(fChokeDuck, 0.0f, 1.0f);

	Vector animationVelocity = GetSmoothedVelocity(m_flLastUpdateIncrement * 2000.0f, velocity_processed, m_player->GetVelocity());
	float speed = std::fminf(animationVelocity.Length(), 260.0f);

	float flMaxMovementSpeed = 260.0f;

	auto pWeapon = m_player->GetWeapon();

	if (pWeapon && pWeapon->GetCSWpnData())
		flMaxMovementSpeed = std::fmaxf(pWeapon->GetCSWpnData()->m_flMaxSpeedAlt, 0.001f);

	float flRunningSpeed = speed / (flMaxMovementSpeed * 0.520f);
	float flDuckingSpeed = speed / (flMaxMovementSpeed * 0.340f);

	flRunningSpeed = std::clamp(flRunningSpeed, 0.0f, 1.0f);

	float flAimMatrixWidthRange = (((anim_state->m_flWalkToRunTransition * -0.30000001) - 0.19999999) * flRunningSpeed) + 1.0f;

	if (flDuckAmount > 0.0f)
	{
		float flDuckingSpeed = std::clamp(flDuckingSpeed, 0.0f, 1.0f);
		flAimMatrixWidthRange += (flDuckAmount * flDuckingSpeed) * (0.5f - flAimMatrixWidthRange);
	}

	float flTempYawMin = m_flAimYawMin * flAimMatrixWidthRange;
	float flTempYawMax = m_flAimYawMax * flAimMatrixWidthRange;

	if (eye_feetyaw_delta <= flTempYawMax)
	{
		if (flTempYawMin > eye_feetyaw_delta)
			m_flGoalFeetYaw = fabs(flTempYawMin) + resolvedAngle;
	}
	else
	{
		m_flGoalFeetYaw = resolvedAngle - fabs(flTempYawMax);
	}

	fnormalize_yaw(m_flGoalFeetYaw);

	if (speed > 0.1f || fabs(velocity_processed.z) > 100.0f)
	{
		//animState->stopToFullRunningFraction
		m_flGoalFeetYaw = ApproachAngle(
			resolvedAngle,
			m_flGoalFeetYaw,
			((anim_state->m_flWalkToRunTransition * 20.0f) + 30.0f)
			* m_flLastUpdateIncrement);
	}
	else
	{
		m_flGoalFeetYaw = ApproachAngle(
			resolvedAngle,
			m_flGoalFeetYaw,
			m_flLastUpdateIncrement * 100.f );
	}
	*/
	//return angle;
}

bool areSame(float a[], int n)
{
	unordered_map<float, int> m;//hash map to store the frequency of every
							 //element

	for (int i = 0; i < n; i++)
		m[a[i]]++;

	if (m.size() == 1)
		return true;
	else
		return false;
}

std::optional<float> CResolver::Do(IBasePlayer* player, animation* curRecord, animation* prev, float serverGFY) {
	static int offresolver_ticks[65] = { 0 };
	static float last_velocity[65] = { 0.f };
	static int ticks_with_zero_pitch[65] = { 0.f };
	int idx = player->GetIndex();

	float finalFoolYaw = 0.f;

	if (player->GetPlayerInfo().fakeplayer) {
		ResolverMode[idx] = crypt_str("OFF");
		return std::nullopt;
	}
	auto animstate = player->GetPlayerAnimState();
	if (!animstate) {
		ResolverMode[idx] = crypt_str("OFF");
		return std::nullopt;
	}

	if (player->HasGunGameImmunity()) {
		ResolverMode[idx] = crypt_str("OFF");
		return std::nullopt;
	}

	if (!vars.ragebot.resolver || !csgo->local->isAlive()) {
		ResolverMode[idx] = crypt_str("OFF");
		return std::nullopt;
	}

	if (player->GetChokedPackets() < 1)
	{
		ResolverMode[idx] = crypt_str("OFF");
		return std::nullopt;
	}

	if (curRecord->updateDelay < 1) {
		ResolverMode[idx] = crypt_str("OFF");
		return std::nullopt;
	}

	if (player->HasGunGameImmunity()) {
		ResolverMode[idx] = crypt_str("OFF");
		return std::nullopt;
	}

	auto nBalanceAdjust = player->GetAnimOverlays()[ANIMATION_LAYER_ADJUST].m_flWeight == 0.0f && player->GetAnimOverlays()[ANIMATION_LAYER_ADJUST].m_flCycle == 0.0f;
	/*
	if (curRecord->didshot && fabs(player->GetEyeAngles().x) < 55 && csgo->onshot_misses[idx] < 1 && !nBalanceAdjust)
	{
		ResolverMode[idx] = crypt_str("SHOT");
		return {curRecord->eye_angles.y};
	}*/

	/*
	if (player->GetEyeAngles().x < 45.f)
		ticks_with_zero_pitch[idx]++;
	else
		ticks_with_zero_pitch[idx] = 0;

	int add = 0;
	if (!shot_snapshots.empty()) {
		const auto& snapshot = shot_snapshots.front();
		const bool& dt_ready = !csgo->dt_charged && Misc::Get().dt_bullets <= 1 && csgo->weapon->isAutoSniper();
		if (dt_ready && snapshot.hitbox == 0 && snapshot.intended_damage > player->GetHealth()) {
			add = Misc::Get().dt_bullets;
		}
	}
	*/

	//int missed_shots = (csgo->actual_misses[idx] + csgo->imaginary_misses[idx] + add) % 4;
	int missed_shots = csgo->actual_misses[idx];

	float angle = player->GetEyeAngles().y;
	float desync_amount = player->GetDSYDelta();

	float estimateDelta = desync_amount;

	static float last_anim_update[64];

	if (curRecord->didshot && fabs(player->GetEyeAngles().x) < 55 && nBalanceAdjust) {
		ResolverMode[idx] = crypt_str("BALANCE-SHOT");
		if (AngleDiff(player->EyeAngles()->y, animstate->m_flFootYaw) > 35.0f)
		{
			return { curRecord->eye_angles.y + desync_amount };
		}
		else if (AngleDiff(player->EyeAngles()->y, animstate->m_flFootYaw) < -35.0f)
		{
			return { curRecord->eye_angles.y - desync_amount };
		}
		else {
			return { curRecord->eye_angles.y };
		}
	}

	if (curRecord->velocity.Length2D() <= 0.3)
	{
		ResolverMode[idx] = crypt_str("LBY");
		if (AngleDiff(player->EyeAngles()->y, animstate->m_flFootYaw) > 35.0f)
		{
			FreestandSide[idx] = 1; //res first, positive side
		}
		else if (AngleDiff(player->EyeAngles()->y, animstate->m_flFootYaw) < -35.0f)
		{
			FreestandSide[idx] = -1; //res second, negative side //test invert
		}

		if (FreestandSide[idx] != 0 && nBalanceAdjust) {
			curRecord->resolved = true;
		}

		last_anim_update[idx] = interfaces.global_vars->realtime;
		ResolverMode[idx] = +crypt_str("F");
	}
	else if (/*prev &&*/curRecord->velocity.Length2D() > 0.3)//  curRecord->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight > 0.f)
	{
		//Player is moving so lets extract some data from animlayers!

		bool shouldUpdate = false;

		//if (!prev && vars.ragebot.antiexploit && !((int)curRecord->layers[ANIMATION_LAYER_LEAN].m_flWeight * 1000.f)) {
			//shouldUpdate = false;
		//}

		if (prev && !((int)curRecord->layers[ANIMATION_LAYER_LEAN].m_flWeight * 1000.f) && (fabsf(curRecord->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight - prev->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flWeight) <= 0.01f)) {
			if (curRecord->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate > 0.f && !((int)curRecord->layers[ANIMATION_LAYER_LEAN].m_flWeight * 1000.f)) {
				shouldUpdate = true;
			}
		}

		float mCycleDelta = -FLT_MAX;
		float a_mCycleDelta[5];
		int side = 0;
		float_t flLeftDelta;
		float_t flLowLeftDelta;
		float_t flLowRightDelta;
		float_t flRightDelta;
		float_t flCenterDelta;

		//if (vars.ragebot.rmode1 || vars.ragebot.antiexploit){
		float best_move = FLT_MAX;
		float best_move2 = FLT_MAX;
		float best_move_delta = FLT_MAX;
		float move_distance = curRecord->velocity.Length2D() / (curRecord->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate / interfaces.global_vars->interval_per_tick);

		//estiminate server torso yawbody_yaw pose

		int poseSide = 0;

		for (float i = 0.f; i <= 1.0f; i += 0.005f) {
			auto body_yaw = player->m_flPoseParameter()[7];
			player->m_flPoseParameter()[7] = i;

			float dist = player->GetSequenceMoveDist_2(player->GetModelPtr(), curRecord->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_nSequence);

			float delta = std::fabsf(dist - move_distance);
			if (best_move_delta > delta) {
				best_move = i;
				best_move2 = i;
				best_move_delta = delta;
			}

			player->m_flPoseParameter()[7] = body_yaw;
		}

		//Msg(std::to_string(best_move2), color_t(255, 0, 0));

		if (best_move != FLT_MAX) {
			auto move_yaw_backup = player->m_flPoseParameter()[7];
			float dist_min = player->GetSequenceMoveDist_2(player->GetModelPtr(), curRecord->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_nSequence);

			player->m_flPoseParameter()[7] = move_yaw_backup;
			player->m_flPoseParameter()[7] = 0.5f;

			float velocityAng = RAD2DEG(atan2f(curRecord->velocity.y, curRecord->velocity.x));
			float previousvelocityAng = RAD2DEG(atan2f(prev->velocity.y, prev->velocity.x));

			float torso_1 = (best_move);
			float torso_2 = (1.0f - best_move);

			torso_1 = (torso_1 * 360.0f) + 180.f;
			torso_2 = (torso_2 * 360.0f) + 180.f;

			NormalizeAngle(torso_1);
			NormalizeAngle(torso_2);

			float delta_1 = std::fabsf(torso_1 - velocityAng);
			float delta_2 = std::fabsf(torso_2 - velocityAng);

			NormalizeAngle(delta_1);
			NormalizeAngle(delta_2);

			if (delta_1 > delta_2) {
				best_move = torso_2;
			}
			else {
				best_move = torso_1;
			}

			float serverTorsoYaw = best_move;

			if (serverTorsoYaw < 0.f)
				serverTorsoYaw += 360.f;

			if (player->GetPlayerAnimState()->m_flMoveYaw < 0.f)
				player->GetPlayerAnimState()->m_flMoveYaw += 360.f;

			NormalizeAngle(serverTorsoYaw);
			NormalizeAngle(player->GetPlayerAnimState()->m_flMoveYaw);

			float delta = Math::AngleDiff(velocityAng, previousvelocityAng);

			if (delta > 0.0f)
				poseSide = 1;
			else
				poseSide = -1;

			//estimateDelta = best_move2 * 60;
			//Msg(std::to_string(best_move2), color_t(255, 0, 0));
			//ResolverMode[idx] = crypt_str("MOVE");
		}

		/*
		auto CalculatePlaybackRate = [&](IBasePlayer* player, CAnimationLayer* anim_layer, float aim_yaw, float move_dist = -1.0f) {
			auto state = player->GetPlayerAnimState();
			auto anim_layer = &anim_layer;

			auto body_yaw = player->m_flPoseParameter()[8];
			player->m_flPoseParameter()[8] = aim_yaw;

			auto seq_dur = player->GetSequenceMoveDist((*(studiohdr_t**)((uintptr_t)player + 0x2950)), curRecord->layers[ANIMATION_LAYER_MOVEMENT_MOVE].m_nSequence);

			float v56;
			if (seq_dur <= 0.0f)
				v56 = 10.0f;
			else
				v56 = 1.0f / seq_dur;

			float v237 = 1.0f / (1.0f / v56);

			auto dist = move_dist;
			if (move_dist == -1.0f) {
				dist = player->GetSequenceMoveDist((*(studiohdr_t**)((uintptr_t)player + 0x2950)), anim_layer->m_nSequence);
			}

			if (dist * v237 <= 0.001f) {
				dist = 0.001f;
			}
			else {
				dist *= v237;
			}

			player->m_flPoseParameter()[8] = body_yaw;

			float speed = curRecord->velocity.Length2D();
			float v50 = (1.0f - (state->m_flWalkToRunTransition * 0.15f)) * ((speed / dist) * v56); // 0x11C
			float new_playback_rate = interfaces.global_vars->interval_per_tick * v50;
			return new_playback_rate;
		};
		*/
		//if (vars.ragebot.rmode2)
			//shouldUpdate = false;

		if (shouldUpdate) {
			ResolverMode[idx] = "A";

			
			float_t flLeftDelta = fabsf(curRecord->layer6_b.m_flPlaybackRate - curRecord->layer6_n.m_flPlaybackRate);
			float_t flLowLeftDelta = fabsf(curRecord->layer6_b.m_flPlaybackRate - curRecord->layer6_nl.m_flPlaybackRate);
			float_t flLowRightDelta = fabsf(curRecord->layer6_b.m_flPlaybackRate - curRecord->layer6_pl.m_flPlaybackRate);
			float_t flRightDelta = fabsf(curRecord->layer6_b.m_flPlaybackRate - curRecord->layer6_p.m_flPlaybackRate);
			float_t flCenterDelta = fabsf(curRecord->layer6_b.m_flPlaybackRate - curRecord->layer6_c.m_flPlaybackRate);

			bool inSlot = false;

			if (flCenterDelta < flRightDelta || flLeftDelta < flRightDelta || (signed int)(float)(flRightDelta * 1000.0))
			{
				if (flCenterDelta >= flLeftDelta && flRightDelta > flLeftDelta && !(signed int)(float)(flLeftDelta * 1000.0))
				{
					inSlot = true;
					// Success, record is desyncing towards positive yaw
					if (flLowRightDelta > flRightDelta) {
						FreestandSide[idx] = 1;
						ResolverMode[idx] += crypt_str("I");
						//estimateDelta = 25.f;
						LastAppliedAngle2[idx] = crypt_str("SPOSITIVELOW");
						LastAppliedAngle3[idx] = true;
						Msg("CHOOSE: PLOW", color_t(255, 0, 0));
					}
					else {
						FreestandSide[idx] = 1;
						ResolverMode[idx] += crypt_str("I");
						LastAppliedAngle2[idx] = crypt_str("SPOSITIVE");
						LastAppliedAngle3[idx] = false;
						Msg("CHOOSE: POSITIVE", color_t(255, 0, 0));
					}
				}
			}

			if (flCenterDelta < flLeftDelta || flRightDelta < flLeftDelta || (signed int)(float)(flLeftDelta * 1000.0)) {

				if (!inSlot) {
					if (flCenterDelta >= flRightDelta && flLeftDelta > flRightDelta && !(signed int)(float)(flRightDelta * 1000.0)) {
						inSlot = true;
						if (flLowLeftDelta > flLeftDelta) {
							FreestandSide[idx] = -1;
							ResolverMode[idx] += crypt_str("I");
							LastAppliedAngle2[idx] = crypt_str("SNEGATIVE");
							LastAppliedAngle3[idx] = false;
							Msg("CHOOSE: NEGATIVE", color_t(255, 0, 0));
						}
						else {
							FreestandSide[idx] = -1;
							ResolverMode[idx] += crypt_str("I");
							LastAppliedAngle2[idx] = crypt_str("SNEGATIVELOW");
							LastAppliedAngle3[idx] = true;
							Msg("CHOOSE: NLOW", color_t(255, 0, 0));
						}
					}
				}
			}

			if (!inSlot) {
				if (flCenterDelta >= flLeftDelta && flCenterDelta >= flRightDelta && flCenterDelta >= flLowRightDelta && flCenterDelta >= flLowLeftDelta) {
					FreestandSide[idx] = 0;
					ResolverMode[idx] += crypt_str("I");
					//estimateDelta = 25.f;
					LastAppliedAngle2[idx] = crypt_str("CENTRE");
					Msg("CHOOSE: CENTRE", color_t(255, 0, 0));
				}
			}
			
		}

		if (LastAppliedAngle3[idx] == true) {
			estimateDelta = player->GetDSYDelta() * 0.5f;
		}
		if (FreestandSide[idx] != 0 && FreestandSide[idx] == poseSide) {
			curRecord->resolved = true;
		}

	}

	//no side detected, we can choose to shoot centre or do a guess based on freestanding logic
	/*
	if (FreestandSide[idx] == 0) {
		AntiFreestand(player, &FreestandSide[idx]);
		ResolverMode[idx] = "TR";
	}
	*/

	//Is the player sideways to our view? If so the side doesnt even matter since we can safepoint the idiot

	/*
	const bool& sideways = false;
		fabsf(Math::NormalizeYaw(angle - GetLeftYaw(player))) < 30.f
		||
		fabsf(Math::NormalizeYaw(angle - GetRightYaw(player))) < 30.f;
		*/

		//is the player forwards toward us? if so we have to invert the side detection
	const bool& forward = false; // fabsf(Math::NormalizeYaw(angle - GetForwardYaw(player))) < 90.f && !sideways;

	resolvedForward[idx] = forward;

	//const bool& forward = false;

	switch (missed_shots % 5) {
	case 0:
		//first guess
		if (FreestandSide[idx] == 0) {
			finalFoolYaw = angle;
		}
		else {
			finalFoolYaw = angle + estimateDelta * (forward ? -FreestandSide[idx] : FreestandSide[idx]);
		}
		//ResolverMode[idx] += " 0";
		break;
	case 1:
		//try the other side
		if (FreestandSide[idx] == 0) {
			finalFoolYaw = angle - estimateDelta;
		}
		else {
			finalFoolYaw = angle - estimateDelta * (forward ? -FreestandSide[idx] : FreestandSide[idx]);
		}
		//ResolverMode[idx] += " 1";
		break;
	case 2:
		//fuck is using lowdelta
		//try the other side
		if (FreestandSide[idx] == 0) {
			finalFoolYaw = angle + estimateDelta;
		}
		else {
			finalFoolYaw = angle + (estimateDelta / 2) * (forward ? -FreestandSide[idx] : FreestandSide[idx]);
		}
		//ResolverMode[idx] += " 2";
		break;
	case 3:
		//lowdelta + side detection failed.
		if (FreestandSide[idx] == 0) {
			finalFoolYaw = angle - estimateDelta / 2;
		}
		else {
			finalFoolYaw = angle - (estimateDelta / 2) * (forward ? -FreestandSide[idx] : FreestandSide[idx]);
		}
		//ResolverMode[idx] += " 3";
		break;
	default:
		//jesus can we stop missing already? Try the middle 
		if (FreestandSide[idx] == 0) {
			finalFoolYaw = angle + estimateDelta / 2;
		}
		else {
			finalFoolYaw = serverGFY;
		}
		//ResolverMode[idx] += " 4";
		//return std::nullopt;
		break;
	}

	ResolvedAngle[idx] = finalFoolYaw; //save total angle for logging purpose

	//unused
	//last_velocity[idx] = player->GetVelocity().Length2D();
	//return the resolved footYaw
	return { finalFoolYaw };
}
