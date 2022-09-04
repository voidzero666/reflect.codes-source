#pragma once
#include "Hooks.h"


void Thirdperson()
{
	if (!interfaces.engine->IsInGame() || !csgo->local)
		return;

//	static float cur_dist = vars.visuals.thirdperson_dist;

	if (!vars.misc.thirdperson.state)
	{
		enabledtp = false;
		//cur_dist = vars.visuals.thirdperson_dist;
		return;
	}

	if (csgo->local->isAlive() && enabledtp && csgo->client_state->iDeltaTick > 0)
	{
		if (!interfaces.input->m_fCameraInThirdPerson)
			interfaces.input->m_fCameraInThirdPerson = true;

		if (interfaces.input->m_fCameraInThirdPerson)
		{
			/*if (cur_dist > 0.f)
				cur_dist -= 1.f * sin(animation_speed * 10.f);*/

			auto distance = vars.visuals.thirdperson_dist /*- cur_dist*/;

			Vector angles;
			interfaces.engine->GetViewAngles(angles);

			Vector inverse_angles;
			interfaces.engine->GetViewAngles(inverse_angles);

			inverse_angles.z = distance;

			Vector forward, right, up;
			Math::AngleVector4(inverse_angles, forward, right, up);

			Ray_t ray;
			CTraceFilterWorldAndPropsOnly filter;
			trace_t trace;

			auto eye_pos = csgo->fake_duck
				? csgo->local->GetRenderOrigin() + interfaces.game_movement->GetPlayerViewOffset(false)
				: csgo->local->GetRenderOrigin() + csgo->local->GetVecViewOffset();
			auto offset = eye_pos + forward * -distance + right + up;

			ray.Init(eye_pos, offset, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 16.0f));
			interfaces.trace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);

			trace.fraction = clamp(trace.fraction, 0.f, 1.f);

			angles.z = distance * trace.fraction;

			interfaces.input->m_vecCameraOffset = angles;
		}
		/*else
			cur_dist = vars.visuals.thirdperson_dist;*/
	}
	else {
		//cur_dist = vars.visuals.thirdperson_dist;
		interfaces.input->m_fCameraInThirdPerson = false;
		interfaces.input->m_vecCameraOffset.z = 0.f;
	}

	static auto b_once = false;

	if (csgo->local->isAlive())
	{
		b_once = false;
		return;
	}

	if (b_once)
		csgo->local->SetObserverMode(5);

	if (csgo->local->GetObserverMode() == 4)
		b_once = true;
}



//													//
//													//
//													//
//													//
//			 Failed Motion Blur Attempt			    //
//													//
//													//
//													//
//												    //													
//const bool mat_motion_blur_forward_enabled = false;
//const float mat_motion_blur_falling_min = 10.f;
//const float mat_motion_blur_falling_max = 20.f;
//const float mat_motion_blur_falling_intensity = 1.f;
//const float mat_motion_blur_rotation_intensity = 1.f;
//const float mat_motion_blur_strength = 1.f;

struct MotionBlurHistory_t
{
	MotionBlurHistory_t()
	{
		m_flLastTimeUpdate = 0.0f;
		m_flPreviousPitch = 0.0f;
		m_flPreviousYaw = 0.0f;
		m_vPreviousPositon.Init(0.0f, 0.0f, 0.0f);
		m_mPreviousFrameBasisVectors;
		m_flNoRotationalMotionBlurUntil = 0.0f;
		m_mPreviousFrameBasisVectors.SetIdentityMatrix();
	}

	float m_flLastTimeUpdate;
	float m_flPreviousPitch;
	float m_flPreviousYaw;
	Vector m_vPreviousPositon;
	matrix m_mPreviousFrameBasisVectors;
	float m_flNoRotationalMotionBlurUntil;
};

enum MotionBlurMode_t
{
	MOTION_BLUR_DISABLE = 1,
	MOTION_BLUR_GAME = 2,			// Game uses real-time inter-frame data
	MOTION_BLUR_SFM = 3				// Use SFM data passed in CViewSetup structure
};

