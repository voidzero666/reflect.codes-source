#include "Hooks.h"
#include "Ragebot.h"
#include "RageBackTracking.h"
#include "Resolver.h"
#include "EnginePrediction.h"
#include "AntiAims.h"
#include "Misc.h"

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

	if (vars.ragebot.baim.active) {
		hitboxes.emplace_back((int)CSGOHitboxID::Chest);
		hitboxes.emplace_back((int)CSGOHitboxID::Stomach);
		hitboxes.emplace_back((int)CSGOHitboxID::Pelvis);
		hitboxes.emplace_back((int)CSGOHitboxID::UpperChest);
		return hitboxes;
	}


	if (csgo->weapon->IsZeus()) {
		hitboxes.emplace_back((int)CSGOHitboxID::Chest);
		hitboxes.emplace_back((int)CSGOHitboxID::Stomach);
		hitboxes.emplace_back((int)CSGOHitboxID::Pelvis);
		return hitboxes;
	}

	if (current_settings.hitscan & 1)
		hitboxes.emplace_back((int)CSGOHitboxID::Head);

	if (current_settings.hitscan & 2) {
		hitboxes.emplace_back((int)CSGOHitboxID::Chest);
		hitboxes.emplace_back((int)CSGOHitboxID::Stomach);
		hitboxes.emplace_back((int)CSGOHitboxID::Pelvis);
		hitboxes.emplace_back((int)CSGOHitboxID::UpperChest);
	}

	if (csgo->weapon->isSniper() && !csgo->local->IsScoped())
		return hitboxes;

	if (current_settings.hitscan & 4) {
		hitboxes.emplace_back((int)CSGOHitboxID::LeftHand);
		hitboxes.emplace_back((int)CSGOHitboxID::RightHand);
		hitboxes.emplace_back((int)CSGOHitboxID::LeftLowerArm);
		hitboxes.emplace_back((int)CSGOHitboxID::RightLowerArm);
		hitboxes.emplace_back((int)CSGOHitboxID::LeftUpperArm);
		hitboxes.emplace_back((int)CSGOHitboxID::RightUpperArm);
	}

	if (current_settings.hitscan & 8) {
		hitboxes.emplace_back((int)CSGOHitboxID::LeftShin);
		hitboxes.emplace_back((int)CSGOHitboxID::RightShin);
		hitboxes.emplace_back((int)CSGOHitboxID::LeftFoot);
		hitboxes.emplace_back((int)CSGOHitboxID::RightFoot);
	}

	return hitboxes;
}

// на боди 0,65 край, дальше = выходит за хитбокс

float Ragebot::GetBodyScale(IBasePlayer* player)
{
	if (!(player->GetFlags() & FL_ONGROUND))
		return 0.f;

	if (current_settings.static_scale)
		return std::clamp(current_settings.scale_body / 100.f, 0.f, 0.65f);

	auto factor = [](float x, float min, float max) {
		return 1.f - 1.f / (1.f + pow(2.f, (-([](float x, float min, float max) {
			return ((x - min) * 2.f) / (max - min) - 1.f;
			}(x, min, max)) / 0.115f)));
	}(player->GetOrigin().DistTo(csgo->local->GetEyePosition()), 0.f, csgo->weapon_range / 4.f);

	if (csgo->weapon->isSniper() && !csgo->local->IsScoped())
		factor = 0.f;

	if (csgo->local->GetDuckAmount() >= 0.9f && !vars.antiaim.fakeduck.active)
		return 0.65f;

	return std::clamp(factor, 0.f, 0.65f);
}

float Ragebot::GetHeadScale(IBasePlayer* player)
{
	if (current_settings.static_scale)
		return std::clamp(current_settings.scale_head / 100.f, 0.f, 0.80f);

	if (!vars.misc.antiuntrusted)
		return 0.90f;

	if (vars.antiaim.fakeduck.active)
		return 0.70f;

	if (player->GetFlags() & FL_ONGROUND)
		return GetBodyScale(player);
	else
		return 0.75f;
}

