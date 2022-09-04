#pragma once
#include "Hooks.h"
#include "netvar_manager.h"
#include "VMT_Manager.h"

bool CCSGameRules::IsValveDS()
{
	if (!this)
		return false;

	static int valveDS = netvars.GetOffset(hs::DT_CSGameRulesProxy.s().c_str(), hs::m_bIsValveDS.s().c_str());
	return *(bool*)((uintptr_t)this + valveDS);
}

bool CCSGameRules::IsBombDropped()
{
	if (!this)
		return false;

	static int bombDropped = netvars.GetOffset(hs::DT_CSGameRulesProxy.s().c_str(), hs::m_bBombDropped.s().c_str());
	return *(bool*)((uintptr_t)this + bombDropped);
}

bool CCSGameRules::IsBombPlanted()
{
	if (!this)
		return false;
	static int bombPlanted = netvars.GetOffset(hs::DT_CSGameRulesProxy.s().c_str(), hs::m_bBombPlanted.s().c_str());
	return *(bool*)((uintptr_t)this + bombPlanted);
}

bool CCSGameRules::IsFreezeTime()
{
	if (!this)
		return false;

	static int freezeTime = netvars.GetOffset(hs::DT_CSGameRulesProxy.s().c_str(), hs::m_bFreezePeriod.s().c_str());
	return *(bool*)((uintptr_t)this + freezeTime);
}

unsigned int FindInDataMap(datamap_t *pMap, const char *name);
bool CGameTrace::DidHitWorld() const {
	return m_pEnt == interfaces.ent_list->GetClientEntity(0);
}

bool CGameTrace::DidHitNonWorldEntity() const {
	return m_pEnt != nullptr && !DidHitWorld();
}

bool IBasePlayer::ComputeHitboxSurroundingBox(Vector* mins, Vector* maxs) {
	using compute_hitbox_surrounding_box_t = bool(__thiscall*)(void*, Vector*, Vector*);
	static auto compute_hitbox_surrounding_box 
		= reinterpret_cast<compute_hitbox_surrounding_box_t>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			crypt_str("55 8B EC 83 E4 F8 81 EC 24 04 00 00 ? ? ? ? ? ?")));
	if (!compute_hitbox_surrounding_box)
		return false;

	return compute_hitbox_surrounding_box(this, mins, maxs);
}

bool& IBasePlayer::TargetSpotted() {
	return *(bool*)(uintptr_t(this) + 0x93D);
}

c_studio_hdr* IBasePlayer::GetModelPtr()
{
	//return *(c_studio_hdr**)((uintptr_t)this + 0x294C);
	return *(c_studio_hdr**)((uintptr_t)this + 0x2950);
	/*using LockStudioHdr_t = void(__thiscall*)(decltype(this));
	static auto m_studiohdr = *(DWORD*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("8B 86 ? ? ? ? 89 44 24 10 85 C0")) + 2);

	static LockStudioHdr_t LockStudioHdr = (LockStudioHdr_t)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 51 53 8B D9 56 57 8D B3 54 29 00 00 ? ? ? ? ? ?")));

	auto GetStudioHdr = [&]() {
		return *(c_studio_hdr**)((DWORD)this + m_studiohdr);
	};

	if (!GetStudioHdr())
		LockStudioHdr(this);

	return GetStudioHdr();*/
}
void IBasePlayer::ParsePoseParameter(std::array<float, 24>& param) {
	if (!this)
		return;

	std::copy(this->m_flPoseParameter().begin(), this->m_flPoseParameter().end(),
		param.begin());
}

void IBasePlayer::SetPoseParameter(std::array<float, 24> param) {
	if (!this)
		return;

	std::copy(param.begin(), param.end(),
		this->m_flPoseParameter().begin());
}

void IBasePlayer::ParseAnimOverlays(CAnimationLayer* anim) {
	if (!this)
		return;

	std::memcpy(anim, this->GetAnimOverlays(),
		(sizeof(CAnimationLayer) * this->GetNumAnimOverlays()));
}

void IBasePlayer::SetAnimOverlays(CAnimationLayer* anim) {
	if (!this)
		return;

	std::memcpy(this->GetAnimOverlays(), anim,
		(sizeof(CAnimationLayer) * this->GetNumAnimOverlays()));
}
void IBasePlayer::AttachmentHelper()
{
	using AttachmentHelperFn = void(__thiscall*)(IBasePlayer*, c_studio_hdr*);
	static AttachmentHelperFn DoAttachments = (AttachmentHelperFn)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4")));
	DoAttachments(this, this->GetModelPtr());
};

int& IBasePlayer::GetPlayerState()
{
	static auto m_iPlayerState = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), crypt_str("m_iPlayerState"));
	return *(int*)(uintptr_t(this) + m_iPlayerState);
}

Vector& IBasePlayer::origin()
{
	static auto origin = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), crypt_str("m_vecOrigin"));
	return *(Vector*)(uintptr_t(this) + origin);
}

bool& IBasePlayer::IsDefusing()
{
	static int m_bIsDefusing = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), crypt_str("m_bIsDefusing"));
	return *(bool*)(uintptr_t(this) + m_bIsDefusing);
}

bool& IBasePlayer::WaitForNoAttack()
{
	static auto m_bWaitForNoAttack = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), crypt_str("m_bWaitForNoAttack"));
	return *(bool*)(uintptr_t(this) + m_bWaitForNoAttack);
}

CBoneAccessor* IBasePlayer::GetBoneAccessor() {
	/*static auto boneAccessorAddr = *(DWORD*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("8D 81 ? ? ? ? 50 8D 84 24")) + 2);*/

	static int offset = netvars.GetOffset(hs::DT_BaseAnimating.s().c_str(), hs::m_nForceBone.s().c_str());
	return (CBoneAccessor*)((uintptr_t)this + offset + 0x1C);
}

CUtlVector<matrix>& IBasePlayer::GetBoneCache() {
	//static auto m_CachedBoneData = *(DWORD*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		//hs::bone_cache.s().c_str()) + 0x2) + 0x4;
	return *(CUtlVector<matrix>*)(uintptr_t(this) + 0x2914);
}

void IBasePlayer::SetBoneCache(matrix* m) {
	auto cache = GetBoneCache();
	memcpy(cache.Base(), m, sizeof(matrix) * cache.Count());
	//this->InvalidateBoneCache();
}

__forceinline void IBasePlayer::ShootPos(Vector* vec)
{
	static auto Pattern = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::shoot_pos.s().c_str());

	typedef float*(__thiscall* ShootPosFn)(void*, Vector*);

	static auto Fn = (ShootPosFn)Pattern;

	if (!Fn)
		return;

	Fn(this, vec);
}

void IBasePlayer::UpdateVisibilityAllEntities()
{
	static DWORD callInstruction = (DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::visibility.s().c_str()); // get the instruction address

	static uintptr_t* update_visibility_all_entities = nullptr;
	if (update_visibility_all_entities == nullptr) {
		static DWORD relativeAddress = *(DWORD*)(callInstruction + 1); // read the rel32
		static DWORD nextInstruction = callInstruction + 5; // get the address of next instruction
		update_visibility_all_entities = (uintptr_t*)(nextInstruction + relativeAddress); // our function address will be nextInstruction + relativeAddress
	}
	else
		reinterpret_cast<void(__thiscall*)(void*)>(update_visibility_all_entities)(this);
}

void IBasePlayer::ModifyEyePosition(CCSGOPlayerAnimState* state, Vector &eye_pos)
{
	if (!state->m_pBaseEntity)
		return;

	if (!state->m_bLanding && state->m_flAnimDuckAmount == 0.0f)
		return;

	matrix* bones = reinterpret_cast<IBasePlayer*>(state->m_pBaseEntity)->GetBoneCache().Base();

	Vector head_pos = reinterpret_cast<IBasePlayer*>(state->m_pBaseEntity)->GetBonePos(bones, 8);

	head_pos.z += 1.7f;

	if (head_pos.z < eye_pos.z)
	{
		auto lerp = std::clamp((std::fabsf(eye_pos.z - head_pos.z) - 4.0f) * (1.0f / 6.0f), 0.0f, 1.0f);
		eye_pos.z += ((head_pos.z - eye_pos.z) * ((powf(lerp, 2) * 3.0f) - ((powf(lerp, 2) * 2.0f) * lerp)));
	}
}

Vector IBasePlayer::GetEyePosition()
{
	Vector eye_pos;

	CallVFunc<void>(this, 285, std::ref(eye_pos));

	if (this->GetPlayerAnimState())
	{
		this->ModifyEyePosition(this->GetPlayerAnimState(), eye_pos);
	}

	return eye_pos;
}

void IBasePlayer::DrawServerHitboxes() {
	auto duration = interfaces.global_vars->interval_per_tick * 2.0f;

	auto serverPlayer = GetServerEdict();
	if (serverPlayer) {
		static auto pCall = (uintptr_t*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::server_dll.s().c_str())]().c_str()),
			hs::server_hitbox.s().c_str()));
		float fDuration = duration;

		__asm
		{
			pushad
			movss xmm1, fDuration
			push 1 //bool monoColor
			mov ecx, serverPlayer
			call pCall
			popad
		}
	}
}
__forceinline uint8_t* IBasePlayer::GetServerEdict() {
	static uintptr_t pServerGlobals = **(uintptr_t**)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::server_dll.s().c_str())]().c_str()),
		hs::server_edict.s().c_str()) + 0x2);
	int iMaxClients = *(int*)((uintptr_t)pServerGlobals + 0x18);
	int index = GetIndex();
	if (index > 0 && iMaxClients >= 1) {
		if (index <= iMaxClients) {
			int v10 = index * 16;
			uintptr_t v11 = *(uintptr_t*)(pServerGlobals + 96);
			if (v11) {
				if (!((*(uintptr_t*)(v11 + v10) >> 1) & 1)) {
					uintptr_t v12 = *(uintptr_t*)(v10 + v11 + 12);
					if (v12) {
						uint8_t* pReturn = nullptr;

						// abusing asm is not good
						__asm
						{
							pushad
							mov ecx, v12
							mov eax, dword ptr[ecx]
							call dword ptr[eax + 0x14]
							mov pReturn, eax
							popad
						}

						return pReturn;
					}
				}
			}
		}
	}
	return nullptr;
}

