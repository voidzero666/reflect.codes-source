#pragma once
#include "Hooks.h"
#include "RageBackTracking.h"
#include "Hitmarker.h"
#include <list>

enum zeus_stages {
	none = -1,
	good,
	warning,
	fatal
};

struct BOX
{
	int h, w, x, y;
};
struct BaseData {
	BaseData(IBasePlayer* entity) noexcept;

	constexpr auto operator<(const BaseData& other) const
	{
		return distanceToLocal > other.distanceToLocal;
	}

	float distanceToLocal;
	Vector obbMins, obbMaxs;
	Vector origin;
	matrix coordinateFrame;
	struct {
		int x, y, w, h;
	} box;
	float time_to_die = 0.f;
	bool m_bDidSmokeEffect;
	int m_nSmokeEffectTickBegin;
};

struct ColorToggleThickness {
	ColorToggleThickness() = default;
	ColorToggleThickness(float thickness) : thickness{ thickness } { }
	float thickness = 1.0f;
};
struct Trail : ColorToggleThickness {
	enum Type {
		Line = 0,
		Circles,
		FilledCircles
	};
	color_t color = color_t(255, 255, 255, 255);
	bool rainbow;
	int type = Line;
	float time = 2.0f;
};
struct WeaponData_t : BaseData {
	WeaponData_t(IBasePlayer *entity) noexcept;

	int clip;
	int reserveAmmo;
	ItemDefinitionIndex index;
	std::string group = crypt_str("All");
	std::string name = crypt_str("All");
	std::string displayName;
};
struct ProjectileInfo_t : BaseData {
	ProjectileInfo_t(IBasePlayer* projectile) noexcept;

	void update(IBasePlayer* projectile) noexcept;

	constexpr auto operator==(int otherHandle) const noexcept
	{
		return handle == otherHandle;
	}

	bool exploded = false;
	bool thrownByLocalPlayer = false;
	bool thrownByEnemy = false;
	int handle;
	std::string name = crypt_str("");
	std::vector<std::pair<float, Vector>> trajectory;
	bool trail_added = false;
	int classID;
	IBasePlayer* entity;
};
struct InfernoInfo_t {
	std::vector<Vector> points;
	Vector entity_origin;
	Vector origin;
	float time_to_die;
	float range;
};
struct INFO_t
{
	IBasePlayer* player;
	std::string weapon_name;
	IBaseCombatWeapon* weapon;
	int hp;
	int ammo, max_ammo;

	int player_distance;

	bool offscreen;
	bool on_screen;
	int zeuser_stages;
	bool is_valid = false;
	bool is_gun;
	int choke;
	bool scope,
		flash, have_armor, have_helmet, dormant,
		have_kit, fake_duck;

	std::string name, last_place;
	Vector origin;
	float angle1, angle2;
	float duck, last_seen_time;
	Vector body_pos;
	bool visible;
	float alpha;
	struct {
		int x, y, w, h;
	}box;
	struct
	{
		std::vector<int> hitboxes;
		std::vector<Vector> points;
		matrix bones[128];
	}AnimInfo;

	studiohdr_t* hdr;

	Vector bone_pos_child[128];
	Vector bone_pos_parent[128];
	int ClassID;

	void Reset() {
		memset(&player, 0, sizeof(player));
		memset(&weapon, 0, sizeof(weapon));
		memset(&hdr, 0, sizeof(hdr));
		memset(&AnimInfo.bones, 0, sizeof(AnimInfo.bones));

		AnimInfo.hitboxes.clear();
		AnimInfo.points.clear();

		name = crypt_str("");
		last_place = crypt_str("");
		weapon_name = crypt_str("");

		hp = 0;
		ammo = 0;
		max_ammo = 0;
		choke = 0;
		duck = 0.f;
		last_seen_time = 0.f;
		player_distance = 0;
		alpha = 0.f;
		box.x = 0;
		box.y = 0;
		box.w = 0;
		box.h = 0;

		zeuser_stages = -1;

		is_valid = false;
		is_gun = false;
		scope = false;
		flash = false;
		have_armor = false;
		have_helmet = false;
		dormant = false;
		have_kit = false;
		fake_duck = false;
		visible = false;

		origin = Vector(0, 0, 0);
		body_pos = Vector(0, 0, 0);

		for (int i = 0; i < 64; i++) 
			csgo->EntityAlpha[i] = 0.f;
		
		for (int i = 0; i < 128; i++) {
			bone_pos_child[i] = Vector(0, 0, 0);
			bone_pos_parent[i] = Vector(0, 0, 0);
		}
		offscreen = false;
		on_screen = false;
		ClassID = 0;
	}
};

