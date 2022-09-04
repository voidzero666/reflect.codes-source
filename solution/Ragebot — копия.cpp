#include "Hooks.h"
#include "Ragebot.h"
#include "RageBackTracking.h"
#include "Resolver.h"
#include "EnginePrediction.h"
#include "AntiAims.h"

std::vector<ShotSnapshot> shot_snapshots;

CWeaponConfig Ragebot::CurrentSettings() {
	if (csgo->weapon->IsAuto() && vars.ragebot.weapon[weap_type::scar].enable)
		return vars.ragebot.weapon[weap_type::scar];
	else if (csgo->weapon->GetItemDefinitionIndex() == WEAPON_SSG08 && vars.ragebot.weapon[weap_type::scout].enable)
		return vars.ragebot.weapon[weap_type::scout];
	else if (csgo->weapon->GetItemDefinitionIndex() == WEAPON_AWP && vars.ragebot.weapon[weap_type::_awp].enable)
		return vars.ragebot.weapon[weap_type::_awp];
	else if (csgo->weapon->isRifle() && vars.ragebot.weapon[weap_type::rifles].enable)
		return vars.ragebot.weapon[weap_type::rifles];
	else if (csgo->weapon->IsHeavyPistol() && vars.ragebot.weapon[weap_type::heavy_pistols].enable)
		return vars.ragebot.weapon[weap_type::heavy_pistols];
	else if (csgo->weapon->isPistol() && vars.ragebot.weapon[weap_type::pistols].enable)
		return vars.ragebot.weapon[weap_type::pistols];
	else
		return vars.ragebot.weapon[weap_type::def];
}

