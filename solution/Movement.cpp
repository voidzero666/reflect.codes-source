#include "Movement.h"
#include "Misc.h"


void Movement::Run(CUserCmd* cmd) // movement after prediction:3
{
	Movement::MiniJump(cmd);
	Movement::EdgeJump(cmd);
	Movement::LadderGlide(cmd);
	Movement::CheckpointCMOVE(cmd);
}


static int saved_tick_count;
bool in_lj;
bool in_mj;
bool in_ej;
bool do_lj;

void Movement::LongJump(CUserCmd* cmd)
{
	if (!csgo->local)
		return;
	if (!vars.movement.longjumpkey.properstate())
		return;
	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
		return;
	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_OBSERVER || csgo->local->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (csgo->local->GetFlags() & FL_ONGROUND)
	{
		saved_tick_count = interfaces.global_vars->tickcount;
	}

	if (interfaces.global_vars->tickcount - saved_tick_count > 3)
	{
		do_lj = false;
	}
	else
	{
		do_lj = true;
	}

	if (do_lj && !(csgo->local->GetFlags() & FL_ONGROUND))
	{
		cmd->buttons |= IN_DUCK;
		in_lj = true;
	}

}

void Movement::MiniJump(CUserCmd* cmd)
{
	if (!csgo->local)
		return;
	
	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
	{
		in_ej = false;
		in_lj = false;
		in_mj = false;
		return;
	}

	if ((engine_prediction->m_store.m_fFlags & FL_ONGROUND))
	{
		in_ej = false;
		in_mj = false;
		in_lj = false;
	}
		
		
	if (!vars.movement.minijumpkey.properstate())
		return;
	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_OBSERVER || csgo->local->GetMoveType() == MOVETYPE_LADDER)
		return;

	if ((engine_prediction->m_store.m_fFlags & FL_ONGROUND) && !(csgo->local->GetFlags() & FL_ONGROUND))
	{
		cmd->buttons |= IN_JUMP;
		cmd->buttons |= IN_DUCK;
		in_mj = true;
	}

}

int lastejtick;

void Movement::EdgeJump(CUserCmd* cmd)
{
	if (!csgo->local)
		return;
	
	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
	{
		return;
	}
		

	if (!vars.movement.edgejumpkey.properstate())
		return;
	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_OBSERVER)
		return;

	if (engine_prediction->m_store.m_fFlags & FL_ONGROUND && !(csgo->local->GetFlags() & FL_ONGROUND))
	{
		cmd->buttons |= IN_JUMP;
		in_ej = true;
	}

	if (vars.movement.ladderej && csgo->local->GetMoveType() == MOVETYPE_LADDER)
	{
		static int saved_tick;
		
		engine_prediction->start(csgo->local, cmd);
		if (csgo->local->GetMoveType() != MOVETYPE_LADDER)
		{
			saved_tick = interfaces.global_vars->tickcount;
			cmd->buttons |= IN_JUMP;
			in_ej = true;
			cmd->forwardmove = 0.f;
			cmd->sidemove = 0.f;
			cmd->buttons = cmd->buttons & ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);
		}
		engine_prediction->end(csgo->local);

		if (interfaces.global_vars->tickcount - saved_tick > 1 && interfaces.global_vars->tickcount - saved_tick < 15)
		{
			cmd->buttons |= IN_DUCK;
			cmd->forwardmove = 0.f;
			cmd->sidemove = 0.f;
			cmd->buttons = cmd->buttons & ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);
		}
	}
}


int tickstojb = 0;


void Movement::JumpBug(CUserCmd* cmd)
{
	if (!csgo->local)
	{
		tickstojb = 0;
		return;
	}
		

	if (!vars.movement.jumpbugkey.properstate())
	{
		tickstojb = 0;
		return;
	}

	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
	{
		tickstojb = 0;
		return;
	}

	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_OBSERVER || csgo->local->GetMoveType() == MOVETYPE_LADDER)
	{
		tickstojb = 0;
		return;
	}


	if (tickstojb == 0)
	{
		if (csgo->local->GetFlags() & FL_ONGROUND)
		{
			cmd->buttons &= ~IN_JUMP;
		}

		engine_prediction->restoreprediction(0, interfaces.prediction->Split->nCommandsPredicted - 1);

		if (csgo->local->GetFlags() & FL_ONGROUND || csgo->local->GetVelocity().z >= 0.f)
			return;

		

		CUserCmd predcmd = *cmd;

		predcmd.buttons |= IN_DUCK;

		engine_prediction->start(csgo->local, &predcmd);

		if (csgo->local->GetFlags() & FL_ONGROUND || csgo->local->GetVelocity().z >= 0.f)
		{
			engine_prediction->end(csgo->local);
			return;
		}
		engine_prediction->end(csgo->local);

		predcmd.buttons &= ~IN_DUCK;
		predcmd.buttons &= ~IN_JUMP;

		engine_prediction->start(csgo->local, &predcmd);

		const Vector pos = csgo->local->GetOrigin();

		CTraceFilter fil;
		fil.pSkip = csgo->local;
		CGameTrace tr;

		const Vector pos1 = pos - Vector{ 0.f,0.f,2.f };

		const Vector mins = csgo->local->GetMins();
		const Vector maxs = csgo->local->GetMaxs();

		Ray_t ray;
		ray.Init( pos, pos1, mins, maxs );
		

		interfaces.trace->TraceRay(ray, MASK_PLAYERSOLID, &fil, &tr);

		if (tr.fraction < 1.f)
		{
			tickstojb = 2;
		}
		engine_prediction->end(csgo->local);
	}
	else
	{
		if (tickstojb == 2)
		{
			cmd->buttons |= IN_DUCK;
			tickstojb--;
		}

		if (tickstojb == 1)
		{
			cmd->buttons &= ~IN_JUMP;
			tickstojb--;
		}

	}

	//Vector mins = csgo->local->GetMins();
	//Vector maxs = csgo->local->GetMaxs();

	//Vector points[4] = { pos + Vector{ mins.x, mins.y, -2.f },  pos + Vector{ maxs.x, mins.y, -2.f },  pos + Vector{ maxs.x, maxs.y, -2.f }, pos + Vector{ mins.x, maxs.y, -2.f } };

	/*
	if (!(engine_prediction->m_store.m_fFlags & FL_ONGROUND) && (csgo->local->GetFlags() & FL_ONGROUND))
	{
		cmd->buttons |= IN_DUCK;
	}

	if (csgo->local->GetFlags() & FL_ONGROUND)
	{
		cmd->buttons &= ~IN_JUMP;
	}
	*/
}

static std::vector<std::pair<Vector, Vector>> detectionpositions;

bool actualebdetection(Vector& old_velocity, Vector& predicted_velocity, Vector& afterpredicted_velocity)
{
	/*
	if (round(predicted_velocity.z) == 0.f || (csgo->local->GetFlags() & 1))
		return false;

	auto mt = csgo->local->GetMoveType();

	if (old_velocity.z < -6.f && predicted_velocity.z > old_velocity.z && predicted_velocity.z < -6.0 && mt != MOVETYPE_LADDER && mt != MOVETYPE_NOCLIP)
	{
		engine_prediction->start(csgo->local, csgo->cmd);
		double roundedvel = round((predicted_velocity.z * interfaces.global_vars->interval_per_tick) + predicted_velocity.z);
		bool correct = round(csgo->local->GetVelocity().z) == roundedvel;
		engine_prediction->end(csgo->local);
		if (correct)
			return true;
		else
			return false;
	}

	if (round(predicted_velocity.z) == -7.f && !(csgo->local->GetFlags() & 1) && old_velocity.z < -7.f)
	{
		return true;
	}

	if (round(predicted_velocity.z) > round(old_velocity.z) && !(csgo->local->GetFlags() & 1) && old_velocity.z < 0.f && predicted_velocity.z < 0.f)
	{
		return true;
	}

	if (round(predicted_velocity.z) == round(old_velocity.z - predicted_velocity.z * interfaces.global_vars->interval_per_tick) && old_velocity.z < -6.f && predicted_velocity.z < -6.f && predicted_velocity.z > old_velocity.z && !(csgo->local->GetFlags() & 1))
	{
		return true;
	}
	*/


	static auto Sv_gravity = interfaces.cvars->FindVar(crypt_str("sv_gravity"));
	auto sv_gravity = Sv_gravity->GetFloat();
	
	if (old_velocity.z < -6.0f && predicted_velocity.z > old_velocity.z && predicted_velocity.z < -6.0f && old_velocity.Length2D() <= predicted_velocity.Length2D())
	{
		const float gravity_vel_const = roundf(-sv_gravity * interfaces.global_vars->interval_per_tick + predicted_velocity.z);

		if (gravity_vel_const == roundf(afterpredicted_velocity.z))
			return true;
	}
	
	
	if (predicted_velocity.Length2D() <= afterpredicted_velocity.Length2D())
	{
		

		float ebzvel = sv_gravity * 0.5f * interfaces.global_vars->interval_per_tick;

		if (-ebzvel > predicted_velocity.z && round(afterpredicted_velocity.z) == round(-ebzvel))
		{
			return true;
		}
	}
	

	return false;
}

