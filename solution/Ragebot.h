#pragma once
#include "Autowall.hpp"
#include "AnimationFix.h"

class ShotSnapshot
{
public:
	IBasePlayer* entity;
	string hitbox_where_shot;
	string resolver;
	string resolver2;
	float time;
	float first_processed_time;
	bool weapon_fire, bullet_impact;
	int hitgroup_hit;
	int damage;
	int intended_damage;
	Vector intended_position;
	int hitbox;
	animation record;
	Vector eyeangles, impact, start;
	int backtrack;
	matrix* pMat;
	std::string player_name;
	string get_info();

};

extern std::vector<ShotSnapshot> shot_snapshots;

extern bool CanHitHitbox(const Vector start, const Vector end, animation* _animation, int box, matrix* bones = nullptr);

struct CPoint {
	CPoint(Vector position = { 0,0,0 }, int hitbox = -1, float damage = -1.f, bool lethal = false, bool safe = false) {
		this->position = position;
		this->hitbox = hitbox;
		this->damage = damage;
		this->lethal = lethal;
		this->safe = safe;
	}
	Vector position;
	int hitbox;
	float damage;
	bool lethal;
	bool safe;
};

class Ragebot : public Singleton<Ragebot>
{
public:
	enum ScanType : uint8_t {
		SCAN_DEFAULT = 0U,
		SCAN_PREFER_SAFEPOINT = 1U,
		SCAN_FORCE_SAFEPOINT = 2U,
		SCAN_SAFE_BAIM = 3U
	};

	bool dt_ready;

	bool can_not_shoot_due_to_cock = false;
	bool m_revolver_fire = false;

	void BackupPlayer(animation*);
	void SetAnims(animation*, matrix* m = nullptr);
	void RestorePlayer(animation*);
	CPoint GetBestPoint(std::vector<CPoint> points, int health, uint8_t scan_mode);
	Vector HeadScan(animation*, int&, float&, float);
	Vector PrimaryScan(animation*, int&, float&, float&, float);

	bool PredictiveStopScan(animation*, float&, float);

	Vector FullScan(animation*, int&, float&, float&, float);
	Vector GetPoint(IBasePlayer*, int, matrix[128]);
	CWeaponConfig CurrentSettings();
	std::vector<int> GetHitboxesToScan(IBasePlayer*);
	float GetBodyScale(IBasePlayer*);
	float GetHeadScale(IBasePlayer*);
	std::vector<Vector> GetAdvancedHeadPoints(IBasePlayer* pBaseEntity, matrix bones[128]);
	std::vector<Vector> GetMultipoints(IBasePlayer*, int, matrix[128]);
	int HitboxToHitgroup(CSGOHitboxID);

	bool force_accuracy;
	bool do_auto_stop;
	void DoQuickStop();

	int GetTicksToShoot();
	int GetTicksToStop();
	bool HoldFiringAnimation();
	bool FastStop();
	Vector GetVisualHitbox(IBasePlayer* ent, int ihitbox);
	Vector GetAimVector(IBasePlayer*, float &, animation*&, int&, int&);
	bool findPredictiveStopPoint(IBasePlayer* pTarget, float& simtime, animation*& best_anims);
	bool Hitchance(Vector, animation*, int&);
	bool IsAbleToShoot(float tick_shift = 0.f);
	void DropTarget();
	uint8_t ScanMode(IBasePlayer* pEntity);
	float LerpTime();
	void DrawCapsule(IBasePlayer*, matrix*, color_t);

	void Run();
	bool in_ragebot = false;
	animation backup_anims[65];
	int target_index = -1;
	float best_distance;
	float current_aim_simulationtime;
	Vector current_aim_position;
	float best_damage;
	CWeaponConfig current_settings;

	bool shot;
	bool target_lethal;
	matrix BoneMatrix[128];
};