std::vector<int> Ragebot::GetHitboxesToScan(IBasePlayer* pEntity)
{
	std::vector< int > hitboxes;

	if (csgo->weapon->IsZeus()) {
		hitboxes.push_back((int)CSGOHitboxID::Chest);
		hitboxes.push_back((int)CSGOHitboxID::Stomach);
		hitboxes.push_back((int)CSGOHitboxID::Pelvis);
		return hitboxes;
	}

	if (current_settings.hitscan & 1)
		hitboxes.push_back((int)CSGOHitboxID::Head);
	if (current_settings.hitscan & 2)
		hitboxes.push_back((int)CSGOHitboxID::Neck);
	if (current_settings.hitscan & 4) {
		hitboxes.push_back((int)CSGOHitboxID::UpperChest);
	}

	if (current_settings.hitscan & 8)
		hitboxes.push_back((int)CSGOHitboxID::Chest);

	if (current_settings.hitscan & 16)
		hitboxes.push_back((int)CSGOHitboxID::Stomach);

	if (current_settings.hitscan & 32)
		hitboxes.push_back((int)CSGOHitboxID::Pelvis);

	if (current_settings.hitscan & 64)
	{
		hitboxes.push_back((int)CSGOHitboxID::LeftHand);
		hitboxes.push_back((int)CSGOHitboxID::RightHand);
		hitboxes.push_back((int)CSGOHitboxID::LeftLowerArm);
		hitboxes.push_back((int)CSGOHitboxID::RightLowerArm);
		hitboxes.push_back((int)CSGOHitboxID::LeftUpperArm);
		hitboxes.push_back((int)CSGOHitboxID::RightUpperArm);
	}
	if (current_settings.hitscan & 128)
	{
		hitboxes.push_back((int)CSGOHitboxID::LeftShin);
		hitboxes.push_back((int)CSGOHitboxID::RightShin);
	}
	if (current_settings.hitscan & 256) {
		hitboxes.push_back((int)CSGOHitboxID::LeftFoot);
		hitboxes.push_back((int)CSGOHitboxID::RightFoot);
	}

	return hitboxes;
}
std::vector<Vector> Ragebot::GetMultipoints(IBasePlayer* pBaseEntity, int iHitbox, matrix BoneMatrix[128])
{
	//std::vector<Vector> points;
	//const model_t* model = pBaseEntity->GetModel();
	//if (!model)
	//	return points;

	//studiohdr_t* hdr = interfaces.models.model_info->GetStudioModel(model);
	//if (!hdr)
	//	return points;

	//mstudiohitboxset_t* set = hdr->pHitboxSet(0);
	//if (!set)
	//	return points;

	//mstudiobbox_t* bbox = set->pHitbox(iHitbox);
	//if (!bbox)
	//	return points;

	//// get hitbox scales.
	//float scale = current_settings.pointscale_head * 0.01f;

	//// big inair fix.
	//if (!(pBaseEntity->GetFlags() & FL_ONGROUND))
	//	scale = 0.7f;

	//float bscale = current_settings.pointscale_body * 0.01f;

	//// these indexes represent boxes.
	//if (bbox->radius <= 0.f) {
	//	// references: 
	//	//      https://developer.valvesoftware.com/wiki/Rotation_Tutorial
	//	//      CBaseAnimating::GetHitboxBonePosition
	//	//      CBaseAnimating::DrawServerHitboxes

	//	// convert rotation angle to a matrix.
	//	matrix rot_matrix;
	//	Math::AngleMatrix(bbox->rotation, rot_matrix);

	//	// apply the rotation to the entity input space (local).
	//	matrix mat;
	//	Math::ConcatTransforms(BoneMatrix[bbox->bone], rot_matrix, mat);

	//	// extract origin from matrix.
	//	Vector origin = mat.GetOrigin();

	//	// compute raw center point.
	//	Vector center = (bbox->bbmin + bbox->bbmax) / 2.f;

	//	// the feet hiboxes have a side, heel and the toe.
	//	if (iHitbox == (int)CSGOHitboxID::RightFoot || iHitbox == (int)CSGOHitboxID::LeftFoot) {
	//		float d1 = (bbox->bbmin.z - center.z) * 0.875f;

	//		// invert.
	//		if (iHitbox == (int)CSGOHitboxID::LeftFoot)
	//			d1 *= -1.f;

	//		// side is more optimal then center.
	//		points.push_back({ center.x, center.y, center.z + d1 });


	//		// get point offset relative to center point
	//		// and factor in hitbox scale.
	//		float d2 = (bbox->bbmin.x - center.x) * scale;
	//		float d3 = (bbox->bbmax.x - center.x) * scale;

	//		// heel.
	//		points.push_back({ center.x + d2, center.y, center.z });

	//		// toe.
	//		points.push_back({ center.x + d3, center.y, center.z });
	//		
	//	}

	//	// nothing to do here we are done.
	//	if (points.empty())
	//		return points;

	//	// rotate our bbox points by their correct angle
	//	// and convert our points to world space.
	//	for (auto& p : points) {
	//		// VectorRotate.
	//		// rotate point by angle stored in matrix.
	//		p = { p.Dot(mat[0]), p.Dot(mat[1]), p.Dot(mat[2]) };

	//		// transform point to world space.
	//		p += origin;
	//	}
	//}

	//// these hitboxes are capsules.
	//else {
	//	// factor in the pointscale.
	//	float r = bbox->radius * scale;
	//	float br = bbox->radius * bscale;

	//	// compute raw center point.
	//	Vector center = (bbox->bbmin + bbox->bbmax) / 2.f;

	//	// head has 5 points.
	//	if (iHitbox == HITBOX_HEAD) {
	//		// add center.
	//		points.push_back(center);

	//		// rotation matrix 45 degrees.
	//		// https://math.stackexchange.com/questions/383321/rotating-x-y-points-45-degrees
	//		// std::cos( deg_to_rad( 45.f ) )
	//		constexpr float rotation = 0.70710678f;

	//		// top/back 45 deg.
	//		// this is the best spot to shoot at.
	//		points.push_back({ bbox->bbmax.x + (rotation * r), bbox->bbmax.y + (-rotation * r), bbox->bbmax.z });

	//		Vector right{ bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + r };

	//		// right.
	//		points.push_back(right);

	//		Vector left{ bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - r };

	//		// left.
	//		points.push_back(left);

	//		// back.
	//		points.push_back({ bbox->bbmax.x, bbox->bbmax.y - r, bbox->bbmax.z });

	//		// get animstate ptr.
	//		CCSGOPlayerAnimState* state = pBaseEntity->GetPlayerAnimState();

	//		// add this point only under really specific circumstances.
	//		// if we are standing still and have the lowest possible pitch pose.
	//		if (state && pBaseEntity->GetVelocity().Length() <= 0.1f /*&& pBaseEntity->GetEyeAngles().x <= state->m_flPitch*/) {

	//			// bottom point.
	//			points.push_back({ bbox->bbmax.x - r, bbox->bbmax.y, bbox->bbmax.z });
	//		}
	//		
	//	}

	//	// body has 5 points.
	//	else if (iHitbox == (int)CSGOHitboxID::Stomach) {
	//		// center.
	//		points.push_back(center);

	//		// back.
	//		//if (g_cfg[XOR("rage_aimbot_multipoint_stomach")].get<bool>())
	//			points.push_back({ center.x, bbox->bbmax.y - br, center.z });
	//	}

	//	else if (iHitbox == HITBOX_PELVIS || iHitbox == (int)CSGOHitboxID::UpperChest) {
	//		// back.
	//		points.push_back({ center.x, bbox->bbmax.y - r, center.z });
	//	}

	//	// other stomach/chest hitboxes have 2 points.
	//	else if (iHitbox == (int)CSGOHitboxID::Chest || iHitbox == (int)CSGOHitboxID::LowerChest) {
	//		// add center.
	//		points.push_back(center);

	//		// add extra point on back.
	//		//if (g_cfg[XOR("rage_aimbot_multipoint_chest")].get<bool>())
	//			points.push_back({ center.x, bbox->bbmax.y - r, center.z });
	//	}

	//	else if (iHitbox == (int)CSGOHitboxID::RightShin || iHitbox == (int)CSGOHitboxID::LeftShin) {
	//		// add center.
	//		points.push_back(center);

	//		// half bottom.
	//		//if (g_cfg[XOR("rage_aimbot_multipoint_legs")].get<bool>())
	//			points.push_back({ bbox->bbmax.x - (bbox->radius / 2.f), bbox->bbmax.y, bbox->bbmax.z });
	//	}

	//	else if (iHitbox == (int)CSGOHitboxID::RightThigh || iHitbox == (int)CSGOHitboxID::LeftThigh) {
	//		// add center.
	//		points.push_back(center);
	//	}

	//	// arms get only one point.
	//	else if (iHitbox == (int)CSGOHitboxID::LeftUpperArm || iHitbox == (int)CSGOHitboxID::RightUpperArm) {
	//		// elbow.
	//		points.push_back({ bbox->bbmax.x + bbox->radius, center.y, center.z });
	//	}

	//	// nothing left to do here.
	//	if (points.empty())
	//		return points;

	//	// transform capsule points.
	//	for (auto& p : points)
	//		Math::VectorTransform(p, BoneMatrix[bbox->bone], p);
	//}
	//return points;
	std::vector<Vector> vPoints;

	if (!pBaseEntity)
		return vPoints;

	studiohdr_t* pStudioModel = interfaces.models.model_info->GetStudioModel(pBaseEntity->GetModel());
	mstudiohitboxset_t* set = pStudioModel->pHitboxSet(0);

	if (!set)
		return vPoints;

	mstudiobbox_t* untransformedBox = set->pHitbox(iHitbox);
	if (!untransformedBox)
		return vPoints;

	Vector vecMin = { 0, 0, 0 };
	Math::VectorTransform_Wrapper(untransformedBox->bbmin, BoneMatrix[untransformedBox->bone], vecMin);

	Vector vecMax = { 0, 0, 0 };
	Math::VectorTransform_Wrapper(untransformedBox->bbmax, BoneMatrix[untransformedBox->bone], vecMax);

	float mod = untransformedBox->radius != -1.f ? untransformedBox->radius : 0.f;
	Vector max;
	Vector min;

	float ps = 0.75f;
	if (pBaseEntity->GetVelocity().Length() > 300.f && iHitbox > 0)
		ps = 0.f;
	else {
		if (iHitbox <= (int)CSGOHitboxID::Neck)
			ps = current_settings.pointscale_head / 100.f;
		else if (iHitbox <= (int)CSGOHitboxID::RightThigh)
			ps = current_settings.pointscale_body / 100.f;
	}

	Math::VectorTransform(untransformedBox->bbmax + mod, BoneMatrix[untransformedBox->bone], max);
	Math::VectorTransform(untransformedBox->bbmin - mod, BoneMatrix[untransformedBox->bone], min);

	auto center = (min + max) * 0.5f;
	if (ps <= 0.05f) {
		vPoints.push_back(center);
		return vPoints;
	}

	auto clamp_shit = [](float val, float min, float max) {
		if (val < min)
			return min;
		if (val > max)
			return max;
		return val;
	};
	Vector curAngles = Math::CalculateAngle(center, csgo->local->GetEyePosition());
	Vector forward;
	Math::AngleVectors(curAngles, forward);
	Vector right = forward.Cross(Vector(0, 0, 1));
	Vector left = Vector(-right.x, -right.y, right.z);
	if (iHitbox == 0) {
		for (auto i = 0; i < 4; ++i)
			vPoints.push_back(center);
		vPoints[1].x += untransformedBox->radius * clamp_shit(0.f, ps - 0.2f, 0.87f); // near left ear
		vPoints[2].x -= untransformedBox->radius * clamp_shit(0.f, ps - 0.2f, 0.87f); // near right ear
		vPoints[3].z += untransformedBox->radius * ps - 0.05f; // forehead
	}
	else if (iHitbox == (int)CSGOHitboxID::Neck)
		vPoints.push_back(center);
	else if (iHitbox == (int)CSGOHitboxID::RightThigh ||
		iHitbox == (int)CSGOHitboxID::LeftThigh ||
		iHitbox == (int)CSGOHitboxID::RightShin ||
		iHitbox == (int)CSGOHitboxID::LeftShin ||
		iHitbox == (int)CSGOHitboxID::RightFoot ||
		iHitbox == (int)CSGOHitboxID::LeftFoot) {

		if (iHitbox == (int)CSGOHitboxID::RightThigh ||
			iHitbox == (int)CSGOHitboxID::LeftThigh) {
			vPoints.push_back(center);
		}
		else if (iHitbox == (int)CSGOHitboxID::RightShin ||
			iHitbox == (int)CSGOHitboxID::LeftShin) {
			vPoints.push_back(center);
		}
		else if (iHitbox == (int)CSGOHitboxID::RightFoot ||
			iHitbox == (int)CSGOHitboxID::LeftFoot) {
			vPoints.push_back(center);
			vPoints[0].z += 2.f;
		}
	}
	else if (iHitbox == (int)CSGOHitboxID::RightHand ||
		iHitbox == (int)CSGOHitboxID::LeftHand ||
		iHitbox == (int)CSGOHitboxID::RightUpperArm ||
		iHitbox == (int)CSGOHitboxID::RightLowerArm ||
		iHitbox == (int)CSGOHitboxID::LeftUpperArm ||
		iHitbox == (int)CSGOHitboxID::LeftLowerArm) {
		vPoints.push_back(center);
	}
	else {
		for (auto i = 0; i < 3; ++i)
			vPoints.push_back(center);
		vPoints[1] += right * (untransformedBox->radius * ps);
		vPoints[2] += left * (untransformedBox->radius * ps);
	}


	return vPoints;

}
Vector Ragebot::GetPoint(IBasePlayer* pBaseEntity, int iHitbox, matrix BoneMatrix[128])
{
	std::vector<Vector> vPoints;

	if (!pBaseEntity)
		return Vector(0, 0, 0);

	studiohdr_t* pStudioModel = interfaces.models.model_info->GetStudioModel(pBaseEntity->GetModel());
	mstudiohitboxset_t* set = pStudioModel->pHitboxSet(0);

	if (!set)
		return Vector(0, 0, 0);

	mstudiobbox_t* untransformedBox = set->pHitbox(iHitbox);
	if (!untransformedBox)
		return Vector(0, 0, 0);

	Vector vecMin = { 0, 0, 0 };
	Math::VectorTransform_Wrapper(untransformedBox->bbmin, BoneMatrix[untransformedBox->bone], vecMin);

	Vector vecMax = { 0, 0, 0 };
	Math::VectorTransform_Wrapper(untransformedBox->bbmax, BoneMatrix[untransformedBox->bone], vecMax);

	float mod = untransformedBox->radius != -1.f ? untransformedBox->radius : 0.f;
	Vector max;
	Vector min;

	Math::VectorTransform(untransformedBox->bbmax + mod, BoneMatrix[untransformedBox->bone], max);
	Math::VectorTransform(untransformedBox->bbmin - mod, BoneMatrix[untransformedBox->bone], min);

	return (min + max) * 0.5f;
}
bool Ragebot::PreferCallback(IBasePlayer* pEntity)
{
	auto weapon = csgo->local->GetWeapon();
	if (!weapon)
		return false;

	if (!csgo->local->isAlive())
		return false;


	if (!pEntity->isAlive())
		return false;
	if (weapon->IsZeus())
		return true;
	if (!(pEntity->GetFlags() & FL_ONGROUND))
		return true;
	if (csgo->missedshots[pEntity->GetIndex()] > 0)
		return true;
	if (vars.ragebot.force_safepoint->active)
		return true;
	return false;
}

