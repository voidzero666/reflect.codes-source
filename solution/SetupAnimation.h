#pragma once
#include "Hooks.h"

void __fastcall Hooked_StandardBlendingRules(IBasePlayer* entity, void* edx, c_studio_hdr* hdr, Vector* pos, Quaternion* q, float curTime, int boneMask)
{
	static auto original = dtStandardBlendingRules.getOriginal<void>(  hdr,   pos,   q,  curTime,  boneMask);

	if (!entity->IsPlayer())
		return original(entity, hdr, pos, q, curTime, boneMask);

	entity->GetEffects() |= 8;
	original(entity, hdr, pos, q, curTime, boneMask);
	entity->GetEffects() &= ~8;
}

void __fastcall Hooked_BuildTransformations(void* ecx, void* edx, int a2, int a3, int a4, int a5, int a6, int a7) {
	auto entity = (IBasePlayer*)ecx;
	if (!entity || !entity->IsPlayer() || !entity->isAlive())
		return H::BuildTransformations(ecx, a2, a3, a4, a5, a6, a7);

	auto hdr = (c_studio_hdr*)a2;

	// backup bone flags.
	const auto backup_bone_flags = hdr->m_boneFlags;

	// stop procedural animations.
	for (auto i = 0; i < hdr->m_boneFlags.Count(); i++)
		hdr->m_boneFlags.Element(i) &= ~0x04;

	auto backup_jiggle_bones = *(bool*)((DWORD)entity + 0x2930);

	*(bool*)((DWORD)entity + 0x2930) = false;

	H::BuildTransformations(ecx, a2, a3, a4, a5, a6, a7);

	hdr->m_boneFlags = backup_bone_flags;
}


bool __fastcall Hooked_SetupBones(void* ecx, void* edx, matrix* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	if (csgo->DoUnload)
		return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	auto entity = (IBasePlayer*)((uintptr_t)ecx - 4);
	if (csgo->game_rules->IsFreezeTime()
		|| entity == nullptr || csgo->local == nullptr || !pBoneToWorldOut 
		|| !entity->IsPlayer()
		|| (entity->EntIndex() != interfaces.engine->GetLocalPlayer()
			&& entity->GetTeam() == csgo->local->GetTeam()))
		return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	auto state = entity->GetPlayerAnimState();
	if (!state)
		return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	if (entity->EntIndex() == interfaces.engine->GetLocalPlayer())
	{
		if (csgo->game_rules->IsFreezeTime()
			|| csgo->local->HasGunGameImmunity() || csgo->local->GetFlags() & FL_FROZEN
			|| !csgo->realmatrix) {
			return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		}
		else {
			//no balanceadjust november
			entity->GetAnimOverlays()[12].m_flWeight = FLT_EPSILON;
			entity->GetAnimOverlays()[3].m_flWeight = 0.f;
			entity->GetAnimOverlays()[3].m_flCycle = 0.f;
			//return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
			

			if (!csgo->UpdateMatrixLocal) {
				if (pBoneToWorldOut) {
					for (auto& i : csgo->realmatrix) {
						i[0][3] += entity->GetRenderOrigin().x;
						i[1][3] += entity->GetRenderOrigin().y;
						i[2][3] += entity->GetRenderOrigin().z;
					}
					memcpy(pBoneToWorldOut, csgo->realmatrix, sizeof(matrix) * nMaxBones);
					for (auto& i : csgo->realmatrix) {
						i[0][3] -= entity->GetRenderOrigin().x;
						i[1][3] -= entity->GetRenderOrigin().y;
						i[2][3] -= entity->GetRenderOrigin().z;
					}
				}

				return true;
			}
			else return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
			
		}
	}
	else 
		return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}



//voidzero's implementation cuz weave coders suck donkey dicks

