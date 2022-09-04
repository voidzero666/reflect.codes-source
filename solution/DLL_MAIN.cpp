#include <thread>
#include <chrono>
#include "DLL_MAIN.h"
#include "Hooks.h"
#include "netvar_manager.h"
#include "EnginePrediction.h"
#include "render.h"
#include "protect/md5.hpp"
#include "protect/cryptoc.hpp"
#include "protect/config.h"
#include "sha256.h"
#include <filesystem>
#include "Recorder.h"
#include "scripting.h"

#pragma comment (lib, "Wininet.lib")
#pragma comment (lib, "Advapi32.lib")
#include <WinInet.h>

CNetVarManager netvars;

std::wstring project_dir;
c_config config;
std::int32_t cfg_last_update_time_in_minutes;
std::string loader_hwid;

std::string user_UUID;

void load_cfg()
{
	if (config.offset > 0)
	{
		config.pop_bool();
		cfg_last_update_time_in_minutes = config.pop_int32_t();

		csgo->password = config.pop_string();
		csgo->username = config.pop_string();
		loader_hwid = config.pop_string();
	}
}

DWORD WINAPI CheatMain(LPVOID lpThreadParameter)
{
	srand(time(0));

	c_config::SH_APPDATA.resize(MAX_PATH);

	c_config::SH_APPDATA.resize(wcslen(&c_config::SH_APPDATA[0]));

	project_dir = c_config::SH_APPDATA + nnx::encoding::utf8to16(crypt_str("\\Reflect"));
	config = c_config(project_dir, nnx::encoding::utf8to16(crypt_str("config.cfg")));

	static wchar_t path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		csgo->config_directory = nnx::encoding::utf16to8(std::wstring(path) + crypt_strw(L"\\Reflect\\"));
		std::filesystem::create_directories(csgo->config_directory + crypt_str("Sounds\\"));
		std::filesystem::create_directories(csgo->config_directory + crypt_str("Scripts\\"));
		//std::filesystem::create_directories(std::string(path) + crypt_str("\\Reflect\\PanoramaFiles"));
	}
	csgo->hitsound_dir = std::wstring(path) + crypt_strw(L"\\Reflect\\Sounds\\");
	route = std::make_unique<savingroute>(crypt_str("Routes"));

	csgo->log_location = project_dir + crypt_strw(L"\\log.txt");

	while (!(csgo->Init.Window = FindWindowA(hs::Valve001.s().c_str(), NULL)))
		this_thread::sleep_for(200ms);
	while (!GetModuleHandleA(hs::client_dll.s().c_str()))
		this_thread::sleep_for(200ms);
	while (!GetModuleHandleA(hs::engine_dll.s().c_str()))
		this_thread::sleep_for(200ms);
	while (!GetModuleHandleA(hs::serverbrowser_dll.s().c_str()))
		this_thread::sleep_for(200ms);

	xs64_reset_seed();

	load_cfg();

	I::Setup();
	netvars.Initialize();
	H::Hook();
	CLua::Get().initialize();
	CLua::Get().refresh_scripts();
	interfaces.engine->ClientCmd_Unrestricted(hs::clear.s().c_str(), 0);

	/*
                _____.__                 __                     .___             
_______   _____/ ____\  |   ____   _____/  |_    ____  ____   __| _/____   ______
\_  __ \_/ __ \   __\|  | _/ __ \_/ ___\   __\ _/ ___\/  _ \ / __ |/ __ \ /  ___/
 |  | \/\  ___/|  |  |  |_\  ___/\  \___|  |   \  \__(  <_> ) /_/ \  ___/ \___ \ 
 |__|    \___  >__|  |____/\___  >\___  >__| /\ \___  >____/\____ |\___  >____  >
             \/                \/     \/     \/     \/           \/    \/     \/ 
	*/
	interfaces.engine->ClientCmd_Unrestricted(crypt_str("echo \"                _____.__                 __                     .___             \""),0);
	interfaces.engine->ClientCmd_Unrestricted(crypt_str("echo \"_______   _____/ ____\\  |   ____   _____/  |_    ____  ____   __| _/____   ______\""),0);
	interfaces.engine->ClientCmd_Unrestricted(crypt_str("echo \"\\_  __ \\_/ __ \\   __\\|  | _/ __ \\_/ ___\\   __\\ _/ ___\\/  _ \\ / __ |/ __ \\ /  ___/\""),0);
	interfaces.engine->ClientCmd_Unrestricted(crypt_str("echo \" |  | \\/\\  ___/|  |  |  |_\\  ___/\\  \\___|  |   \\  \\__(  <_> ) /_/ \\  ___/ \\___ \\ \""),0);
	interfaces.engine->ClientCmd_Unrestricted(crypt_str("echo \" |__|    \\___  >__|  |____/\\___  >\\___  >__| /\\ \\___  >____/\\____ |\\___  >____  >\""),0);
	interfaces.engine->ClientCmd_Unrestricted(crypt_str("echo \"             \\/                \\/     \\/     \\/     \\/           \\/    \\/     \\/ \""),0);

	interfaces.engine->ClientCmd_Unrestricted(hs::unload_message1.s().c_str(), 0);
	while (!csgo->DoUnload)
		this_thread::sleep_for(1s);

	interfaces.engine->ClientCmd_Unrestricted(hs::clear.s().c_str(), 0);
	interfaces.engine->ClientCmd_Unrestricted(hs::unload_message.s().c_str(), 0);

	SetWindowLongPtr(csgo->Init.Window, GWL_WNDPROC, (LONG_PTR)csgo->Init.OldWindow);
	FreeLibraryAndExitThread(csgo->Init.Dll, 0);
	
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpThreadParameter) {
	
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(CheatMain), lpThreadParameter, 0, nullptr);
		csgo->Init.Dll = hDll;
	}
	return TRUE;
}