bool DoImageSpaceMotionBlur(CViewSetup* setup)
{

	//H::DoDepthOfField(view_ptr);

	// Do motion blur here


	//if(view.m_nMotionBlurMode == MOTION_BLUR_DISABLE || !vars.visuals.motion_blur)

	if (!vars.visuals.motionblur.enabled)
		return false;


	if (setup)
	{

		static MotionBlurHistory_t history;

		const float fltimeelapsed = interfaces.global_vars->realtime - history.m_flLastTimeUpdate;

		const auto viewangles = setup->angles;

		const float flcurrentpitch = Math::NormalizeYaw(viewangles.x);
		const float flcurrentyaw = Math::NormalizeYaw(viewangles.y);

		Vector vcurrentsidevec;
		Vector vcurrentforwardvec;
		Vector vcurrentupvec;
		Math::AngleVector4(setup->angles, vcurrentforwardvec, vcurrentsidevec, vcurrentupvec);

		Vector vcurrentposition = setup->origin;
		Vector vpositionchange = history.m_vPreviousPositon - vcurrentposition;

		if ((vpositionchange.Length() > 30.0f) && (fltimeelapsed >= 0.5f))
		{
			csgo->g_vMotionBlurValues[0] = 0.0f;
			csgo->g_vMotionBlurValues[1] = 0.0f;
			csgo->g_vMotionBlurValues[2] = 0.0f;
			csgo->g_vMotionBlurValues[3] = 0.0f;
		}
		else if (fltimeelapsed > (1.0f / 30.0f))
		{

			// if slower than 30 fps, don't motion blur //

			csgo->g_vMotionBlurValues[0] = 0.0f;
			csgo->g_vMotionBlurValues[1] = 0.0f;
			csgo->g_vMotionBlurValues[2] = 0.0f;
			csgo->g_vMotionBlurValues[3] = 0.0f;
		}
		else if (vpositionchange.Length() > 50.0f)
		{
			history.m_flNoRotationalMotionBlurUntil = interfaces.global_vars->realtime + 1.0f; // wait a second until the portal craziness calms down
		}
		else
		{
			float flhorizontalfov = setup->fov;
			float flverticalfov = (setup->m_flAspectRatio <= 0.0f) ? (setup->fov) : (setup->fov / setup->m_flAspectRatio);
			float flviewdotmotion = vcurrentforwardvec.Dot(vpositionchange);

			// FORWARD MOTION BLUR
			if (vars.visuals.motionblur.fwd_enabled)
				csgo->g_vMotionBlurValues[2] = flviewdotmotion;

			float flsidedotmotion = vcurrentsidevec.Dot(vpositionchange);
			float flyawdifforiginal = history.m_flPreviousYaw - flcurrentyaw;
			if (((history.m_flPreviousYaw - flcurrentyaw > 180.0f) || (history.m_flPreviousYaw - flcurrentyaw < -180.0f)) &&
				((history.m_flPreviousYaw + flcurrentyaw > -180.0f) && (history.m_flPreviousYaw + flcurrentyaw < 180.0f)))
				flyawdifforiginal = history.m_flPreviousYaw + flcurrentyaw;

			float flyawdiffadjusted = flyawdifforiginal + (flsidedotmotion / 3.0f);

			if (flyawdifforiginal < 0.0f)
				flyawdiffadjusted = std::clamp(flyawdiffadjusted, flyawdifforiginal, 0.0f);
			else
				flyawdiffadjusted = std::clamp(flyawdiffadjusted, 0.0f, flyawdifforiginal);

			float flundampenedyaw = flyawdiffadjusted / flhorizontalfov;
			csgo->g_vMotionBlurValues[0] = flundampenedyaw * (1.0f - (fabs(flcurrentpitch) / 90.0f));

			float flpitchcompensatemask = 1.0f - ((1.0f - fabs(vcurrentforwardvec[2])) * (1.0f - fabs(vcurrentforwardvec[2])));
			float flpitchdifforiginal = history.m_flPreviousPitch - flcurrentpitch;
			float flpitchdiffadjusted = flpitchdifforiginal;

			if (flcurrentpitch > 0.0f)
				flpitchdiffadjusted = flpitchdifforiginal - ((flviewdotmotion / 2.0f) * flpitchcompensatemask);
			else
				flpitchdiffadjusted = flpitchdifforiginal + ((flviewdotmotion / 2.0f) * flpitchcompensatemask);


			if (flpitchdifforiginal < 0.0f)
				flpitchdiffadjusted = std::clamp(flpitchdiffadjusted, flpitchdifforiginal, 0.0f);
			else
				flpitchdiffadjusted = std::clamp(flpitchdiffadjusted, 0.0f, flpitchdifforiginal);

			csgo->g_vMotionBlurValues[1] = flpitchdiffadjusted / flverticalfov;
			csgo->g_vMotionBlurValues[3] = flundampenedyaw;
			csgo->g_vMotionBlurValues[3] *= (fabs(flcurrentpitch) / 90.0f) * (fabs(flcurrentpitch) / 90.0f) * (fabs(flcurrentpitch) / 90.0f); // dampen roll based on pitch^3

			if (fltimeelapsed > 0.0f)
				csgo->g_vMotionBlurValues[2] /= fltimeelapsed * 30.0f;
			else
				csgo->g_vMotionBlurValues[2] = 0.0f;

			csgo->g_vMotionBlurValues[2] = std::clamp((fabs(csgo->g_vMotionBlurValues[2]) - vars.visuals.motionblur.falling_min) / (vars.visuals.motionblur.falling_max - vars.visuals.motionblur.falling_min), 0.0f, 1.0f) * (csgo->g_vMotionBlurValues[2] >= 0.0f ? 1.0f : -1.0f);            //g_vmotionblurvalues[2] = std::clamp((fabs(g_vmotionblurvalues[2]) - 10.f) / (20.f - 10.f), 0.0f, 1.0f) * (g_vmotionblurvalues[2] >= 0.0f ? 1.0f : -1.0f);
			csgo->g_vMotionBlurValues[2] /= 30.0f;
			csgo->g_vMotionBlurValues[0] *= vars.visuals.motionblur.rotation_intensity * .15f * vars.visuals.motionblur.strength;
			csgo->g_vMotionBlurValues[1] *= vars.visuals.motionblur.rotation_intensity * .15f * vars.visuals.motionblur.strength;
			csgo->g_vMotionBlurValues[2] *= vars.visuals.motionblur.rotation_intensity * .15f * vars.visuals.motionblur.strength;
			csgo->g_vMotionBlurValues[3] *= vars.visuals.motionblur.falling_intensity * .15f * vars.visuals.motionblur.strength;

		}

		if (interfaces.global_vars->realtime < history.m_flNoRotationalMotionBlurUntil)
		{
			csgo->g_vMotionBlurValues[0] = 0.0f; // x
			csgo->g_vMotionBlurValues[1] = 0.0f; // y
			csgo->g_vMotionBlurValues[3] = 0.0f; // roll
		}
		else
		{
			history.m_flNoRotationalMotionBlurUntil = 0.0f;
		}
		history.m_vPreviousPositon = vcurrentposition;

		history.m_flPreviousPitch = flcurrentpitch;
		history.m_flPreviousYaw = flcurrentyaw;
		history.m_flLastTimeUpdate = interfaces.global_vars->realtime;
		return true;
	}
	else
		{
		const auto material = interfaces.material_system->FindMaterial(crypt_str("dev/motion_blur"), crypt_str("RenderTargets"), false);

		if (!material)
			return false;

		int w, h;
		interfaces.engine->GetScreenSize(w, h);
		auto MotionBlurInternal = material->FindVar(crypt_str("$MotionBlurInternal"), nullptr, false);

		MotionBlurInternal->setVecComponentValue(csgo->g_vMotionBlurValues[0], 0);
		MotionBlurInternal->setVecComponentValue(csgo->g_vMotionBlurValues[1], 1);
		MotionBlurInternal->setVecComponentValue(csgo->g_vMotionBlurValues[2], 2);
		MotionBlurInternal->setVecComponentValue(csgo->g_vMotionBlurValues[3], 3);

		auto MotionBlurViewPortInternal = material->FindVar(crypt_str("$MotionBlurViewportInternal"), nullptr, false);

		MotionBlurViewPortInternal->setVecComponentValue(0.0f, 0);
		MotionBlurViewPortInternal->setVecComponentValue(0.0f, 1);
		MotionBlurViewPortInternal->setVecComponentValue(1.0f, 2);
		MotionBlurViewPortInternal->setVecComponentValue(1.0f, 3);

		//drawingScreen Efect

		const auto draw_function = csgo->drawScreenEffectMaterial;
		__asm {
			__asm push h
			__asm push w
			__asm push 0
			__asm xor edx, edx
			__asm mov ecx, material
			__asm call draw_function
			__asm add esp, 12
		}
	}

}


