#pragma once
#include "Hooks.h"

#define DAMAGE_NO		0
#define DAMAGE_EVENTS_ONLY	1
#define DAMAGE_YES		2
#define DAMAGE_AIM		3
#define CHAR_TEX_STEAM_PIPE		11

enum {
	CHAR_TEX_ANTLION = 'A',
	CHAR_TEX_BLOODYFLESH = 'B',
	CHAR_TEX_CONCRETE = 'C',
	CHAR_TEX_DIRT = 'D',
	CHAR_TEX_EGGSHELL = 'E',
	CHAR_TEX_FLESH = 'F',
	CHAR_TEX_GRATE = 'G',
	CHAR_TEX_ALIENFLESH = 'H',
	CHAR_TEX_CLIP = 'I',
	CHAR_TEX_PLASTIC = 'L',
	CHAR_TEX_METAL = 'M',
	CHAR_TEX_SAND = 'N',
	CHAR_TEX_FOLIAGE = 'O',
	CHAR_TEX_COMPUTER = 'P',
	CHAR_TEX_SLOSH = 'S',
	CHAR_TEX_TILE = 'T',
	CHAR_TEX_CARDBOARD = 'U',
	CHAR_TEX_VENT = 'V',
	CHAR_TEX_WOOD = 'W',
	CHAR_TEX_GLASS = 'Y',
	CHAR_TEX_WARPSHIELD = 'Z',
};
/*
struct PenetrationData_t
{
	Vector m_vecShootPosition = Vector(0, 0, 0);
	Vector m_vecTargetPosition = Vector(0, 0, 0);
	Vector m_vecDirection = Vector(0, 0, 0);

	int32_t m_PenetrationCount = 4;

	float_t m_flPenetrationPower = 0.0f;
	float_t m_flPenetrationDistance = 0.0f;

	float_t m_flDamageModifier = 0.5f;
	float_t m_flPenetrationModifier = 1.0f;

	float_t m_flMaxRange = 0.0f;
	float_t m_flWeaponDamage = 0.0f;
	float_t m_flCurrentDamage = 0.0f;
	float_t m_flCurrentDistance = 0.0f;

	float m_DamageReduction;

	CGameTrace m_EnterTrace;
	CGameTrace m_ExitTrace;

	CCSWeaponInfo* m_WeaponData = NULL;
	IBaseCombatWeapon* m_Weapon = NULL;
};
*/
class C_AutoWall
{
public:
	virtual void TraceLine(Vector& absStart, const Vector& absEnd, unsigned int mask, IBasePlayer* ignore, CGameTrace* ptr);
	virtual float ScaleDamage(IBasePlayer* player, float damage, float armor_ratio, int hitgroup);
	virtual void ScaleDamage(CGameTrace& enterTrace, CCSWeaponInfo* weaponData, float& currentDamage);
	//void ScaleDamage(C_BasePlayer* entity, int hitgroup, weapon_info* weaponData, float& currentDamage);
	//virtual uint32_t get_filter_simple_vtable();
	virtual bool TraceToExit(const Vector& start, const Vector dir, Vector& out, trace_t* enter_trace, trace_t* exit_trace);
	//bool TraceToExit(trace_t& enter_trace, trace_t& exit_trace, C_BasePlayer* ignore, const Vector start_position, const Vector direction);
	virtual bool HandleBulletPenetration(IBasePlayer* ignore, CCSWeaponInfo* weaponData, trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, float ff_damage_bullet_penetration, bool pskip = false);
	//bool HandleBulletPenetration(weapon_info* weaponData, trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, float shit, bool pskip = false);
	virtual void FixTraceRay(Vector end, Vector start, trace_t* oldtrace, IBasePlayer* ent);
	virtual void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, uint32_t mask, ITraceFilter* filter, trace_t* tr, IBasePlayer* ent);
	virtual int HitboxToHitgroup(IBasePlayer* m_player, int ihitbox);
	virtual bool FireBullet(Vector eyepos, IBaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, IBasePlayer* ignore, IBasePlayer* to_who = nullptr, bool* was_viable = nullptr, std::vector<float>* = nullptr);
	virtual float CanHit(Vector& vecEyePos, Vector& point, IBasePlayer* target);
	virtual float CanHit(const Vector& vecEyePos, Vector& point, IBasePlayer* ignore_ent, IBasePlayer* start_ent, bool* was_viable = nullptr);
	virtual float SimulateShot(Vector& vecEyePos, Vector& point, IBasePlayer* ignore_ent, IBasePlayer* to_who, bool* was_viable = nullptr);
	bool BreakableEntity(IBasePlayer* entity);
};

inline C_AutoWall* g_AutoWall = new C_AutoWall();