void IBasePlayer::InvalidateBoneCache()
{
	static DWORD addr = (DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::invalidate_bone_cache.s().c_str());

	[&] { // anti eblan always on

		*reinterpret_cast<int*>((uintptr_t)this + 0xA30) = interfaces.global_vars->framecount;
		*reinterpret_cast<int*>((uintptr_t)this + 0xA28) = 0;

		unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
		*reinterpret_cast<unsigned int*>((DWORD)this + 0x2928) = 0xFF7FFFFF; //m_flLastBoneSetupTime 
		*reinterpret_cast<unsigned int*>((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); //offset_iMostRecentModelBoneCounter
		//*reinterpret_cast<unsigned int*>((DWORD)this + 0x26AC) = 0;

	}();
}

bool IBasePlayer::IsValid()
{
	if (this == nullptr
		|| !this->IsPlayer()
		|| this->IsDormant()
		|| !this->isAlive()
		|| this == csgo->local
		|| !this->isEnemy(csgo->local))
		return false;

	return true;
}

Vector& IBasePlayer::GetMins() {
	static int offset = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), hs::m_vecMins.s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

Vector& IBasePlayer::GetMaxs() {
	static int offset = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), hs::m_vecMaxs.s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

IBasePlayer* IBasePlayer::GetObserverTarget() noexcept
{
	typedef IBasePlayer*(__thiscall* GetObserverTargetFn)(void*);
	return getvfunc<GetObserverTargetFn>(this, 295)(this);
}

int IBasePlayer::GetObserverTargetHandle()
{
	static int m_hObserverTarget = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_hObserverTarget.s().c_str());
	return *(int*)((uintptr_t)this + m_hObserverTarget);
}

uint64_t IBasePlayer::getSteamID64()
{
	player_info_t info;
	if (interfaces.engine->GetPlayerInfo(this->GetIndex(), &info))
	{
		return info.steamID64;
	}
	else return 0;
}

int IBasePlayer::get_prop(std::string table, std::string prop)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	return *(int*)((DWORD)this + offset);
}

float IBasePlayer::get_prop_fl(std::string table, std::string prop)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	return *(float*)((DWORD)this + offset);
}

bool IBasePlayer::get_prop_b(std::string table, std::string prop)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	return *(bool*)((DWORD)this + offset);
}

Vector IBasePlayer::get_prop_vec(std::string table, std::string prop)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	return *(Vector*)((DWORD)this + offset);
}

void IBasePlayer::set_prop(std::string table, std::string prop, int val)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	*(int*)((DWORD)this + offset) = val;
}

void IBasePlayer::set_prop_fl(std::string table, std::string prop, float val)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	*(float*)((DWORD)this + offset) = val;
}

void IBasePlayer::set_prop_b(std::string table, std::string prop, bool val)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	*(bool*)((DWORD)this + offset) = val;
}

void IBasePlayer::set_prop_vec(std::string table, std::string prop, Vector val)
{
	static auto offset = netvars.GetOffset(table.c_str(), prop.c_str());

	*(Vector*)((DWORD)this + offset) = val;
}

Vector& IBasePlayer::GetAbsVelocity()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_vecAbsVelocity.s().c_str());
	return *(Vector*)((uintptr_t)this + offset);
}
int& IBasePlayer::GetGroundEntity()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_hGroundEntity.s().c_str());
	return *(int*)((uintptr_t)this + offset);
}

float IBasePlayer::GetOldSimulationTime()
{
	static int m_flSimulationTime = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flSimulationTime.s().c_str()) + 4;
	return *(float*)((DWORD)this + m_flSimulationTime);
}

float IBasePlayer::CameFromDormantTime()
{
	return csgo->dormant_time[GetIndex()];
}

__forceinline void IBasePlayer::SetLowerBodyYaw(float value)
{
	static int m_flLowerBodyYawTarget = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flLowerBodyYawTarget.s().c_str());
	*reinterpret_cast<float*>(uintptr_t(this) + m_flLowerBodyYawTarget) = value;
}

__forceinline void IBasePlayer::SetVelocity(Vector velocity)
{
	*reinterpret_cast<Vector*>(uintptr_t(this) + 0x110) = velocity;
}

__forceinline player_info_t IBasePlayer::GetPlayerInfo()
{
	player_info_t pinfo;
	interfaces.engine->GetPlayerInfo(GetIndex(), &pinfo);
	return pinfo;
}

__forceinline void IBasePlayer::SetFlags(int flags)
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_fFlags.s().c_str());
	*reinterpret_cast<int*>(uintptr_t(this) + offset) = flags;
}
float &IBasePlayer::HealthShotBoostExpirationTime()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flHealthShotBoostExpirationTime.s().c_str());
	return *(float*)((DWORD)this + offset);
}
void IBasePlayer::ResetAnimationState(CCSGOPlayerAnimState *state)
{
	if (!state)
		return;

	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::reset_animstate.s().c_str());
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void IBasePlayer::CreateAnimationState(CCSGOPlayerAnimState *state)
{
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, IBasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::create_animstate.s().c_str());
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

Vector IBasePlayer::GetBaseVelocity()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_vecBaseVelocity.s().c_str());
	return *reinterpret_cast<Vector*>((DWORD)this + offset);
}

void IBasePlayer::UpdateAnimationState(CCSGOPlayerAnimState *state, Vector angle)
{
	using fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
	static auto ret = reinterpret_cast<fn>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::update_animstate.s().c_str()));

	if (!ret)
		return;

	ret(state, NULL, NULL, angle.y, angle.x, NULL);
}

IBasePlayer* IBasePlayer::GetDefuser() {
	static int offset = netvars.GetOffset(crypt_str("DT_PlantedC4"), crypt_str("m_hBombDefuser"));
	DWORD defuser = *(DWORD*)((DWORD)this + offset);
	return (IBasePlayer*)interfaces.ent_list->GetClientEntityFromHandle(defuser);
}

float IBasePlayer::GetTimerLength() {
	static std::uintptr_t m_flTimerLength = netvars.GetOffset(crypt_str("DT_PlantedC4"), crypt_str("m_flTimerLength"));
	return *(float*)((std::uintptr_t)this + m_flTimerLength);
}

float IBasePlayer::GetDefuseCooldown() {
	static std::uintptr_t m_flDefuseCountDown = netvars.GetOffset(crypt_str("DT_PlantedC4"), crypt_str("m_flDefuseCountDown"));
	return *(float*)((std::uintptr_t)this + m_flDefuseCountDown);
}

float IBasePlayer::GetC4Blow() {
	static std::uintptr_t m_flC4Blow = netvars.GetOffset(crypt_str("DT_PlantedC4"), crypt_str("m_flC4Blow"));
	return *(float*)((std::uintptr_t)this + m_flC4Blow);
}

float IBasePlayer::GetBombTicking() {
	static std::uintptr_t m_bBombTicking = netvars.GetOffset(crypt_str("DT_PlantedC4"), crypt_str("m_bBombTicking"));
	return *(float*)((std::uintptr_t)this + m_bBombTicking);
}

bool IBasePlayer::IsBombDefused() {
	static std::uintptr_t m_bBombDefused = netvars.GetOffset(crypt_str("DT_PlantedC4"), crypt_str("m_bBombDefused"));
	return *(bool*)((std::uintptr_t)this + m_bBombDefused);
}

int* IBasePlayer::GetButtons()
{
	static std::uintptr_t m_nButtons = FindInDataMap(this->GetPredDescMap(), hs::m_nButtons.s().c_str());
	return (int*)((std::uintptr_t)this + m_nButtons);
}

int& IBasePlayer::GetButtonLast()
{
	static std::uintptr_t m_afButtonLast = FindInDataMap(this->GetPredDescMap(), (hs::m_afButtonLast.s().c_str()));
	return *(int*)((std::uintptr_t)this + m_afButtonLast);
}

int& IBasePlayer::GetButtonPressed()
{
	static std::uintptr_t m_afButtonPressed = FindInDataMap(this->GetPredDescMap(), (hs::m_afButtonPressed.s().c_str()));
	return *(int*)((std::uintptr_t)this + m_afButtonPressed);
}

int& IBasePlayer::GetButtonReleased()
{
	static std::uintptr_t m_afButtonReleased = FindInDataMap(this->GetPredDescMap(), (hs::m_afButtonReleased.s().c_str()));
	return *(int*)((std::uintptr_t)this + m_afButtonReleased);
}

int IBasePlayer::m_fireCount() {
	static int m_fireCount = netvars.GetOffset(crypt_str("DT_Inferno"), crypt_str("m_fireCount"));
	return *reinterpret_cast<int*>(uintptr_t(this) + m_fireCount);
}

bool* IBasePlayer::m_bFireIsBurning() {
	static int m_bFireIsBurning = netvars.GetOffset(crypt_str("DT_Inferno"), crypt_str("m_bFireIsBurning"));
	return reinterpret_cast<bool*>(uintptr_t(this) + m_bFireIsBurning);
}

int* IBasePlayer::m_fireXDelta() {
	static int m_fireXDelta = netvars.GetOffset(crypt_str("DT_Inferno"), crypt_str("m_fireXDelta"));
	return reinterpret_cast<int*>(uintptr_t(this) + m_fireXDelta);
}

int* IBasePlayer::m_fireYDelta() {
	static int m_fireYDelta = netvars.GetOffset(crypt_str("DT_Inferno"), crypt_str("m_fireYDelta"));
	return reinterpret_cast<int*>(uintptr_t(this) + m_fireYDelta);
}

int* IBasePlayer::m_fireZDelta() {
	static int m_fireZDelta = netvars.GetOffset(crypt_str("DT_Inferno"), crypt_str("m_fireZDelta"));
	return reinterpret_cast<int*>(uintptr_t(this) + m_fireZDelta);
}

bool IBasePlayer::m_bDidSmokeEffect() {
	static int m_bDidSmokeEffect = netvars.GetOffset(crypt_str("DT_SmokeGrenadeProjectile"), crypt_str("m_bDidSmokeEffect"));
	return *reinterpret_cast<bool*>(uintptr_t(this) + m_bDidSmokeEffect);
}

int IBasePlayer::m_nSmokeEffectTickBegin() {
	static int m_nSmokeEffectTickBegin = netvars.GetOffset(crypt_str("DT_SmokeGrenadeProjectile"), crypt_str("m_nSmokeEffectTickBegin"));
	return *reinterpret_cast<int*>(uintptr_t(this) + m_nSmokeEffectTickBegin);
}

__forceinline int IBasePlayer::GetButtonDisabled()
{
	return *(int*)((std::uintptr_t)this + 0x3330);
}

__forceinline int IBasePlayer::GetButtonForced()
{
	return *(int*)((std::uintptr_t)this + 0x3334);
}

__forceinline void IBasePlayer::UpdateCollisionBounds()
{
	CallVFunc<void>(this, 339);
}

__forceinline void IBasePlayer::SetSequence(int iSequence)
{
	CallVFunc<void>(this, 218, iSequence);
}

__forceinline void IBasePlayer::StudioFrameAdvance()
{
	CallVFunc<void>(this, 219);
}

