#pragma once
#include "Hooks.h"
#include "Utils.h"
#include "Hitmarker.h"
#include "AutoPeek.h"
#include "hitsounds.h"
#include "Dormant.h"
#include "config_.hpp"
#pragma comment(lib,"Winmm.lib")

extern std::vector<IGameEventListener2*> g_pGameEventManager;
class CGameEventListener : public IGameEventListener2
{
private:
	std::string eventName;
	Fucntion Call;
	bool server;
public:
	CGameEventListener() : eventName(crypt_str("")), Call(nullptr), server(false)
	{
	}

	CGameEventListener(const std::string& gameEventName, const Fucntion Func, bool Server) : eventName(gameEventName), Call(Func), server(Server)
	{
		this->RegListener();
	}

	virtual void FireGameEvent(IGameEvent* event)
	{
		this->Call(event);
	}

	virtual int IndicateEventHandling(void)
	{
		return 0x2A;
	}

	int GetEventDebugID() override
	{
		return 0x2A;
	}

	void RegListener()
	{
		static bool first = true;
		if (!first)
			return;
		if (interfaces.event_manager->AddListener(this, this->eventName.c_str(), server))
			g_pGameEventManager.push_back(this);
	}
};

#define REG_EVENT_LISTENER(p, e, n, s) p = new CGameEventListener(n, e, s)


player_info_t GetInfo(int Index)
{
	player_info_t Info;
	interfaces.engine->GetPlayerInfo(Index, &Info);
	return Info;
}

std::string HitgroupToName(int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_GENERIC: return hs::h_generic.s();
	case HITGROUP_HEAD: return hs::head.s();
	case HITGROUP_CHEST: return hs::chest.s();
	case HITGROUP_STOMACH: return hs::stomach.s();
	case HITGROUP_LEFTARM: return hs::left_arm.s();
	case HITGROUP_RIGHTARM: return hs::right_arm.s();
	case HITGROUP_LEFTLEG: return hs::left_leg.s();
	case HITGROUP_RIGHTLEG: return hs::right_leg.s();
	case HITGROUP_NECK: return crypt_str("neck");
	case HITGROUP_GEAR: return hs::gear.s();
	default: return crypt_str("hitgroup_") + std::to_string(hitgroup);
	}
}

void LogHits(IGameEvent* Event)
{
	if (!vars.visuals.eventlog)
		return;

	int attackerid = Event->GetInt(crypt_str("attacker"));
	int entityid = interfaces.engine->GetPlayerForUserID(attackerid);
	if (entityid == interfaces.engine->GetLocalPlayer())
	{
		int nUserID = Event->GetInt(crypt_str("attacker"));
		int nDead = Event->GetInt(crypt_str("userid"));

		if (nUserID || nDead)
		{
			player_info_t killed_info = GetInfo(interfaces.engine->GetPlayerForUserID(nDead));
			player_info_t killer_info = GetInfo(interfaces.engine->GetPlayerForUserID(nUserID));
			string Message;
			Message += crypt_str("Hit ");
			Message += killed_info.name;
			int hitgroup = Event->GetInt(crypt_str("hitgroup"));
			if (hitgroup != HITGROUP_GENERIC && hitgroup != HITGROUP_GEAR) {
				Message += crypt_str(" at ");
				Message += HitgroupToName(hitgroup);
			}
			Message += crypt_str(" for ");
			Message += Event->GetString(crypt_str("dmg_health"));
			Message += crypt_str(" hp (");
			Message += Event->GetString(crypt_str("health"));
			Message += crypt_str("hp left) ");

			if (!shot_snapshots.empty())
			{
				auto snapshot = shot_snapshots.front();
				if (snapshot.entity == interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(nDead)))
				{
					Message += snapshot.get_info();
					const auto angle = Math::CalculateAngle(snapshot.start, snapshot.impact);
					Vector forward;
					Math::AngleVectors(angle, forward);
					const auto end = snapshot.impact + forward * 2000.f;
					bool hitable_in_head = CanHitHitbox(snapshot.start, end, &snapshot.record, 0, snapshot.record.bones);

					if (snapshot.hitbox == 0
						&& hitgroup != HITGROUP_HEAD
						&& hitable_in_head) {
						csgo->imaginary_misses[snapshot.entity->GetIndex()]++;

						Message += crypt_str(" [Hitbox mismatch]");

					}
					//g_Hitmarker->Add(snapshot.intended_position, hitgroup == HITGROUP_HEAD, Event->GetString(crypt_str("dmg_health")));
					shot_snapshots.erase(shot_snapshots.begin());
				}
				auto& new_hitlog = csgo->custom_hitlog.emplace_back();
				new_hitlog.name = snapshot.entity->GetName();
				new_hitlog.backtrack = snapshot.backtrack;
				new_hitlog.hitbox = snapshot.hitbox;
				new_hitlog.resolver = snapshot.resolver;
				new_hitlog.spread = false;
			}
			Msg(Message, vars.visuals.eventlog_color);
		}
	}
}