void Ragebot::BackupPlayer(animation* anims) {
	auto i = anims->player->GetIndex();
	backup_anims[i].origin = anims->player->GetOrigin();
	backup_anims[i].abs_origin = anims->player->GetAbsOrigin();
	backup_anims[i].obb_mins = anims->player->GetMins();
	backup_anims[i].obb_maxs = anims->player->GetMaxs();
	backup_anims[i].bone_cache = anims->player->GetBoneCache().Base();
}
void Ragebot::SetAnims(animation* anims) {
	anims->player->GetOrigin() = anims->origin;
	anims->player->SetAbsOrigin(anims->abs_origin);
	anims->player->GetMins() = anims->obb_mins;
	anims->player->GetMaxs() = anims->obb_maxs;
	anims->player->SetBoneCache(anims->bones);
}
void Ragebot::RestorePlayer(animation* anims) {
	auto i = anims->player->GetIndex();
	anims->player->GetOrigin() = backup_anims[i].origin;
	anims->player->SetAbsOrigin(backup_anims[i].abs_origin);
	anims->player->GetMins() = backup_anims[i].obb_mins;
	anims->player->GetMaxs() = backup_anims[i].obb_maxs;
	anims->player->SetBoneCache(backup_anims[i].bone_cache);
}


bool CanHitHitbox(const Vector start, const Vector end, animation* _animation, int box, matrix* bones)
{
	studiohdr_t* pStudioModel = interfaces.models.model_info->GetStudioModel(_animation->player->GetModel());
	mstudiohitboxset_t* set = pStudioModel->pHitboxSet(0);

	if (!set)
		return false;

	mstudiobbox_t* studio_box = set->pHitbox(box);
	if (!studio_box)
		return false;

	Vector min, max;

	const auto is_capsule = studio_box->radius != -1.f;

	if (is_capsule)
	{
		Math::VectorTransform(studio_box->bbmin, bones[studio_box->bone], min);
		Math::VectorTransform(studio_box->bbmax, bones[studio_box->bone], max);
		
		const auto dist = Math::segment_to_segment(start, end, min, max);

		if (dist < studio_box->radius)
			return true;
	}
	else
	{
		Ragebot::Get().BackupPlayer(_animation);
		Ragebot::Get().SetAnims(_animation);
		trace_t tr;
		Ray_t ray;

		ray.Init(start, end);
		interfaces.trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, _animation->player, &tr);
		Ragebot::Get().RestorePlayer(_animation);
		if (auto ent = tr.m_pEnt; ent)
		{
			if (ent == _animation->player) {
				if ((tr.hitgroup == HITGROUP_LEFTLEG && box == (int)CSGOHitboxID::RightFoot)
					|| (tr.hitgroup == HITGROUP_RIGHTLEG && box == (int)CSGOHitboxID::LeftFoot))
					return true;
			}
		}
	}
	return false;
}

