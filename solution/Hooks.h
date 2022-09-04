#include "Math.h"
#include "render.h"
#include "interfaces.h"
#include "VMT_Manager.h"
#include "Hooks/MinHook.h"
#include "Variables.h"
#include "Chams.h"
#include "Glow.h"
#include "ParticleCollection.h"
#include "MinHook/MinHook.h"



#pragma comment(lib, "ntdll.lib")

extern "C" NTSYSAPI NTSTATUS NTAPI NtWriteVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	IN PVOID Buffer,
	IN ULONG NumberOfBytesToWrite,
	OUT PULONG NumberOfBytesWritten OPTIONAL
);

class c_studio_hdr;

#pragma region Typedefs
typedef void(__thiscall* ModifyEyePosFn)(CCSGOPlayerAnimState*, const Vector&);
typedef void(__thiscall* PaintTraverseFn)(void*, unsigned int, bool, bool);
using SendDatagramFn = int(__thiscall*)(void*, void*);
typedef void(__stdcall* FrameStageNotifyFn)(ClientFrameStage_t);
typedef bool(__thiscall* CreateMoveFn)(void*, float, CUserCmd*);
typedef long(__stdcall* EndSceneFn)(IDirect3DDevice9*);
typedef long(__stdcall* ResetFn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef void(__fastcall* SceneEndFn)(void*, void*);
typedef bool(__thiscall* DoPostScreenEffectsFn)(void*, CViewSetup*);
typedef void(__thiscall* BeginFrameFn)(void*);
typedef bool(__thiscall* temp_entities_fn)(void*, void*);
using ProcessPacket_t = void(__thiscall*)(void*, void*, bool);
typedef bool(__thiscall* FireEventClientSide)(void*, IGameEvent*);
typedef bool(*LineGoesThroughSmokeFn) (Vector, Vector, int16_t);
typedef void(__stdcall* OverRideViewFn)(void*);
typedef void(__thiscall* fnDrawModel)(void*, void*, const DrawModelInfo_t&, matrix*, float*, float*, const Vector&, int);
typedef void(__fastcall* HudDeathNotifyFn)(PVOID, IGameEvent*);
typedef void(__thiscall* DrawModelExecuteFn)(void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix*);
typedef bool(__thiscall* SvCheatsGetBoolFn)(void*);
typedef float(__thiscall* AspectRatioFn)(void* pEcx, int32_t iWidth, int32_t iHeight);
using TraceRayFn = void(__thiscall*)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
typedef bool(__thiscall* InPredictionFn)(CPrediction*);
typedef void(__thiscall* fire_event_t)(IEngineClient*);
typedef void(__thiscall* fnDrawModel)(void*, void*, const DrawModelInfo_t&, matrix*, float*, float*, const Vector&, int);
typedef void(__thiscall* fnCamera)();
typedef void(__thiscall* RunCommandFn)(void*, IBasePlayer*, CUserCmd*, IMoveHelper*);
typedef float(__stdcall* GetViewModelFOVFn)();
typedef int(__thiscall* ListLeavesInBoxFn)(void*, const Vector&, const Vector&, unsigned short*, int);
typedef long(__stdcall* PresentFn)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(__stdcall* steamoverlay_present)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(__stdcall* steamoverlay_reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef void(__thiscall* DoExtraBoneProcessingFn)(IBasePlayer*, c_studio_hdr*, Vector*, Quaternion*, const matrix&, uint8_t*, void*);


typedef Vector* (__thiscall* GetEyeAnglesFn)(void*);

typedef void(__thiscall* StandardBlendingRulesFn)(IBasePlayer*, c_studio_hdr*, Vector*, Quaternion*, float, int);
typedef void(__fastcall* PhysicsSimulateFn)(IBasePlayer*);
typedef void(__thiscall* UpdateClientSideAnimationFn)(IBasePlayer*);
typedef bool(__fastcall* ShouldSkipAnimFrameFn)(void*);
typedef bool(__fastcall* IsUsingStaticPropDebugModesFn)();
//typedef void(__thiscall* GetColorModulationFn)(void*, float*, float*, float*);
typedef void(__thiscall* BuildTransformationsFn)(void* ecx, int a2, int a3, int a4, int a5, int a6, int a7);
typedef int32_t(__thiscall* BoxVisibleFn)(IEngineClient*, Vector&, Vector&);
typedef bool(__thiscall* IsHLTVFn)(IEngineClient*); // IsPausedFn
typedef bool(__thiscall* IsPausedFn)(void*);
typedef bool(__thiscall* IsRenderableInPVSFn)(void*, IClientRenderable*);
typedef Vector* (__thiscall* EyeAnglesFn)(IBasePlayer*);
typedef bool(__fastcall* SetupBonesFn) (void*, void*, matrix*, int, int, float);
typedef void(__thiscall* FireBulletsFn)(C_TEFireBullets*, DataUpdateType_t);
typedef bool(__thiscall* WriteUsercmdDeltaToBufferFn) (void*, int, void*, int, int, bool);
using do_animation_event_fn = void* (__thiscall*)(void*, unsigned int, int);
using send_net_msg_fn = bool(__thiscall*)(INetChannel*, INetMessage&, bool, bool);
using send_datagram_fn = int(__thiscall*)(void*, void*);
using net_showfragments_t = bool(__thiscall*)(void*);
using fnPacketStart = void(__thiscall*)(void*, int, int);
using fnPacketEnd = void(__thiscall*)(void*);
typedef void(__thiscall* ClipRayCollideableFn)(void*, const Ray_t&, uint32_t, ICollideable*, CGameTrace*);
typedef const char* (__thiscall* GetForeignFallbackFontNameFn)(void*);
typedef void(__thiscall* PaintFn)(IEngineVGui*, int);
typedef void(__thiscall* DoAnimationEventFn)(void*, int, int);
//void __fastcall hkClMove(void* ecx,void* edx, float accumulated_extra_samples, bool bFinalTick)
typedef void(__cdecl* ClMoveFn)(float, bool);
typedef int(__stdcall* EmitSoundFn)(void*, int, int, const char*, unsigned int, const char*, float, int, int, int, int, const Vector&, const Vector&, void*, bool, float, int, void*);
typedef int(__thiscall* findHudElementFn)(DWORD, const char*);
typedef int(__thiscall* getUnverifiedFileHashesFn)(void*, void*, int);
typedef int(__thiscall* canLoadThirdPartyFilesFn)(void*, void*);
typedef void(__stdcall* onscreensizechangedFN)(int, int);
typedef bool(__stdcall* DoDepthOfFieldFn)(CViewSetup&);
typedef bool(__stdcall* IsDepthOfFieldEnabledFn)();
typedef void(__thiscall* ParticleSimulationsFn)(CParticleCollection*);
#pragma endregion

#pragma region ExternClases
class CNetVarManager;

extern c_interfaces interfaces;
extern CNetVarManager netvars;
extern cStructManager* csgo;

#pragma endregion

#pragma region NameSpaces

namespace I
{
	extern void Setup();
}

namespace F
{
	extern float KillDelayTime;
	extern LineGoesThroughSmokeFn LineToSmoke;
	extern bool Shooting();
}

#pragma endregion

namespace VMT
{
	extern std::unique_ptr< MinHook  > g_pDirectXHook;
	extern std::unique_ptr< MinHook  > g_pClientStateAdd;
	extern std::unique_ptr< MinHook  > g_pPanelHook;
	extern std::unique_ptr< MinHook  > g_pClientModeHook;
	extern std::unique_ptr< MinHook  > g_pClientHook;
	extern std::unique_ptr< MinHook  > g_pRenderViewHook;
	extern std::unique_ptr< MinHook  > g_pStudioRenderHook;
	//extern std::unique_ptr< VMTHook > g_pFireEventHook;
	extern std::unique_ptr< MinHook  > g_pModelRenderHook;
	extern std::unique_ptr< MinHook  > g_pMaterialSystemHook;
	extern std::unique_ptr< MinHook  > g_pPredictHook;
	extern std::unique_ptr< MinHook  > g_pMovementHook;
	extern std::unique_ptr< MinHook  > g_pQueryHook;
	extern std::unique_ptr< MinHook  > g_pGetBoolHook;
	extern std::unique_ptr< MinHook  > g_pCameraHook;
	extern std::unique_ptr< MinHook  > g_pEngineHook;
	extern std::unique_ptr< MinHook  > g_pEngineSoundHook;
	extern std::unique_ptr< MinHook  > g_pEngineTraceHook;
	//extern std::unique_ptr< VMTHook > g_pPlayerHook;
	extern std::unique_ptr< MinHook  > g_pTEFireBullets;
	extern std::unique_ptr< MinHook  > g_pNetShowFragments;
	//extern std::unique_ptr< VMTHook > g_pLeafSystemHook;
	extern std::unique_ptr< MinHook  > g_pVGUIHook;
	extern std::unique_ptr< MinHook  > g_pFileSystemHook;
	extern std::unique_ptr< MinHook  > g_pSurfaceHook;
	extern std::unique_ptr< MinHook  > g_pLocalizeHook;
	extern std::unique_ptr<MinHook> g_pViewRenderHook;
	extern std::unique_ptr<MinHook> g_gameTypeHook;

	extern MinHook dtColorModulation;
	extern MinHook dtUpdateClientSideAnimation;
	extern MinHook dtDoExtraBoneProcessing;
	extern MinHook dtShouldSkipAnimFrame;
	extern MinHook dtStandardBlendingRules;
	extern MinHook dtModifyEyePos;
	extern MinHook physSim;
	extern MinHook dtSetupBones;
	extern MinHook dtIsUsingStaticPropDebugModes;
	extern MinHook dwdoDepthOfField;
	extern MinHook dwisDepthOfFieldEnabled;
	extern MinHook dtPerfScreenOverlay;
	extern MinHook dtCZip_UnkLoadFiles;
	extern MinHook dtParticleSimulations;
	extern MinHook dtTier0_Msg;
	extern MinHook dtReinitPredictables;
}
using namespace VMT;

namespace H
{
	/*extern PaintTraverseFn PaintTraverse;
	extern CreateMoveFn CreateMove;
	extern FrameStageNotifyFn FrameStageNotify;
	extern EndSceneFn EndScene;
	extern ResetFn Reset;
	extern SceneEndFn SceneEnd;
	extern fire_event_t FireEvent;
	extern DoPostScreenEffectsFn DoPostScreenEffects;
	extern FireEventClientSide FireEventClientSideNine;
	extern OverRideViewFn OverRideView;
	extern BeginFrameFn BeginFrame;
	extern HudDeathNotifyFn HudDeathNotify;
	extern DrawModelExecuteFn DrawModelExecute;
	extern TraceRayFn TraceRay;

	extern SvCheatsGetBoolFn SvCheatsGetBool;
	extern InPredictionFn InPrediction;
	extern fnDrawModel DrawModel;
	extern fnCamera Camera;

	extern ListLeavesInBoxFn ListLeavesInBox;
	extern PresentFn Present;




	extern BoxVisibleFn BoxVisible;
	extern IsHLTVFn IsHLTV;
	extern do_animation_event_fn DoAnimEvent;
	extern IsRenderableInPVSFn IsRenderableInPVS;
	extern EyeAnglesFn EyeAngles;

	extern FireBulletsFn FireBullets;
	extern net_showfragments_t NetShowFragments;
	extern SetupBonesFn SetupBones;*/

	extern ModifyEyePosFn ModifyEyePos;
	extern ClMoveFn ClMove;
	extern SetupBonesFn SetupBones;
	extern send_net_msg_fn SendNetMsg;
	extern send_datagram_fn SendDatagram;
	extern temp_entities_fn TempEntities;
	extern CClientEffectRegistration* Effects;
	extern GetForeignFallbackFontNameFn GetForeignFallbackFontName;
	extern StandardBlendingRulesFn StandardBlendingRules;
	extern ShouldSkipAnimFrameFn ShouldSkipAnimFrame;
	extern DoExtraBoneProcessingFn DoExtraBoneProcessing;
	extern GetEyeAnglesFn GetEyeAngles;
	extern UpdateClientSideAnimationFn UpdateClientSideAnimation;
	extern PhysicsSimulateFn PhysicsSimulate;
	extern ClientEffectCallback ImpactCallback;
	extern steamoverlay_present SteamPresent;
	extern steamoverlay_reset SteamReset;
	extern BuildTransformationsFn BuildTransformations;
	//extern GetColorModulationFn GetColorModulation;
	extern IsUsingStaticPropDebugModesFn IsUsingStaticPropDebugModes;
	extern DoAnimationEventFn DoAnimationEvent;
	extern EmitSoundFn EmitSound;
	extern findHudElementFn findHudElement;
	extern uintptr_t hud;
	extern DoDepthOfFieldFn DoDepthOfField;
	extern IsDepthOfFieldEnabledFn IsDepthOfFieldEnabled;
	extern ParticleSimulationsFn ParticleSimulations;



	void Hook();
	void UnHook();
}

extern void Msg(string str, color_t clr);