bool profesionalcompare(float a, float b, float inacc)
{
	if (b < a + inacc && b > a - inacc)
		return true;
	else
		return false;
}

static Vector ebpos = { 0,0,0 };

void Movement::EdgeBug(CUserCmd* cmd)
{
	if (!csgo->local)
	{
		csgo->detectdata.detecttick = 0;
		csgo->detectdata.edgebugtick = 0;
		return;
	}	

	if (!vars.movement.edgebug.key.properstate())
	{
		csgo->detectdata.detecttick = 0;
		csgo->detectdata.edgebugtick = 0;
		return;
	}
		
	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
	{
		csgo->detectdata.detecttick = 0;
		csgo->detectdata.edgebugtick = 0;
		return;
	}
		
	if (engine_prediction->m_store.m_fFlags & 1 || csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_OBSERVER || csgo->local->GetMoveType() == MOVETYPE_LADDER)
	{
		csgo->detectdata.detecttick = 0;
		csgo->detectdata.edgebugtick = 0;
		return;
	}

	if (interfaces.global_vars->tickcount >= csgo->detectdata.detecttick && interfaces.global_vars->tickcount <= csgo->detectdata.edgebugtick)
	{

		if (csgo->detectdata.crouched)
			cmd->buttons |= IN_DUCK;
		else
			cmd->buttons &= ~IN_DUCK;


		if (csgo->detectdata.strafing)
		{
			cmd->forwardmove = csgo->detectdata.forwardmove;
			cmd->sidemove = csgo->detectdata.sidemove;
			cmd->viewangles.y = Math::NormalizeYaw(csgo->detectdata.startingyaw + (csgo->detectdata.yawdelta * (interfaces.global_vars->tickcount - csgo->detectdata.detecttick))); // extrapolate the viewangle using a static delta and the amount of ticks that have passed from detection
			//interfaces.engine->SetViewAngles(cmd->viewangles);
		}
		else
		{
			cmd->forwardmove = 0.f;
			cmd->sidemove = 0.f;
		}
		
		return;
	}
		
	Vector originalpos = engine_prediction->m_store.m_vecOrigin;
	Vector originalvel = engine_prediction->m_store.m_vecVelocity;
	int originalflags = engine_prediction->m_store.m_fFlags;
	float originalfmove = cmd->forwardmove;
	float originalsmove = cmd->sidemove;
	Vector originalangles = cmd->viewangles;
	
	float yawdelta = std::clamp(cmd->viewangles.y - csgo->lasttickyaw, -(180.f / vars.movement.edgebug.ticks), 180.f / vars.movement.edgebug.ticks);
	//prediction
	
	if (interfaces.global_vars->tickcount < csgo->detectdata.detecttick || interfaces.global_vars->tickcount > csgo->detectdata.edgebugtick)
	{
		const int desiredrounds = (vars.movement.edgebug.detect_strafe && yawdelta < 0.1f) ? 4 : 2;


		for (int predRound = 0; predRound < desiredrounds; predRound++)
		{

			engine_prediction->restoreprediction(0, interfaces.prediction->Split->nCommandsPredicted - 1);

			//create desired cmd
			CUserCmd predictcmd = *cmd;

			csgo->detectdata.startingyaw = originalangles.y;

			if (predRound == 0)
			{
				csgo->detectdata.crouched = true;
				predictcmd.buttons |= IN_DUCK;
				csgo->detectdata.strafing = false;
				predictcmd.forwardmove = 0.f;
				predictcmd.sidemove = 0.f;
				
			}
			else if(predRound == 1)
			{
				csgo->detectdata.crouched = false;
				predictcmd.buttons &= ~IN_DUCK;
				csgo->detectdata.strafing = false;
				predictcmd.forwardmove = 0.f;
				predictcmd.sidemove = 0.f;
				
			}
			else if (predRound == 2)
			{
				csgo->detectdata.crouched = true;
				predictcmd.buttons |= IN_DUCK;
				csgo->detectdata.strafing = true;
				predictcmd.forwardmove = originalfmove;
				predictcmd.sidemove = originalsmove;
			}
			else if (predRound == 3)
			{
				csgo->detectdata.crouched = false;
				predictcmd.buttons &= ~IN_DUCK;
				csgo->detectdata.strafing = true;
				predictcmd.forwardmove = originalfmove;
				predictcmd.sidemove = originalsmove;
			}


			detectionpositions.clear();
			detectionpositions.push_back(std::pair<Vector,Vector>(csgo->local->GetOrigin(), csgo->local->GetVelocity()));

			

			for (int ticksPredicted = 0; ticksPredicted < vars.movement.edgebug.ticks; ticksPredicted++)
			{
				Vector old_velocity = csgo->local->GetVelocity();
				int old_flags = csgo->local->GetFlags();
				Vector old_pos = csgo->local->GetOrigin();

				if (csgo->detectdata.strafing)
				{
					predictcmd.viewangles.y = Math::NormalizeYaw(originalangles.y + (yawdelta * ticksPredicted));
				}


				engine_prediction->start(csgo->local, &predictcmd); // predict 1 more tick
				Vector predicted_velocity = csgo->local->GetVelocity();
				int predicted_flags = csgo->local->GetFlags();
				detectionpositions.push_back(std::pair<Vector, Vector>(csgo->local->GetOrigin(), csgo->local->GetVelocity()));
				engine_prediction->end(csgo->local);

				if ((old_flags & 1) || (predicted_flags & 1) || round(predicted_velocity.Length2D()) == 0.f || round(old_velocity.Length2D()) == 0.f || csgo->local->GetMoveType() == MOVETYPE_LADDER || old_velocity.z > 0.f)
				{
					csgo->detectdata.detecttick = 0;
					csgo->detectdata.edgebugtick = 0;
					break;
				}

				if (detectionpositions.size() > 2)
				{
					if (actualebdetection(detectionpositions.at(detectionpositions.size() - 3).second, detectionpositions.at(detectionpositions.size() - 2).second, detectionpositions.at(detectionpositions.size() - 1).second))
					{

						csgo->detectdata.detecttick = interfaces.global_vars->tickcount;
						csgo->detectdata.edgebugtick = interfaces.global_vars->tickcount + (ticksPredicted + 1);

						csgo->detectdata.forwardmove = originalfmove;
						csgo->detectdata.sidemove = originalsmove;

						csgo->detectdata.yawdelta = yawdelta;

						if (predRound < 2)
						{
							cmd->forwardmove = 0.f;
							cmd->sidemove = 0.f;
						}
						else
						{
							cmd->forwardmove = originalfmove;
							cmd->sidemove = originalsmove;
						}

						if (predRound == 0 || predRound == 2)
						{
							cmd->buttons |= IN_DUCK;
						}
						else
						{
							cmd->buttons &= ~IN_DUCK;
						}

						csgo->mdxOnEB = cmd->mousedx;
						csgo->ebpos = old_pos;
						csgo->angOnEB = cmd->viewangles;

						return;
					}

				}
			}
		}
		
	}
}

float float_max(float value1, float value2) {
	if (value1 >= value2) {
		return value1;
	}
	return value2;
}
//variables
float Distance = 0;
int pre = 0;
int Max = 0;
int strafes = 0;
int sync = 0;
float height = 0.0;
//variables to calculate the other variables abover
Vector jump_positions[1];
bool last_jumping = false;
float jumpbugz;
int bhops = 0;
bool jumpbug = false;
bool strafe_left;
bool strafe_right;
//calculate height?
bool calcheight;
Vector jumpposheight;
int syncticks;
int durationticks;
int durationticks2;
int lastvelosynced;
double lol = 0.0;
int savedongroundtick;
bool jumpbugbool;
int saved_tick_count_air;
bool bhop;
bool multibhop;

