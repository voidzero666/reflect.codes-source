#include "Hooks.h"
#include "netvar_manager.h"
#include "binutils.h"

// Include hooks
#include "PaintTraverse.h"
#include "Createmove.h"
#include "FrameStageNotify.h"
#include "EndScene.h"
#include "WndProc.h"
#include "SceneEnd.h"
#include "DoPostScreenEffects.h"
#include "FireEventClientSide.h"
#include "BeginFrame.h"
#include "OverRideView.h"
#include "DrawModelExecuted.h"
#include "SetupAnimation.h"
#include "EngineHook.h"
#include "TraceRay.h"
#include "EmitSound.h"
#include "FileSystem.h"
#include "kitparser.hpp"
#include "vmtsmarthook.hpp"
#include "recvprophook.hpp"
#include "Recorder.h"
#include "DispatchUserMessage.h"
#include "PerfScreenOverlay.h"
#include "findIndex.h"
#include "NewFunctionsBypass.h"
#include "ParticleSimulations.h"
#include "kitparser.hpp"




ModifyEyePosFn H::ModifyEyePos;
ClMoveFn H::ClMove;
GetForeignFallbackFontNameFn H::GetForeignFallbackFontName;
send_net_msg_fn H::SendNetMsg;
send_datagram_fn H::SendDatagram;
SetupBonesFn H::SetupBones;
temp_entities_fn H::TempEntities;
ClientEffectCallback H::ImpactCallback;
CClientEffectRegistration* H::Effects;
DoExtraBoneProcessingFn H::DoExtraBoneProcessing;
GetEyeAnglesFn H::GetEyeAngles;
StandardBlendingRulesFn H::StandardBlendingRules;
ShouldSkipAnimFrameFn H::ShouldSkipAnimFrame;
UpdateClientSideAnimationFn H::UpdateClientSideAnimation;
PhysicsSimulateFn H::PhysicsSimulate;
steamoverlay_present H::SteamPresent;
steamoverlay_reset H::SteamReset;
BuildTransformationsFn H::BuildTransformations;
//GetColorModulationFn H::GetColorModulation;

IsUsingStaticPropDebugModesFn H::IsUsingStaticPropDebugModes;
DoAnimationEventFn H::DoAnimationEvent;
EmitSoundFn H::EmitSound;
findHudElementFn H::findHudElement;
uintptr_t H::hud;
DoDepthOfFieldFn H::DoDepthOfField;
IsDepthOfFieldEnabledFn H::IsDepthOfFieldEnabled;


cStructManager* csgo = new cStructManager();
features_t* features = new features_t();
c_interfaces interfaces;
std::vector<IGameEventListener2*> g_pGameEventManager;


void cStructManager::updatelocalplayer()
{
	local = interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer());
}

void Msg(string str, color_t clr)
{
	interfaces.cvars->ConsoleColorPrintf(clr, hs::prefix.s().c_str());
	interfaces.cvars->ConsoleColorPrintf(color_t(255, 255, 255), hs::_s_n.s().c_str(), str.c_str());

	if (!vars.visuals.eventlog)
		return;

	features->Eventlog->messages.push_back(CMessage(str, csgo->get_absolute_time(), clr));
}

template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
	return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

template< typename T >
T* Interface(const char* strModule, const char* strInterface)
{
	typedef T* (*CreateInterfaceFn)(const char* szName, int iReturn);
	CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(strModule), (hs::create_interface.s().c_str()));

	if (!CreateInterface)
		return nullptr;

	return CreateInterface(strInterface, 0);
}

void CSignatures::Initialize() {
	Reset = *reinterpret_cast<std::uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::gameoverlayrenderer_dll.s().c_str())]().c_str()), crypt_str("C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8")) + 9);
	Present = *reinterpret_cast<std::uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::gameoverlayrenderer_dll.s().c_str())]().c_str()), crypt_str("FF 15 ? ? ? ? 8B F0 85 FF")) + 2);
	tpfilesengine = (uintptr_t*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()), crypt_str("83 F8 02 75 6C 68 ? ? ? ? FF 15 ? ? ? ? 8B 4C 24 28 83 C4 04")));
	tpfilespanorama = (uintptr_t*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("83 F8 02 0F 85 ? ? ? ? 8B 3D ? ? ? ? FF D7")));
	csgo->registeredPanoramaEvents = reinterpret_cast<decltype(csgo->registeredPanoramaEvents)>(*(uintptr_t*)(csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("E8 ? ? ? ? A1 ? ? ? ? A8 01 75 21") + 6)) - 36);
	csgo->makePanoramaSymbolFn = relativeToAbsolute<decltype(csgo->makePanoramaSymbolFn)>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("E8 ? ? ? ? 0F B7 45 0E 8D 4D 0E")) + 1);
	csgo->lineGoesThroughSmoke = relativeToAbsolute<decltype(csgo->lineGoesThroughSmoke)>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("E8 ? ? ? ? 8B 4C 24 30 33 D2")) + 1);
	csgo->clearHudWeapon = reinterpret_cast<decltype(csgo->clearHudWeapon)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE ?")));
	csgo->keyValuesFromString = relativeToAbsolute<decltype(csgo->keyValuesFromString)>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("E8 ? ? ? ? 83 C4 04 89 45 D8")) + 1);
	csgo->keyValuesFindKey = relativeToAbsolute<decltype(csgo->keyValuesFindKey)>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("E8 ? ? ? ? F7 45")) + 1);
	csgo->keyValuesSetString = relativeToAbsolute<decltype(csgo->keyValuesSetString)>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("E8 ? ? ? ? 89 77 38")) + 1);
	csgo->UpdateScreenEffectTexture = reinterpret_cast<decltype(csgo->UpdateScreenEffectTexture)>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("55 8B EC 83 E4 F0 83 EC 48 8B 45 08 56 89 44 24 30 8B 45 0C 57 8B F9")));
	csgo->getPlayerViewmodelArmConfigForPlayerModel = relativeToAbsolute<decltype(csgo->getPlayerViewmodelArmConfigForPlayerModel)>((std::uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(hs::client_dll.s().c_str()), crypt_str("E8 ? ? ? ? 89 87 ? ? ? ? 6A")) + 1);
	csgo->dwUnlockInventory = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("84 C0 75 05 B0 01 5F"));
	csgo->tracefilterdw = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3D;

	csgo->dwSeqMoveDist = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 83 EC 0C 56 8B F1 57 8B FA 85 F6 75 14"));

	csgo->sequenceDuration = relativeToAbsolute<decltype(csgo->sequenceDuration)>((DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::seq_duration.s().c_str()) + 1);

	//csgo->tracefilterdw = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3D;

	csgo->tracefilterskip2dw = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 81 EC BC 00 00 00 56 8B F1 8B 86")) + 0x21E;

	csgo->m_IsBreakableEntity = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("55 8B EC 51 56 8B F1 85 F6 74 68"));

	csgo->ptrboneCacheInvalidation = reinterpret_cast<uint32_t>((void*)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::bone_cache_validation.s().c_str())) + 2;

	csgo->return_to_accumulate_layers = csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::accum_layers.s().c_str());


	csgo->return_to_setup_vel = csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::setup_vel.s().c_str());

	csgo->pSetClantag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>((DWORD)(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
		hs::set_clantag.s().c_str())));

	csgo->drawScreenEffectMaterial = relativeToAbsolute<uintptr_t>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("E8 ? ? ? ? 83 C4 0C 8D 4D F8")) + 1);
	csgo->transferData = reinterpret_cast<decltype(csgo->transferData)>((void*)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 8B 45 10 53 56 8B F1 57")));

	csgo->getSequenceLinearMotion = relativeToAbsolute<decltype(csgo->getSequenceLinearMotion)>((DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::get_seq_linear_motion.s().c_str()) + 1);

	csgo->createSimpleThread = reinterpret_cast<decltype(csgo->createSimpleThread)>(GetProcAddress(GetModuleHandleA("tier0.dll"), "CreateSimpleThread"));
	csgo->releaseThreadHandle = reinterpret_cast<decltype(csgo->releaseThreadHandle)>(GetProcAddress(GetModuleHandleA("tier0.dll"), "ReleaseThreadHandle"));
	game_data::itemSystem = relativeToAbsolute<decltype(game_data::itemSystem)>((DWORD)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("E8 ? ? ? ? 0F B7 0F")) + 1);
}