IBasePlayer* GetPlayer(int ID)
{
	int index = interfaces.engine->GetPlayerForUserID(ID);
	return interfaces.ent_list->GetClientEntity(index);
}

void ClearMissedShots(IGameEvent* game_event)
{
	memset(csgo->actual_misses, 0, sizeof(csgo->actual_misses));
	memset(csgo->imaginary_misses, 0, sizeof(csgo->actual_misses));
}

void player_hurt(IGameEvent* game_event)
{
	if (csgo->script_loaded)
		for (auto current : CLua::Get().hooks.get_hooks(crypt_str("on_event")))
			current.func(game_event);

	LogHits(game_event);
	features->Visuals->TeamDamage_CollectInfo(game_event);
	int attackerid = game_event->GetInt(crypt_str("attacker"));
	int entityid = interfaces.engine->GetPlayerForUserID(attackerid);
	if (entityid == interfaces.engine->GetLocalPlayer())
	{
		g_Hitmarker->PlayerHurt(game_event);
		if (vars.visuals.hitmarker_sound) {
			switch (vars.visuals.hitmarker_sound_type) {
			case 0:
				interfaces.engine->ClientCmd_Unrestricted(crypt_str("play buttons/arena_switch_press_02.wav"), 0);
				break;
			case 1:
				interfaces.engine->ClientCmd_Unrestricted(crypt_str("play resource/warning.wav"), 0);
				break;
			case 2:
				PlaySoundA(vineboom, NULL, SND_ASYNC | SND_MEMORY);
				break;
			case 3:
				PlaySoundA(cod_sound, NULL, SND_ASYNC | SND_MEMORY);
				break;
			case 4:
				PlaySoundA(tjorbenmoan, NULL, SND_ASYNC | SND_MEMORY);
				break;
			case 5:
				std::wstring pp = csgo->hitsound_dir + nnx::encoding::utf8to16(vars.visuals.hitmarker_name);
				PlaySoundW(pp.c_str() , NULL, SND_ASYNC | SND_FILENAME);
				break;
			}
		}
	}

	if (!csgo->local || csgo->local->GetHealth() <= 0 || !game_event)
		return;

	if (shot_snapshots.empty())
		return;

	auto& snapshot = shot_snapshots.front();

	if (interfaces.engine->GetPlayerForUserID(game_event->GetInt(crypt_str("attacker"))) != interfaces.engine->GetLocalPlayer())
		return;

	

	if (!snapshot.bullet_impact)
	{
		snapshot.bullet_impact = true;
		snapshot.first_processed_time = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
	}
	snapshot.damage = game_event->GetInt(crypt_str("dmg_health"));
	snapshot.hitgroup_hit = game_event->GetInt(crypt_str("hitgroup"));
}