void resetjumpstats() {
	Distance = 0;
	pre = 0;
	Max = 0;
	strafes = 1;
	sync = 0;
	height = 0.0f;
	syncticks = 0;
	durationticks = 0;
	lastvelosynced = 0;
	savedongroundtick = 0;
	durationticks2 = 0;
	calcheight = false; strafe_left = false; strafe_right = false;
	jumpbugz = 0;
	jumpbug = false;
}

void Movement::JumpStatsBhop(CUserCmd* cmd) {

	if (!csgo->is_connected || !interfaces.engine->IsInGame() || !csgo->local->isAlive() || !csgo->local)
		return;

	const int move_type = csgo->local->GetMoveType();
	if (move_type == MOVETYPE_LADDER || move_type == MOVETYPE_NOCLIP || move_type == MOVETYPE_OBSERVER) {
		resetjumpstats();
		return;
	}

	//jb detect
	if (!(csgo->local->GetFlags() & FL_ONGROUND))
	{
		if (csgo->local->GetVelocity().z < jumpbugz)
		{
			jumpbugz = csgo->local->GetVelocity().z;
			jumpbugbool = true;
		}
		if (csgo->local->GetVelocity().z > jumpbugz && jumpbugbool == true)
		{
			strafes = 0;
			strafe_left = false;
			strafe_right = false;
			syncticks = 0;
			durationticks = 0;
			durationticks2 = 0;
			pre = round(csgo->local->GetVelocity().Length2D());
			jump_positions[0] = csgo->local->GetAbsOrigin();
			jumpbug = true;
			jumpbugbool = false;

		}
	}
	else if (!jumpbug)
	{
		jumpbugbool = false;
	}

	//bhop detection
	if ((csgo->local->GetFlags() & FL_ONGROUND) && (cmd->buttons & IN_JUMP)) {
		bhops += 1;
		jumpbugz = 0;
	}

	//bhop
	if (bhops == 1) {
		bhop = true;
	}
	else {
		bhop = false;
	}

	//multibhop
	if (bhops > 1) {
		multibhop = true;
	}
	else {
		multibhop = false;
	}
	//resettings stats
	if (bhops >= 0 && interfaces.global_vars->tickcount - saved_tick_count_air > 1)
		bhops = 0;
}