void I::Setup()
{
	csgo->Utils.FindPatternIDA = [](HMODULE hModule, const char* szSignature) -> uint8_t* {
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		//auto Module = GetModuleHandleA(szModule);

		auto dosHeader = (PIMAGE_DOS_HEADER)hModule;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(szSignature);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(hModule);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return nullptr;
	};

	[]() {
		g_Modules[fnva1(hs::serverbrowser_dll.s().c_str())] = hs::serverbrowser_dll.s().c_str();
		g_Modules[fnva1(hs::client_dll.s().c_str())] = hs::client_dll.s().c_str();
		g_Modules[fnva1(hs::engine_dll.s().c_str())] = hs::engine_dll.s().c_str();
		g_Modules[fnva1(hs::vstdlib_dll.s().c_str())] = hs::vstdlib_dll.s().c_str();
		g_Modules[fnva1(hs::input_system_dll.s().c_str())] = hs::input_system_dll.s().c_str();
		g_Modules[fnva1(hs::server_dll.s().c_str())] = hs::server_dll.s().c_str();
		g_Modules[fnva1(hs::vgui_mat_surface_dll.s().c_str())] = hs::vgui_mat_surface_dll.s().c_str();
		g_Modules[fnva1(hs::vgui2_dll.s().c_str())] = hs::vgui2_dll.s().c_str();
		g_Modules[fnva1(hs::mat_sys_dll.s().c_str())] = hs::mat_sys_dll.s().c_str();
		g_Modules[fnva1(hs::studio_render_dll.s().c_str())] = hs::studio_render_dll.s().c_str();
		g_Modules[fnva1(hs::physics_dll.s().c_str())] = hs::physics_dll.s().c_str();
		g_Modules[fnva1(hs::data_cache_dll.s().c_str())] = hs::data_cache_dll.s().c_str();
		g_Modules[fnva1(hs::tier0_dll.s().c_str())] = hs::tier0_dll.s().c_str();
		g_Modules[fnva1(hs::gameoverlayrenderer_dll.s().c_str())] = hs::gameoverlayrenderer_dll.s().c_str();
		g_Modules[fnva1(hs::filesystem_dll.s().c_str())] = hs::filesystem_dll.s().c_str();
	}();

	g_Patterns[fnva1(hs::client_state.s().c_str())] = hs::client_state_p.s().c_str();
	g_Patterns[fnva1(hs::move_helper.s().c_str())] = hs::move_helper_p.s().c_str();
	g_Patterns[fnva1(hs::reset.s().c_str())] = hs::reset_p.s().c_str();
	g_Patterns[fnva1(hs::input.s().c_str())] = hs::input_p.s().c_str();
	g_Patterns[fnva1(hs::glow_manager.s().c_str())] = hs::glow_manager_p.s().c_str();
	g_Patterns[fnva1(hs::beams.s().c_str())] = hs::beams_p.s().c_str();
	g_Patterns[fnva1(hs::update_clientside_anim.s().c_str())] = hs::update_clientside_anim_p.s().c_str();
	g_Patterns[fnva1(hs::debp.s().c_str())] = hs::debp_p.s().c_str();
	g_Patterns[fnva1(hs::standard_blending_rules.s().c_str())] = hs::standard_blending_rules_p.s().c_str();
	g_Patterns[fnva1(hs::should_skip_anim_frame.s().c_str())] = hs::should_skip_anim_frame_p.s().c_str();
	g_Patterns[fnva1(hs::get_foreign_fall_back_name.s().c_str())] = hs::get_foreign_fall_back_name_p.s().c_str();
	g_Patterns[fnva1(hs::setup_bones.s().c_str())] = hs::setup_bones_p.s().c_str();
	g_Patterns[fnva1(hs::build_transformations.s().c_str())] = hs::build_transformations_p.s().c_str();
	g_Patterns[fnva1(hs::effects.s().c_str())] = hs::effects_p.s().c_str();

	[]() {
		g_HookIndices[fnva1(hs::Hooked_ProcessMovement.s().c_str())] = 1;
		g_HookIndices[fnva1(hs::Hooked_ProcessPacket.s().c_str())] = 39;
		g_HookIndices[fnva1(hs::Hooked_SendDatagram.s().c_str())] = 46;
		g_HookIndices[fnva1(hs::Hooked_GetColorModulation.s().c_str())] = 47;
		g_HookIndices[fnva1(hs::Hooked_FrameStageNotify.s().c_str())] = 37;
		g_HookIndices[fnva1(hs::Hooked_WriteUsercmdDeltaToBuffer.s().c_str())] = 24;
		g_HookIndices[fnva1(hs::Hooked_IsPaused.s().c_str())] = 90;
		g_HookIndices[fnva1(hs::Hooked_IsBoxVisible.s().c_str())] = 32;
		g_HookIndices[fnva1(hs::Hooked_IsHLTV.s().c_str())] = 93;
		g_HookIndices[fnva1(hs::Hooked_GetScreenAspectRatio.s().c_str())] = 101;
		g_HookIndices[fnva1(hs::Hooked_ShouldDrawShadow.s().c_str())] = 13;
		g_HookIndices[fnva1(hs::Hooked_CreateMove.s().c_str())] = 24;
		g_HookIndices[fnva1(hs::Hooked_ShouldDrawFog.s().c_str())] = 17;
		g_HookIndices[fnva1(hs::Hooked_OverrideView.s().c_str())] = 18;
		g_HookIndices[fnva1(hs::Hooked_ViewModel.s().c_str())] = 35;
		g_HookIndices[fnva1(hs::Hooked_DoPostScreenEffects.s().c_str())] = 44;
		g_HookIndices[fnva1(hs::Hooked_PaintTraverse.s().c_str())] = 41;
		g_HookIndices[fnva1(hs::Hooked_BeginFrame.s().c_str())] = 9;
		g_HookIndices[fnva1(hs::Hooked_PacketStart.s().c_str())] = 5;
		g_HookIndices[fnva1(hs::Hooked_PacketEnd.s().c_str())] = 6;
		g_HookIndices[fnva1(hs::Hooked_DrawModelExecute.s().c_str())] = 21;
		g_HookIndices[fnva1(hs::Hooked_RunCommand.s().c_str())] = 19;
		g_HookIndices[fnva1(hs::Hooked_InPrediction.s().c_str())] = 14;
		g_HookIndices[fnva1(hs::Hooked_ListLeavesInBox.s().c_str())] = 6;
		g_HookIndices[fnva1(hs::Hooked_GetBool.s().c_str())] = 13;
		g_HookIndices[fnva1(hs::Hooked_SceneEnd.s().c_str())] = 9;
		g_HookIndices[fnva1(hs::Hooked_ClipRayColliedable.s().c_str())] = 4;
		g_HookIndices[fnva1(hs::Hooked_EmitSound.s().c_str())] = 5;
		g_HookIndices[fnva1(hs::Hooked_OverrideMouseInput.s().c_str())] = 23;
		g_HookIndices[fnva1(hs::Hooked_GetUnverifiedFileHashes.s().c_str())] = 101;
		g_HookIndices[fnva1(hs::Hooked_CanLoadThirdPartyFiles.s().c_str())] = 128;
		g_HookIndices[fnva1(hs::Hooked_OnScreenSizeChanged.s().c_str())] = 116;
		g_HookIndices[fnva1(hs::Hooked_DispatchUserMessage.s().c_str())] = 38;
		g_HookIndices[fnva1(hs::Hooked_FindIndex.s().c_str())] = 11;
	}();

	[]() {
		interfaces.client = Interface< IBaseClientDll >((
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			hs::client.s().c_str());

		interfaces.ent_list = Interface< CEntityList >((
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			hs::ent_list.s().c_str());

		interfaces.leaf_system = Interface< IClientLeafSystem >((
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			hs::leaf_sys.s().c_str());

		interfaces.cvars = Interface< ICVar >(
			g_Modules[fnva1(hs::vstdlib_dll.s().c_str())]().c_str(),
			hs::cvar.s().c_str());

		interfaces.engine = Interface< IEngineClient >(((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str())),
			hs::engine_client.s().c_str());

		interfaces.trace = Interface< IEngineTrace >(((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str())),
			hs::engine_trace.s().c_str());

		interfaces.inputsystem = Interface< IInputSystem >(
			g_Modules[fnva1(hs::input_system_dll.s().c_str())]().c_str(),
			hs::input_sys.s().c_str());

		interfaces.surfaces = Interface< ISurface >(
			g_Modules[fnva1(hs::vgui_mat_surface_dll.s().c_str())]().c_str(),
			hs::vgui_surface.s().c_str());

		interfaces.server = GetInterface<IServerGameDLL>(
			g_Modules[fnva1(hs::server_dll.s().c_str())]().c_str(),
			hs::server_game_dll.s().c_str());

		interfaces.v_panel = Interface< IVPanel >(
			g_Modules[fnva1(hs::vgui2_dll.s().c_str())]().c_str(),
			hs::vgui_panel.s().c_str());

		interfaces.models.model_render = Interface< IVModelRender >(((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str())),
			hs::engine_model.s().c_str());

		interfaces.material_system = Interface< IMaterialSystem >(
			g_Modules[fnva1(hs::mat_sys_dll.s().c_str())]().c_str(),
			hs::mat_sys.s().c_str());

		interfaces.models.model_info = Interface< IVModelInfo >(((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str())),
			hs::model_info.s().c_str());

		interfaces.render_view = Interface< IVRenderView >(((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str())),
			hs::render_view.s().c_str());

		interfaces.debug_overlay = Interface< IVDebugOverlay >(((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str())),
			hs::debug_overlay.s().c_str());

		interfaces.event_manager = Interface< IGameEventManager2 >(((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str())),
			hs::events_manager.s().c_str());

		interfaces.studio_render = Interface< IStudioRender >(
			g_Modules[fnva1(hs::studio_render_dll.s().c_str())]().c_str(),
			hs::studio_render.s().c_str());

		interfaces.phys_props = Interface< IPhysicsSurfaceProps >(
			g_Modules[fnva1(hs::physics_dll.s().c_str())]().c_str(),
			hs::phys_props.s().c_str());

		interfaces.game_movement = Interface< CGameMovement >((
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			hs::game_movement.s().c_str());

		interfaces.model_cache = Interface< IMDLCache >(
			g_Modules[fnva1(hs::data_cache_dll.s().c_str())]().c_str(),
			hs::mdl_cache.s().c_str());

		interfaces.engine_vgui = Interface<IEngineVGui>((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
			hs::engine_vgui.s().c_str());

		interfaces.engine_sound = Interface< IEngineSound >((
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
			hs::engine_sound.s().c_str());

		interfaces.move_helper = **reinterpret_cast<IMoveHelper***>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			g_Patterns[fnva1(hs::move_helper.s().c_str())]().c_str()) + 2);

		interfaces.prediction = Interface< CPrediction >((
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			hs::client_prediction.s().c_str());

		interfaces.input = reinterpret_cast<CInput*>(*reinterpret_cast<DWORD*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			g_Patterns[fnva1(hs::input.s().c_str())]().c_str()) + 1));

		interfaces.glow_manager = *reinterpret_cast<CGlowObjectManager**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			g_Patterns[fnva1(hs::glow_manager.s().c_str())]().c_str()) + 3);

		interfaces.beams = *reinterpret_cast<IViewRenderBeams**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			g_Patterns[fnva1(hs::beams.s().c_str())]().c_str()) + 1);

		interfaces.memalloc = *reinterpret_cast<IMemAlloc**>(GetProcAddress(GetModuleHandleA(
			g_Modules[fnva1(hs::tier0_dll.s().c_str())]().c_str()),
			hs::mem_alloc.s().c_str()));

		interfaces.v_file_system = Interface<IVFileSystem>((
			g_Modules[fnva1(hs::filesystem_dll.s().c_str())]().c_str()),
			hs::file_sys.s().c_str());

		interfaces.localize = Interface<sdk::ILocalize>(crypt_str("localize.dll"), crypt_str("Localize_001"));

		interfaces.gameType = Interface<sdk::IGameTypes>(crypt_str("matchmaking.dll"), crypt_str("VENGINE_GAMETYPES_VERSION002"));

		interfaces.panorama_ui_engine = Interface<PanoramaUIEngine>(hs::panorama_dll.s().c_str(), hs::panorama_ui_engine.s().c_str());

		interfaces.iv_effects = Interface<IVEffects>(hs::engine_dll.s().c_str(), crypt_str("VEngineEffects001"));

		interfaces.network_string_table_container = Interface<NetworkStringTableContainer>(
			g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str(),
			crypt_str("VEngineClientStringTable001")
			);

		interfaces.viewrender = *reinterpret_cast<CViewRender**>((uintptr_t)csgo->Utils.FindPatternIDA(GetModuleHandleA(
			g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("A1 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? FF 10")) + 1);

	}();
	interfaces.global_vars = **reinterpret_cast <global_vars_t***> ((*(DWORD**)interfaces.client)[0] + 0x1F);
	interfaces.client_mode = **reinterpret_cast<IClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[10] + 0x5u);

	CSignatures::Get().Initialize();
}