/*

void __stdcall Hooked_RenderSmokeOverlay(bool update)
{
	g_pViewRenderHook->callOriginal<void, 41>(update);

	//if (update)
		//DoImageSpaceMotionBlur(nullptr);

}

*/

void __stdcall Hooked_OverrideView(void* pSetup)
{
	//static auto OverRideView = g_pClientModeHook->GetOriginal< OverRideViewFn >(g_HookIndices[fnva1(hs::Hooked_OverrideView.s().c_str())]);


	if (csgo->DoUnload)
		return 	g_pClientModeHook->callOriginal<void, 18>(pSetup);

	auto Setup = reinterpret_cast<CViewSetup*>(pSetup);

	if (csgo->is_connected) {
		float fov_addition = vars.misc.worldfov;
		fov_addition = clamp(fov_addition, 0.f, 50.f);


		if (csgo->is_local_alive) {
			if (vars.visuals.remove[0])
				Setup->angles -= csgo->local->GetPunchAngle() * 0.9f + csgo->local->GetViewPunchAngle();

			CGrenadePrediction::Get().View(Setup);

			if (vars.visuals.remove[4]) {
				if (csgo->zoom_level > 0)
					Setup->fov = (90.f + fov_addition) / csgo->zoom_level;
				else
					Setup->fov = (90.f + fov_addition);
			}
			else
				Setup->fov += fov_addition;
		}

		Thirdperson();

		auto setViewmodel = [](IBasePlayer* viewModel, const Vector& angles) noexcept
		{
			if (viewModel)
			{
				Vector forward;
				Math::AngleVectors(angles, forward);
				Vector up;
				Math::AngleVectors(angles - Vector{ 90.0f, 0.0f, 0.0f }, up);
				Vector side = forward.Cross(up);
				Vector offset = side * vars.misc.viewmodel_x + forward * vars.misc.viewmodel_y + up * vars.misc.viewmodel_z;
				viewModel->SetAbsOrigin(viewModel->GetRenderOrigin() + offset);
				viewModel->SetAbsAngles(angles + Vector{ 0.0f, 0.0f, vars.misc.viewmodel_roll });
			}
		};
		if (csgo->local)
		{
			if (csgo->is_local_alive)
			{
				if (vars.misc.enableviewmodel && !csgo->local->IsScoped() && !interfaces.input->m_fCameraInThirdPerson)
					setViewmodel(csgo->local->GetViewModel(), Setup->angles);
			}
			else if (auto observed = csgo->local->GetObserverTarget(); observed && csgo->local->GetObserverMode() == 4)
			{
				if (vars.misc.enableviewmodel && !observed->IsScoped())
					setViewmodel(observed->GetViewModel(), Setup->angles);
			}
		}

		DoImageSpaceMotionBlur(Setup);

		g_pClientModeHook->callOriginal<void, 18>(pSetup);

		

		if (csgo->is_local_alive && csgo->fake_duck)
		{
			Setup->origin = csgo->local->GetRenderOrigin() + Vector(0.0f, 0.0f, interfaces.game_movement->GetPlayerViewOffset(false).z + 0.064f);

			if (interfaces.input->m_fCameraInThirdPerson)
			{
				auto camera_angles = Vector(interfaces.input->m_vecCameraOffset.x, interfaces.input->m_vecCameraOffset.y, 0.0f); //-V807
				auto camera_forward = Vector();

				Math::AngleVectors(camera_angles, camera_forward);
				Math::VectorMA(Setup->origin, -interfaces.input->m_vecCameraOffset.z, camera_forward, Setup->origin);
			}
		}
	}
	else
		g_pClientModeHook->callOriginal<void, 18>(pSetup);
}

