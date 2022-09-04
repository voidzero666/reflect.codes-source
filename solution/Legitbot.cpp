#include "Hooks.h"
#include "Legitbot.h"
#include "Ragebot.h"
#include "Features.h"

animation* Legitbot::GetAimAnimation() {
	if (!csgo->local->isAlive() || !csgo->cmd || !csgo->weapon->IsGun())
		return nullptr;
	//Vector center_of_screen = Vector(csgo->w / 2, csgo->h / 2, 0);
	float closest_distance = FLT_MAX;
	animation* closest_info = nullptr;
	for (auto i = 0; i < interfaces.engine->GetMaxClients(); i++)
	{
		auto entity = interfaces.ent_list->GetClientEntity(i);
		if (!entity || entity == csgo->local)
			continue;	
		if (entity->IsDormant()  || entity->HasGunGameImmunity() || !entity->IsPlayer() || !entity->isEnemy(csgo->local))
			continue;

		auto ref = entity->GetRefEHandle();

		if (!ref)
			continue;

		auto animation_info = g_Animfix->animation_infos.find(ref);
		if (animation_info != g_Animfix->animation_infos.end()) {
			for (auto anims = animation_info->second->frames.begin(); anims != animation_info->second->frames.end(); anims++)
			{
				if (!anims->is_valid(vars.legitbot.btlength, 0.2f))
					continue;
				matrix mat[128];
				memcpy(mat, anims->bones, sizeof(matrix[128]));
				auto head_pos3d = Ragebot::Get().GetPoint(entity, 0, mat);
				
				auto distance = Legitbot::Get().calculateRelativeAngle(csgo->local->GetEyePosition(), head_pos3d, csgo->cmd->viewangles).Length2D();
				if (distance < closest_distance) {
					closest_distance = distance;
					closest_info = &*anims;
				}
				
			}
		}
	}
	
	return closest_info;
}