float F::KillDelayTime = NULL;
LineGoesThroughSmokeFn F::LineToSmoke = NULL;

void jews(CRecvProxyData* data)
{
	if (!vars.visuals.radarhack)
		return;

	data->m_Value.m_Int = TRUE;
}

void radarhack_fn(const CRecvProxyData* const_data, void* arg2, void* arg3)
{
	static auto original_fn = bSpottedHook->get_original_function();

	auto proxy_data = const_cast<CRecvProxyData*>(const_data);

	jews(proxy_data);

	original_fn(const_data, arg2, arg3);
}

bool F::Shooting()
{
	if (csgo->weapon->GetItemDefinitionIndex() == weapon_revolver) {
		return Ragebot::Get().m_revolver_fire && csgo->cmd->buttons & IN_ATTACK;
	}
	if (!csgo->weapon)
		return false;
	if (csgo->weapon->IsBomb())
		return false;
	else if (csgo->weapon->IsNade())
	{
		CBaseCSGrenade* csGrenade = (CBaseCSGrenade*)csgo->weapon;
		if (csGrenade->GetThrowTime() > 0.f)
		{
			return true;
		}
	}
	else if (csgo->weapon->IsKnife()) {
		if (csgo->cmd->buttons & IN_ATTACK || csgo->cmd->buttons & IN_ATTACK2 && Ragebot::Get().IsAbleToShoot())
			return true;
	}
	else if (csgo->cmd->buttons & IN_ATTACK && Ragebot::Get().IsAbleToShoot())
	{
		return true;
	}
	return false;
}

