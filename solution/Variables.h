#pragma once
#include "DLL_MAIN.h"
#include "json/json.h"
#include "GUI/keybind.h"


using namespace std;

enum removals
{
	vis_recoil = 0,
	vis_smoke,
	flash,
	scope,
	zoom,
	post_processing,
	fog,
	shadow,
};

enum visual_indicators
{
	fake = 0, // 1
	lag_comp, // 2
	fake_duck, // 4
	override_dmg,// 8
	force_baim, // 16
	double_tap, // 32
	manual_antiaim // 64
};

enum chams_type
{
	arms,
	weapon,
	//sleeve,
	history,
	desync,
	shot_record
};

enum weap_type {
	def,
	scar,
	scout,
	_awp,
	rifles,
	pistols,
	heavy_pistols
};

struct ColorA {
	color_t color{ 255, 255, 255, 255 };
	bool rainbow = false;
	float rainbowSpeed = 0.6f;

	auto operator==(const ColorA& c) const
	{
		return color == c.color
			&& rainbow == c.rainbow
			&& rainbowSpeed == c.rainbowSpeed;
	}
};

struct CWeaponConfig {
	bool enable;
	int mindamage;
	int mindamage_override;
	int hitchance;
	int doubletap_hc;
	bool multipoint;
	bool prefer_safepoint;
	//bool quickstop;
	int autostopMode;
	int quickstop_options;
	int max_misses;
	bool static_scale;
	int scale_head;
	int scale_body;
	bool hitboxes[10];
};

struct CLegitbotWeapon {
	bool enabled;
	float fov;
	float smooth = 1.f;
	float rcsx;
	float rcsy;
	bool silent;
	bool scopedonly;
	bool ignoreflash;
	bool ignoresmoke;
	bool hitboxes[5];
	bool aimbacktrack;
	bool aimlock;
};

struct CGlobalVariables
{
	struct {
		bool backtrack;
		float btlength;
		bool aimbot;
		bool onkey;
		pKeybind key;
		std::array<CLegitbotWeapon, 6> weapons; // General, Pistol, Rifle, Awp, Scout, Smg
		bool teammates;
		int active_index;
		bool autopistol;
		bool rcsstandalone;

		struct {
			bool enabled;
			bool onkey;
			pKeybind trigkey;
			float delay;
			int mindmg;
			std::array<bool, 5> hitboxes;
			bool teammates;
			bool checksmoke;
			bool checkflash;
			bool scopedonly;
			bool shootatbacktrack;
		} triggerbot;

	} legitbot;

	struct
	{
		bool enable;
		bool onkey;
		pKeybind key;
		bool silent;
		int fov;
		bool autoshoot;
		bool autoscope;
		bool resolver;
		bool rmode1;
		bool rmode2;
		bool rmode3;
		bool rmode4;
		bool rmode5;
		//bool shotrecord;
		bool zeusbot;
		int zeuschance;
		//color_t shot_clr;
		pKeybind override_dmg;
		pKeybind force_safepoint;
		pKeybind baim;
		int baim_type;
		pKeybind double_tap;
		pKeybind hideShots;
		//int double_tap_stop;
		bool dt_teleport;
		bool antiexploit;
		bool hideshot;
		// scar, scout, awp, rifles, pistols, heavy pistols
		CWeaponConfig weapon[7];
		int dt_charge_delay;

		int active_index;
		bool extended_backtrack;
		
	} ragebot;
	struct
	{
		bool enable;
		bool shotDesync;
		int pitch;
		bool attarget;
		bool attarget_off_when_offsreen;
		bool static_delta;
		int desync_amount;
		bool avoid_overlap;
		pKeybind inverter;
		bool aa_on_use;

		bool manual_antiaim;
		bool ignore_attarget;
		pKeybind manual_left;
		pKeybind manual_right;
		pKeybind manual_back;
		pKeybind manual_forward;

		int fakelag;
		bool fakelag_when_standing;
		bool fakelag_when_exploits;
		int fakelagfactor;
		int fakelagvariance;
		int min, max;

		pKeybind fakeduck;
		pKeybind slowwalk;
		int slowwalkspeed;
		int jitter_angle;
		int zlean;
		bool zleanenable;
		bool extendlby;
		pKeybind fakehead;
		int fakehead_mode;
		int base_yaw;
		bool simtimeStopper;
	} antiaim;
	struct
	{
		bool enable;
		bool teammates;
		bool dormant;
		bool box;
		color_t box_color;
		color_t box_color_occluded;
		bool healthbar;
		bool name;
		color_t name_color;
		color_t name_color_occluded;
		unsigned int flags;
		color_t flags_color;
		color_t flags_color_occluded;
		bool weapon;
		color_t weapon_color;
		color_t weapon_color_occluded;
		bool weaponicontype[2];