__forceinline int IBasePlayer::PostThink()
{
	// @ida postthink: 56 8B 35 ? ? ? ? 57 8B F9 8B CE 8B 06 FF 90 ? ? ? ? 8B 07

	using PostThinkVPhysicsFn = bool(__thiscall*)(IBasePlayer*);
	static auto oPostThinkVPhysics = (PostThinkVPhysicsFn)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::post_think.s().c_str()));

	using SimulatePlayerSimulatedEntitiesFn = void(__thiscall*)(IBasePlayer*);
	static auto oSimulatePlayerSimulatedEntities = (SimulatePlayerSimulatedEntitiesFn)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::simulate_player.s().c_str()));

	// begin lock
	CallVFunc<void>(interfaces.model_cache, 33);

	if (this->isAlive())
	{
		this->UpdateCollisionBounds();

		if (this->GetFlags() & FL_ONGROUND)
			this->GetFallVelocity() = 0.f;

		if (this->GetSequence() == -1)
			this->SetSequence(0);

		this->StudioFrameAdvance();
		oPostThinkVPhysics(this);
	}

	oSimulatePlayerSimulatedEntities(this);
	// end lock
	return CallVFunc<int>(interfaces.model_cache, 34);
}

int* IBasePlayer::GetNextThinkTick()
{
	static int nNextThinkTick = netvars.GetOffset(hs::DT_LocalPlayerExclusive.s().c_str(), hs::nNextThinkTick.s().c_str());
	return (int*)((std::uintptr_t)this + nNextThinkTick);
}

bool IBasePlayer::PhysicsRunThink(int unk01)
{
	using PhysicsRunThinkFn = bool(__thiscall*)(void*, int);
	static auto oPhysicsRunThink = (PhysicsRunThinkFn)csgo->Utils.FindPatternIDA((GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str())),
		hs::physics_run_think.s().c_str());
	return oPhysicsRunThink(this, unk01);
}

void IBasePlayer::SetAbsAngles(const Vector &angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const Vector &angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::set_abs_angles.s().c_str());
	SetAbsAngles(this, angles);
}

void IBasePlayer::UnkFunc()
{
	static auto fn = reinterpret_cast<void(__thiscall*)(int)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::unk_func.s().c_str()));
	fn(0);
}

void IBasePlayer::ForceBoneCache() {
	static auto bone_counter = *(int**)(uintptr_t(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("FF 05 ? ? ? ? FF 05 ? ? ? ? ")) + 2));
	*(int*)(uintptr_t(this) + 0x2690) = *bone_counter; //55 8B EC 53 56 57 8B F9 8B 87 ? ? ? ? 3B 
}

void IBasePlayer::SetAbsOrigin(const Vector &origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector &origin);
	static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::set_abs_origin.s().c_str());
	SetAbsOrigin(this, origin);
}

float_t IBasePlayer::m_surfaceFriction()
{
	static unsigned int _m_surfaceFriction = FindInDataMap(GetPredDescMap(), hs::m_surfaceFriction.s().c_str());
	return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
}
void IBasePlayer::SetLocalViewAngles(Vector angle) {
	typedef void(__thiscall *original)(void*, Vector&);
	return getvfunc<original>(this, 372)(this, angle);
}

datamap_t *IBasePlayer::GetDataDescMap()
{
	typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
	return getvfunc<o_GetPredDescMap>(this, 15)(this);
}
//VFUNC(set_local_view_angles(QAngle& angle), 372, void(__thiscall*)(void*, QAngle&), angle)
datamap_t *IBasePlayer::GetPredDescMap()
{
	typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
	return getvfunc<o_GetPredDescMap>(this, 17)(this);
}

bool IBasePlayer::IsWeapon()
{
	//return getvfunc<bool*(__thiscall*)(void*)>(this, 165)(this);
	typedef bool (__thiscall* Fn)(void*);
	return (getvfunc<Fn>(this, 165))(this);
}

std::array<float, 24> &IBasePlayer::m_flPoseParameter()
{
	static int pos_par_hh = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flPoseParameter.s().c_str());
	return *reinterpret_cast<std::array<float, 24>*>(reinterpret_cast<uintptr_t>(this) + pos_par_hh);
}

/*
#pragma optimize( "", off)
#pragma runtime_checks("", off) //Disable runtime checks to prevent ESP error
void IBasePlayer::getSequenceLinearMotion(studiohdr_t* studioHdr, int sequence, Vector& v) 
{
	memory->getSequenceLinearMotion(studioHdr, sequence, (float*)&poseParameters(), &v);
}
#pragma runtime_checks("", restore) //Restore runtime checks
#pragma optimize( "", on )
*/

float IBasePlayer::SequenceDuration(int sequence) 
{
	float returnValue;
	csgo->sequenceDuration(this, sequence);
	__asm movss returnValue, xmm0;
	return returnValue;
}

float IBasePlayer::GetSequenceCycleRate(int sequence) 
{
	const float t = SequenceDuration(sequence);
	if (t > 0.0f)
		return 1.0f / t;
	else
		return 1.0f / 0.1f;
}

#pragma runtime_checks( "", off )
float IBasePlayer::GetSequenceMoveDist(studiohdr_t* pStudioHdr, int iSequence) {
	Vector vecReturn = { 0.f , 0.f, 0.f };

	static auto func = csgo->dwSeqMoveDist;
	((void(__fastcall*)(void*, int, float*, Vector*))func)(pStudioHdr, iSequence, m_flPoseParameter().data(), &vecReturn);
	return vecReturn.Length();
}
#pragma runtime_checks( "", restore )

#pragma optimize( "", off)
#pragma runtime_checks("", off) //Disable runtime checks to prevent ESP error
void IBasePlayer::getSequenceLinearMotion(c_studio_hdr* studioHdr, int sequence, Vector& v) noexcept
{
	csgo->getSequenceLinearMotion(studioHdr, sequence, (float*)&m_flPoseParameter(), &v);
}
#pragma runtime_checks("", restore) //Restore runtime checks
#pragma optimize( "", on )

float IBasePlayer::GetSequenceMoveDist_2(c_studio_hdr* studioHdr, int sequence) noexcept
{
	Vector v{};

	getSequenceLinearMotion(studioHdr, sequence, v);

	return v.Length();
}

bool IBasePlayer::GrenadeExploded()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseGrenade"), crypt_str("m_bIsLive"));
	return !(*(bool*)((DWORD)this + offset));
}

Vector& IBasePlayer::GetVecViewOffset()
{
	static int iOffset = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_vecViewOffset.s().c_str());
	return *(Vector*)((DWORD)this + iOffset);
}
float& IBasePlayer::GetDuckSpeed()
{
	static auto m_flDuckSpeed = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flDuckSpeed.s().c_str());
	return *(float*)((DWORD)this + m_flDuckSpeed);
}
const char* IBasePlayer::GetLastPlace() {
	return (const char*)((DWORD)this + 0x35B4);
}
float& IBasePlayer::GetDuckAmount()
{
	static int m_flDuckAmount = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flDuckAmount.s().c_str());
	return *(float*)((DWORD)this + m_flDuckAmount);
}
void IBasePlayer::SetObserverMode(int value) {
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_iObserverMode.s().c_str());
	*reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + offset) = value;
}

int32_t IBasePlayer::GetObserverMode()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_iObserverMode.s().c_str());
	return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + offset);
}

Vector IBasePlayer::GetEyeAngles()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_angEyeAngles.s().c_str());
	return *(Vector*)((DWORD)this + offset);
}
float &IBasePlayer::GetVelocityModifier()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flVelocityModifier.s().c_str());
	return *(float*)((DWORD)this + offset);
}
Vector* IBasePlayer::GetEyeAnglesPointer()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_angEyeAngles.s().c_str());
	return (Vector*)((DWORD)this + offset);
}
int IBasePlayer::GetSequence()
{
	static int offset = netvars.GetOffset(hs::DT_BaseAnimating.s().c_str(), hs::m_nSequence.s().c_str());
	return *(int*)((DWORD)this + offset);
}
float& IBasePlayer::GetFallVelocity()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_flFallVelocity.s().c_str());
	return *(float*)((DWORD)this + offset);
}
bool IBasePlayer::IsPlayer()
{
	return ((ClientClass*)this->GetClientClass())->m_ClassID == ClassId->CCSPlayer;
}

bool IBasePlayer::DormantWrapped()
{
	if (!IsDormant())
		return false;

	return fabsf(csgo->LastSeenTime[GetIndex()] - csgo->get_absolute_time()) > 5.f;
}

int IBasePlayer::GetTeam()
{
	if (!this)
		return -1;
	static int offset = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), hs::m_iTeamNum.s().c_str());
	return *(int*)((DWORD)this + offset);
}

int IBasePlayer::getSurvivalTeam()
{
	if (!this)
		return -1;
	static int offset = netvars.GetOffset(crypt_str("DT_CSPlayer"), crypt_str("m_nSurvivalTeam"));
	return *(int*)((DWORD)this + offset);
}

bool IBasePlayer::isEnemy(IBasePlayer* local)
{
	if (!this)
		return false;

	if (interfaces.gameType->getrealgamemode() == GAMEMODE_DANGERZONE) // if dangerzone
	{
		if (local->getSurvivalTeam() != -1) // if not solo dangerzone
		{
			return (this->getSurvivalTeam() != local->getSurvivalTeam()); // check team
		}
		else // if solo everybody = enemy
			return this != local;
	}
		

	static auto mp_teammates_are_enemies = interfaces.cvars->FindVar(crypt_str("mp_teammates_are_enemies"));

	// dm?
	if (mp_teammates_are_enemies != nullptr && mp_teammates_are_enemies->GetBool() && this->GetTeam() == local->GetTeam() && this != local)
		return true;

	if (this->GetTeam() != local->GetTeam())
		return true;

	return false;
}



float& IBasePlayer::GetFlashDuration()
{
	return *(float*)((DWORD)this + 0x10470);
}

bool IBasePlayer::IsFlashed() {
	return GetFlashDuration() > 0.f;
}

bool IBasePlayer::HaveDefuser() {
	static auto offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_bHasDefuser.s().c_str());
	return *(bool*)((DWORD)this + offset);
}

int IBasePlayer::GetArmor()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_ArmorValue.s().c_str());
	return *(int*)((DWORD)this + offset);
}

bool IBaseCombatWeapon::m_bReloadVisuallyComplete() {
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase.s().c_str(), hs::m_bReloadVisuallyComplete.s().c_str());
	return *(bool*)((DWORD)this + offset);
}

int32_t IBaseCombatWeapon::GetZoomLevel() {
	
	if (!this)
		return -1;
	static int offset = netvars.GetOffset(crypt_str("DT_WeaponCSBaseGun"), hs::m_zoomLevel.s().c_str());
	return *(int32_t*)((DWORD)this + offset);
}

int IBaseCombatWeapon::GetAmmo(bool second)
{
	const auto data = GetCSWpnData();
	if (!this || !data)
		return 0;
	static int offset1 = netvars.GetOffset(hs::DT_BaseCombatWeapon.s().c_str(), hs::m_iClip1.s().c_str());
	if (second)
	{
		return data->m_iMaxClip1;
	}
	else
		return *(int*)((DWORD)this + offset1);
}
__forceinline bool IBasePlayer::SelectItem(const char* string, int sub_type) {
	using original = bool(__thiscall*)(void*, const char*, int);
	static auto _func = (original)csgo->Utils.FindPatternIDA((GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str())),
		hs::select_item.s().c_str());
	return _func(this, string, sub_type);
}