namespace VMT
{
	std::unique_ptr< MinHook > g_pDirectXHook = nullptr;
	std::unique_ptr< MinHook  > g_pClientStateAdd = nullptr;
	std::unique_ptr< MinHook  > g_pPanelHook = nullptr;
	std::unique_ptr< MinHook  > g_pClientModeHook = nullptr;
	std::unique_ptr< MinHook  > g_pClientHook = nullptr;
	std::unique_ptr< MinHook  > g_pRenderViewHook = nullptr;
	std::unique_ptr< MinHook  > g_pStudioRenderHook = nullptr;
	//std::unique_ptr< VMTHook > g_pFireEventHook = nullptr;
	std::unique_ptr< MinHook  > g_pModelRenderHook = nullptr;
	std::unique_ptr< MinHook  > g_pPredictHook = nullptr;
	std::unique_ptr< MinHook  > g_pMovementHook = nullptr;
	std::unique_ptr< MinHook  > g_pQueryHook = nullptr;
	std::unique_ptr< MinHook  > g_pGetBoolHook = nullptr;
	std::unique_ptr< MinHook  > g_pCameraHook = nullptr;
	std::unique_ptr< MinHook  > g_pEngineHook = nullptr;
	std::unique_ptr< MinHook  > g_pEngineSoundHook = nullptr;
	std::unique_ptr< MinHook  > g_pEngineTraceHook = nullptr;
	//std::unique_ptr< VMTHook > g_pPlayerHook = nullptr;
	std::unique_ptr< MinHook  > g_pLeafSystemHook = nullptr;
	//std::unique_ptr< VMTHook > g_pVGUIHook = nullptr;
	std::unique_ptr< MinHook  > g_pTEFireBullets = nullptr;
	std::unique_ptr< MinHook  > g_pNetShowFragments = nullptr;
	std::unique_ptr< MinHook  > g_pShadow = nullptr;
	std::unique_ptr< MinHook  > g_pMaterialSystemHook = nullptr;
	std::unique_ptr< MinHook  > g_pFileSystemHook = nullptr;
	std::unique_ptr< MinHook  > g_pSurfaceHook = nullptr;
	std::unique_ptr< MinHook  > g_pLocalizeHook = nullptr;
	std::unique_ptr< MinHook  > g_pViewRenderHook = nullptr;
	std::unique_ptr< MinHook  > g_gameTypeHook = nullptr;

