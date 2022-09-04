#include "Autowall.hpp"
#define SLOBYTE(x)   (*((int8_t*)&(x)))
/*
void C_AutoWall::CacheWeaponData()
{
	m_PenetrationData.m_Weapon = csgo->local->GetWeapon();
	if (!m_PenetrationData.m_Weapon)
		return;

	m_PenetrationData.m_WeaponData = m_PenetrationData.m_Weapon->GetCSWpnData();
	if (!m_PenetrationData.m_WeaponData)
		return;

	m_PenetrationData.m_flMaxRange = m_PenetrationData.m_WeaponData->m_flRange;
	m_PenetrationData.m_flWeaponDamage = m_PenetrationData.m_WeaponData->m_iDamage;
	m_PenetrationData.m_flPenetrationPower = m_PenetrationData.m_WeaponData->m_flPenetration;
	m_PenetrationData.m_flPenetrationDistance = 3000.0f;

	m_PenetrationData.m_DamageReduction = interfaces.cvars->FindVar(crypt_str("ff_damage_reduction_bullets"))->GetFloat();
}
*/

void C_AutoWall::TraceLine(Vector& absStart, const Vector& absEnd, unsigned int mask, IBasePlayer* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	interfaces.trace->TraceRay(ray, mask, &filter, ptr);
}

float C_AutoWall::ScaleDamage(IBasePlayer* player, float damage, float armor_ratio, int hitgroup) {
	if (!player || !player->GetClientClass() || !csgo->local->GetWeapon() || player->GetClientClass()->m_ClassID != ClassId->CCSPlayer)
		return 0.f;

	auto new_damage = damage;

	const auto is_zeus = csgo->local->GetWeapon()->GetItemDefinitionIndex() == WEAPON_TASER;

	static auto is_armored = [](IBasePlayer* player, int armor, int hitgroup) {
		if (player && player->GetArmor() > 0)
		{
			if (player->HasHelmet() && hitgroup == HITGROUP_HEAD || (hitgroup >= HITGROUP_CHEST && hitgroup <= HITGROUP_RIGHTARM || hitgroup == 8 || hitgroup == 0))
				return true;
		}
		return false;
	};

	if (!is_zeus) {
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			new_damage *= csgo->local->GetWeapon()->GetCSWpnData()->flHeadshotMultiplier;
			break;
		case HITGROUP_STOMACH:
			new_damage *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			new_damage *= .75f;
			break;
		default:
			break;
			/*4.0; 1
			1.0; 2
			1.25; 3
			1.0; 4
			1.0; 5
			0.75; 6
			0.75; 7
			1.0; 8*/
		}
	}
	else
		new_damage *= 0.92f;

	if (is_armored(player, player->GetArmor(), hitgroup))
	{
		float flHeavyRatio = 1.0f;
		float flBonusRatio = 0.5f;
		float flRatio = armor_ratio * 0.5f;
		float flNewDamage;

		if (!player->HeavyArmor())
		{
			flNewDamage = damage * flRatio;
		}
		else
		{
			flBonusRatio = 0.33f;
			flRatio = armor_ratio * 0.25f;
			flHeavyRatio = 0.33f;
			flNewDamage = (damage * flRatio) * 0.85f;
		}

		int iArmor = player->GetArmor();

		if (((damage - flNewDamage) * (flHeavyRatio * flBonusRatio)) > iArmor)
			flNewDamage = damage - (iArmor / flBonusRatio);

		new_damage = flNewDamage;
	}

	return floorf(new_damage);
}

void C_AutoWall::ScaleDamage(CGameTrace& enterTrace, CCSWeaponInfo* weaponData, float& currentDamage)
{
	if (!enterTrace.m_pEnt || !enterTrace.m_pEnt->GetClientClass() || !csgo->local->GetWeapon() || enterTrace.m_pEnt->GetClientClass()->m_ClassID != ClassId->CCSPlayer)
		return;

	IBasePlayer* target = (IBasePlayer*)enterTrace.m_pEnt;

	auto new_damage = currentDamage;

	const int hitgroup = enterTrace.hitgroup;
	const auto is_zeus = csgo->local->GetWeapon()->GetItemDefinitionIndex() == WEAPON_TASER;

	static auto is_armored = [](IBasePlayer* player, int armor, int hitgroup) {
		if (player && player->GetArmor() > 0)
		{
			if (player->HasHelmet() && hitgroup == HITGROUP_HEAD || (hitgroup >= HITGROUP_CHEST && hitgroup <= HITGROUP_RIGHTARM))
				return true;
		}
		return false;
	};

	if (!is_zeus) {
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			new_damage *= csgo->local->GetWeapon()->GetCSWpnData()->flHeadshotMultiplier;
			break;
		case HITGROUP_STOMACH:
			new_damage *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			new_damage *= .75f;
			break;
		default:
			break;
			/*4.0; 1
			1.0; 2
			1.25; 3
			1.0; 4
			1.0; 5
			0.75; 6
			0.75; 7
			1.0; 8*/
		}
	}
	else
		new_damage *= 0.92f;

	if (is_armored(target, target->GetArmor(), hitgroup))
	{
		float flHeavyRatio = 1.0f;
		float flBonusRatio = 0.5f;
		float flRatio = weaponData->m_flArmorRatio * 0.5f;
		float flNewDamage;

		if (!target->HeavyArmor())
		{
			flNewDamage = new_damage * flRatio;
		}
		else
		{
			flBonusRatio = 0.33f;
			flRatio = weaponData->m_flArmorRatio * 0.5f;
			flHeavyRatio = 0.33f;
			flNewDamage = (new_damage * (flRatio * 0.5)) * 0.85f;
		}

		int iArmor = target->GetArmor();

		if (((new_damage - flNewDamage) * (flBonusRatio * flHeavyRatio)) > iArmor)
			flNewDamage = new_damage - (iArmor / flBonusRatio);

		new_damage = flNewDamage;
	}

	currentDamage = new_damage;
}