int HitboxToHitgroup(CSGOHitboxID hitbox) {
	switch (hitbox)
	{
	case CSGOHitboxID::Head: return HITGROUP_HEAD;
	case CSGOHitboxID::Neck: return HITGROUP_GENERIC;
	case CSGOHitboxID::Pelvis:
	case CSGOHitboxID::Stomach:
		return HITGROUP_STOMACH;
	case CSGOHitboxID::LowerChest:
	case CSGOHitboxID::Chest:
	case CSGOHitboxID::UpperChest:
		return HITGROUP_CHEST;
	case CSGOHitboxID::RightShin:	
	case CSGOHitboxID::RightThigh:
	case CSGOHitboxID::RightFoot:
		return HITGROUP_RIGHTARM;
	case CSGOHitboxID::LeftThigh:
	case CSGOHitboxID::LeftShin:
	case CSGOHitboxID::LeftFoot:
		return HITGROUP_LEFTLEG;
	case CSGOHitboxID::RightUpperArm:
	case CSGOHitboxID::RightLowerArm:
	case CSGOHitboxID::RightHand:
		return HITGROUP_RIGHTLEG;
	case CSGOHitboxID::LeftHand:
	case CSGOHitboxID::LeftUpperArm:
	case CSGOHitboxID::LeftLowerArm:
		return HITGROUP_LEFTARM;

	default: return HITGROUP_GENERIC;
	}
}

CPoint bestDamage(std::vector<CPoint*> arr) {
	CPoint bestPoint{ .damage = -1.f };
	for (const auto& p : arr) 
		if (p->damage > bestPoint.damage) 
			bestPoint = *p;
	return bestPoint;
}

Vector Ragebot::HeadScan(animation* anims, int& hitbox, float& best_damage, float min_dmg) {
//	Vector best_point = Vector(0, 0, 0);
	memcpy(BoneMatrix, anims->bones, sizeof(matrix[128]));
	SetAnims(anims);
	int health = anims->player->GetHealth();
	if (min_dmg > health)
		min_dmg = health + 1;
	hitbox = 0;
	std::vector<Vector> Points = GetMultipoints(anims->player, 0, BoneMatrix);
	std::vector<CPoint*> unsafePoints, safePoints;

	for (auto Hitbox : Points) {
		auto info = g_AutoWall.Think(Hitbox, anims->player/*, HitboxToHitgroup((CSGOHitboxID)hitbox)*/);

		if (info.m_damage > min_dmg/* && info.m_damage > best_damage*/)
		{
			bool safepoint = CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, 0, anims->unresolved_bones)
				&& CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, 0, anims->inversed_bones);

			if (safepoint)
				safePoints.push_back(new CPoint{
					.position = Hitbox,
					.hitbox = hitbox,
					.damage = (float)info.m_damage,
					.lethal = info.m_damage > health,
				});
			else
				unsafePoints.push_back(new CPoint{
					.position = Hitbox,
					.hitbox = hitbox,
					.damage = (float)info.m_damage,
					.lethal = info.m_damage > health,
				});
		}
	}

	auto bestSafepoint = bestDamage(safePoints);
	if (bestSafepoint.damage != -1.f) {
		RestorePlayer(anims);
		anims->safepoints = true;
		return bestSafepoint.position;
	}
	else
	{
		if (!vars.ragebot.force_safepoint->active) {
			auto bestUnsafepoint = bestDamage(unsafePoints);
			if (bestUnsafepoint.damage != -1.f) {
				RestorePlayer(anims);
				anims->safepoints = false;
				return bestUnsafepoint.position;
			}
		}
	}
	RestorePlayer(anims);
	return Vector();
}

Vector Ragebot::PrimaryScan(animation* anims, int& hitbox, float& simtime, float& best_damage, float min_dmg) {
	memcpy(BoneMatrix, anims->bones, sizeof(matrix[128]));
	simtime = anims->sim_time;
	SetAnims(anims);

	best_damage = -1;
	const auto damage = min_dmg;
	auto best_point = Vector(0, 0, 0);
	auto health = anims->player->GetHealth();
	if (min_dmg > health)
		min_dmg = health + 1;

	static const vector<int> hitboxes = {
		(int)CSGOHitboxID::Head,
		(int)CSGOHitboxID::Stomach,
		(int)CSGOHitboxID::LeftShin,
		(int)CSGOHitboxID::RightShin,
	};

	for (auto HitboxID : hitboxes)
	{
		auto point = GetPoint(anims->player, HitboxID, BoneMatrix);
		auto info = g_AutoWall.Think(point, anims->player/*, HitboxToHitgroup((CSGOHitboxID)hitbox)*/);
		if ((info.m_damage > min_dmg && info.m_damage > best_damage))
		{
			hitbox = HitboxID;
			best_point = point;
			best_damage = info.m_damage;
		}
	}
	RestorePlayer(anims);
	return best_point;
}