void bullet_impact(IGameEvent* game_event)
{
	if (csgo->script_loaded)
		for (auto current : CLua::Get().hooks.get_hooks(crypt_str("on_event")))
			current.func(game_event);

	features->BulletTracer->Log(game_event);

	if (!csgo->local || csgo->local->GetHealth() <= 0 || !game_event)
		return;

	g_Hitmarker->BulletImpact(game_event);

	if (shot_snapshots.empty())
		return;

	auto& snapshot = shot_snapshots.front();

	if (interfaces.engine->GetPlayerForUserID(game_event->GetInt(crypt_str("userid"))) != interfaces.engine->GetLocalPlayer())
		return;

	

	if (!snapshot.bullet_impact)
	{
		snapshot.bullet_impact = true;
		snapshot.first_processed_time = csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
		snapshot.impact = Vector(game_event->GetFloat(crypt_str("x")), game_event->GetFloat(crypt_str("y")), game_event->GetFloat(crypt_str("z")));
	}
}

void weapon_fire(IGameEvent* game_event)
{
	if (csgo->script_loaded)
		for (auto current : CLua::Get().hooks.get_hooks(crypt_str("on_event")))
			current.func(game_event);

	if (!csgo->local || csgo->local->GetHealth() <= 0 || !game_event)
		return;
	csgo->last_shoot_pos = csgo->local->GetEyePosition();

	if (shot_snapshots.empty())
		return;

	auto& snapshot = shot_snapshots.front();

	if (!snapshot.weapon_fire)
		snapshot.weapon_fire = true;
	csgo->not_damagedshots++;
}

std::string cheatNames[] = {
		crypt_str("nice crack, you enjoy missing?"),
		crypt_str("Get Good. Get Reflect.codes"),
		crypt_str("Did you take bhop's cheat suggestions?"),
		crypt_str("I hope you didnt pay for that"),
		crypt_str("hdf hurensohn"),
		crypt_str("UID issue"),
		crypt_str("sit nn dog"),
		crypt_str("1"),
		crypt_str("i've seen better cheats on MPGH.net"),
		crypt_str("interesting intelligence"),
		crypt_str("Reflect.codes your n1 cheat provider"),
		crypt_str("you're going negative again? shouldve gotten Reflect.codes")
};


void player_death(IGameEvent* game_event)
{
	if (csgo->script_loaded)
		for (auto current : CLua::Get().hooks.get_hooks(crypt_str("on_event")))
			current.func(game_event);

	features->Visuals->TeamDamage_CollectInfo(game_event);
	int attacker = interfaces.engine->GetPlayerForUserID(game_event->GetInt(crypt_str("attacker")));
	int iLocalPlayer = interfaces.engine->GetLocalPlayer();
	if (csgo->local && csgo->local->isAlive() && attacker == iLocalPlayer)
	{
		if(vars.visuals.kill_effect)
			csgo->local->HealthShotBoostExpirationTime() = interfaces.global_vars->curtime + 1.f;

		if (const auto icon_override = g_config.get_icon_override(game_event->GetString(crypt_str("weapon"))))
			game_event->SetString(crypt_str("weapon"), icon_override);

		//killsay
		if (vars.misc.killsay) {
			std::string killMessage = "";
			killMessage = cheatNames[rand() % ARRAYSIZE(cheatNames)];

			std::string cmd = crypt_str("say \"");
			cmd += killMessage;
			cmd += '"';
			interfaces.engine->ClientCmd_Unrestricted(cmd.c_str());
		}
	}

}

void round_end(IGameEvent* game_event) {
	csgo->ForceOffAA = true;
	g_Hitmarker->hitmarkers.clear();
	ClearMissedShots(game_event);
	
}