void Movement::JumpStats(CUserCmd* cmd) {


	if (!csgo->is_connected || !interfaces.engine->IsInGame() || !csgo->local->isAlive() || !csgo->local)
		return;
	
	const int move_type = csgo->local->GetMoveType();

	// Movetype
	if (move_type == MOVETYPE_LADDER || move_type == MOVETYPE_NOCLIP || move_type == MOVETYPE_OBSERVER) {
		resetjumpstats();
		return;
	}

	//initialization
	Vector velocity = csgo->local->GetVelocity();
	bool landed = false;


	//get jumppos after jump, this works instead of jumppositions
	if (cmd->buttons & IN_JUMP && !calcheight)
	{
		jumpposheight = csgo->local->GetOrigin();
		calcheight = true;
	}

	// If we are jumping
	if (!(csgo->local->GetFlags() & FL_ONGROUND)) {

		saved_tick_count_air = interfaces.global_vars->tickcount;

		//strafes
		//check which strafe is first
		if (cmd->mousedx < lol && !strafe_right && !strafe_left)
			strafe_right = true;
		else if (cmd->mousedx > lol && !strafe_right && !strafe_left)
			strafe_left = true;

		//check what direction is the strafe
		if (cmd->mousedx < lol && !strafe_right && strafe_left)
		{
			strafe_right = true;
			strafe_left = false;
			strafes++;
		}
		else if (cmd->mousedx > lol && strafe_right && !strafe_left) {
			strafe_right = false;
			strafe_left = true;
			strafes++;
		}

		//gather data for sync
		if (interfaces.global_vars->tickcount != durationticks2)
		{
			durationticks = interfaces.global_vars->tickcount - savedongroundtick;
			syncticks += round(velocity.Length2D()) > lastvelosynced ? 1 : 0;
			lastvelosynced = round(velocity.Length2D());
			durationticks2 = interfaces.global_vars->tickcount;
		}

		//max
		Max = float_max(Max, round(velocity.Length2D()));

		//height
		height = float_max(height, csgo->local->GetOrigin().z - jumpposheight.z + 4.5999);

		if (!last_jumping)
		{
			// Save our current location
			jump_positions[0] = csgo->local->GetOrigin();
			// Save our velocity before jumping, called pre speed, yea, smol brains won't know this
			pre = round(velocity.Length2D());
		}

		// Update our jumping state
		last_jumping = true;
		return;
	}

	// If isn't jumping
	if (csgo->local->GetFlags() & FL_ONGROUND)
	{
		savedongroundtick = interfaces.global_vars->tickcount;

		// And we were jumping before
		if (last_jumping)
		{
			// Then save our location and change the landing state
			landed = true;
			jump_positions[1] = csgo->local->GetOrigin();
		}
		// Update the jumping state
		last_jumping = false;
	}

	//check if we hit the ground
	if (!landed)
		return;

	//distance
	Distance = (32.0 + (sqrt(pow(jump_positions[1].x - jump_positions[0].x, 2.0) + pow(jump_positions[1].y - jump_positions[0].y, 2.0))));

	//calculate sync
	sync = ((float)syncticks / (float)durationticks * 100.f);

	//optimization
	if (pre > 250 && pre < 253)
		pre = 250;
	if (sync >= 100.f)
		sync = 100.f;

	//print out stats

	if (vars.movement.ljstats)
	{



		if (Distance > 200.f && Distance < 280.f && jump_positions[1].z - jump_positions[0].z <= 64.f && jump_positions[1].z - jump_positions[0].z >= -8.f)
		{
			if (auto hud = (IHudChat*)H::findHudElement(H::hud, crypt_str("CHudChat")))
			{
				if (bhop && !multibhop && !jumpbug) {
					height -= 3.2f;
					Distance += 2.7f;
					if (Distance >= 245.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01|\x01 Bunnyhop\x01: \x0E%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 240.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01|\x01 Bunnyhop\x01: \x10%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 235.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01|\x01 Bunnyhop\x01: \x02%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 233.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01|\x01 Bunnyhop\x01: \x04%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 230.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01|\x01 Bunnyhop\x01: \x0B%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x08|\x08 Bunnyhop\x08: \x08%.2f \x08| \x08%i \x08strafes | \x08%i \x08pre | \x08%i \x08max \x08| \x08%.1f \x08height"), Distance, strafes, pre, Max, height);

				}
				else if (multibhop && !bhop && !jumpbug) {
					height -= 3.5f;
					Distance += 4.2f;
					if (Distance >= 245.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Multibhop\x01: \x0E%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 248.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Multibhop\x01: \x10%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 245.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Multibhop\x01: \x02%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 240.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Multibhop\x01: \x04%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 235.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Multibhop\x01: \x0B%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x08| \x08Multibhop\x08: \x08%.2f \x08| \x08%i \x08strafes | \x08%i \x08pre | \x08%i \x08max \x08| \x08%.1f \x08height"), Distance, strafes, pre, Max, height);

				}
				else if (jumpbug && !bhop && !multibhop) {
					height -= 4.6f;
					Distance += 1.4f;
					if (Distance >= 268.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Jumpbug\x01: \x0E%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 265.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Jumpbug\x01: \x10%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 263.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Jumpbug\x01: \x02%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 260.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Jumpbug\x01: \x04%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 255.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Jumpbug\x01: \x0B%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x08| \x08Jumpbug\x08: \x08%.2f \x08| \x08%i \x08strafes | \x08%i \x08pre | \x08%i \x08max \x08| \x08%.1f \x08height"), Distance, strafes, pre, Max, height);

				}
				else if (!jumpbug && !bhop && !multibhop) {
					height -= 4.6f;
					Distance += 4.6f;
					if (Distance >= 245.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Longjump\x01: \x0E%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 243.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Longjump\x01: \x10%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 240.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Longjump\x01: \x02%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 235.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Longjump\x01: \x04%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else if (Distance >= 230.f)
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x01| \x01Longjump\x01: \x0B%.2f \x01| \x05%i \x01strafes | \x05%i \x01pre | \x05%i \x01max \x01| \x05%.1f \x01height"), Distance, strafes, pre, Max, height);
					else
						hud->ChatPrintf(0, 0, crypt_str(" \x03 [Reflect] \x08| \x08Longjump\x08: \x08%.2f \x08| \x08%i \x08strafes | \x08%i \x08pre | \x08%i \x08max \x08| \x08%.1f \x08height"), Distance, strafes, pre, Max, height);
				}
			}
		}
	}
	resetjumpstats();
}


struct DATAFORDETECT
{
	Vector velocity;
	bool onground;
};

std::deque<DATAFORDETECT> VelocitiesForDetection;

void Movement::EBDetect(CUserCmd* cmd)
{
	if (!csgo->local)
		return;

	if (!vars.movement.ebdetect.mastertoggle)
		return;

	if (!vars.movement.ebdetect.enabled[0] && !vars.movement.ebdetect.enabled[1] && !vars.movement.ebdetect.enabled[2] && !vars.movement.ebdetect.enabled[3])
		return;

	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
		return;

	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_OBSERVER || csgo->local->GetMoveType() == MOVETYPE_LADDER || (csgo->flags & 1))
	{
		VelocitiesForDetection.clear();
		csgo->ebcombo = 0;
		return;
	}

	DATAFORDETECT d;
	d.velocity = csgo->local->GetVelocity();
	d.onground = csgo->local->GetFlags() & FL_ONGROUND;
	if (d.onground)
		csgo->ebcombo = 0;

	VelocitiesForDetection.push_front(d);

	if (VelocitiesForDetection.size() > 2)
		VelocitiesForDetection.pop_back();

	static auto sv_Gravity = interfaces.cvars->FindVar(crypt_str("sv_gravity"));
	float negativezvel = sv_Gravity->GetFloat() * -0.5f * interfaces.global_vars->interval_per_tick;

	if (VelocitiesForDetection.size() == 2 && ((round(negativezvel * 100.f) == round(VelocitiesForDetection.at(0).velocity.z * 100.f) && VelocitiesForDetection.at(1).velocity.z < negativezvel && !VelocitiesForDetection.at(1).onground && !VelocitiesForDetection.at(0).onground) || csgo->detectdata.edgebugtick == interfaces.global_vars->tickcount))
	{
		VelocitiesForDetection.clear();
		csgo->ebamount++;
		csgo->ebcombo++;
		csgo->lastEBDETECT = interfaces.global_vars->realtime;
		if (vars.movement.ebdetect.enabled[1])
		{
			if (auto chatelement = (IHudChat*)H::findHudElement(H::hud, crypt_str("CHudChat")))
			{

				switch (csgo->ebcombo)
				{
				case 1:
					chatelement->ChatPrintf(0, 0, crypt_str("\x01 \x03[Reflect]\x01 | Edgebug [%d]"), csgo->ebamount);
					break;
				case 2:
					chatelement->ChatPrintf(0, 0, crypt_str("\x01 \x03[Reflect]\x01 | Edgebug [%d] |\x0B 2 Combo!"), csgo->ebamount);
					break;
				case 3:
					chatelement->ChatPrintf(0, 0, crypt_str("\x01 \x03[Reflect]\x01 | Edgebug [%d] |\x03 3 Combo!"), csgo->ebamount);
					break;
				case 4:
					chatelement->ChatPrintf(0, 0, crypt_str("\x01 \x03[Reflect]\x01 | Edgebug [%d] |\x06 4 Combo!"), csgo->ebamount);
					break;
				case 5:
					chatelement->ChatPrintf(0, 0, crypt_str("\x01 \x03[Reflect]\x01 | Edgebug [%d] |\x09 5 Combo!"), csgo->ebamount);
					break;
				default:
					chatelement->ChatPrintf(0, 0, crypt_str("\x01 \x03[Reflect]\x01 | Edgebug [%d] |\x10 %d Combo!"), csgo->ebamount, csgo->ebcombo);
					break;
				}
			}
			/*
			std::string comand = "echo pre eb: " + to_string(zvels.at(2));
			interfaces.engine->ClientCmd_Unrestricted(comand.c_str());
			std::string command = "echo at eb: " + to_string(zvels.at(1));
			interfaces.engine->ClientCmd_Unrestricted(command.c_str());
			std::string commande = "echo after eb: " + to_string(zvels.at(0));
			interfaces.engine->ClientCmd_Unrestricted(commande.c_str());
			std::string commandee = "echo 2t after eb: " + to_string(csgo->local->GetVelocity().z);
			interfaces.engine->ClientCmd_Unrestricted(commandee.c_str());
			*/
		}

		if (vars.movement.ebdetect.enabled[0])
		{
			csgo->local->HealthShotBoostExpirationTime() = interfaces.global_vars->curtime + 1.f;
		}

		if (vars.movement.ebdetect.enabled[3])
		{
			switch (vars.movement.ebdetect.soundtype) {
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
				std::wstring pp = csgo->hitsound_dir + nnx::encoding::utf8to16(vars.movement.ebdetect.sound_name);
				PlaySoundW(pp.c_str(), NULL, SND_ASYNC | SND_FILENAME);
				break;
			}
		}
	}

}

void Movement::FastDuck(CUserCmd* cmd)
{
	if (vars.movement.fastduck && vars.misc.restrict_type != 2 && csgo->local->isAlive())
	{
		cmd->buttons |= IN_BULLRUSH;
	}
}

void Movement::Slidewalk(CUserCmd* cmd)
{
	if (vars.movement.slidewalk && csgo->is_local_alive && csgo->local->GetMoveType() != MOVETYPE_LADDER)
	{
		cmd->buttons ^= IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;
	}
}


void Movement::AutoDuck(CUserCmd* cmd)
{
	if (!vars.movement.autoduck || vars.movement.autoduckafter == 0.f || vars.movement.autoduckbefore == 0.f)
		return;

	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
		return;

	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_OBSERVER || csgo->local->GetMoveType() == MOVETYPE_LADDER || engine_prediction->m_store.m_fFlags & 1)
		return;

	if (engine_prediction->m_store.m_fFlags & FL_ONGROUND)
		return;

	float timepredicted = 0.f;
	float savedzpos = -999999.f;
	bool foundground = false;
	CUserCmd savedcmd = *cmd;

	//finding ground on crouch
	engine_prediction->restoreprediction(0, interfaces.prediction->Split->nCommandsPredicted - 1);
	//engine_prediction->restoreprediction();
	while (timepredicted < vars.movement.autoduckbefore && !(csgo->local->GetFlags() & FL_ONGROUND))
	{
		cmd->buttons |= IN_DUCK;
		engine_prediction->start(csgo->local, cmd);
		if (csgo->local->GetFlags() & FL_ONGROUND)
		{
			foundground = true;
			savedzpos = csgo->local->GetOrigin().z;
			engine_prediction->end(csgo->local);
			break;
		}
		else
		{
			engine_prediction->end(csgo->local);
			timepredicted += interfaces.global_vars->interval_per_tick;
		}
	}

	
	bool foundgstanding = false;
	float savedzposstanding = -999999.f;

	if (foundground && savedzpos != -999999.f)
	{
		*cmd = savedcmd;
		engine_prediction->restoreprediction(0, interfaces.prediction->Split->nCommandsPredicted - 1);
		//engine_prediction->restoreprediction();
		
		timepredicted = 0.f;
		
		while (timepredicted < vars.movement.autoduckbefore && !(csgo->local->GetFlags() & FL_ONGROUND))
		{
			cmd->buttons &= ~IN_DUCK;
			engine_prediction->start(csgo->local, cmd);
			if (csgo->local->GetFlags() & FL_ONGROUND)
			{
				foundgstanding = true;
				savedzposstanding = csgo->local->GetOrigin().z;
				engine_prediction->end(csgo->local);
				break;
			}
			else
			{
				engine_prediction->end(csgo->local);
				timepredicted += interfaces.global_vars->interval_per_tick;
			}
		}
	}

	if (foundground && foundgstanding)
	{
		if (savedzpos > savedzposstanding)
		{
			cmd->buttons |= IN_DUCK;
		}
	}
	else if (foundground && !foundgstanding)
	{
		cmd->buttons |= IN_DUCK;
	}
	else
	{
		*cmd = savedcmd;
	}


}

void Movement::LadderGlide(CUserCmd* cmd)
{
	if (!vars.movement.ladderglidekey.properstate())
	{
		csgo->laddergliding = false;
		return;
	}
		

	if (csgo->local->GetMoveType() & MOVETYPE_LADDER)
	{
		cmd->buttons |= IN_JUMP;
		csgo->laddergliding = true;
	}
	else
	{
		csgo->laddergliding = false;
	}
}

std::deque<std::pair<Vector, Vector>> checkpoints;

int currentcheckpoint{ 0 };
float pressdelay{ 0.f };

void Movement::CheckpointCMOVE(CUserCmd* cmd)
{
	if (!csgo->local || !interfaces.engine->IsConnected() || !interfaces.engine->IsInGame() || !vars.movement.checkpoint.enabled)
	{
		checkpoints.clear();
		currentcheckpoint = 0;
		pressdelay = 0;
		return;
	}
		
	if (csgo->local->isAlive() && pressdelay < interfaces.global_vars->realtime)
	{
		
		if (vars.movement.checkpoint.checkpointkey.properstate())
		{
			checkpoints.push_front({ Vector(cmd->viewangles.x, cmd->viewangles.y, 0.f), csgo->origin });
			currentcheckpoint = 0;
			if (checkpoints.size() > 99)
				checkpoints.pop_back();
			pressdelay = interfaces.global_vars->realtime + 0.2;
		}

		if (!checkpoints.empty())
		{
			if (vars.movement.checkpoint.teleportkey.properstate())
			{

				auto& cur = checkpoints.at(currentcheckpoint);
				std::string cmd = crypt_str("setpos ") + std::to_string(cur.second.x) + crypt_str(" ") + std::to_string(cur.second.y) + crypt_str(" ") + std::to_string(cur.second.z) + crypt_str(";");
				interfaces.engine->ClientCmd_Unrestricted(cmd.c_str());
				cmd = crypt_str("setang ") + std::to_string(cur.first.x) + crypt_str(" ") + std::to_string(cur.first.y) + crypt_str(" ") + std::to_string(cur.first.z) + crypt_str(";");
				interfaces.engine->ClientCmd_Unrestricted(cmd.c_str());
				pressdelay = interfaces.global_vars->realtime + 0.2;
			}

			if (vars.movement.checkpoint.nextkey.properstate())
			{
				if (currentcheckpoint > 0)
					currentcheckpoint--;
				pressdelay = interfaces.global_vars->realtime + 0.2;
			}

			if (vars.movement.checkpoint.prevkey.properstate())
			{
				if (currentcheckpoint + 1 < checkpoints.size())
					currentcheckpoint++;
				pressdelay = interfaces.global_vars->realtime + 0.2;
			}

			if (vars.movement.checkpoint.undokey.properstate())
			{
				checkpoints.erase(checkpoints.begin() + currentcheckpoint);
				if (currentcheckpoint >= checkpoints.size())
					currentcheckpoint = max(0,checkpoints.size() - 1);
				pressdelay = interfaces.global_vars->realtime + 0.2;
			}
		}
	}
}


void Movement::CheckpointENDSCENE()
{
	if (vars.movement.checkpoint.enabled && ImGui::GetCurrentContext() && ((!interfaces.engine->IsInGame() && vars.menu.open) || interfaces.engine->IsInGame()))
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, vars.menu.window_opacity / 100.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.f, 1.f, 0.5f, 1.f));
		//selectable colors
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));

		auto& style = ImGui::GetStyle();

		style.WindowRounding = 3.f;
		style.WindowTitleAlign = { 0.5, 0.5 };
		style.WindowBorderSize = 0.f;
		style.FrameBorderSize = 0.f;
		style.ChildRounding = 3.f;
		style.WindowPadding.x = 5.f;
		style.FramePadding = { 0.f, 2.f };
		style.FrameRounding = 3.f;

		ImGui::GetStyle().WindowPadding = { 5,3 };
		ImGui::PushFont(fonts::logger);
		ImGui::SetNextWindowSizeConstraints({ 90,30 }, { 400,800 });
		ImGui::Begin(crypt_str("Checkpoints"), (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		//ImGui::PopStyleVar();
		

		if (vars.menu.inLoadTeleport) {
			ImGui::SetWindowPos(ImVec2(std::clamp(vars.movement.checkpoint.teleportx * csgo->w, 0.f, (float)csgo->w), std::clamp(vars.movement.checkpoint.teleporty * csgo->h, 0.f, (float)csgo->h)), ImGuiCond_Always);
			vars.menu.inLoadTeleport = false;
		}

		vars.menu.cteleportx = ImGui::GetWindowPos().x / csgo->w;
		vars.menu.cteleporty = ImGui::GetWindowPos().y / csgo->h;

		if (ImGui::BeginTable(crypt_str("##checkpoint_table"), 2))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			auto checkcolor = vars.movement.checkpoint.checkpointkey.properstate() ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(0.75, 0.75, 0.75, 1.0);
			ImGui::TextColored(checkcolor, crypt_str("Checkpoint\t"));
			ImGui::TableNextColumn();
			ImGui::TextColored(checkcolor, std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.checkpointkey.key)).c_str());
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			auto telecolor = vars.movement.checkpoint.teleportkey.properstate() ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(0.75, 0.75, 0.75, 1.0);
			ImGui::TextColored(telecolor, crypt_str("Teleport\t"));
			ImGui::TableNextColumn();
			ImGui::TextColored(telecolor, std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.teleportkey.key)).c_str());
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			auto prevcolor = vars.movement.checkpoint.prevkey.properstate() ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(0.75, 0.75, 0.75, 1.0);
			ImGui::TextColored(prevcolor, crypt_str("Prev Checkpoint\t"));
			ImGui::TableNextColumn();
			ImGui::TextColored(prevcolor, std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.prevkey.key)).c_str());
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			auto nextcolor = vars.movement.checkpoint.nextkey.properstate() ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(0.75, 0.75, 0.75, 1.0);
			ImGui::TextColored(nextcolor, crypt_str("Next Checkpoint\t"));
			ImGui::TableNextColumn();
			ImGui::TextColored(nextcolor, std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.nextkey.key)).c_str());
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			auto undocolor = vars.movement.checkpoint.undokey.properstate() ? ImVec4(1.0, 1.0, 1.0, 1.0) : ImVec4(0.75, 0.75, 0.75, 1.0);
			ImGui::TextColored(undocolor, crypt_str("Undo Checkpoint\t"));
			ImGui::TableNextColumn();
			ImGui::TextColored(undocolor, std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.undokey.key)).c_str());
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(0.75, 0.75, 0.75, 1.0), crypt_str("Current checkpoint:"));
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(0.75, 0.75, 0.75, 1.0), std::to_string(currentcheckpoint).c_str());
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(0.75, 0.75, 0.75, 1.0), crypt_str("Checkpoints count:"));
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(0.75, 0.75, 0.75, 1.0), std::to_string(checkpoints.size()).c_str());
			ImGui::EndTable();
		}
		/*
		std::string text;
		text += crypt_str("Checkpoint Menu\n\n");

		if (vars.movement.checkpoint.checkpointkey.properstate())
			text += crypt_str("[") + (vars.movement.checkpoint.checkpointkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.checkpointkey.key)) : crypt_str("No key")) + crypt_str("] ") + crypt_str("Checkpoint\n");
		else
			text += (vars.movement.checkpoint.checkpointkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.checkpointkey.key)) : crypt_str("No key")) + crypt_str(" ") + crypt_str("Checkpoint\n");

		if (vars.movement.checkpoint.teleportkey.properstate())
			text += crypt_str("[") + (vars.movement.checkpoint.teleportkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.teleportkey.key)) : crypt_str("No key")) + crypt_str("] ") + crypt_str("Teleport\n");
		else
			text += (vars.movement.checkpoint.teleportkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.teleportkey.key)) : crypt_str("No key")) + crypt_str(" ") + crypt_str("Teleport\n");

		if (vars.movement.checkpoint.prevkey.properstate())
			text += crypt_str("[") + (vars.movement.checkpoint.prevkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.prevkey.key)) : crypt_str("No key")) + crypt_str("] ") + crypt_str("Prev Checkpoint\n");
		else
			text += (vars.movement.checkpoint.prevkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.prevkey.key)) : crypt_str("No key")) + crypt_str(" ") + crypt_str("Prev Checkpoint\n");

		if (vars.movement.checkpoint.nextkey.properstate())
			text += crypt_str("[") + (vars.movement.checkpoint.nextkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.nextkey.key)) : crypt_str("No key")) + crypt_str("] ") + crypt_str("Next Checkpoint\n");
		else
			text += (vars.movement.checkpoint.nextkey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.nextkey.key)) : crypt_str("No key")) + crypt_str(" ") + crypt_str("Next Checkpoint\n");

		if (vars.movement.checkpoint.undokey.properstate())
			text += crypt_str("[") + (vars.movement.checkpoint.undokey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.undokey.key)) : crypt_str("No key")) + crypt_str("] ") + crypt_str("Undo Checkpoint\n");
		else
			text += (vars.movement.checkpoint.undokey.key ? std::string(interfaces.inputsystem->vkey2string(vars.movement.checkpoint.undokey.key)) : crypt_str("No key")) + crypt_str(" ") + crypt_str("Undo Checkpoint\n");
	
		text += crypt_str("Checkpoints count: ") + std::to_string(checkpoints.size()) + crypt_str("\nCurrrent checkpoint: ") + std::to_string(currentcheckpoint) + crypt_str("\n");
		

		ImGui::TextWrapped(text.c_str());
		*/
		
		ImGui::PopFont();
		ImGui::PopStyleColor(16);
		ImGui::End();
	}
	
}

