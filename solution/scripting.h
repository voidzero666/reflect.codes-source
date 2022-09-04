#pragma once
#include "Hooks.h"
#include "lua/lua.hpp"
#include "lua/sol.hpp"
#include <filesystem>

struct lua_hook_t
{
	int index;
	sol::protected_function func;
};

class CLuaHookManager
{
public:
	void register_hook(std::string eventName, int scriptId, sol::protected_function func);
	void unregister_hooks(int scriptId);

	std::vector<lua_hook_t> get_hooks(std::string eventName);

private:
	std::map<std::string, std::vector<lua_hook_t>> hooks;
};

class CLua : public Singleton<CLua>
{
public:
	void initialize();
	void refresh_scripts();

	void load_script(int idx);
	void unload_script(int idx);

	std::string open_script(int idx);
	void save_script(int idx, std::string text);

	void reload_all_scripts();
	void unload_all_scripts();

	int get_script_id(const std::string& name);
	int get_script_id_by_path(const std::string& path);
	std::string get_current_script(sol::this_state s);
	int get_current_script_id(sol::this_state s);

	std::vector <bool> loaded;
	std::vector <std::string> scripts;
	std::unordered_map <int, std::unordered_map <std::string, sol::protected_function>> events;

	CLuaHookManager hooks;

private:
	std::string get_script_path(const std::string& name);
	std::string get_script_path(int id);

	std::vector <std::filesystem::path> paths;
};