float segmentToSegment(const Vector& s1, const Vector& s2, const Vector& k1, const Vector& k2) noexcept
{
	static auto constexpr epsilon = 0.00000001f;

	auto u = s2 - s1;
	auto v = k2 - k1;
	auto w = s1 - k1;
	
	auto a = u.Dot(u); //-V525
	auto b = u.Dot(v);
	auto c = v.Dot(v);
	auto d = u.Dot(w);
	auto e = v.Dot(w);
	auto D = a * c - b * b;

	auto sn = 0.0f, sd = D;
	auto tn = 0.0f, td = D;

	if (D < epsilon)
	{
		sn = 0.0f;
		sd = 1.0f;
		tn = e;
		td = c;
	}
	else
	{
		sn = b * e - c * d;
		tn = a * e - b * d;

		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	if (tn < 0.0f)
	{
		tn = 0.0f;

		if (-d < 0.0f)
			sn = 0.0f;
		else if (-d > a)
			sn = sd;
		else
		{
			sn = -d;
			sd = a;
		}
	}
	else if (tn > td)
	{
		tn = td;

		if (-d + b < 0.0f)
			sn = 0.0f;
		else if (-d + b > a)
			sn = sd;
		else
		{
			sn = -d + b;
			sd = a;
		}
	}

	auto sc = fabs(sn) < epsilon ? 0.0f : sn / sd;
	auto tc = fabs(tn) < epsilon ? 0.0f : tn / td;

	auto dp = w + u * sc - v * tc;
	return dp.Length();
}

Vector vectorRotate(Vector& in1, Vector& in2) noexcept
{
	auto vector_rotate = [](const Vector& in1, const matrix& in2)
	{
		return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
	};
	auto angleMatrix = [](const Vector& angles, matrix& matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		sinCos(DEG2RAD(angles[1]), &sy, &cy);
		sinCos(DEG2RAD(angles[0]), &sp, &cp);
		sinCos(DEG2RAD(angles[2]), &sr, &cr);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp * crcy + srsy);
		matrix[1][2] = (sp * crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	};
	matrix m;
	angleMatrix(in2, m);
	return vector_rotate(in1, m);
}

void vectorITransform(const Vector& in1, const matrix& in2, Vector& out) noexcept
{
	out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
	out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
	out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
}


void vectorIRotate(Vector in1, matrix in2, Vector& out) noexcept
{
	out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
	out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
	out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
}

bool intersectLineWithBb(Vector& start, Vector& end, Vector& min, Vector& max) noexcept
{
	float d1, d2, f;
	auto start_solid = true;
	auto t1 = -1.0f, t2 = 1.0f;

	const float s[3] = { start.x, start.y, start.z };
	const float e[3] = { end.x, end.y, end.z };
	const float mi[3] = { min.x, min.y, min.z };
	const float ma[3] = { max.x, max.y, max.z };

	for (auto i = 0; i < 6; i++) {
		if (i >= 3) {
			const auto j = i - 3;

			d1 = s[j] - ma[j];
			d2 = d1 + e[j];
		}
		else {
			d1 = -s[i] + mi[i];
			d2 = d1 - e[i];
		}

		if (d1 > 0.0f && d2 > 0.0f)
			return false;

		if (d1 <= 0.0f && d2 <= 0.0f)
			continue;

		if (d1 > 0)
			start_solid = false;

		if (d1 > d2) {
			f = d1;
			if (f < 0.0f)
				f = 0.0f;

			f /= d1 - d2;
			if (f > t1)
				t1 = f;
		}
		else {
			f = d1 / (d1 - d2);
			if (f < t2)
				t2 = f;
		}
	}

	return start_solid || (t1 < t2&& t1 >= 0.0f);
}

bool hitboxIntersection(IBasePlayer* entity, const matrix inmatrix[128], int iHitbox, mstudiohitboxset_t* set, const Vector& start, const Vector& end) noexcept
{
	auto VectorTransform_Wrapper = [](const Vector& in1, const matrix in2, Vector& out)
	{
		auto VectorTransform = [](const float* in1, const matrix in2, float* out)
		{
			auto DotProducts = [](const float* v1, const float* v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};

	mstudiobbox_t* hitbox = set->pHitbox(iHitbox);
	if (!hitbox)
		return false;

	if (hitbox->radius == -1.f)
		return false;
	Vector mins, maxs;
	const auto isCapsule = hitbox->radius != -1.f;
	if (isCapsule)
	{
		VectorTransform_Wrapper(hitbox->bbmin, inmatrix[hitbox->bone], mins);
		VectorTransform_Wrapper(hitbox->bbmax, inmatrix[hitbox->bone], maxs);
		const auto dist = segmentToSegment(start, end, mins, maxs);

		if (dist < hitbox->radius)
			return true;
	}
	else
	{
		VectorTransform_Wrapper(vectorRotate(hitbox->bbmin, hitbox->rotation), inmatrix[hitbox->bone], mins);
		VectorTransform_Wrapper(vectorRotate(hitbox->bbmax, hitbox->rotation), inmatrix[hitbox->bone], maxs);

		vectorITransform(start, inmatrix[hitbox->bone], mins);
		vectorIRotate(end, inmatrix[hitbox->bone], maxs);

		if (intersectLineWithBb(mins, maxs, hitbox->bbmin, hitbox->bbmax))
			return true;
	}
	return false;
}



void Legitbot::Backtrack()
{
	if (auto closest_animation = GetAimAnimation(); closest_animation != nullptr)
		csgo->cmd->tick_count = TIME_TO_TICKS(closest_animation->sim_time + Ragebot::Get().LerpTime());
}

void Legitbot::AutoPistol() {
	if (!csgo->local || !csgo->cmd || !csgo->weapon)
		return;

	if (csgo->local->GetFlags() & FL_FROZEN
		|| csgo->local->HasGunGameImmunity()
		|| csgo->game_rules->IsFreezeTime())
		return;

	if (csgo->weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER
		|| csgo->weapon->GetItemDefinitionIndex() == WEAPON_HEALTHSHOT
		|| csgo->weapon->IsNade()
		|| csgo->weapon->IsBomb()
		|| csgo->weapon->isAutoSniper())
		return;

	if (!Ragebot::Get().IsAbleToShoot()) {
		if (csgo->cmd->buttons & IN_ATTACK)
			csgo->cmd->buttons &= ~IN_ATTACK;
		else if (csgo->weapon->IsKnife() && (csgo->cmd->buttons & IN_ATTACK || csgo->cmd->buttons & IN_ATTACK2))
		{
			if (csgo->cmd->buttons & IN_ATTACK)
				csgo->cmd->buttons &= ~IN_ATTACK;

			if (csgo->cmd->buttons & IN_ATTACK2)
				csgo->cmd->buttons &= ~IN_ATTACK2;
		}
	}
}

Vector Legitbot::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles)
{
	return ((destination - source).toAngle() - viewAngles).Normalize();
}


Vector find_target(Vector2D recoilangle, CLegitbotWeapon curweapon, std::vector<int>& hitboxes_to_check, int* besthitbox, CUserCmd* cmd)
{

	IBasePlayer* target = nullptr;
	int bestbox = -1;
	float fov = curweapon.fov;
	Vector angle = cmd->viewangles;
	Vector bestposition = Vector{ 360.f,360.f, 360.f };
	angle.x += recoilangle.x;
	angle.y += recoilangle.y;



	if (curweapon.aimbacktrack && vars.legitbot.backtrack)
	{
		auto anim = Legitbot::Get().GetAimAnimation();



		if (anim && anim->is_valid(vars.legitbot.btlength, 0.2f))
		{
			matrix mat[128];
			memcpy(mat, anim->bones, sizeof(matrix[128]));
			for (auto curhitbox : hitboxes_to_check)
			{
				Vector pos = Ragebot::Get().GetPoint(anim->player, curhitbox, mat);
				float w = Legitbot::Get().calculateRelativeAngle(csgo->local->GetEyePosition(), pos, angle).Length2D();
				if (fov > w)
				{
					CTraceFilter filter;
					filter.pSkip = csgo->local;
					CGameTrace tr;
					interfaces.trace->TraceRay(Ray_t(csgo->local->GetEyePosition(), pos), MASK_PLAYERSOLID, &filter, &tr);
					if (tr.IsVisible()) {
						bestbox = curhitbox;
						fov = w;
						target = anim->player;
						bestposition = pos;
					}
				}
			}
		}
	}
	else
	{
		for (int i = 1; i <= interfaces.global_vars->maxClients; i++)
		{
			auto ent = interfaces.ent_list->GetClientEntity(i);

			if (!ent)
				continue;

			if (ent->HasGunGameImmunity())
				continue;

			if (ent == csgo->local)
				continue;

			if (ent->GetClientClass()->m_ClassID != 40)
				continue;

			if (ent->IsDormant())
				continue;

			if (!vars.legitbot.teammates && !ent->isEnemy(csgo->local))
				continue;

			if (!ent->isAlive())
				continue;

			for (auto curhitbox : hitboxes_to_check) // find closest hitbox
			{
				Vector pos = Ragebot::Get().GetVisualHitbox(ent, curhitbox);
				Vector calcang = Legitbot::Get().calculateRelativeAngle(csgo->local->GetEyePosition(), pos, angle);

				if (fov > calcang.Length2D()) {
					CTraceFilter filter;
					filter.pSkip = csgo->local;
					CGameTrace tr;
					interfaces.trace->TraceRay(Ray_t(csgo->local->GetEyePosition(), pos), MASK_PLAYERSOLID, &filter, &tr);
					if (tr.IsVisible()) {
						bestbox = curhitbox;
						fov = calcang.Length2D();
						target = ent;
						bestposition = pos;
					}
				}
			}
		}
	}

	*besthitbox = bestbox;
	if (target)
		return bestposition;
	else
		return Vector{ 360.f,360.f,360.f };
}

void smooth_angle(Vector& angle, float ptr, const Vector& viewangles)
{
	Vector delta = angle - viewangles;
	delta.Clamp();

	float smooth = powf(ptr / 50.f, 0.4f);

	smooth = ((0.99f) < (smooth) ? (0.99f) : (smooth));
	Vector to_change;

	// slow end
	// to_change = delta - delta * smooth;
	float coeff = (1.0f - smooth) / delta.Length() * 4.f;

	// fast end
	// coeff = powf( coeff, 2.f ) * 10.f;
	coeff = ((1.f) < (coeff) ? (1.f) : (coeff));

	to_change = delta * coeff;

	angle = viewangles + to_change;
}

void Legitbot::Run(CUserCmd* cmd)
{
	if (!csgo->local || !cmd || !csgo->weapon || !vars.legitbot.aimbot)
		return;

	if (!csgo->local->isAlive() || csgo->local->IsDefusing() || csgo->local->WaitForNoAttack() || csgo->local->GetWeapon()->GetAmmo(false) == 0 || csgo->weapon->IsKnife() || csgo->weapon->IsNade() || csgo->weapon->IsBomb() || csgo->weapon->IsMiscWeapon())
		return;

	static int besttarget = -1;
	Vector bestdist = {360,360,0};
	Vector bestpos = { 0,0,0 };

	CLegitbotWeapon curweapon;

	curweapon = vars.legitbot.weapons[0];

	if (csgo->weapon->isPistol() && vars.legitbot.weapons[1].enabled)
	{
		curweapon = vars.legitbot.weapons[1];
	}
	else if (csgo->weapon->isRifle() && vars.legitbot.weapons[2].enabled)
	{
		curweapon = vars.legitbot.weapons[2];
	}
	else if ((csgo->weapon->GetItemDefinitionIndex() == WEAPON_SSG08) && vars.legitbot.weapons[3].enabled)
	{
		curweapon = vars.legitbot.weapons[3];
	}
	else if (csgo->weapon->IsAWP() && vars.legitbot.weapons[4].enabled)
	{
		curweapon = vars.legitbot.weapons[4];
	}
	else if (csgo->weapon->IsSMG() && vars.legitbot.weapons[5].enabled)
	{
		curweapon = vars.legitbot.weapons[5];
	}

	if (!curweapon.enabled)
		return;
	
	//if (!curweapon.aimlock && (cmd->buttons & IN_ATTACK) && !curweapon.silent)
		//return;
	
	if (curweapon.scopedonly && !csgo->local->IsScoped() && csgo->weapon->isSniper())
		return;

	if (curweapon.ignoreflash && csgo->local->IsFlashed())
		return;

	std::vector<int> hitboxes_to_check;
	hitboxes_to_check.clear();

	if (curweapon.hitboxes[0])
	{
		hitboxes_to_check.push_back(HITBOX_HEAD);
	}
	if (curweapon.hitboxes[1])
	{
		hitboxes_to_check.push_back(4); // thorax
		hitboxes_to_check.push_back(5); // lower chest
		hitboxes_to_check.push_back(6); // upper chest
	}
	if (curweapon.hitboxes[2])
	{
		hitboxes_to_check.push_back(2); // pelvis
		hitboxes_to_check.push_back(3); // belly
	}
	if (curweapon.hitboxes[3])
	{
		hitboxes_to_check.push_back(15); // right upper arm
		hitboxes_to_check.push_back(16); // right forearm
		hitboxes_to_check.push_back(17); // left upper arm
		hitboxes_to_check.push_back(18); // left forearm
	}
	if (curweapon.hitboxes[4])
	{
		hitboxes_to_check.push_back(9); // right calf
		hitboxes_to_check.push_back(7); // right thigh
		hitboxes_to_check.push_back(10); // left calf
		hitboxes_to_check.push_back(8); // left thigh
	}

	if (hitboxes_to_check.empty())
		return;

	Vector2D recoil_angle{};

	static Vector lastAngles{ cmd->viewangles };
	static int lastCommand{};

	if (curweapon.rcsx > 0.f || curweapon.rcsy > 0.f) {
		const auto recoil_scale = interfaces.cvars->FindVar(crypt_str("weapon_recoil_scale"))->GetFloat();
		//const auto recoil_scale = interfaces::console->get_convar("weapon_recoil_scale")->get_float();
		const auto aim_punch = csgo->local->GetPunchAngle();

		recoil_angle =
		{
			aim_punch.x * (recoil_scale * curweapon.rcsx),
			aim_punch.y * (recoil_scale * curweapon.rcsy)
		};
	}


	int besthitbox = -1;
	Vector hitbox = Vector{ 360.f, 360.f, 360.f };
	
	hitbox = find_target(recoil_angle, curweapon, hitboxes_to_check, &besthitbox, cmd);

	if (hitbox == Vector{ 360.f,360.f,360.f } || besthitbox == -1)
		return;

	if (csgo->lineGoesThroughSmoke(csgo->local->GetEyePosition(), hitbox, 1) && curweapon.ignoresmoke)
		return;
	
	if (lastCommand == cmd->command_number - 1 && !lastAngles.IsZero() && curweapon.silent)
		cmd->viewangles = lastAngles;

	Vector m_angPoint = cmd->viewangles + Legitbot::Get().calculateRelativeAngle(csgo->local->GetEyePosition(), hitbox, cmd->viewangles);

	// call before smoothing so the rcs gets smoothed later!!

	m_angPoint.x -= recoil_angle.x;
	m_angPoint.y -= recoil_angle.y;

	if (curweapon.smooth > 1.f) {
		
		smooth_angle(m_angPoint, curweapon.smooth, cmd->viewangles);
	}

	


	m_angPoint.Normalize();
	m_angPoint.Clamp();

	auto is_aimbotting = [cmd]() -> bool {

		if (vars.legitbot.onkey) {	
			return vars.legitbot.key.state;
		}
		else
		{
			return true;
		}
		
	};

	

	if (is_aimbotting()) {
		
		cmd->viewangles = m_angPoint;
		if(!curweapon.silent)
			interfaces.engine->SetViewAngles(cmd->viewangles);
		if (curweapon.smooth > 1.f)
			lastAngles = cmd->viewangles;
		else
			lastAngles = Vector{ 0,0,0 };
		lastCommand = cmd->command_number;

	}
	else {
		besttarget = -1;
	}
}




/*
void Legitbot::Triggerbot(CUserCmd* cmd)
{
	if (!vars.legitbot.triggerbot.enabled)
	{
		return;
	}
		
	if (!csgo->local->isAlive() || csgo->local->IsDefusing() || csgo->local->WaitForNoAttack() || csgo->local->GetWeapon()->GetAmmo(false) == 0 || csgo->weapon->IsKnife() || csgo->weapon->IsNade() || csgo->weapon->IsBomb() || csgo->weapon->IsMiscWeapon())
	{
		return;
	}

	if (vars.legitbot.triggerbot.onkey && !vars.legitbot.triggerbot.trigkey.state)
	{
		return;
	}


	std::vector<int> hitboxes_to_check;

	if (vars.legitbot.triggerbot.hitboxes[0])
	{
		hitboxes_to_check.push_back(HITBOX_HEAD);
	}
	if (vars.legitbot.triggerbot.hitboxes[1])
	{
		hitboxes_to_check.push_back(4); // thorax
		hitboxes_to_check.push_back(5); // lower chest
		hitboxes_to_check.push_back(6); // upper chest
	}
	if (vars.legitbot.triggerbot.hitboxes[2])
	{
		hitboxes_to_check.push_back(2); // pelvis
		hitboxes_to_check.push_back(3); // belly
	}
	if (vars.legitbot.triggerbot.hitboxes[3])
	{
		hitboxes_to_check.push_back(15); // right upper arm
		hitboxes_to_check.push_back(16); // right forearm
		hitboxes_to_check.push_back(17); // left upper arm
		hitboxes_to_check.push_back(18); // left forearm
	}
	if (vars.legitbot.triggerbot.hitboxes[4])
	{
		hitboxes_to_check.push_back(9); // right calf
		hitboxes_to_check.push_back(7); // right thigh
		hitboxes_to_check.push_back(10); // left calf
		hitboxes_to_check.push_back(8); // left thigh
	}

	if (hitboxes_to_check.empty())
		return;
	
	Vector forward;
	Math::AngleVectors(cmd->viewangles, forward);
	Vector point = csgo->local->GetEyePosition() + (forward * 56754);

	auto info = g_AutoWall.Think(point, );
	if (info.m_hit_entity && !info.m_did_penetrate_wall && info.m_damage >= vars.legitbot.triggerbot.mindmg)
	{
		for (auto hitbox : hitboxes_to_check)
		{
			if (Ragebot::Get().HitboxToHitgroup((CSGOHitboxID)hitbox) == info.m_hitgroup)
			{
				cmd->buttons |= IN_ATTACK;
				break;
			}	
		}

	}
	g_AutoWall.reset();
		
	
}

*/

auto getDamageMultiplier = [](int hitgroup, CCSWeaponInfo* data)
{

	switch (hitgroup) {
	case 1:
		return data->flHeadshotMultiplier;
	case 3:
		return 1.25f;
	case 6:
	case 7:
		return 0.75f;
	default:
		return 1.0f;
	}

};

auto isArmored = [](int hitGroup, bool helmet)
{
	switch (hitGroup) {
	case 1:
		return helmet;

	case 2:
	case 3:
	case 4:
	case 5:
		return true;
	default:
		return false;
	}
};

static bool keyPressed;

void Legitbot::Triggerbot(CUserCmd* cmd)
{

	if (!vars.legitbot.triggerbot.enabled)
		return;

	if (!csgo->local || !csgo->local->isAlive() || csgo->local->m_flNextAttack() > csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick || csgo->local->IsDefusing() || csgo->local->WaitForNoAttack())
		return;

	const auto activeWeapon = csgo->local->GetWeapon();
	if (!activeWeapon || !activeWeapon->GetAmmo(false) || activeWeapon->NextPrimaryAttack() > csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick || csgo->weapon->IsKnife() || csgo->weapon->IsNade() || csgo->weapon->IsBomb() || csgo->weapon->IsMiscWeapon())
		return;

	if (csgo->local->GetShootsFired() > 0 && !activeWeapon->GetCSWpnData()->m_bFullAuto)
		return;

	static auto lastTime = 0.0f;
	static auto lastContact = 0.0f;

	const auto now = interfaces.global_vars->realtime;

	

	if (vars.legitbot.triggerbot.onkey)
	{
		if (!vars.legitbot.triggerbot.trigkey.properstate())
			return;
	}
		

	if (now - lastTime < vars.legitbot.triggerbot.delay)
		return;


	std::vector<int> hitboxes_to_check;

	if (vars.legitbot.triggerbot.hitboxes[0])
	{
		hitboxes_to_check.push_back(HITBOX_HEAD);
	}
	if (vars.legitbot.triggerbot.hitboxes[1])
	{
		hitboxes_to_check.push_back(4); // thorax
		hitboxes_to_check.push_back(5); // lower chest
		hitboxes_to_check.push_back(6); // upper chest
	}
	if (vars.legitbot.triggerbot.hitboxes[2])
	{
		hitboxes_to_check.push_back(2); // pelvis
		hitboxes_to_check.push_back(3); // belly
	}
	if (vars.legitbot.triggerbot.hitboxes[3])
	{
		hitboxes_to_check.push_back(15); // right upper arm
		hitboxes_to_check.push_back(16); // right forearm
		hitboxes_to_check.push_back(17); // left upper arm
		hitboxes_to_check.push_back(18); // left forearm
	}
	if (vars.legitbot.triggerbot.hitboxes[4])
	{
		hitboxes_to_check.push_back(9); // right calf
		hitboxes_to_check.push_back(7); // right thigh
		hitboxes_to_check.push_back(10); // left calf
		hitboxes_to_check.push_back(8); // left thigh
	}

	if (hitboxes_to_check.empty())
		return;


	
	if (vars.legitbot.triggerbot.checkflash && csgo->local->IsFlashed())
		return;

	if (vars.legitbot.triggerbot.scopedonly && csgo->weapon->isSniper() && !csgo->local->IsScoped())
		return;
		

	const auto weaponData = activeWeapon->GetCSWpnData();
	if (!weaponData)
		return;

	const auto startPos = csgo->eyepos;

	auto Sexfunc = [](Vector angle) {
		return Vector{ std::cos(DEG2RAD(angle.x)) * std::cos(DEG2RAD(angle.y)),
					   std::cos(DEG2RAD(angle.x)) * std::sin(DEG2RAD(angle.y)),
					  -std::sin(DEG2RAD(angle.x)) };
	};

	const auto recoil_scale = interfaces.cvars->FindVar(crypt_str("weapon_recoil_scale"))->GetFloat();
	const auto aim_punch = csgo->local->GetPunchAngle();

	
	Vector recoil_angle =
	{
		aim_punch.x * recoil_scale,
		aim_punch.y * recoil_scale,
		0.f
	};


	const auto endPos = startPos + Sexfunc(cmd->viewangles + recoil_angle) * weaponData->m_flRange;

	
	if (vars.legitbot.triggerbot.checksmoke && csgo->lineGoesThroughSmoke(startPos, endPos, 1))
		return;

	lastTime = now;

	for (int i = 1; i <= interfaces.engine->GetMaxClients(); i++)
	{
		auto entity = interfaces.ent_list->GetClientEntity(i);
		if (!entity || entity == csgo->local || entity->IsDormant() || !entity->isAlive()
			|| !entity->isEnemy(csgo->local) || entity->HasGunGameImmunity())
			continue;

		auto player = g_Animfix->animation_infos.find(entity->GetRefEHandle())->second;
		if (!player)
			continue;

		auto& newestrecord = player->frames.at(0);

		const model_t* model = entity->GetModel();
		if (!model)
			continue;
		studiohdr_t* hdr = interfaces.models.model_info->GetStudioModel(model);
		if (!hdr)
			continue;
		mstudiohitboxset_t* set = hdr->pHitboxSet(0);
		if (!set)
			continue;

		for (int j = 0; j < hitboxes_to_check.size(); j++)
		{			
			if (hitboxIntersection(entity, newestrecord.bones, hitboxes_to_check.at(j), set, startPos, endPos))
			{
				trace_t trace;
				CTraceFilter fil;
				fil.pSkip = csgo->local;
				interfaces.trace->TraceRay({ startPos, endPos }, 0x46004009, &fil, &trace);
				if (trace.m_pEnt != entity)
				{
					break;
				}

				float damage = (activeWeapon->GetItemDefinitionIndex() != WEAPON_TASER ? getDamageMultiplier(trace.hitgroup, weaponData) : 1.0f) * weaponData->m_iDamage * std::pow(weaponData->m_flRangeModifier, trace.fraction * weaponData->m_flRange / 500.0f);
				
				if (float armorRatio{ weaponData->m_flArmorRatio / 2.0f }; activeWeapon->GetItemDefinitionIndex() != WEAPON_TASER && isArmored(trace.hitgroup, trace.m_pEnt->HasHelmet()))
					damage -= (trace.m_pEnt->GetArmor() < damage * armorRatio / 2.0f ? trace.m_pEnt->GetArmor() * 4.0f : damage) * (1.0f - armorRatio);

				if (damage >= (vars.legitbot.triggerbot.mindmg))
				{
					cmd->buttons |= IN_ATTACK;
					cmd->tick_count = TIME_TO_TICKS(newestrecord.sim_time + Ragebot::Get().LerpTime());
					lastTime = 0.0f;
					lastContact = now;
				}
				
				break;
			}
		}


		if (!vars.legitbot.triggerbot.shootatbacktrack || !vars.legitbot.backtrack)
			continue;

		auto& records = g_Animfix->animation_infos.find(entity->GetRefEHandle())->second->frames;
		
		if (records.empty())
			continue;

		float closest_distance = FLT_MAX;
		animation* closest_info = nullptr;

		for (int k = 0; k < records.size(); k++)
		{
			if (k <= 3)
				continue;
			auto& anims = records.at(k);

			if (!anims.is_valid(vars.legitbot.btlength, 0.2f))
				continue;
			matrix mat[128];
			memcpy(mat, anims.bones, sizeof(matrix[128]));
			auto head_pos3d = Ragebot::Get().GetPoint(anims.player, 0, mat);

			auto distance = Legitbot::Get().calculateRelativeAngle(csgo->local->GetEyePosition(), head_pos3d, csgo->cmd->viewangles).Length2D();
			if (distance < closest_distance) {
				closest_distance = distance;
				closest_info = &anims;
			}

		}

		if (closest_distance == FLT_MAX || closest_info == nullptr)
			continue;

		const auto record = *closest_info;

		model = entity->GetModel();
		if (!model)
		{
			continue;
		}

		hdr = interfaces.models.model_info->GetStudioModel(model);
		if (!hdr)
		{
			continue;
		}

		set = hdr->pHitboxSet(0);
		if (!set)
		{
			continue;
		}

		for (int j = 0; j < hitboxes_to_check.size(); j++)
		{

			if (hitboxIntersection(entity, record.bones, hitboxes_to_check.at(j), set, startPos, endPos))
			{
				trace_t trace;
				CTraceFilter fil;
				fil.pSkip = csgo->local;
				interfaces.trace->TraceRay({ startPos, endPos }, 0x46004009, &fil, &trace);
				if (trace.m_pEnt != entity)
				{
					break;
				}

				float damage = (activeWeapon->GetItemDefinitionIndex() != WEAPON_TASER ? getDamageMultiplier(trace.hitgroup, weaponData) : 1.0f) * weaponData->m_iDamage * std::pow(weaponData->m_flRangeModifier, trace.fraction * weaponData->m_flRange / 500.0f);

				if (float armorRatio{ weaponData->m_flArmorRatio / 2.0f }; activeWeapon->GetItemDefinitionIndex() != WEAPON_TASER && isArmored(trace.hitgroup, trace.m_pEnt->HasHelmet()))
					damage -= (trace.m_pEnt->GetArmor() < damage * armorRatio / 2.0f ? trace.m_pEnt->GetArmor() * 4.0f : damage) * (1.0f - armorRatio);

				if (damage >= (vars.legitbot.triggerbot.mindmg))
				{
					cmd->buttons |= IN_ATTACK;
					cmd->tick_count = TIME_TO_TICKS(newestrecord.sim_time + Ragebot::Get().LerpTime());
					lastTime = 0.0f;
					lastContact = now;
				}
				break;
			}
		}
	}
}

void updateInput() 
{
	keyPressed = !(vars.legitbot.triggerbot.trigkey.key != 0) || vars.legitbot.triggerbot.trigkey.state;
}



















	/*
	for (int i = 0; i < interfaces.engine->GetMaxClients(); i++)
	{
		auto curent = interfaces.ent_list->GetClientEntity(i);

		if (!curent)
			continue;

		if (curent == csgo->local || !curent->isAlive() || curent->IsDormant() || curent->HasGunGameImmunity())
			continue;

		if (!vars.legitbot.teammates)
		{
			if (curent->GetTeam() == csgo->local->GetTeam())
				continue;
		}
		
		Vector closest = {0,0,0};
		float bestfov = curweapon.fov;


		for (auto curhitbox : hitboxes_to_check) // find closest hitbox
		{
			
			Vector pos = Ragebot::Get().GetVisualHitbox(curent, curhitbox);
			// visible Check:)
			CTraceFilter filter;
			filter.pSkip = csgo->local;
			CGameTrace tr;
			interfaces.trace->TraceRay(Ray_t(csgo->local->GetEyePosition(), pos), MASK_PLAYERSOLID, &filter, &tr);
			if (tr.m_pEnt != curent)
				continue; // if not visible continue to Next Hitbox

			float curfov = calculateRelativeAngle(csgo->local->GetEyePosition(), pos, cmd->viewangles).Length2D();
			if (curfov < bestfov)
			{
				bestfov = curfov;
				closest = pos;
			}
		}

		//here do backtrack target selection shit with hitboxes

		
		

		if (closest == Vector{ 0,0,0 }) // if none of hitboxes visible On This Entity, continue to Next Entity
			continue;

		if (csgo->lineGoesThroughSmoke(csgo->local->GetEyePosition(), closest, 1) && curweapon.ignoresmoke)
			return;
		
		auto curdist = calculateRelativeAngle(csgo->local->GetEyePosition(), closest, cmd->viewangles);
		if (curdist.Length2D() > curweapon.fov)
			continue;
			
		if (curdist.Length2D() < bestdist.Length2D())
		{
			bestpos = closest;
			bestdist = curdist;
			besttarget = i;
		}
	}

	if (besttarget == -1 || bestdist == Vector{360, 360,0})
		return;
	*/


	/*
	static Vector lastAngles{ cmd->viewangles };
	static int lastCommand{};

	if ((lastCommand == cmd->command_number - 1) && lastAngles == Vector{ 0,0,0 } && curweapon.silent)
	{
		cmd->viewangles = lastAngles;
	}

	Vector aimang = calculateRelativeAngle(csgo->local->GetEyePosition(), bestpos, cmd->viewangles);

	aimang /= curweapon.smooth;

	static auto accscale = interfaces.cvars->FindVar("weapon_recoil_scale");
	auto punchang = csgo->local->GetPunchAngle() * accscale->GetFloat(); // get rcs ang

	aimang -= punchang * (curweapon.rcs * 2); // apply rcs

	Vector toset = cmd->viewangles + aimang;

	if (!curweapon.silent)
		interfaces.engine->SetViewAngles(toset);

	cmd->viewangles = toset; // apply angle

	if (curweapon.smooth > 1.f)
	{
		lastAngles = cmd->viewangles;
	}
	else
	{
		lastAngles = Vector{ 0,0,0 };
	}

	lastCommand = cmd->command_number;
	*/