struct BombInfo_t {
	Vector origin;
	float time;
	float blow;
	float blow_time;
	float bomb_ticking;
	float defuse_time;
	float defuse_cooldown;
	bool is_defusing;
	bool bomb_defused;
};

class CVisuals
{
public:
	std::list<ProjectileInfo_t> ProjectileInfo;
	std::vector<WeaponData_t> WeaponData;
	std::vector<InfernoInfo_t> InfernoInfo;
	std::vector<BombInfo_t> BombInfo;

	int health[65];

	INFO_t player_info[64] = {};
	void DrawWatermark();
	void DrawLocalVisuals();
	void Draw();
	void StoreOtherInfo();
	void ResetInfo();
	void RecordInfo();
	void DrawAngleLine(Vector origin, float angle, color_t color);
	void RecoilCrosshair();
	void TeamDamage_CollectInfo(IGameEvent* event);
	void TeamDamage_DisplayInfo();
	void DrawKeybindsWindow();
	void DrawSpectatorWindow();
	void Graph_CollectInfo();
	void DrawDlights();
	void sunsetmode();
	void fog();
	Vector strored_origin[64];
};

namespace Z {
	enum class WeaponType {
		Knife = 0,
		Pistol,
		SubMachinegun,
		Rifle,
		Shotgun,
		SniperRifle,
		Machinegun,
		C4,
		Placeholder,
		Grenade,
		Unknown,
		StackableItem,
		Fists,
		BreachCharge,
		BumpMine,
		Tablet,
		Melee
	};

	enum class WeaponId : short {
		Deagle = 1,
		Elite,
		Fiveseven,
		Glock,
		Ak47 = 7,
		Aug,
		Awp,
		Famas,
		G3SG1,
		GalilAr = 13,
		M249,
		M4A1 = 16,
		Mac10,
		P90 = 19,
		ZoneRepulsor,
		Mp5sd = 23,
		Ump45,
		Xm1014,
		Bizon,
		Mag7,
		Negev,
		Sawedoff,
		Tec9,
		Taser,
		Hkp2000,
		Mp7,
		Mp9,
		Nova,
		P250,
		Shield,
		Scar20,
		Sg553,
		Ssg08,
		GoldenKnife,
		Knife,
		Flashbang = 43,
		HeGrenade,
		SmokeGrenade,
		Molotov,
		Decoy,
		IncGrenade,
		C4,
		Healthshot = 57,
		KnifeT = 59,
		M4a1_s,
		Usp_s,
		Cz75a = 63,
		Revolver,
		TaGrenade = 68,
		Axe = 75,
		Hammer,
		Spanner = 78,
		GhostKnife = 80,
		Firebomb,
		Diversion,
		FragGrenade,
		Snowball,
		BumpMine,
		Bayonet = 500,
		ClassicKnife = 503,
		Flip = 505,
		Gut,
		Karambit,
		M9Bayonet,
		Huntsman,
		Falchion = 512,
		Bowie = 514,
		Butterfly,
		Daggers,
		Paracord,
		SurvivalKnife,
		Ursus = 519,
		Navaja,
		NomadKnife,
		Stiletto = 522,
		Talon,
		SkeletonKnife = 525,
		GloveStuddedBloodhound = 5027,
		GloveT,
		GloveCT,
		GloveSporty,
		GloveSlick,
		GloveLeatherWrap,
		GloveMotorcycle,
		GloveSpecialist,
		GloveHydra
	};

	enum class ClassId {
		BaseCSGrenadeProjectile = 9,
		BreachChargeProjectile = 29,
		BumpMineProjectile = 33,
		C4,
		Chicken = 36,
		CSPlayer = 40,
		CSPlayerResource,
		CSRagdoll = 42,
		Deagle = 46,
		DecoyProjectile = 48,
		Drone,
		Dronegun,
		DynamicProp = 52,
		EconEntity = 53,
		EconWearable,
		Hostage = 97,
		Healthshot = 104,
		Cash,
		Knife = 107,
		KnifeGG,
		MolotovProjectile = 114,
		AmmoBox = 125,
		LootCrate,
		RadarJammer,
		WeaponUpgrade,
		PlantedC4,
		PropDoorRotating = 143,
		SensorGrenadeProjectile = 153,
		SmokeGrenadeProjectile = 157,
		SnowballPile = 160,
		SnowballProjectile,
		Tablet = 172,
		Aug = 232,
		Awp,
		Elite = 239,
		FiveSeven = 241,
		G3sg1,
		Glock = 245,
		P2000,
		P250 = 258,
		Scar20 = 261,
		Sg553 = 265,
		Ssg08 = 267,
		Tec9 = 269
	};

}