/*
bool C_AutoWall::HandleBulletPenetration()
{
	bool bSolidSurf = ((m_PenetrationData.m_EnterTrace.contents >> 3) & CONTENTS_SOLID);
	bool bLightSurf = (m_PenetrationData.m_EnterTrace.surface.flags >> 7) & SURF_LIGHT;

	int nEnterMaterial = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_EnterTrace.surface.surfaceProps)->game.material;
	if (!m_PenetrationData.m_PenetrationCount && !bLightSurf && !bSolidSurf && nEnterMaterial != 9)
	{
		if (nEnterMaterial != 71)
			return false;
	}

	Vector vecEnd;
	if (m_PenetrationData.m_PenetrationCount <= 0 || m_PenetrationData.m_flPenetrationPower <= 0.0f)
		return false;

	if (!TraceToExit(m_PenetrationData.m_EnterTrace.endpos, vecEnd, &m_PenetrationData.m_EnterTrace, &m_PenetrationData.m_ExitTrace))
		if (!(interfaces.trace->GetPointContents(vecEnd, 0x600400B) & 0x600400B))
			return false;

	float flDamLostPercent = 0.16;
	float flDamageModifier = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_EnterTrace.surface.surfaceProps)->game.flDamageModifier;
	float flPenetrationModifier = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_EnterTrace.surface.surfaceProps)->game.flPenetrationModifier;

	int iEnterMaterial = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_EnterTrace.surface.surfaceProps)->game.material;
	if (bSolidSurf || bLightSurf || iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
	{
		if (iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
		{
			flPenetrationModifier = 3.0f;
			flDamLostPercent = 0.05;
		}
		else
			flPenetrationModifier = 1.0f;

		flDamageModifier = 0.99f;
	}
	else if (iEnterMaterial == CHAR_TEX_FLESH && m_PenetrationData.m_DamageReduction == 0
		&& m_PenetrationData.m_EnterTrace.m_pEnt && ((IBasePlayer*)(m_PenetrationData.m_EnterTrace.m_pEnt))->IsPlayer() && !((IBasePlayer*)(m_PenetrationData.m_EnterTrace.m_pEnt))->isEnemy(csgo->local))
	{
		if (m_PenetrationData.m_DamageReduction == 0)
			return true;

		flPenetrationModifier = m_PenetrationData.m_DamageReduction;
		flDamageModifier = m_PenetrationData.m_DamageReduction;
	}
	else
	{
		float flExitPenetrationModifier = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_ExitTrace.surface.surfaceProps)->game.flPenetrationModifier;
		float flExitDamageModifier = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_ExitTrace.surface.surfaceProps)->game.flDamageModifier;
		flPenetrationModifier = (flPenetrationModifier + flExitPenetrationModifier) / 2;
		flDamageModifier = (flDamageModifier + flExitDamageModifier) / 2;
	}

	int iExitMaterial = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_ExitTrace.surface.surfaceProps)->game.material;
	if (iEnterMaterial == iExitMaterial)
	{
		if (iExitMaterial == CHAR_TEX_WOOD || iExitMaterial == CHAR_TEX_CARDBOARD)
		{
			flPenetrationModifier = 3;
		}
		else if (iExitMaterial == CHAR_TEX_PLASTIC)
		{
			flPenetrationModifier = 2;
		}
	}

	float flTraceDistance = (m_PenetrationData.m_ExitTrace.endpos - m_PenetrationData.m_EnterTrace.endpos).Length();

	float flPenMod = fmax(0.f, (1.f / flPenetrationModifier));

	float flPercentDamageChunk = m_PenetrationData.m_flCurrentDamage * flDamLostPercent;
	float flPenWepMod = flPercentDamageChunk + fmax(0, (3 / m_PenetrationData.m_flPenetrationPower) * 1.25) * (flPenMod * 3.0);

	float flLostDamageObject = ((flPenMod * (flTraceDistance * flTraceDistance)) / 24);
	float flTotalLostDamage = flPenWepMod + flLostDamageObject;

	m_PenetrationData.m_flCurrentDamage -= fmax(flTotalLostDamage, 0.0f);
	if (m_PenetrationData.m_flCurrentDamage <= 0.0f)
		return false;

	m_PenetrationData.m_vecShootPosition = m_PenetrationData.m_ExitTrace.endpos;
	m_PenetrationData.m_PenetrationCount--;

	return true;
}*/