std::vector<Vector> Ragebot::GetMultipoints(IBasePlayer* pBaseEntity, int iHitbox, matrix bones[128])
{
	std::vector<Vector> points;

	const model_t* model = pBaseEntity->GetModel2();
	if (!model)
		return points;

	studiohdr_t* hdr = interfaces.models.model_info->GetStudioModel(model);
	if (!hdr)
		return points;

	mstudiohitboxset_t* set = hdr->pHitboxSet(pBaseEntity->GetHitboxSet());
	if (!set)
		return points;

	mstudiobbox_t* bbox = set->pHitbox(iHitbox);
	if (!bbox)
		return points;

	// these indexes represent boxes.
	if (bbox->radius <= 0.f) {
		// references: 
		//      https://developer.valvesoftware.com/wiki/Rotation_Tutorial
		//      CBaseAnimating::GetHitboxBonePosition
		//      CBaseAnimating::DrawServerHitboxes

		// convert rotation angle to a matrix.
		matrix rot_matrix;
		Math::AngleMatrix(bbox->rotation, rot_matrix);

		// apply the rotation to the entity input space (local).
		matrix mat;
		Math::ConcatTransforms(bones[bbox->bone], rot_matrix, mat);

		// extract origin from matrix.
		Vector origin = mat.GetOrigin();

		// compute raw center point.
		Vector center = (bbox->bbmin + bbox->bbmax) / 2.f;

		// the feet hiboxes have a side, heel and the toe.
		if (iHitbox == (int)CSGOHitboxID::RightFoot || iHitbox == (int)CSGOHitboxID::LeftFoot) {
			float d1 = (bbox->bbmin.z - center.z) * 0.875f;

			// invert.
			if (iHitbox == iHitbox == (int)CSGOHitboxID::LeftFoot)
				d1 *= -1.f;

			// side is more optimal then center.
			points.emplace_back(Vector{ center.x, center.y, center.z + d1 });

			float d2 = (bbox->bbmin.x - center.x) * 0.5f;
			float d3 = (bbox->bbmax.x - center.x) * 0.5f;

			// heel.
			points.emplace_back(Vector{ center.x + d2, center.y, center.z });

			// toe.
			points.emplace_back(Vector{ center.x + d3, center.y, center.z });

		}

		// nothing to do here we are done.
		if (points.empty())
			return points;

		// rotate our bbox points by their correct angle
		// and convert our points to world space.
		for (auto& p : points) {
			// VectorRotate.
			// rotate point by angle stored in matrix.
			p = { p.Dot(mat[0]), p.Dot(mat[1]), p.Dot(mat[2]) };

			// transform point to world space.
			p += origin;
		}
	}

	// these hitboxes are capsules.
	else {
		// compute raw center point.
		Vector max = bbox->bbmax;
		Vector min = bbox->bbmin;
		Vector center = (bbox->bbmin + bbox->bbmax) / 2.f;

		// head has 5 points.
		if (iHitbox == (int)CSGOHitboxID::Head) {
			// add center.
			float r = bbox->radius * GetHeadScale(pBaseEntity);
			points.emplace_back(center);

			if (true) {
				// rotation matrix 45 degrees.
				// https://math.stackexchange.com/questions/383321/rotating-x-y-points-45-degrees
				// std::cos( deg_to_rad( 45.f ) )
				constexpr float rotation = 0.70710678f;

				// top/back 45 deg.
				// this is the best spot to shoot at.
				points.emplace_back(Vector{ max.x + (rotation * r), max.y + (-rotation * r), max.z });

				Vector right{ max.x, max.y, max.z + r };

				// right.
				points.emplace_back(right);

				Vector left{ max.x, max.y, max.z - r };

				// left.
				points.emplace_back(left);

				// back.
				points.emplace_back(Vector{ max.x, max.y - r, max.z });

				// get animstate ptr.
				CCSGOPlayerAnimState* state = pBaseEntity->GetPlayerAnimState();

				// add this point only under really specific circumstances.
				// if we are standing still and have the lowest possible pitch pose.
				if (state && pBaseEntity->GetVelocity().Length() <= 0.1f && pBaseEntity->GetEyeAngles().x <= 75.f) {

					// bottom point.
					points.emplace_back(Vector{ max.x - r, max.y, max.z });
				}
			}
		}

		// body has 5 points.
		else {
			float r = bbox->radius * GetBodyScale(pBaseEntity);

			if (iHitbox == (int)CSGOHitboxID::Stomach) {
				// center.
				points.emplace_back(center);
				points.emplace_back(Vector(center.x, center.y, min.z + r));
				points.emplace_back(Vector(center.x, center.y, max.z - r));
				// back.
				points.emplace_back(Vector{ center.x, max.y - r, center.z });
			}

			else if (iHitbox == (int)CSGOHitboxID::Pelvis || iHitbox == (int)CSGOHitboxID::UpperChest) {
				//points.emplace_back(center);
				// left & right points
				points.emplace_back(Vector(center.x, center.y, max.z + r));
				points.emplace_back(Vector(center.x, center.y, min.z - r));
			}

			// other stomach/chest hitboxes have 2 points.
			else if (iHitbox == (int)CSGOHitboxID::LowerChest || iHitbox == (int)CSGOHitboxID::Chest) {
				// left & right points
				points.emplace_back(Vector(center.x, center.y, max.z + r));
				points.emplace_back(Vector(center.x, center.y, min.z - r));
				// add extra point on back.
				points.emplace_back(Vector{ center.x, max.y - r, center.z });
			}

			else if (iHitbox == (int)CSGOHitboxID::RightShin || iHitbox == (int)CSGOHitboxID::LeftShin) {
				// add center.
				points.emplace_back(center);

				// half bottom.
				if (true/*g_cfg[XOR("rage_aimbot_multipoint_legs")].get<bool>()*/)
					points.emplace_back(Vector{ max.x - (bbox->radius / 2.f), max.y, max.z });
			}

			else if (iHitbox == (int)CSGOHitboxID::RightThigh || iHitbox == (int)CSGOHitboxID::LeftThigh) {
				// add center.
				points.emplace_back(center);
			}

			// arms get only one point.
			else if (iHitbox == (int)CSGOHitboxID::RightUpperArm || iHitbox == (int)CSGOHitboxID::LeftUpperArm) {
				// elbow.
				points.emplace_back(Vector{ max.x + bbox->radius, center.y, center.z });
			}
		}
		// nothing left to do here.
		if (points.empty())
			return points;

		// transform capsule points.
		for (auto& p : points)
			Math::VectorTransform(p, bones[bbox->bone], p);
	}

	return points;
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

uint8_t Ragebot::ScanMode(IBasePlayer* pEntity)
{
	int scan_mode = [&]() -> int {
		auto weapon = csgo->local->GetWeapon();
		if (!weapon || !pEntity->isAlive() || !csgo->local->isAlive())
			return ScanType::SCAN_DEFAULT;

		if (weapon->IsZeus())
			return ScanType::SCAN_FORCE_SAFEPOINT;

		if (vars.ragebot.baim.active)
			return ScanType::SCAN_SAFE_BAIM;

		if (vars.ragebot.force_safepoint.active)
			return ScanType::SCAN_FORCE_SAFEPOINT;

		const int& missed_shots = csgo->actual_misses[pEntity->GetIndex()] + std::clamp(csgo->imaginary_misses[pEntity->GetIndex()] - 1, 0, INT_MAX);
		if (current_settings.max_misses > 0 && missed_shots >= current_settings.max_misses)
			return missed_shots == current_settings.max_misses ? ScanType::SCAN_FORCE_SAFEPOINT : ScanType::SCAN_SAFE_BAIM;

		if (current_settings.prefer_safepoint)
			return ScanType::SCAN_PREFER_SAFEPOINT;

		return ScanType::SCAN_DEFAULT;
	}();

	if (!(current_settings.hitscan & 2))
		return clamp(scan_mode, (int)ScanType::SCAN_DEFAULT, (int)ScanType::SCAN_PREFER_SAFEPOINT);

	return scan_mode;
}

void Ragebot::BackupPlayer(animation* anims) {
	auto i = anims->player->GetIndex();
	backup_anims[i].origin = anims->player->GetOrigin();
	backup_anims[i].abs_origin = anims->player->GetAbsOrigin();
	backup_anims[i].obb_mins = anims->player->GetMins();
	backup_anims[i].obb_maxs = anims->player->GetMaxs();
	backup_anims[i].bone_cache = anims->player->GetBoneCache().Base();
}
void Ragebot::SetAnims(animation* anims, matrix* m) {
	anims->player->GetOrigin() = anims->origin;
	anims->player->SetAbsOrigin(anims->abs_origin);
	anims->player->GetMins() = anims->obb_mins;
	anims->player->GetMaxs() = anims->obb_maxs;

	anims->player->SetBoneCache(m ? m : anims->bones);
}
void Ragebot::RestorePlayer(animation* anims) {
	auto i = anims->player->GetIndex();
	anims->player->GetOrigin() = backup_anims[i].origin;
	anims->player->SetAbsOrigin(backup_anims[i].abs_origin);
	anims->player->GetMins() = backup_anims[i].obb_mins;
	anims->player->GetMaxs() = backup_anims[i].obb_maxs;
	anims->player->SetBoneCache(backup_anims[i].bone_cache);
}

bool CompareHitboxesWithHitgroup(int hitbox, int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_HEAD: return hitbox == (int)CSGOHitboxID::Head; break;
	case HITGROUP_CHEST: return hitbox == (int)CSGOHitboxID::Chest
		|| hitbox == (int)CSGOHitboxID::UpperChest
		|| hitbox == (int)CSGOHitboxID::LowerChest;
		break;
	case HITGROUP_STOMACH: return hitbox == (int)CSGOHitboxID::Stomach || hitbox == (int)CSGOHitboxID::Pelvis; break;
	case HITGROUP_LEFTARM: return hitbox == (int)CSGOHitboxID::LeftLowerArm
		|| hitbox == (int)CSGOHitboxID::LeftUpperArm
		|| hitbox == (int)CSGOHitboxID::LeftHand; break;
	case HITGROUP_RIGHTARM: return hitbox == (int)CSGOHitboxID::RightLowerArm
		|| hitbox == (int)CSGOHitboxID::RightUpperArm
		|| hitbox == (int)CSGOHitboxID::RightHand; break;
	case HITGROUP_LEFTLEG: return hitbox == (int)CSGOHitboxID::LeftFoot
		|| hitbox == (int)CSGOHitboxID::LeftThigh
		|| hitbox == (int)CSGOHitboxID::LeftShin; break;
	case HITGROUP_RIGHTLEG: return hitbox == (int)CSGOHitboxID::RightFoot
		|| hitbox == (int)CSGOHitboxID::RightThigh
		|| hitbox == (int)CSGOHitboxID::RightShin; break;
	}
	return false;
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

		if (dist < studio_box->radius) {
			if (box == (int)CSGOHitboxID::Head) {
				Ragebot::Get().BackupPlayer(_animation);
				Ragebot::Get().SetAnims(_animation, bones);

				trace_t tr;
				Ray_t ray;
				ray.Init(start, end);
				interfaces.trace->ClipRayToEntity(ray, MASK_SHOT, _animation->player, &tr);

				Ragebot::Get().RestorePlayer(_animation);

				return tr.m_pEnt == _animation->player && tr.hitgroup == HITGROUP_HEAD && box == (int)CSGOHitboxID::Head;
			}
			return true;
		}
	}
	else
	{
		Ragebot::Get().BackupPlayer(_animation);
		Ragebot::Get().SetAnims(_animation, bones);

		trace_t tr;
		Ray_t ray;
		ray.Init(start, end);
		interfaces.trace->ClipRayToEntity(ray, MASK_SHOT, _animation->player, &tr);

		Ragebot::Get().RestorePlayer(_animation);
		if (auto ent = tr.m_pEnt; ent)
		{
			if (ent == _animation->player) {
				if (CompareHitboxesWithHitgroup(box, tr.hitgroup))
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
	case CSGOHitboxID::Neck: return HITGROUP_NECK;
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
		return HITGROUP_RIGHTLEG;
	case CSGOHitboxID::LeftThigh:
	case CSGOHitboxID::LeftShin:
	case CSGOHitboxID::LeftFoot:
		return HITGROUP_LEFTLEG;
	case CSGOHitboxID::RightUpperArm:
	case CSGOHitboxID::RightLowerArm:
	case CSGOHitboxID::RightHand:
		return HITGROUP_RIGHTARM;
	case CSGOHitboxID::LeftHand:
	case CSGOHitboxID::LeftUpperArm:
	case CSGOHitboxID::LeftLowerArm:
		return HITGROUP_LEFTARM;

	default: return HITGROUP_GENERIC;
	}
}

static const auto& is_baim_hitbox = [](int id) {
	static auto baim_hitbox = {
		CSGOHitboxID::Pelvis,
		CSGOHitboxID::Stomach,
		CSGOHitboxID::LowerChest,
		CSGOHitboxID::Chest,
		CSGOHitboxID::UpperChest,
	};
	bool found = false;
	for (const auto& hitbox : baim_hitbox) {
		if ((int)hitbox == id) {
			found = true;
			break;
		}
	}
	return found;
};

CPoint Ragebot::GetBestPoint(std::vector<CPoint> points, int health, uint8_t scan_mode) {
	switch (scan_mode)
	{
	default:
	case ScanType::SCAN_DEFAULT:
	{
		// in fact this scan mode means we're choosing points by best damage,
		// but we're checking first lethal damage in body
		CPoint bestPoint{ .damage = -1.f };
		for (const auto& p : points)
			if (p.damage > bestPoint.damage && is_baim_hitbox(p.hitbox))
				bestPoint = p;

		if (bestPoint.damage > health)
			return bestPoint;

		bestPoint.damage = -1.f;

		for (const auto& p : points)
			if (p.damage > bestPoint.damage)
				bestPoint = p;

		return bestPoint;
	};
	case ScanType::SCAN_PREFER_SAFEPOINT:
	{
		CPoint bestPoint{ .damage = -1.f };

		for (const auto& p : points) // layer 1
			if (p.damage > bestPoint.damage && p.safe && is_baim_hitbox(p.hitbox))
				bestPoint = p;

		if (bestPoint.damage > health)
			return bestPoint;

		bestPoint.damage = -1.f;

		for (const auto& p : points) // layer 2
			if (p.damage > bestPoint.damage && p.safe)
				bestPoint = p;


		if (bestPoint.damage != -1.f)
			return bestPoint;

		bestPoint.damage = -1.f;

		for (const auto& p : points) // layer 3
			if (p.damage > bestPoint.damage && is_baim_hitbox(p.hitbox))
				bestPoint = p;

		if (bestPoint.damage > health)
			return bestPoint;

		bestPoint.damage = -1.f;

		for (const auto& p : points) // layer 4
			if (p.damage > bestPoint.damage)
				bestPoint = p;

		return bestPoint;
	};
	case ScanType::SCAN_FORCE_SAFEPOINT:
	{
		CPoint bestPoint{ .damage = -1.f };

		// force safepoint is easier than prefer safepoint cuz in every scan mode
		// we're checking for lethal damage in body first
		// than we're checking other hitboxes etc.
		// so, here we're checking safe and body first, than safe & head and limbs

		for (const auto& p : points)
			if (p.damage > bestPoint.damage && p.safe && is_baim_hitbox(p.hitbox))
				bestPoint = p;

		if (bestPoint.damage > health)
			return bestPoint;

		bestPoint.damage = -1.f;

		for (const auto& p : points)
			if (p.damage > bestPoint.damage && p.safe)
				bestPoint = p;

		return bestPoint;
	};
	case ScanType::SCAN_SAFE_BAIM:
	{
		CPoint bestPoint{ .damage = -1.f };

		// this is easiest scan mode 'cause we're checking only safe points in body

		for (const auto& p : points)
			if (p.damage > bestPoint.damage && p.safe && is_baim_hitbox(p.hitbox))
				bestPoint = p;

		return bestPoint;
	};
	}
}

Vector Ragebot::HeadScan(animation* anims, int& hitbox, float& best_damage, float min_dmg) {
	memcpy(BoneMatrix, anims->bones, sizeof(matrix[128]));
	SetAnims(anims);
	int health = anims->player->GetHealth();
	if (min_dmg > health)
		min_dmg = health + 1;
	hitbox = 0;
	std::vector<Vector> Points = GetMultipoints(anims->player, 0, BoneMatrix);
	std::vector<CPoint> points;

	for (const auto& Hitbox : Points) {
		const auto& info = g_AutoWall.Think(Hitbox, anims->player, HitboxToHitgroup(CSGOHitboxID::Head));

		if (info.m_damage > min_dmg/* && info.m_damage > best_damage*/)
		{
			points.emplace_back(CPoint{
				.position = Hitbox,
				.hitbox = hitbox,
				.damage = (float)info.m_damage,
				.lethal = info.m_damage > health,
				.safe = CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, 0, anims->unresolved_bones)
					&& CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, 0, anims->inversed_bones)
				});
		}
	}

	auto scan_mode = vars.ragebot.force_safepoint.active ? ScanType::SCAN_FORCE_SAFEPOINT : ScanType::SCAN_PREFER_SAFEPOINT;
	auto best_point = GetBestPoint(points, health, scan_mode);

	RestorePlayer(anims);
	hitbox = best_point.hitbox;
	anims->safepoints = best_point.safe;
	anims->scan_mode = scan_mode;
	best_damage = best_point.damage;
	return best_point.position;
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
		(int)CSGOHitboxID::LeftFoot,
		(int)CSGOHitboxID::RightFoot,
	};

	for (auto HitboxID : hitboxes)
	{
		const auto& point = GetPoint(anims->player, HitboxID, BoneMatrix);
		const auto& info = g_AutoWall.Think(point, anims->player, HitboxToHitgroup((CSGOHitboxID)HitboxID));
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
	SetAnims(anims);

	int health = anims->player->GetHealth();
	if (min_dmg > health)
		min_dmg = health + 1;
	auto hitboxes = GetHitboxesToScan(anims->player);
	uint8_t vscan = ScanMode(anims->player);
	std::vector<CPoint> points;
	for (auto HitboxID : hitboxes)
	{
		for (const auto& Hitbox : GetMultipoints(anims->player, HitboxID, BoneMatrix)) {
			const auto& info = g_AutoWall.Think(Hitbox, anims->player, HitboxToHitgroup((CSGOHitboxID)HitboxID));
			if (info.m_damage > min_dmg)
				points.emplace_back(CPoint{
					.position = Hitbox,
					.hitbox = HitboxID,
					.damage = is_baim_hitbox(HitboxID) && dt_ready ? info.m_damage * 2.f : info.m_damage ,
					.lethal = info.m_damage > health,
					.safe = CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, HitboxID, anims->unresolved_bones)
						&& CanHitHitbox(csgo->local->GetEyePosition(), Hitbox, anims, HitboxID, anims->inversed_bones)
					});
		}
	}

	const auto& scan_mode = ScanMode(anims->player);
	const auto& best_point = GetBestPoint(points, health, scan_mode);

	RestorePlayer(anims);
	hitbox = best_point.hitbox;
	anims->safepoints = best_point.safe;
	anims->scan_mode = scan_mode;
	best_damage = best_point.damage;
	return best_point.position;
}