Vector Ragebot::FullScan(animation* anims, int& hitbox, float& simtime, float& best_damage, float min_dmg) {
	memcpy(BoneMatrix, anims->bones, sizeof(matrix[128]));
	simtime = anims->sim_time;
	best_damage = -1;
	Vector best_point = Vector(0, 0, 0);
	SetAnims(anims);

	int health = anims->player->GetHealth();
	if (min_dmg > health)
		min_dmg = health + 1;
	auto hitboxes = GetHitboxesToScan(anims->player);


	if (current_settings.prefer_fallback || PreferCallback(anims->player)) {

		std::vector<CPoint*> unsafePoints, safePoints;
		for (auto HitboxID : hitboxes)
		{
			std::vector<Vector> Points = GetMultipoints(anims->player, HitboxID, BoneMatrix);

			for (auto Hitbox : Points) {
				auto info = g_AutoWall.Think(Hitbox, anims->player/*, HitboxToHitgroup((CSGOHitboxID)hitbox)*/);

				if (info.m_damage > min_dmg/* && info.m_damage > best_damage*/)
				{
					hitbox = HitboxID;
					bool safepoint = CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, HitboxID, anims->unresolved_bones)
						&& CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, HitboxID, anims->inversed_bones);

					if (safepoint)
						safePoints.push_back(new CPoint{
							.position = Hitbox,
							.hitbox = HitboxID,
							.damage = (float)info.m_damage,
							.lethal = info.m_damage > health,
							});
					else
						unsafePoints.push_back(new CPoint{
							.position = Hitbox,
							.hitbox = HitboxID,
							.damage = (float)info.m_damage,
							.lethal = info.m_damage > health,
							});
				}
			}
		}

		auto bestSafepoint = bestDamage(safePoints);
		if (bestSafepoint.damage != -1.f) {
			RestorePlayer(anims);
			hitbox = bestSafepoint.hitbox;
			anims->safepoints = true;
			return bestSafepoint.position;
		}
		else
		{
			if (!vars.ragebot.force_safepoint->active) {
				auto bestUnsafepoint = bestDamage(unsafePoints);
				if (bestUnsafepoint.damage != -1.f) {
					RestorePlayer(anims);
					hitbox = bestUnsafepoint.hitbox;
					anims->safepoints = false;
					return bestUnsafepoint.position;
				}
			}
		}
		if (best_damage > health + 2)
			target_lethal = true;
		RestorePlayer(anims);
		return best_point;
	}
	else {
		std::vector<CPoint*> unsafePoints, safePoints;
		for (auto HitboxID : hitboxes)
		{
			std::vector<Vector> Points = GetMultipoints(anims->player, HitboxID, BoneMatrix);
			for (int k = 0; k < Points.size(); k++)
			{
				auto info = g_AutoWall.Think(Points[k], anims->player);
				if (vars.ragebot.force_safepoint->active) {
					if ((info.m_damage > min_dmg && info.m_damage > best_damage)
						&& CanHitHitbox(csgo->local->GetEyePosition(), Points[k], anims, HitboxID, anims->unresolved_bones)
						&& CanHitHitbox(csgo->local->GetEyePosition(), Points[k], anims, HitboxID, anims->inversed_bones))
					{
						anims->safepoints = true;
						hitbox = HitboxID;
						best_point = Points[k];
						best_damage = info.m_damage;
					}
				}
				else {
					if (info.m_damage > min_dmg && info.m_damage > best_damage)
					{
						anims->safepoints = false;
						hitbox = HitboxID;
						best_point = Points[k];
						best_damage = info.m_damage;
					}
				}
			}
			
		}

		if (best_damage > health + 2)
			target_lethal = true;
		RestorePlayer(anims);
		return best_point;
	}
	return Vector(0, 0, 0);
}

Vector Ragebot::GetAimVector(IBasePlayer* pTarget, float& simtime, animation*& best_anims, int& hitbox)
{
	if (GetHitboxesToScan(pTarget).size() == 0)
		return Vector(0, 0, 0);

	float m_damage = 0.f;
	if (csgo->weapon->IsZeus()) {
		m_damage = 100.f;
	}
	else {
		if (vars.ragebot.override_dmg->active)
		{
			m_damage = current_settings.mindamage_override;
		}
		else
			m_damage = current_settings.mindamage;
	}


	const auto& latest_animation = g_Animfix->get_latest_animation(pTarget);
	auto record = latest_animation;
	if (!record || !record->player)
		return Vector(0, 0, 0);

	BackupPlayer(record);


	record = g_Animfix->get_latest_firing_animation(pTarget);
		
	if (record && record->player) {
		float damage = -1.f;
		best_anims = record;
		simtime = record->sim_time;
		Vector backshoot = HeadScan(record, hitbox, damage, m_damage);
		if (backshoot != Vector(0, 0, 0))
			return backshoot;
	}

	const auto& oldest_animation = g_Animfix->get_oldest_animation(pTarget);
	Vector latest_origin = Vector(0, 0, 0);
	float best_damage_0 = -1.f, best_damage_1 = -1.f;

	record = latest_animation;
	if (record)
	{
		latest_origin = record->origin;
		float damage = -1.f;
		Vector full = PrimaryScan(record, hitbox, simtime, damage, m_damage);
		if (full != Vector(0, 0, 0))
			best_damage_0 = damage;
	}

	record = oldest_animation;

	if (record && record->origin.DistTo(latest_animation->origin) > 25.f
		&& record->resolver_mode == latest_animation->resolver_mode) // stupid fixes, but work sometimes
	{
		float damage = -1.f;
		Vector full = PrimaryScan(record, hitbox, simtime, damage, m_damage);
		if (full != Vector(0, 0, 0))
			best_damage_1 = damage;
	}

	if (best_damage_0 >= best_damage_1)
		record = latest_animation;
	else
		record = oldest_animation;

	if (record)
	{
		float damage = -1.f;
		best_anims = record;
		return FullScan(record, hitbox, simtime, damage, m_damage);
	}
	return Vector(0, 0, 0);
}

static std::vector<std::tuple<float, float, float, float, float, float>> precomputed_seeds = {};

typedef void(*RandomSeed_t)(UINT);
RandomSeed_t m_RandomSeed = 0;
void random_seed(uint32_t seed) {
	if (m_RandomSeed == nullptr)
		m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed");
	m_RandomSeed(seed);
}

typedef float(*RandomFloat_t)(float, float);
RandomFloat_t m_RandomFloat;
float random_float(float flLow, float flHigh)
{
	if (m_RandomFloat == nullptr)
		m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat");

	return m_RandomFloat(flLow, flHigh);
}

static const int total_seeds = 255;

void build_seed_table()
{
	if (!precomputed_seeds.empty())
		return;

	for (auto i = 0; i < total_seeds; i++) {
		std::tuple<float, float, float, float, float, float> seed;
		random_seed(i + 1);

		const auto pi_seed = random_float(0.f, PI * 2);

		auto a = random_float(0.0f, 1.0f);
		auto b = random_float(0.0f, PI);
		auto c = random_float(0.0f, 1.0f);
		auto d = random_float(0.0f, PI);

		/*spread_values[i][0] = a;
		spread_values[i][1] = c;*/
		std::get<0>(seed) = a;
		std::get<1>(seed) = c;

		auto sin_b = sin(b), cos_b = cos(b);
		auto sin_d = sin(d), cos_d = cos(d);

		std::get<2>(seed) = sin_b;
		std::get<3>(seed) = cos_b;
		std::get<4>(seed) = sin_d;
		std::get<5>(seed) = cos_d;

		precomputed_seeds.emplace_back(seed);
		/*precomputed_seeds.emplace_back(random_float(0.f, 1.f),
			sin(pi_seed), cos(pi_seed));*/
	}
}