bool C_AutoWall::TraceToExit(const Vector& start, const Vector dir, Vector& out, trace_t* enter_trace, trace_t* exit_trace)
{
	static ConVar* sv_clip_penetration_traces_to_players = interfaces.cvars->FindVar(crypt_str("sv_clip_penetration_traces_to_players"));
		

	Vector          new_end;
	float           dist = 0.0f;
	int				iterations = 23;
	int				first_contents = 0;
	int             contents;
	Ray_t r{};

	while (1)
	{
		iterations--;

		if (iterations <= 0 || dist > 90.f)
			break;

		if (interfaces.global_vars->frametime > interfaces.global_vars->interval_per_tick) {
			dist += 8.0f;
			//low qual
		}
		else {
			dist += 4.0f;
		}
		out = start + (dir * dist);

		contents = interfaces.trace->GetPointContents(out, 0x4600400B, nullptr);

		if (first_contents == -1)
			first_contents = contents;

		if (contents & 0x600400B && (!(contents & CONTENTS_HITBOX) || first_contents == contents))
			continue;

		new_end = out - (dir * 4.f);

		//r.Init(out, new_end);
		//CTraceFilter filter;
		//filter.pSkip = nullptr;
		//csgo.m_engine_trace()->TraceRay(r, MASK_SHOT, &filter, exit_trace);
		TraceLine(out, new_end, 0x4600400B, nullptr, exit_trace);

		if (exit_trace->startsolid && (exit_trace->surface.flags & SURF_HITBOX) != 0)
		{
			//r.Init(out, start);
			//filter.pSkip = exit_trace->m_pEnt;
			//filter.pSkip = (exit_trace->m_pEnt);
			//csgo.m_engine_trace()->TraceRay(r, MASK_SHOT_HULL, &filter, exit_trace);
			TraceLine(out, start, MASK_SHOT_HULL, (IBasePlayer*)exit_trace->m_pEnt, exit_trace);

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				out = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!exit_trace->DidHit() || exit_trace->startsolid)
		{
			if (enter_trace->m_pEnt != interfaces.ent_list->GetClientEntity(NULL)) {
				if (exit_trace->m_pEnt && BreakableEntity(((IBasePlayer*)(exit_trace->m_pEnt))))
				{
					exit_trace->surface.surfaceProps = enter_trace->surface.surfaceProps;
					exit_trace->endpos = start + dir;
					return true;
				}
			}

			continue;
		}

		if ((exit_trace->surface.flags & 0x80u) != 0)
		{
			if (enter_trace->m_pEnt && BreakableEntity(((IBasePlayer*)(enter_trace->m_pEnt)))
				&& exit_trace->m_pEnt && BreakableEntity(((IBasePlayer*)(exit_trace->m_pEnt))))
			{
				out = exit_trace->endpos;
				return true;
			}

			if (!(enter_trace->surface.flags & 0x80u))
				continue;
		}

		if (exit_trace->plane.normal.Dot(dir) <= 1.f) // exit nodraw is only valid if our entrace is also nodraw
		{
			out -= dir * (exit_trace->fraction * 4.0f);
			return true;
		}
	}

	return false;
}



/*
bool C_AutoWall::TraceToExit(Vector vecStart, Vector& vecEnd, CGameTrace* pEnterTrace, CGameTrace* pExitTrace)
{
	int	nFirstContents = 0;

	CTraceFilterSkipEntity tracert1(csgo->local);

	float flDistance = 0.0f;
	while (flDistance <= 90.0f)
	{
		// increase distance

		if ((interfaces.global_vars->frametime < interfaces.global_vars->interval_per_tick)) {
			flDistance += 4.0f;
		}
		else {
			flDistance += 8.0f;
		}

		// calc new end
		vecEnd = vecStart + (m_PenetrationData.m_vecDirection * flDistance);

		// cache contents
		int nContents = interfaces.trace->GetPointContents(vecEnd, 0x4600400B, nullptr);
		if (!nFirstContents)
			nFirstContents = nContents;

		Vector vecTraceEnd = vecEnd - (m_PenetrationData.m_vecDirection * 4.0f);
		if (nContents & 0x600400B && (!(nContents & 0x40000000) || nFirstContents == nContents))
			continue;

		// trace line
		TraceLine(vecEnd, vecTraceEnd, 0x4600400B, csgo->local, NULL, pExitTrace);

		// clip trace to player
		ClipTraceToPlayers((IBasePlayer*)(pExitTrace->m_pEnt), vecEnd, vecTraceEnd, pExitTrace, (CTraceFilter*)(&tracert1), 0x4600400B);

		// check solid and falgs
		if (pExitTrace->startsolid && pExitTrace->surface.flags & SURF_HITBOX)
		{

			CTraceFilterSkipTwoEntities tracert(csgo->local, pExitTrace->m_pEnt);

			Ray_t Ray;
			Ray.Init(vecEnd, vecStart);

			interfaces.trace->TraceRay(Ray, 0x600400B, (CTraceFilter*)(&tracert), pExitTrace);
			if (pExitTrace->DidHit() && !pExitTrace->startsolid)
			{
				vecEnd = pExitTrace->endpos;
				return true;
			}

			continue;
		}

		if (!pExitTrace->DidHit() || pExitTrace->startsolid)
		{
			if (pEnterTrace->m_pEnt != interfaces.ent_list->GetClientEntity(NULL))
			{
				if (pExitTrace->m_pEnt && BreakableEntity(((IBasePlayer*)(pExitTrace->m_pEnt))))
				{
					pExitTrace->surface.surfaceProps = pEnterTrace->surface.surfaceProps;
					pExitTrace->endpos = vecStart + m_PenetrationData.m_vecDirection;

					return true;
				}
			}

			continue;
		}

		if (pExitTrace->surface.flags & 0x80)
		{
			if (pEnterTrace->m_pEnt && BreakableEntity(((IBasePlayer*)(pEnterTrace->m_pEnt))) && pExitTrace->m_pEnt && BreakableEntity(((IBasePlayer*)(pExitTrace->m_pEnt))))
			{
				vecEnd = pExitTrace->endpos;
				return true;
			}

			if (!(pEnterTrace->surface.flags & 0x80u))
				continue;
		}

		if (pExitTrace->plane.normal.Dot(m_PenetrationData.m_vecDirection) > 1.0f)
			return false;

		vecEnd -= m_PenetrationData.m_vecDirection * (pExitTrace->fraction * 4.0f);
		return true;
	}

	return false;
}
*/