		bool dlights;
		color_t dlights_color;
		color_t dlights_color_occluded;

		bool skeleton;
		color_t skeleton_color;
		color_t skeleton_color_occluded;

		bool ammo;
		color_t ammo_color;
		color_t ammo_color_occluded;

		bool zeus_warning;

		bool hitmarker;
		bool visualize_damage;
		color_t hitmarker_color;
		bool hitmarker_sound;
		int hitmarker_sound_type;
		char hitmarker_name[32];

		bool remove[8];
		int scope_thickness;
		bool radarhack;
		bool glow;
		color_t glow_color;
		color_t glow_color_occluded;
		int glowtype;

		bool DURKA;
		bool preverse_killfeed;

		bool local_glow;
		color_t local_glow_clr;

		bool antiaim_arrows;
		color_t antiaim_arrows_color;
		bool indicators;
		bool indicators_rage;
		bool hitlog;
		bool innacuracyoverlay;
		int innacuracyoverlaytype;
		color_t innacuracyoverlay_color;
		int innacuracyoverlay_rotatingspeed;
		int innacuracyoverlay_alpha;
		bool bullet_tracer;
		color_t bullet_tracer_color;
		bool bullet_impact;
		bool bullet_tracer_local;
		color_t bullet_tracer_local_color;
		color_t bullet_impact_color;
		float impacts_size;
		int bullet_tracer_type;
		int aspect_ratio;
		bool shot_multipoint;
		//bool removesmoke, removeflash, removescope, removezoom;
		bool snipercrosshair;
		bool clantagspammer;
		int clantagtype;
		char clantagcustomname[32];
		bool clantagrotating;
		float clantagspeed;
		int transparentprops;
		//bool removevisualrecoil;
		bool chams;
		bool chamsteam;
		bool chamsxqz;
		bool chams_ragdoll;
		int overlay;
		int overlay_xqz;
		color_t chamscolor;
		color_t chamscolor_xqz;
		color_t chamscolor_glow;
		color_t chamscolor_glow_xqz;
		//bool backtrackchams;
		int chamstype;
		bool aaindicator, aaindicator2;
		bool autowallcrosshair;
		bool lbyindicator;
		bool showgrenades;
		bool showdroppedweapons;
		bool showbomb;

		bool out_of_fov;
		bool out_of_fov_drawvisible;
		color_t out_of_fov_color;
		color_t out_of_fov_visiblecolor;
		int out_of_fov_distance;
		int out_of_fov_size;
		int chams_brightness;
		int local_chams_brightness;
		bool interpolated_dsy;
		bool interpolated_bt;
		struct MiscChams_t
		{
			bool enable;
			int material;
			color_t clr;
			color_t glow_clr;

			int chams_brightness;

			// metallic
			int phong_exponent;
			int phong_boost;
			int rim;
			color_t metallic_clr;
			color_t metallic_clr2;

			// meme
			int pearlescent;
			int overlay = 1;
		};
		int active_chams_index;
		MiscChams_t misc_chams[5];

		// local player esp
		bool localchams;
		bool localpulsatingeffect;
		int localchamstype;
		color_t local_glow_color;
		color_t localchams_color;
		color_t localchams_color_glow;
		bool nightmode;
		bool customize_color;
		int nightmode_amount;
		//bool removepostprocessing;
		int nightmode_val = 100;
		color_t nightmode_color;
		color_t nightmode_prop_color;
		color_t nightmode_skybox_color;
		int thirdperson_dist = 120;
		bool chamsragdoll;
		int phong_exponent;
		int phong_boost;
		int rim;
		int pearlescent;
		color_t metallic_clr;
		color_t metallic_clr2;
		color_t glow_col;
		color_t glow_col_xqz;

		int points_size;

		struct
		{
			int phong_exponent;
			int phong_boost;
			int rim;
			color_t metallic_clr;
			color_t metallic_clr2;
			int pearlescent;
			int overlay;
		} local_chams;

		bool override_hp;
		color_t hp_color;
		color_t hp_color_occluded;

		int prop, wall;

		bool show_points;
		color_t points_color;
		int taser_range;
		color_t taser_range_color;
		bool nadepred;
		color_t nadepred_color = color_t(255, 255, 255, 255);
		bool eventlog;
		color_t eventlog_color;
		bool print_votes;
		int print_votes_chat;
		bool speclist;
		float speclistx;
		float speclisty;
		float keybindx;
		float keybindy;
		bool watermark;
		bool monitor;
		bool force_crosshair;
		bool recoil_crosshair;