float __stdcall Hooked_ViewModel()
{
	float fov = g_pClientModeHook->callOriginal<float, 35>();

	if (csgo->DoUnload)
		return fov;

	if (interfaces.engine->IsConnected() && interfaces.engine->IsInGame())
	{
		if (csgo->local)
		{
			fov = vars.misc.viewmodelfov == 0 ? 68 : vars.misc.viewmodelfov;
		}
	}
	return fov;
}



void __fastcall Hooked_OverrideMouseInput(IClientMode* thisptr, int edx, float* x, float* y)
{
	//static auto oOverrideMouseInput = g_pClientModeHook->GetOriginal<void(__fastcall*)(IClientMode*,int, float*, float*)>(g_HookIndices[fnva1(hs::Hooked_OverrideMouseInput.s().c_str())]);

	if (!interfaces.engine->IsInGame() || !csgo->local)
		return g_pClientModeHook->callOriginal<void, 23>(x, y);

	if (interfaces.global_vars->tickcount > csgo->detectdata.detecttick && interfaces.global_vars->tickcount <= csgo->detectdata.edgebugtick)
	{

		switch (vars.movement.edgebug.locktype)
		{
		case 0:
			*x *= 1.f - vars.movement.edgebug.lock;
			break;
		case 1:

			if (*x != 0.f)
			{
				float ratio = vars.movement.edgebug.lock;

				float full_tick_amount = csgo->detectdata.edgebugtick - csgo->detectdata.detecttick;
				float ticks_left_to_eb = csgo->detectdata.edgebugtick - interfaces.global_vars->tickcount;

				float ratioratio = 1.f - (ticks_left_to_eb / full_tick_amount);
				
				*x *= (ratio * ratioratio);
			}
			

			/*
			float force = 1.f;
			if (csgo->mdxOnEB != 0 && vars.movement.edgebug.lock != 0.f)
				force = std::clamp((float)(abs(csgo->mdxOnEB - csgo->cmd->mousedx) / abs(csgo->mdxOnEB)), 0.f, 1.f);


			*x *= std::clamp(force * ((1.f - vars.movement.edgebug.lock)), 0.f, 1.f);
			*/
			break;
		}
		
		
		
	}
	else
	{
		if (!(csgo->flags & FL_ONGROUND) && csgo->local->GetMoveType() != MoveType_t::MOVETYPE_LADDER)
		{

			if (abs(*x) > vars.movement.mslx * 10.f && vars.movement.msl)
			{
				if (*x > 0.f)
				{
					*x = vars.movement.mslx * 10.f;
				}
				else
				{
					*x = -vars.movement.mslx * 10.f;
				}
			}
		}
	}

	return g_pClientModeHook->callOriginal<void, 23>(x, y);
}



bool __stdcall isDepthOfFieldEnabled_Hooked()
{
	DoImageSpaceMotionBlur(nullptr);
	return false;
}