void DoBuyBot() {
	if (vars.misc.autobuy.enable) {
		std::string buy_str = "";

		switch (vars.misc.autobuy.main)
		{
		case 1:
			buy_str += (crypt_str("buy scar20; "));
			buy_str += (crypt_str("buy g3sg1; "));
			break;
		case 2:
			buy_str += (crypt_str("buy ssg08; "));
			break;
		case 3:
			buy_str += (crypt_str("buy awp; "));
			break;
		case 4:
			buy_str += (crypt_str("buy negev; "));
			break;
		case 5:
			buy_str += (crypt_str("buy m249; "));
			break;
		case 6:
			buy_str += (crypt_str("buy ak47; "));
			buy_str += (crypt_str("buy m4a1; "));
			buy_str += (crypt_str("buy m4a1_silencer; "));
			break;
		case 7:
			buy_str += (crypt_str("buy aug; "));
			buy_str += (crypt_str("buy sg556; "));
			break;
		}

		switch (vars.misc.autobuy.pistol)
		{
		case 1:
			buy_str += (crypt_str("buy elite; "));
			break;
		case 2:
			buy_str += (crypt_str("buy p250; "));
			break;
		case 3:
			buy_str += (crypt_str("buy tec9; "));
			buy_str += (crypt_str("buy fn57; "));
			break;
		case 4:
			buy_str += (crypt_str("buy deagle; "));
			buy_str += (crypt_str("buy revolver; "));
			break;
		}

		if (vars.misc.autobuy.misc[0])
			buy_str += (crypt_str("buy vesthelm; "));
		if (vars.misc.autobuy.misc[1])
			buy_str += (crypt_str("buy vest; "));

		if (vars.misc.autobuy.misc[2])
			buy_str += (crypt_str("buy hegrenade; "));
		if (vars.misc.autobuy.misc[3]) {
			buy_str += (crypt_str("buy molotov; "));
			buy_str += (crypt_str("buy incgrenade; "));
		}

		if (vars.misc.autobuy.misc[4])
			buy_str += (crypt_str("buy smokegrenade; "));
		if (vars.misc.autobuy.misc[5])
			buy_str += (crypt_str("buy taser; "));
		if (vars.misc.autobuy.misc[6])
			buy_str += (crypt_str("buy defuser; "));


		interfaces.engine->ExecuteClientCmd(buy_str.c_str());
	}
}

void round_prestart(IGameEvent* game_event) {
	csgo->ForceOffAA = true;
	g_Hitmarker->hitmarkers.clear();
	features->Visuals->ResetInfo();
	ClearMissedShots(game_event);
	csgo->disable_dt = true;
	csgo->m_nTickbaseShift = 0;
	for (int i = 0; i < 64; i++) {
		features->Visuals->player_info[i].Reset();
		CDormantEsp::Get().m_cSoundPlayers[i].reset();
	}
	CDormantEsp::Get().m_utlvecSoundBuffer.RemoveAll();
	CDormantEsp::Get().m_utlCurSoundList.RemoveAll();
	CAutopeek::Get().Reset();
	MoveRecorder->forcestop();

	csgo->round_start = true;
	csgo->ebamount = 0;

	DoBuyBot();
}

void round_start(IGameEvent* game_event) {
	csgo->packets.clear();
	csgo->ForceOffAA = false;
	g_Hitmarker->hitmarkers.clear();
	ClearMissedShots(game_event);
	vars.visuals.DURKA = false;
	csgo->disable_dt = false;
	CAutopeek::Get().Reset();
	

	csgo->round_start = true;

	for (int i = 0; i < 64; i++) {
		features->Visuals->player_info[i].Reset();
		CDormantEsp::Get().m_cSoundPlayers[i].reset();
	}
	CDormantEsp::Get().m_utlvecSoundBuffer.RemoveAll();
	CDormantEsp::Get().m_utlCurSoundList.RemoveAll();
	DoBuyBot();
}