		bool kill_effect;

		int anim_id;

		bool blend_on_scope;
		int blend_value;
		bool teamdmglist;
		float teamdmgx;
		float teamdmgy;
		bool cooldownsay;
		char cooldownsaytext[32];
		bool addcounter;
		bool ragdoll_force;
		bool spotifysong;
		int playermodelct;
		int playermodelt;

		bool sunset_enabled;
		float rot_x; // sun rotation
		float rot_y; // sun rotation
		int rot_z; // sun rotation

		bool editparticles;
		float smoke_alpha;
		color_t molotov_particle_color;
		color_t blood_particle_color;


		struct {
			bool enabled;
			bool fwd_enabled;
			float falling_min;
			float falling_max;
			float falling_intensity;
			float rotation_intensity;
			float strength;
		}motionblur;

		struct {
			bool enabled;
			float start;
			float end;
			float density;
			color_t color;
		}fog;

		struct {
			struct {
				bool enable;
				bool trajectories;
				bool indtype[2];
				bool filter[3]; // team, enemy, local
				color_t colors[3];
			} projectiles;
			struct {
				bool enabled;
				bool indtype[2];
				bool planted_bomb;
				float planted_bombheight;
				color_t color;
			} weapons;
		} world;
	} visuals;

	struct
	{
		bool bunnyhop;
		bool autostrafe;
		bool autostrafeonkey;
		pKeybind autostrafekey;
		pKeybind thirdperson;
		pKeybind peek_assist;
		bool knifebot;
		int restrict_type;
		bool antiuntrusted = true;
		bool removelocalplayermomentum = false;
		bool enableviewmodel;
		int viewmodelfov = 68;
		float viewmodel_x, viewmodel_y, viewmodel_z, viewmodel_roll;
		int worldfov = 90;
		pKeybind blockbot;
		bool sv_purebypass;
		bool unlockinventoryaccess;
		bool revealranks;
		bool autoaccept;
		float autoacceptdelay;
		bool killsay;

		bool ublockOrigin;
		
		struct {
			bool enable;
			int main;
			int pistol;
			bool misc[7];
		}autobuy;

		struct {
			bool enable;
			bool chickenbox;
			bool owner;
			bool sidelist;
			bool revealkiller;
			bool saykiller;
			bool fish;
			bool fishbox;
			color_t fishcolor;
		}chicken;

	} misc;

	struct
	{
		bool jumpbug;
		pKeybind jumpbugkey;
		int jumpbugtype;
		float lengthcheck;
		int autostrafetype;
		int dxtostrafe;
		bool msl;
		float mslx;

		struct {
			bool enabled;
			pKeybind key;
			float lock;
			int locktype;
			int ticks;
			bool marker;
			color_t marker_color;
			int marker_thickness;
			bool detect_strafe;
		}edgebug;

		bool edgejump;
		pKeybind edgejumpkey;
		
		bool ladderej;
		bool minijump;
		pKeybind minijumpkey;
		
		bool longjump;
		pKeybind longjumpkey;
		
		bool showvelocity;
		bool staticvelcolor;
		color_t velcolor;
		bool velocityfade;
		float velfadevelocity;
		bool showtakeoff;
		float showvelpos;
		int showveltype;
		bool ljstats;
		bool fastduck;
		bool slidewalk;
		bool autoduck;
		float autoduckbefore;
		float autoduckafter;
		bool ladderglide;
		pKeybind ladderglidekey;
		bool autopixelsurf;
		pKeybind autopixelsurfkey;
		int autopixelsurf_ticks;
		pKeybind autopixelsurf_align;
		bool autopixelsurf_freestand;
		bool autopixelsurf_alignind;
		color_t autopixelsurf_indcol1;
		color_t autopixelsurf_indcol2;

		bool showstamina;
		color_t staminacol;
		bool staminatakeoff;
		bool staminafade;
		float staminafadevel;
		float staminascrpos;
		int showstaminatype;

		struct {
			bool show_active;
			color_t passive_col;
			color_t active_col;
			float height;
			bool on[8];
			enum {
				EB = 0,
				EJ,
				JB,
				LG,
				LJ,
				MJ,
				PX,
				AL
			}inds;
		}indicators;

		struct {
			bool mastertoggle;
			bool enabled[4];
			//bool effect;
			//bool chat;
			//bool counter;
			//bool sound;
			int soundtype;
			char sound_name[32];
		}ebdetect;