bool C_AutoWall::HandleBulletPenetration(IBasePlayer* ignore, CCSWeaponInfo* weaponData, trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, float ff_damage_bullet_penetration, bool pskip)
{
	static ConVar* ff_damage_reduction_bullets = interfaces.cvars->FindVar(crypt_str("ff_damage_reduction_bullets")); //csgo.m_engine_cvars()->FindVar(sxor("ff_damage_reduction_bullets"));

	if (possibleHitsRemaining <= 0 || weaponData->m_flPenetration <= 0 || currentDamage < 1) {
		//possibleHitsRemaining = 0;
		return false;
	}

	//SafeLocalPlayer() false;
	CGameTrace exitTrace;
	auto* pEnemy = (IBasePlayer*)enterTrace.m_pEnt;
	auto* const enterSurfaceData = interfaces.phys_props->GetSurfaceData(enterTrace.surface.surfaceProps);
	const int enter_material = enterSurfaceData->game.material;

	if (!enterSurfaceData || enterSurfaceData->game.flPenetrationModifier <= 0)
		return false;

	const auto enter_penetration_modifier = enterSurfaceData->game.flPenetrationModifier;
	//float enterDamageModifier = enterSurfaceData->game.damagemodifier;// , modifier, finalDamageModifier, combinedPenetrationModifier;
	const bool isSolidSurf = (enterTrace.contents & CONTENTS_GRATE);
	const bool isLightSurf = (enterTrace.surface.flags & SURF_NODRAW);

	if ((possibleHitsRemaining <= 0 && !isLightSurf && !isSolidSurf && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS)
		|| penetrationPower <= 0)
		return false;

	Vector end;

	if (!TraceToExit(enterTrace.endpos, direction, end, &enterTrace, &exitTrace)) {
		if (!(interfaces.trace->GetPointContents(end, 0x600400B) & 0x600400B))
			return false;
	}

	auto* const exitSurfaceData = interfaces.phys_props->GetSurfaceData(exitTrace.surface.surfaceProps);
	const int exitMaterial = exitSurfaceData->game.material;
	const float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
	//float exitDamageModifier = exitSurfaceData->game.damagemodifier;

	float combined_damage_modifier = 0.16f;
	float combined_penetration_modifier;

	//Are we using the newer penetration system?
	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE) {
		combined_damage_modifier = 0.05f;
		combined_penetration_modifier = 3;
	}
	else if (isSolidSurf || isLightSurf) {
		combined_damage_modifier = 0.16f;
		combined_penetration_modifier = 1;
	}
	else if (enter_material == CHAR_TEX_FLESH && ff_damage_reduction_bullets->GetFloat() == 0 && pEnemy != nullptr && pEnemy->IsPlayer() && pEnemy->GetTeam() == csgo->local->GetTeam())
	{
		if (ff_damage_bullet_penetration == 0)
		{
			// don't allow penetrating players when FF is off
			combined_penetration_modifier = 0;
			return false;
		}

		combined_penetration_modifier = ff_damage_bullet_penetration;
	}
	else {
		combined_penetration_modifier = (enter_penetration_modifier + exitSurfPenetrationModifier) / 2;
	}

	if (enter_material == exitMaterial) {
		if (exitMaterial == CHAR_TEX_WOOD || exitMaterial == CHAR_TEX_CARDBOARD)
			combined_penetration_modifier = 3;
		else if (exitMaterial == CHAR_TEX_PLASTIC)
			combined_penetration_modifier = 2;
	}

	/*auto v22 = fmaxf(1.0f / combined_penetration_modifier, 0.0f);
	auto v23 = fmaxf(3.0f / penetrationPower, 0.0f);
	auto penetration_modifier = fmaxf(0.f, 1.f / combined_penetration_modifier);
	auto penetration_distance = (exitTrace.endpos - enterTrace.endpos).Length();
	auto damage_lost = ((currentDamage * combined_damage_modifier) + ((v23 * v22) * 3.0f)) + (((penetration_distance * penetration_distance) * v22) * 0.041666668f);
	auto new_damage = currentDamage - damage_lost;
	currentDamage = new_damage;
	if (new_damage > 0.0f)
	{
		*eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;
		return true;
	}*/

	auto penetration_mod = fmaxf(0.f, (3.f / penetrationPower) * 1.25f);

	float modifier = fmaxf(0, 1.0f / combined_penetration_modifier);

	auto thickness = (exitTrace.endpos - enterTrace.endpos).Length();
	/*thickness *= thickness;
	thickness *= flPenMod;
	thickness /= 24.f;*/

	const auto lost_damage = ((modifier * 3.f) * penetration_mod + (currentDamage * combined_damage_modifier)) + (((thickness * thickness) * modifier) / 24.f);
	/*fmaxf(0, currentDamage * combined_damage_modifier + flPenMod
		* 3.f * fmaxf(0, 3.f / penetrationPower) * 1.25f + thickness);*/

	currentDamage -= std::fmaxf(0.f, lost_damage);

	if (currentDamage < 1.f)
		return false;

	eyePosition = exitTrace.endpos;
	--possibleHitsRemaining;

	return true;
}

void C_AutoWall::FixTraceRay(Vector end, Vector start, trace_t* oldtrace, IBasePlayer* ent) {


	
	if (!ent)
		return;

	const auto mins = ent->GetCollideable()->OBBMins();
	const auto maxs = ent->GetCollideable()->OBBMaxs();

	auto dir(end - start);
	auto len = dir.iamnigger(); //juderei

	const auto center = (mins + maxs) / 2;
	const auto pos(ent->origin() + center);

	auto to = pos - start;
	const float range_along = dir.Dot(to);

	float range;
	if (range_along < 0.f) {
		range = -(to).Length();
	}
	else if (range_along > len) {
		range = -(pos - end).Length();
	}
	else {
		auto ray(pos - (start + (dir * range_along)));
		range = ray.Length();
	}

	if (range <= 60.f) {

		Ray_t ray;
		ray.Init(start, end);

		trace_t trace;
		interfaces.trace->ClipRayToEntity(ray, 0x4600400B, ent, &trace);

		if (oldtrace->fraction > trace.fraction)
			*oldtrace = trace;
	}
	
}