__forceinline void IBasePlayer::StandardBlendingRules(c_studio_hdr *hdr, Vector *pos, Quaternion *q, float curtime, int boneMask)
{
	
}

__forceinline void IBasePlayer::BuildTransformations(c_studio_hdr *hdr, Vector *pos, Quaternion *q, const matrix &cameraTransform, int boneMask, BYTE *computed)
{

}

__forceinline float IBasePlayer::GetLastSeenTime() {
	static float time[65];
	float atime = csgo->get_absolute_time();
	if (!IsDormant()) {
		time[GetIndex()] = atime;
		return 0.f;
	}
	else
		return time[GetIndex()];
}
bool IBasePlayer::BadMatrix() {
	return false;
}
bool IBasePlayer::HasHelmet()
{
	static int m_ArmorValue = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_bHasHelmet.s().c_str());
	return *(bool*)(((DWORD)this + m_ArmorValue));
}

bool IBasePlayer::HeavyArmor()
{
	static int m_bHasHeavyArmor = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_bHasHeavyArmor.s().c_str());
	return *(bool*)(((DWORD)this + m_bHasHeavyArmor));
}

Vector IBasePlayer::GetAbsOriginVec()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_vecAbsOrigin.s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

Vector IBasePlayer::GetAbsOrigin()
{
	return GetOrigin();
}

int IBasePlayer::Thrower()
{
	// NETVAR(thrower, "CBaseGrenade", "m_hThrower", int)
	static int m_hThrower = netvars.GetOffset(crypt_str("DT_BaseGrenade"), crypt_str("m_hThrower"));
	return *(int*)((DWORD)this + m_hThrower);
}

float& IBasePlayer::GetSimulationTime()
{
	static int m_flSimulationTime = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flSimulationTime.s().c_str());
	return *(float*)((DWORD)this + m_flSimulationTime);
}

int& IBasePlayer::GetEFlags()
{
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_iEFlags.s().c_str());
	return *(int*)((uintptr_t)this + offset);
}

int& IBasePlayer::GetEffects() {
	static unsigned int offset = FindInDataMap(GetPredDescMap(), hs::m_fEffects.s().c_str());
	return *(int*)((uintptr_t)this + offset);
}

float IBaseCombatWeapon::LastShotTime() {
	static int m_fLastShotTime = netvars.GetOffset(hs::DT_WeaponCSBase.s().c_str(), hs::m_fLastShotTime.s().c_str());
	return *(float*)((DWORD)this + m_fLastShotTime);
}

float &IBaseCombatWeapon::GetRecoilIndex() {
	static int m_flRecoilIndex = netvars.GetOffset(hs::DT_WeaponCSBase.s().c_str(), hs::m_flRecoilIndex.s().c_str());
	return *(float*)((DWORD)this + m_flRecoilIndex);
}

float &IBaseCombatWeapon::GetAccuracyPenalty() {
	static int m_fAccuracyPenalty = netvars.GetOffset(hs::DT_WeaponCSBase.s().c_str(), hs::m_fAccuracyPenalty.s().c_str());
	return *(float*)((DWORD)this + m_fAccuracyPenalty);
}

bool IBaseCombatWeapon::StartedArming()
{
	static int offset = netvars.GetOffset(hs::DT_BaseCombatWeapon.s().c_str(), hs::m_bStartedArming.s().c_str());
	return *(bool*)((DWORD)this + offset);
}

Vector IBasePlayer::GetNetworkOrigin()
{
	static auto m_vecNetworkOrigin = FindInDataMap(GetPredDescMap(), crypt_str("m_vecNetworkOrigin"));
	return *reinterpret_cast<Vector*>(uintptr_t(this) + m_vecNetworkOrigin);
}

bool IBasePlayer::HasGunGameImmunity()
{
	if (!this)
		return false;
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_bGunGameImmunity.s().c_str());
	return *(bool*)((DWORD)this + offset);
}

float& IBasePlayer::GetMaxSpeed()
{
	static int m_flMaxSpeed = netvars.GetOffset(crypt_str("DT_BasePlayer"), crypt_str("m_flMaxSpeed"));
	return *(float*)(uintptr_t(this) + m_flMaxSpeed);
}

bool& IBasePlayer::IsDucking()
{
	static auto m_bDucking = netvars.GetOffset(crypt_str("DT_CSPlayer"), crypt_str("m_bDucking"));
	return *(bool*)(uintptr_t(this) + m_bDucking);
}

bool& IBasePlayer::IsWalking()
{
	static int m_bIsWalking = netvars.GetOffset(crypt_str("DT_CSPlayer"), crypt_str("m_bIsWalking"));
	return *(bool*)(uintptr_t(this) + m_bIsWalking);
}

int& IBasePlayer::GetMoveState()
{
	static int m_iMoveState = netvars.GetOffset(crypt_str("DT_CSPlayer"), crypt_str("m_iMoveState"));
	return *(int*)(uintptr_t(this) + m_iMoveState);
}

float& IBasePlayer::GetThirdpersonRecoil()
{
	static auto m_flThirdpersonRecoil = netvars.GetOffset(crypt_str("DT_CSPlayer"), crypt_str("m_flThirdpersonRecoil"));
	return *(float*)(uintptr_t(this) + m_flThirdpersonRecoil);
}

DWORD IBasePlayer::GetLeaderHandle()
{
	static auto m_leader = netvars.GetOffset(crypt_str("DT_CChicken"), crypt_str("m_leader"));
	return *(DWORD*)(uintptr_t(this) + m_leader);
}

int& IBasePlayer::ragdoll()
{
	static auto m_hRagdoll = netvars.GetOffset(crypt_str("DT_CSPlayer"), crypt_str("m_hRagdoll"));
	return *(int*)(uintptr_t(this) + m_hRagdoll);
}

bool IBasePlayer::SetupBonesFixed(matrix* out, int maxBones, int boneMask, float currentTime) 
{
	Vector absOrigin =  GetAbsOrigin();
	int backupEffects = GetEffects();
	int* render = reinterpret_cast<int*>(this + 0x274);
	int backup = *render;

	*reinterpret_cast<int*>(this + 0xA28) = 0;
	*reinterpret_cast<int*>(this + 0xA30) = interfaces.global_vars->framecount;
	this->InvalidateBoneCache();
	GetEffects() |= 8;
	*render = 0;

	SetAbsOrigin(origin());

	auto res = SetupBones(out, maxBones, boneMask, currentTime);

	SetAbsOrigin(absOrigin);

	GetEffects() = backupEffects;
	*render = backup;
	return res;
}

bool IBasePlayer::SetupBones(matrix* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	void* pRenderable = reinterpret_cast<void*>(uintptr_t(this) + 0x4);
	if (!pRenderable)
		return false;

	typedef bool(__thiscall* Fn)(void*, matrix*, int, int, float);
	return getvfunc<Fn>(pRenderable, 13)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}

std::vector< IBaseCombatWeapon* > IBasePlayer::GetWeapons()
{
	int* m_hMyWeapons = reinterpret_cast< int* >((DWORD)this + 0x2DF8);
	std::vector< IBaseCombatWeapon* > list = {};
	for (auto i = 0; i < 64; ++i)
	{
		auto Weapon = interfaces.ent_list->GetClientEntityFromHandle(m_hMyWeapons[i]);
		if (Weapon)
		{
			list.push_back((IBaseCombatWeapon*)Weapon);
		}
	}
	return list;
}

int32_t IBaseCombatWeapon::WeaponMode()
{
	static int m_weaponMode = netvars.GetOffset(hs::DT_WeaponCSBaseGun.s().c_str(), hs::m_weaponMode.s().c_str());
	return *(int32_t*)((DWORD)this + m_weaponMode);
}

CCSWeaponInfo* IBaseCombatWeapon::GetCSWpnData()
{
	if (!this) return nullptr;
	typedef CCSWeaponInfo*(__thiscall* OriginalFn)(void*);
	return getvfunc<OriginalFn>(this, 461)(this);
}

__forceinline void IBasePlayer::SetCurrentCommand(CUserCmd* cmd)
{
	auto m_hConstraintEntity = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_hConstraintEntity.s().c_str());
	auto m_pCurrentCommand = (m_hConstraintEntity - 0xC);

	if (m_pCurrentCommand)
		*(CUserCmd**)(uintptr_t(this) + m_pCurrentCommand) = cmd;
}

int& IBaseCombatWeapon::GetWeaponWorldModel()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseCombatWeapon"), crypt_str("m_hWeaponWorldModel"));
	return *(int*)((DWORD)this + offset);
}

int& IBaseViewModel::GetOwner()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseViewModel"), crypt_str("m_hOwner"));
	return *(int*)((DWORD)this + offset);
}

int* IBasePlayer::GetWeaponsHandles()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseCombatCharacter"), crypt_str("m_hMyWeapons"));
	return (int*)((DWORD)this + offset);
}

int* IBasePlayer::GetWearables()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseCombatCharacter"), crypt_str("m_hMyWearables"));
	return (int*)((DWORD)this + offset);
}

// use this for assigning index to entity if you want to for some reason
int& IBasePlayer::index() 
{
	static int offset = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), crypt_str("m_bIsAutoaimTarget"));
	return *(int*)((DWORD)this + offset + 0x4);
}

__forceinline int IBasePlayer::GetTickBase(void)
{
	static int m_nTickBase = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_nTickBase.s().c_str());
	return *(int*)((DWORD)this + m_nTickBase);
}

int* IBasePlayer::GetImpulse()
{
	static std::uintptr_t m_nImpulse = FindInDataMap(GetPredDescMap(), hs::m_nImpulse.s().c_str());
	return (int*)((std::uintptr_t)this + m_nImpulse);
}

float& IBaseViewModel::GetAnimtime()
{
	static std::uintptr_t m_flAnimTime = FindInDataMap(GetPredDescMap(), hs::m_flAnimTime.s().c_str());
	return *(float*)((std::uintptr_t)this + m_flAnimTime);
}
float& IBasePlayer::GetAnimtime()
{
	static std::uintptr_t m_flAnimTime = FindInDataMap(GetPredDescMap(), hs::m_flAnimTime.s().c_str());
	return *(float*)((std::uintptr_t)this + m_flAnimTime);
}
float& IBasePlayer::GetCycle()
{
	static std::uintptr_t m_flCycle = FindInDataMap(GetPredDescMap(), hs::m_flCycle.s().c_str());
	return *(float*)((std::uintptr_t)this + m_flCycle);
}
float& IBaseViewModel::GetCycle()
{

	static std::uintptr_t m_flCycle = FindInDataMap(GetPredDescMap(), hs::m_flCycle.s().c_str());
	return *(float*)((std::uintptr_t)this + m_flCycle);
}

int &IBasePlayer::GetTickBasePtr(void)
{
	static int m_nTickBase = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_nTickBase.s().c_str());
	return *(int*)((DWORD)this + m_nTickBase);
}