std::array<std::pair<bool, Vector>, 4> pointsss;

void Movement::CheckpointDraw()
{
	
	if (csgo->local)
	{
		if (csgo->is_connected && csgo->local->isAlive())
		{
			int cur_height = (int)((float)csgo->h * vars.movement.indicators.height);


			if (vars.movement.indicators.on[vars.movement.indicators.EB])
			{
				if (vars.movement.edgebug.key.properstate())
				{
					color_t ebcol = (csgo->unpdred_tick >= csgo->detectdata.detecttick && csgo->unpdred_tick <= csgo->detectdata.edgebugtick) ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("eb"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("eb"));
					
					cur_height += 29;
				}
			}

			if (vars.movement.indicators.on[vars.movement.indicators.EJ])
			{
				if (vars.movement.edgejumpkey.properstate())
				{
					color_t ebcol = in_ej ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("ej"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("ej"));
					
					cur_height += 29;
				}
			}

			if (vars.movement.indicators.on[vars.movement.indicators.JB])
			{
				if (vars.movement.jumpbugkey.properstate())
				{
					color_t ebcol = jumpbug ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("jb"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("jb"));
					
					cur_height += 29;
				}
			}

			if (vars.movement.indicators.on[vars.movement.indicators.LG])
			{
				if (vars.movement.ladderglidekey.properstate())
				{
					color_t ebcol = csgo->laddergliding ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("lg"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("lg"));
					
					cur_height += 29;
				}
			}


			if (vars.movement.indicators.on[vars.movement.indicators.LJ])
			{
				if (vars.movement.longjumpkey.properstate())
				{
					
					color_t ebcol = in_lj  ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("lj"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("lj"));
					
					cur_height += 29;
				}
			}


			if (vars.movement.indicators.on[vars.movement.indicators.MJ])
			{
				if (vars.movement.minijumpkey.properstate())
				{

					color_t ebcol = in_mj ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("mj"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("mj"));
					
					cur_height += 29;
				}
			}

			if (vars.movement.indicators.on[vars.movement.indicators.PX])
			{
				if (vars.movement.autopixelsurfkey.properstate())
				{

					color_t ebcol = csgo->in_pixelsurf ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("px"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("px"));
					
					cur_height += 29;
				}
			}

			if (vars.movement.indicators.on[vars.movement.indicators.AL])
			{
				if (vars.movement.autopixelsurf_align.properstate())
				{
					bool p = false;
					for (auto& o : pointsss)
					{
						if (o.first)
							p = true;
					}

					color_t ebcol = p ? vars.movement.indicators.active_col : vars.movement.indicators.passive_col;
					g_Render->DrawString(((float)csgo->w / 2.f) + 1.f, cur_height + 1.f, color_t(0, 0, 0, ebcol.get_alpha()), render::centered_x, fonts::move_indicator, crypt_str("al"));
					g_Render->DrawString((float)csgo->w / 2.f, cur_height, ebcol, render::centered_x, fonts::move_indicator, crypt_str("al"));
					
					cur_height += 29;
				}
			}
			int mt = csgo->local->GetMoveType();
			if (vars.movement.autopixelsurf && vars.movement.autopixelsurf_alignind && vars.movement.autopixelsurfkey.properstate() && mt != MOVETYPE_LADDER && mt != MOVETYPE_NOCLIP && !(csgo->flags & FL_ONGROUND))
			{
				for (int i = 0; i < 4; i++)
				{
					int alpha = pointsss.at(i).first ? 255 : 60;
					color_t col = vars.movement.autopixelsurf_indcol1;
					col.set_alpha(alpha);

					if (i != 3)
					{
						Vector out1, out2;
						if (Math::WorldToScreen(pointsss.at(i).second, out1) && Math::WorldToScreen(pointsss.at(i + 1).second, out2))
							g_Render->DrawLine(out1.x, out1.y, out2.x, out2.y, col, 2);
							
							//ImGui::GetBackgroundDrawList()->AddLine({ out1.x, out1.y }, { out2.x, out2.y }, color_t(col).u32());
					}
					else
					{
						Vector out1, out2;
						if (Math::WorldToScreen(pointsss.at(i).second, out1) && Math::WorldToScreen(pointsss.at(0).second, out2))
							g_Render->DrawLine(out1.x, out1.y, out2.x, out2.y, col, 2);
					}
					
				}
			}
		}
	}


	if (vars.movement.checkpoint.enabled)
	{
		if (!checkpoints.empty())
		{
			static auto world_circle = [](Vector location, float radius) {
				static constexpr float Step = PI * 2.0f / 60;
				std::vector<ImVec2> points;
				for (float lat = 0.f; lat <= PI * 2.0f; lat += Step)
				{
					const auto& point3d = Vector(sin(lat), cos(lat), 0.f) * radius;
					Vector point2d;
					if (Math::WorldToScreen(location + point3d, point2d))
						points.push_back(ImVec2(point2d.x, point2d.y));
				}
				auto flags_backup = ImGui::GetBackgroundDrawList()->Flags;
				ImGui::GetBackgroundDrawList()->Flags |= ImDrawListFlags_AntiAliasedFill;
				ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(points.data(), points.size(), vars.movement.checkpoint.color.u32());
				//g_Render->_drawList->AddPolyline(points.data(), points.size(), color_t(0, 0, 255, 255).u32(), true, 2.f);
				ImGui::GetBackgroundDrawList()->Flags = flags_backup;
			};

			world_circle(checkpoints.at(currentcheckpoint).second, 10.f);
		}
	}

	if (csgo->unpdred_tick >= csgo->detectdata.detecttick && csgo->unpdred_tick <= csgo->detectdata.edgebugtick && vars.movement.edgebug.marker)
	{
		for (int i = 0; i < detectionpositions.size() - 1; i++)
		{
			Vector cur;
			Vector next;
			if (Math::WorldToScreen(detectionpositions.at(i).first, cur) && Math::WorldToScreen(detectionpositions.at(i + 1).first, next))
			{
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ cur.x,cur.y }, ImVec2{ next.x,next.y }, vars.movement.edgebug.marker_color.u32(), (float)vars.movement.edgebug.marker_thickness);
			}
		}

		Vector endpoints[4];
		if (Math::WorldToScreen(Vector{ detectionpositions.at(detectionpositions.size() - 1).first.x - 16.f, detectionpositions.at(detectionpositions.size() - 1).first.y - 16.f, detectionpositions.at(detectionpositions.size() - 1).first.z }, endpoints[0]) &&
			Math::WorldToScreen(Vector{ detectionpositions.at(detectionpositions.size() - 1).first.x - 16.f, detectionpositions.at(detectionpositions.size() - 1).first.y + 16.f, detectionpositions.at(detectionpositions.size() - 1).first.z }, endpoints[1]) &&
			Math::WorldToScreen(Vector{ detectionpositions.at(detectionpositions.size() - 1).first.x + 16.f, detectionpositions.at(detectionpositions.size() - 1).first.y - 16.f, detectionpositions.at(detectionpositions.size() - 1).first.z }, endpoints[2]) &&
			Math::WorldToScreen(Vector{ detectionpositions.at(detectionpositions.size() - 1).first.x + 16.f, detectionpositions.at(detectionpositions.size() - 1).first.y + 16.f, detectionpositions.at(detectionpositions.size() - 1).first.z }, endpoints[3]))
		{
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ endpoints[0].x, endpoints[0].y}, ImVec2{ endpoints[1].x, endpoints[1].y }, vars.movement.edgebug.marker_color.u32(), (float)vars.movement.edgebug.marker_thickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ endpoints[1].x, endpoints[1].y }, ImVec2{ endpoints[3].x, endpoints[3].y }, vars.movement.edgebug.marker_color.u32(), (float)vars.movement.edgebug.marker_thickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ endpoints[3].x, endpoints[3].y }, ImVec2{ endpoints[2].x, endpoints[2].y }, vars.movement.edgebug.marker_color.u32(), (float)vars.movement.edgebug.marker_thickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2{ endpoints[2].x, endpoints[2].y }, ImVec2{ endpoints[0].x, endpoints[0].y }, vars.movement.edgebug.marker_color.u32(), (float)vars.movement.edgebug.marker_thickness);
		}
		//std::string msg = "crouched: " + to_string(csgo->detectdata.crouched) + "\n" + "strafing: " + to_string(csgo->detectdata.strafing) + "\n" + "detecttick: " + to_string(csgo->detectdata.detecttick) + "\n" + "edgebugtick: " + to_string(csgo->detectdata.edgebugtick) + "\n" + "currenttick: " + to_string(interfaces.global_vars->tickcount) + "\n" + "yawdelta: " + to_string(csgo->detectdata.yawdelta) + "\n" + "forwardmove: " + to_string(csgo->detectdata.forwardmove) + "\n" + "sidemove: " + to_string(csgo->detectdata.sidemove) + "\n";
		//g_Render->DrawString(csgo->w / 2, csgo->h * 0.75, color_t(255, 0, 255, 255), 0, fonts::menu_main, msg.c_str());
	}
}
std::pair<int, Vector> get_align_side()
{
	float originx = csgo->origin.x;
	float originy = csgo->origin.y;
	float one_minus_x_floor = 1.f - (originx - floor(originx));
	float one_minus_y_floor = 1.f - (originy - floor(originy));
	float x_floor = originx - floor(originx);
	float y_floor = originy - floor(originy);

	Vector mins = csgo->local->GetMins();
	Vector maxs = csgo->local->GetMaxs();
	const Vector org_origin = csgo->origin;

	trace_t info[4];

	for (int i = 0; i < 4; i++)
	{
		Vector startpos = org_origin;
		Vector endpos = org_origin;
		switch (i)
		{
		case 0:
			startpos.y += mins.y; // adding mins.y so were on the edge of bbox
			endpos.y = floor(startpos.y); // flooring so we trace to the nearest whole hammer unit
			pointsss.at(i).second = org_origin + Vector{ mins.x, mins.y, 0.f };
			break;
		case 1:
			startpos.x += maxs.x;
			endpos.x = floor(startpos.x) + 1.f;
			pointsss.at(i).second = org_origin + Vector{ maxs.x, mins.y, 0.f };
			break;
		case 2:
			startpos.y += maxs.y;
			endpos.y = floor(startpos.y) + 1.f; //flooring y and adding 1 so we trace to opposite side
			pointsss.at(i).second = org_origin + Vector{ maxs.x, maxs.y, 0.f };
			break;
		case 3: //flooring x
			startpos.x += mins.x;
			endpos.x = floor(startpos.x); // negative x from origin
			pointsss.at(i).second = org_origin + Vector{ mins.x, maxs.y, 0.f };
			break;
		}

		trace_t tr;
		CTraceFilter fil;
		fil.pSkip = csgo->local;
		Ray_t ray;
		ray.Init(org_origin, endpos);
		interfaces.trace->TraceRay(ray, MASK_PLAYERSOLID, &fil, &tr);
		//pointsss.at(i) = { tr.startpos, tr.endpos };
		info[i] = tr;
	}

	float min_frac = 1.f;
	int bestind = -1;
	for (int i = 0; i < 4; i++)
	{
		auto& tr = info[i];
		if ((tr.fraction < 1.f || tr.allsolid || tr.startsolid) && (tr.m_pEnt ? !tr.m_pEnt->IsPlayer() : true))
		{
			min_frac = tr.fraction;
			bestind = i;
			switch (i)
			{
			case 0:
				if (y_floor < 0.03125)
					pointsss.at(i).first = true;
				else
					pointsss.at(i).first = false;
				break;

			case 1:
				if (one_minus_x_floor < 0.03125)
					pointsss.at(i).first = true;
				else
					pointsss.at(i).first = false;
				break;
			case 2:
				if (one_minus_y_floor < 0.03125)
					pointsss.at(i).first = true;
				else
					pointsss.at(i).first = false;
				break;
			case 3:
				if (x_floor < 0.03125)
					pointsss.at(i).first = true;
				else
					pointsss.at(i).first = false;
				break;
			}
		}
		else
		{
			pointsss.at(i).first = false;
		}
	}

	if (bestind != -1)
	{
		return { bestind, info[bestind].endpos};
	}
	
	
	return { bestind, {0,0,0} };
}