void C_AutoWall::ClipTraceToPlayers(const Vector& start, const Vector& end, uint32_t mask, ITraceFilter* filter, trace_t* tr, IBasePlayer* target) {
	Vector     pos, to, dir, on_ray;
	float      len, range_along, range;
	//CGameTrace new_trace;

	float smallestFraction = tr->fraction;
	constexpr float maxRange = 60.0f;

	//Vector delta(vecAbsEnd - vecAbsStart);
	//const float delta_length = delta.Normalize();

	dir = start - end;
	len = dir.iamnigger();

	Ray_t ray;
	ray.Init(start, end);

	//for (int i = 1; i <= 64; ++i) {
		IBasePlayer* ent = target;
		if (!ent || ent->IsDormant() || !ent->isAlive())
			return;

		if (filter && !filter->ShouldHitEntity(ent, mask))
			return;

		//matrix3x4_t coordinate_frame;
		//Math::AngleMatrix(ent->get_abs_angles(), ent->m_vecOrigin(), coordinate_frame);

		//auto collideble = ent->GetCollideable();
		//auto mins = collideble->OBBMins();
		//auto maxs = collideble->OBBMaxs();

		//auto obb_center = (maxs + mins) * 0.5f;
		//Math::VectorTransform(obb_center, coordinate_frame, obb_center);

		/*auto extend = (obb_center - vecAbsStart);
		auto rangeAlong = delta.Dot(extend);
		float range;
		if (rangeAlong >= 0.0f) {
			if (rangeAlong <= delta_length)
				range = Vector(obb_center - ((delta * rangeAlong) + vecAbsStart)).Length();
			else
				range = -(obb_center - vecAbsEnd).Length();
		}
		else {
			range = -extend.Length();
		}*/

		// set some local vars.
		pos = ent->origin() + ((ent->GetCollideable()->OBBMins() + ent->GetCollideable()->OBBMaxs()) * 0.5f);
		to = pos - start;
		range_along = dir.Dot(to);

		// off start point.
		if (range_along < 0.f)
			range = -(to).Length();

		// off end point.
		else if (range_along > len)
			range = -(pos - end).Length();

		// within ray bounds.
		else {
			on_ray = start + (dir * range_along);
			range = (pos - on_ray).Length();
		}

		if (/*range > 0.0f && */range <= maxRange) {
			trace_t playerTrace;

			//ray.Init(start, end);

			interfaces.trace->ClipRayToEntity(ray, 0x4600400B, ent, &playerTrace);

			if (playerTrace.fraction < smallestFraction) {
				// we shortened the ray - save off the trace
				*tr = playerTrace;
				smallestFraction = playerTrace.fraction;
			}
		}
	//}
}

int C_AutoWall::HitboxToHitgroup(IBasePlayer* m_player, int ihitbox)
{/*
	if (ihitbox < 0 || ihitbox > 19) return 0;
	if (!m_player) return 0;
	const auto model = m_player->GetModel();
	if (!model)
		return 0;
	auto pStudioHdr = csgo.m_model_info()->GetStudioModel(model);
	if (!pStudioHdr)
		return 0;
	auto hitbox = pStudioHdr->pHitbox(ihitbox, m_player->m_nHitboxSet());
	if (!hitbox)
		return 0;
	return hitbox->group;*/

	switch (ihitbox)
	{
	case VHITBOX_HEAD:
	case VHITBOX_NECK:
		return HITGROUP_HEAD;
	case VHITBOX_UPPER_CHEST:
	case VHITBOX_CHEST:
	case VHITBOX_THORAX:
	case VHITBOX_LEFT_UPPER_ARM:
	case VHITBOX_RIGHT_UPPER_ARM:
		return HITGROUP_CHEST;
	case VHITBOX_PELVIS:
	case VHITBOX_LEFT_THIGH:
	case VHITBOX_RIGHT_THIGH:
	case VHITBOX_BODY:
		return HITGROUP_STOMACH;
	case VHITBOX_LEFT_CALF:
	case VHITBOX_LEFT_FOOT:
		return HITGROUP_LEFTLEG;
	case VHITBOX_RIGHT_CALF:
	case VHITBOX_RIGHT_FOOT:
		return HITGROUP_RIGHTLEG;
	case VHITBOX_LEFT_FOREARM:
	case VHITBOX_LEFT_HAND:
		return HITGROUP_LEFTARM;
	case VHITBOX_RIGHT_FOREARM:
	case VHITBOX_RIGHT_HAND:
		return HITGROUP_RIGHTARM;
	default:
		return HITGROUP_STOMACH;
	}
}