float& IBasePlayer::GetLBY()
{
	float f = 0.f;
	if (!this)
		return f;
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_flLowerBodyYawTarget.s().c_str());
	return *(float*)((DWORD)this + offset);
}

int IBasePlayer::GetHitboxSet()
{
	static int offset = netvars.GetOffset(hs::DT_BaseAnimating.s().c_str(), crypt_str("m_nHitboxSet"));
	return *(int*)((uintptr_t)this + offset);
}

std::string IBasePlayer::GetName()
{
	return GetPlayerInfo().name;
}

float IBasePlayer::GetStamina()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), crypt_str("m_flStamina"));
	return *(float*)((uintptr_t)this + offset);
}

float IBaseCombatWeapon::GetPostponeFireReadyTime()
{
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase.s().c_str(), hs::m_flPostponeFireReadyTime.s().c_str());
	return *(float*)((uintptr_t)this + offset);
}

void IBaseCombatWeapon::SetPostPoneTime(float asdasdasd) {
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase.s().c_str(), hs::m_flPostponeFireReadyTime.s().c_str());
	*(float*)((uintptr_t)this + offset) = asdasdasd;
}

bool IBasePlayer::IsScoped()
{
	if (!this)
		return false;
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_bIsScoped.s().c_str());
	return *(bool*)((uintptr_t)this + offset);
}

int IBasePlayer::GetSequenceActivity(int sequence)
{
	auto model = this->GetModel();
	if (!model)
		return -1;
	auto hdr = interfaces.models.model_info->GetStudioModel(model);

	if (!hdr)
		return -1;

	// c_csplayer vfunc 242, follow calls to find the function.
	static DWORD fn = NULL;

	if (!fn) // 55 8B EC 83 7D 08 FF 56 8B F1 74
		fn = (DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			hs::seq_activity.s().c_str());

	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(fn);

	return GetSequenceActivity(this, hdr, sequence);
}

matrix& IBasePlayer::GetrgflCoordinateFrame()
{
	static int offset = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), hs::m_CollisionGroup.s().c_str()) - 48;
	return *(matrix*)(this + offset);
}

IBaseViewModel* IBasePlayer::GetViewModel()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_hViewModel.s().c_str());
	DWORD ViewModelData = *(DWORD*)((DWORD)this + offset);
	return (IBaseViewModel*)interfaces.ent_list->GetClientEntityFromHandle(ViewModelData);
}


CBaseHandle IBaseViewModel::GetViewmodelWeapon()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseViewModel"), crypt_str("m_hWeapon"));
	return *(CBaseHandle*)(uintptr_t(this) + offset);
}

int IBaseViewModel::GetViewModelIndex()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseViewModel"), crypt_str("m_nViewModelIndex"));
	return *(int*)(uintptr_t(this) + offset);
}

float& IBaseViewModel::GetCurrentCycle()
{
	static unsigned int m_flCycle = FindInDataMap(GetPredDescMap(), "m_flCycle");
	return *(float*)(uintptr_t(this) + m_flCycle);
}

float& IBaseViewModel::GetModelAnimTime()
{
	static unsigned int m_flAnimTime = FindInDataMap(GetPredDescMap(), "m_flAnimTime");
	return *(float*)(uintptr_t(this) + m_flAnimTime);
}

int& IBaseViewModel::GetCurrentSequence()
{
	static unsigned int m_nSequence = FindInDataMap(GetPredDescMap(), "m_nSequence");
	return *(int*)(uintptr_t(this) + m_nSequence);
}

int& IBaseViewModel::GetAnimationParity()
{
	static int m_nAnimationParity = netvars.GetOffset(crypt_str("DT_BaseViewModel"), crypt_str("m_nAnimationParity"));
	return *(int*)(uintptr_t(this) + m_nAnimationParity);
}

IBaseCombatWeapon* IBasePlayer::GetWeapon()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_hActiveWeapon.s().c_str());
	DWORD weaponData = *(DWORD*)((DWORD)this + offset);
	return (IBaseCombatWeapon*)interfaces.ent_list->GetClientEntityFromHandle(weaponData);
}

IBaseCombatWeapon* IBasePlayer::GetWeaponWorldModel()
{
	static int offset = netvars.GetOffset(hs::DT_BaseCombatWeapon.s().c_str(), crypt_str("m_hWeaponWorldModel"));
	int weaponData = *(int*)((DWORD)this + offset);
	return (IBaseCombatWeapon*)interfaces.ent_list->GetClientEntityFromHandle(weaponData);
}

unsigned& IBasePlayer::GetModelIndex()
{
	static int offset = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), crypt_str("m_nModelIndex"));
	return *(unsigned*)((DWORD)this + offset);
}

Vector IBasePlayer::GetAimPunchAngle()
{
	return *(Vector*)((DWORD)this + 0x303C);
}

Vector IBasePlayer::GetPunchAngle()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_aimPunchAngle.s().c_str());
	return *(Vector*)((DWORD)this + offset);
}
Vector& IBasePlayer::GetPunchAngleVel()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_aimPunchAngleVel.s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

__forceinline Vector* IBasePlayer::GetPunchAnglePtr()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_aimPunchAngle.s().c_str());
	return (Vector*)((DWORD)this + offset);
}

Vector IBasePlayer::GetViewPunchAngle()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_viewPunchAngle.s().c_str());
	return *(Vector*)((DWORD)this + offset);
}

Vector* IBasePlayer::GetViewPunchAnglePtr()
{
	static int offset = netvars.GetOffset(hs::DT_BasePlayer.s().c_str(), hs::m_viewPunchAngle.s().c_str());
	return (Vector*)((DWORD)this + offset);
}

__forceinline bool IBasePlayer::IsChicken()
{
	if (!this)
	{
		return false;
	}
	ClientClass* cClass = (ClientClass*)this->GetClientClass();

	return cClass->m_ClassID == ClassId->CChicken;
}

Vector& IBasePlayer::GetAbsAngles()
{
	typedef Vector& (__thiscall* Fn)(void*);
	return (getvfunc<Fn>(this, 11))(this);
}
Vector& IBasePlayer::GetAbsOriginVirtual()
{
	typedef Vector& (__thiscall* Fn)(void*);
	return (getvfunc<Fn>(this, 10))(this);
}
__forceinline float IBasePlayer::m_angAbsRotation()
{
	static int offset = netvars.GetOffset(hs::DT_CSPlayer.s().c_str(), hs::m_angAbsRotation.s().c_str());
	return *(float*)((DWORD)this + offset);
}

__forceinline bool IBasePlayer::IsHostage()
{
	if (!this)
	{
		return false;
	}
	ClientClass* cClass = (ClientClass*)this->GetClientClass();

	return cClass->m_ClassID == ClassId->CHostage;
}

__forceinline Vector IBasePlayer::GetRagdollPos()
{
	static int offset = netvars.GetOffset(hs::DT_Ragdoll.s().c_str(), hs::m_ragPos.s().c_str());
	return *reinterpret_cast<Vector*>(uintptr_t(this) + offset);
}

bool& IBasePlayer::GetClientSideAnims()
{
	static int offset = netvars.GetOffset(hs::DT_BaseAnimating.s().c_str(), hs::m_bClientSideAnimation.s().c_str());
	return *reinterpret_cast<bool*>(uintptr_t(this) + offset);
}

float IBasePlayer::GetDSYDelta()
{
	const auto animState = this->GetPlayerAnimState();

	if (!animState)
		return 0.0f;

	float yawModifier = (animState->m_flWalkToRunTransition * -0.3f - 0.2f) * std::clamp(animState->m_flSpeedAsPortionOfWalkTopSpeed, 0.0f, 1.0f) + 1.0f;

	if (animState->m_flAnimDuckAmount> 0.0f)
		yawModifier += (animState->m_flAnimDuckAmount * std::clamp(animState->m_flSpeedAsPortionOfCrouchTopSpeed, 0.0f, 1.0f) * (0.5f - yawModifier));

	return std::clamp(animState->m_flAimYawMax * yawModifier, 18.f, 58.f);
}

CUtlVector< CAnimationLayer >& IBasePlayer::GetAnimOverlayVector() {
	static auto AnimOverlayVector = *(DWORD*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("8B 89 ?? ?? ?? ?? 8D 0C D1")) + 2);
	return *(CUtlVector< CAnimationLayer >*)((uintptr_t)this + AnimOverlayVector);
}

CCSGOPlayerAnimState* IBasePlayer::GetPlayerAnimState()
{
	if (!this || !this->isAlive())
		return nullptr;
	return *reinterpret_cast<CCSGOPlayerAnimState**>(uintptr_t(this) + 0x9960);
}

void IBasePlayer::InvalidatePhysicsRecursive(int32_t flags) {
	static const auto invalidate_physics_recursive = reinterpret_cast<void(__thiscall*)(IBasePlayer*, int32_t)>(
		csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), hs::physics_recursive.s().c_str()));
	invalidate_physics_recursive(this, flags);
}

void IBasePlayer::SetAnimState(CCSGOPlayerAnimState* anims) {
	*reinterpret_cast<CCSGOPlayerAnimState**>(uintptr_t(this) + 0x3914) = anims;
}

Vector& IBasePlayer::GetVecForce()
{
	static auto offset = netvars.GetOffset(crypt_str("DT_CSRagdoll"), crypt_str("m_vecForce"));
	return *(Vector*)((uintptr_t)this + offset);
}

void IBasePlayer::SetModelIndex(int index)
{
	//VirtualMethod::call<void, 75>(this, index);

	using oFnc = void(__thiscall*)(void*, int);
	(*(oFnc**)this)[75](this, index);

	//typedef void (__thiscall* oFnc)(void*, int);
	//(getvfunc<oFnc>(this, 75))(this, index);


}

bool IBasePlayer::IsVisible(Vector pos)
{
	CTraceFilter filter;
	filter.pSkip = csgo->local;
	CGameTrace tr;
	interfaces.trace->TraceRay(Ray_t(this->GetEyePosition(), pos), MASK_PLAYERSOLID, &filter, &tr);
	return tr.IsVisible();
}

bool IBasePlayer::IsVisible(IBasePlayer* to)
{
	CTraceFilter filter;
	filter.pSkip = csgo->local;
	CGameTrace tr;
	interfaces.trace->TraceRay(Ray_t(this->GetEyePosition(), to->GetEyePosition()), MASK_PLAYERSOLID, &filter, &tr);
	return tr.fraction < 1.f && tr.m_pEnt == to;
}