void other_death(IGameEvent* event)
{
	if (vars.misc.chicken.enable)
	{
		int otherid = event->GetInt(crypt_str("otherid"));
		int attackerid = event->GetInt(crypt_str("attacker"));
		IBasePlayer* killed = interfaces.ent_list->GetClientEntity(otherid);
		if (killed && killed->GetClientClass()->m_ClassID == ClassId->CChicken)
		{
			auto attacker = interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(attackerid));
			if (interfaces.ent_list->GetClientEntityFromHandle(killed->GetLeaderHandle()) == csgo->local)
			{
				if (vars.misc.chicken.revealkiller) {
					if (auto chatelement = (IHudChat*)H::findHudElement(H::hud, crypt_str("CHudChat")))
					{
						std::string name = crypt_str("\x01 Killed by:\x02 ") + attacker->GetName();
						chatelement->ChatPrintf(0, 0, name.c_str());
					}
				}
			}

			if (vars.misc.chicken.saykiller)
			{
				std::string txt = crypt_str("say \"Warning! Player ") + attacker->GetName() + crypt_str(" has killed an innocent chicken! \"");
				interfaces.engine->ClientCmd_Unrestricted(txt.c_str());
			}

		}
	}
}

void cs_win_panel_match(IGameEvent* event)
{
	features->Visuals->TeamDamage_CollectInfo(event);
}

void vote_cast(IGameEvent* event)
{
	if (!vars.visuals.eventlog)
		return;

	int ent_id = event->GetInt(crypt_str("entityid"));
	int option = event->GetInt(crypt_str("vote_option"));

	auto name = interfaces.ent_list->GetClientEntity(ent_id)->GetName();
	
	std::string msg;
	color_t col;

	switch (option)
	{
	case 0:
		msg = name + crypt_str(" voted Yes");
		col = color_t(0, 255, 0, 255);
		break;
	case 1:
		msg = name + crypt_str(" voted No");
		col = color_t(255, 0, 0, 255);
		break;
	default:
		msg = name + crypt_str(" voted for unknown option");
		col = color_t(255, 255, 255, 255);
		break;
	}

	Msg(msg, col);

	if (vars.visuals.print_votes)
	{
		if (vars.visuals.print_votes_chat)
		{
			interfaces.engine->ClientCmd_Unrestricted(std::string(crypt_str("say \"") + msg + '"').c_str());
		}
		else
		{
			interfaces.engine->ClientCmd_Unrestricted(std::string(crypt_str("say_team \"") + msg + '"').c_str());
		}
	}
}

CGameEventListener* _player_hurt;
CGameEventListener* _bullet_impacts;
CGameEventListener* _weapon_fire;
CGameEventListener* _player_death;
CGameEventListener* _round_end;
CGameEventListener* _round_prestart;
CGameEventListener* _round_start;
CGameEventListener* _other_death;
CGameEventListener* _cs_win_panel_match;
CGameEventListener* _vote_cast;


void RegListeners()
{
	REG_EVENT_LISTENER(_player_hurt, &player_hurt, crypt_str("player_hurt"), false);
	REG_EVENT_LISTENER(_bullet_impacts, &bullet_impact, crypt_str("bullet_impact"), false);
	REG_EVENT_LISTENER(_weapon_fire, &weapon_fire, crypt_str("weapon_fire"), false);
	REG_EVENT_LISTENER(_player_death, &player_death, crypt_str("player_death"), false);
	REG_EVENT_LISTENER(_round_end, &round_end, crypt_str("round_end"), false);
	REG_EVENT_LISTENER(_round_prestart, &round_prestart, crypt_str("round_prestart"), false);
	REG_EVENT_LISTENER(_round_start, &round_start, crypt_str("round_start"), false);
	REG_EVENT_LISTENER(_other_death, &other_death, crypt_str("other_death"), false);
	REG_EVENT_LISTENER(_cs_win_panel_match, &cs_win_panel_match, crypt_str("cs_win_panel_match"), false);
	REG_EVENT_LISTENER(_vote_cast, &vote_cast, crypt_str("vote_cast"), false);
}