bool C_AutoWall::FireBullet(Vector eyepos, IBaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, IBasePlayer* ignore, IBasePlayer* to_who, bool* was_viable, std::vector<float>* power)
{
	if (!pWeapon || !ignore)
		return false;

	//SafeLocalPlayer() false;
	//bool sv_penetration_type;
	//	  Current bullet travel Power to penetrate Distance to penetrate Range               Player bullet reduction convars			  Amount to extend ray by
	float currentDistance = 0;

	static ConVar* damageBulletPenetration = interfaces.cvars->FindVar(crypt_str("ff_damage_bullet_penetration"));

	const float ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	CCSWeaponInfo* weaponData = pWeapon->GetCSWpnData();
	CGameTrace enterTrace;

	CTraceFilter filter;
	filter.pSkip = ignore;

	if (!weaponData)
		return false;

	//Set our current damage to what our gun's initial damage reports it will do
	currentDamage = float(weaponData->m_iDamage);
	auto maxRange = weaponData->m_flRange;
	auto penetrationDistance = weaponData->m_flRange;
	auto penetrationPower = weaponData->m_flPenetration;
	auto RangeModifier = weaponData->m_flRangeModifier;

	//This gets set in FX_Firebullets to 4 as a pass-through value.
	//CS:GO has a maximum of 4 surfaces a bullet can pass-through before it 100% stops.
	//Excerpt from Valve: https://steamcommunity.com/sharedfiles/filedetails/?id=275573090
	//"The total number of surfaces any bullet can penetrate in a single flight is capped at 4." -CS:GO Official
	csgo->last_penetrated_count = 4;
	csgo->last_hitgroup = -1;

	if (power)
	{
		maxRange = power->at(0);
		penetrationDistance = power->at(1);
		penetrationPower = power->at(2);
		currentDamage = power->at(3);
		RangeModifier = power->at(4);
	}

	int penetrated = 0;

	//If our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while (/*ctx.last_penetrated_count >= 0 &&*/currentDamage > 0)
	{
		//Calculate max bullet range

		//Create endpoint of bullet
		Vector end = eyepos + direction * (maxRange - currentDistance);

		TraceLine(eyepos, end, 0x4600400B/*_HULL | CONTENTS_HITBOX*/, ignore, &enterTrace);

		/*if (enterTrace.startsolid)
		{
			enterTrace.endpos = enterTrace.startpos;
			enterTrace.fraction = 0.0f;
		}*/
		//else
		//if (!(enterTrace.contents & CONTENTS_HITBOX)) {
		//if (to_who/* && target_hitbox == HITBOX_HEAD*/ || enterTrace.contents & CONTENTS_HITBOX && enterTrace.m_pEnt) {
			//Pycache/aimware traceray fix for head while players are jumping
		//	FixTraceRay(eyepos + (direction * 40.f), eyepos, &enterTrace, (to_who != nullptr ? to_who : (IBasePlayer*)enterTrace.m_pEnt));
		//}
		//else
		ClipTraceToPlayers(eyepos, eyepos + (direction * 40.f), 0x4600400B, &filter, &enterTrace, to_who);
		//}

		if (enterTrace.fraction == 1.0f)
			return false;

		//if (enterTrace.m_pEnt == nullptr || !((C_BasePlayer*)enterTrace.m_pEnt)->IsPlayer())// !hit_entity || !hit_entity->is_player
		//	enterTrace.m_pEnt = nullptr;

		//We have to do this *after* tracing to the player.
		//int enterMaterial = enterSurfaceData->game.material;

		//calculate the damage based on the distance the bullet traveled.
		currentDistance += enterTrace.fraction * (maxRange - currentDistance);

		//Let's make our damage drops off the further away the bullet is.
		currentDamage *= powf(RangeModifier, (currentDistance / 500));

		if (!(enterTrace.contents & CONTENTS_HITBOX))
			enterTrace.hitgroup = 1;

		//This looks gay as fuck if we put it into 1 long line of code.
		const bool canDoDamage = enterTrace.hitgroup > 0 && enterTrace.hitgroup <= 8 || enterTrace.hitgroup == HITGROUP_GEAR;
		const bool isPlayer = enterTrace.m_pEnt != nullptr
			&& enterTrace.m_pEnt->GetClientClass()
			&& enterTrace.m_pEnt->GetClientClass()->m_ClassID == ClassId->CCSPlayer
			&& (!csgo->local || !csgo->local->isAlive() || ((IBasePlayer*)enterTrace.m_pEnt)->GetTeam() != csgo->local->GetTeam());

		if (to_who)
		{
			if (enterTrace.m_pEnt && to_who == enterTrace.m_pEnt && canDoDamage && isPlayer) {
				const int group = (pWeapon->GetItemDefinitionIndex() == WEAPON_TASER) ? HITGROUP_GENERIC : enterTrace.hitgroup;
				csgo->last_hitgroup = group;

				ScaleDamage(enterTrace, weaponData, currentDamage);

				if (was_viable != nullptr)
					*was_viable = (penetrated == 0);

				csgo->force_hitbox_penetration_accuracy = false;

				return true;
			}
		}
		else
		{
			if (enterTrace.m_pEnt && canDoDamage && isPlayer) {
				const int group = (pWeapon->GetItemDefinitionIndex() == WEAPON_TASER) ? HITGROUP_GENERIC : enterTrace.hitgroup;
				csgo->last_hitgroup = group;

				ScaleDamage(enterTrace, weaponData, currentDamage);

				if (was_viable != nullptr)
					*was_viable = (penetrated == 0);

				csgo->force_hitbox_penetration_accuracy = false;

				return true;
			}
		}

		//Sanity checking / Can we actually shoot through?
		if (currentDistance > maxRange && penetrationPower
			|| interfaces.phys_props->GetSurfaceData(enterTrace.surface.surfaceProps)->game.flPenetrationModifier < 0.1f) {
			return false;//ctx.last_penetrated_count = 0;
		}

		const auto prev =csgo->last_penetrated_count;

		//Calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if (!HandleBulletPenetration(ignore, weaponData, enterTrace, eyepos, direction, csgo->last_penetrated_count, currentDamage, penetrationPower, ff_damage_bullet_penetration)) {
			break;
		}
		if (prev != csgo->last_penetrated_count)
			penetrated++;
	}

	return false;
}

////////////////////////////////////// Usage Calls //////////////////////////////////////
float C_AutoWall::CanHit(Vector& vecEyePos, Vector& point, IBasePlayer* target)
{
	Vector angles, direction;
	Vector tmp = point - vecEyePos;
	float currentDamage = 0;

	Math::VectorAngles(tmp, angles);
	Math::AngleVectors(angles, &direction);
	direction.Normalize();

	if (csgo->local->GetWeapon() != nullptr && csgo->local->GetWeapon()->IsGun() && FireBullet(vecEyePos, csgo->local->GetWeapon(), direction, currentDamage, csgo->local, target))
		return currentDamage;

	return -1; //That wall is just a bit too thick buddy
}

float C_AutoWall::CanHit(const Vector& vecEyePos, Vector& point, IBasePlayer* ignore_ent, IBasePlayer* to_who, bool* was_viable)
{
	if (ignore_ent == nullptr || to_who == nullptr)
		return 0;

	Vector direction;
	Vector tmp = point - vecEyePos;
	float currentDamage = 0;

	//Math::VectorAngles(tmp, angles);
	//Math::AngleVectors(angles, &direction);
	direction = tmp.Normalized();

	if (csgo->local->GetWeapon() != nullptr)
	{
		if (csgo->local->GetWeapon()->IsGun() && FireBullet(vecEyePos, csgo->local->GetWeapon(), direction, currentDamage, ignore_ent, to_who, was_viable))
			return currentDamage;
		else
			return -1;
	}

	return -1; //That wall is just a bit too thick buddy
}