//dumped from bitcheat DLL LOL:D

int savedbuttons;
int predictedtickamount = -1;

void Movement::AutoPixelSurf(CUserCmd* cmd)
{
	if (!vars.movement.autopixelsurfkey.properstate())
		return;

	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
		return;

	if (csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_LADDER || !csgo->local->isAlive())
		return;

	if (csgo->flags & FL_ONGROUND)
		return;

	int tickspredicted = 0;
	bool foundpx = false;

	engine_prediction->restoreprediction(0, interfaces.prediction->Split->nCommandsPredicted - 1);
	//engine_prediction->restoreprediction();

	auto al_res = get_align_side();

	float org_fmove = cmd->forwardmove;
	float org_smove = cmd->sidemove;

	if (al_res.first != -1 && !(csgo->flags & FL_ONGROUND) && cmd->sidemove == 0.f && cmd->forwardmove == 0.f && vars.movement.autopixelsurf_freestand)
	{
		Vector ang_diff = ((al_res.second - csgo->origin).toAngle() - cmd->viewangles).Normalize(); // calcrelativeangle

		//Misc::Get().FixMovement()

		Vector  move, dir;
		float   delta, len;
		Vector   move_angle;


		move = { 450.f, 0.f, 0.f };

		len = move.NormalizeMovement();

		if (len)
		{
			Math::VectorAngles(move, move_angle);

			delta = ((cmd->viewangles.y + ang_diff.y) - cmd->viewangles.y);

			move_angle.y += delta;

			Math::AngleVectors(move_angle, &dir);

			dir *= len;

			if (cmd->viewangles.x < -90 || cmd->viewangles.x > 90)
				dir.x = -dir.x;

			cmd->forwardmove = dir.x;
			cmd->sidemove = -dir.y;
		}
		//std::string sex = "ang diff: " + to_string(ang_diff.y) + " | fmove: " + to_string(cmd->forwardmove) + " | smove: " + to_string(cmd->sidemove);
		//Msg(sex, color_t(255, 255, 255, 255));
	}

	for (int tickspredicted = 0; tickspredicted <= 1; tickspredicted++)
	{

		float pre_pred_vel = csgo->local->GetVelocity().z;
		int restorebut = cmd->buttons;
		cmd->buttons |= IN_DUCK;
		engine_prediction->start(csgo->local, cmd);
		cmd->buttons = restorebut;
		float post_pred_vel = csgo->local->GetVelocity().z;
		int post_pred_flags = csgo->local->GetFlags();
		if (csgo->local->GetMoveType() == MOVETYPE_LADDER || (post_pred_flags & FL_ONGROUND))
			break;


		if (pre_pred_vel < -6.25 && post_pred_vel < -6.25 && post_pred_vel > pre_pred_vel && !(post_pred_flags & FL_ONGROUND))
		{
			foundpx = true;
			predictedtickamount = tickspredicted;
			savedbuttons = cmd->buttons;
			engine_prediction->end(csgo->local);
			break;
		}
		engine_prediction->end(csgo->local);
	}


	if (foundpx)
	{
		csgo->in_pixelsurf = true;
		cmd->buttons |= IN_DUCK;
	}
	else
	{
		if (csgo->local->GetVelocity().z == -6.25 || csgo->local->GetVelocity().z == -3.125)
		{
			csgo->in_pixelsurf = true;
			cmd->buttons |= IN_DUCK;

			/*
			auto chatelement = (IHudChat*)H::findHudElement(H::hud, crypt_str("CHudChat"));
			if (chatelement)
			{
				Vector orig = csgo->local->GetOrigin();
				std::string sex = "[Ecstasy] Pixelsurf | x: " + to_string(orig.x) + " | y: " + to_string(orig.y) + " | z : " + to_string(orig.z);
				chatelement->ChatPrintf(0, 0, sex.c_str());
			}
			*/
		}
		else
		{
			cmd->forwardmove = org_fmove;
			cmd->sidemove = org_smove;
			csgo->in_pixelsurf = false;
		}

	}

	if (predictedtickamount == -1)
	{
		csgo->pixelsurf_lockmouse = false;
	}
	else
	{
		--predictedtickamount;
		csgo->pixelsurf_lockmouse = true;
		cmd->buttons = savedbuttons;
	}
}