Vector Ragebot::GetAimVector(IBasePlayer* pTarget, float& simtime, animation*& best_anims, int& hitbox)
{
	float m_damage = 0.f;
	if (csgo->weapon->IsZeus()) {
		m_damage = 100.f;
	}
	else {
		if (vars.ragebot.override_dmg.active)
		{
			m_damage = current_settings.mindamage_override;
		}
		else
			m_damage = current_settings.mindamage;
	}


	const auto& latest_animation = g_Animfix->get_latest_animation(pTarget);
	auto record = latest_animation;
	if (!record || !record->player/* || latest_animation->resolver_mode != ResolverMode[pTarget->GetIndex()]*/)
		return Vector(0, 0, 0);

	BackupPlayer(record);


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

	Vector full_scan = Vector(0, 0, 0);
	float best_dmg = -1.f;
	if (record)
	{
		float damage = -1.f;
		best_anims = record;
		full_scan = FullScan(record, hitbox, simtime, damage, m_damage);
		best_dmg = damage;
		int health = record->player->GetHealth();

		if (damage > health) {
			best_damage = damage;
			return full_scan;
		}
	}

	auto latest_firing_animation = g_Animfix->get_latest_firing_animation(pTarget);

	if (latest_firing_animation && latest_firing_animation->player && current_settings.hitscan & 1 && !vars.ragebot.baim.active) {
		float damage = -1.f;
		Vector backshoot = HeadScan(latest_firing_animation, hitbox, damage, m_damage);
		if (backshoot != Vector(0, 0, 0)) {
			best_anims = latest_firing_animation;
			simtime = latest_firing_animation->sim_time;
			best_damage = damage;
			return backshoot;
		}
	}
	if (record) {
		simtime = record->sim_time;
		best_anims = record;
		best_damage = best_dmg;
		return full_scan;
	}
	return Vector(0, 0, 0);
}