	MinHook dtColorModulation;
	MinHook dtUpdateClientSideAnimation;
	MinHook	dtDoExtraBoneProcessing;
	MinHook	dtShouldSkipAnimFrame;
	MinHook	dtStandardBlendingRules;
	MinHook	dtModifyEyePos;
	MinHook	physSim;
	MinHook	dtSetupBones;
	MinHook	dtIsUsingStaticPropDebugModes;
	MinHook dtNewFunctionClientDLL;
	MinHook dtNewFunctionEngineDLL;
	MinHook dtNewFunctionStudioRenderDLL;
	MinHook dtNewFunctionMaterialSystemDLL;
	MinHook dtdoDepthOfField;
	MinHook dtisDepthOfFieldEnabled;
	MinHook	dtPerfScreenOverlay;
	MinHook dtCZip_UnkLoadFiles;
	MinHook dtParticleSimulations;
	MinHook dtTier0_Msg;
	MinHook dtReinitPredictables;
}

void H::Hook()
{
	MH_Initialize();



	game_data::initialize_kits();
	[]() {
		engine_prediction->m_hConstraintEntity = netvars.GetOffset(crypt_str("DT_BasePlayer"), crypt_str("m_hConstraintEntity"));
		engine_prediction->initalize();
	}();

	[]() {
		memset(csgo->key_pressed, 0, sizeof(csgo->key_pressed));
		memset(csgo->key_down, 0, sizeof(csgo->key_down));
		memset(csgo->key_pressedticks, 0, sizeof(csgo->key_pressedticks));
	}();

	[]() {
		features->Visuals = new CVisuals();
		features->Bunnyhop = new CBunnyhop();
		features->BulletTracer = new CBulletTracer();
		features->Eventlog = new CEventlog();

		ClassId = new ClassIdManager();
	}();

	csgo->client_state = **reinterpret_cast<CClientState***>(
		csgo->Utils.FindPatternIDA(
			GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
			g_Patterns[fnva1(hs::client_state.s().c_str())]().c_str()) + 1);

	void* dwUpdateClientSideAnimation = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		g_Patterns[fnva1(hs::update_clientside_anim.s().c_str())]().c_str());

	void* dwDoExtraBoneProcessing = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		g_Patterns[fnva1(hs::debp.s().c_str())]().c_str());

	/*
	csgo->postProcess = csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85")) + 0x2;
		*/


	void* dwGetEyeAngles = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), crypt_str("56 8B F1 85 F6 74 32"));

	void* dwStandardBlendingRules = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		g_Patterns[fnva1(hs::standard_blending_rules.s().c_str())]().c_str());

	void* dwShouldSkipAnimFrame = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		g_Patterns[fnva1(hs::should_skip_anim_frame.s().c_str())]().c_str());

	void* dwPhysicsSimulate = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::physicssimulate.s().c_str());

	void* dwSetupBones = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), g_Patterns[fnva1(hs::setup_bones.s().c_str())]().c_str());

	void* dwBuildTrans = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		g_Patterns[fnva1(hs::build_transformations.s().c_str())]().c_str());

	void* dwEyePos = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		hs::eye_pos.s().c_str());

	//DWORD* dwAnimEvent = (DWORD*)(csgo->Utils.FindPatternIDA(
	//	GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
	//	hs::do_anim_event.s().c_str()) + 14);

	void* dwColorModulation = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::mat_sys_dll.s().c_str())]().c_str()),
		hs::get_clr_modulation.s().c_str());

	void* dwPerfScreenOverlay = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 51 A1 ? ? ? ? 53 56 8B D9"));

	void* dwIsUsingStaticPropDebugModes = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
		hs::using_static_prop.s().c_str());

	void* dwNewFunctionClient = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 83 E4 F8 83 EC 34 56 C7"));

	void* dwNewFunctionEngine = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 56 8B F1 33 C0 57"));

	void* dwNewFunctionStudioRender = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::studio_render_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 56 8B F1 33"));

	void* dwNewFunctionMaterialSystem = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::mat_sys_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 56 8B F1 33 C0"));

	void* dwPanoramaZipFunction;
	DWORD tempaddy = FindClassVtable(GetModuleHandleA(crypt_str("panorama.dll")), crypt_str(".?AVCZip@@"), 0, 0x0);
	if (tempaddy)
	{
		dwPanoramaZipFunction = (void*)(((DWORD*)tempaddy)[14]); // panorama zip file parsing function
	}


	H::Effects = **reinterpret_cast<CClientEffectRegistration***>(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		g_Patterns[fnva1(hs::effects.s().c_str())]().c_str()) + 2);

	auto temp = reinterpret_cast<uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()), hs::hud.s().c_str()) + 1);

	H::hud = *temp;
	H::findHudElement = relativeToAbsolute<findHudElementFn>(reinterpret_cast<uintptr_t>(temp) + 5);

	void* dwChicken_EventKilled = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::server_dll.s().c_str())]().c_str()), hs::chicken_eventkilled.s().c_str());

	DWORD* dwDoDepthOfField = (DWORD*)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 83 EC 24 53 56 8B F1 57 89 75 E0 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ?"));
	DWORD* dwIsDepthOfFieldEnabled = (DWORD*)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("8B 0D ? ? ? ? 56 8B 01 FF 50 34 8B F0 85 F6 75 04"));

	DWORD* dwParticleSimulations = (DWORD*)csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("55 8B EC 83 E4 F8 83 EC 30 56 57 8B F9 0F 28 E1 8B 0D ? ? ? ? F3 0F 11 64 24 ? 89 7C 24 18 8B 81"));

	DWORD* dwTier0_Msg = (DWORD*)GetProcAddress(GetModuleHandle(crypt_str("tier0.dll")), crypt_str("Msg"));

	auto sv_cheats = interfaces.cvars->FindVar(hs::sv_cheats.s().c_str());
	auto camera = interfaces.cvars->FindVar(hs::camera.s().c_str());
	auto shadows = interfaces.cvars->FindVar(hs::shadows.s().c_str());

	g_pClientStateAdd = std::make_unique< MinHook  >();
	g_pEngineHook = std::make_unique< MinHook  >();
	g_pEngineSoundHook = std::make_unique< MinHook  >();
	g_pPanelHook = std::make_unique< MinHook  >();
	g_pClientModeHook = std::make_unique< MinHook  >();
	g_pShadow = std::make_unique< MinHook  >();
	g_pStudioRenderHook = std::make_unique< MinHook  >();
	g_pModelRenderHook = std::make_unique< MinHook  >();
	g_pPredictHook = std::make_unique< MinHook  >();
	g_pMovementHook = std::make_unique< MinHook  >();
	g_pQueryHook = std::make_unique< MinHook  >();
	g_pCameraHook = std::make_unique< MinHook  >();
	g_pGetBoolHook = std::make_unique< MinHook  >();
	g_pDirectXHook = std::make_unique< MinHook  >();
	g_pRenderViewHook = std::make_unique< MinHook  >();
	g_pLeafSystemHook = std::make_unique< MinHook  >();
	g_pClientHook = std::make_unique< MinHook  >();
	g_pMaterialSystemHook = std::make_unique< MinHook  >();
	g_pEngineTraceHook = std::make_unique< MinHook  >();
	g_pFileSystemHook = std::make_unique< MinHook  >();
	g_pSurfaceHook = std::make_unique< MinHook  >();
	g_pLocalizeHook = std::make_unique<MinHook>();
	g_pViewRenderHook = std::make_unique<MinHook>();
	g_gameTypeHook = std::make_unique<MinHook>();

	g_pEngineHook->init(interfaces.engine);
	g_pEngineSoundHook->init(interfaces.engine_sound);
	g_pPanelHook->init(interfaces.v_panel);
	g_pClientHook->init(interfaces.client);
	g_pClientModeHook->init(interfaces.client_mode);
	g_pShadow->init(shadows);
	g_pStudioRenderHook->init(interfaces.studio_render);
	g_pModelRenderHook->init(interfaces.models.model_render);
	g_pPredictHook->init(interfaces.prediction);
	g_pMovementHook->init(interfaces.game_movement);
	g_pQueryHook->init(interfaces.engine->GetBSPTreeQuery());
	g_pCameraHook->init(camera);
	g_pGetBoolHook->init(sv_cheats);
	g_pRenderViewHook->init(interfaces.render_view);
	g_pLeafSystemHook->init(interfaces.leaf_system);
	g_pMaterialSystemHook->init(interfaces.material_system);
	g_pClientStateAdd->init((CClientState*)(uint32_t(csgo->client_state) + 0x8));
	g_pEngineTraceHook->init(interfaces.trace);
	g_pFileSystemHook->init(interfaces.v_file_system);
	g_pSurfaceHook->init(interfaces.surfaces);
	g_pLocalizeHook->init(interfaces.localize);
	g_pViewRenderHook->init(interfaces.viewrender);
	g_gameTypeHook->init(interfaces.gameType);

	//g_pEngineTraceHook->Hook(g_HookIndices[fnva1(hs::Hooked_ClipRayColliedable.s().c_str())], Hooked_ClipRayColliedable);
	g_pClientHook->hookAt(g_HookIndices[fnva1(hs::Hooked_FrameStageNotify.s().c_str())], Hooked_FrameStageNotify);
	g_pClientHook->hookAt(22, hkCreateMoveProxy);


	g_pClientHook->hookAt(g_HookIndices[fnva1(hs::Hooked_WriteUsercmdDeltaToBuffer.s().c_str())], Hooked_WriteUsercmdDeltaToBuffer);
	g_pClientHook->hookAt(g_HookIndices[fnva1(hs::Hooked_DispatchUserMessage.s().c_str())], Hooked_DispatchUserMessage);
	g_pEngineHook->hookAt(g_HookIndices[fnva1(hs::Hooked_IsPaused.s().c_str())], Hooked_IsPaused);
	g_pEngineHook->hookAt(g_HookIndices[fnva1(hs::Hooked_IsBoxVisible.s().c_str())], Hooked_IsBoxVisible);
	g_pEngineHook->hookAt(g_HookIndices[fnva1(hs::Hooked_IsHLTV.s().c_str())], Hooked_IsHLTV);
	g_pEngineHook->hookAt(27, Hooked_IsConnected);
	g_pEngineHook->hookAt(g_HookIndices[fnva1(hs::Hooked_GetScreenAspectRatio.s().c_str())], Hooked_GetScreenAspectRatio);


	//g_pShadow->hookAt(g_HookIndices[fnva1(hs::Hooked_ShouldDrawShadow.s().c_str())], Hooked_ShouldDrawShadow);
	//g_pClientModeHook->hookAt(g_HookIndices[fnva1(hs::Hooked_CreateMove.s().c_str())], Hooked_CreateMove);
	//g_pClientModeHook->hookAt(g_HookIndices[fnva1(hs::Hooked_ShouldDrawFog.s().c_str())], Hooked_ShouldDrawFog);
	g_pClientModeHook->hookAt(g_HookIndices[fnva1(hs::Hooked_OverrideView.s().c_str())], Hooked_OverrideView);
	g_pClientModeHook->hookAt(g_HookIndices[fnva1(hs::Hooked_ViewModel.s().c_str())], Hooked_ViewModel);
	g_pClientModeHook->hookAt(g_HookIndices[fnva1(hs::Hooked_DoPostScreenEffects.s().c_str())], Hooked_DoPostScreenEffects);
	g_pClientModeHook->hookAt(g_HookIndices[fnva1(hs::Hooked_OverrideMouseInput.s().c_str())], Hooked_OverrideMouseInput);

	g_pPanelHook->hookAt(g_HookIndices[fnva1(hs::Hooked_PaintTraverse.s().c_str())], Hooked_PaintTraverse);
	g_pStudioRenderHook->hookAt(g_HookIndices[fnva1(hs::Hooked_BeginFrame.s().c_str())], Hooked_BeginFrame);
	g_pClientStateAdd->hookAt(g_HookIndices[fnva1(hs::Hooked_PacketStart.s().c_str())], Hooked_PacketStart);
	g_pClientStateAdd->hookAt(g_HookIndices[fnva1(hs::Hooked_PacketEnd.s().c_str())], Hooked_PacketEnd);
	g_pQueryHook->hookAt(g_HookIndices[fnva1(hs::Hooked_ListLeavesInBox.s().c_str())], Hooked_ListLeavesInBox);
	g_pModelRenderHook->hookAt(g_HookIndices[fnva1(hs::Hooked_DrawModelExecute.s().c_str())], Hooked_DrawModelExecute);

	g_pMovementHook->hookAt(g_HookIndices[fnva1(hs::Hooked_ProcessMovement.s().c_str())], Hooked_ProcessMovement);
	g_pPredictHook->hookAt(g_HookIndices[fnva1(hs::Hooked_RunCommand.s().c_str())], Hooked_RunCommand);
	g_pPredictHook->hookAt(g_HookIndices[fnva1(hs::Hooked_InPrediction.s().c_str())], Hooked_InPrediction);
	g_pGetBoolHook->hookAt(g_HookIndices[fnva1(hs::Hooked_GetBool.s().c_str())], Hooked_GetBool);
	//g_pRenderViewHook->hookAt(g_HookIndices[fnva1(hs::Hooked_SceneEnd.s().c_str())], Hooked_SceneEnd);
	g_pEngineSoundHook->hookAt(g_HookIndices[fnva1(hs::Hooked_EmitSound.s().c_str())], EmitSound_Hooked);
	g_pFileSystemHook->hookAt(g_HookIndices[fnva1(hs::Hooked_GetUnverifiedFileHashes.s().c_str())], Hooked_getUnverifiedFileHashes);
	g_pFileSystemHook->hookAt(g_HookIndices[fnva1(hs::Hooked_CanLoadThirdPartyFiles.s().c_str())], Hooked_canLoadThirdPartyFiles);
	g_pSurfaceHook->hookAt(g_HookIndices[fnva1(hs::Hooked_OnScreenSizeChanged.s().c_str())], on_screen_size_changed);
	g_pLocalizeHook->hookAt(11, Hooked_FindIndex);

	//g_pViewRenderHook->hookAt(41, Hooked_RenderSmokeOverlay);



	dtUpdateClientSideAnimation.detour(dwUpdateClientSideAnimation, Hooked_UpdateClientSideAnimation);
	//H::UpdateClientSideAnimation = (UpdateClientSideAnimationFn)DetourFunction((PBYTE)dwUpdateClientSideAnimation, (PBYTE)Hooked_UpdateClientSideAnimation);

	dtDoExtraBoneProcessing.detour(dwDoExtraBoneProcessing, Hooked_DoExtraBoneProcessing);
	//H::DoExtraBoneProcessing = (DoExtraBoneProcessingFn)DetourFunction((PBYTE)dwDoExtraBoneProcessing, (PBYTE)Hooked_DoExtraBoneProcessing);



	//we do not use eyeang for thirdperson so this is fucking useless lmfao
	//H::GetEyeAngles = (GetEyeAnglesFn)DetourFunction((PBYTE)dwGetEyeAngles, (PBYTE)Hooked_GetEyeAngles);


	dtShouldSkipAnimFrame.detour(dwShouldSkipAnimFrame, Hooked_ShouldSkipAnimFrame);
	//H::ShouldSkipAnimFrame = (ShouldSkipAnimFrameFn)DetourFunction((PBYTE)dwShouldSkipAnimFrame, (PBYTE)Hooked_ShouldSkipAnimFrame);


	dtStandardBlendingRules.detour(dwStandardBlendingRules, Hooked_StandardBlendingRules);
	//H::StandardBlendingRules = (StandardBlendingRulesFn)DetourFunction((PBYTE)dwStandardBlendingRules, (PBYTE)Hooked_StandardBlendingRules);


	dtModifyEyePos.detour(dwEyePos, Hooked_ModifyEyePos);
	//H::ModifyEyePos = (ModifyEyePosFn)DetourFunction((PBYTE)dwEyePos, (PBYTE)Hooked_ModifyEyePos);

	physSim.detour(dwPhysicsSimulate, physicsSimulateHook);


	//dtSetupBones.detour(dwSetupBones, Hooked_SetupBones2);
	//H::SetupBones = (SetupBonesFn)DetourFunction((PBYTE)dwSetupBones, (PBYTE)Hooked_SetupBones2);


	dtColorModulation.detour(dwColorModulation, Hooked_GetColorModulation);

	//H::GetColorModulation = (GetColorModulationFn)DetourFunction((PBYTE)dwColorModulation, (PBYTE)Hooked_GetColorModulation);
	//H::mGetColorModulation.detour((uintptr_t)dwColorModulation, Hooked_GetColorModulation);


	dtIsUsingStaticPropDebugModes.detour(dwIsUsingStaticPropDebugModes, Hooked_IsUsingStaticPropDebugModes);
	dtNewFunctionClientDLL.detour(dwNewFunctionClient, Hooked_NewFunctionClientBypass);
	dtNewFunctionEngineDLL.detour(dwNewFunctionEngine, Hooked_NewFunctionEngineBypass);
	dtNewFunctionStudioRenderDLL.detour(dwNewFunctionStudioRender, Hooked_NewFunctionStudioRenderBypass);
	dtNewFunctionMaterialSystemDLL.detour(dwNewFunctionMaterialSystem, Hooked_NewFunctionMaterialSystemBypass);

	dtPerfScreenOverlay.detour(dwPerfScreenOverlay, hkPerfScreenOverlay);

	//H::IsUsingStaticPropDebugModes = (IsUsingStaticPropDebugModesFn)DetourFunction((PBYTE)dwIsUsingStaticPropDebugModes, (PBYTE)Hooked_IsUsingStaticPropDebugModes);

	//dtdoDepthOfField.detour(dwDoDepthOfField, Hooked_DoDepthOfField);
	dtisDepthOfFieldEnabled.detour(dwIsDepthOfFieldEnabled, isDepthOfFieldEnabled_Hooked);
	//H::DoDepthOfField = (DoDepthOfFieldFn)DetourFunction((PBYTE)dwDoDepthOfField, (PBYTE)Hooked_DoDepthOfField);
	//H::IsDepthOfFieldEnabled = (IsDepthOfFieldEnabledFn)DetourFunction((PBYTE)dwIsDepthOfFieldEnabled, (PBYTE)Hooked_IsDepthOfFieldEnabled);
	//H::DoAnimationEvent = (DoAnimationEventFn)DetourFunction((PBYTE)dwAnimEvent, (PBYTE)Hooked_DoAnimationEvent);

	dtParticleSimulations.detour(dwParticleSimulations, Hooked_ParticleSimulations);
	dtTier0_Msg.detour(dwTier0_Msg, Hooked_Tier0_Msg);
	dtReinitPredictables.detour(engine_prediction->ReinitPredictables, Hooked_ReinitPredictables);
	//H::BuildTransformations = (BuildTransformationsFn)DetourFunction((PBYTE)dwBuildTrans, (PBYTE)Hooked_BuildTransformations);
	csgo->panorama_CZip_UnkLoadFiles_original = dwPanoramaZipFunction;
	//dtCZip_UnkLoadFiles.detour(dwPanoramaZipFunction, MyPanorama_CZip_UnkLoadFiles);

	H::SteamPresent = *reinterpret_cast<decltype(H::SteamPresent)*>(CSignatures::Get().Present);
	*reinterpret_cast<decltype(::Hooked_Present)**>(CSignatures::Get().Present) = ::Hooked_Present;

	H::SteamReset = *reinterpret_cast<decltype(H::SteamReset)*>(CSignatures::Get().Reset);
	*reinterpret_cast<decltype(::Hooked_Reset)**>(CSignatures::Get().Reset) = ::Hooked_Reset;

	if (csgo->Init.Window)
		csgo->Init.OldWindow = (WNDPROC)SetWindowLongPtr(csgo->Init.Window, GWL_WNDPROC, (LONG_PTR)Hooked_WndProc);

	RecvProp* recvProp = 0;
	netvars.Get_Prop(crypt_str("DT_BaseViewModel"), crypt_str("m_nSequence"), &recvProp);
	if (recvProp)
	{
		SequenceHook = new recv_prop_hook(recvProp, sequence_proxy_fn);
	}

	RecvProp* radar_prop = 0;
	netvars.Get_Prop(crypt_str("DT_BasePlayer"), crypt_str("m_bSpotted"), &radar_prop);
	if (radar_prop)
	{
		bSpottedHook = new recv_prop_hook(radar_prop, radarhack_fn);
	}

	auto address = csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnva1(hs::engine_dll.s().c_str())]().c_str()),
		hs::choke_limit.s().c_str()) + 0x1;

	uint32_t choke_clamp = 62;

	DWORD oldProtect = 0;
	VirtualProtect((void*)address, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &oldProtect);
	*(uint32_t*)address = choke_clamp;
	VirtualProtect((void*)address, sizeof(uint32_t), oldProtect, &oldProtect);

	RegListeners();

	MH_EnableHook(MH_ALL_HOOKS);
}