void autoalign_adjustfsmove(CUserCmd* cmd, Vector align_angle)
{
	Vector align_vector_forward = { 0.f,0.f,0.f }, align_vector_right = { 0.f,0.f,0.f }, align_vector_up = { 0.f,0.f,0.f };
	Math::AngleVector4(align_angle, align_vector_forward, align_vector_right, align_vector_up);
	float nor_length2d_alignvecforward = 1.f / (align_vector_forward.Length2D() + FLT_EPSILON); // normalized length2d of forward align vector
	float nor_length2d_alignvecright = 1.f / (align_vector_right.Length2D() + FLT_EPSILON); // normalized length2d of forward align vector
	float up_vecz_align_angle = align_vector_up.z;

	Vector va_vector_forward = { 0.f,0.f,0.f }, va_vector_right = { 0.f,0.f,0.f }, va_vector_up = { 0.f,0.f,0.f };
	Math::AngleVector4(cmd->viewangles, va_vector_forward, va_vector_right, va_vector_up);
	//do tha shit with forward
	float nor_length2d_vavecforward = 1.f / (va_vector_forward.Length2D() + FLT_EPSILON);
	float v35 = nor_length2d_vavecforward * va_vector_forward.x;
	float v32 = nor_length2d_vavecforward * va_vector_forward.y;
	//do tha shit with right
	float nor_length2d_vavecright = 1.f / (va_vector_right.Length2D() + FLT_EPSILON);
	float v34 = nor_length2d_vavecright * va_vector_right.x;
	float v39 = nor_length2d_vavecright * va_vector_right.y;
	//do tha shit with up
	float nor_length2d_vavecup = 1.f / (va_vector_up.z + FLT_EPSILON); // this is v27

	//save fmove, smove
	float saved_fmove = cmd->forwardmove;
	float saved_smove = cmd->sidemove;

	//calculate the desired fmove, smove, umove

	float modified_smove_by_y = saved_smove * nor_length2d_alignvecright * align_vector_right.y; // this is v2
	float modified_smove_by_x = saved_smove * nor_length2d_alignvecright * align_vector_right.x; // this is v45.m128i_i64[1]
	
	float modified_fmove_by_y = saved_fmove * nor_length2d_alignvecforward * align_vector_forward.y; // this is v29.m128i_i64[1]
	float modified_fmove_by_x = saved_fmove * nor_length2d_alignvecforward * align_vector_forward.x; // this is v30.m128i_i64[1]

	float modified_umove_by_z = cmd->upmove * (1.f / (up_vecz_align_angle + FLT_EPSILON)) * up_vecz_align_angle;

	//further calculation

	float calculated_forwardmove = v35 * modified_fmove_by_x + v35 * modified_smove_by_x + v32 * modified_fmove_by_y + v32 * modified_fmove_by_y;
	float calculated_sidemove = v39 * modified_smove_by_y + v34 * modified_smove_by_x + v34 * modified_fmove_by_x + v39 * modified_fmove_by_y;

	//TODO: DETERMINE WHEN TO ACTUALLY SET THIS
	
	if ((align_angle.y == 90.f || align_angle.y == -90.f) && fabsf(saved_smove) > 0.01f)
	{
		cmd->forwardmove = 450.f + calculated_forwardmove;
	}
	else
	{
		cmd->forwardmove = calculated_forwardmove;
	}

	//cmd->forwardmove = calculated_forwardmove

	if ((align_angle.y == 0.f || align_angle.y == 180.f) && fabsf(saved_fmove) > 0.01f)
	{
		cmd->sidemove = 450.f + calculated_sidemove;
	}
	else
	{
		cmd->sidemove = calculated_sidemove;
	}

	cmd->forwardmove = std::clamp(cmd->forwardmove, -450.f, 450.f);
	cmd->sidemove = std::clamp(cmd->sidemove, -450.f, 450.f);
}