bool IBaseCombatWeapon::IsGun()
{
	if (!this)
		return false;

	
	auto class_id = this->GetClientClass()->m_ClassID;
	if (class_id == ClassId->CAK47 || class_id == ClassId->CDEagle || class_id == ClassId->CWeaponAug || class_id == ClassId->CWeaponAWP || class_id == ClassId->CWeaponBaseItem || class_id == ClassId->CWeaponBizon
		|| class_id == ClassId->CWeaponCSBase || class_id == ClassId->CWeaponCSBaseGun || class_id == ClassId->CWeaponCycler || class_id == ClassId->CWeaponElite || class_id == ClassId->CWeaponFamas || class_id == ClassId->CWeaponFiveSeven
		|| class_id == ClassId->CWeaponG3SG1 || class_id == ClassId->CWeaponGalil || class_id == ClassId->CWeaponGalilAR || class_id == ClassId->CWeaponGlock || class_id == ClassId->CWeaponHKP2000 || class_id == ClassId->CWeaponM249
		|| class_id == ClassId->CWeaponM3 || class_id == ClassId->CWeaponM4A1 || class_id == ClassId->CWeaponMAC10 || class_id == ClassId->CWeaponMag7 || class_id == ClassId->CWeaponMP5Navy || class_id == ClassId->CWeaponMP7
		|| class_id == ClassId->CWeaponMP9 || class_id == ClassId->CWeaponNegev || class_id == ClassId->CWeaponNOVA || class_id == ClassId->CWeaponP228 || class_id == ClassId->CWeaponP250 || class_id == ClassId->CWeaponP90
		|| class_id == ClassId->CWeaponSawedoff || class_id == ClassId->CWeaponSCAR20 || class_id == ClassId->CWeaponScout || class_id == ClassId->CWeaponSG550 || class_id == ClassId->CWeaponSG552 || class_id == ClassId->CWeaponSG556
		|| class_id == ClassId->CWeaponSSG08 || class_id == ClassId->CWeaponTaser || class_id == ClassId->CWeaponTec9 || class_id == ClassId->CWeaponTMP || class_id == ClassId->CWeaponUMP45 || class_id == ClassId->CWeaponUSP || class_id == ClassId->CWeaponXM1014)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//DT_BaseCombatWeapon

int IBaseCombatWeapon::GetItemDefinitionIndex()
{
	if (!this)
		return -1;
	static int offset = netvars.GetOffset(hs::DT_BaseAttributableItem.s().c_str(), hs::m_iItemDefinitionIndex.s().c_str());
	return *(short*)((DWORD)this + offset);
}
bool IBaseCombatWeapon::IsKnife()
{
	if (!this)
		return false;
	//int iWeaponID = this->GetItemDefinitionIndex();
	//return (iWeaponID == tknife || iWeaponID == ctknife
	//	|| iWeaponID == goldknife || iWeaponID == 59 || iWeaponID == 41
	//	|| iWeaponID == 500 || iWeaponID == 505 || iWeaponID == 506
	//	|| iWeaponID == 507 || iWeaponID == 508 || iWeaponID == 509
	//	|| iWeaponID == 515);
	const auto& classId = ((IBasePlayer*)this)->GetClientClass();
	if (!classId)
		return false;
	return classId->m_ClassID == ClassId->CKnife || classId->m_ClassID == ClassId->CKnifeGG;
}
bool IBaseCombatWeapon::IsNade()
{
	const auto& classId = this->GetClientClass();
	if (!classId)
		return false;
	if (classId->m_ClassID == ClassId->CSnowball)
		return true;
	int WeaponId = this->GetItemDefinitionIndex();

	return WeaponId == WEAPON_FLASHBANG || WeaponId == WEAPON_HEGRENADE || WeaponId == WEAPON_SMOKEGRENADE ||
		WeaponId == WEAPON_MOLOTOV || WeaponId == WEAPON_DECOY || WeaponId == WEAPON_INCGRENADE;
}
float& IBasePlayer::m_flNextAttack()
{
	return *(float*)((uintptr_t)this + 0x2D70);
}
int IBaseCombatWeapon::Owner() {
	//  NETVAR(ownerEntity, "IBasePlayer", "m_hOwnerEntity", int)
	static int offset = netvars.GetOffset(hs::DT_BaseEntity.s().c_str(), crypt_str("m_hOwnerEntity"));
	return *(int*)((DWORD)this + offset);
}

bool IBaseCombatWeapon::InReload()
{
	return *(bool*)((DWORD)this + 0x32B5);
}

float& IBaseCombatWeapon::NextSecondaryAttack()
{
	static int offset = netvars.GetOffset(hs::DT_BaseCombatWeapon.s().c_str(), hs::m_flNextSecondaryAttack.s().c_str());
	return *(float*)((DWORD)this + offset);
}

float IBaseCombatWeapon::NextPrimaryAttack()
{
	static int offset = netvars.GetOffset(hs::DT_BaseCombatWeapon.s().c_str(), hs::m_flNextPrimaryAttack.s().c_str());
	return *(float*)((DWORD)this + offset);
}

float IBaseCombatWeapon::GetLastShotTime()
{
	static int offset = netvars.GetOffset(hs::DT_WeaponCSBase.s().c_str(), hs::m_fLastShotTime.s().c_str());
	return *(float*)((DWORD)this + offset);
}

float CBaseCSGrenade::GetThrowTime()
{
	static int m_fThrowTime = netvars.GetOffset((hs::DT_BaseCSGrenade.s().c_str()), hs::m_fThrowTime.s().c_str());
	return *(float*)((uintptr_t)this + m_fThrowTime);
}

bool IBaseCombatWeapon::CanFire()
{
	static decltype(this) storedWeapon = nullptr;
	static auto storedTick = 0;

	if (storedWeapon != this || storedTick >= csgo->local->GetTickBase()) {
		storedWeapon = this;
		storedTick = csgo->local->GetTickBase();
		return false; //wait 1 tick after wep switch before attacking 
	}

	return true;
	
	/*
	if (!this)return false;

	static decltype(this) storedWeapon = nullptr;
	static auto storedTick = 0;

	if (storedWeapon != this || storedTick >= csgo->local->GetTickBase()) {
		storedWeapon = this;
		storedTick = csgo->local->GetTickBase();
		return false; //wait 1 tick after wep switch before attacking 
	}

	float nextPrimAttack = csgo->weapon->NextPrimaryAttack();
	float serverTime = TICKS_TO_TIME(csgo->local->GetTickBase());// + I::pGlobals->frametime;absoluteframetime

	if (csgo->weapon->GetAmmo(false) < 1 || csgo->weapon->InReload())
		return false;

	

	if (nextPrimAttack < serverTime)
		return true;
		*/

}

//DT_BaseAttributableItem

int& IBaseAttributableItem::GetAccountID()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_iAccountID"));
	return *(int*)((DWORD)this + offset);
}

short& IBaseAttributableItem::GetItemDefinitionIndex()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_iItemDefinitionIndex"));
	return *(short*)((DWORD)this + offset);
}

int& IBaseAttributableItem::GetItemIDHigh()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_iItemIDHigh"));
	return *(int*)((DWORD)this + offset);
}

int& IBaseAttributableItem::GetEntityQuality()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_iEntityQuality"));
	return *(int*)((DWORD)this + offset);
}

char* IBaseAttributableItem::GetCustomName()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_szCustomName"));
	return (char*)((DWORD)this + offset);
}

unsigned& IBaseAttributableItem::GetFallbackPaintKit()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_nFallbackPaintKit"));
	return *(unsigned*)((DWORD)this + offset);
}

unsigned& IBaseAttributableItem::GetFallbackSeed()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_nFallbackSeed"));
	return *(unsigned*)((DWORD)this + offset);
}

float& IBaseAttributableItem::GetFallbackWear()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_flFallbackWear"));
	return *(float*)((DWORD)this + offset);
}

unsigned& IBaseAttributableItem::GetFallBackStatTrak()
{
	static int offset = netvars.GetOffset(crypt_str("DT_BaseAttributableItem"), crypt_str("m_nFallbackStatTrak"));
	return *(unsigned*)((DWORD)this + offset);
}

#pragma region meme

unsigned int FindInDataMap(datamap_t *pMap, const char *name)
{
	while (pMap)
	{
		for (int i = 0; i<pMap->dataNumFields; i++)
		{
			if (pMap->dataDesc[i].fieldName == NULL)
				continue;

			if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
				return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

			if (pMap->dataDesc[i].fieldType == 10)
			{
				if (pMap->dataDesc[i].td)
				{
					unsigned int offset;

					if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
						return offset;
				}
			}
		}
		pMap = pMap->baseMap;
	}

	return 0;
}
#pragma endregion

ClassIdManager * ClassId = nullptr;

inline int ClassIdManager::GetClassID(const char * classname)
{
	ServerClass * serverclass = interfaces.server->GetAllServerClasses();
	int id = 0;
	while (serverclass)
	{
		if (!strcmp(serverclass->m_pNetworkName, classname))
			return id;
		serverclass = serverclass->m_pNext, id++;
	}
	return -1;
}