/*
void H::UnHook()
{
	if (interfaces.inputsystem)
		interfaces.inputsystem->EnableInput(true);

	if (g_Animfix->FakeAnimstate)
		interfaces.memalloc->Free(g_Animfix->FakeAnimstate);

	DetourRemove((PBYTE)H::DoExtraBoneProcessing, (PBYTE)Hooked_DoExtraBoneProcessing);
	DetourRemove((PBYTE)H::UpdateClientSideAnimation, (PBYTE)Hooked_UpdateClientSideAnimation);
	DetourRemove((PBYTE)H::StandardBlendingRules, (PBYTE)Hooked_StandardBlendingRules);
	DetourRemove((PBYTE)H::ShouldSkipAnimFrame, (PBYTE)Hooked_ShouldSkipAnimFrame);
	DetourRemove((PBYTE)H::ModifyEyePos, (PBYTE)Hooked_ModifyEyePos);
	DetourRemove((PBYTE)H::SetupBones, (PBYTE)Hooked_SetupBones);
	DetourRemove((PBYTE)H::GetColorModulation, (PBYTE)Hooked_GetColorModulation);
	DetourRemove((PBYTE)H::IsUsingStaticPropDebugModes, (PBYTE)Hooked_IsUsingStaticPropDebugModes);
	//DetourRemove((PBYTE)H::DoAnimationEvent, (PBYTE)Hooked_DoAnimationEvent);

	*reinterpret_cast<void**>(CSignatures::Get().Present) = SteamPresent;
	*reinterpret_cast<void**>(CSignatures::Get().Reset) = SteamReset;

	g_pClientStateAdd->Unhook(g_HookIndices[fnva1(hs::Hooked_PacketStart.s().c_str())]);
	g_pClientStateAdd->Unhook(g_HookIndices[fnva1(hs::Hooked_PacketEnd.s().c_str())]);
	g_pClientHook->Unhook(g_HookIndices[fnva1(hs::Hooked_FrameStageNotify.s().c_str())]);
	g_pClientHook->Unhook(g_HookIndices[fnva1(hs::Hooked_WriteUsercmdDeltaToBuffer.s().c_str())]);
	g_pPanelHook->Unhook(g_HookIndices[fnva1(hs::Hooked_PaintTraverse.s().c_str())]);
	g_pEngineHook->Unhook(g_HookIndices[fnva1(hs::Hooked_IsPaused.s().c_str())]);
	g_pEngineHook->Unhook(g_HookIndices[fnva1(hs::Hooked_IsBoxVisible.s().c_str())]);
	g_pEngineHook->Unhook(g_HookIndices[fnva1(hs::Hooked_IsHLTV.s().c_str())]);
	g_pEngineHook->Unhook(g_HookIndices[fnva1(hs::Hooked_GetScreenAspectRatio.s().c_str())]);
	g_pShadow->Unhook(g_HookIndices[fnva1(hs::Hooked_ShouldDrawShadow.s().c_str())]);
	g_pClientModeHook->Unhook(g_HookIndices[fnva1(hs::Hooked_ShouldDrawFog.s().c_str())]);
	g_pClientModeHook->Unhook(g_HookIndices[fnva1(hs::Hooked_OverrideView.s().c_str())]);
	g_pClientModeHook->Unhook(g_HookIndices[fnva1(hs::Hooked_CreateMove.s().c_str())]);
	g_pClientModeHook->Unhook(g_HookIndices[fnva1(hs::Hooked_OverrideView.s().c_str())]);
	g_pClientModeHook->Unhook(g_HookIndices[fnva1(hs::Hooked_OverrideMouseInput.s().c_str())]);
	g_pClientModeHook->Unhook(g_HookIndices[fnva1(hs::Hooked_DoPostScreenEffects.s().c_str())]);
	g_pStudioRenderHook->Unhook(g_HookIndices[fnva1(hs::Hooked_BeginFrame.s().c_str())]);
	g_pModelRenderHook->Unhook(g_HookIndices[fnva1(hs::Hooked_DrawModelExecute.s().c_str())]);
	g_pPredictHook->Unhook(g_HookIndices[fnva1(hs::Hooked_RunCommand.s().c_str())]);
	g_pPredictHook->Unhook(g_HookIndices[fnva1(hs::Hooked_InPrediction.s().c_str())]);
	g_pRenderViewHook->Unhook(g_HookIndices[fnva1(hs::Hooked_SceneEnd.s().c_str())]);
	g_pQueryHook->Unhook(g_HookIndices[fnva1(hs::Hooked_ListLeavesInBox.s().c_str())]);
	g_pGetBoolHook->Unhook(g_HookIndices[fnva1(hs::Hooked_GetBool.s().c_str())]);
	g_pMovementHook->Unhook(g_HookIndices[fnva1(hs::Hooked_ProcessMovement.s().c_str())]);
	g_pEngineTraceHook->Unhook(g_HookIndices[fnva1(hs::Hooked_ClipRayColliedable.s().c_str())]);
	g_pEngineSoundHook->Unhook(g_HookIndices[fnva1(hs::Hooked_EmitSound.s().c_str())]);

	for (auto listener : g_pGameEventManager)
		interfaces.event_manager->RemoveListener(listener);
}
*/