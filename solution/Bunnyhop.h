#pragma once
#include "Hooks.h"

#define M_PI2	1.57079632679489661923
#define M_PI4	0.785398163397448309616
const float M_U_DEG = 360.0 / 65536;
const float M_U_RAD = 3.14159265358979323846 / 32768;

class CBunnyhop // movemement before prediction
{
public:
	void Run(Vector& orig)
	{
		if (csgo->local->GetMoveType() == MoveType_t::MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MoveType_t::MOVETYPE_LADDER)
			return;

		if (vars.misc.bunnyhop && !vars.movement.jumpbugkey.properstate()) {
			static bool bLastJumped = false;
			static bool bShouldFake = false;

			if (!bLastJumped && bShouldFake)
			{
				bShouldFake = false;
				csgo->cmd->buttons |= IN_JUMP;
			}
			else if (csgo->cmd->buttons & IN_JUMP)
			{
				if (csgo->local->GetFlags() & FL_ONGROUND)
				{
					bShouldFake = bLastJumped = true;
				}
				else
				{
					csgo->cmd->buttons &= ~IN_JUMP;
					bLastJumped = false;
				}
			}
			else
			{
				bShouldFake = bLastJumped = false;
			}
		}

		if (csgo->cmd->buttons & IN_SPEED || !vars.misc.autostrafe  || (vars.misc.autostrafeonkey && !vars.misc.autostrafekey.properstate()) || csgo->local->GetVelocity().Length2D() < 10.f)
			return; // doesn't allow strafe when you hold shift and you're not moving

		static float yaw_add = 0.f;
		static const auto cl_sidespeed = interfaces.cvars->FindVar(hs::cl_sidespeed.s().c_str());
		static const auto cl_forwardspeed = interfaces.cvars->FindVar(hs::cl_forwardspeed.s().c_str());
		if (vars.movement.autostrafetype == 0)
		{
			if (!(csgo->local->GetFlags() & FL_ONGROUND))
			{
				bool back = csgo->cmd->buttons & IN_BACK;
				bool forward = csgo->cmd->buttons & IN_FORWARD;
				bool right = csgo->cmd->buttons & IN_MOVELEFT;
				bool left = csgo->cmd->buttons & IN_MOVERIGHT;

				if (back) {
					yaw_add = -180.f;
					if (right)
						yaw_add -= 45.f;
					else if (left)
						yaw_add += 45.f;
				}
				else if (right) {
					yaw_add = 90.f;
					if (back)
						yaw_add += 45.f;
					else if (forward)
						yaw_add -= 45.f;
				}
				else if (left) {
					yaw_add = -90.f;
					if (back)
						yaw_add -= 45.f;
					else if (forward)
						yaw_add += 45.f;
				}
				else {
					yaw_add = 0.f;
				}

				orig.y += yaw_add;
				csgo->cmd->forwardmove = 0.f;
				csgo->cmd->sidemove = 0.f;

				const auto delta = Math::NormalizeYaw(orig.y - RAD2DEG(atan2(csgo->local->GetVelocity().y, csgo->local->GetVelocity().x)));

				csgo->cmd->sidemove = delta > 0.f ? -cl_sidespeed->GetFloat() : cl_sidespeed->GetFloat();

				orig.y = Math::NormalizeYaw(orig.y - delta);
			}
		}
		else if (vars.movement.autostrafetype == 1)
		{
			if (!(csgo->local->GetFlags() & FL_ONGROUND) && abs(csgo->cmd->mousedx) > vars.movement.dxtostrafe)
			{
				bool fw = false; bool bw = false; bool rt = false; bool lt = false;
				Vector vva;
				interfaces.engine->GetViewAngles(vva);
				float dl = ((csgo->local->GetVelocity() + csgo->local->GetOrigin() - csgo->local->GetOrigin()).toAngle() - vva).Normalize().y;
				float delta = abs(dl);

				if (delta < 45.f)
				{
					fw = true;
				}
				else if (delta < 135.f)
				{
					if (dl > 0.f)
					{
						rt = true;
					}
					else
					{
						lt = true;
					}

				}
				else if (delta < 180.f)
				{
					bw = true;
				}
				
				if (fw)
					csgo->cmd->sidemove = csgo->cmd->mousedx > 0 ? -cl_sidespeed->GetFloat() : cl_sidespeed->GetFloat();
				else if (bw)
					csgo->cmd->sidemove = csgo->cmd->mousedx > 0 ? cl_sidespeed->GetFloat() : -cl_sidespeed->GetFloat();
				else if (rt)
					csgo->cmd->forwardmove = csgo->cmd->mousedx > 0 ? -cl_forwardspeed->GetFloat() : cl_forwardspeed->GetFloat();
				else if (lt)
					csgo->cmd->forwardmove = csgo->cmd->mousedx > 0 ? cl_forwardspeed->GetFloat() : -cl_forwardspeed->GetFloat();
			}
		}
	}
};