bool HitTraces(animation* _animation, const Vector position, const float chance, int box, matrix* bones)
{
	build_seed_table();

	const auto weapon = csgo->weapon;

	if (!weapon)
		return false;

	const auto info = weapon->GetCSWpnData();

	if (!info)
		return false;

	const auto studio_model = interfaces.models.model_info->GetStudioModel(_animation->player->GetModel());

	if (!studio_model)
		return false;

	// performance optimization.
	if ((csgo->local->GetEyePosition() - position).Length2D() > info->m_flRange)
		return false;

	// setup calculation parameters.
	const auto id = weapon->GetItemDefinitionIndex();
	const auto round_acc = [](const float accuracy) { return roundf(accuracy * 1000.f) / 1000.f; };
	const auto sniper = weapon->isSniper();
	const auto crouched = csgo->local->GetFlags() & FL_DUCKING;

	// calculate inaccuracy.
	const auto weapon_inaccuracy = csgo->innacuracy/*weapon->GetInaccuracy()*/;

	// calculate start and angle.
	auto start = csgo->unpred_eyepos/*csgo->local->GetEyePosition()*/;
	const auto aim_angle = Math::CalculateAngle(start, position);
	Vector forward, right, up;
	Math::AngleVectors(aim_angle, forward, right, up);

	// keep track of all traces that hit the enemy.
	auto current = 0;

	// setup calculation parameters.
	Vector total_spread, spread_angle, end;
	float inaccuracy, spread, spread_x, spread_y;
	std::tuple<float, float, float, float, float, float>* seed;

	for (auto i = 0u; i < total_seeds; i++)  // NOLINT(modernize-loop-convert)
	{
		// get seed.
		seed = &precomputed_seeds[i];

		/*

			auto inaccuracy = spread_values[i][0] * g_ctx.globals.inaccuracy;
			auto spread = spread_values[i][1] * g_ctx.globals.spread;

			auto spread_x = spread_values[i][3] * inaccuracy + spread_values[i][5] * spread;
			auto spread_y = spread_values[i][2] * inaccuracy + spread_values[i][4] * spread;

		*/

		// calculate spread.
		inaccuracy = std::get<0>(*seed) * csgo->innacuracy;
		spread = std::get<1>(*seed) * csgo->spread;

		spread_x = std::get<3>(*seed) * inaccuracy + std::get<5>(*seed) * spread;
		spread_y = std::get<2>(*seed) * inaccuracy + std::get<4>(*seed) * spread;
		//inaccuracy = std::get<0>(*seed) * weapon_inaccuracy;
		//spread_x = std::get<2>(*seed) * inaccuracy;
		//spread_y = std::get<1>(*seed) * inaccuracy;

		total_spread = (forward + right * spread_x + up * spread_y).Normalize();

		// calculate angle with spread applied.
		Math::VectorAngles(total_spread, spread_angle);

		// calculate end point of trace.
		Math::AngleVectors(spread_angle, end);
		end = start + end.Normalize() * info->m_flRange;

		// did we hit the hitbox?

		if (CanHitHitbox(start, end, _animation, box, bones))
			current++;
		
		// abort if hitchance is already sufficent.
		if (static_cast<float>(current) / static_cast<float>(total_seeds) >= chance)
			return true;

		// abort if we can no longer reach hitchance.
		if (static_cast<float>(current + total_seeds - i) / static_cast<float>(total_seeds) < chance)
			return false;
	}

	return static_cast<float>(current) / static_cast<float>(total_seeds) >= chance;
}

bool Ragebot::Hitchance(Vector Aimpoint, animation* best, int& hitbox)
{
	if (vars.ragebot.force_safepoint->active || best->safepoints) {
		return HitTraces(best, Aimpoint, csgo->weapon->IsZeus() ? 0.8f : current_settings.hitchance / 100.f, hitbox, best->bones)
			&& HitTraces(best, Aimpoint, csgo->weapon->IsZeus() ? 0.8f : current_settings.hitchance / 100.f, hitbox, best->inversed_bones)
			&& HitTraces(best, Aimpoint, csgo->weapon->IsZeus() ? 0.8f : current_settings.hitchance / 100.f, hitbox, best->unresolved_bones);
	}
	else {
		return HitTraces(best, Aimpoint, csgo->weapon->IsZeus() ? 0.8f : current_settings.hitchance / 100.f, hitbox, best->bones);
	}
}

Vector Ragebot::GetVisualHitbox(IBasePlayer* ent, int ihitbox)
{
	if (ihitbox < 0 || ihitbox > 19) return Vector(0, 0, 0);

	if (!ent) return Vector(0, 0, 0);

	const auto model = ent->GetModel();

	if (!model)
		return Vector(0, 0, 0);

	auto pStudioHdr = interfaces.models.model_info->GetStudioModel(model);

	if (!pStudioHdr)
		return Vector(0, 0, 0);

	auto hitbox = pStudioHdr->pHitbox(ihitbox, 0);

	if (!hitbox)
		return Vector(0, 0, 0);

	Vector min, max;
	Math::VectorTransform(hitbox->bbmin, ent->GetBoneCache()[hitbox->bone], min);
	Math::VectorTransform(hitbox->bbmax, ent->GetBoneCache()[hitbox->bone], max);

	auto center = (min + max) / 2.f;

	return center;
}

static bool can_not_shoot_due_to_cock = false;

bool Ragebot::IsAbleToShoot()
{
	auto time = TICKS_TO_TIME(csgo->fixed_tickbase);

	if (!csgo->local || !csgo->local->GetWeapon())
		return false;

	if (csgo->cmd->weaponselect)
	{
		return false;
	}

	if (!csgo->weapon)
	{
		return false;
	}

	const auto info = csgo->weapon->GetCSWpnData();

	if (!info)
	{
		return false;
	}

	const auto is_zeus = csgo->weapon->GetItemDefinitionIndex() == weapon_taser;
	const auto is_knife = !is_zeus && info->m_iWeaponType == WEAPONTYPE_KNIFE;

	if (csgo->weapon->GetItemDefinitionIndex() == weapon_c4 || !csgo->weapon->IsGun())
	{
		return false;
	}

	if (csgo->weapon->GetAmmo(false) < 1 && !is_knife)
	{
		return false;
	}

	if (csgo->weapon->InReload())
	{
		return false;
	}

	if ((csgo->local->m_flNextAttack() > time /*&& !CanDT()*/)
		|| csgo->weapon->NextPrimaryAttack() > time
		|| csgo->weapon->NextSecondaryAttack() > time)
	{
		//if (csgo->weapon->GetItemDefinitionIndex() != weapon_revolver && info->m_iWeaponType == WEAPONTYPE_PISTOL)
		//	csgo->cmd->buttons &= ~IN_ATTACK;

		return false;
	}

	return true;
}

float Ragebot::LerpTime() {
	static auto cl_interp = interfaces.cvars->FindVar(hs::cl_interp.s().c_str());
	static auto cl_updaterate = interfaces.cvars->FindVar(hs::cl_updaterate.s().c_str());
	const auto update_rate = cl_updaterate->GetInt();
	const auto interp_ratio = cl_interp->GetFloat();

	auto lerp = interp_ratio / update_rate;

	if (lerp <= interp_ratio)
		lerp = interp_ratio;

	return lerp;
};