		struct {
			bool enabled;
			bool types[2];

			float xscreenpos;
			color_t color;
			color_t staminacol;
			color_t staminatextcol;
			color_t textcolor;
			int linewidth;
			int thickness;
			float height;
			int size;
			float speed;
			int fadetype;
			float speedtofade;
			float staminatofade;
			bool gaincolors;
			bool staminagaincolors;
		}graph;

		struct {
			bool enabled;
			bool infowindow;
			std::array<bool, 5> infowindowselected;
			float smoothing;
			int maxrender;
			pKeybind startrecord;
			pKeybind stoprecord;
			pKeybind startplayback;
			pKeybind stopplayback;
			pKeybind clearrecord;
			float infowindowx;
			float infowindowy;
			bool lockva;
			color_t startring;
			color_t endring;
			color_t linecolor;
			color_t textcolor;
			float ringsize;

		}recorder;
		
		struct {
			bool enabled;
			pKeybind teleportkey;
			pKeybind checkpointkey;
			pKeybind nextkey;
			pKeybind prevkey;
			pKeybind undokey;
			color_t color;
			float teleportx, teleporty;
		}checkpoint;

	} movement;


	struct Chams {
		struct Material : ColorA {
			bool enabled = false;
			bool healthBased = false;
			bool blinking = false;
			bool wireframe = false;
			int material = 0;
		};
		std::vector<Material> materials{ {Material()}, {Material()} };
	};

	std::array<Chams, 21> chams;


	struct
	{
		vector<std::string> loaded_luas;
		int menu_key;
		std::string active_config_name;
		int active_config_index;
		color_t color;
		bool open;
		int y;
		bool guiinited;
		int current_tab;
		bool inLoad = false;
		bool inLoad2 = false;
		bool inLoad3 = false;
		bool inLoadteamdmg = false;
		bool inLoadTeleport = false;
		float cspeclistx;
		float cspeclisty;
		float ckeybindx;
		float ckeybindy;
		float crecorderx;
		float crecordery;
		float cteamdmgx;
		float cteamdmgy;
		float cteleportx, cteleporty;
		int window_opacity;
	} menu;
};

extern CGlobalVariables vars;

typedef Json::Value json_t;

class CConfig
{
private:

	string GetModuleFilePath(HMODULE hModule);
	string GetModuleBaseDir(HMODULE hModule);

public:
	vector<std::string> ConfigList;
	void ReadConfigs(LPCTSTR lpszFileName);
	void ResetToDefault();
	bool init = false;
	CConfig()
	{
		Setup();
	}
	void SaveColor(color_t color, const char* name, json_t* json) {
		auto& j = *json;
		j[name][crypt_str("red")] = color.get_red();
		j[name][crypt_str("green")] = color.get_green();
		j[name][crypt_str("blue")] = color.get_blue();
		j[name][crypt_str("alpha")] = color.get_alpha();
	}
	void SaveBind(pKeybind* bind, const char* name, json_t* json) {
		auto& j = *json;
		j[name][crypt_str("key")] = bind->key;
		j[name][crypt_str("type")] = bind->type;
		j[name][crypt_str("active")] = bind->state;
	}

	void LoadStr(std::string* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asString();
	}

	void LoadBool(bool* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asBool();
	}
	void LoadInt(int* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asInt();
	}
	void LoadUInt(unsigned int* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asUInt();
	}
	void LoadFloat(float* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asFloat();
	}
	void LoadColor(color_t* pointer, const char* name, json_t json) {
		if (!json.isMember(name))
			return;
		const auto& location = json[name];
		if (location.isMember(crypt_str("red")) && location.isMember(crypt_str("green"))
			&& location.isMember(crypt_str("blue")) && location.isMember(crypt_str("alpha"))) {
			pointer->set_red(location[crypt_str("red")].asInt());
			pointer->set_green(location[crypt_str("green")].asInt());
			pointer->set_blue(location[crypt_str("blue")].asInt());
			pointer->set_alpha(location[crypt_str("alpha")].asInt());
		}
	}
	void LoadBind(pKeybind* pointer, const char* name, json_t json) {
		if (!json.isMember(name))
			return;
		const auto& location = json[name];
		if (location.isMember(crypt_str("key")) && location.isMember(crypt_str("type"))) {
			pointer->key = location[crypt_str("key")].asUInt();
			pointer->type = location[crypt_str("type")].asUInt();
			pointer->state = location[crypt_str("active")].asBool();
		}
	}
	void Setup();

	void Save(string cfg_name);
	void Load(string cfg_name);
};

extern CConfig Config;