float C_AutoWall::SimulateShot(Vector& vecEyePos, Vector& point, IBasePlayer* ignore_ent, IBasePlayer* to_who, bool* was_viable)
{
	if (ignore_ent == nullptr || to_who == nullptr)
		return 0;

	Vector angles, direction;
	const Vector tmp(point - vecEyePos);
	float currentDamage = 0.f;

	Math::VectorAngles(tmp, angles);
	Math::AngleVectors(angles, &direction);
	direction.Normalize();

	/*
		maxRange = power[0];
		penetrationDistance = power[1];
		penetrationPower = power[2];
		currentDamage = power[3];
	*/

	static std::vector<float> power = { 4000.f, 4000.f, 2.50f, 80.f, 1.f };

	if (csgo->local->GetWeapon() != nullptr)
	{
		if (FireBullet(vecEyePos, csgo->local->GetWeapon(), direction, currentDamage, ignore_ent, to_who,  was_viable, &power))
			return currentDamage;
		else
			return -1;
	}

	return -1; //That wall is just a bit too thick buddy
}

/*
bool C_AutoWall::SimulateFireBullet()
{
	if (!m_PenetrationData.m_Weapon || !m_PenetrationData.m_WeaponData)
		return false; 

	IBasePlayer* m_LastPlayerHit = nullptr;
	while (m_PenetrationData.m_PenetrationCount > 0)
	{
		float flRemains = (m_PenetrationData.m_flMaxRange - m_PenetrationData.m_flCurrentDistance);
		Vector vecEnd = m_PenetrationData.m_vecShootPosition + (m_PenetrationData.m_vecDirection * flRemains);


		Ray_t Ray;
		Ray.Init(m_PenetrationData.m_vecShootPosition, vecEnd);

		CTraceFilterSkipTwoEntities tracert(csgo->local, m_LastPlayerHit);

		interfaces.trace->TraceRay(Ray, MASK_SHOT_HULL | CONTENTS_HITBOX, (CTraceFilter*)&tracert, &m_PenetrationData.m_EnterTrace);

		ClipTraceToPlayers((IBasePlayer*)(m_PenetrationData.m_EnterTrace.m_pEnt), m_PenetrationData.m_vecShootPosition, vecEnd + (m_PenetrationData.m_vecDirection * 40.0f), &m_PenetrationData.m_EnterTrace, (CTraceFilter*)&tracert, MASK_SHOT_HULL | CONTENTS_HITBOX);

		m_LastPlayerHit = m_PenetrationData.m_EnterTrace.m_pEnt;

		surfacedata_t* pEnterSurfaceData = interfaces.phys_props->GetSurfaceData(m_PenetrationData.m_EnterTrace.surface.surfaceProps);
		if ((int)(m_PenetrationData.m_EnterTrace.fraction))
			break;

		float flEnterSurfacePenetrationModifier = pEnterSurfaceData->game.flPenetrationModifier;
		if (m_PenetrationData.m_flCurrentDistance > m_PenetrationData.m_flPenetrationDistance && m_PenetrationData.m_flPenetrationPower > 0.f || flEnterSurfacePenetrationModifier < 0.1f)
			break;

		m_PenetrationData.m_flCurrentDistance += m_PenetrationData.m_EnterTrace.fraction * flRemains;
		m_PenetrationData.m_flCurrentDamage *= std::pow(m_PenetrationData.m_WeaponData->m_flRangeModifier, (m_PenetrationData.m_flCurrentDistance / 500.0f));

		if (m_PenetrationData.m_EnterTrace.m_pEnt)
		{
			IBasePlayer* pPlayer = (IBasePlayer*)(m_PenetrationData.m_EnterTrace.m_pEnt);
			if (pPlayer && pPlayer->IsPlayer())
			{
				if (!pPlayer->isAlive() || !pPlayer->isEnemy(csgo->local))
					return false;

				if (m_PenetrationData.m_EnterTrace.hitgroup != HITGROUP_GEAR && m_PenetrationData.m_EnterTrace.hitgroup != HITGROUP_GENERIC)
				{

					this->ScaleDamage(m_PenetrationData.m_EnterTrace, m_PenetrationData.m_flCurrentDamage);
					return true;
				}
			}
		}

		if (this->HandleBulletPenetration() && m_PenetrationData.m_flCurrentDamage > 0.0f)
			continue;

		break;
	}

	return false;
}
*/
/*
void C_AutoWall::ScaleDamage(CGameTrace& Trace, float_t& flDamage)
{
	bool bHasHeavyArmour = ((IBasePlayer*)(Trace.m_pEnt))->HeavyArmor();
	switch (Trace.hitgroup)
	{
	case HITGROUP_HEAD:
		if (bHasHeavyArmour)
			flDamage *= 2.0f;
		else
			flDamage *= m_PenetrationData.m_WeaponData->flHeadshotMultiplier;

		break;

	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:

		flDamage *= 1.f;
		break;

	case HITGROUP_STOMACH:

		flDamage *= 1.25f;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:

		flDamage *= 0.75f;
		break;
	}

	if (!this->IsArmored(((IBasePlayer*)(Trace.m_pEnt)), Trace.hitgroup))
		return;

	float fl47 = 1.f, flArmorBonusRatio = 0.5f;

	float_t flArmorRatio = m_PenetrationData.m_WeaponData->m_flArmorRatio * 0.5f;
	if (bHasHeavyArmour)
	{
		flArmorBonusRatio = 0.33f;
		flArmorRatio = m_PenetrationData.m_WeaponData->m_flArmorRatio * 0.25f;
		fl47 = 0.33f;
	}

	float flNewDamage = flDamage * flArmorRatio;
	if (bHasHeavyArmour)
		flNewDamage *= 0.85f;

	if (((flDamage - (flDamage * flArmorRatio)) * (fl47 * flArmorBonusRatio)) > ((IBasePlayer*)(Trace.m_pEnt))->GetArmor())
		flNewDamage = flDamage - (((IBasePlayer*)(Trace.m_pEnt))->GetArmor() / flArmorBonusRatio);

	flDamage = flNewDamage;
}

bool C_AutoWall::IsArmored(IBasePlayer* pPlayer, int32_t nHitGroup)
{
	bool bResult = false;
	if (pPlayer->GetArmor() > 0)
	{
		switch (nHitGroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			bResult = true;
			break;
		case HITGROUP_HEAD:
			bResult = pPlayer->HasHelmet();
			break;
		}
	}

	return bResult;
}

void C_AutoWall::ClipTraceToPlayers(IBasePlayer* pPlayer, Vector vecStart, Vector vecEnd, CGameTrace* Trace, CTraceFilter* pTraceFilter, uint32_t nMask)
{
	if (!pPlayer)
		return;


	Ray_t Ray;
	Ray.Init(vecStart, vecEnd);

	CGameTrace NewTrace;
	if (!pPlayer || !pPlayer->IsPlayer() || !pPlayer->isAlive() || pPlayer->IsDormant() || !pPlayer->isEnemy(csgo->local))
		return;

	if (pTraceFilter && !pTraceFilter->ShouldHitEntity(pPlayer, MASK_SHOT_HULL | CONTENTS_HITBOX))
		return;

	float flRange = Math::DistanceToRay(pPlayer->WorldSpaceCenter(), vecStart, vecEnd);
	if (flRange < -60.0f || flRange > 60.0f)
		return;

	interfaces.trace->ClipRayToEntity(Ray, nMask, pPlayer, &NewTrace);
	if (NewTrace.fraction > Trace->fraction)
		std::memcpy(Trace, &NewTrace, sizeof(CGameTrace));
}

float C_AutoWall::GetPointDamage(Vector vecShootPosition, Vector vecTargetPosition)
{
	if (!m_PenetrationData.m_Weapon || !m_PenetrationData.m_WeaponData)
		return false;

	m_PenetrationData.m_vecShootPosition = vecShootPosition;
	m_PenetrationData.m_vecTargetPosition = vecTargetPosition;
	m_PenetrationData.m_vecDirection = GetPointDirection(vecShootPosition, vecTargetPosition);
	m_PenetrationData.m_flCurrentDamage = m_PenetrationData.m_flWeaponDamage;
	m_PenetrationData.m_flCurrentDistance = 0.0f;
	m_PenetrationData.m_PenetrationCount = 4;

	m_PenetrationData.m_flDamageModifier = 0.5f;
	m_PenetrationData.m_flPenetrationModifier = 1.0f;

	if (SimulateFireBullet())
		return m_PenetrationData.m_flCurrentDamage;

	return 0.0f;
}

Vector C_AutoWall::GetPointDirection(Vector vecShootPosition, Vector vecTargetPosition)
{
	Vector vecDirection;
	Vector angDirection;

	Math::VectorAngles(vecTargetPosition - vecShootPosition, angDirection);
	Math::AngleVectors(angDirection, vecDirection);

	vecDirection.NormalizeInPlace();
	return vecDirection;
}*/
/*
bool CGameTrace::DidHitWorld() const
{
	return m_pEnt == interfaces.ent_list->GetClientEntity(NULL);
}

bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEnt != nullptr && !DidHitWorld();
}
*/
/*
bool C_AutoWall::IsPenetrablePoint(Vector vecShootPosition, Vector vecTargetPosition)
{
	Vector angLocalAngles;
	interfaces.engine->GetViewAngles(angLocalAngles);

	Vector vecDirection;
	Math::AngleVectors(angLocalAngles, vecDirection);

	CGameTrace Trace;
	TraceLine(vecShootPosition, vecTargetPosition, MASK_SHOT_HULL | CONTENTS_HITBOX, csgo->local, NULL, &Trace);

	if ((int)(Trace.fraction))
		return false;

	PenetrationData_t* OldPenetrationData = &m_PenetrationData;
	m_PenetrationData.m_EnterTrace = Trace;
	m_PenetrationData.m_vecDirection = vecDirection;
	m_PenetrationData.m_flCurrentDistance = 0.0f;
	m_PenetrationData.m_flCurrentDamage = m_PenetrationData.m_flWeaponDamage;
	m_PenetrationData.m_PenetrationCount = 1;
	m_PenetrationData.m_vecShootPosition = vecShootPosition;
	m_PenetrationData.m_vecTargetPosition = vecTargetPosition;

	if (!this->HandleBulletPenetration())
	{
		m_PenetrationData = *OldPenetrationData;
		return false;
	}

	m_PenetrationData = *OldPenetrationData;
	return true;
}*/

bool C_AutoWall::BreakableEntity(IBasePlayer* entity)
{
	if (!entity)
		return false;

	ClientClass* pClass = (ClientClass*)entity->GetClientClass();

	if (!pClass)
	{
		return false;
	}

	if (pClass == nullptr)
	{
		return false;
	}

	return pClass->m_ClassID == 30 || pClass->m_ClassID == 31;
}
/*
void C_AutoWall::TraceLine(const Vector src, const Vector dst, int mask, IHandleEntity* entity, int collision_group, CGameTrace* trace) {
	std::uintptr_t filter[4] = {
		*reinterpret_cast<std::uintptr_t*>(csgo->tracefilterdw),
		reinterpret_cast<std::uintptr_t>(entity),
		(uintptr_t)(collision_group),
		0
	};

	auto ray = Ray_t();
	ray.Init(src, dst);

	return interfaces.trace->TraceRay(ray, mask, (CTraceFilter*)(filter), trace);
}*/