void Ragebot::DropTarget()
{
	target_index = -1;
	best_distance = INT_MAX;
	current_aim_position = Vector();
	shot = false;
	csgo->should_stop_slide = false;
	csgo->last_forced_tickcount = -1;
	csgo->should_stop = false;
	g_AutoWall.reset();
}

bool CockRevolver()
{
	if (!vars.ragebot.enable)
		return false;

	if (!csgo->local)
		return false;

	static auto r8cock_flag = true;
	static auto r8cock_time = 0.0f;

	//ZALUPA
	float REVOLVER_COCK_TIME = 0.2421875f;
	const int count_needed = floor(REVOLVER_COCK_TIME / interfaces.global_vars->interval_per_tick);
	static int cocks_done = 0;

	if (!csgo->weapon ||
		csgo->weapon->GetItemDefinitionIndex() != WEAPON_REVOLVER ||
		csgo->weapon->NextPrimaryAttack() > interfaces.global_vars->curtime)
	{
		if (csgo->weapon && csgo->weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
			csgo->cmd->buttons &= ~IN_ATTACK;
		Ragebot::Get().shot = false;
		csgo->weapon_struct.work = false;
		return false;
	}

	csgo->weapon_struct.work = true;

	if (cocks_done < count_needed)
	{
		csgo->cmd->buttons |= IN_ATTACK;
		++cocks_done;
		return false;
	}
	else
	{
		csgo->cmd->buttons &= ~IN_ATTACK;
		cocks_done = 0;
		return true;
	}

	csgo->cmd->buttons |= IN_ATTACK;
	float curtime = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
	static float next_shoot_time = 0.f;

	bool ret = false;

	if (fabsf(next_shoot_time - curtime) < 0.5)
		next_shoot_time = curtime + 0.2f - interfaces.global_vars->interval_per_tick; // -1 because we already cocked THIS tick ???

	if (next_shoot_time - curtime - interfaces.global_vars->interval_per_tick <= 0.f)
	{
		next_shoot_time = curtime + 0.2f;
		ret = true;
		// should still go for one more tick but if we do, we're gonna shoot sooo idk how2do rn, its late
		// the aimbot should decide whether to shoot or not yeh
	}
	return ret;
}

string HitboxToString(int id)
{
	switch (id)
	{
	case 0: return hs::head.s();  break;
	case 1: return hs::neck.s(); break;
	case 2: return hs::pelvis.s(); break;
	case 3: return hs::stomach.s(); break;
	case 4: return hs::lower_chest.s(); break;
	case 5: return hs::chest.s(); break;
	case 6: return hs::upper_chest.s(); break;
	case 7: return hs::right_thigh.s(); break;
	case 8: return hs::left_thigh.s(); break;
	case 9: return hs::right_leg.s(); break;
	case 10: return hs::left_leg.s(); break;
	case 11: return hs::right_foot.s(); break;
	case 12: return hs::left_foot.s(); break;
	case 13: return hs::right_hand.s(); break;
	case 14: return hs::left_hand.s(); break;
	case 15: return hs::right_upper_arm.s(); break;
	case 16: return hs::right_lower_arm.s(); break;
	case 17: return hs::left_upper_arm.s(); break;
	case 18: return hs::left_lower_arm.s(); break;
		break;
	}
}

string ShotSnapshot::get_info() {
	string ret;
#ifdef _DEBUG
	ret += hs::H_.s() + hitbox_where_shot + hs::prefix_end.s();
	if (vars.ragebot.resolver && resolver.size() > 0)
		ret += hs::R_.s() + resolver + hs::prefix_end.s();
	ret += hs::B_.s() + std::to_string(backtrack) + hs::prefix_end.s();
	if (record.didshot)
		ret += hs::SHOT.s();
	if (record.safepoints)
		ret += hs::sp.s();

#endif

	return ret;
}

void Ragebot::FastStop() {
	auto wpn_info = csgo->weapon->GetCSWpnData();
	if (!wpn_info)
		return;
	auto get_standing_accuracy = [&]() -> const float
	{
		const auto max_speed = csgo->weapon->GetZoomLevel() > 0 ? wpn_info->m_flMaxSpeedAlt : wpn_info->m_flMaxSpeed;
		return max_speed / 3.f;
	};
	auto velocity = csgo->local->GetVelocity();
	float speed = velocity.Length2D();
	if (speed <= get_standing_accuracy())
		return;

	float max_speed = (csgo->weapon->GetZoomLevel() == 0 ? wpn_info->m_flMaxSpeed : wpn_info->m_flMaxSpeedAlt) * 0.1f;
	if (speed > max_speed) {
		csgo->should_stop_slide = false;
	}
	else {
		csgo->should_stop_slide = true;
		return;
	}

	Vector direction;
	Math::VectorAngles(velocity, direction);
	direction.y = csgo->original.y - direction.y;
	Vector forward;
	Math::AngleVectors(direction, forward);
	Vector negated_direction = forward * -speed;
	csgo->cmd->forwardmove = negated_direction.x;
	csgo->cmd->sidemove = negated_direction.y;
}

int Ragebot::GetTicksToShoot() {
	if (Ragebot::Get().IsAbleToShoot())
		return -1;
	auto flServerTime = csgo->fixed_tickbase;
	auto flNextPrimaryAttack = csgo->local->GetWeapon()->NextPrimaryAttack();

	return TIME_TO_TICKS(fabsf(flNextPrimaryAttack - flServerTime));
}

int Ragebot::GetTicksToStop() {
	static auto predict_velocity = [](Vector* velocity)
	{
		float speed = velocity->Length2D();
		static auto sv_friction = interfaces.cvars->FindVar(hs::sv_friction.s().c_str());
		static auto sv_stopspeed = interfaces.cvars->FindVar(hs::sv_stopspeed.s().c_str());

		if (speed >= 1.f)
		{
			float friction = sv_friction->GetFloat();
			float stop_speed = std::max< float >(speed, sv_stopspeed->GetFloat());
			float time = std::max< float >(interfaces.global_vars->interval_per_tick, interfaces.global_vars->frametime);
			*velocity *= std::max< float >(0.f, speed - friction * stop_speed * time / speed);
		}
	};
	Vector vel = csgo->local->GetVelocity()/*csgo->vecUnpredictedVel*/;
	int ticks_to_stop = 0;
	while (true)
	{
		if (vel.Length2D() < 1.f)
			break;
		predict_velocity(&vel);
		ticks_to_stop++;
	}
	return ticks_to_stop;
}

bool Ragebot::HoldFiringAnimation() {
	return (csgo->weapon && !IsAbleToShoot() &&
		fabsf(csgo->weapon->LastShotTime() - (csgo->fixed_tickbase)) < 0.2f);
}

void Ragebot::DrawCapsule(animation* anims) {
	studiohdr_t* pStudioModel = interfaces.models.model_info->GetStudioModel(anims->player->GetModel());
	if (!pStudioModel)
		return;
	mstudiohitboxset_t* pHitboxSet = pStudioModel->pHitboxSet(0);
	if (!pHitboxSet)
		return;

	for (int i = 0; i < pHitboxSet->numhitboxes; i++)
	{
		mstudiobbox_t* pHitbox = pHitboxSet->pHitbox(i);
		if (!pHitbox)
			continue;

		Vector vMin, vMax;
		Math::VectorTransform(pHitbox->bbmin, anims->bones[pHitbox->bone], vMin);
		Math::VectorTransform(pHitbox->bbmax, anims->bones[pHitbox->bone], vMax);

		int r = vars.ragebot.shot_clr[0],
			g = vars.ragebot.shot_clr[1],
			b = vars.ragebot.shot_clr[2],
			a = vars.ragebot.shot_clr[3];

		if (pHitbox->radius != -1)
			interfaces.debug_overlay->add_capsule_overlay(vMin, vMax, pHitbox->radius, r, g, b, a, 5.f, 0, 1);
	}
}

void Ragebot::Run()
{
	auto weapon = csgo->weapon;

	if (!weapon->IsGun())
		return;

	int curhitbox;
	animation* best_anims = nullptr;
	int hitbox = -1;

	float simtime = 0;
	animation* anims = nullptr;
	int current_hitbox;
	Ragebot::Get().shot = false;
	csgo->should_stop_slide = false;

	bool in_air = !(csgo->local->GetFlags() & FL_ONGROUND);
	bool cock_revolver = CockRevolver();
	
	bool is_able_to_shoot = IsAbleToShoot() || (weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER && cock_revolver);
	current_settings = CurrentSettings();
	for (auto i = 1; i <= interfaces.global_vars->maxClients; i++)
	{
		auto pEntity = interfaces.ent_list->GetClientEntity(i);
		if (!pEntity || pEntity == csgo->local)
			continue;
		if (!pEntity->isAlive() || pEntity->GetHealth() <= 0 || pEntity->GetTeam() == csgo->local->GetTeam()) {
			csgo->missedshots[pEntity->GetIndex()] = 0;
			continue;
		}
		if (pEntity->IsDormant() || pEntity->HasGunGameImmunity())
			continue;

		target_lethal = false;

		Vector aim_position = GetAimVector(pEntity, simtime, anims, current_hitbox);

		if (!anims)
			continue;
		int health = pEntity->GetHealth();
		if (best_distance > health
			&& anims->player == pEntity && aim_position != Vector(0, 0, 0))
		{
			best_distance = health;
			target_index = i;
			current_aim_position = aim_position;
			current_aim_simulationtime = simtime;
			best_anims = anims;
			hitbox = current_hitbox;
		}
	}

	if (hitbox != -1 && target_index != -1 && best_anims && current_aim_position != Vector(0, 0, 0))
	{
		auto can_shoot_if_fakeduck = csgo->duck_amount > 0.9f || csgo->duck_amount < 0.1f;
		bool hitchance = Hitchance(current_aim_position, best_anims, hitbox);

		if (vars.ragebot.autoscope == 1) {
			if (weapon->isSniper() && csgo->weapon->GetZoomLevel() == 0)
			{
				csgo->cmd->buttons |= IN_ATTACK2;
				return;
			}
		}
		else if (vars.ragebot.autoscope == 2) {
			if (weapon->isSniper() && csgo->weapon->GetZoomLevel() == 0 && !hitchance)
			{
				csgo->cmd->buttons |= IN_ATTACK2;
				return;
			}
		}

		csgo->should_stop_slide = false;

		if (csgo->local->GetFlags() & FL_ONGROUND && !vars.antiaim.slowwalk->active) {
			if (!csgo->weapon->IsZeus()) {
				bool should_stop = GetTicksToShoot() <= GetTicksToStop()
					|| (current_settings.quickstop_options & 1) && !is_able_to_shoot;
				if (should_stop && current_settings.quickstop)
				{
					if (!csgo->should_stop_slide)
						FastStop();
					csgo->should_stop_slide = true;
				}
				else
					csgo->should_stop_slide = false;
			}
		}

		if (vars.ragebot.autoshoot) {
		
			if (hitchance && can_shoot_if_fakeduck && is_able_to_shoot)
			{
				if (!csgo->fake_duck) {
					csgo->send_packet = true;
					csgo->max_fakelag_choke = CanDT() ? 1 : 2;
				}

				csgo->cmd->buttons |= IN_ATTACK;	
			}
		}
		if (hitchance && can_shoot_if_fakeduck && is_able_to_shoot)
		{
			if (csgo->cmd->buttons & IN_ATTACK) {
				csgo->last_forced_tickcount = csgo->cmd->tick_count;
				csgo->cmd->viewangles = Math::CalculateAngle(csgo->local->GetEyePosition(), current_aim_position);

				float last_pitch = csgo->cmd->viewangles.x;
				if (!vars.misc.antiuntrusted) {
					csgo->cmd->viewangles.y += 180.f;
					csgo->cmd->viewangles.x += 180.f - last_pitch * 2.f;
					csgo->cmd->viewangles -= csgo->local->GetPunchAngle() * 2.f;
				}
				else
					csgo->cmd->viewangles -= csgo->local->GetPunchAngle() * 2.f;

				csgo->cmd->tick_count = TIME_TO_TICKS(best_anims->sim_time + LerpTime());
				ShotSnapshot snapshot;
				snapshot.entity = best_anims->player;
				snapshot.hitbox_where_shot = HitboxToString(hitbox);
				snapshot.resolver = ResolverMode[best_anims->player->GetIndex()];
				snapshot.time = interfaces.global_vars->interval_per_tick * csgo->local->GetTickBase();
				snapshot.first_processed_time = 0.f;
				snapshot.bullet_impact = false;
				snapshot.weapon_fire = false;
				snapshot.damage = -1;
				snapshot.start = csgo->local->GetEyePosition();
				snapshot.hitgroup_hit = -1;
				snapshot.backtrack = TIME_TO_TICKS(fabsf(best_anims->player->GetSimulationTime() - current_aim_simulationtime));
				snapshot.eyeangles = Math::normalize(best_anims->player->GetEyeAngles());
				snapshot.hitbox = hitbox;
				snapshot.record = *best_anims;
				shot_snapshots.push_back(snapshot);
				shot = true;
				csgo->firedshots[best_anims->player->GetIndex()]++;
#ifdef _DEBUG
				if (vars.ragebot.shotrecord)
					DrawCapsule(best_anims);
#endif
			}
		}
	}
	if (is_able_to_shoot && csgo->cmd->buttons & IN_ATTACK)
		shot = true;
}