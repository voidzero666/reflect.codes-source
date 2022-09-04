#pragma once
#include "Hooks.h"
#include <unordered_map>
#include <optional>
#include <deque>

struct animation
{
	animation() = default;
	explicit animation(IBasePlayer* player);
	explicit animation(IBasePlayer* player, Vector last_reliable_angle);
	void restore(IBasePlayer* player) const;
	void apply(IBasePlayer* player) const;
	void build_inversed_bones(IBasePlayer* player);
	void build_unresoved_bones(IBasePlayer* player);
	void build_server_bones(IBasePlayer* player);
	void build_safe_matrix(IBasePlayer* player, matrix* aMatrix);
	bool is_valid(float range, float max_unlag);
	bool is_valid_extended();

	IBasePlayer* player{};
	int32_t index{};

	bool valid{}, has_anim_state{};
	alignas(16) matrix bones[128];
	//old system
	//alignas(16) matrix n_matrix[128];
	//alignas(16) matrix p_matrix[128];
	alignas(16) matrix negative_matrix[128];
	alignas(16) matrix positive_matrix[128];

	bool dormant{};
	uint8_t scan_mode = 128;
	Vector velocity;
	bool resolved;
	Vector origin;
	matrix* bone_cache;
	Vector abs_origin;
	Vector obb_mins;
	Vector obb_maxs;
	CAnimationLayer layers[13];
	CAnimationLayer resolver_layer[4];
	CAnimationLayer layer6_b;
	CAnimationLayer layer6_n;
	CAnimationLayer layer6_nl;
	CAnimationLayer layer6_p;
	CAnimationLayer layer6_pl;
	CAnimationLayer layer6_c;

	//ragebot backup vars test
	CAnimationLayer Rlayers[13]; //layers for the ragebot to backup and readjust
	std::array<float, 24> Rposes;
	Vector Reye_angles;
	float Rduck{};
	int Rflags{};

	std::array<float, 24> poses;
	CCSGOPlayerAnimState* anim_state;
	float anim_time{};
	float sim_time{};
	float interp_time{};
	float duck{};
	float lby{};
	float last_shot_time{};
	Vector last_reliable_angle{};
	Vector eye_angles;
	Vector abs_ang;
	int flags{};
	int eflags{};
	int effects{};
	float m_flFeetCycle{};
	float m_flFeetYawRate{};
	int lag{};
	bool didshot;
	bool safepoints;
	bool jumped;
	int came_from_dormant = -1;
	int updateDelay = 0;
	string resolver_mode;
};


class CMAnimationFix
{
public:
	struct animation_info {
		animation_info(IBasePlayer* player, std::deque<animation> animations)
			: player(player), frames(std::move(animations)), last_spawn_time(0) { }

		void update_animations(animation* to, animation* from);

		IBasePlayer* player{};
		std::deque<animation> frames;

		// last time this player spawned
		float last_spawn_time;
		float goal_feet_yaw;
		Vector last_reliable_angle;
		float air_time = 0.f;
		float last_landtime;
	};


	unordered_map<CBaseHandle, animation_info*> animation_infos;

	CCSGOPlayerAnimState* US_Animstate[64];
	CCSGOPlayerAnimState* IS_Animstate[64];

	void UpdateRealState();

	void FixPvs();

	void UpdatePlayers();
	void UpdateFakeState();

	CCSGOPlayerAnimState* FakeAnimstate = nullptr;
	CCSGOPlayerAnimState* RealAnimstate = nullptr;

	void rotationResolve(IBasePlayer* player, animation* record, animation* previous);
	std::optional<float> generateRotations(IBasePlayer* entity, animation* record, animation* previous);
	void simStepUpdate(IBasePlayer* player);
	animation_info* get_animation_info(IBasePlayer* player);
	animation* get_latest_animation(IBasePlayer* player);
	std::vector<animation*> get_valid_animations(IBasePlayer* player);
	animation* get_oldest_animation(IBasePlayer* player);
	animation* get_latest_firing_animation(IBasePlayer* player);
	animation* get_resolved_animation(IBasePlayer* player);
};
extern CMAnimationFix* g_Animfix;