static std::vector<std::tuple<float, float, float>> precomputed_seeds = {};

typedef void(*RandomSeed_t)(UINT);
RandomSeed_t m_RandomSeed = 0;
void random_seed(uint32_t seed) {
	if (m_RandomSeed == nullptr)
		m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle(str("vstdlib.dll")), str("RandomSeed"));
	m_RandomSeed(seed);
}

typedef float(*RandomFloat_t)(float, float);
RandomFloat_t m_RandomFloat;
float random_float(float flLow, float flHigh)
{
	if (m_RandomFloat == nullptr)
		m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle(str("vstdlib.dll")), str("RandomFloat"));

	return m_RandomFloat(flLow, flHigh);
}

static const int total_seeds = 255;

void build_seed_table()
{
	if (!precomputed_seeds.empty())
		return;

	for (auto i = 0; i < total_seeds; i++) {
		random_seed(i + 1);

		const auto pi_seed = random_float(0.f, PI * 2);

		precomputed_seeds.emplace_back(random_float(0.f, 1.f),
			sin(pi_seed), cos(pi_seed));
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

	//// performance optimization.
	//if ((csgo->local->GetEyePosition() - position).Length2D() > info->m_flRange)
	//	return false;

	// setup calculation parameters.
	const auto id = weapon->GetItemDefinitionIndex();
	const auto round_acc = [](const float accuracy) { return roundf(accuracy * 1000.f) / 1000.f; };
	const auto sniper = weapon->isSniper();
	const auto crouched = csgo->local->GetFlags() & FL_DUCKING;

	// calculate inaccuracy.
	const auto weapon_inaccuracy = weapon->GetInaccuracy();

	// calculate start and angle.
	auto start = csgo->local->GetEyePosition();
	const auto aim_angle = Math::CalculateAngle(start, position);
	Vector forward, right, up;
	Math::AngleVectors(aim_angle, forward, right, up);

	// keep track of all traces that hit the enemy.
	auto current = 0;

	// setup calculation parameters.
	Vector total_spread, spread_angle, end;
	float inaccuracy, spread_x, spread_y;
	std::tuple<float, float, float>* seed;

	for (auto i = 0u; i < total_seeds; i++)  // NOLINT(modernize-loop-convert)
	{
		// get seed.
		seed = &precomputed_seeds[i];

		// calculate spread.
		inaccuracy = std::get<0>(*seed) * weapon_inaccuracy;
		spread_x = std::get<2>(*seed) * inaccuracy;
		spread_y = std::get<1>(*seed) * inaccuracy;
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
		if ((float)current / (float)total_seeds >= chance)
			return true;

		// abort if we can no longer reach hitchance.
		if ((float)(current + total_seeds - i) / (float)(total_seeds) < chance)
			return false;
	}

	return (float)current / (float)total_seeds >= chance;
}

bool Ragebot::Hitchance(Vector Aimpoint, animation* best, int& hitbox)
{
	float chance = current_settings.hitchance;
	const bool& scoped = csgo->weapon->isSniper() && !csgo->local->IsScoped();
	if (csgo->weapon->IsZeus())
		chance = vars.ragebot.zeuschance / 100.f;
	else
		chance /= 100.f;

	if (Misc::Get().dt_bullets == 1) {
		chance = current_settings.doubletap_hc / 100.f;
		if (scoped)
			chance /= 1.5f;
	}
	else
	{
		if (scoped)
			chance /= 2.f;
	}

	//if (csgo->weapon->GetItemDefinitionIndex() == weapon_revolver)
	//	chance /= 1.5f;

	chance = clamp(chance, 0.f, 1.f);

	if (vars.ragebot.force_safepoint.active || best->safepoints) {
		return HitTraces(best, Aimpoint, chance, hitbox, best->bones)
			&& HitTraces(best, Aimpoint, chance, hitbox, best->inversed_bones)
			&& HitTraces(best, Aimpoint, chance, hitbox, best->unresolved_bones);
	}
	else {
		return HitTraces(best, Aimpoint, chance, hitbox, best->bones);
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
	const auto is_revolver = csgo->weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER;
	const auto is_zeus = csgo->weapon->GetItemDefinitionIndex() == weapon_taser;
	const auto is_knife = !is_zeus && info->m_iWeaponType == WEAPONTYPE_KNIFE;

	if (csgo->weapon->GetItemDefinitionIndex() == weapon_c4 || !csgo->weapon->IsGun() || csgo->weapon->GetAmmo(false) <= 0 && !is_knife)
	{
		return false;
	}

	if (csgo->weapon->InReload())
	{
		return false;
	}

	if (is_revolver) {
		return m_revolver_fire;
	}
	else
	{
		if (csgo->local->m_flNextAttack() > time
			|| csgo->weapon->NextPrimaryAttack() > time
			|| csgo->weapon->NextSecondaryAttack() > time)
		{
			if (csgo->weapon->GetItemDefinitionIndex() != weapon_revolver && info->m_iWeaponType == WEAPONTYPE_PISTOL)
				csgo->cmd->buttons &= ~IN_ATTACK;

			return false;
		}
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

void CockRevolver() {

	if (!vars.ragebot.enable)
		return;

	if (!csgo->local || !csgo->local->isAlive())
		return;

	if (csgo->weapon->GetItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	if (csgo->weapon->GetAmmo(false) <= 0)
		return;

	if (csgo->local->m_flNextAttack() > TICKS_TO_TIME(csgo->fixed_tickbase))
		return;

	static auto last_checked = 0;
	static auto last_spawn_time = 0.f;
	static auto tick_cocked = 0;
	static auto tick_strip = 0;
	auto time = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;


	const auto max_ticks = TIME_TO_TICKS(.25f) - 1;
	const auto tick_base = TIME_TO_TICKS(time);

	if (csgo->local->GetSpawnTime() != last_spawn_time) {
		tick_cocked = tick_base;
		tick_strip = tick_base - max_ticks - 1;
		last_spawn_time = csgo->local->GetSpawnTime();
	}

	if (csgo->weapon->NextPrimaryAttack() > time) {
		csgo->cmd->buttons &= ~IN_ATTACK;
		Ragebot::Get().m_revolver_fire = false;
		return;
	}

	if (last_checked == tick_base)
		return;

	last_checked = tick_base;
	Ragebot::Get().m_revolver_fire = false;

	if (tick_base - tick_strip > 2 && tick_base - tick_strip < 14)
		Ragebot::Get().m_revolver_fire = true;

	if (csgo->cmd->buttons & IN_ATTACK && Ragebot::Get().m_revolver_fire)
		return;

	csgo->cmd->buttons |= IN_ATTACK;

	if (csgo->weapon->NextSecondaryAttack() >= time)
		csgo->cmd->buttons |= IN_ATTACK2;

	if (tick_base - tick_cocked > max_ticks * 2 + 1) {
		tick_cocked = tick_base;
		tick_strip = tick_base - max_ticks - 1;
	}

	const auto cock_limit = tick_base - tick_cocked >= max_ticks;
	const auto after_strip = tick_base - tick_strip <= max_ticks;

	if (cock_limit || after_strip) {
		tick_cocked = tick_base;
		csgo->cmd->buttons &= ~IN_ATTACK;

		if (cock_limit)
			tick_strip = tick_base;
	}
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
	case 11: return hs::left_foot.s(); break;
	case 12: return hs::right_foot.s(); break;
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

#ifdef _BETA
	ret += hs::H_.s() + hitbox_where_shot + hs::prefix_end.s();
	if (vars.ragebot.resolver && resolver.size() > 0 && resolver != str("OFF"))
		ret += hs::R_.s() + resolver + hs::prefix_end.s();
	ret += hs::B_.s() + std::to_string(backtrack) + hs::prefix_end.s();
	if (record.didshot)
		ret += hs::SHOT.s();
	if (record.safepoints)
		ret += hs::sp.s();
	if (record.scan_mode != 128) {
		ret += str("[S:") + std::to_string(record.scan_mode);
		ret += str("]");
	}
	else
		ret += str(" err");
#endif

	return ret;
}

bool Ragebot::FastStop() {
	auto wpn_info = csgo->weapon->GetCSWpnData();
	if (!wpn_info)
		return false;

	auto get_standing_accuracy = [&]() -> const float
	{
		const auto max_speed = csgo->weapon->GetZoomLevel() > 0 ? wpn_info->m_flMaxSpeedAlt : wpn_info->m_flMaxSpeed;
		return max_speed / 3.f;
	};
	auto velocity = csgo->vecUnpredictedVel/*csgo->local->GetVelocity()*/;
	float speed = velocity.Length2D();
	if (speed <= get_standing_accuracy())
		return true;

	float max_speed = (csgo->weapon->GetZoomLevel() == 0 ? wpn_info->m_flMaxSpeed : wpn_info->m_flMaxSpeedAlt) * 0.1f;
	if (speed > max_speed) {
		csgo->should_stop_slide = false;
	}
	else {
		csgo->should_stop_slide = true;
		return true;
	}

	Vector direction;
	Math::VectorAngles(velocity, direction);
	direction.y = csgo->original.y - direction.y;
	Vector forward;
	Math::AngleVectors(direction, forward);
	Vector negated_direction = forward * -speed;
	csgo->cmd->forwardmove = negated_direction.x;
	csgo->cmd->sidemove = negated_direction.y;
	return !(current_settings.quickstop_options & 2);
}

int Ragebot::GetTicksToShoot() {
	if (Ragebot::Get().IsAbleToShoot())
		return -1;
	auto flServerTime = TICKS_TO_TIME(csgo->fixed_tickbase);
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
	Vector vel = csgo->vecUnpredictedVel;
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

	if (csgo->weapon->IsZeus() && !vars.ragebot.zeusbot)
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
	bool is_revolver = weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER;
	CockRevolver();

	Vector viewangles;
	interfaces.engine->GetViewAngles(viewangles);

	bool is_able_to_shoot = IsAbleToShoot();
	current_settings = CurrentSettings();
	dt_ready = Misc::Get().dt_bullets == 1 && csgo->weapon->isAutoSniper();

	for (auto i = 1; i <= interfaces.global_vars->maxClients; i++)
	{
		auto pEntity = interfaces.ent_list->GetClientEntity(i);
		if (!pEntity || pEntity == csgo->local)
			continue;
		if (!pEntity->isAlive() || pEntity->GetHealth() <= 0 || pEntity->GetTeam() == csgo->local->GetTeam()) {
			csgo->actual_misses[pEntity->GetIndex()] = 0;
			csgo->imaginary_misses[pEntity->GetIndex()] = 0;
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
			&& anims->player == pEntity && aim_position != Vector(0, 0, 0)
			&& Math::GetFov(viewangles, Math::CalculateAngle(csgo->eyepos, aim_position)) <= vars.ragebot.fov)
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
		bool can_shoot_if_fakeduck = true;

		if (vars.antiaim.fakeduck.active) {
			can_shoot_if_fakeduck = csgo->local->GetFlags() & FL_ONGROUND && (csgo->duck_amount > 0.9f || csgo->duck_amount < 0.1f);
		}

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

		bool force_accuracy = !current_settings.quickstop;

		if (!csgo->weapon->IsZeus() && csgo->local->GetFlags() & FL_ONGROUND && !vars.antiaim.slowwalk.active) {
			bool should_stop = GetTicksToShoot() <= GetTicksToStop()
				|| (current_settings.quickstop_options & 1) && !is_able_to_shoot;
			if (should_stop && current_settings.quickstop)
			{
				if (csgo->weapon->GetItemDefinitionIndex() == WEAPON_SSG08
					|| csgo->weapon->GetItemDefinitionIndex() == WEAPON_AWP) {
					FastStop();
					force_accuracy = true;
				}
				else {
					if (!csgo->should_stop_slide) {
						force_accuracy = FastStop();
						csgo->should_stop_slide = true;
					}
				}
			}
			else
				csgo->should_stop_slide = false;
		}

		if (!(csgo->local->GetFlags() & FL_ONGROUND))
			force_accuracy = true;

		if (vars.antiaim.slowwalk.active
			|| csgo->weapon->IsZeus()
			|| csgo->weapon->GetItemDefinitionIndex() == weapon_revolver)
			force_accuracy = true;

		if (vars.ragebot.autoshoot) {

			if (hitchance && can_shoot_if_fakeduck && is_able_to_shoot)
			{
				if (!csgo->fake_duck) {
					if (!csgo->last_sendpacket || vars.ragebot.double_tap.active)
						csgo->send_packet = true;
					csgo->max_fakelag_choke = CanDT() ? 1 : 2;
				}
				if (force_accuracy)
					csgo->cmd->buttons |= IN_ATTACK;
			}
		}
		if (hitchance && can_shoot_if_fakeduck && is_able_to_shoot)
		{
			if (csgo->cmd->buttons & IN_ATTACK) {
				csgo->last_forced_tickcount = csgo->cmd->tick_count;
				csgo->cmd->viewangles = Math::CalculateAngle(csgo->local->GetEyePosition(), current_aim_position);

				if (!vars.misc.antiuntrusted) {
					csgo->cmd->viewangles.y += 180.f;
					csgo->cmd->viewangles.x += 180.f - csgo->cmd->viewangles.x * 2.f;
				}

				csgo->cmd->viewangles -= csgo->local->GetPunchAngle() * interfaces.cvars->FindVar(str("weapon_recoil_scale"))->GetFloat();
				csgo->cmd->tick_count = TIME_TO_TICKS(best_anims->sim_time + LerpTime());
				ShotSnapshot* snapshot = new ShotSnapshot();
				snapshot->entity = best_anims->player;
				snapshot->hitbox_where_shot = HitboxToString(hitbox);
				snapshot->resolver = ResolverMode[best_anims->player->GetIndex()];
				snapshot->time = interfaces.global_vars->interval_per_tick * csgo->local->GetTickBase();
				snapshot->first_processed_time = 0.f;
				snapshot->bullet_impact = false;
				snapshot->weapon_fire = false;
				snapshot->damage = -1;
				snapshot->intended_damage = best_damage;
				snapshot->start = csgo->local->GetEyePosition();
				snapshot->hitgroup_hit = -1;
				snapshot->backtrack = TIME_TO_TICKS(fabsf(best_anims->player->GetSimulationTime() - current_aim_simulationtime));
				snapshot->eyeangles = Math::normalize(best_anims->player->GetEyeAngles());
				snapshot->hitbox = hitbox;
				snapshot->player_name = best_anims->player->GetName();
				snapshot->intended_position = current_aim_position;
				snapshot->record = *best_anims;
				shot_snapshots.emplace_back(*snapshot);
				delete snapshot;
				shot = true;
				if (vars.ragebot.shotrecord)
					DrawCapsule(best_anims);/*Chams->AddHitmatrix(best_anims);*/

				if (!vars.ragebot.silent)
					interfaces.engine->SetViewAngles(csgo->cmd->viewangles);
			}
		}
	}
	if (is_able_to_shoot && csgo->cmd->buttons & IN_ATTACK)
		shot = true;
}