ClassIdManager::ClassIdManager()
{
	CAI_BaseNPC = GetClassID(crypt_str("CAI_BaseNPC"));
	CAK47 = GetClassID(crypt_str("CAK47"));
	CBaseAnimating = GetClassID(crypt_str("CBaseAnimating"));
	CBaseAnimatingOverlay = GetClassID(crypt_str("CBaseAnimatingOverlay"));
	CBaseAttributableItem = GetClassID(crypt_str("CBaseAttributableItem"));
	CBaseButton = GetClassID(crypt_str("CBaseButton"));
	CBaseCombatCharacter = GetClassID(crypt_str("CBaseCombatCharacter"));
	CBaseCombatWeapon = GetClassID(crypt_str("CBaseCombatWeapon"));
	CBaseCSGrenade = GetClassID(crypt_str("CBaseCSGrenade"));
	CBaseCSGrenadeProjectile = GetClassID(crypt_str("CBaseCSGrenadeProjectile"));
	CBaseDoor = GetClassID(crypt_str("CBaseDoor"));
	IBasePlayer = GetClassID(crypt_str("IBasePlayer"));
	CBaseFlex = GetClassID(crypt_str("CBaseFlex"));
	CBaseGrenade = GetClassID(crypt_str("CBaseGrenade"));
	CBaseParticleEntity = GetClassID(crypt_str("CBaseParticleEntity"));
	CBasePlayer = GetClassID(crypt_str("CBasePlayer"));
	CBasePropDoor = GetClassID(crypt_str("CBasePropDoor"));
	CBaseTeamObjectiveResource = GetClassID(crypt_str("CBaseTeamObjectiveResource"));
	CBaseTempEntity = GetClassID(crypt_str("CBaseTempEntity"));
	CBaseToggle = GetClassID(crypt_str("CBaseToggle"));
	CBaseTrigger = GetClassID(crypt_str("CBaseTrigger"));
	CBaseViewModel = GetClassID(crypt_str("CBaseViewModel"));
	CBaseVPhysicsTrigger = GetClassID(crypt_str("CBaseVPhysicsTrigger"));
	CBaseWeaponWorldModel = GetClassID(crypt_str("CBaseWeaponWorldModel"));
	CBeam = GetClassID(crypt_str("CBeam"));
	CBeamSpotlight = GetClassID(crypt_str("CBeamSpotlight"));
	CBoneFollower = GetClassID(crypt_str("CBoneFollower"));
	CBRC4Target = GetClassID(crypt_str("CBRC4Target"));
	CBreachCharge = GetClassID(crypt_str("CBreachCharge"));
	CBreachChargeProjectile = GetClassID(crypt_str("CBreachChargeProjectile"));
	CBreakableProp = GetClassID(crypt_str("CBreakableProp"));
	CBreakableSurface = GetClassID(crypt_str("CBreakableSurface"));
	CBumpMineProjectile = GetClassID(crypt_str("CBumpMineProjectile"));
	CC4 = GetClassID(crypt_str("CC4"));
	CCascadeLight = GetClassID(crypt_str("CCascadeLight"));
	CChicken = GetClassID(crypt_str("CChicken"));
	CColorCorrection = GetClassID(crypt_str("CColorCorrection"));
	CColorCorrectionVolume = GetClassID(crypt_str("CColorCorrectionVolume"));
	CCSGameRulesProxy = GetClassID(crypt_str("CCSGameRulesProxy"));
	CCSPlayer = GetClassID(hs::CCSPlayer.s().c_str());
	CCSPlayerResource = GetClassID(crypt_str("CCSPlayerResource"));
	CCSRagdoll = GetClassID(crypt_str("CCSRagdoll"));
	CCSTeam = GetClassID(crypt_str("CCSTeam"));
	CDangerZone = GetClassID(crypt_str("CDangerZone"));
	CDangerZoneController = GetClassID(crypt_str("CDangerZoneController"));
	CDEagle = GetClassID(crypt_str("CDEagle"));
	CDecoyGrenade = GetClassID(crypt_str("CDecoyGrenade"));
	CDecoyProjectile = GetClassID(crypt_str("CDecoyProjectile"));
	CDrone = GetClassID(crypt_str("CDrone"));
	CDronegun = GetClassID(crypt_str("CDronegun"));
	CDynamicLight = GetClassID(crypt_str("CDynamicLight"));
	CDynamicProp = GetClassID(crypt_str("CDynamicProp"));
	CEconEntity = GetClassID(crypt_str("CEconEntity"));
	CEconWearable = GetClassID(crypt_str("CEconWearable"));
	CEmbers = GetClassID(crypt_str("CEmbers"));
	CEntityDissolve = GetClassID(crypt_str("CEntityDissolve"));
	CEntityFlame = GetClassID(crypt_str("CEntityFlame"));
	CEntityFreezing = GetClassID(crypt_str("CEntityFreezing"));
	CEntityParticleTrail = GetClassID(crypt_str("CEntityParticleTrail"));
	CEnvAmbientLight = GetClassID(crypt_str("CEnvAmbientLight"));
	CEnvDetailController = GetClassID(crypt_str("CEnvDetailController"));
	CEnvDOFController = GetClassID(crypt_str("CEnvDOFController"));
	CEnvGasCanister = GetClassID(crypt_str("CEnvGasCanister"));
	CEnvParticleScript = GetClassID(crypt_str("CEnvParticleScript"));
	CEnvProjectedTexture = GetClassID(crypt_str("CEnvProjectedTexture"));
	CEnvQuadraticBeam = GetClassID(crypt_str("CEnvQuadraticBeam"));
	CEnvScreenEffect = GetClassID(crypt_str("CEnvScreenEffect"));
	CEnvScreenOverlay = GetClassID(crypt_str("CEnvScreenOverlay"));
	CEnvTonemapController = GetClassID(crypt_str("CEnvTonemapController"));
	CEnvWind = GetClassID(crypt_str("CEnvWind"));
	CFEPlayerDecal = GetClassID(crypt_str("CFEPlayerDecal"));
	CFireCrackerBlast = GetClassID(crypt_str("CFireCrackerBlast"));
	CFireSmoke = GetClassID(crypt_str("CFireSmoke"));
	CFireTrail = GetClassID(crypt_str("CFireTrail"));
	CFish = GetClassID(crypt_str("CFish"));
	CFists = GetClassID(crypt_str("CFists"));
	CFlashbang = GetClassID(crypt_str("CFlashbang"));
	CFogController = GetClassID(crypt_str("CFogController"));
	CFootstepControl = GetClassID(crypt_str("CFootstepControl"));
	CFunc_Dust = GetClassID(crypt_str("CFunc_Dust"));
	CFunc_LOD = GetClassID(crypt_str("CFunc_LOD"));
	CFuncAreaPortalWindow = GetClassID(crypt_str("CFuncAreaPortalWindow"));
	CFuncBrush = GetClassID(crypt_str("CFuncBrush"));
	CFuncConveyor = GetClassID(crypt_str("CFuncConveyor"));
	CFuncLadder = GetClassID(crypt_str("CFuncLadder"));
	CFuncMonitor = GetClassID(crypt_str("CFuncMonitor"));
	CFuncMoveLinear = GetClassID(crypt_str("CFuncMoveLinear"));
	CFuncOccluder = GetClassID(crypt_str("CFuncOccluder"));
	CFuncReflectiveGlass = GetClassID(crypt_str("CFuncReflectiveGlass"));
	CFuncRotating = GetClassID(crypt_str("CFuncRotating"));
	CFuncSmokeVolume = GetClassID(crypt_str("CFuncSmokeVolume"));
	CFuncTrackTrain = GetClassID(crypt_str("CFuncTrackTrain"));
	CGameRulesProxy = GetClassID(crypt_str("CGameRulesProxy"));
	CGrassBurn = GetClassID(crypt_str("CGrassBurn"));
	CHandleTest = GetClassID(crypt_str("CHandleTest"));
	CHEGrenade = GetClassID(crypt_str("CHEGrenade"));
	CHostage = GetClassID(crypt_str("CHostage"));
	CHostageCarriableProp = GetClassID(crypt_str("CHostageCarriableProp"));
	CIncendiaryGrenade = GetClassID(crypt_str("CIncendiaryGrenade"));
	CInferno = GetClassID(crypt_str("CInferno"));
	CInfoLadderDismount = GetClassID(crypt_str("CInfoLadderDismount"));
	CInfoMapRegion = GetClassID(crypt_str("CInfoMapRegion"));
	CInfoOverlayAccessor = GetClassID(crypt_str("CInfoOverlayAccessor"));
	CItem_Healthshot = GetClassID(crypt_str("CItem_Healthshot"));
	CItemCash = GetClassID(crypt_str("CItemCash"));
	CItemDogtags = GetClassID(crypt_str("CItemDogtags"));
	CKnife = GetClassID(crypt_str("CKnife"));
	CKnifeGG = GetClassID(crypt_str("CKnifeGG"));
	CLightGlow = GetClassID(crypt_str("CLightGlow"));
	CMaterialModifyControl = GetClassID(crypt_str("CMaterialModifyControl"));
	CMelee = GetClassID(crypt_str("CMelee"));
	CMolotovGrenade = GetClassID(crypt_str("CMolotovGrenade"));
	CMolotovProjectile = GetClassID(crypt_str("CMolotovProjectile"));
	CMovieDisplay = GetClassID(crypt_str("CMovieDisplay"));
	CParadropChopper = GetClassID(crypt_str("CParadropChopper"));
	CParticleFire = GetClassID(crypt_str("CParticleFire"));
	CParticlePerformanceMonitor = GetClassID(crypt_str("CParticlePerformanceMonitor"));
	CParticleSystem = GetClassID(crypt_str("CParticleSystem"));
	CPhysBox = GetClassID(crypt_str("CPhysBox"));
	CPhysBoxMultiplayer = GetClassID(crypt_str("CPhysBoxMultiplayer"));
	CPhysicsProp = GetClassID(crypt_str("CPhysicsProp"));
	CPhysicsPropMultiplayer = GetClassID(crypt_str("CPhysicsPropMultiplayer"));
	CPhysMagnet = GetClassID(crypt_str("CPhysMagnet"));
	CPhysPropAmmoBox = GetClassID(crypt_str("CPhysPropAmmoBox"));
	CPhysPropLootCrate = GetClassID(crypt_str("CPhysPropLootCrate"));
	CPhysPropRadarJammer = GetClassID(crypt_str("CPhysPropRadarJammer"));
	CPhysPropWeaponUpgrade = GetClassID(crypt_str("CPhysPropWeaponUpgrade"));
	CPlantedC4 = GetClassID(crypt_str("CPlantedC4"));
	CPlasma = GetClassID(crypt_str("CPlasma"));
	CPlayerResource = GetClassID(crypt_str("CPlayerResource"));
	CPointCamera = GetClassID(crypt_str("CPointCamera"));
	CPointCommentaryNode = GetClassID(crypt_str("CPointCommentaryNode"));
	CPointWorldText = GetClassID(crypt_str("CPointWorldText"));
	CPoseController = GetClassID(crypt_str("CPoseController"));
	CPostProcessController = GetClassID(crypt_str("CPostProcessController"));
	CPrecipitation = GetClassID(crypt_str("CPrecipitation"));
	CPrecipitationBlocker = GetClassID(crypt_str("CPrecipitationBlocker"));
	CPredictedViewModel = GetClassID(crypt_str("CPredictedViewModel"));
	CProp_Hallucination = GetClassID(crypt_str("CProp_Hallucination"));
	CPropCounter = GetClassID(crypt_str("CPropCounter"));
	CPropDoorRotating = GetClassID(crypt_str("CPropDoorRotating"));
	CPropJeep = GetClassID(crypt_str("CPropJeep"));
	CPropVehicleDriveable = GetClassID(crypt_str("CPropVehicleDriveable"));
	CRagdollManager = GetClassID(crypt_str("CRagdollManager"));
	CRagdollProp = GetClassID(crypt_str("CRagdollProp"));
	CRagdollPropAttached = GetClassID(crypt_str("CRagdollPropAttached"));
	CRopeKeyframe = GetClassID(crypt_str("CRopeKeyframe"));
	CSCAR17 = GetClassID(crypt_str("CSCAR17"));
	CSceneEntity = GetClassID(crypt_str("CSceneEntity"));
	CSensorGrenade = GetClassID(crypt_str("CSensorGrenade"));
	CSensorGrenadeProjectile = GetClassID(crypt_str("CSensorGrenadeProjectile"));
	CShadowControl = GetClassID(crypt_str("CShadowControl"));
	CSlideshowDisplay = GetClassID(crypt_str("CSlideshowDisplay"));
	CSmokeGrenade = GetClassID(crypt_str("CSmokeGrenade"));
	CSmokeGrenadeProjectile = GetClassID(crypt_str("CSmokeGrenadeProjectile"));
	CSmokeStack = GetClassID(crypt_str("CSmokeStack"));
	CSnowball = GetClassID(crypt_str("CSnowball"));
	CSnowballPile = GetClassID(crypt_str("CSnowballPile"));
	CSnowballProjectile = GetClassID(crypt_str("CSnowballProjectile"));
	CSpatialEntity = GetClassID(crypt_str("CSpatialEntity"));
	CSpotlightEnd = GetClassID(crypt_str("CSpotlightEnd"));
	CSprite = GetClassID(crypt_str("CSprite"));
	CSpriteOriented = GetClassID(crypt_str("CSpriteOriented"));
	CSpriteTrail = GetClassID(crypt_str("CSpriteTrail"));
	CStatueProp = GetClassID(crypt_str("CStatueProp"));
	CSteamJet = GetClassID(crypt_str("CSteamJet"));
	CSun = GetClassID(crypt_str("CSun"));
	CSunlightShadowControl = GetClassID(crypt_str("CSunlightShadowControl"));
	CSurvivalSpawnChopper = GetClassID(crypt_str("CSurvivalSpawnChopper"));
	CTablet = GetClassID(crypt_str("CTablet"));
	CTeam = GetClassID(crypt_str("CTeam"));
	CTeamplayRoundBasedRulesProxy = GetClassID(crypt_str("CTeamplayRoundBasedRulesProxy"));
	CTEArmorRicochet = GetClassID(crypt_str("CTEArmorRicochet"));
	CTEBaseBeam = GetClassID(crypt_str("CTEBaseBeam"));
	CTEBeamEntPoint = GetClassID(crypt_str("CTEBeamEntPoint"));
	CTEBeamEnts = GetClassID(crypt_str("CTEBeamEnts"));
	CTEBeamFollow = GetClassID(crypt_str("CTEBeamFollow"));
	CTEBeamLaser = GetClassID(crypt_str("CTEBeamLaser"));
	CTEBeamPoints = GetClassID(crypt_str("CTEBeamPoints"));
	CTEBeamRing = GetClassID(crypt_str("CTEBeamRing"));
	CTEBeamRingPoint = GetClassID(crypt_str("CTEBeamRingPoint"));
	CTEBeamSpline = GetClassID(crypt_str("CTEBeamSpline"));
	CTEBloodSprite = GetClassID(crypt_str("CTEBloodSprite"));
	CTEBloodStream = GetClassID(crypt_str("CTEBloodStream"));
	CTEBreakModel = GetClassID(crypt_str("CTEBreakModel"));
	CTEBSPDecal = GetClassID(crypt_str("CTEBSPDecal"));
	CTEBubbles = GetClassID(crypt_str("CTEBubbles"));
	CTEBubbleTrail = GetClassID(crypt_str("CTEBubbleTrail"));
	CTEClientProjectile = GetClassID(crypt_str("CTEClientProjectile"));
	CTEDecal = GetClassID(crypt_str("CTEDecal"));
	CTEDust = GetClassID(crypt_str("CTEDust"));
	CTEDynamicLight = GetClassID(crypt_str("CTEDynamicLight"));
	CTEEffectDispatch = GetClassID(crypt_str("CTEEffectDispatch"));
	CTEEnergySplash = GetClassID(crypt_str("CTEEnergySplash"));
	CTEExplosion = GetClassID(crypt_str("CTEExplosion"));
	CTEFireBullets = GetClassID(crypt_str("CTEFireBullets"));
	CTEFizz = GetClassID(crypt_str("CTEFizz"));
	CTEFootprintDecal = GetClassID(crypt_str("CTEFootprintDecal"));
	CTEFoundryHelpers = GetClassID(crypt_str("CTEFoundryHelpers"));
	CTEGaussExplosion = GetClassID(crypt_str("CTEGaussExplosion"));
	CTEGlowSprite = GetClassID(crypt_str("CTEGlowSprite"));
	CTEImpact = GetClassID(crypt_str("CTEImpact"));
	CTEKillPlayerAttachments = GetClassID(crypt_str("CTEKillPlayerAttachments"));
	CTELargeFunnel = GetClassID(crypt_str("CTELargeFunnel"));
	CTEMetalSparks = GetClassID(crypt_str("CTEMetalSparks"));
	CTEMuzzleFlash = GetClassID(crypt_str("CTEMuzzleFlash"));
	CTEParticleSystem = GetClassID(crypt_str("CTEParticleSystem"));
	CTEPhysicsProp = GetClassID(crypt_str("CTEPhysicsProp"));
	CTEPlantBomb = GetClassID(crypt_str("CTEPlantBomb"));
	CTEPlayerAnimEvent = GetClassID(crypt_str("CTEPlayerAnimEvent"));
	CTEPlayerDecal = GetClassID(crypt_str("CTEPlayerDecal"));
	CTEProjectedDecal = GetClassID(crypt_str("CTEProjectedDecal"));
	CTERadioIcon = GetClassID(crypt_str("CTERadioIcon"));
	CTEShatterSurface = GetClassID(crypt_str("CTEShatterSurface"));
	CTEShowLine = GetClassID(crypt_str("CTEShowLine"));
	CTesla = GetClassID(crypt_str("CTesla"));
	CTESmoke = GetClassID(crypt_str("CTESmoke"));
	CTESparks = GetClassID(crypt_str("CTESparks"));
	CTESprite = GetClassID(crypt_str("CTESprite"));
	CTESpriteSpray = GetClassID(crypt_str("CTESpriteSpray"));
	CTest_ProxyToggle_Networkable = GetClassID(crypt_str("CTest_ProxyToggle_Networkable"));
	CTestTraceline = GetClassID(crypt_str("CTestTraceline"));
	CTEWorldDecal = GetClassID(crypt_str("CTEWorldDecal"));
	CTriggerPlayerMovement = GetClassID(crypt_str("CTriggerPlayerMovement"));
	CTriggerSoundOperator = GetClassID(crypt_str("CTriggerSoundOperator"));
	CVGuiScreen = GetClassID(crypt_str("CVGuiScreen"));
	CVoteController = GetClassID(crypt_str("CVoteController"));
	CWaterBullet = GetClassID(crypt_str("CWaterBullet"));
	CWaterLODControl = GetClassID(crypt_str("CWaterLODControl"));
	CWeaponAug = GetClassID(crypt_str("CWeaponAug"));
	CWeaponAWP = GetClassID(crypt_str("CWeaponAWP"));
	CWeaponBaseItem = GetClassID(crypt_str("CWeaponBaseItem"));
	CWeaponBizon = GetClassID(crypt_str("CWeaponBizon"));
	CWeaponCSBase = GetClassID(crypt_str("CWeaponCSBase"));
	CWeaponCSBaseGun = GetClassID(crypt_str("CWeaponCSBaseGun"));
	CWeaponCycler = GetClassID(crypt_str("CWeaponCycler"));
	CWeaponElite = GetClassID(crypt_str("CWeaponElite"));
	CWeaponFamas = GetClassID(crypt_str("CWeaponFamas"));
	CWeaponFiveSeven = GetClassID(crypt_str("CWeaponFiveSeven"));
	CWeaponG3SG1 = GetClassID(crypt_str("CWeaponG3SG1"));
	CWeaponGalil = GetClassID(crypt_str("CWeaponGalil"));
	CWeaponGalilAR = GetClassID(crypt_str("CWeaponGalilAR"));
	CWeaponGlock = GetClassID(crypt_str("CWeaponGlock"));
	CWeaponHKP2000 = GetClassID(crypt_str("CWeaponHKP2000"));
	CWeaponM249 = GetClassID(crypt_str("CWeaponM249"));
	CWeaponM3 = GetClassID(crypt_str("CWeaponM3"));
	CWeaponM4A1 = GetClassID(crypt_str("CWeaponM4A1"));
	CWeaponMAC10 = GetClassID(crypt_str("CWeaponMAC10"));
	CWeaponMag7 = GetClassID(crypt_str("CWeaponMag7"));
	CWeaponMP5Navy = GetClassID(crypt_str("CWeaponMP5Navy"));
	CWeaponMP7 = GetClassID(crypt_str("CWeaponMP7"));
	CWeaponMP9 = GetClassID(crypt_str("CWeaponMP9"));
	CWeaponNegev = GetClassID(crypt_str("CWeaponNegev"));
	CWeaponNOVA = GetClassID(crypt_str("CWeaponNOVA"));
	CWeaponP228 = GetClassID(crypt_str("CWeaponP228"));
	CWeaponP250 = GetClassID(crypt_str("CWeaponP250"));
	CWeaponP90 = GetClassID(crypt_str("CWeaponP90"));
	CWeaponSawedoff = GetClassID(crypt_str("CWeaponSawedoff"));
	CWeaponSCAR20 = GetClassID(crypt_str("CWeaponSCAR20"));
	CWeaponScout = GetClassID(crypt_str("CWeaponScout"));
	CWeaponSG550 = GetClassID(crypt_str("CWeaponSG550"));
	CWeaponSG552 = GetClassID(crypt_str("CWeaponSG552"));
	CWeaponSG556 = GetClassID(crypt_str("CWeaponSG556"));
	CWeaponSSG08 = GetClassID(crypt_str("CWeaponSSG08"));
	CWeaponTaser = GetClassID(crypt_str("CWeaponTaser"));
	CWeaponTec9 = GetClassID(crypt_str("CWeaponTec9"));
	CWeaponTMP = GetClassID(crypt_str("CWeaponTMP"));
	CWeaponUMP45 = GetClassID(crypt_str("CWeaponUMP45"));
	CWeaponUSP = GetClassID(crypt_str("CWeaponUSP"));
	CWeaponXM1014 = GetClassID(crypt_str("CWeaponXM1014"));
	CWorld = GetClassID(crypt_str("CWorld"));
	CWorldVguiText = GetClassID(crypt_str("CWorldVguiText"));
	DustTrail = GetClassID(crypt_str("DustTrail"));
	MovieExplosion = GetClassID(crypt_str("MovieExplosion"));
	ParticleSmokeGrenade = GetClassID(crypt_str("ParticleSmokeGrenade"));
	RocketTrail = GetClassID(crypt_str("RocketTrail"));
	SmokeTrail = GetClassID(crypt_str("SmokeTrail"));
	SporeExplosion = GetClassID(crypt_str("SporeExplosion"));
	SporeTrail = GetClassID(crypt_str("SporeTrail"));
}


// class fn defs for the fog entity :D
int& CFogController::enable()
{
	static int offset = netvars.GetOffset(crypt_str("DT_FogController"), crypt_str("m_fog.enable"));
	return *(int*)((DWORD)this + offset);
}


float& CFogController::start()
{
	static int offset = netvars.GetOffset(crypt_str("DT_FogController"), crypt_str("m_fog.start"));
	return *(float*)((DWORD)this + offset);
}

float& CFogController::end()
{
	static int offset = netvars.GetOffset(crypt_str("DT_FogController"), crypt_str("m_fog.end"));
	return *(float*)((DWORD)this + offset);
}


float& CFogController::density()
{
	static int offset = netvars.GetOffset(crypt_str("DT_FogController"), crypt_str("m_fog.maxdensity"));
	return *(float*)((DWORD)this + offset);
}

int& CFogController::colorPrimary()
{
	static int offset = netvars.GetOffset(crypt_str("DT_FogController"), crypt_str("m_fog.colorPrimary"));
	return *(int*)((DWORD)this + offset);
}