void Movement::PixelSurfAlign(CUserCmd* cmd)
{
	if (!csgo->local || csgo->in_pixelsurf || !csgo->local->isAlive() || csgo->local->GetMoveType() == MOVETYPE_NOCLIP || (csgo->flags & FL_ONGROUND) || csgo->local->GetMoveType() == MOVETYPE_LADDER || !vars.movement.autopixelsurf_align.properstate())
		return;

	float originx = csgo->origin.x;
	float originy = csgo->origin.y;
	float one_minus_x_floor = 1.f - (originx - floor(originx));
	float one_minus_y_floor = 1.f - (originy - floor(originy));
	float x_floor = originx - floor(originx);
	float y_floor = originy - floor(originy);
	float forward_move = cmd->forwardmove;
	float side_move = cmd->sidemove;

	int alignside;

	auto al_res = get_align_side();

	alignside = al_res.first;

	//if (alignside != -1 && ((one_minus_x_floor >= 0.00050000002 && one_minus_x_floor <= 0.03125) || (one_minus_y_floor >= 0.00050000002 && one_minus_y_floor <= 0.03125) || (x_floor >= 0.00050000002 && x_floor <= 0.03125) || (y_floor >= 0.00050000002 && y_floor <= 0.03125)) && !(csgo->flags & FL_ONGROUND))
	if((alignside == 0 && y_floor >= 0.03125) || (alignside == 1 && one_minus_x_floor >= 0.03125) || (alignside == 2 && one_minus_y_floor >= 0.03125) || (alignside == 3 && x_floor >= 0.03125))
	{
		Vector orig = csgo->original;//original angle

		float yang = 0;
		//TrolOLOsoososososososooSOOSOSOSOSOSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
		switch (alignside)
		{
		case 0:
			yang = -90.f;
			break;
		case 1:
			yang = 0.f;
			break;
		case 2:
			yang = 90.f;
			break;
		case 3:
			yang = 180.f;
			break;
		}

		Vector ang_diff = { cmd->viewangles.x, yang, 0.f };

		autoalign_adjustfsmove(cmd, ang_diff);

#ifdef _DEBUG
		auto chatelement = (IHudChat*)H::findHudElement(H::hud, crypt_str("CHudChat"));
		if (chatelement)
		{
			Vector orig = csgo->local->GetOrigin();
			std::string sex = "[Reflect] Align | 1-xfloor: " + to_string(one_minus_x_floor) + " | 1-yfloor: " + to_string(one_minus_y_floor) + " | xfloor : " + to_string(x_floor) + " | yfloor: " + to_string(y_floor) + " | fmove: " + to_string(cmd->forwardmove) + " | smove: " + to_string(cmd->sidemove) + " | angle: " + to_string(ang_diff.y) + " | ofmove: " + to_string(forward_move) + " | osmove: " + to_string(side_move) + " | al_side: " + to_string(alignside);
			Msg(sex, color_t(255, 0, 255, 255));
		}
#endif
	}
}