bool __fastcall Hooked_SetupBones2(void* ecx, void* edx, matrix* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{

	static auto original = dtSetupBones.getOriginal<bool>(pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	auto entity = (IBasePlayer*)((uintptr_t)ecx - 4);

	if (entity != nullptr) {
		if (csgo->local != nullptr && entity->IsPlayer() && entity->EntIndex() == interfaces.engine->GetLocalPlayer()) {
			entity->GetAnimOverlays()[12].m_flWeight = FLT_EPSILON;
			entity->GetAnimOverlays()[3].m_flWeight = 0.f;
			entity->GetAnimOverlays()[3].m_flCycle = 0.f;
		}
	}

	return original(ecx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	/*
	
	if (csgo->DoUnload)
		return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	if (!ecx) //sanity check
		return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	else {
		auto callEntity = (IBasePlayer*)((uintptr_t)ecx - 4);


		if (callEntity == nullptr || csgo->local == nullptr || !callEntity->IsPlayer() || !callEntity->isAlive() || callEntity->IsDormant() || callEntity->GetTeam() == csgo->local->GetTeam() || !pBoneToWorldOut) {
			return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		}
		else if (callEntity->EntIndex() == interfaces.engine->GetLocalPlayer()) { //local player yes bro
			callEntity->GetAnimOverlays()[12].m_flWeight = FLT_EPSILON;
			callEntity->GetAnimOverlays()[3].m_flWeight = 0.f;
			callEntity->GetAnimOverlays()[3].m_flWeight = 0.f;
			return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		}
		
		//else if (!callEntity->GetBoneCache().Count()) {
		//	return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		//}
		//else if (pBoneToWorldOut && nMaxBones != -1) {
			//memcpy(pBoneToWorldOut, callEntity->GetBoneCache().Base(), callEntity->GetBoneCache().Count() * sizeof(matrix));
		//}
		
		else {
			return H::SetupBones(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		}
	}
	*/

}







bool __fastcall Hooked_ShouldSkipAnimFrame(void* ecx, void* edx)
{
	static auto original = dtShouldSkipAnimFrame.getOriginal<bool>();


	auto player = (IBasePlayer*)ecx;
	if (!player)
		return original(ecx);

	if (player->EntIndex() == interfaces.engine->GetLocalPlayer())
		return false;
	else
		return original(ecx);
}

void __fastcall Hooked_DoExtraBoneProcessing(IBasePlayer* player, uint32_t, c_studio_hdr* hdr, Vector* pos, Quaternion* q,
	const matrix& mat, uint8_t* bone_computed, void* context)
{
	return;
}


Vector* __fastcall Hooked_GetEyeAngles(void* ecx, void* edx)
{
	/**/
	static int* WantedReturnAddress1 = (int*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("8B CE F3 0F 10 00 8B 06 F3 0F 11 45 ? FF 90 ? ? ? ? F3 0F 10 55 ?")));//Update Animations X/Y
	static int* WantedReturnAddress2 = (int*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("F3 0F 10 55 ? 51 8B 8E ? ? ? ?"))); //Update Animations X/Y
	static int* WantedReturnAddress3 = (int*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("8B 55 0C 8B C8 E8 ? ? ? ? 83 C4 08 5E 8B E5"))); //Retarded valve fix

	//static auto oGetEyeAngles = _yes_fuck;

	if (_ReturnAddress() != WantedReturnAddress1 && _ReturnAddress() != WantedReturnAddress2 && _ReturnAddress() != WantedReturnAddress3)
		return H::GetEyeAngles(ecx);

	if (!ecx || ((IBasePlayer*)ecx)->EntIndex() != interfaces.engine->GetLocalPlayer())
		return H::GetEyeAngles(ecx);

	return &csgo->CurAngle;
}
/*
void __fastcall Hooked_UpdateClientSideAnimation(IBasePlayer* player, uint32_t)
{
	//
	if (csgo->DoUnload)
		return H::UpdateClientSideAnimation(player);

	if (!player || !csgo->local || !csgo->local->isAlive())
	{
		H::UpdateClientSideAnimation(player);
		return;
	}

	if (csgo->local == player && csgo->ShouldUpdate
		|| player != csgo->local && csgo->EnableBones)
		H::UpdateClientSideAnimation(player);
		//

	
	if (csgo->DoUnload || !player || !csgo->local || !csgo->local->isAlive() || (player->GetTeam() == csgo->local->GetTeam() && player != csgo->local)
		|| csgo->game_rules->IsFreezeTime()
		|| csgo->local->HasGunGameImmunity()
		|| csgo->local->GetFlags() & FL_FROZEN) {
		player->GetClientSideAnims() = true;
		return H::UpdateClientSideAnimation(player);
	}
	
	if (csgo->local == player) {
	
		if (csgo->ShouldUpdate)
			H::UpdateClientSideAnimation(player);
		else {
			
		}
	}
	else {
		if (csgo->EnableBones) 
			H::UpdateClientSideAnimation(player);
	}
	
	

}*/


void __fastcall Hooked_UpdateClientSideAnimation(IBasePlayer* player, uint32_t)
{
	static auto original = dtUpdateClientSideAnimation.getOriginal<void>();


	if (csgo->DoUnload || !player || !csgo->local || !csgo->local->isAlive() || (!player->isEnemy(csgo->local) && player != csgo->local)
		|| csgo->game_rules->IsFreezeTime()
		|| csgo->local->HasGunGameImmunity()
		|| csgo->local->GetFlags() & FL_FROZEN) {
		player->GetClientSideAnims() = true;
		return original(player);
	}

	if (csgo->local == player) {
		if (csgo->ShouldUpdate)
			original(player);
		else {

			//player->GetPlayerAnimState()->m_flFootYaw = csgo->last_gfy;
			//player->SetAbsAngles(Vector(0, csgo->last_gfy, 0));

			/*
			std::array<float, 24> poses = {};
			CAnimationLayer layers[13] = {};

			// backup old data
			player->ParseAnimOverlays(layers);
			player->ParsePoseParameter(poses);

			// set last recieved data on bones
			player->SetAnimOverlays(csgo->layers);
			player->SetPoseParameter(csgo->poses);

			// apply foot angle on bones
			player->GetPlayerAnimState()->m_flFootYaw = csgo->last_gfy;
			player->SetAbsAngles(Vector(0, csgo->last_gfy, 0));
			player->SetupBones(nullptr, -1, 0x0000FF00, interfaces.global_vars->curtime);
			//H::UpdateClientSideAnimation(player);

			// restore old data so it won't fuck up
			player->SetAnimOverlays(layers);
			player->SetPoseParameter(poses);
			*/
		}
	}
	else {
		if (csgo->EnableBones)
			original(player);
	}

}

//void __fastcall Hooked_DoAnimationEvent(void* ecx, void* edx, int animEvent, int data) {
//	auto state = (CCSGOPlayerAnimState*)ecx;
//	if (!state || !state->m_pBaseEntity)
//		return H::DoAnimationEvent(ecx, animEvent, data);
//
//	auto entity = (IBasePlayer*)state->m_pBaseEntity;
//	if (entity->EntIndex() != interfaces.engine->GetLocalPlayer())
//		return H::DoAnimationEvent(ecx, animEvent, data);
//
//	if (csgo->in_cm)
//		csgo->animQueue.emplace_back(animEvent);
//
//	H::DoAnimationEvent(ecx, animEvent, data);
//}