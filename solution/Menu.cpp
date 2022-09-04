#include "Menu.h"
#include "Misc.h"
#include "GUI/gui.h"
#include "GUI/keybind.h"
#include "config_.hpp"
#include "nSkinz.hpp"
#include "Recorder.h"
#include <functional>
#include "scripting.h"
#include "images.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or 
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed 



static int lua_current_item = 0;
#define bl(name) static bool name = false;


vector<string> ConfigList;
typedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);




void c_menu::color_picker4(const char* name, color_t& clr, bool label = true) 
{

	ImVec4 clrs = ImVec4(std::clamp((float)clr[0] / 255.f, 0.f, 1.f), std::clamp((float)clr[1] / 255.f, 0.f, 1.f), std::clamp((float)clr[2] / 255.f, 0.f, 1.f), std::clamp((float)clr[3] / 255.f, 0.f, 1.f));


	float color[4] = { clrs.x, clrs.y,clrs.z,clrs.w };
	if (label)
	{
		std::string nm = crypt_str("\t") + std::string(name);
		ImGui::Text(nm.c_str());
		ImGui::SameLine();
	}
	ImGui::ColorEdit4(name, color, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);


	clr[0] = std::clamp(static_cast<int>(color[0] * 255), 0, 255);
	clr[1] = std::clamp(static_cast<int>(color[1] * 255), 0, 255);
	clr[2] = std::clamp(static_cast<int>(color[2] * 255), 0, 255);
	clr[3] = std::clamp(static_cast<int>(color[3] * 255), 0, 255);

	/*
	static ImVec4 color = ImVec4(std::clamp((float)clr[0] / 255.f, 0.f, 1.f), std::clamp((float)clr[1] / 255.f, 0.f, 1.f), std::clamp((float)clr[2] / 255.f, 0.f, 1.f), std::clamp((float)clr[3] / 255.f, 0.f, 1.f));


	if (ImGui::ColorButton(name, color, 0))
	{
		*enable = !*enable;
	}

	if (enable)
		ImGui::ColorPicker4(name, (float*)&color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSmallPreview);

	clr[0] = std::clamp(static_cast<int>(color.x * 255), 0, 255);
	clr[1] = std::clamp(static_cast<int>(color.y * 255), 0, 255);
	clr[2] = std::clamp(static_cast<int>(color.z * 255), 0, 255);
	clr[3] = std::clamp(static_cast<int>(color.w * 255), 0, 255);
	*/
}


BOOL SearchFiles(LPCTSTR lpszFileName, LPSEARCHFUNC lpSearchFunc, BOOL bInnerFolders = FALSE)
{
	LPTSTR part;
	char tmp[MAX_PATH];
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, crypt_str("*.*"));
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				if (!strncmp(wfd.cFileName, crypt_str("."), 1) || !strncmp(wfd.cFileName, crypt_str(".."), 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, crypt_str("\\"));
					strcat(next, name);

					SearchFiles(next, lpSearchFunc, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd));
			FindClose(hSearch);
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE;
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			lpSearchFunc(wfd.cFileName);
		}
	while (FindNextFile(hSearch, &wfd));
	FindClose(hSearch);
	return TRUE;
}

static std::string configs = crypt_str("");
char* buffer = new char[32];


void ReadConfigs(LPCTSTR lpszFileName)
{
	ConfigList.push_back(lpszFileName);
	
}

void RefreshConfigs()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		ConfigList.clear();
		string ConfigDir = std::string(path) + crypt_str("\\Reflect\\*.cfg");
		configs.clear();
		SearchFiles(ConfigDir.c_str(), ReadConfigs, FALSE);

		for (auto& config : ConfigList)
			configs += std::string(config + crypt_str("\0"));
	}

}

namespace ImGui
{
	// ImGui ListBox lambda binder
	static bool ListBox(const char* label, int* current_item, std::function<const char* (int)> lambda, int items_count, int height_in_items)
	{
		return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
			{
				*out_text = (*reinterpret_cast<std::function<const char* (int)>*>(data))(idx);
				return true;
			}, &lambda, items_count, height_in_items);
	}
}

void EnableHiddenCVars()
{
	auto p = **reinterpret_cast<ConCommandBase***>(interfaces.cvars + 0x34);
	for (auto c = p->m_pNext; c != nullptr; c = c->m_pNext)
	{
		ConCommandBase* cmd = c;
		cmd->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
		cmd->m_nFlags &= ~FCVAR_HIDDEN;
	}
}

Vector2D g_mouse;
namespace menu_colors {
	color_t main_color;
	color_t background1;
	color_t background2;
}

void c_menu::draw_indicators()
{
}

//using this to manage keybinds, remember to set the name of the keybind the same as the label to render;)
void init_keybinds()
{
	keybinds.clear();
	//legitbot
	keybinds.push_back(&vars.legitbot.key);
	vars.legitbot.key.name = crypt_str("Legitbot Key");

	

	keybinds.push_back(&vars.legitbot.triggerbot.trigkey);
	vars.legitbot.triggerbot.trigkey.name = crypt_str("Triggerbot Key");
	//ragebot
	keybinds.push_back(&vars.ragebot.key);
	vars.ragebot.key.name = crypt_str("Ragebot Key");
	keybinds.push_back(&vars.ragebot.override_dmg);
	vars.ragebot.override_dmg.name = crypt_str("Override Minimum Damage");
	keybinds.push_back(&vars.ragebot.force_safepoint);
	vars.ragebot.force_safepoint.name = crypt_str("Force Safepoints");
	keybinds.push_back(&vars.ragebot.baim);
	vars.ragebot.baim.name = crypt_str("Force Body Aim");
	keybinds.push_back(&vars.ragebot.double_tap);
	vars.ragebot.double_tap.name = crypt_str("Double Tap");
	keybinds.push_back(&vars.ragebot.hideShots);
	vars.ragebot.hideShots.name = crypt_str("Hideshots");
	//antiaim
	keybinds.push_back(&vars.antiaim.inverter);
	vars.antiaim.inverter.name = crypt_str("Invert real angle");
	keybinds.push_back(&vars.antiaim.manual_left);
	vars.antiaim.manual_left.name = crypt_str("Left");
	vars.antiaim.manual_left.special = true;
	keybinds.push_back(&vars.antiaim.manual_right);
	vars.antiaim.manual_right.name = crypt_str("Right");
	vars.antiaim.manual_right.special = true;
	keybinds.push_back(&vars.antiaim.manual_back);
	vars.antiaim.manual_back.name = crypt_str("Back");
	vars.antiaim.manual_back.special = true;
	keybinds.push_back(&vars.antiaim.manual_forward);
	vars.antiaim.manual_forward.name = crypt_str("Forward");
	vars.antiaim.manual_forward.special = true;
	keybinds.push_back(&vars.antiaim.fakeduck);
	vars.antiaim.fakeduck.name = crypt_str("Fake Duck");
	keybinds.push_back(&vars.antiaim.slowwalk);
	vars.antiaim.slowwalk.name = crypt_str("Slow Walk");
	keybinds.push_back(&vars.antiaim.fakehead);
	vars.antiaim.fakehead.name = crypt_str("Fake Head");
	//misc
	keybinds.push_back(&vars.misc.thirdperson);
	vars.misc.thirdperson.name = crypt_str("Thirdperson");
	keybinds.push_back(&vars.misc.peek_assist);
	vars.misc.peek_assist.name = crypt_str("Peek Assistance");
	keybinds.push_back(&vars.misc.blockbot);
	vars.misc.blockbot.name = crypt_str("Blockbot");
	//movement
	keybinds.push_back(&vars.movement.jumpbugkey);
	vars.movement.jumpbugkey.name = crypt_str("Jumpbug Key");
	keybinds.push_back(&vars.movement.edgebug.key);
	vars.movement.edgebug.key.name = crypt_str("Edgebug Key");
	keybinds.push_back(&vars.movement.edgejumpkey);
	vars.movement.edgejumpkey.name = crypt_str("Edge Jump Key");
	keybinds.push_back(&vars.movement.minijumpkey);
	vars.movement.minijumpkey.name = crypt_str("Mini Jump Key");
	keybinds.push_back(&vars.movement.longjumpkey);
	vars.movement.longjumpkey.name = crypt_str("Long Jump Bind Key");
	keybinds.push_back(&vars.movement.recorder.startrecord);
	vars.movement.recorder.startrecord.name = crypt_str("Start Recording");
	keybinds.push_back(&vars.movement.recorder.startplayback);
	vars.movement.recorder.startplayback.name = crypt_str("Start Playback");
	keybinds.push_back(&vars.movement.recorder.stoprecord);
	vars.movement.recorder.stoprecord.name = crypt_str("Stop Recording");
	keybinds.push_back(&vars.movement.recorder.stopplayback);
	vars.movement.recorder.stopplayback.name = crypt_str("Stop Playback");
	keybinds.push_back(&vars.movement.recorder.clearrecord);
	vars.movement.recorder.clearrecord.name = crypt_str("Clear Recording");
	keybinds.push_back(&vars.movement.ladderglidekey);
	vars.movement.ladderglidekey.name = crypt_str("Ladder Glide Key");
	keybinds.push_back(&vars.movement.checkpoint.teleportkey);
	vars.movement.checkpoint.teleportkey.name = crypt_str("Teleport Key");
	keybinds.push_back(&vars.movement.checkpoint.checkpointkey);
	vars.movement.checkpoint.checkpointkey.name = crypt_str("Checkpoint Key");
	keybinds.push_back(&vars.movement.checkpoint.nextkey);
	vars.movement.checkpoint.nextkey.name = crypt_str("Next Checkpoint Key");
	keybinds.push_back(&vars.movement.checkpoint.prevkey);
	vars.movement.checkpoint.prevkey.name = crypt_str("Prev Checkpoint Key");
	keybinds.push_back(&vars.movement.checkpoint.undokey);
	vars.movement.checkpoint.undokey.name = crypt_str("Undo Checkpoint Key");
	keybinds.push_back(&vars.movement.autopixelsurfkey);
	vars.movement.autopixelsurfkey.name = crypt_str("Auto Pixel Surf Key");
	keybinds.push_back(&vars.movement.autopixelsurf_align);
	vars.movement.autopixelsurf_align.name = crypt_str("Auto Align Key");
	keybinds.push_back(&vars.misc.autostrafekey);
	vars.misc.autostrafekey.name = crypt_str("Auto Strafe Key");
}


/*
void c_menu::render()
{
	

	if (!ImGui::GetCurrentContext())
		return;

	
	static std::array<bool, 65>clrpck; // colorpicker states

	static bool once = false;
	if (!once)
	{
		Config.ResetToDefault();
		vars.menu.open = true;
		memset(buffer, 0, 32);
		RefreshConfigs();
		init_keybinds();
		window = new c_window();
		for (auto& i : clrpck)
			i = false;

		once = true;
	}


	if (!vars.menu.open)
		return;

	
	auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ 730, 488 };
	ImGui::SetNextWindowSize(ImVec2(gs));
	ImGui::Begin(crypt_str("##GUI"), NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse);
	{
		{//draw
			s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2); p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y); auto draw = ImGui::GetWindowDrawList();
			draw->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), ImColor(15, 17, 19), 5);
			draw->AddRectFilled(ImVec2(p.x, p.y + 40), ImVec2(p.x + s.x, p.y + s.y), ImColor(18, 20, 21), 5, ImDrawCornerFlags_Bot);
			/*
			ImGui::PushFont(fonts::logo);
			auto logo_size = ImGui::CalcTextSize(crypt_str("Reflect"));
			draw->AddText(ImVec2(p.x + 54 - logo_size.x / 2, p.y + 20 - logo_size.y / 2), ImColor(0, 255, 128), crypt_str("Reflect"));
			ImGui::PopFont();
			
			draw->Flags |= ImDrawListFlags_AntiAliasedLinesUseTex;
			draw->AddImage(textures::xtclogo.texture, ImVec2(p.x + 3, p.y - 5), ImVec2(p.x + 55, p.y + 49));
		}

		{//tabs
			ImGui::PushFont(fonts::tab_names);
			ImGui::SetCursorPosX(63);
			ImGui::BeginGroup();
			if (ImGui::Tab(crypt_str("Legit"), vars.menu.current_tab == 0)) vars.menu.current_tab = 0; ImGui::SameLine();
			if (ImGui::Tab(crypt_str("Rage"), vars.menu.current_tab == 1)) vars.menu.current_tab = 1; ImGui::SameLine();
			if (ImGui::Tab(crypt_str("Anti-aim"), vars.menu.current_tab == 2)) vars.menu.current_tab = 2; ImGui::SameLine();
			if (ImGui::Tab(crypt_str("Players"), vars.menu.current_tab == 3)) vars.menu.current_tab = 3; ImGui::SameLine();
			if (ImGui::Tab(crypt_str("Visuals"), vars.menu.current_tab == 4)) vars.menu.current_tab = 4; ImGui::SameLine();
			if (ImGui::Tab(crypt_str("Misc"), vars.menu.current_tab == 5)) vars.menu.current_tab = 5; ImGui::SameLine();
			if (ImGui::Tab(crypt_str("Config"), vars.menu.current_tab == 6)) vars.menu.current_tab = 6; ImGui::SameLine();
			if (ImGui::Tab(crypt_str("LUA"), vars.menu.current_tab == 7)) vars.menu.current_tab = 7;
			ImGui::EndGroup();
			ImGui::PopFont();
		}

		ImGui::PushFont(fonts::font);

		switch (vars.menu.current_tab)
		{
		case 0:
		{
			{
				ImGui::SetCursorPosY(50);
				ImGui::BeginGroup();
				ImGui::SetCursorPosX(16);
				ImGui::MenuChild(crypt_str("Legitbot"), ImVec2(345, 235), false, ImGuiWindowFlags_NoScrollbar);
				{
					if (!vars.ragebot.enable)
					{
						ImGui::Checkbox(crypt_str("Aimbot"), &vars.legitbot.aimbot);
						if (vars.legitbot.aimbot)
						{
							ImGui::Checkbox(crypt_str("On Key"), &vars.legitbot.onkey);
							if (vars.legitbot.onkey)
							{
								ImGui::Dummy(ImVec2(0.0f, 0.0f));
								ImGui::SameLine();
								vars.legitbot.key.imgui(crypt_str("Legitbot Key"));
							}
							ImGui::Checkbox(crypt_str("Target Teammates"), &vars.legitbot.teammates);
							//ImGui::Checkbox(crypt_str("RCS Standalone"), &vars.legitbot.rcsstandalone);
						}
						ImGui::Checkbox(crypt_str("Auto Pistol"), &vars.legitbot.autopistol);

						ImGui::Checkbox(crypt_str("Backtrack"), &vars.legitbot.backtrack);
						if (vars.legitbot.backtrack)
						{
							ImGui::PushID(crypt_str("polsjdsd"));
							ImGui::SliderFloat(crypt_str("Backtrack max length"), &vars.legitbot.btlength, 0.f, 0.2f, crypt_str("%.3f"));
							ImGui::PopID();
						}
					}

				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}

			{
				ImGui::SetCursorPosY(290);
				ImGui::BeginGroup();
				ImGui::SetCursorPosX(16);
				ImGui::MenuChild(crypt_str("Triggerbot"), ImVec2(345, 183), false, ImGuiWindowFlags_NoScrollbar);
				{
					ImGui::Checkbox(crypt_str("Enable"), &vars.legitbot.triggerbot.enabled);
					if (vars.legitbot.triggerbot.enabled)
					{
						ImGui::Checkbox(crypt_str("On Key"), &vars.legitbot.triggerbot.onkey);
						if (vars.legitbot.triggerbot.onkey)
						{
							ImGui::Dummy(ImVec2(0.0f, 0.0f));
							ImGui::SameLine();
							vars.legitbot.triggerbot.trigkey.imgui(crypt_str("Triggerbot key"));
						}

						//ImGui::Checkbox(crypt_str("Target Teammates"), &vars.legitbot.triggerbot.teammates);
						ImGui::Checkbox(crypt_str("Scoped Only"), &vars.legitbot.triggerbot.scopedonly);
						ImGui::Checkbox(crypt_str("Flash Check"), &vars.legitbot.triggerbot.checkflash);
						ImGui::Checkbox(crypt_str("Smoke Check"), &vars.legitbot.triggerbot.checksmoke);
						if (vars.legitbot.backtrack)
							ImGui::Checkbox(crypt_str("Shoot at backtrack"), &vars.legitbot.triggerbot.shootatbacktrack);

						ImGui::SliderFloat(crypt_str("Delay"), &vars.legitbot.triggerbot.delay, 0.f, 0.5f, crypt_str("%.3fs"), ImGuiSliderFlags_Logarithmic);
						ImGui::SliderInt(crypt_str("Minimum damage"), &vars.legitbot.triggerbot.mindmg, 1, 100);
						const std::string hitboxes[5]{ crypt_str("Head"), crypt_str("Chest") , crypt_str("Stomach"), crypt_str("Arms"), crypt_str("Legs") };
						std::string hitbox_label = crypt_str("");
						for (int i = 0; i < 5; i++)
						{
							if (vars.legitbot.triggerbot.hitboxes[i])
							{
								if (i != 4) {
									hitbox_label += std::string(hitboxes[i]) + crypt_str(", ");
								}
								else
								{
									hitbox_label += std::string(hitboxes[i]);
								}
							}
						}

						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::Text(crypt_str("Hitbox selection"));
						if (ImGui::BeginCombo(crypt_str("Hitbox"), hitbox_label.c_str()))
						{
							for (size_t i = 0; i < 5; i++)
							{
								ImGui::Selectable(hitboxes[i].c_str(), &vars.legitbot.triggerbot.hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							}

							ImGui::EndCombo();
						}
					}
				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}

				{
					ImGui::SetCursorPosY(50);
					ImGui::BeginGroup();
					ImGui::SetCursorPosX(369);
					ImGui::MenuChild(crypt_str("Weapon configuration"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
					{
						if (vars.legitbot.aimbot)
						{
							static std::string weapons[] = { crypt_str("Default"), crypt_str("Pistol"), crypt_str("Rifle"), crypt_str("Scout"), crypt_str("AWP"), crypt_str("SMG")};
							ImGui::Combo(weapons[vars.legitbot.active_index].c_str(), &vars.legitbot.active_index, crypt_str("Default\0Pistol\0Rifle\0Scout\0AWP\0SMG\0"), 6);
							ImGui::Checkbox(crypt_str("Enable"), &vars.legitbot.weapons[vars.legitbot.active_index].enabled);
							if (vars.legitbot.weapons[vars.legitbot.active_index].enabled)
							{
								ImGui::Checkbox(crypt_str("Silent"), &vars.legitbot.weapons[vars.legitbot.active_index].silent);
								//ImGui::Checkbox(crypt_str("Lock"), &vars.legitbot.weapons[vars.legitbot.active_index].aimlock);
								ImGui::SliderFloat(crypt_str("FOV"), &vars.legitbot.weapons[vars.legitbot.active_index].fov, 0.f, 180.f, crypt_str("%.3f"), ImGuiSliderFlags_Logarithmic);
								ImGui::SliderFloat(crypt_str("Smooth"), &vars.legitbot.weapons[vars.legitbot.active_index].smooth, 1.f, 100.f, crypt_str("%.4f"));
								ImGui::SliderFloat(crypt_str("RCS X"), &vars.legitbot.weapons[vars.legitbot.active_index].rcsx, 0.f, 1.f, crypt_str("%.3f"));
								ImGui::SliderFloat(crypt_str("RCS Y"), &vars.legitbot.weapons[vars.legitbot.active_index].rcsy, 0.f, 1.f, crypt_str("%.3f"));
								ImGui::Checkbox(crypt_str("Scoped Only"), &vars.legitbot.weapons[vars.legitbot.active_index].scopedonly);
								ImGui::Checkbox(crypt_str("Flashbang Check"), &vars.legitbot.weapons[vars.legitbot.active_index].ignoreflash);
								ImGui::Checkbox(crypt_str("Smoke Check"), &vars.legitbot.weapons[vars.legitbot.active_index].ignoresmoke);
								if (vars.legitbot.backtrack)
									ImGui::Checkbox(crypt_str("Aim at Backtrack"), &vars.legitbot.weapons[vars.legitbot.active_index].aimbacktrack);

								static std::string hitboxes[]{ crypt_str("Head"), crypt_str("Chest") , crypt_str("Stomach"), crypt_str("Arms"), crypt_str("Legs")};
								std::string hitbox_label = crypt_str("");
								for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
								{
									if (vars.legitbot.weapons[vars.legitbot.active_index].hitboxes[i])
									{
										if (i != ARRAYSIZE(hitboxes) - 1) {
											hitbox_label += std::string(hitboxes[i]) + crypt_str(", ");
										}
										else
										{		
											hitbox_label += std::string(hitboxes[i]);										
										}
									}	
								}

								ImGui::Dummy(ImVec2(0.0f, 0.0f));
								ImGui::SameLine();
								ImGui::Text(crypt_str("Hitbox selection"));
								if (ImGui::BeginCombo(crypt_str("Hitbox"), hitbox_label.c_str()))
								{
									for (size_t i = 0; i < ARRAYSIZE(hitboxes); i++)
									{
										ImGui::Selectable(hitboxes[i].c_str(), &vars.legitbot.weapons[vars.legitbot.active_index].hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
									}

									ImGui::EndCombo();
								}


							}
						}
					}
					ImGui::EndChild();
					ImGui::EndGroup();
				}
			
		}
		break;

		case 1:
		{
			{
				ImGui::SetCursorPosY(50);
				ImGui::BeginGroup();
				ImGui::SetCursorPosX(16);
				ImGui::MenuChild(crypt_str("Aimbot"), ImVec2(345, 250), false, ImGuiWindowFlags_NoScrollbar);
				{
					ImGui::Checkbox(crypt_str("Enable"), &vars.ragebot.enable);
					if (vars.ragebot.enable)
					{
						ImGui::Checkbox(crypt_str("On Key"), &vars.ragebot.onkey);
						if (vars.ragebot.onkey)
						{
							ImGui::Dummy(ImVec2(0.0f, 0.0f));
							ImGui::SameLine();
							vars.ragebot.key.imgui(crypt_str("Ragebot Key"));
						}

						ImGui::Checkbox(crypt_str("Silent aim"), &vars.ragebot.silent);
						ImGui::Checkbox(crypt_str("Auto Shoot"), &vars.ragebot.autoshoot);
						
						ImGui::Checkbox(crypt_str("Automatic scope"), &vars.ragebot.autoscope);
						ImGui::SliderInt(crypt_str("FOV"), &vars.ragebot.fov, 0, 180);
						ImGui::Checkbox(crypt_str("Resolver"), &vars.ragebot.resolver);
						//ImGui::Checkbox(crypt_str("Resolver mode 1"), &vars.ragebot.rmode1);
						//ImGui::Checkbox(crypt_str("Resolver mode 2"), &vars.ragebot.rmode2);
						//ImGui::Checkbox(crypt_str("Resolver mode 3"), &vars.ragebot.rmode3);
						//ImGui::Checkbox(crypt_str("Resolver mode 4"), &vars.ragebot.rmode4);
						//ImGui::Checkbox(crypt_str("Old resolver lmao"), &vars.ragebot.rmode5);

						ImGui::Checkbox(crypt_str("Zeus Bot"), &vars.ragebot.zeusbot);
						if (vars.ragebot.zeusbot)
						{
							ImGui::PushID(crypt_str("22"));
							ImGui::SliderInt(crypt_str("Zeus Hitchance"), &vars.ragebot.zeuschance, 25, 100);
							ImGui::PopID();
						}
						ImGui::Checkbox(crypt_str("Knife Bot"), &vars.misc.knifebot);
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::PushID(crypt_str("4545"));
						vars.ragebot.force_safepoint.imgui(crypt_str("Force Safepoints"));
						ImGui::PopID();
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::PushID(crypt_str("4546"));
						vars.ragebot.baim.imgui(crypt_str("Force Body Aim"));
						ImGui::PopID();
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::PushID(crypt_str("4547"));
						vars.ragebot.override_dmg.imgui(crypt_str("Override Minimum Damage"));
						ImGui::PopID();
					}
				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}

			{
				ImGui::SetCursorPosY(305);
				ImGui::BeginGroup();
				ImGui::SetCursorPosX(16);
				ImGui::MenuChild(crypt_str("Exploits"), ImVec2(345, 168), false, ImGuiWindowFlags_NoScrollbar);
				{
					if (vars.ragebot.enable)
					{
						if (vars.misc.restrict_type == 1)
						{
							ImGui::Dummy(ImVec2(0.0f, 0.0f));
							ImGui::SameLine();
							ImGui::PushID(crypt_str("8789"));
							vars.ragebot.double_tap.imgui(crypt_str("Double Tap"));
							ImGui::PopID();
							if (vars.ragebot.double_tap.type != 0)
							{
								ImGui::Checkbox(crypt_str("Teleport boost"), &vars.ragebot.dt_teleport);
								//ImGui::Checkbox(crypt_str("Hideshot on DT"), &vars.ragebot.hideshot);
							}

							ImGui::Dummy(ImVec2(0.0f, 0.0f));
							ImGui::SameLine();
							ImGui::PushID(crypt_str("hideshotteringkeying"));
							vars.ragebot.hideShots.imgui(crypt_str("Hide Shots"));
							ImGui::PopID();

							ImGui::Checkbox(crypt_str("Anti lagpeek"), &vars.ragebot.antiexploit);
						}
					}
				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}

			{
				ImGui::SetCursorPosY(50);
				ImGui::BeginGroup();
				ImGui::SetCursorPosX(369);
				ImGui::MenuChild("Weapon configuration", ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
				{
					if (vars.ragebot.enable)
					{
						
						static std::string weapons[] = { crypt_str("Default"), crypt_str("Autosniper"), crypt_str("Scout"), crypt_str("AWP"), crypt_str("Rifles"), crypt_str("Pistols"), crypt_str("Heavy Pistols")};					
						ImGui::Combo(weapons[vars.ragebot.active_index].c_str(), &vars.ragebot.active_index, crypt_str("Default\0Autosniper\0Scout\0AWP\0Rifles\0Pistols\0Heavy Pistols\0"));
						std::string nigger123 = crypt_str("Enable ") + weapons[vars.ragebot.active_index] + crypt_str(" weapon config");
						ImGui::Checkbox(nigger123.c_str(), &vars.ragebot.weapon[vars.ragebot.active_index].enable);
						if (vars.ragebot.weapon[vars.ragebot.active_index].enable)
						{
							ImGui::SliderInt(crypt_str("Hitchance"), &vars.ragebot.weapon[vars.ragebot.active_index].hitchance, 0, 100, crypt_str("%d%%"));
							if (vars.ragebot.active_index != 2 && vars.ragebot.active_index != 3 && vars.ragebot.double_tap.type != 0)
							{
								ImGui::PushID(crypt_str("55"));
								ImGui::SliderInt(crypt_str("Double tap hitchance"), &vars.ragebot.weapon[vars.ragebot.active_index].doubletap_hc, 0, 100, crypt_str("%d%%"));
								ImGui::PopID();
							}

							if (vars.ragebot.weapon[vars.ragebot.active_index].mindamage == 101) {
								ImGui::SliderInt(crypt_str("Minimum damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage, 0, 101, crypt_str("HP+1"));
							}
							else {
								ImGui::SliderInt(crypt_str("Minimum damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage, 0, 101, crypt_str("%d HP"));
							}



							if (vars.ragebot.override_dmg.valid()) {

								ImGui::PushID(crypt_str("67"));
								if (vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override != 101) {
									ImGui::SliderInt(crypt_str("Override Minimum Damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override, 0, 101, crypt_str("%d HP"));
								}
								else
								{
									ImGui::SliderInt(crypt_str("Override Minimum Damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override, 0, 101, crypt_str("HP + 1"));
								}
								ImGui::PopID();
							}
							
							//-----
							//-----
							static std::string crhitboxes[]{ crypt_str("Head"), crypt_str("Stomach"), crypt_str("Chest"), crypt_str("UpperChest"), crypt_str("LowerChest"), crypt_str("Pelvis"), crypt_str("Thighs"), crypt_str("Shins"), crypt_str("Feet"), crypt_str("Arms"), crypt_str("Hands") };
							std::string rhitbox_label = crypt_str("");
							for (int i = 0; i < ARRAYSIZE(crhitboxes); i++)
							{
								if (i != ARRAYSIZE(crhitboxes) - 1) {
									if (vars.ragebot.weapon[vars.ragebot.active_index].hitboxes[i])
									{
										rhitbox_label += std::string(crhitboxes[i]) + crypt_str(", ");
									}
								}
								else
								{
									if (vars.ragebot.weapon[vars.ragebot.active_index].hitboxes[i])
									{
										rhitbox_label += std::string(crhitboxes[i]);
									}
								}
							}

							ImGui::Dummy(ImVec2(0.0f, 0.0f));
							ImGui::SameLine();
							ImGui::Text(crypt_str("Hitbox selection"));
							if (ImGui::BeginCombo(crypt_str("Hitbox"), rhitbox_label.c_str()))
							{
								for (size_t i = 0; i < ARRAYSIZE(crhitboxes); i++)
								{
									ImGui::Selectable(crhitboxes[i].c_str(), &vars.ragebot.weapon[vars.ragebot.active_index].hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
								}

								ImGui::EndCombo();
							}
							//--

							//ImGui::Checkbox(crypt_str("Automatic stop"), &vars.ragebot.weapon[vars.ragebot.active_index].quickstop);
							ImGui::Dummy(ImVec2(0.0f, 0.0f));
							ImGui::SameLine();
							ImGui::Text(crypt_str("Auto-Stop"));
							ImGui::Combo(crypt_str("Auto-Stop"), &vars.ragebot.weapon[vars.ragebot.active_index].autostopMode, crypt_str("Disabled\0Normal\0Quickstop\0Predictive\0"));

							ImGui::Checkbox(crypt_str("Static pointscale"), &vars.ragebot.weapon[vars.ragebot.active_index].static_scale);
							if (vars.ragebot.weapon[vars.ragebot.active_index].static_scale)
							{
								ImGui::SliderInt(crypt_str("Head scale"), &vars.ragebot.weapon[vars.ragebot.active_index].scale_head, 0, 100, crypt_str("%d%%"));
								ImGui::SliderInt(crypt_str("Body scale"), &vars.ragebot.weapon[vars.ragebot.active_index].scale_body, 0, 100, crypt_str("%d%%"));
							}

							ImGui::SliderInt(crypt_str("Maximum misses"), &vars.ragebot.weapon[vars.ragebot.active_index].max_misses, 0, 6, crypt_str("%d"));

							ImGui::Checkbox(crypt_str("Prefer safe points"), &vars.ragebot.weapon[vars.ragebot.active_index].prefer_safepoint);
						}
					}
				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}
		}
		break;

		case 2:
		{
			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(16);
			ImGui::MenuChild(crypt_str("Anti-aim"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{
				ImGui::Checkbox(crypt_str("Enable"), &vars.antiaim.enable);
				if (vars.antiaim.enable)
				{
					ImGui::Checkbox(crypt_str("On use"), &vars.antiaim.aa_on_use);
					ImGui::Checkbox(crypt_str("Desync on shot"), &vars.antiaim.shotDesync);

					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::Text(crypt_str("Pitch"));
					ImGui::Combo(crypt_str("Pitchaa"), &vars.antiaim.pitch, crypt_str("Disabled\0Down\0Up\0"));

					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::Text(crypt_str("Yaw"));
					ImGui::Combo(crypt_str("baseyaw"), &vars.antiaim.base_yaw, crypt_str("Backward\0Forward\0"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();

					ImGui::PushID(crypt_str("1090"));
					vars.antiaim.inverter.imgui(crypt_str("Invert real angle"));
					ImGui::PopID();
					ImGui::SliderInt(crypt_str("Jitter Angle"), &vars.antiaim.jitter_angle, 0, 45, crypt_str("%d degrees"));
					ImGui::Checkbox(crypt_str("At targets"), &vars.antiaim.attarget);
					//if (vars.antiaim.attarget)
					//{
						//ImGui::Checkbox(crypt_str("Off when offscreen"), &vars.antiaim.attarget_off_when_offsreen);
					//}


					
					ImGui::Checkbox(crypt_str("Static desync"), &vars.antiaim.static_delta);
					
					if (!vars.antiaim.static_delta)
					{
						ImGui::PushID(crypt_str("23424"));
						ImGui::SliderInt(crypt_str("Desync amount"), &vars.antiaim.desync_amount, 0, 100, crypt_str("%d%%"));
						ImGui::PopID();
						//ImGui::Checkbox(crypt_str("Avoid overlap"), &vars.antiaim.avoid_overlap);
					}

					ImGui::Checkbox(crypt_str("Extend"), &vars.antiaim.extendlby);


					if (vars.misc.restrict_type == 1 && !vars.misc.antiuntrusted) {

						ImGui::Checkbox(crypt_str("Z-lean"), &vars.antiaim.zleanenable);
						ImGui::SliderInt(crypt_str("Z-lean angle"), &vars.antiaim.zlean, -180, 180, crypt_str("%d degrees"));
					}

					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					vars.antiaim.fakehead.imgui(crypt_str("fakehead"));
					if (vars.antiaim.fakehead.valid())
					{
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::Text(crypt_str("Fake Head Mode"));
						ImGui::Combo(crypt_str("fakeheadmode"), &vars.antiaim.fakehead_mode, crypt_str("Prefer Safety\0Prefer Speed\0"));
					}

					ImGui::Checkbox(crypt_str("Manual anti-aim"), &vars.antiaim.manual_antiaim);
					if (vars.antiaim.manual_antiaim)
					{
						ImGui::Checkbox(crypt_str("Indicator"), &vars.visuals.antiaim_arrows);
						if (vars.visuals.antiaim_arrows)
						{
							
							color_picker4(crypt_str("Indicator Color"), vars.visuals.antiaim_arrows_color);
							
						}
						

						ImGui::Checkbox(crypt_str("Ignore at target"), &vars.antiaim.ignore_attarget);
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::PushID(crypt_str("1091"));
						vars.antiaim.manual_left.imgui(crypt_str("Left"));
						ImGui::PopID();
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::PushID(crypt_str("1092"));
						vars.antiaim.manual_right.imgui(crypt_str("Right"));
						ImGui::PopID();
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::PushID(crypt_str("1093"));
						vars.antiaim.manual_back.imgui(crypt_str("Back"));
						ImGui::PopID();
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::PushID(crypt_str("1094"));
						vars.antiaim.manual_forward.imgui(crypt_str("Forward"));
						ImGui::PopID();
					}
					
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();

			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("Extra"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{
				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text(crypt_str("Fake-lag"));
				ImGui::Combo(crypt_str("Fake-lag"), &vars.antiaim.fakelag, crypt_str("Disabled\0Max\0Jitter\0BreakLC\0"));
				if (vars.antiaim.fakelag > 0)
				{
					ImGui::SliderInt(crypt_str("Fake-lag ticks"), &vars.antiaim.fakelagfactor, 0, 14, crypt_str("%d ticks"));
					if (vars.antiaim.fakelag == 2)
					{
						ImGui::PushID(crypt_str("23445"));
						ImGui::SliderInt(crypt_str("Fake-lag jitter"), &vars.antiaim.fakelagvariance, 0, 100, crypt_str("%d%%"));
						ImGui::PopID();
					}
					ImGui::Checkbox(crypt_str("Standby choke"), &vars.antiaim.fakelag_when_standing);
					//ImGui::Checkbox(crypt_str("Standing"), &vars.antiaim.fakelag_when_standing);
				}
				if (vars.misc.restrict_type == 1)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					vars.antiaim.fakeduck.imgui(crypt_str("Fake Duck"));
				}
				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				vars.antiaim.slowwalk.imgui(crypt_str("Slow Walk"));
				if (vars.antiaim.slowwalk.valid())
				{
					ImGui::PushID(crypt_str("SEXOOAOXOAXOXOAXOAXOAOXOXAOA"));
					ImGui::SliderInt(crypt_str("Slow Walk Speed"), &vars.antiaim.slowwalkspeed, 0, 100, crypt_str("%d%%"));
					ImGui::PopID();
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		break;

		case 3:
		{
			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(16);
			ImGui::MenuChild(crypt_str("Player ESP"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{
				
				ImGui::Checkbox(crypt_str("Enable"), &vars.visuals.enable);
				if (vars.visuals.enable)
				{
					
					ImGui::Checkbox(crypt_str("Box"), &vars.visuals.box);
					if (vars.visuals.box) {
						
						color_picker4(crypt_str("Box Color"), vars.visuals.box_color);
						
					}
					

					ImGui::Checkbox(crypt_str("Dormant ESP"), &vars.visuals.dormant);
					
					ImGui::Checkbox(crypt_str("Name"), &vars.visuals.name);
					if (vars.visuals.name)
					{
						color_picker4(crypt_str("Name Color"), vars.visuals.name_color);
					}
					
					ImGui::Checkbox(crypt_str("Health Bar"), &vars.visuals.healthbar);
					if (vars.visuals.healthbar)
					{
						ImGui::Checkbox(crypt_str("Override health color"), &vars.visuals.override_hp);
						
						if (vars.visuals.override_hp)
							color_picker4(crypt_str("Health Bar Color"), vars.visuals.hp_color);
						
					}

					ImGui::Checkbox("Skeleton", &vars.visuals.skeleton);
					if (vars.visuals.skeleton)
						color_picker4(crypt_str("Skeleton Color"), vars.visuals.skeleton_color);

					ImGui::Checkbox("Glow", &vars.visuals.glow);
					if (vars.visuals.glow)
						color_picker4(crypt_str("Glow color"), vars.visuals.glow_color);
					
					ImGui::Checkbox(crypt_str("Glow on Local"), &vars.visuals.local_glow);
					if (vars.visuals.local_glow)
						color_picker4(crypt_str("Local Glow Color"), vars.visuals.local_glow_clr);
					ImGui::Combo(crypt_str("Glow Style"), &vars.visuals.glowtype, crypt_str("Normal\0Pulsating\0"));

					ImGui::Checkbox(crypt_str("Weapon"), &vars.visuals.weapon);
					if (vars.visuals.weapon)
						color_picker4(crypt_str("Weapon Color"), vars.visuals.weapon_color);

					ImGui::Checkbox(crypt_str("Ammo"), &vars.visuals.ammo);
					if (vars.visuals.ammo)
						color_picker4(crypt_str("Ammo Color"), vars.visuals.ammo_color);
					ImGui::Checkbox(crypt_str("Dlights"), &vars.visuals.dlights);
					if (vars.visuals.dlights)
						color_picker4(crypt_str("Dlights Color"), vars.visuals.dlights_color);

					static std::string flags[]{ crypt_str("Armor"), crypt_str("Scoped"), crypt_str("Flashed"), crypt_str("Defuse kit"), crypt_str("Fake duck") };
					static bool flag[5] = { false, false, false, false, false };
					static std::string flag_label = crypt_str("");
					for (size_t i = 0; i < 5; i++)
					{
						flag[i] = (vars.visuals.flags & 1 << i) == 1 << i;
					}

					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::TextColored(ImVec4{0.55,0.55,0.55,1.f}, crypt_str("Player flags"));

					if (ImGui::BeginCombo(crypt_str("Flags"), flag_label.c_str()))
					{
						for (size_t i = 0; i < 5; i++)
						{
							ImGui::Selectable(flags[i].c_str(), &flag[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						}

						ImGui::EndCombo();
					}

					for (size_t i = 0; i < 5; i++)
					{
						if (i == 0) flag_label = crypt_str("");

						if (flag[i])
						{
							flag_label += flag_label.size() ? std::string(crypt_str(", ")) + flags[i] : flags[i];
							vars.visuals.flags |= 1 << i;
						}
						else
						{
							vars.visuals.flags &= ~(1 << i);
						}
					}
					if (flag[0] || flag[1] || flag[2] || flag[3] || flag[4])
						color_picker4(crypt_str("Flags Color"), vars.visuals.flags_color);

					ImGui::Checkbox(crypt_str("Out of fov arrows"), &vars.visuals.out_of_fov);
					if (vars.visuals.out_of_fov)
					{
						color_picker4(crypt_str("Out of fov arrows color"), vars.visuals.out_of_fov_color);


						ImGui::PushID(crypt_str("09239"));
						ImGui::SliderInt(crypt_str("Size"), &vars.visuals.out_of_fov_size, 5, 80);
						ImGui::PopID();
						ImGui::PushID(crypt_str("092392"));
						ImGui::SliderInt(crypt_str("Distance"), &vars.visuals.out_of_fov_distance, 0, 500);
						ImGui::PopID();
					}
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();

			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("Chams"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{

				static int currentCategory{ 0 };
				
				ImGui::Dummy(ImVec2(0, 0));
				ImGui::SameLine();
				ImGui::Text(crypt_str("Target"));
				ImGui::PushID(0);

				static int material = 0;

				if (ImGui::Combo("", &currentCategory, crypt_str("Allies\0Enemies\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0Desync\0")))
					material = 0;

				ImGui::PopID();

				static int currentItem{ 0 };

				if (currentCategory <= 1) {
					
					static int currentType{ 0 };
					ImGui::Dummy(ImVec2(0, 0));
					ImGui::SameLine();
					ImGui::Text(crypt_str("Visibility"));
					ImGui::PushID(1);
					if (ImGui::Combo("", &currentType, crypt_str("Visible\0Occluded\0")))
						material = 0;
					ImGui::PopID();
					currentItem = currentCategory * 2 + currentType;
				}
				else {
					currentItem = currentCategory + 2;
				}

				ImGui::Dummy(ImVec2(0, 0));
				ImGui::SameLine();
				ImGui::Text(crypt_str("Layer"));
				ImGui::Combo(crypt_str("Layer"), &material, crypt_str(" 1\0 2\0"), 2);
			
				auto& chams{ vars.chams[currentItem].materials[material] };
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Enabled"), &chams.enabled);
				ImGui::Checkbox(crypt_str("Health based"), &chams.healthBased);
				ImGui::Checkbox(crypt_str("Blinking"), &chams.blinking);
				ImGui::Combo(crypt_str("Material"), &chams.material, crypt_str("Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0Pearlescent\0Metallic\0"));
				ImGui::Checkbox(crypt_str("Wireframe"), &chams.wireframe);
				if (!chams.rainbow)
					color_picker4(crypt_str("Color"), chams.color);

				ImGui::Checkbox(crypt_str("Rainbow"), &chams.rainbow);
				if (chams.rainbow)
					ImGui::SliderFloat(crypt_str("Rainbow speed"), &chams.rainbowSpeed, 0.01f, 5.f, crypt_str("%.2f"));
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		break;

		case 4:
		{
			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(16);
			ImGui::MenuChild(crypt_str("Other ESP"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{
				static std::array<std::string, 8>removals{ crypt_str("Visual recoil"), crypt_str("Smoke"), crypt_str("Flash"), crypt_str("Scope"), crypt_str("Zoom"), crypt_str("Post processing"), crypt_str("Fog"), crypt_str("Shadows")};
				static bool removal[removals.size()] = { false, false, false, false, false, false, false, false };
				std::string removal_label = crypt_str("");
				
				for (int s = 0; s < removals.size(); s++)
				{
					if (vars.visuals.remove[s])
					{
						if (s != removals.size() - 1)
						{
							removal_label += std::string(removals[s]) + crypt_str(", ");
						}
						else
						{
							removal_label += std::string(removals[s]);
						}
						
					}
				}

				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text(crypt_str("Removals"));
				if (ImGui::BeginCombo(crypt_str("Removals"), removal_label.c_str()))
				{
					for (size_t i = 0; i < removals.size(); i++)
					{
						ImGui::Selectable(removals.at(i).c_str(), &vars.visuals.remove[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
					}

					ImGui::EndCombo();
				}

				

				ImGui::Checkbox(crypt_str("Hitsound"), &vars.visuals.hitmarker_sound);
				if (vars.visuals.hitmarker_sound)
				{
					ImGui::PushID(crypt_str("hit sound!"));
					ImGui::Combo(crypt_str("Hitsound type"), &vars.visuals.hitmarker_sound_type, crypt_str("Switch\0Warning\0BOOM\0COD\0FemboyMoan\0Custom\0"));
					if (vars.visuals.hitmarker_sound_type == 5)
					{
						ImGui::Dummy({ 0, 0 });
						ImGui::SameLine();
						ImGui::InputText(crypt_str("Filename"), vars.visuals.hitmarker_name, 32);
					}
					ImGui::PopID();

				}
				ImGui::Checkbox(crypt_str("Hitmarker"), &vars.visuals.hitmarker);
				if (vars.visuals.hitmarker)
				{
					ImGui::Checkbox(crypt_str("Visualize damage"), &vars.visuals.visualize_damage);
					if (vars.visuals.visualize_damage)
						color_picker4(crypt_str("Headshot color"), vars.visuals.hitmarker_color);
				}
				ImGui::Checkbox(crypt_str("Server Bullet impacts"), &vars.visuals.bullet_impact);
					
				if (vars.visuals.bullet_impact)
				{
					color_picker4(crypt_str("Bullet impacts color"), vars.visuals.bullet_impact_color);
					ImGui::SliderFloat(crypt_str("Bullet impacts size"), &vars.visuals.impacts_size, 2.0f, 5.0f);
				}

				ImGui::Checkbox(crypt_str("Enemy bullet tracers"), &vars.visuals.bullet_tracer);
				if (vars.visuals.bullet_tracer)
					color_picker4(crypt_str("Enemy bullet tracers color"), vars.visuals.bullet_tracer_color);
				ImGui::Checkbox(crypt_str("Local bullet tracers"), &vars.visuals.bullet_tracer_local);
				if (vars.visuals.bullet_tracer_local)
					color_picker4(crypt_str("Local bullet tracers color"), vars.visuals.bullet_tracer_local_color);
				if (vars.visuals.bullet_tracer || vars.visuals.bullet_tracer_local)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Bullet tracers type"));
					ImGui::Combo(crypt_str("Bullet tracers type"), &vars.visuals.bullet_tracer_type, crypt_str("Default\0Phys beam\0")); //crypt_str("Default\0Phys beam\0Bubble\0Glow\0"));
				}

				ImGui::Checkbox(crypt_str("Preserve killfeed"), &vars.visuals.preverse_killfeed);

				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				vars.misc.thirdperson.imgui(crypt_str("Thirdperson"));
				if (vars.misc.thirdperson.valid())
				{
					ImGui::PushID(crypt_str("Dist"));
					ImGui::SliderInt(crypt_str("Distance"), &vars.visuals.thirdperson_dist, 0, 300, crypt_str("%d units"));
					ImGui::PopID();
				}

				ImGui::Checkbox(crypt_str("Nightmode"), &vars.visuals.nightmode);
				if (vars.visuals.nightmode)
				{
					
					ImGui::SliderInt(crypt_str("Nightmode amount"), &vars.visuals.nightmode_amount, 0, 100, crypt_str("%d%%"));
					ImGui::Checkbox(crypt_str("Color"), &vars.visuals.customize_color);
					
					if (vars.visuals.customize_color)
					{
						
						color_picker4(crypt_str("World"), vars.visuals.nightmode_color);
						
						color_picker4(crypt_str("Props"), vars.visuals.nightmode_prop_color);
						
						color_picker4(crypt_str("Skybox"), vars.visuals.nightmode_skybox_color);
					}

				}

				ImGui::Checkbox(crypt_str("Sunset mode"), &vars.visuals.sunset_enabled);
				if (vars.visuals.sunset_enabled)
				{
					ImGui::PushID(91323);
					ImGui::SliderFloat("", &vars.visuals.rot_x, 0, 360, crypt_str("X rotation: %.1f"));
					ImGui::PopID();
					ImGui::PushID(93523);
					ImGui::SliderFloat("", &vars.visuals.rot_y, 0, 360, crypt_str("Y rotation: %.1f"));
					ImGui::PopID();
				}

				if (!vars.visuals.remove[6]) // not allowing to edit fog when removed
				{
					ImGui::Checkbox(crypt_str("Fog Customization"), &vars.visuals.fog.enabled);
					if (vars.visuals.fog.enabled)
					{
						ImGui::PushID(84523);
						ImGui::SliderFloat(crypt_str("Fog start"), &vars.visuals.fog.start, 0, 5000);
						ImGui::PopID();

						ImGui::PushID(90023);
						ImGui::SliderFloat(crypt_str("Fog end"), &vars.visuals.fog.end, 0, 5000);
						ImGui::PopID();

						ImGui::PushID(12487);
						ImGui::SliderFloat(crypt_str("Fog density"), &vars.visuals.fog.density, 0.001f, 1.f, crypt_str("%.3f"));
						ImGui::PopID();

						color_picker4(crypt_str("Fog color"), vars.visuals.fog.color);
					}
				}
				

				ImGui::Checkbox(crypt_str("Motion blur"), &vars.visuals.motionblur.enabled);
				if (vars.visuals.motionblur.enabled)
				{
					ImGui::Checkbox(crypt_str("Forward enabled"), &vars.visuals.motionblur.fwd_enabled);
					ImGui::PushID(crypt_str("BLACKPERNOSSS"));
					ImGui::SliderFloat(crypt_str("Falling min"), &vars.visuals.motionblur.falling_min, 0.f, 50.f);
					ImGui::PopID();
					ImGui::PushID("falingMax");
					ImGui::SliderFloat(crypt_str("Falling max"), &vars.visuals.motionblur.falling_max, 0.f, 50.f);
					ImGui::PopID();
					ImGui::PushID("falingintersnsn");
					ImGui::SliderFloat(crypt_str("Falling intensity"), &vars.visuals.motionblur.falling_intensity, 0.f, 8.f);
					ImGui::PopID();
					ImGui::PushID("rotataintersnsn");
					ImGui::SliderFloat(crypt_str("Rotation intensity"), &vars.visuals.motionblur.rotation_intensity, 0.f, 8.f);
					ImGui::PopID();
					ImGui::PushID("sterererererxcxcxc");
					ImGui::SliderFloat(crypt_str("Strength"), &vars.visuals.motionblur.strength, 0.f, 8.f); 
					ImGui::PopID();
				}
				ImGui::Checkbox(crypt_str("Force crosshair"), &vars.visuals.force_crosshair);
				ImGui::Checkbox(crypt_str("Recoil crosshair"), &vars.visuals.recoil_crosshair);
				ImGui::Checkbox(crypt_str("Radar Reveal"), &vars.visuals.radarhack);
				ImGui::SliderInt(crypt_str("Aspect ratio"), &vars.visuals.aspect_ratio, 0, 300, crypt_str("%d"));
			}
			ImGui::EndChild();
			ImGui::EndGroup();

			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("World ESP"), ImVec2(345, 250), false, ImGuiWindowFlags_NoScrollbar);
			{
				ImGui::Checkbox(crypt_str("Weapon ESP"), &vars.visuals.world.weapons.enabled);
				if (vars.visuals.world.weapons.enabled)
					color_picker4(crypt_str("Weapon ESP color"), vars.visuals.world.weapons.color);
				ImGui::Checkbox(crypt_str("Grenade ESP"), &vars.visuals.world.projectiles.enable);
				if (vars.visuals.world.projectiles.enable)
				{
					std::string grennames[] = { crypt_str("Team"), crypt_str("Enemy"), crypt_str("Local") };
					std::string label = crypt_str("");
					for (int i = 0; i < 3; i++)
					{
						if (i != 3 - 1)
						{
							if (vars.visuals.world.projectiles.filter[i])
								label += std::string(grennames[i]) + crypt_str(", ");
						}
						else
						{
							if (vars.visuals.world.projectiles.filter[i])
								label += std::string(grennames[i]);
						}
					}

					if(ImGui::BeginCombo(crypt_str("Grenade ESP Target"), label.c_str()))
					{
						for (size_t i = 0; i < 3; i++)
						{
							ImGui::Selectable(grennames[i].c_str(), &vars.visuals.world.projectiles.filter[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						}

						ImGui::EndCombo();
					}

				}
				ImGui::Checkbox(crypt_str("Grenade prediction"), &vars.visuals.nadepred);
				ImGui::Checkbox(crypt_str("Bomb timer"), &vars.visuals.world.weapons.planted_bomb);
				if (vars.visuals.world.weapons.planted_bomb)
				{
					ImGui::PushID(crypt_str("bombotimero"));
					ImGui::SliderFloat(crypt_str("Bomb timer height"), &vars.visuals.world.weapons.planted_bombheight, 0.f, 1.f, crypt_str("%.3f"));
					ImGui::PopID();	
				}
				ImGui::SliderInt(crypt_str("FOV"), &vars.misc.worldfov, 0, 120, crypt_str("%d"));
				ImGui::SliderInt(crypt_str("Viewmodel FOV"), &vars.misc.viewmodelfov, 60, 150, crypt_str("%d"));
				
				ImGui::Checkbox(crypt_str("Viewmodel Customization"), &vars.misc.enableviewmodel);
				if (vars.misc.enableviewmodel)
				{
					ImGui::PushID(crypt_str("323"));
					ImGui::SliderFloat(crypt_str(""), &vars.misc.viewmodel_x, -25.f, 25.f, crypt_str("x: %.2f"));
					ImGui::PopID();
					ImGui::PushID(crypt_str("324"));
					ImGui::SliderFloat(crypt_str(""), &vars.misc.viewmodel_y, -25.f, 25.f, crypt_str("y: %.2f"));
					ImGui::PopID();
					ImGui::PushID(crypt_str("325"));
					ImGui::SliderFloat(crypt_str(""), &vars.misc.viewmodel_z, -25.f, 25.f, crypt_str("z: %.2f"));
					ImGui::PopID();
				}
			}

			ImGui::EndChild();
			ImGui::EndGroup();
			ImGui::SetCursorPosY(305);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("Animal"), ImVec2(345, 168), false, ImGuiWindowFlags_NoScrollbar);
			{
				ImGui::Checkbox(crypt_str("Chicken ESP"), &vars.misc.chicken.enable);
				if (vars.misc.chicken.enable)
				{
					//ImGui::Checkbox(crypt_str("Chicken box"), &vars.misc.chicken.chickenbox);
					ImGui::Checkbox(crypt_str("Owner name"), &vars.misc.chicken.owner);
					ImGui::Checkbox(crypt_str("Owned list"), &vars.misc.chicken.sidelist);
				}
				ImGui::Checkbox(crypt_str("Reveal chicken killer"), &vars.misc.chicken.revealkiller);
				ImGui::Checkbox(crypt_str("Announce chicken killers"), &vars.misc.chicken.saykiller);
				ImGui::Checkbox(crypt_str("Fish ESP"), &vars.misc.chicken.fish);
				if (vars.misc.chicken.fish)
				{
					//ImGui::Checkbox(crypt_str("Fish box"), &vars.misc.chicken.fishbox);
					color_picker4(crypt_str("Fish esp color"), vars.misc.chicken.fishcolor);
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		break;

		case 5:
		{
			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(16);
			ImGui::MenuChild(crypt_str("Misc"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{
				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text(crypt_str("Menu Key"));
				ImGui::SameLine();
				std::string keyname;
				static bool menukeybindopened;
				if (!menukeybindopened)
				{
					if (vars.menu.menu_key)
					{
						//ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("[ %s ]"), interfaces.inputsystem->vkey2string(key));
						keyname = std::string(crypt_str("[ ")) + std::string(interfaces.inputsystem->vkey2string(vars.menu.menu_key)) + std::string(crypt_str(" ]"));
					}
					else
					{
						//ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("[ key ]"));
						keyname = crypt_str("[ key ]");
					}
				}
				else
				{
					keyname = crypt_str("[ Press key ]");
				}


				if (menukeybindopened)
				{
					ImGuiIO& io = ImGui::GetIO();
					for (int i = 0; i < 512; i++)
					{
						if (ImGui::IsKeyPressed(i))
						{
							vars.menu.menu_key = i != VK_ESCAPE ? i : VK_INSERT;
							menukeybindopened = false;
						}
					}

					for (int i = 0; i < 5; i++)
					{
						if (ImGui::IsMouseDown(i))
						{
							vars.menu.menu_key = VK_INSERT;
							menukeybindopened = false;
						}
					}
				}


				if (ImGui::Button(keyname.c_str()))
				{
					menukeybindopened = true;
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip(crypt_str("Click to change keybind"));
				}

				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				ImGui::Text(crypt_str("Restrictions"));
				ImGui::Combo(crypt_str("Restrictions"), &vars.misc.restrict_type, crypt_str("MM\0HvH\0KZ\0"));
				//if (vars.misc.restrict_type == 0)
				//{
					ImGui::Checkbox(crypt_str("Anti untrusted"), &vars.misc.antiuntrusted);
				//}
				ImGui::Checkbox(crypt_str("Watermark"), &vars.visuals.watermark);
				ImGui::Checkbox(crypt_str("Block server ads & overlays"), &vars.misc.ublockOrigin);
				ImGui::Checkbox(crypt_str("Spotify status"), &vars.visuals.spotifysong);
				ImGui::Checkbox(crypt_str("Keybinds list"), &vars.visuals.indicators);
				ImGui::Checkbox(crypt_str("Spectator list"), &vars.visuals.speclist);
				ImGui::Checkbox(crypt_str("Team damage list"), &vars.visuals.teamdmglist);
				if (vars.visuals.teamdmglist)
				{
					ImGui::Checkbox(crypt_str("Cooldown say"), &vars.visuals.cooldownsay);
					if (vars.visuals.cooldownsay)
					{
						ImGui::PushID(crypt_str("seXoANale"));
						ImGui::Dummy(ImVec2(0, 0));
						ImGui::SameLine();
						ImGui::InputText(crypt_str("Cooldown say text"), vars.visuals.cooldownsaytext, 32);
						ImGui::PopID();

						ImGui::Checkbox(crypt_str("Add counter text to the end"), &vars.visuals.addcounter);
					}
				}
				//
				ImGui::Checkbox(crypt_str("Event logger"), &vars.visuals.eventlog);
				if (vars.visuals.eventlog)
				{
					color_picker4(crypt_str("Event logger color"), vars.visuals.eventlog_color);
					ImGui::Checkbox(crypt_str("Print votes to chat"), &vars.visuals.print_votes);
					if (vars.visuals.print_votes)
						ImGui::Combo(vars.visuals.print_votes_chat ? crypt_str("Team") : crypt_str("All"), &vars.visuals.print_votes_chat, crypt_str("Team\0All\0"));
				}
				ImGui::Checkbox(crypt_str("Reveal ranks"), &vars.misc.revealranks);
				ImGui::Checkbox(crypt_str("Auto accept"), &vars.misc.autoaccept);
				if (vars.misc.autoaccept)
				{
					ImGui::PushID(crypt_str("Sexiw"));
					ImGui::SliderFloat(crypt_str("Auto accept delay"), &vars.misc.autoacceptdelay, 0.f, 25.f, crypt_str("%.1fs"));
					ImGui::PopID();
				}
				ImGui::Checkbox(crypt_str("Bypass sv_pure"), &vars.misc.sv_purebypass);
				ImGui::Checkbox(crypt_str("Unlock inventory access"), &vars.misc.unlockinventoryaccess);
				ImGui::Checkbox(crypt_str("Killsay"), &vars.misc.killsay);
				
				ImGui::Checkbox(crypt_str("Clantag spammer"), &vars.visuals.clantagspammer);
				if (vars.visuals.clantagspammer)
				{
					ImGui::PushID(crypt_str("Clantagbullshit"));
					std::string combonames[] = { crypt_str("Static"), crypt_str("Animated"), crypt_str("Custom"), crypt_str("Clock")};
					ImGui::Combo(combonames[vars.visuals.clantagtype].c_str(), &vars.visuals.clantagtype, crypt_str("Static\0Animated\0Custom\0Clock\0"));
					ImGui::PopID();
					if (vars.visuals.clantagtype == 2)
					{
						ImGui::PushID(crypt_str("clantge"));
						ImGui::Dummy(ImVec2(0.0f, 0.0f));
						ImGui::SameLine();
						ImGui::InputTextWithHint(crypt_str(""), crypt_str("Custom Clantag"), vars.visuals.clantagcustomname, 32);
						ImGui::PopID();
						ImGui::Checkbox(crypt_str("Rotating"), &vars.visuals.clantagrotating);
						ImGui::PushID(crypt_str("sliderForSpedddd"));
						ImGui::SliderFloat(crypt_str("Clantag speed"), &vars.visuals.clantagspeed, 0.01, 5.f, crypt_str("%.2f"));
						ImGui::PopID();
					}
				}
				ImGui::Checkbox(crypt_str("Buy-bot"), &vars.misc.autobuy.enable);
				if (vars.misc.autobuy.enable)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::Text(crypt_str("Primary"));
					ImGui::Combo(crypt_str("Primary"), &vars.misc.autobuy.main, crypt_str("None\0Autosniper\0Scout\0AWP\0Negev\0M249\0Rifle\0AUG/SG553\0"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::Text(crypt_str("Secondary"));
					ImGui::PushID(crypt_str("2323424"));
					ImGui::Combo(crypt_str("Secondary"), &vars.misc.autobuy.pistol, crypt_str("None\0Dualies\0P250\0Five-seveN\0Deagle/R8 Revolver\0"));
					ImGui::PopID();

					static std::string utility[]{ crypt_str("Kevlar & helmet"), crypt_str("Kevlar"), crypt_str("Frag grenade"), crypt_str("Molotov"), crypt_str("Smoke"), crypt_str("Taser"), crypt_str("Defuser") };
					static bool util[7] = { false, false, false, false, false, false, false };
					static std::string utility_label = crypt_str("");
					for (size_t i = 0; i < 7; i++)
					{
						util[i] = (vars.misc.autobuy.misc & 1 << i) == 1 << i;
					}

					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::Text(crypt_str("Utility"));
					if (ImGui::BeginCombo(crypt_str("Utility"), utility_label.c_str()))
					{
						for (size_t i = 0; i < 7; i++)
						{
							ImGui::Selectable(utility[i].c_str(), &util[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						}

						ImGui::EndCombo();
					}

					for (size_t i = 0; i < 7; i++)
					{
						if (i == 0) utility_label = "";

						if (util[i])
						{
							utility_label += utility_label.size() ? std::string(crypt_str(", ")) + utility[i] : utility[i];
							vars.misc.autobuy.misc |= 1 << i;
						}
						else
						{
							vars.misc.autobuy.misc &= ~(1 << i);
						}
					}
				}


				ImGui::Checkbox(crypt_str("Checkpoint Menu"), &vars.movement.checkpoint.enabled);
				if (vars.movement.checkpoint.enabled)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					vars.movement.checkpoint.teleportkey.imgui(crypt_str("Teleport Key"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					vars.movement.checkpoint.checkpointkey.imgui(crypt_str("Checkpoint Key"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					vars.movement.checkpoint.nextkey.imgui(crypt_str("Next Checkpoint Key"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					vars.movement.checkpoint.prevkey.imgui(crypt_str("Prev Checkpoint Key"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					vars.movement.checkpoint.undokey.imgui(crypt_str("Undo Checkpoint Key"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					color_picker4(crypt_str("Checkpoint marker color"), vars.movement.checkpoint.color);
				}

				//
				//
				ImGui::Dummy({ 0, 0 });
				ImGui::SameLine();
				if (ImGui::Button(crypt_str("Unlock hidden ConVars"), { 217, 23 }))
				{
					EnableHiddenCVars();
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();

			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("Movement"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{
				ImGui::Checkbox(crypt_str("Bunnyhop"), &vars.misc.bunnyhop);
				
				if (vars.misc.restrict_type != 2)
				{
					ImGui::Separator();
					ImGui::Checkbox(crypt_str("Fast Duck"), &vars.movement.fastduck);
				}
					
				ImGui::Separator();
				//ImGui::Checkbox(crypt_str("Slidewalk"), &vars.movement.slidewalk);
				ImGui::Checkbox(crypt_str("Autostrafe"), &vars.misc.autostrafe);
				if (vars.misc.autostrafe)
				{
					ImGui::Combo(vars.movement.autostrafetype ? crypt_str("Rage") : crypt_str("AutoKey"), &vars.movement.autostrafetype, crypt_str("Rage\0AutoKey\0"));
					if (vars.movement.autostrafetype == 1)
					{
						ImGui::PushID(crypt_str("efjoeofjeoj"));
						ImGui::SliderInt(crypt_str("Mouse horizontal speed to strafe"), &vars.movement.dxtostrafe, 0, 50);
						ImGui::PopID();
					}
				}
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				ImGui::PushID(crypt_str("bbot"));
				vars.misc.blockbot.imgui(crypt_str("Blockbot"));
				ImGui::PopID();
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::SameLine();
				ImGui::PushID(crypt_str("pass"));
				vars.misc.peek_assist.imgui(crypt_str("Peek assistance"));
				ImGui::PopID();
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Edge Jump"), &vars.movement.edgejump);
				if (vars.movement.edgejump) {
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::PushID(crypt_str("ejej"));
					vars.movement.edgejumpkey.imgui(crypt_str("Edge Jump Key"));
					ImGui::PopID();
					ImGui::Checkbox(crypt_str("Edge Jump from Ladders"), &vars.movement.ladderej);
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Ladder Glide"), &vars.movement.ladderglide);
				if (vars.movement.ladderglide)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::PushID(crypt_str("ladderGLID"));
					vars.movement.ladderglidekey.imgui(crypt_str("Ladder Glide Key"));
					ImGui::PopID();
				}

				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Long Jump Bind"), &vars.movement.longjump);
				if (vars.movement.longjump)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::PushID(crypt_str("ljlj"));
					vars.movement.longjumpkey.imgui(crypt_str("Long Jump Bind Key"));
					ImGui::PopID();
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Mini Jump"), &vars.movement.minijump);
				if (vars.movement.minijump)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::PushID(crypt_str("mjmj"));
					vars.movement.minijumpkey.imgui(crypt_str("Mini Jump key"));
					ImGui::PopID();
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Jumpbug"), &vars.movement.jumpbug);
				if (vars.movement.jumpbug)
				{
					std::string jbtypenames[] = { crypt_str("Standard"), crypt_str("Hybrid"), crypt_str("Trace") };
					ImGui::Combo(jbtypenames[vars.movement.jumpbugtype].c_str(), &vars.movement.jumpbugtype, crypt_str("Standard\0Hybrid\0Trace\0"));
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::PushID(crypt_str("jbjb"));
					vars.movement.jumpbugkey.imgui(crypt_str("Jumpbug key"));
					ImGui::PopID();
					if (vars.movement.jumpbugtype == 1 || vars.movement.jumpbugtype == 2)
					{
						ImGui::PushID(crypt_str("dwddwdwdwdcccw"));
						ImGui::SliderFloat(crypt_str("Jumpbug check depth"), &vars.movement.lengthcheck, 3.f, 6.f, crypt_str("%.3f units"));
						ImGui::PopID();
					}
					
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Edgebug"), &vars.movement.edgebug.enabled);
				if (vars.movement.edgebug.enabled)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::PushID(crypt_str("eb1"));
					vars.movement.edgebug.key.imgui(crypt_str("Edgebug Key"));
					ImGui::PopID();
					ImGui::PushID(crypt_str("eb2"));
					ImGui::SliderInt(crypt_str("Tick Limit"), &vars.movement.edgebug.ticks, 0, 64);
					ImGui::PopID();
					ImGui::PushID(crypt_str("eb3"));
					ImGui::SliderFloat(crypt_str("Lock"), &vars.movement.edgebug.lock, 0.f, 1.f, "%.3f");
					ImGui::PopID();
					ImGui::Text(crypt_str("\tLock Type"));
					ImGui::Combo(&vars.movement.edgebug.locktype ? crypt_str("Static Lock") : crypt_str("Dynamic Lock"), &vars.movement.edgebug.locktype, crypt_str("Static Lock\0Dynamic Lock (New)\0"));
					//ImGui::Checkbox(crypt_str("Marker"), &vars.movement.edgebug.marker);
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("EB Detect - Effect"), &vars.movement.ebdetect.effect);
				ImGui::Checkbox(crypt_str("EB Detect - Chat"), &vars.movement.ebdetect.chat);
				ImGui::Checkbox(crypt_str("EB Detect - Counter"), &vars.movement.ebdetect.counter);
				ImGui::Checkbox(crypt_str("EB Detect - Sound"), &vars.movement.ebdetect.sound);
				if (vars.movement.ebdetect.sound)
				{
					ImGui::Dummy({ 0, 0 });
					ImGui::SameLine();
					ImGui::InputText(crypt_str("Filename"), vars.movement.ebdetect.sound_name, 32);
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Autoduck on pixelsurf"), &vars.movement.autopixelsurf);
				if (vars.movement.autopixelsurf)
				{
					ImGui::Dummy({ 0, 0 });
					ImGui::SameLine();
					vars.movement.autopixelsurfkey.imgui(crypt_str("Autoduck on pixelsurf key"));
					//ImGui::PushID(crypt_str("autopxmenudd"));
					//ImGui::SliderInt(crypt_str("Tick amount"), &vars.movement.autopixelsurf_ticks, 1, 16);
					//ImGui::PopID();
					ImGui::Checkbox(crypt_str("Freestanding on pixelsurf"), &vars.movement.autopixelsurf_freestand);
					//ImGui::Checkbox(crypt_str("Align to pixelsurf"), &vars.movement.autopixelsurf_align);
					ImGui::Checkbox(crypt_str("Align indicator"), &vars.movement.autopixelsurf_alignind);
					if (vars.movement.autopixelsurf_alignind)
						color_picker4(crypt_str("Indicator color"), vars.movement.autopixelsurf_indcol1);

				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Mouse speed limiter"), &vars.movement.msl);
				if (vars.movement.msl)
				{
					ImGui::PushID(crypt_str("13223sdddd"));
					ImGui::SliderFloat(crypt_str("Max horizontal speed"), &vars.movement.mslx, 0.f, 100.f, crypt_str("%.3f"));
					ImGui::PopID();
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Draw velocity"), &vars.movement.showvelocity);
				if (vars.movement.showvelocity)
				{
					ImGui::PushID(crypt_str("velpos"));
					ImGui::SliderFloat(crypt_str("Height"), &vars.movement.showvelpos, 0.f, 1.f, crypt_str("%.4f"));
					ImGui::PopID();
					ImGui::Checkbox(crypt_str("Takeoff velocity"), &vars.movement.showtakeoff);
					ImGui::Checkbox(crypt_str("Static color"), &vars.movement.staticvelcolor);
					if (vars.movement.staticvelcolor)
						color_picker4(crypt_str("Velocity color"), vars.movement.velcolor);
					ImGui::PushID(crypt_str("faddderoino2222"));
					ImGui::Checkbox(crypt_str("Fade by velocity"), &vars.movement.velocityfade);
					ImGui::PopID();
					if (vars.movement.velocityfade)
					{
						ImGui::PushID(crypt_str("velfadeTrololo"));
						ImGui::SliderFloat(crypt_str("Velocity to fade to"), &vars.movement.velfadevelocity, 1.f, 286.f, crypt_str("%.1f u/s"));
						ImGui::PopID();
					}
				}
				ImGui::Separator();
				ImGui::Checkbox(crypt_str("Show LJ Stats"), &vars.movement.ljstats);
				ImGui::Separator();

				ImGui::Checkbox(crypt_str("Draw velocity graph"), &vars.movement.graph.enabled);
				if (vars.movement.graph.enabled)
				{
					ImGui::Dummy(ImVec2(0.0f, 0.0f));
					ImGui::SameLine();
					ImGui::SliderInt(crypt_str("Size"), &vars.movement.graph.size, 32, 256);
					ImGui::SliderInt(crypt_str("Width"), &vars.movement.graph.linewidth, 1, 10);
					ImGui::SliderInt(crypt_str("Line thickness"), &vars.movement.graph.thickness, 1, 10);
					ImGui::SliderFloat(crypt_str("Height"), &vars.movement.graph.height, 0.f, 10.f);
					const std::string fadetypenames[] = {crypt_str("No Fade"), crypt_str("Fade start and end"), crypt_str("Fade by velocity"), crypt_str("Fade by start, end, velocity")};
					ImGui::Text(crypt_str("\tGraph Fade Type"));
					ImGui::Combo(fadetypenames[vars.movement.graph.fadetype].c_str(), &vars.movement.graph.fadetype, crypt_str("No Fade\0Fade start and end\0Fade by velocity\0Fade by start, end, velocity\0"));
					if (vars.movement.graph.fadetype == 2 || vars.movement.graph.fadetype == 3)
					{
						ImGui::PushID(crypt_str("grpher"));
						ImGui::SliderFloat(crypt_str("Max Fade Speed"), &vars.movement.graph.speedtofade, 1.f, 286.f, crypt_str("%.1f u/s"));
						ImGui::PopID();
					}
					//ImGui::SliderFloat(crypt_str("Speed"), &vars.movement.graph.speed, 0.1f, 5.f);

					color_picker4(crypt_str("Graph color"), vars.movement.graph.color);
					color_picker4(crypt_str("Text color"), vars.movement.graph.textcolor);
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();
			
		}
		break;
		case 6:
		{
			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(16);
			ImGui::MenuChild(crypt_str("Config"), ImVec2(345, 208), false, ImGuiWindowFlags_NoScrollbar);
			{
				if (!ConfigList.empty())
				{
					if (ImGui::BeginCombo("", ConfigList[vars.menu.active_config_index].c_str()))
					{
						for (int n = 0; n < ConfigList.size(); n++)
						{
							bool isselected = vars.menu.active_config_index == n;
							if (ImGui::Selectable(ConfigList.at(n).c_str(), isselected))
							{
								vars.menu.active_config_index = n;
							}

							if (isselected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}
				
				//ImGui::Combo(ConfigList.at(vars.menu.active_config_index).c_str(), &vars.menu.active_config_index, configs.c_str());

				ImGui::Dummy({ 0, 0 });
				ImGui::SameLine();
				ImGui::InputTextWithHint(crypt_str("Config name"), crypt_str("Input config name to create"), buffer, 32);
				
				


				vars.menu.active_config_name = std::string(buffer);
				if (!vars.menu.active_config_name.empty())
				{
					ImGui::Dummy({ 0, 0 });
					ImGui::SameLine();
					if (ImGui::Button(crypt_str("Create"), { 217, 23 }))
					{
						std::string add;
						if (vars.menu.active_config_name.find(crypt_str(".cfg")) == -1)
							add = crypt_str(".cfg");
						Config.Save(vars.menu.active_config_name + add);
						vars.menu.active_config_name = "";
						memset(buffer, 0, 32);
						RefreshConfigs();
					}
				}

				if (ConfigList.size() > 0 && vars.menu.active_config_index > -1)
				{
					ImGui::Dummy({ 0, 0 });
					ImGui::SameLine();
					if (ImGui::Button(crypt_str("Load"), { 217, 23 }))
					{
						vars.menu.active_config_name = ConfigList.at(vars.menu.active_config_index);
						Config.Load(ConfigList.at(vars.menu.active_config_index));
						memcpy(buffer, ConfigList.at(vars.menu.active_config_index).c_str(), max(ConfigList.at(vars.menu.active_config_index).size(), 32));
						Skinchanger::Get().scheduleHudUpdate();
					}

					ImGui::Dummy({ 0, 0 });
					ImGui::SameLine();
					if (ImGui::Button(crypt_str("Save"), { 217, 23 }))
					{
						Config.Save(ConfigList.at(vars.menu.active_config_index));
					}
				}

				ImGui::Dummy({ 0, 0 });
				ImGui::SameLine();
				if (ImGui::Button(crypt_str("Load defaults"), { 217, 23 }))
				{
					Config.ResetToDefault();
					//Skinchanger::Get().scheduleHudUpdate();
				}

				ImGui::Dummy({ 0, 0 });
				ImGui::SameLine();
				if (ImGui::Button(crypt_str("Refresh configs"), { 217, 23 }))
				{
					vars.menu.active_config_index = 0;
					RefreshConfigs();
				}

				ImGui::Dummy({ 0, 0 });
				ImGui::SameLine();
				if (ImGui::Button(crypt_str("Open config folder"), { 217, 23 }))
				{
					std::string BAM = crypt_str("start ");
					BAM += csgo->config_directory;
					system(BAM.c_str());
				}

			}
			ImGui::EndChild();
			ImGui::EndGroup();

			ImGui::SetCursorPosY(262);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(16);
			ImGui::MenuChild(crypt_str("Recorder"), ImVec2(345, 210), false, ImGuiWindowFlags_NoScrollbar);
			{
				MoveRecorder->Draw();
			}
			ImGui::EndChild();
			ImGui::EndGroup();


			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("Skin Changer"), ImVec2(345, 300), false, ImGuiWindowFlags_NoScrollbar);
			{
				static auto selected_id = 0;
				/*
				auto& entries = g_config.get_items();
				
				{
					ImGui::PushItemWidth(-1);

					char element_name[64];

					

					ImGui::ListBox("##config", &selected_id, [&element_name, &entries](int idx)
						{
							sprintf_s(element_name, "%s (%s)", entries.at(idx).name, game_data::weapon_names.at(entries.at(idx).definition_vector_index).name);
							return element_name;
						}, entries.size(), 11);

					const auto button_size = ImVec2(ImGui::GetColumnWidth() / 2 - 12.5f, 31);

					if (ImGui::Button("Add", button_size))
					{
						entries.push_back(item_setting());
						selected_id = entries.size() - 1;
					}
					ImGui::SameLine();

					if (ImGui::Button("Remove", button_size) && entries.size() > 1)
						entries.erase(entries.begin() + selected_id);

					ImGui::PopItemWidth();
				}
				
				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Weapon"));
				ImGui::Combo("", &selected_id, [](void* data, int idx, const char** out_text)
					{
						*out_text = game_data::weapon_names[idx].name;
						return true;
					}, nullptr, game_data::weapon_names.size(), 8);

				

				auto& selected_entry = g_config.get_items().at(selected_id);
				selected_entry.definition_vector_index = selected_id;

				{
					static char SkinFilter[32];

					

					// Enabled
					ImGui::Checkbox(crypt_str("Enabled"), &selected_entry.enabled);


					// Paint kit
					
					if (selected_entry.definition_index != GLOVE_T_SIDE)
					{
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Skin"));
						ImGui::PushID(crypt_str("P"));
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						ImGui::InputTextWithHint(crypt_str(""), crypt_str("Search for skin"), SkinFilter, 32);
						ImGui::PopID();
						ImGui::PushID(crypt_str("Paintkit"));
						if(ImGui::BeginCombo(crypt_str(""), game_data::skin_kits[selected_entry.paint_kit_vector_index].name.c_str()))
						{
							
							for (int i = 0; i < game_data::skin_kits.size(); i++)
							{
								bool is_selected = (selected_entry.paint_kit_vector_index == i);
								std::string Skrrt = SkinFilter;

								if (!Skrrt.empty())
								{
									if (game_data::skin_kits[i].name.find(Skrrt) == string::npos)
										continue;
								}

								
								//std::string label = game_data::skin_kits[i].name + crypt_str("###food") + to_string(i);

								ImGui::PushID(i * 11);

								if (ImGui::Selectable(game_data::skin_kits[i].name.c_str(), &is_selected))
								{
									selected_entry.paint_kit_vector_index = i;
									//memset(SkinFilter, 0, 32);
								}
									
								ImGui::PopID();
								

								if (is_selected)
									ImGui::SetItemDefaultFocus();
								
							}
							ImGui::EndCombo();
						}
						ImGui::PopID();
					}
					else
					{
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Skin"));
						ImGui::PushID(crypt_str("Peee"));
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						ImGui::InputTextWithHint(crypt_str(""), crypt_str("Search for skin"), SkinFilter, 32);
						ImGui::PopID();
						ImGui::PushID(crypt_str("Glovekit"));
						if (ImGui::BeginCombo(crypt_str(""), game_data::glove_kits[selected_entry.paint_kit_vector_index].name.c_str()))
						{
							for (int i = 0; i < game_data::glove_kits.size(); i++)
							{
								bool is_selected = (selected_entry.paint_kit_vector_index == i);
								std::string Skrrt = SkinFilter;

								if (!Skrrt.empty())
								{
									if (game_data::glove_kits[i].name.find(Skrrt) == string::npos)
										continue;
								}

								
								//std::string label = game_data::glove_kits[i].name + crypt_str("###foo") + to_string(i);

								ImGui::PushID(i * 23);

								if (ImGui::Selectable(game_data::glove_kits[i].name.c_str(), &is_selected))
								{
									selected_entry.paint_kit_vector_index = i;
									//memset(SkinFilter, 0, 32);
								}
								
								ImGui::PopID();

								if (is_selected)
									ImGui::SetItemDefaultFocus();

							}
							ImGui::EndCombo();
						}
						ImGui::PopID();
					}
					
					// Yes we do it twice to decide knifes
					selected_entry.update<sync_type::KEY_TO_VALUE>();

					ImGui::PushID(crypt_str("knif/glov"));
					// Item defindex override
					if (selected_entry.definition_index == WEAPON_KNIFE)
					{
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Knife"));
						ImGui::Combo("", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
							{
								*out_text = game_data::knife_names.at(idx).name;
								return true;
							}, nullptr, game_data::knife_names.size(), 5);
					}
					else if (selected_entry.definition_index == GLOVE_T_SIDE)
					{
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Glove"));
						ImGui::Combo("", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
							{
								*out_text = game_data::glove_names.at(idx).name;
								return true;
							}, nullptr, game_data::glove_names.size(), 5);
					}
					else
					{
						// We don't want to override weapons other than knives or gloves
						
						static auto unused_value = 0;
						selected_entry.definition_override_vector_index = 0;
						
						//ImGui::Combo(crypt_str("------------"), &unused_value, crypt_str("For knives or gloves\0"));
					}
					ImGui::PopID();

					selected_entry.update<sync_type::KEY_TO_VALUE>();

					// Quality
					ImGui::Dummy({ 0,0 });
					ImGui::SameLine();
					ImGui::PushID(crypt_str("qualqual"));
					ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Prefix"));
					ImGui::Combo("", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = game_data::quality_names[idx].name;
							return true;
						}, nullptr, game_data::quality_names.size(), 5);
					ImGui::PopID();

					ImGui::SliderFloat(crypt_str("Wear"), &selected_entry.wear, FLT_MIN, 1.f, "%.10f", ImGuiSliderFlags_None);
					ImGui::Dummy({ 0,0 });
					ImGui::SameLine();
					ImGui::InputInt(crypt_str("StatTrak"), &selected_entry.stat_trak);
					ImGui::Dummy({ 0,0 });
					ImGui::SameLine();
					ImGui::InputInt(crypt_str("Seed"), &selected_entry.seed);
					ImGui::Dummy({ 0,0 });
					ImGui::SameLine();
					ImGui::InputText(crypt_str("Name Tag"), selected_entry.custom_name, 32);
					
				}

				ImGui::Separator();

				/*
				ImGui::PushID("sticker");

				static auto selected_sticker_slot = 0;

				auto& selected_sticker = selected_entry.stickers[selected_sticker_slot];

				ImGui::PushItemWidth(-1);

				char element_name[64];

				ImGui::ListBox("", &selected_sticker_slot, [&selected_entry, &element_name](int idx)
					{
						auto kit_vector_index = selected_entry.stickers[idx].kit_vector_index;
						sprintf_s(element_name, "#%d (%s)", idx + 1, game_data::sticker_kits.at(kit_vector_index).name.c_str());
						return element_name;
					}, 5, 5);
				ImGui::PopItemWidth();

				ImGui::Combo("Sticker Kit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = game_data::sticker_kits.at(idx).name.c_str();
						return true;
					}, nullptr, game_data::sticker_kits.size(), 10);

				ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.f, "%.10f");

				ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.f, "%.3f");

				ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.f, 360.f);

				ImGui::PopID();
				

				

				// Lower buttons for modifying items and saving
				{
					ImGui::Dummy({ 0,0 });
					ImGui::SameLine();
					if (ImGui::Button(crypt_str("Update"), { 217, 23 }))
					{
						Skinchanger::Get().scheduleHudUpdate();
						
					}
						//(*g_client_state)->ForceFullUpdate();
				}

				

			}
			ImGui::EndChild(false, true);
			ImGui::EndGroup();


			ImGui::SetCursorPosY(355);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("Model Changer"), ImVec2(345, 116), false, ImGuiWindowFlags_NoScrollbar);
			{
				static int DD{ vars.visuals.playermodelct };
				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				ImGui::Text("CT Model");
				ImGui::Combo(crypt_str("Label"), &vars.visuals.playermodelct, crypt_str("Default\0Agent Ava\0Operator\0Markus Delrow\0Michael Syfers\0Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Ricksaw\0Third Commando Company\0McCoy\0Dragomir\0Rezan The Ready\0Romanov\0Maximus\0Blackwolf\0Mr.Muhlik\0Ground Rebel\0Osiris\0Prof. Shahmat\0Enforcer\0Slingshot\0Soldier\0Pirate 1\0Pirate 2\0Pirate 3\0Pirate 4\0Pirate 5\0Anarchist 1\0Anarchist 2\0Anarchist 3\0Anarchist 4\0Anarchist 5\0Balkan 1\0Balkan 2\0Balkan 3\0Balkan 4\0Balkan 5\0Jumpsuit 1\0Jumpsuit 2\0Jumpsuit 3\0Street Soldier\0Buckshot\0McCoy\0Rezan\0Dragomir\0Jamison\0Farlow\0John Kask\0Bio-Haz Specialis\0Sergeant Bombson\0Chem-Haz Specialist\0Sir Bloody Miami Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Skullhead Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Loudmouth Darryl\0Safecracker Voltzmann\0Little Kev\0Number K\0Getaway Sally\0"));
				if (DD != vars.visuals.playermodelct)
				{
					Skinchanger::Get().scheduleHudUpdate();
					DD = vars.visuals.playermodelct;
				}
					


				static int CC{vars.visuals.playermodelt};
				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				ImGui::Text("T Model");
				ImGui::Combo(crypt_str("LabelE"), &vars.visuals.playermodelt, crypt_str("Default\0Agent Ava\0Operator\0Markus Delrow\0Michael Syfers\0Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Ricksaw\0Third Commando Company\0McCoy\0Dragomir\0Rezan The Ready\0Romanov\0Maximus\0Blackwolf\0Mr.Muhlik\0Ground Rebel\0Osiris\0Prof. Shahmat\0Enforcer\0Slingshot\0Soldier\0Pirate 1\0Pirate 2\0Pirate 3\0Pirate 4\0Pirate 5\0Anarchist 1\0Anarchist 2\0Anarchist 3\0Anarchist 4\0Anarchist 5\0Balkan 1\0Balkan 2\0Balkan 3\0Balkan 4\0Balkan 5\0Jumpsuit 1\0Jumpsuit 2\0Jumpsuit 3\0Street Soldier\0Buckshot\0McCoy\0Rezan\0Dragomir\0Jamison\0Farlow\0John Kask\0Bio-Haz Specialis\0Sergeant Bombson\0Chem-Haz Specialist\0Sir Bloody Miami Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Skullhead Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Loudmouth Darryl\0Safecracker Voltzmann\0Little Kev\0Number K\0Getaway Sally\0"));
				if (CC != vars.visuals.playermodelt)
				{
					Skinchanger::Get().scheduleHudUpdate();
					CC = vars.visuals.playermodelt;
				}
					


				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				if (ImGui::Button(crypt_str("Update"), { 217, 23 }))
				{
					Skinchanger::Get().scheduleHudUpdate();
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		break;

		case 7:
		{
			ImGui::SetCursorPosY(50);
			ImGui::PushID(crypt_str("hello team"));
			ImGui::BeginGroup();
			ImGui::PopID();
			ImGui::SetCursorPosX(16);
			ImGui::MenuChild(crypt_str("LUA Scripting"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{

				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				ImGui::Text(crypt_str("Lua scripting disabled temporarily due to instability"));

				/*
				if (!CLua::Get().scripts.empty())
				{

					if (ImGui::BeginCombo(crypt_str("##LUACOMBO"), CLua::Get().scripts.at(lua_current_item).c_str()))
					{

						for (int n = 0; n < CLua::Get().scripts.size(); n++)
						{
							const bool is_selected = (lua_current_item == n);

							if (CLua::Get().loaded.at(n))
							{
								std::string txt = CLua::Get().scripts.at(n) + crypt_str(" [ Running ]");
								if (ImGui::Selectable(txt.c_str(), is_selected))
									lua_current_item = n;
							}
							else
							{
								if (ImGui::Selectable(CLua::Get().scripts.at(n).c_str(), is_selected))
									lua_current_item = n;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				
				
					if (CLua::Get().loaded.at(lua_current_item))
					{
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						if (ImGui::Button(crypt_str("Unload script"), { 217, 23 }))
						{
							CLua::Get().unload_script(lua_current_item);
							CLua::Get().refresh_scripts();
						}
					}
					else
					{
						ImGui::Dummy({ 0,0 });
						ImGui::SameLine();
						if (ImGui::Button(crypt_str("Load script"), { 217, 23 }))
						{
							CLua::Get().load_script(lua_current_item);
							CLua::Get().refresh_scripts();
						}
					}
				}

				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				if (ImGui::Button(crypt_str("Refresh scripts"), { 217, 23 }))
				{
					CLua::Get().refresh_scripts();
				}

				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				if (ImGui::Button(crypt_str("Open LUA folder"), { 217, 23 }))
				{
					std::string BAM = crypt_str("start ");
					BAM += csgo->config_directory;
					BAM += crypt_str("Scripts\\");
					system(BAM.c_str());
				}


				ImGui::Dummy({ 0,0 });
				ImGui::SameLine();
				std::string texto = crypt_str("Scripts loaded:\n");
				for (int n = 0; n < CLua::Get().scripts.size(); n++)
				{
					if (CLua::Get().loaded.at(n))
						texto += CLua::Get().scripts.at(n) + crypt_str("\n");
				}
				ImGui::Text(texto.c_str());
				

			}
			ImGui::EndChild();
			ImGui::EndGroup();

			ImGui::SetCursorPosY(50);
			ImGui::BeginGroup();
			ImGui::SetCursorPosX(369);
			ImGui::MenuChild(crypt_str("LUA Items"), ImVec2(345, 423), false, ImGuiWindowFlags_NoScrollbar);
			{
				if (csgo->script_loaded)
					for (auto current : CLua::Get().hooks.get_hooks(crypt_str("menu_items")))
						current.func();
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		break;
		}

		ImGui::PopFont();
	}
	ImGui::End();
}

*/

int lefthash = std::hash<std::string>{}(crypt_str("Left"));
int righthash = std::hash<std::string>{}(crypt_str("Right"));
int forwardhash = std::hash<std::string>{}(crypt_str("Forward"));
int backhash = std::hash<std::string>{}(crypt_str("Back"));

void c_menu::update_binds()
{
 	if (GetForegroundWindow() != csgo->Init.Window)
	{
		return;
	}

	if (Misc::Get().IsChatOpened() || interfaces.engine->Con_BoneIsVisible()) {
		return;
	}

	for (auto keybind : keybinds)
	{
		keybind->key = std::clamp(keybind->key, 0, 255);
		if (keybind->key != 0)
		{
			if (keybind->special) 
			{
				if (!vars.antiaim.manual_antiaim || !vars.antiaim.enable) {
					keybind->state = false;
					continue;
				}

				int namehash = std::hash<std::string>{}(keybind->name);

				keybind->type = 2;
				if (keybind->key > 0 && window->key_updated(keybind->key)) {
					if (namehash == lefthash) {
						keybind->state = !keybind->state;
						if (keybind->state) {
							vars.antiaim.manual_back.state = false;
							vars.antiaim.manual_right.state = false;
							vars.antiaim.manual_forward.state = false;
						}
					}
					else if (namehash == righthash) {
						keybind->state = !keybind->state;
						if (keybind->state) {
							vars.antiaim.manual_back.state = false;
							vars.antiaim.manual_left.state = false;
							vars.antiaim.manual_forward.state = false;
						}
					}
					else if (namehash == backhash) {
						keybind->state = !keybind->state;
						if (keybind->state) {
							vars.antiaim.manual_left.state = false;
							vars.antiaim.manual_right.state = false;
							vars.antiaim.manual_forward.state = false;
						}
					}
					else if (namehash == forwardhash) {
						keybind->state = !keybind->state;
						if (keybind->state) {
							vars.antiaim.manual_left.state = false;
							vars.antiaim.manual_back.state = false;
							vars.antiaim.manual_right.state = false;
						}
					}

				}
			}
			else
			{
				switch (keybind->type)
				{
				case 0: // Always off
					keybind->state = false;
					break;
				case 1: // Hold
					keybind->state = csgo->key_pressed[keybind->key];
					break;
				case 2: // Toggle
					if (window->key_updated(keybind->key))
						keybind->state = !keybind->state;
					break;
				case 3: // Release
					keybind->state = !csgo->key_pressed[keybind->key];
					break;
				case 4: // Always on
					keybind->state = true;
					break;
				}
			}
		}
		else
		{
			switch (keybind->type)
			{
			case 0: // Always off
				keybind->state = false;
				break;
			case 4: // Always on
				keybind->state = true;
				break;
			}
		}
	}

	/*
	for (auto e : window->elements) {
		if (e->type == c_elementtype::child) {
			for (auto el : ((c_child*)e)->elements) {
				if (el->type == c_elementtype::keybind) {
					auto binder = ((c_keybind*)el)->bind;
					auto name = ((c_keybind*)el)->get_label();
					if (binder) {
						if (name == crypt_str("Left") || name == crypt_str("Right") || name == crypt_str("Back") || name == crypt_str("Forward")) {
							if (!vars.antiaim.manual_antiaim || !vars.antiaim.enable) {
								binder->active = false;
								continue;
							}
							binder->type = 2;
							if (binder->key > 0 && window->key_updated(binder->key)) {
								if (name == crypt_str("Left")) {
									binder->active = !binder->active;
									if (binder->active) {
										vars.antiaim.manual_back.active = false;
										vars.antiaim.manual_right.active = false;
										vars.antiaim.manual_forward.active = false;
									}
								}
								else if (name == crypt_str("Right")) {
									binder->active = !binder->active;
									if (binder->active) {
										vars.antiaim.manual_back.active = false;
										vars.antiaim.manual_left.active = false;
										vars.antiaim.manual_forward.active = false;
									}
								}
								else if (name == crypt_str("Back")) {
									binder->active = !binder->active;
									if (binder->active) {
										vars.antiaim.manual_left.active = false;
										vars.antiaim.manual_right.active = false;
										vars.antiaim.manual_forward.active = false;
									}
								}
								else if (name == crypt_str("Forward")) {
									binder->active = !binder->active;
									if (binder->active) {
										vars.antiaim.manual_left.active = false;
										vars.antiaim.manual_back.active = false;
										vars.antiaim.manual_right.active = false;
									}
								}
							}

						}
						else {
							binder->key = std::clamp<unsigned int>(binder->key, 0, 255);

							if (binder->type == 2 && binder->key > 0) {
								if (window->key_updated(binder->key)) {
									binder->active = !binder->active;
								}
							}
							else if (binder->type == 1 && binder->key > 0) {
								binder->active = csgo->key_pressed[binder->key];
							}
							else if (binder->type == 3 && binder->key > 0) {
								binder->active = !csgo->key_pressed[binder->key];
							}
							else if (binder->type == 0)
								binder->active = false;
							else if (binder->type == 4)
								binder->active = true;
						}
					}
				}
			}
		}
	}

*/


	if (vars.misc.restrict_type == 0)
	{
		vars.ragebot.double_tap.state = false;
		vars.ragebot.hideShots.state = false;
		vars.antiaim.fakeduck.state = false;
		//vars.antiaim.fakelagfactor = std::clamp(vars.antiaim.fakelagfactor, 1, 8);
	}
}

void c_menu::reinit_config() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == crypt_str("Configurations")) {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}

	RefreshConfigs();
	auto config_child = new c_child(crypt_str("Configurations"), tab_t::misc, window);
	config_child->set_size(Vector2D(g_size + 90, 520));
	config_child->set_position(Vector2D(g_size + 98, 0)); {
		config_child->add_element(new c_listbox(crypt_str("Configurations"), &vars.menu.active_config_index, ConfigList, 150.f, false));
		config_child->add_element(new c_input_text(crypt_str("Configuration name"), &vars.menu.active_config_name, false));

		config_child->add_element(new c_button(crypt_str("Load"), []() {
			Config.Load(ConfigList[vars.menu.active_config_index]);
			}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button(crypt_str("Save"), []() {
			Config.Save(ConfigList[vars.menu.active_config_index]);
			}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button(crypt_str("Refresh"), []() {
			g_Menu->should_reinit_config = true;
			}));

		config_child->add_element(new c_button(crypt_str("Create"), []() {
			string add;
			if (vars.menu.active_config_name.find(crypt_str(".cfg")) == -1)
				add = crypt_str(".cfg");
			Config.Save(vars.menu.active_config_name + add);
			g_Menu->should_reinit_config = true;
			vars.menu.active_config_name.clear();
			}, []() { return vars.menu.active_config_name.size() > 0; }));

		config_child->add_element(new c_button(crypt_str("Reset to default"), []() { Config.ResetToDefault(); },
			[]() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->initialize_elements();
	}
	window->add_element(config_child);
}

bool TabAlt(ImageData image, bool selected, const char* bigid, ImVec2 min_offset, ImVec2 max_offset)
{

	ImGuiContext& g = *ImGui::GetCurrentContext();
	ImGuiWindow* window = g.CurrentWindow;
	if (window->SkipItems)
		return false;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(bigid);
	const ImVec2 label_size = { 75, 60 };
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(ImVec2(75, 60), 50 + style.FramePadding.x, label_size.y + style.FramePadding.y);
	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	if (pressed)
		ImGui::MarkItemEdited(id);
	auto draw = ImGui::GetWindowDrawList();
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	float t = selected ? 1.0f : 0.0f;
	float ANIM_SPEED = 0.39f; // Bigger = Slower
	if (g.LastActiveId == g.CurrentWindow->GetID(bigid)) {
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = selected ? (t_anim) : (0);
	}
	int a = int(t * 255);
	auto text_color = ImGui::GetColorU32(ImLerp(ImVec4(180 / 255.f, 180 / 255.f, 180 / 255.f, 180 / 255.f), ImVec4(0 / 255.f, 255 / 255.f, 128 / 255.f, 255.f / 255.f), t));
	draw->AddRectFilled(ImVec2(bb.Min.x, bb.Max.y - 2), bb.Max, ImColor(0, 255, 128, a));
	draw->AddImage(image.texture, ImVec2{ pos.x + min_offset.x, pos.y + min_offset.y }, { pos.x + 70.f + max_offset.x , pos.y + 60.f + max_offset.y }, { 0,0 }, { 1,1 }, text_color);
	//draw->AddText(ImVec2(pos.x + 75 / 2 - label_size.x / 2, pos.y + 30 - label_size.y / 2), text_color, label);
	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
	return pressed;
}


std::string to_lower_string(const std::string& str)
{
	std::string lower;
	lower.resize(str.size());
	for (int i = 0; i < str.size(); i++)
	{
		lower[i] = std::tolower(str[i]);
	}


	return lower;
}

ImVec2 window_size;
ImVec2 window_pos;
bool rage_or_aa = true;
int material = 0;
static int currentCategory{ 0 };
static std::string material_labels[] = { crypt_str("Normal"),crypt_str("Flat"),crypt_str("Animated"),crypt_str("Platinum"),crypt_str("Glass"),crypt_str("Chrome"),crypt_str("Crystal"),crypt_str("Silver"), crypt_str("Gold"),crypt_str("Plastic"),crypt_str("Glow"),crypt_str("Pearlescent"),crypt_str("Metallic") };
static std::string cham_labels[] = { crypt_str("Allies"), crypt_str("Enemies"), crypt_str("Backtrack"), crypt_str("Local player"), crypt_str("Weapons"), crypt_str("Hands"),  crypt_str("Sleeves"), crypt_str("Desync")};
static std::string mat_labels[] = { crypt_str("1"), crypt_str("2") };
static int currentItem{ 0 };

static std::array<std::string, 8>removals{ crypt_str("Visual recoil"), crypt_str("Smoke"), crypt_str("Flash"), crypt_str("Scope"), crypt_str("Zoom"), crypt_str("Post processing"), crypt_str("Fog"), crypt_str("Shadows") };
static bool removal[removals.size()] = { false, false, false, false, false, false, false, false };
std::string removal_label = crypt_str("");
static std::string beam_labels[] = { crypt_str("Default"), crypt_str("Phys beam") };
static std::string restrict_labels[] = { crypt_str("MM"), crypt_str("HvH"), crypt_str("KZ") };
static std::string voteprint_label[] = { crypt_str("Team"), crypt_str("All") };
static std::string clantag_labels[] = { crypt_str("Static"), crypt_str("Animated"), crypt_str("Custom"), crypt_str("Clock") };
//crypt_str("None\0Autosniper\0Scout\0AWP\0Negev\0M249\0Rifle\0AUG/SG553\0")
static std::string autobuyprimary_labels[] = { crypt_str("None"), crypt_str("Autosniper"), crypt_str("Scout"),crypt_str("AWP"), crypt_str("Negev"), crypt_str("M249"), crypt_str("Rifle"),crypt_str("AUG/SG553") };
//crypt_str("None\0Dualies\0P250\0Five-seveN\0Deagle/R8 Revolver\0")
static std::string autobuysecondary_labels[] = { crypt_str("None"),crypt_str("Dualies"),crypt_str("P250"),crypt_str("Five-SeveN"), crypt_str("Deagle/R8") };

static std::string utility[]{ crypt_str("Kevlar & helmet"), crypt_str("Kevlar"), crypt_str("Frag grenade"), crypt_str("Molotov"), crypt_str("Smoke"), crypt_str("Taser"), crypt_str("Defuser") };
static bool util[7] = { false, false, false, false, false, false, false };
static std::string utility_label = crypt_str("");
static std::string keyname;
static bool menukeybindopened;
static std::string jbtypenames[] = { crypt_str("Standard"), crypt_str("Hybrid"), crypt_str("Trace") };
// crypt_str("Static Lock\0Dynamic Lock (New)\0")
static std::string locktype_labels[] = { crypt_str("Static lock"), crypt_str("Dynamic Lock (New)") };
static std::string fadetypenames[] = { crypt_str("No Fade"), crypt_str("Fade start and end"), crypt_str("Fade by velocity"), crypt_str("Fade by start, end, velocity") };
static int selected_id = 0;
char* SkinFilter = new char[32];
static std::string ebdetecteffect_label = crypt_str("");
static std::string ebdetect_labels[] = { crypt_str("Effect"), crypt_str("Chat"), crypt_str("Counter"), crypt_str("Sound") };
static std::string autostrafetype_labels[] = { crypt_str("Rage"), crypt_str("Only Keypress") };
static std::string veltypelabels[] = { crypt_str("Below"), crypt_str("Right"), crypt_str("Left"), crypt_str("Above") };
static std::string icontypelabels[] = { crypt_str("Text"), crypt_str("Icons") };
static std::string velgraphselectlabels[] = { crypt_str("Velocity"), crypt_str("Stamina") };
const std::string ind_labels[] = { crypt_str("Edgebug"), crypt_str("Edgejump"), crypt_str("Jumpbug"), crypt_str("Ladderglide"), crypt_str("Longjump bind"), crypt_str("Mini jump"), crypt_str("Auto Pixelsurf"), crypt_str("Auto Align") };
int parar = 0;


auto& entries = g_config.get_items();
item_setting selected_entry;

void AltBeginChild(ImVec2 min, ImVec2 max, std::string title)
{
	ImGui::GetWindowDrawList()->AddRectFilled({ window_pos.x + min.x , window_pos.y + min.y }, { window_pos.x + max.x, window_pos.y + max.y }, ImColor(30.f / 255.f, 30.f / 255.f, 31.f / 255.f, 1.f), 3.f);
	ImGui::PushFont(fonts::logger);
	ImVec2 TitleSize = ImGui::CalcTextSize(title.c_str());
	ImGui::SetCursorPosX((min.x + max.x) / 2.f - TitleSize.x / 2.f);
	ImGui::SetCursorPosY(min.y + 2);
	ImGui::Text(title.c_str());
	ImGui::PopFont();
	ImGui::GetWindowDrawList()->AddRect({ window_pos.x + min.x , window_pos.y + min.y}, { window_pos.x + max.x , window_pos.y + min.y + TitleSize.y + 5 }, ImColor(38.f / 255.f, 38.f / 255.f, 39.f / 255.f, 1.f), 3.f);
	ImGui::SetCursorPosX(min.x + 5);
	ImGui::SetCursorPosY(min.y + 10 + TitleSize.y);

	ImGui::BeginChild(title.c_str(), { (max.x - min.x) - 5, (max.y - min.y) - 15 - TitleSize.y }, false, 0);
	//ImGui::Spacing();

}

void AltEndChild()
{
	ImGui::EndChild();
}


void AltCombo(std::string title, std::string* labels, int size, int& curselection)
{
	ImGui::Text(title.c_str());
	ImGui::SetNextItemWidth(305.f);
	if (ImGui::BeginCombo(title.c_str(), labels[curselection].c_str()))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
		for (int i = 0; i < size; i++)
		{
			bool selected = (curselection == i);
			if (ImGui::Selectable(labels[i].c_str(), &selected, 0, { 305.f,16.f }))
				curselection = i;
		}
		ImGui::EndCombo();
	}
	

	
}


void AltMultiCombo(std::string title, std::string* labels, bool* selections, int itemcount)
{
	ImGui::Text(title.c_str());
	std::string label = "";
	for (int i = 0; i < itemcount; i++)
	{
		if (selections[i])
			label += (i == itemcount - 1) ? labels[i] : labels[i] + crypt_str(", ");
	}


	ImGui::SetNextItemWidth(305.f);
	if (ImGui::BeginCombo(title.c_str(), label.c_str()))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
		for (int i = 0; i < itemcount; i++)
		{
			ImGui::Selectable(labels[i].c_str(), &selections[i], ImGuiSelectableFlags_DontClosePopups, { 305.f,16.f });
		}

		ImGui::EndCombo();
	}

}

std::array<float,3> colorconvert(int hexValue)
{
	std::array<float, 3> rgbColor;
	rgbColor[2] = ((hexValue >> 16) & 0xFF) / 255.0;
	rgbColor[1] = ((hexValue >> 8) & 0xFF) / 255.0;
	rgbColor[0] = ((hexValue) & 0xFF) / 255.0;
	return rgbColor;
}


void c_menu::renderNewMenu()
{
	if (!ImGui::GetCurrentContext())
		return;

	static bool once = false;
	if (!once)
	{
		Config.ResetToDefault();
		vars.menu.open = true;
		memset(buffer, 0, 32);
		memset(SkinFilter, 0, 32);
		RefreshConfigs();
		init_keybinds();
		window = new c_window();

		once = true;
	}


	if (!vars.menu.open)
		return;

	auto& style = ImGui::GetStyle();

	style.WindowRounding = 4.f;
	style.WindowTitleAlign = { 0.5, 0.5 };
	style.WindowBorderSize = 0.f;
	style.WindowPadding = ImVec2{ 0.f,0.f };
	style.ChildRounding = 4.f;
	style.FrameRounding = 4.f;
	style.FrameBorderSize = 1.f;

	ImVec2 display_size = ImGui::GetIO().DisplaySize;

	if (display_size.x < 1920 && display_size.y < 1080)
	{
		ImGui::SetNextWindowSize({ 650.f , 720.f / 1080.f * display_size.y });
		window_size = { 650.f , 720.f / 1080.f * display_size.y };
	}
	else
	{
		ImGui::SetNextWindowSize({ 650.f, 720.f });
		window_size = { 650.f, 720.f };
	}

	ImGui::SetNextWindowPos({ 100,100 }, ImGuiCond_Once);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
	
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.f, 1.f, 0.5f, 1.f));
	//selectable colors
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));
	


	ImGui::Begin(crypt_str("Hello, world!"), nullptr, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	window_pos = ImGui::GetWindowPos();


	/*
	//fade
	ImGui::GetWindowDrawList()->AddRectFilledMultiColor({ window_pos.x, window_pos.y }, { window_pos.x + 75.f,  window_pos.y + 60.f }, ImColor(29.f / 255.f, 88.f / 255.f, 95.f / 255.f, 1.f), ImColor(30.f / 255.f, 41.f / 255.f, 56.f / 255.f, 1.f), ImColor(30.f / 255.f, 41.f / 255.f, 56.f / 255.f, 1.f), ImColor(29.f / 255.f, 88.f / 255.f, 95.f / 255.f, 1.f));


	// two trapezoids :) so we cover the gradient rect with a nice angle :)
	std::array<ImVec2, 4> polypoints = { // lighter colored
		ImVec2{window_pos.x + 75.f, window_pos.y}, // top left
		ImVec2{window_pos.x + window_size.x, window_pos.y}, // top right
		ImVec2{window_pos.x + window_size.x, window_pos.y + 60.f}, // bottom right
		ImVec2{window_pos.x + 60.f, window_pos.y + 60.f} // bottom left
	};

	ImGui::GetWindowDrawList()->AddConvexPolyFilled(polypoints.data(), 4, ImColor(27.f / 255.f, 37.f / 255.f, 51.f / 255.f, 1.f));

	polypoints = { //darker colored
		ImVec2{window_pos.x + 80.f, window_pos.y}, // top left
		ImVec2{window_pos.x + window_size.x, window_pos.y}, // top right
		ImVec2{window_pos.x + window_size.x, window_pos.y + 60.f}, // bottom right
		ImVec2{window_pos.x + 65.f, window_pos.y + 60.f} // bottom left
	};

	ImGui::GetWindowDrawList()->AddConvexPolyFilled(polypoints.data(), 4, ImColor(23.f / 255.f, 30.f / 255.f, 41.f / 255.f, 1.f));
	*/


	//logo drawing
	int savedflags = ImGui::GetWindowDrawList()->Flags;
	ImGui::GetWindowDrawList()->Flags |= (ImDrawListFlags_AntiAliasedLinesUseTex | ImDrawListFlags_AntiAliasedFill | ImDrawListFlags_AntiAliasedLines);
	ImGui::GetWindowDrawList()->AddImage(LoadedImages::xtclogo.texture, ImVec2{ window_pos.x - 4.f, window_pos.y - 9.f }, ImVec2{ window_pos.x + 72.f, window_pos.y + 75.f });
	ImGui::GetWindowDrawList()->Flags = savedflags;

	ImGui::SetCursorPosX(70);

	// tabs

	const std::string tabnames[]{ crypt_str("Legitbot"), crypt_str("Ragebot"), crypt_str("Visuals"), crypt_str("World"), crypt_str("Misc"), crypt_str("Config"), crypt_str("LUA") };
	CGlobalVariables::Chams::Material& chams{ vars.chams[currentItem].materials[material] };

	ImGui::BeginGroup();
	ImGui::PushFont(XtcFonts::tab_names);

	const std::pair<ImVec2, ImVec2> tabimageoffsets[7] = // offsets for the pictures on the tabs so theyre centered
	{
		{{13.f,2.f}, {0.f,-5.f}}, // legitbot
		{{12.f,3.f}, {-4.f,-5.f}}, // ragebot
		{{10.f,3.f}, {-4.f,1.f}}, // visuals
		{{5.f,-1.f}, {1.f,-1.f}}, // world
		{{2.f,-6.f}, {3.f,5.f}}, // misc
		{{10.f,3.f}, {-5.f,-5.f}}, // config
		{{8.f,2.f}, {-1.f,-3.f}}  // lua
	};

	for (int i = 0; i < 7; i++)
	{
		bool selected = (vars.menu.current_tab == i);
		if (TabAlt(LoadedImages::tabIcons[i], selected, tabnames[i].c_str(), tabimageoffsets[i].first, tabimageoffsets[i].second))
		{
			vars.menu.current_tab = i;
		}

		if (i != 6)
			ImGui::SameLine();
	}
	ImGui::PopFont();
	ImGui::EndGroup();

	ImGui::Spacing();
	ImGui::Spacing();

	//local defs
	//static std::string Penises[]{ "Black",  "White", "Asian(Small)", "Voidzero Dick ( BIG )", "Tabascofarmer Dick (WTF BIg!!!!!!)" };
	//static std::string Vaginas[]{ "Black",  "White", "Asian(Tight)" };
	
	//tabs

	ImGui::PushFont(fonts::font);
	ImVec2 lgbt_txt_sz = ImGui::CalcTextSize(tabnames[0].c_str());

	style.WindowPadding.x = 5.f;
	style.FramePadding = { 0.f, 2.f };
	style.FrameRounding = 3.f;
	//style.ItemInnerSpacing.x = 10.f;
	//style.SelectableTextAlign = { 0.1f, 0.f };

	const int menufont_height = 13;

	switch (vars.menu.current_tab)
	{
	case 0: //legit bot
		AltBeginChild({ 5 ,  65 }, { 5 + ((window_size.x - 10) / 2), 35 + (window_size.x / 2) }, tabnames[0]);

		ImGui::Checkbox(crypt_str("Aimbot"), &vars.legitbot.aimbot);
		if (vars.legitbot.aimbot)
		{

			ImGui::Checkbox(crypt_str("On Key"), &vars.legitbot.onkey);
			if (vars.legitbot.onkey)
			{
				vars.legitbot.key.imgui(crypt_str("Legitbot Key"));
				//ImGui::Text("Keybind Placeholder");
				//ImGui::Dummy(ImVec2(0.0f, 0.0f));
				//ImGui::SameLine();
				//vars.legitbot.key.imgui(crypt_str("Legitbot Key"));
			}

			ImGui::Checkbox(crypt_str("Target Teammates"), &vars.legitbot.teammates);
			//ImGui::Checkbox(crypt_str("RCS Standalone"), &vars.legitbot.rcsstandalone);
		}

		ImGui::Checkbox(crypt_str("Auto Pistol"), &vars.legitbot.autopistol);

		ImGui::Checkbox(crypt_str("Backtrack"), &vars.legitbot.backtrack);

		if (vars.legitbot.backtrack)
		{
			ImGui::PushID(crypt_str("polsjdsd"));
			ImGui::Text(crypt_str("Backtrack max length"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Backtrack max lengtheringsexing"), &vars.legitbot.btlength, 0.f, 0.2f, crypt_str("%.3fs"));
			ImGui::PopID();
		}

		AltEndChild();

		AltBeginChild({ 5, 40 + (window_size.x / 2) }, { 5 + ((window_size.x - 10) / 2),  window_size.y - 10 }, crypt_str("Triggerbot"));

		ImGui::Checkbox(crypt_str("Enable"), &vars.legitbot.triggerbot.enabled);
		if (vars.legitbot.triggerbot.enabled)
		{
			ImGui::Checkbox(crypt_str("On Key"), &vars.legitbot.triggerbot.onkey);
			if (vars.legitbot.triggerbot.onkey)
			{
				vars.legitbot.triggerbot.trigkey.imgui(crypt_str("Triggerbot key"));
			}

			//ImGui::Checkbox(crypt_str("Target Teammates"), &vars.legitbot.triggerbot.teammates);
			ImGui::Checkbox(crypt_str("Scoped Only"), &vars.legitbot.triggerbot.scopedonly);
			ImGui::Checkbox(crypt_str("Flash Check"), &vars.legitbot.triggerbot.checkflash);
			ImGui::Checkbox(crypt_str("Smoke Check"), &vars.legitbot.triggerbot.checksmoke);
			if (vars.legitbot.backtrack)
				ImGui::Checkbox(crypt_str("Shoot at backtrack"), &vars.legitbot.triggerbot.shootatbacktrack);

			ImGui::Text(crypt_str("Delay"));
			ImGui::SliderFloat(crypt_str("Delay"), &vars.legitbot.triggerbot.delay, 0.f, 0.5f, crypt_str("%.3fs"), ImGuiSliderFlags_Logarithmic);
			ImGui::Text(crypt_str("Minimum damage"));
			ImGui::SliderInt(crypt_str("Minimum damage"), &vars.legitbot.triggerbot.mindmg, 1, 100);
			const std::string hitboxes[5]{ crypt_str("Head"), crypt_str("Chest") , crypt_str("Stomach"), crypt_str("Arms"), crypt_str("Legs") };
			std::string hitbox_label = crypt_str("");
			for (int i = 0; i < 5; i++)
			{
				if (vars.legitbot.triggerbot.hitboxes[i])
				{
					if (i != 4) {
						hitbox_label += std::string(hitboxes[i]) + crypt_str(", ");
					}
					else
					{
						hitbox_label += std::string(hitboxes[i]);
					}
				}
			}

			ImGui::Text(crypt_str("Hitbox selection"));
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("Hitbox"), hitbox_label.c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (size_t i = 0; i < 5; i++)
				{
					ImGui::Selectable(hitboxes[i].c_str(), &vars.legitbot.triggerbot.hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, { 305.f, menufont_height + 3 });
				}

				ImGui::EndCombo();
			}
		}

		AltEndChild();

		AltBeginChild({ 10 + ((window_size.x - 10) / 2), 65 }, { window_size.x - 5, window_size.y - 10 }, crypt_str("Weapon Configuration"));

		if (vars.legitbot.aimbot)
		{
			static std::string weapons[] = { crypt_str("Default"), crypt_str("Pistol"), crypt_str("Rifle"), crypt_str("Scout"), crypt_str("AWP"), crypt_str("SMG") };
			
			//ImGui::Combo(weapons[vars.legitbot.active_index].c_str(), &vars.legitbot.active_index, crypt_str("Default\0Pistol\0Rifle\0Scout\0AWP\0SMG\0"), 6);
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("weapon_select_legitbot"), weapons[vars.legitbot.active_index].c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (int i = 0; i < 6; i++)
				{
					bool selected = (vars.legitbot.active_index == i);
					if (ImGui::Selectable(weapons[i].c_str(), &selected, 0, { 305.f, menufont_height + 3 }))
					{
						vars.legitbot.active_index = i;
					}
				}
				ImGui::EndCombo();
			}


			ImGui::Checkbox(crypt_str("Enable"), &vars.legitbot.weapons[vars.legitbot.active_index].enabled);
			if (vars.legitbot.weapons[vars.legitbot.active_index].enabled)
			{
				ImGui::Checkbox(crypt_str("Silent"), &vars.legitbot.weapons[vars.legitbot.active_index].silent);
				//ImGui::Checkbox(crypt_str("Lock"), &vars.legitbot.weapons[vars.legitbot.active_index].aimlock);
				ImGui::Text(crypt_str("FOV"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("FOV"), &vars.legitbot.weapons[vars.legitbot.active_index].fov, 0.f, 180.f, crypt_str("%.3f"), ImGuiSliderFlags_Logarithmic);
				ImGui::Text(crypt_str("Smooth"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Smooth"), &vars.legitbot.weapons[vars.legitbot.active_index].smooth, 1.f, 100.f, crypt_str("%.4f"));
				ImGui::Text(crypt_str("RCS X"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("RCS X"), &vars.legitbot.weapons[vars.legitbot.active_index].rcsx, 0.f, 1.f, crypt_str("%.3f"));
				ImGui::Text(crypt_str("RCS Y"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("RCS Y"), &vars.legitbot.weapons[vars.legitbot.active_index].rcsy, 0.f, 1.f, crypt_str("%.3f"));
				ImGui::Checkbox(crypt_str("Scoped Only"), &vars.legitbot.weapons[vars.legitbot.active_index].scopedonly);
				ImGui::Checkbox(crypt_str("Flashbang Check"), &vars.legitbot.weapons[vars.legitbot.active_index].ignoreflash);
				ImGui::Checkbox(crypt_str("Smoke Check"), &vars.legitbot.weapons[vars.legitbot.active_index].ignoresmoke);
				if (vars.legitbot.backtrack)
					ImGui::Checkbox(crypt_str("Aim at Backtrack"), &vars.legitbot.weapons[vars.legitbot.active_index].aimbacktrack);

				static std::string hitboxes[]{ crypt_str("Head"), crypt_str("Chest") , crypt_str("Stomach"), crypt_str("Arms"), crypt_str("Legs") };
				std::string hitbox_label = crypt_str("");
				for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
				{
					if (vars.legitbot.weapons[vars.legitbot.active_index].hitboxes[i])
					{
						if (i != ARRAYSIZE(hitboxes) - 1) {
							hitbox_label += std::string(hitboxes[i]) + crypt_str(", ");
						}
						else
						{
							hitbox_label += std::string(hitboxes[i]);
						}
					}
				}

				
				ImGui::Text(crypt_str("Hitbox selection"));
				ImGui::SetNextItemWidth(305.f);
				if (ImGui::BeginCombo(crypt_str("Hitbox"), hitbox_label.c_str()))
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
					for (size_t i = 0; i < ARRAYSIZE(hitboxes); i++)
					{
						ImGui::Selectable(hitboxes[i].c_str(), &vars.legitbot.weapons[vars.legitbot.active_index].hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, { 305.f, menufont_height + 3 });
					}

					ImGui::EndCombo();
				}


			}
		}
		
		AltEndChild();

		break;

	case 1: // rage bot
		
		ImGui::SetCursorPosX(5);
		ImGui::SetCursorPosY(65);
		ImGui::PushFont(fonts::logger);
		ImGui::PushID(crypt_str("regebotpushbaton"));
		if (ImGui::Button(crypt_str("Ragebot"), { 320, 20 }))
			rage_or_aa = true;
		ImGui::PopID();
		ImGui::SameLine(0.f, 4.f);
		if (ImGui::Button(crypt_str("Anti-Aim"), { 318, 20 }))
			rage_or_aa = false;
		ImGui::PopFont();

		if (rage_or_aa) // ragebot
		{
			
			AltBeginChild({ 5 ,  90 }, { 5 + ((window_size.x - 10) / 2), 35 + (window_size.x / 2) }, crypt_str("Aimbot"));
			ImGui::Checkbox(crypt_str("Enable"), &vars.ragebot.enable);
			if (vars.ragebot.enable)
			{
				ImGui::Checkbox(crypt_str("On Key"), &vars.ragebot.onkey);
				if (vars.ragebot.onkey)
				{
					vars.ragebot.key.imgui(crypt_str("Ragebot Key"));
				}

				ImGui::Checkbox(crypt_str("Silent aim"), &vars.ragebot.silent);
				ImGui::Checkbox(crypt_str("Auto Shoot"), &vars.ragebot.autoshoot);

				ImGui::Checkbox(crypt_str("Automatic scope"), &vars.ragebot.autoscope);
				ImGui::Text(crypt_str("FOV"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("FOV"), &vars.ragebot.fov, 0, 180);
				ImGui::Checkbox(crypt_str("Resolver"), &vars.ragebot.resolver);
				//ImGui::Checkbox(crypt_str("Experimental move Resolver"), &vars.ragebot.rmode1);
				//ImGui::Checkbox(crypt_str("ONLY EXPERIMENTAL"), &vars.ragebot.rmode2);
				//ImGui::Checkbox(crypt_str("Resolver mode 2"), &vars.ragebot.rmode2);
				//ImGui::Checkbox(crypt_str("Resolver mode 3"), &vars.ragebot.rmode3);
				//ImGui::Checkbox(crypt_str("Resolver mode 4"), &vars.ragebot.rmode4);
				//ImGui::Checkbox(crypt_str("Old resolver lmao"), &vars.ragebot.rmode5);

				ImGui::Checkbox(crypt_str("Zeus Bot"), &vars.ragebot.zeusbot);
				if (vars.ragebot.zeusbot)
				{
					ImGui::PushID(crypt_str("22"));
					ImGui::Text(crypt_str("Zeus Hitchance"));
					ImGui::SetNextItemWidth(270.f);
					ImGui::SliderInt(crypt_str("Zeus Hitchance"), &vars.ragebot.zeuschance, 25, 100);
					ImGui::PopID();
				}
				ImGui::Checkbox(crypt_str("Knife Bot"), &vars.misc.knifebot);
				ImGui::PushID(crypt_str("4545"));
				vars.ragebot.force_safepoint.imgui(crypt_str("Force Safepoints"));
				ImGui::PopID();
				ImGui::PushID(crypt_str("4546"));
				vars.ragebot.baim.imgui(crypt_str("Force Body Aim"));
				ImGui::PopID();
				ImGui::PushID(crypt_str("4547"));
				vars.ragebot.override_dmg.imgui(crypt_str("Override Minimum Damage"));
				ImGui::PopID();
			}
			AltEndChild();

			AltBeginChild({ 5, 40 + (window_size.x / 2) }, { 5 + ((window_size.x - 10) / 2),  window_size.y - 10 }, crypt_str("Exploits"));
			if (vars.ragebot.enable)
			{
				if (vars.misc.restrict_type == 1)
				{
					ImGui::PushID(crypt_str("8789"));
					vars.ragebot.double_tap.imgui(crypt_str("Double Tap"));
					ImGui::PopID();
					if (vars.ragebot.double_tap.type != 0)
					{
						ImGui::Checkbox(crypt_str("Teleport boost"), &vars.ragebot.dt_teleport);
						//ImGui::Checkbox(crypt_str("Hideshot on DT"), &vars.ragebot.hideshot);
					}
					ImGui::PushID(crypt_str("hideshotteringkeying"));
					vars.ragebot.hideShots.imgui(crypt_str("Hide Shots"));
					ImGui::PopID();
					ImGui::Checkbox(crypt_str("Anti lagpeek"), &vars.ragebot.antiexploit);
				}
				else
				{
					ImGui::Text(crypt_str("Misc->Restrictions and set to HVH Mode to enable exploits"));
				}
			}

			AltEndChild();

			AltBeginChild({ 10 + ((window_size.x - 10) / 2), 90 }, { window_size.x - 5, window_size.y - 10 }, crypt_str("Weapon Configuration"));

			if (vars.ragebot.enable)
			{

				static std::string weapons[] = { crypt_str("Default"), crypt_str("Autosniper"), crypt_str("Scout"), crypt_str("AWP"), crypt_str("Rifles"), crypt_str("Pistols"), crypt_str("Heavy Pistols") };
				//ImGui::Combo(weapons[vars.ragebot.active_index].c_str(), &vars.ragebot.active_index, crypt_str("Default\0Autosniper\0Scout\0AWP\0Rifles\0Pistols\0Heavy Pistols\0"));
				ImGui::SetNextItemWidth(305.f);
				if (ImGui::BeginCombo(crypt_str("epic_ragebot_weapon_selector"), weapons[vars.ragebot.active_index].c_str()))
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
					for (int i = 0; i < 7; i++)
					{
						bool selected = (i == vars.ragebot.active_index);
						if (ImGui::Selectable(weapons[i].c_str(), &selected, 0, { 305.f, 16.f }))
							vars.ragebot.active_index = i;
					}

					ImGui::EndCombo();
				}

				std::string nigger123 = crypt_str("Enable ") + weapons[vars.ragebot.active_index] + crypt_str(" weapon config");
				ImGui::Checkbox(nigger123.c_str(), &vars.ragebot.weapon[vars.ragebot.active_index].enable);
				if (vars.ragebot.weapon[vars.ragebot.active_index].enable)
				{
					ImGui::Text(crypt_str("Hitchance"));
					ImGui::SetNextItemWidth(270.f);
					ImGui::SliderInt(crypt_str("Hitchance"), &vars.ragebot.weapon[vars.ragebot.active_index].hitchance, 0, 100, crypt_str("%d%%"));
					if (vars.ragebot.active_index != 2 && vars.ragebot.active_index != 3 && vars.ragebot.double_tap.type != 0)
					{
						ImGui::PushID(crypt_str("55"));
						ImGui::Text(crypt_str("Double tap hitchance"));
						ImGui::SetNextItemWidth(270.f);
						ImGui::SliderInt(crypt_str("Double tap hitchance"), &vars.ragebot.weapon[vars.ragebot.active_index].doubletap_hc, 0, 100, crypt_str("%d%%"));
						ImGui::PopID();
					}

					ImGui::Text(crypt_str("Minimum damage"));
					ImGui::SetNextItemWidth(270.f);

					if (vars.ragebot.weapon[vars.ragebot.active_index].mindamage == 101) {
						ImGui::SliderInt(crypt_str("Minimum damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage, 0, 101, crypt_str("HP+1"));
					}
					else {
						ImGui::SliderInt(crypt_str("Minimum damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage, 0, 101, crypt_str("%d HP"));
					}



					if (vars.ragebot.override_dmg.valid()) {

						ImGui::Text(crypt_str("Override Minimum Damage"));
						ImGui::SetNextItemWidth(270.f);

						ImGui::PushID(crypt_str("67"));
						if (vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override != 101) {
							ImGui::SliderInt(crypt_str("Override Minimum Damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override, 0, 101, crypt_str("%d HP"));
						}
						else
						{
							ImGui::SliderInt(crypt_str("Override Minimum Damage"), &vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override, 0, 101, crypt_str("HP + 1"));
						}
						ImGui::PopID();
					}

					//-----
					//-----
					static std::string crhitboxes[]{ crypt_str("Head"), crypt_str("Stomach"), crypt_str("Chest"), crypt_str("UpperChest"), crypt_str("LowerChest"), crypt_str("Pelvis"), crypt_str("Thighs"), crypt_str("Shins"), crypt_str("Feet"), crypt_str("Arms"), crypt_str("Hands") };
					std::string rhitbox_label = crypt_str("");
					for (int i = 0; i < ARRAYSIZE(crhitboxes); i++)
					{
						if (i != ARRAYSIZE(crhitboxes) - 1) {
							if (vars.ragebot.weapon[vars.ragebot.active_index].hitboxes[i])
							{
								rhitbox_label += std::string(crhitboxes[i]) + crypt_str(", ");
							}
						}
						else
						{
							if (vars.ragebot.weapon[vars.ragebot.active_index].hitboxes[i])
							{
								rhitbox_label += std::string(crhitboxes[i]);
							}
						}
					}

					
					ImGui::Text(crypt_str("Hitbox selection"));
					ImGui::SetNextItemWidth(305.f);
					if (ImGui::BeginCombo(crypt_str("Hitbox"), rhitbox_label.c_str()))
					{
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
						for (size_t i = 0; i < ARRAYSIZE(crhitboxes); i++)
						{
							ImGui::Selectable(crhitboxes[i].c_str(), &vars.ragebot.weapon[vars.ragebot.active_index].hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, {305.f, 16.f});
						}

						ImGui::EndCombo();
					}
					//--

					//ImGui::Checkbox(crypt_str("Automatic stop"), &vars.ragebot.weapon[vars.ragebot.active_index].quickstop);
					static std::string auto_stop_modes[] = { crypt_str("Disabled"), crypt_str("Normal"), crypt_str("Quickstop"), crypt_str("Predictive") };
					ImGui::Text(crypt_str("Auto-Stop"));
					ImGui::SetNextItemWidth(305.f);
					if (ImGui::BeginCombo(crypt_str("autostopmodering"), auto_stop_modes[vars.ragebot.weapon[vars.ragebot.active_index].autostopMode].c_str()))
					{
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
						for (int i = 0; i < 4; i++)
						{
							bool selected = (i == vars.ragebot.weapon[vars.ragebot.active_index].autostopMode);
							if (ImGui::Selectable(auto_stop_modes[i].c_str(), &selected, 0, {305.f, 16.f}))
								vars.ragebot.weapon[vars.ragebot.active_index].autostopMode = i;
						}

						ImGui::EndCombo();
					}
					//ImGui::Combo(crypt_str("Auto-Stop"), &vars.ragebot.weapon[vars.ragebot.active_index].autostopMode, crypt_str("Disabled\0Normal\0Quickstop\0Predictive\0"));

					ImGui::Checkbox(crypt_str("Static pointscale"), &vars.ragebot.weapon[vars.ragebot.active_index].static_scale);
					if (vars.ragebot.weapon[vars.ragebot.active_index].static_scale)
					{
						ImGui::Text(crypt_str("Head scale"));
						ImGui::SetNextItemWidth(270.f);
						ImGui::SliderInt(crypt_str("Head scale"), &vars.ragebot.weapon[vars.ragebot.active_index].scale_head, 0, 100, crypt_str("%d%%"));

						ImGui::Text(crypt_str("Body scale"));
						ImGui::SetNextItemWidth(270.f);
						ImGui::SliderInt(crypt_str("Body scale"), &vars.ragebot.weapon[vars.ragebot.active_index].scale_body, 0, 100, crypt_str("%d%%"));
					}

					ImGui::Text(crypt_str("Maximum misses"));
					ImGui::SetNextItemWidth(270.f);
					ImGui::SliderInt(crypt_str("Maximum misses"), &vars.ragebot.weapon[vars.ragebot.active_index].max_misses, 0, 6, crypt_str("%d"));

					ImGui::Checkbox(crypt_str("Prefer safe points"), &vars.ragebot.weapon[vars.ragebot.active_index].prefer_safepoint);
				}
			}

			AltEndChild();
		}
		else // antiaim
		{
			AltBeginChild({ 5 ,  90 }, { 5 + ((window_size.x - 10) / 2), window_size.y - 10  }, crypt_str("Anti-aim Configuration"));


			ImGui::Checkbox(crypt_str("Enable"), &vars.antiaim.enable);
			if (vars.antiaim.enable)
			{
				ImGui::Checkbox(crypt_str("On use"), &vars.antiaim.aa_on_use);
				ImGui::Checkbox(crypt_str("Desync on shot"), &vars.antiaim.shotDesync);


				ImGui::Text(crypt_str("Pitch"));
				ImGui::SetNextItemWidth(305.f);
				static std::string pitch_labels[] = { crypt_str("Disabled"), crypt_str("Down"), crypt_str("Up") };
				if (ImGui::BeginCombo(crypt_str("pitch_aa_cfg"), pitch_labels[vars.antiaim.pitch].c_str()))
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
					for (int i = 0; i < 3; i++)
					{
						bool selected = (i == vars.antiaim.pitch);
						if (ImGui::Selectable(pitch_labels[i].c_str(),&selected,0,{305.f,16.f}))
							vars.antiaim.pitch = i;
					}

					ImGui::EndCombo();
				}


				//ImGui::Combo(crypt_str("Pitchaa"), &vars.antiaim.pitch, crypt_str("Disabled\0Down\0Up\0"));


				ImGui::Text(crypt_str("Yaw"));
				static std::string yaw_labels[] = { crypt_str("Backward"), crypt_str("Forward") };
				ImGui::SetNextItemWidth(305.f);
				if (ImGui::BeginCombo(crypt_str("epic_yawselectorign"), yaw_labels[vars.antiaim.base_yaw].c_str()))
				{
					
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
					for (int i = 0; i < 2; i++)
					{
						bool selected = (i == vars.antiaim.base_yaw);
						if (ImGui::Selectable(yaw_labels[i].c_str(), &selected, 0, { 305.f,16.f }))
							vars.antiaim.base_yaw = i;
					}

					ImGui::EndCombo();
				}

				//ImGui::Combo(crypt_str("baseyaw"), &vars.antiaim.base_yaw, crypt_str("Backward\0Forward\0"));


				ImGui::PushID(crypt_str("1090"));
				vars.antiaim.inverter.imgui(crypt_str("Invert real angle"));
				ImGui::PopID();
				ImGui::Text(crypt_str("Jitter Angle"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Jitter Angle"), &vars.antiaim.jitter_angle, 0, 45, crypt_str("%d°"));
				ImGui::Checkbox(crypt_str("At targets"), &vars.antiaim.attarget);
				//if (vars.antiaim.attarget)
				//{
					//ImGui::Checkbox(crypt_str("Off when offscreen"), &vars.antiaim.attarget_off_when_offsreen);
				//}



				ImGui::Checkbox(crypt_str("Static desync"), &vars.antiaim.static_delta);

				if (!vars.antiaim.static_delta)
				{
					ImGui::PushID(crypt_str("23424"));
					ImGui::Text(crypt_str("Desync amount"));
					ImGui::SetNextItemWidth(270.f);
					ImGui::SliderInt(crypt_str("Desync amount"), &vars.antiaim.desync_amount, 0, 100, crypt_str("%d%%"));
					ImGui::PopID();
					//ImGui::Checkbox(crypt_str("Avoid overlap"), &vars.antiaim.avoid_overlap);
				}

				ImGui::Checkbox(crypt_str("Extend"), &vars.antiaim.extendlby);


				if (vars.misc.restrict_type == 1) {

					ImGui::Checkbox(crypt_str("Z-lean"), &vars.antiaim.zleanenable);
					ImGui::Text(crypt_str("Z-lean angle"));
					ImGui::SetNextItemWidth(270.f);
					ImGui::SliderInt(crypt_str("Z-lean angle"), &vars.antiaim.zlean, -180, 180, crypt_str("%d degrees"));
				}


				vars.antiaim.fakehead.imgui(crypt_str("fakehead"));
				if (vars.antiaim.fakehead.valid())
				{
					ImGui::Text(crypt_str("Fake Head Mode"));
					static std::string fakehead_labels[] = { crypt_str("Prefer Safety"),crypt_str("Prefer Speed") };
					ImGui::SetNextItemWidth(305.f);
					if (ImGui::BeginCombo(crypt_str("epic_fakeheading"), fakehead_labels[vars.antiaim.fakehead_mode].c_str()))
					{

						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
						for (int i = 0; i < 2; i++)
						{
							bool selected = (i == vars.antiaim.fakehead_mode);
							if (ImGui::Selectable(fakehead_labels[i].c_str(), &selected, 0, { 305.f,16.f }))
								vars.antiaim.fakehead_mode = i;
						}

						ImGui::EndCombo();
					}

					//ImGui::Combo(crypt_str("fakeheadmode"), &vars.antiaim.fakehead_mode, crypt_str("Prefer Safety\0Prefer Speed\0"));
				}

				ImGui::Checkbox(crypt_str("Manual anti-aim"), &vars.antiaim.manual_antiaim);
				if (vars.antiaim.manual_antiaim)
				{
					ImGui::Checkbox(crypt_str("Indicator"), &vars.visuals.antiaim_arrows);
					if (vars.visuals.antiaim_arrows)
					{

						color_picker4(crypt_str("Indicator Color"), vars.visuals.antiaim_arrows_color);

					}


					ImGui::Checkbox(crypt_str("Ignore at target"), &vars.antiaim.ignore_attarget);

					ImGui::PushID(crypt_str("1091"));
					vars.antiaim.manual_left.imgui(crypt_str("Left"));
					ImGui::PopID();

					ImGui::PushID(crypt_str("1092"));
					vars.antiaim.manual_right.imgui(crypt_str("Right"));
					ImGui::PopID();

					ImGui::PushID(crypt_str("1093"));
					vars.antiaim.manual_back.imgui(crypt_str("Back"));
					ImGui::PopID();

					ImGui::PushID(crypt_str("1094"));
					vars.antiaim.manual_forward.imgui(crypt_str("Forward"));
					ImGui::PopID();
				}

			}


			AltEndChild();
			AltBeginChild({ 10 + ((window_size.x - 10) / 2), 90 }, { window_size.x - 5, window_size.y - 10 }, crypt_str("Extra"));

			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			ImGui::SameLine();
			ImGui::Text(crypt_str("Fake-lag"));
			static std::string fakelag_labels[] = { crypt_str("Disabled"),crypt_str("Max"), crypt_str("Jitter"), crypt_str("BreakLC")};
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("fakelag_modering"), fakelag_labels[vars.antiaim.fakelag].c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (int i = 0; i < 4; i++)
				{
					bool selected = (i == vars.antiaim.fakelag);
					if (ImGui::Selectable(fakelag_labels[i].c_str(), &selected, 0, { 305.f,16.f }))
					{
						vars.antiaim.fakelag = i;
					}
				}

				ImGui::EndCombo();
			}

			//ImGui::Combo(crypt_str("Fake-lag"), &vars.antiaim.fakelag, crypt_str("Disabled\0Max\0Jitter\0BreakLC\0"));
			if (vars.antiaim.fakelag > 0)
			{
				ImGui::Text(crypt_str("Fake-lag ticks"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Fake-lag ticks"), &vars.antiaim.fakelagfactor, 0, 14, crypt_str("%d ticks"));
				if (vars.antiaim.fakelag == 2)
				{
					ImGui::PushID(crypt_str("23445"));
					ImGui::Text(crypt_str("Fake-lag jitter"));
					ImGui::SetNextItemWidth(270.f);
					ImGui::SliderInt(crypt_str("Fake-lag jitter"), &vars.antiaim.fakelagvariance, 0, 100, crypt_str("%d%%"));
					ImGui::PopID();
				}
				ImGui::Checkbox(crypt_str("Standby choke"), &vars.antiaim.fakelag_when_standing);
				//ImGui::Checkbox(crypt_str("Standing"), &vars.antiaim.fakelag_when_standing);
			}
			if (vars.misc.restrict_type == 1)
			{
				vars.antiaim.fakeduck.imgui(crypt_str("Fake Duck"));
			}
			vars.antiaim.slowwalk.imgui(crypt_str("Slow Walk"));
			if (vars.antiaim.slowwalk.valid())
			{
				ImGui::PushID(crypt_str("SEXOOAOXOAXOXOAXOAXOAOXOXAOA"));
				ImGui::Text(crypt_str("Slow Walk Speed"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Slow Walk Speed"), &vars.antiaim.slowwalkspeed, 0, 100, crypt_str("%d%%"));
				ImGui::PopID();
			}

			//ImGui::Checkbox(crypt_str("Godmode/Instaplant Exploit (Enable before joining server)"), &vars.antiaim.simtimeStopper);


			AltEndChild();
		}

		break;
	case 2: // visuals
		
		AltBeginChild({ 5 ,  65 }, { 5 + ((window_size.x - 10) / 2), window_size.y - 10 }, crypt_str("Player ESP"));

		ImGui::Checkbox(crypt_str("Enable"), &vars.visuals.enable);
		if (vars.visuals.enable)
		{

			ImGui::Checkbox(crypt_str("Box"), &vars.visuals.box);
			ImGui::SameLine();
			color_picker4(crypt_str("Box Color Visible"), vars.visuals.box_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Box Color Occluded"), vars.visuals.box_color_occluded, false);
			


			ImGui::Checkbox(crypt_str("Dormant ESP"), &vars.visuals.dormant);

			ImGui::Checkbox(crypt_str("Name"), &vars.visuals.name);
			ImGui::SameLine();
			color_picker4(crypt_str("Name Color Visible"), vars.visuals.name_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Name Color Occluded"), vars.visuals.name_color_occluded, false);

			ImGui::Checkbox(crypt_str("Health Bar"), &vars.visuals.healthbar);
			if (vars.visuals.healthbar)
			{
				ImGui::Checkbox(crypt_str("Override health color"), &vars.visuals.override_hp);
				ImGui::SameLine();
				color_picker4(crypt_str("Health Bar Color Visible"), vars.visuals.hp_color, false);
				ImGui::SameLine();
				color_picker4(crypt_str("Health Bar Color Occluded"), vars.visuals.hp_color_occluded, false);
			}

			ImGui::Checkbox("Skeleton", &vars.visuals.skeleton);
			ImGui::SameLine();
			color_picker4(crypt_str("Skeleton Color Visible"), vars.visuals.skeleton_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Skeleton Color Occluded"), vars.visuals.skeleton_color_occluded, false);


			ImGui::Checkbox("Glow", &vars.visuals.glow);
			ImGui::SameLine();
			color_picker4(crypt_str("Glow color Visible"), vars.visuals.glow_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Glow color Occluded"), vars.visuals.glow_color_occluded, false);

			ImGui::Checkbox(crypt_str("Glow on Local"), &vars.visuals.local_glow);
			if (vars.visuals.local_glow)
				color_picker4(crypt_str("Local Glow Color"), vars.visuals.local_glow_clr);
			ImGui::Combo(crypt_str("Glow Style"), &vars.visuals.glowtype, crypt_str("Normal\0Pulsating\0"));

			ImGui::Checkbox(crypt_str("Weapon"), &vars.visuals.weapon);
			ImGui::SameLine();
			color_picker4(crypt_str("Weapon Color Visible"), vars.visuals.weapon_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Weapon Color Occluded"), vars.visuals.weapon_color_occluded, false);
			AltMultiCombo(crypt_str("Weapon Info Type"), icontypelabels, vars.visuals.weaponicontype, 2);
			
				

			ImGui::Checkbox(crypt_str("Ammo"), &vars.visuals.ammo);
			ImGui::SameLine();
			color_picker4(crypt_str("Ammo Color Visible"), vars.visuals.ammo_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Ammo Color Occluded"), vars.visuals.ammo_color_occluded, false);

			ImGui::Checkbox(crypt_str("Dlights"), &vars.visuals.dlights);
			ImGui::SameLine();
			color_picker4(crypt_str("Dlights Color Visible"), vars.visuals.dlights_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Dlights Color Occluded"), vars.visuals.dlights_color_occluded, false);

			static std::string flags[]{ crypt_str("Armor"), crypt_str("Scoped"), crypt_str("Flashed"), crypt_str("Defuse kit"), crypt_str("Fake duck") };
			static bool flag[5] = { false, false, false, false, false };
			static std::string flag_label = crypt_str("");
			for (size_t i = 0; i < 5; i++)
			{
				flag[i] = (vars.visuals.flags & 1 << i) == 1 << i;
			}
;
			ImGui::Text(crypt_str("Player flags"));
			ImGui::SameLine();
			color_picker4(crypt_str("Flags Color Visible"), vars.visuals.flags_color, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Flags Color Occluded"), vars.visuals.flags_color_occluded, false);


			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("Flags"), flag_label.c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (size_t i = 0; i < 5; i++)
				{
					ImGui::Selectable(flags[i].c_str(), &flag[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, {305.f, 16.f});
				}

				ImGui::EndCombo();
			}

			for (size_t i = 0; i < 5; i++)
			{
				if (i == 0) flag_label = crypt_str("");

				if (flag[i])
				{
					flag_label += flag_label.size() ? std::string(crypt_str(", ")) + flags[i] : flags[i];
					vars.visuals.flags |= 1 << i;
				}
				else
				{
					vars.visuals.flags &= ~(1 << i);
				}
			}
			
				

			ImGui::Checkbox(crypt_str("Out of fov arrows"), &vars.visuals.out_of_fov);
			ImGui::SameLine();
			color_picker4(crypt_str("Out of fov arrows color Visible"), vars.visuals.out_of_fov_visiblecolor, false);
			ImGui::SameLine();
			color_picker4(crypt_str("Out of fov arrows color Occluded"), vars.visuals.out_of_fov_color, false);

			if (vars.visuals.out_of_fov)
			{
				


				ImGui::PushID(crypt_str("09239"));
				ImGui::Text(crypt_str("Size"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Size"), &vars.visuals.out_of_fov_size, 5, 80);
				ImGui::PopID();
				ImGui::PushID(crypt_str("092392"));
				ImGui::Text(crypt_str("Distance"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Distance"), &vars.visuals.out_of_fov_distance, 0, 500);
				ImGui::PopID();
			}
		}
		AltEndChild();

		AltBeginChild({ 10 + ((window_size.x - 10) / 2), 65 }, { window_size.x - 5, window_size.y - 10 }, crypt_str("Chams"));
		ImGui::PushID(0);

		//if (ImGui::Combo("", &currentCategory, crypt_str("Allies\0Enemies\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0Desync\0")))
			//material = 0;
		ImGui::SetNextItemWidth(305.f);
		if (ImGui::BeginCombo(crypt_str("##chamweaponselector"), cham_labels[currentCategory].c_str()))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
			for (int i = 0; i < 8; i++)
			{
				bool selected = (currentCategory == i);
				if (ImGui::Selectable(cham_labels[i].c_str(), &selected, 0, { 305.f,16.f }))
				{
					currentCategory = i;
					material = 0;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();

		if (currentCategory < 3) {

			static int currentType{ 0 };
			ImGui::Text(crypt_str("Visibility"));
			ImGui::PushID(1);
			static std::string vis_labels[] = { crypt_str("Visible"), crypt_str("Occluded") };
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("##visibility_combor"), vis_labels[currentType].c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (int i = 0; i < 2; i++)
				{
					bool selected = (currentType == i);
					if (ImGui::Selectable(vis_labels[i].c_str(), &selected, 0, { 305.f,16.f }))
					{
						currentType = i;
						material = 0;
					}

				}
				ImGui::EndCombo();
			}
			ImGui::PopID();
			currentItem = currentCategory * 2 + currentType;
		}
		else {
			currentItem = currentCategory + 3; // 3 is number of visible and occluded targets
		}

		ImGui::Text(crypt_str("Layer"));
		
		ImGui::SetNextItemWidth(305.f);
		if (ImGui::BeginCombo(crypt_str("##materialo_combor"), mat_labels[material].c_str()))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
			for (int i = 0; i < 2; i++)
			{
				bool selected = (material == i);
				if (ImGui::Selectable(mat_labels[i].c_str(), &selected, 0, { 305.f,16.f }))
				{
					material = i;
				}

			}
			ImGui::EndCombo();
		}

		//ImGui::Combo(crypt_str("Layer"), &material, crypt_str(" 1\0 2\0"), 2);

		ImGui::Separator();
		ImGui::Checkbox(crypt_str("Enabled"), &chams.enabled);
		ImGui::Checkbox(crypt_str("Health based"), &chams.healthBased);
		ImGui::Checkbox(crypt_str("Blinking"), &chams.blinking);

		
		ImGui::SetNextItemWidth(305.f);
		if (ImGui::BeginCombo(crypt_str("##material_combo"), material_labels[chams.material].c_str()))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
			for (int i = 0; i < 13; i++)
			{
				bool selected = (chams.material == i);
				if (ImGui::Selectable(material_labels[i].c_str(), &selected, 0, { 305.f,16.f }))
				{
					chams.material = i;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Checkbox(crypt_str("Wireframe"), &chams.wireframe);
		if (!chams.rainbow)
			color_picker4(crypt_str("Color"), chams.color);

		ImGui::Checkbox(crypt_str("Rainbow"), &chams.rainbow);
		if (chams.rainbow)
		{
			ImGui::Text(crypt_str("Rainbow speed"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Rainbow speed"), &chams.rainbowSpeed, 0.01f, 5.f, crypt_str("%.2f"));
		}	

		AltEndChild();
		break;
	case 3: // world visuals

		AltBeginChild({ 5 ,  65 }, { 5 + ((window_size.x - 10) / 2), window_size.y - 10 }, crypt_str("World Visuals"));
		
		removal_label.clear();
		for (int s = 0; s < removals.size(); s++)
		{
			if (vars.visuals.remove[s])
			{
				if (s != removals.size() - 1)
				{
					removal_label += std::string(removals[s]) + crypt_str(", ");
				}
				else
				{
					removal_label += std::string(removals[s]);
				}

			}
		}

		ImGui::Text(crypt_str("Removals"));
		ImGui::SetNextItemWidth(305.f);
		if (ImGui::BeginCombo(crypt_str("Removals"), removal_label.c_str()))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
			for (size_t i = 0; i < removals.size(); i++)
			{
				ImGui::Selectable(removals.at(i).c_str(), &vars.visuals.remove[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, {305.f, 16.f});
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox(crypt_str("Hitsound"), &vars.visuals.hitmarker_sound);
		if (vars.visuals.hitmarker_sound)
		{
			ImGui::PushID(crypt_str("hit sound!"));
			static std::string hitsound_label[] = { crypt_str("Switch"),crypt_str("Warning"), crypt_str("BOOM"), crypt_str("COD"), crypt_str("FemboyMoan"), crypt_str("Custom") };
			//ImGui::Combo(crypt_str("Hitsound type"), &vars.visuals.hitmarker_sound_type, crypt_str("Switch\0Warning\0BOOM\0COD\0FemboyMoan\0Custom\0"));
			ImGui::Text("Hitsound type");
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("Hitsound type"), hitsound_label[vars.visuals.hitmarker_sound_type].c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (size_t i = 0; i < 6; i++)
				{
					bool selected = (vars.visuals.hitmarker_sound_type == i);
					if(ImGui::Selectable(hitsound_label[i].c_str(), &selected, 0,{ 305.f, 16.f }))
						vars.visuals.hitmarker_sound_type = i;
				}

				ImGui::EndCombo();
			}

			if (vars.visuals.hitmarker_sound_type == 5)
			{
				ImGui::PushID(crypt_str("soundtypehitmarkerfilename"));
				ImGui::InputText(crypt_str("Filename"), vars.visuals.hitmarker_name, 32);
				ImGui::PopID();
			}
			ImGui::PopID();

		}
		ImGui::Checkbox(crypt_str("Hitmarker"), &vars.visuals.hitmarker);
		if (vars.visuals.hitmarker)
		{
			ImGui::Checkbox(crypt_str("Visualize damage"), &vars.visuals.visualize_damage);
			if (vars.visuals.visualize_damage)
				color_picker4(crypt_str("Headshot color"), vars.visuals.hitmarker_color);
		}
		ImGui::Checkbox(crypt_str("Server Bullet impacts"), &vars.visuals.bullet_impact);

		if (vars.visuals.bullet_impact)
		{
			color_picker4(crypt_str("Bullet impacts color"), vars.visuals.bullet_impact_color);
			ImGui::Text(crypt_str("Bullet impacts size"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Bullet impacts size"), &vars.visuals.impacts_size, 2.0f, 5.0f);
		}

		ImGui::Checkbox(crypt_str("Enemy bullet tracers"), &vars.visuals.bullet_tracer);
		if (vars.visuals.bullet_tracer)
			color_picker4(crypt_str("Enemy bullet tracers color"), vars.visuals.bullet_tracer_color);
		ImGui::Checkbox(crypt_str("Local bullet tracers"), &vars.visuals.bullet_tracer_local);
		if (vars.visuals.bullet_tracer_local)
			color_picker4(crypt_str("Local bullet tracers color"), vars.visuals.bullet_tracer_local_color);
		if (vars.visuals.bullet_tracer || vars.visuals.bullet_tracer_local)
		{
			AltCombo(crypt_str("Bullet tracers type"), beam_labels, 2, vars.visuals.bullet_tracer_type);
			//ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("Bullet tracers type"));
			//ImGui::Combo(crypt_str("Bullet tracers type"), &vars.visuals.bullet_tracer_type, crypt_str("Default\0Phys beam\0")); //crypt_str("Default\0Phys beam\0Bubble\0Glow\0"));
		}

		ImGui::Checkbox(crypt_str("Preserve killfeed"), &vars.visuals.preverse_killfeed);


		vars.misc.thirdperson.imgui(crypt_str("Thirdperson"));
		if (vars.misc.thirdperson.valid())
		{
			ImGui::PushID(crypt_str("Dist"));
			ImGui::Text(crypt_str("Distance"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderInt(crypt_str("Distance"), &vars.visuals.thirdperson_dist, 0, 300, crypt_str("%d units"));
			ImGui::PopID();
		}

		ImGui::Checkbox(crypt_str("Nightmode"), &vars.visuals.nightmode);
		if (vars.visuals.nightmode)
		{
			ImGui::Text(crypt_str("Nightmode amount"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderInt(crypt_str("Nightmode amount"), &vars.visuals.nightmode_amount, 0, 100, crypt_str("%d%%"));
			ImGui::Checkbox(crypt_str("Color"), &vars.visuals.customize_color);

			if (vars.visuals.customize_color)
			{

				color_picker4(crypt_str("World"), vars.visuals.nightmode_color);

				color_picker4(crypt_str("Props"), vars.visuals.nightmode_prop_color);

				color_picker4(crypt_str("Skybox"), vars.visuals.nightmode_skybox_color);
			}

		}

		ImGui::Checkbox(crypt_str("Sunset mode"), &vars.visuals.sunset_enabled);
		if (vars.visuals.sunset_enabled)
		{
			ImGui::PushID(91323);
			ImGui::SetNextItemWidth(230.f);
			ImGui::SliderFloat("", &vars.visuals.rot_x, 0, 360, crypt_str("X rotation: %.1f"));
			ImGui::PopID();
			ImGui::PushID(93523);
			ImGui::SetNextItemWidth(230.f);
			ImGui::SliderFloat("", &vars.visuals.rot_y, 0, 360, crypt_str("Y rotation: %.1f"));
			ImGui::PopID();
		}

		if (!vars.visuals.remove[6]) // not allowing to edit fog when removed
		{
			ImGui::Checkbox(crypt_str("Fog Customization"), &vars.visuals.fog.enabled);
			if (vars.visuals.fog.enabled)
			{
				ImGui::PushID(84523);
				ImGui::Text(crypt_str("Fog start"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Fog start"), &vars.visuals.fog.start, 0, 5000);
				ImGui::PopID();

				ImGui::PushID(90023);
				ImGui::Text(crypt_str("Fog end"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Fog end"), &vars.visuals.fog.end, 0, 5000);
				ImGui::PopID();

				ImGui::PushID(12487);
				ImGui::Text(crypt_str("Fog density"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Fog density"), &vars.visuals.fog.density, 0.001f, 1.f, crypt_str("%.3f"));
				ImGui::PopID();

				color_picker4(crypt_str("Fog color"), vars.visuals.fog.color);
			}
		}
		
		ImGui::Checkbox(crypt_str("Particle Customization"), &vars.visuals.editparticles);
		if (vars.visuals.editparticles)
		{
			if (!vars.visuals.remove[1])
			{
				ImGui::Text(crypt_str("Smoke transparency"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("##smoketransparenceum"), &vars.visuals.smoke_alpha, 0.f, 1.f);
			}

			color_picker4(crypt_str("Molotov color"), vars.visuals.molotov_particle_color);
			color_picker4(crypt_str("Blood color"), vars.visuals.blood_particle_color);
		}


		ImGui::Checkbox(crypt_str("Motion blur"), &vars.visuals.motionblur.enabled);
		if (vars.visuals.motionblur.enabled)
		{
			ImGui::Checkbox(crypt_str("Forward enabled"), &vars.visuals.motionblur.fwd_enabled);
			ImGui::PushID(crypt_str("BLACKPERNOSSS"));
			ImGui::Text(crypt_str("Falling min"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Falling min"), &vars.visuals.motionblur.falling_min, 0.f, 50.f);
			ImGui::PopID();
			ImGui::PushID("falingMax");
			ImGui::Text(crypt_str("Falling max"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Falling max"), &vars.visuals.motionblur.falling_max, 0.f, 50.f);
			ImGui::PopID();
			ImGui::PushID("falingintersnsn");
			ImGui::Text(crypt_str("Falling intensity"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Falling intensity"), &vars.visuals.motionblur.falling_intensity, 0.f, 8.f);
			ImGui::PopID();
			ImGui::PushID("rotataintersnsn");
			ImGui::Text(crypt_str("Rotation intensity"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Rotation intensity"), &vars.visuals.motionblur.rotation_intensity, 0.f, 8.f);
			ImGui::PopID();
			ImGui::PushID("sterererererxcxcxc");
			ImGui::Text(crypt_str("Strength"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Strength"), &vars.visuals.motionblur.strength, 0.f, 8.f);
			ImGui::PopID();
		}
		ImGui::Checkbox(crypt_str("Force crosshair"), &vars.visuals.force_crosshair);
		ImGui::Checkbox(crypt_str("Recoil crosshair"), &vars.visuals.recoil_crosshair);
		ImGui::Checkbox(crypt_str("Radar Reveal"), &vars.visuals.radarhack);
		ImGui::Text(crypt_str("Aspect ratio"));
		ImGui::SetNextItemWidth(270.f);
		ImGui::SliderInt(crypt_str("Aspect ratio"), &vars.visuals.aspect_ratio, 0, 300, crypt_str("%d"));
		AltEndChild();


		AltBeginChild({ 10 + ((window_size.x - 10) / 2) ,  65 }, { window_size.x - 5, 155 + (window_size.y / 2) }, crypt_str("Item Visuals"));

		ImGui::Checkbox(crypt_str("Weapon ESP"), &vars.visuals.world.weapons.enabled);
		if (vars.visuals.world.weapons.enabled)
		{
			
			AltMultiCombo(crypt_str("Weapon ESP Type"), icontypelabels, vars.visuals.world.weapons.indtype, 2);

			color_picker4(crypt_str("Weapon ESP color"), vars.visuals.world.weapons.color);
		}
			

		ImGui::Checkbox(crypt_str("Grenade ESP"), &vars.visuals.world.projectiles.enable);
		if (vars.visuals.world.projectiles.enable)
		{
			std::string grennames[] = { crypt_str("Team"), crypt_str("Enemy"), crypt_str("Local") };
			std::string label = crypt_str("");
			for (int i = 0; i < 3; i++)
			{
				if (i != 3 - 1)
				{
					if (vars.visuals.world.projectiles.filter[i])
						label += std::string(grennames[i]) + crypt_str(", ");
				}
				else
				{
					if (vars.visuals.world.projectiles.filter[i])
						label += std::string(grennames[i]);
				}
			}
			ImGui::Text(crypt_str("Grenade ESP Target"));
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("Grenade ESP Target"), label.c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (size_t i = 0; i < 3; i++)
				{
					ImGui::Selectable(grennames[i].c_str(), &vars.visuals.world.projectiles.filter[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, {305.f, 16.f});
				}

				ImGui::EndCombo();
			}

			
			AltMultiCombo(crypt_str("Grenade ESP Type"), icontypelabels, vars.visuals.world.projectiles.indtype, 2);
			
		}
		ImGui::Checkbox(crypt_str("Grenade prediction"), &vars.visuals.nadepred);
		ImGui::Checkbox(crypt_str("Bomb timer"), &vars.visuals.world.weapons.planted_bomb);
		if (vars.visuals.world.weapons.planted_bomb)
		{
			ImGui::PushID(crypt_str("bombotimero"));
			ImGui::Text(crypt_str("Bomb timer height"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Bomb timer height"), &vars.visuals.world.weapons.planted_bombheight, 0.f, 1.f, crypt_str("%.3f"));
			ImGui::PopID();
		}
		ImGui::Text(crypt_str("FOV"));
		ImGui::SetNextItemWidth(270.f);
		ImGui::SliderInt(crypt_str("##world_FOV"), &vars.misc.worldfov, 0, 120, crypt_str("%d"));

		ImGui::Text(crypt_str("Viewmodel FOV"));
		ImGui::SetNextItemWidth(270.f);
		ImGui::SliderInt(crypt_str("##Viewmodel_FOV"), &vars.misc.viewmodelfov, 60, 150, crypt_str("%d"));

		ImGui::Checkbox(crypt_str("Viewmodel Customization"), &vars.misc.enableviewmodel);
		if (vars.misc.enableviewmodel)
		{
			ImGui::PushID(crypt_str("323"));
						ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str(""), &vars.misc.viewmodel_x, -25.f, 25.f, crypt_str("x: %.2f"));
			ImGui::PopID();
			ImGui::PushID(crypt_str("324"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str(""), &vars.misc.viewmodel_y, -25.f, 25.f, crypt_str("y: %.2f"));
			ImGui::PopID();
			ImGui::PushID(crypt_str("325"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str(""), &vars.misc.viewmodel_z, -25.f, 25.f, crypt_str("z: %.2f"));
			ImGui::PopID();

			ImGui::PushID(crypt_str("23399232"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str(""), &vars.misc.viewmodel_roll, -179.9f, 179.9f, crypt_str("roll: %.2f"));
			ImGui::PopID();
		}
		AltEndChild();


		AltBeginChild({ 10 + ((window_size.x - 10) / 2), 160 + (window_size.y / 2) }, { window_size.x - 5,  window_size.y - 10 }, crypt_str("Animals"));
		ImGui::Checkbox(crypt_str("Chicken ESP"), &vars.misc.chicken.enable);
		if (vars.misc.chicken.enable)
		{
			//ImGui::Checkbox(crypt_str("Chicken box"), &vars.misc.chicken.chickenbox);
			ImGui::Checkbox(crypt_str("Owner name"), &vars.misc.chicken.owner);
			ImGui::Checkbox(crypt_str("Owned list"), &vars.misc.chicken.sidelist);
		}
		ImGui::Checkbox(crypt_str("Reveal chicken killer"), &vars.misc.chicken.revealkiller);
		ImGui::Checkbox(crypt_str("Announce chicken killers"), &vars.misc.chicken.saykiller);
		ImGui::Checkbox(crypt_str("Fish ESP"), &vars.misc.chicken.fish);
		if (vars.misc.chicken.fish)
		{
			//ImGui::Checkbox(crypt_str("Fish box"), &vars.misc.chicken.fishbox);
			color_picker4(crypt_str("Fish esp color"), vars.misc.chicken.fishcolor);
		}
		AltEndChild();

		break;
	case 4: // misc
		AltBeginChild({ 5 ,  65 }, { 5 + ((window_size.x - 10) / 2), 35 + (window_size.x / 2) }, crypt_str("Misc"));
		ImGui::Dummy(ImVec2(0.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Text(crypt_str("Menu Key"));
		ImGui::SameLine();
		
		if (!menukeybindopened)
		{
			if (vars.menu.menu_key)
			{
				//ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("[ %s ]"), interfaces.inputsystem->vkey2string(key));
				keyname = std::string(crypt_str("[ ")) + std::string(interfaces.inputsystem->vkey2string(vars.menu.menu_key)) + std::string(crypt_str(" ]"));
			}
			else
			{
				//ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, crypt_str("[ key ]"));
				keyname = crypt_str("[ key ]");
			}
		}
		else
		{
			keyname = crypt_str("[ Press key ]");
		}


		if (menukeybindopened)
		{
			ImGuiIO& io = ImGui::GetIO();
			for (int i = 0; i < 512; i++)
			{
				if (ImGui::IsKeyPressed(i))
				{
					vars.menu.menu_key = i != VK_ESCAPE ? i : VK_INSERT;
					menukeybindopened = false;
				}
			}

			for (int i = 0; i < 5; i++)
			{
				if (ImGui::IsMouseDown(i))
				{
					vars.menu.menu_key = VK_INSERT;
					menukeybindopened = false;
				}
			}
		}


		if (ImGui::Button(keyname.c_str()))
		{
			menukeybindopened = true;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(crypt_str("Click to change keybind"));
		}

		
		//ImGui::Text(crypt_str("Restrictions"));
		//ImGui::Combo(crypt_str("Restrictions"), &vars.misc.restrict_type, crypt_str("MM\0HvH\0KZ\0"));
		
		AltCombo(crypt_str("Restrictions"), restrict_labels, 3, vars.misc.restrict_type);


		//if (vars.misc.restrict_type == 0)
		//{
		ImGui::Checkbox(crypt_str("Anti untrusted"), &vars.misc.antiuntrusted);
		//}
		ImGui::Checkbox(crypt_str("Watermark"), &vars.visuals.watermark);
		ImGui::Checkbox(crypt_str("Block server ads & overlays"), &vars.misc.ublockOrigin);
		ImGui::Checkbox(crypt_str("Spotify status"), &vars.visuals.spotifysong);
		
		//
		ImGui::Checkbox(crypt_str("Event logger"), &vars.visuals.eventlog);
		if (vars.visuals.eventlog)
		{
			color_picker4(crypt_str("Event logger color"), vars.visuals.eventlog_color);
			ImGui::Checkbox(crypt_str("Print votes to chat"), &vars.visuals.print_votes);
			if (vars.visuals.print_votes)
				AltCombo(crypt_str("Chat selection"), voteprint_label, 2, vars.visuals.print_votes_chat);
				//ImGui::Combo(vars.visuals.print_votes_chat ? crypt_str("Team") : crypt_str("All"), &vars.visuals.print_votes_chat, crypt_str("Team\0All\0"));
		}
		ImGui::Checkbox(crypt_str("Reveal ranks"), &vars.misc.revealranks);
		ImGui::Checkbox(crypt_str("Auto accept"), &vars.misc.autoaccept);
		if (vars.misc.autoaccept)
		{
			ImGui::PushID(crypt_str("Sexiw"));
			ImGui::Text(crypt_str("Auto accept delay"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Auto accept delay"), &vars.misc.autoacceptdelay, 0.f, 25.f, crypt_str("%.1fs"));
			ImGui::PopID();
		}
		ImGui::Checkbox(crypt_str("Bypass sv_pure"), &vars.misc.sv_purebypass);
		ImGui::Checkbox(crypt_str("Unlock inventory access"), &vars.misc.unlockinventoryaccess);
		ImGui::Checkbox(crypt_str("Killsay"), &vars.misc.killsay);

		ImGui::Checkbox(crypt_str("Clantag spammer"), &vars.visuals.clantagspammer);
		if (vars.visuals.clantagspammer)
		{
			ImGui::PushID(crypt_str("Clantagbullshit"));
			AltCombo(crypt_str("Clantag type"), clantag_labels, 4, vars.visuals.clantagtype);

			//ImGui::Combo(combonames[vars.visuals.clantagtype].c_str(), &vars.visuals.clantagtype, crypt_str("Static\0Animated\0Custom\0Clock\0"));
			ImGui::PopID();
			if (vars.visuals.clantagtype == 2)
			{
				ImGui::PushID(crypt_str("clantagerge"));
				ImGui::InputTextWithHint(crypt_str("##epiccustomclantag"), crypt_str("Custom Clantag"), vars.visuals.clantagcustomname, 32);
				ImGui::PopID();
				ImGui::Checkbox(crypt_str("Rotating"), &vars.visuals.clantagrotating);
				ImGui::PushID(crypt_str("sliderForSpedddd"));
				ImGui::Text(crypt_str("Clantag speed"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Clantag speed"), &vars.visuals.clantagspeed, 0.1, 5.f, crypt_str("%.1f"));
				ImGui::PopID();
			}
		}
		ImGui::Checkbox(crypt_str("Buy-bot"), &vars.misc.autobuy.enable);
		if (vars.misc.autobuy.enable)
		{

			//ImGui::Combo(crypt_str("Primary"), &vars.misc.autobuy.main, crypt_str("None\0Autosniper\0Scout\0AWP\0Negev\0M249\0Rifle\0AUG/SG553\0"));
			AltCombo(crypt_str("Primary"), autobuyprimary_labels, 8, vars.misc.autobuy.main);

			//ImGui::Combo(crypt_str("Secondary"), &vars.misc.autobuy.pistol, crypt_str("None\0Dualies\0P250\0Five-seveN\0Deagle/R8 Revolver\0"));
			AltCombo(crypt_str("Secondary"), autobuysecondary_labels, 5, vars.misc.autobuy.pistol);

			
			

			for (size_t i = 0; i < 7; i++)
			{
				if (i == 0) utility_label = "";

				if (vars.misc.autobuy.misc[i])
				{
					utility_label += utility_label.size() ? std::string(crypt_str(", ")) + utility[i] : utility[i];
				}
				
			}

			ImGui::Text(crypt_str("Utility"));
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("##utilitarian"), utility_label.c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (size_t i = 0; i < 7; i++)
				{
					ImGui::Selectable(utility[i].c_str(), &vars.misc.autobuy.misc[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, { 305.f, 16.f });
				}

				ImGui::EndCombo();
			}

			
		}
		//
		//
		if (ImGui::Button(crypt_str("Unlock hidden ConVars"), { 270, 23 }))
		{
			EnableHiddenCVars();
		}

		
		AltEndChild();

		AltBeginChild({ 5, 40 + (window_size.x / 2) }, { 5 + ((window_size.x - 10) / 2),  window_size.y - 10 }, crypt_str("Indicators"));


		ImGui::Checkbox(crypt_str("Edgebug detection"), &vars.movement.ebdetect.mastertoggle);
		if (vars.movement.ebdetect.mastertoggle)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (i == 0) ebdetecteffect_label = "";

				if (vars.movement.ebdetect.enabled[i])
				{
					ebdetecteffect_label += ebdetecteffect_label.size() ? std::string(crypt_str(", ")) + ebdetect_labels[i] : ebdetect_labels[i];
				}

			}

			ImGui::Text(crypt_str("Detection effects"));
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::BeginCombo(crypt_str("##detecteffects"), ebdetecteffect_label.c_str()))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (size_t i = 0; i < 4; i++)
				{
					ImGui::Selectable(ebdetect_labels[i].c_str(), &vars.movement.ebdetect.enabled[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups, { 305.f, 16.f });
				}

				ImGui::EndCombo();
			}

			if (vars.movement.ebdetect.enabled[3])
			{
				static std::string hitsound_label[] = { crypt_str("Switch"),crypt_str("Warning"), crypt_str("BOOM"), crypt_str("COD"), crypt_str("FemboyMoan"), crypt_str("Custom") };
				//ImGui::Combo(crypt_str("Hitsound type"), &vars.visuals.hitmarker_sound_type, crypt_str("Switch\0Warning\0BOOM\0COD\0FemboyMoan\0Custom\0"));
				ImGui::Text("Sound type");
				ImGui::SetNextItemWidth(305.f);
				if (ImGui::BeginCombo(crypt_str("##ebdetectsoundtype"), hitsound_label[vars.movement.ebdetect.soundtype].c_str()))
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
					for (size_t i = 0; i < 6; i++)
					{
						bool selected = (vars.movement.ebdetect.soundtype == i);
						if (ImGui::Selectable(hitsound_label[i].c_str(), &selected, 0, { 305.f, 16.f }))
							vars.movement.ebdetect.soundtype = i;
					}

					ImGui::EndCombo();
				}

				if (vars.movement.ebdetect.soundtype == 5)
				{
					ImGui::PushID(crypt_str("soundtypehitmarkerfilename"));
					ImGui::InputText(crypt_str("Filename"), vars.movement.ebdetect.sound_name, 32);
					ImGui::PopID();
				}
			}
		}

		ImGui::Checkbox(crypt_str("Show LJ Stats"), &vars.movement.ljstats);


		ImGui::Checkbox(crypt_str("Draw velocity"), &vars.movement.showvelocity);
		if (vars.movement.showvelocity)
		{
			ImGui::PushID(crypt_str("velpos"));
			ImGui::Text(crypt_str("Height"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Height"), &vars.movement.showvelpos, 0.f, 1.f, crypt_str("%.4f"));
			ImGui::PopID();
			ImGui::Checkbox(crypt_str("Takeoff velocity"), &vars.movement.showtakeoff);
			if (vars.movement.showtakeoff)
				AltCombo(crypt_str("Takeoff Type"), veltypelabels, 4, vars.movement.showveltype);
			ImGui::Checkbox(crypt_str("Static color"), &vars.movement.staticvelcolor);
			if (vars.movement.staticvelcolor)
				color_picker4(crypt_str("Velocity color"), vars.movement.velcolor);
			ImGui::PushID(crypt_str("faddderoino2222"));
			ImGui::Checkbox(crypt_str("Fade by velocity"), &vars.movement.velocityfade);
			ImGui::PopID();
			if (vars.movement.velocityfade)
			{
				ImGui::PushID(crypt_str("velfadeTrololo"));
				ImGui::Text(crypt_str("Velocity to fade to"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Velocity to fade to"), &vars.movement.velfadevelocity, 1.f, 286.f, crypt_str("%.1f u/s"));
				ImGui::PopID();
			}
		}

		ImGui::Checkbox(crypt_str("Draw stamina"), &vars.movement.showstamina);
		if (vars.movement.showstamina)
		{
			ImGui::PushID(crypt_str("staminapos"));
			ImGui::Text(crypt_str("Height"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Height"), &vars.movement.staminascrpos, 0.f, 1.f, crypt_str("%.4f"));
			ImGui::PopID();
			ImGui::Checkbox(crypt_str("Takeoff stamina"), &vars.movement.staminatakeoff);
			if (vars.movement.showtakeoff)
				AltCombo(crypt_str("Stamina Takeoff Type"), veltypelabels, 4, vars.movement.showstaminatype);
			color_picker4(crypt_str("Stamina color"), vars.movement.staminacol);
			ImGui::PushID(crypt_str("stafaddderoino2222"));
			ImGui::Checkbox(crypt_str("Fade by stamina"), &vars.movement.staminafade);
			ImGui::PopID();
			if (vars.movement.staminafade)
			{
				ImGui::PushID(crypt_str("staminafadeTrololo"));
				ImGui::Text(crypt_str("Stamina to fade to"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Stamina to fade to"), &vars.movement.staminafadevel, 1.f, 50.f, crypt_str("%.1f"));
				ImGui::PopID();
			}
		}


		ImGui::Checkbox(crypt_str("Draw graph"), &vars.movement.graph.enabled);
		if (vars.movement.graph.enabled)
		{
			AltMultiCombo(crypt_str("Graph type"), velgraphselectlabels, vars.movement.graph.types, 2);
			if (vars.movement.graph.types[0] || vars.movement.graph.types[1])
			{
				ImGui::Text(crypt_str("X Position"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("graphX Position"), &vars.movement.graph.xscreenpos, 0.f, 1.f, crypt_str("%.3f"));
				ImGui::Text(crypt_str("Size"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Size"), &vars.movement.graph.size, 32, 256);
				ImGui::Text(crypt_str("Width"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Width"), &vars.movement.graph.linewidth, 1, 10);
				ImGui::Text(crypt_str("Line thickness"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Line thickness"), &vars.movement.graph.thickness, 1, 10);
				ImGui::Text(crypt_str("Height"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderFloat(crypt_str("Height"), &vars.movement.graph.height, 0.f, 10.f);
				AltCombo(crypt_str("Graph Fade Type"), fadetypenames, 4, vars.movement.graph.fadetype);
				//ImGui::Text(crypt_str("\tGraph Fade Type"));
				//ImGui::Combo(fadetypenames[vars.movement.graph.fadetype].c_str(), &vars.movement.graph.fadetype, crypt_str("No Fade\0Fade start and end\0Fade by velocity\0Fade by start, end, velocity\0"));
				if (vars.movement.graph.fadetype == 2 || vars.movement.graph.fadetype == 3)
				{
					if (vars.movement.graph.types[0])
					{
						ImGui::PushID(crypt_str("grpher"));
						ImGui::Text(crypt_str("Max Fade Speed"));
						ImGui::SetNextItemWidth(270.f);
						ImGui::SliderFloat(crypt_str("Max Fade Speed"), &vars.movement.graph.speedtofade, 1.f, 286.f, crypt_str("%.1f u/s"));
						ImGui::PopID();
					}

					if (vars.movement.graph.types[1])
					{
						ImGui::PushID(crypt_str("grpherstam"));
						ImGui::Text(crypt_str("Max Fade Stamina"));
						ImGui::SetNextItemWidth(270.f);
						ImGui::SliderFloat(crypt_str("Max Fade Stamina"), &vars.movement.graph.speedtofade, 1.f, 50.f, crypt_str("%.1f"));
						ImGui::PopID();
					}

				}
				//ImGui::SliderFloat(crypt_str("Speed"), &vars.movement.graph.speed, 0.1f, 5.f);
				if (vars.movement.graph.types[0])
				{
					if (!vars.movement.graph.gaincolors)
						color_picker4(crypt_str("Velocity line color"), vars.movement.graph.color);

					ImGui::Checkbox(crypt_str("Dynamic velocity color"), &vars.movement.graph.gaincolors);
					color_picker4(crypt_str("Velocity text color"), vars.movement.graph.textcolor);
				}

				if (vars.movement.graph.types[1])
				{
					if (!vars.movement.graph.staminagaincolors)
						color_picker4(crypt_str("Stamina line color"), vars.movement.graph.staminacol);

					ImGui::Checkbox(crypt_str("Dynamic stamina color"), &vars.movement.graph.staminagaincolors);
					color_picker4(crypt_str("Stamina text color"), vars.movement.graph.staminatextcol);
				}
			}

		}

		
		AltMultiCombo(crypt_str("Key Press Indicators"), (std::string*)ind_labels, vars.movement.indicators.on, 8);
		
		parar = 0;
		for (int i = 0; i < 8; i++)
			parar += vars.movement.indicators.on[i] ? 1 : 0;

		if (parar > 0)
		{
			ImGui::Checkbox(crypt_str("Draw active color"), &vars.movement.indicators.show_active);
			ImGui::SameLine();
			color_picker4(crypt_str("indi_active_colar"), vars.movement.indicators.active_col, false);
			ImGui::SameLine();
			color_picker4(crypt_str("indi_passive_colar"), vars.movement.indicators.passive_col, false);
			ImGui::SliderFloat(crypt_str("Indicator height"), &vars.movement.indicators.height, 0.f, 1.f, crypt_str("%.3f"));
		}

		AltEndChild();

		AltBeginChild({ 10 + ((window_size.x - 10) / 2), 65 }, { window_size.x - 5, window_size.y - 10 }, crypt_str("Movement"));

		ImGui::Checkbox(crypt_str("Bunnyhop"), &vars.misc.bunnyhop);

		if (vars.misc.restrict_type != 2)
		{
			
			ImGui::Checkbox(crypt_str("Fast Duck"), &vars.movement.fastduck);
		}

		//ImGui::Checkbox(crypt_str("Slidewalk"), &vars.movement.slidewalk);
		ImGui::Checkbox(crypt_str("Auto Strafe"), &vars.misc.autostrafe);
		if (vars.misc.autostrafe)
		{
			ImGui::Checkbox(crypt_str("On key"), &vars.misc.autostrafeonkey);
			if (vars.misc.autostrafeonkey)
			 vars.misc.autostrafekey.imgui(crypt_str("Auto Strafe Key"));

			AltCombo(crypt_str("Auto Strafe Type"), autostrafetype_labels, 2, vars.movement.autostrafetype);
			//ImGui::Combo(vars.movement.autostrafetype ? crypt_str("Rage") : crypt_str("AutoKey"), &vars.movement.autostrafetype, crypt_str("Rage\0AutoKey\0"));
			if (vars.movement.autostrafetype == 1)
			{
				ImGui::PushID(crypt_str("efjoeofjeoj"));
				ImGui::Text(crypt_str("Mouse horizontal speed to strafe"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("Mouse horizontal speed to strafe"), &vars.movement.dxtostrafe, 0, 50);
				ImGui::PopID();
			}
		}


		ImGui::PushID(crypt_str("bbot"));
		vars.misc.blockbot.imgui(crypt_str("Blockbot"));
		ImGui::PopID();

		ImGui::PushID(crypt_str("pass"));
		vars.misc.peek_assist.imgui(crypt_str("Peek assistance"));
		ImGui::PopID();

		ImGui::PushID(crypt_str("ejej"));
		vars.movement.edgejumpkey.imgui(crypt_str("Edge Jump Key"));
		ImGui::PopID();
		if (vars.movement.edgejumpkey.valid())
			ImGui::Checkbox(crypt_str("Edge Jump from Ladders"), &vars.movement.ladderej);

		ImGui::PushID(crypt_str("ladderGLID"));
		vars.movement.ladderglidekey.imgui(crypt_str("Ladder Glide Key"));
		ImGui::PopID();

		ImGui::PushID(crypt_str("ljlj"));
		vars.movement.longjumpkey.imgui(crypt_str("Long Jump Bind Key"));
		ImGui::PopID();

		ImGui::PushID(crypt_str("mjmj"));
		vars.movement.minijumpkey.imgui(crypt_str("Mini Jump key"));
		ImGui::PopID();
		

		ImGui::PushID(crypt_str("jbjb"));
		vars.movement.jumpbugkey.imgui(crypt_str("Jumpbug key"));
		ImGui::PopID();
		


		ImGui::PushID(crypt_str("eb1"));
		vars.movement.edgebug.key.imgui(crypt_str("Edgebug Key"));
		ImGui::PopID();
		if (vars.movement.edgebug.key.valid())
		{
			ImGui::Checkbox(crypt_str("Advanced Detection"), &vars.movement.edgebug.detect_strafe);
			ImGui::PushID(crypt_str("eb2"));
			ImGui::Text(crypt_str("Tick Limit"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderInt(crypt_str("Tick Limit"), &vars.movement.edgebug.ticks, 0, 64);
			ImGui::PopID();
			ImGui::PushID(crypt_str("eb3"));
			ImGui::Text(crypt_str("Lock"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Lock"), &vars.movement.edgebug.lock, 0.f, 1.f, "%.3f");
			ImGui::PopID();
			AltCombo(crypt_str("Lock Type"), locktype_labels, 2, vars.movement.edgebug.locktype);
			ImGui::Checkbox(crypt_str("Visualize Path"), &vars.movement.edgebug.marker);
			if (vars.movement.edgebug.marker)
			{
				color_picker4(crypt_str("Path Color"), vars.movement.edgebug.marker_color);
				ImGui::Text(crypt_str("Path Thickness"));
				ImGui::SetNextItemWidth(270.f);
				ImGui::SliderInt(crypt_str("##paththick"), &vars.movement.edgebug.marker_thickness, 0, 10);
			}
		}

		

		//ImGui::Checkbox(crypt_str("EB Detect - Effect"), &vars.movement.ebdetect.effect);
		//ImGui::Checkbox(crypt_str("EB Detect - Chat"), &vars.movement.ebdetect.chat);
		//ImGui::Checkbox(crypt_str("EB Detect - Counter"), &vars.movement.ebdetect.counter);
		//ImGui::Checkbox(crypt_str("EB Detect - Sound"), &vars.movement.ebdetect.sound);
		

		
		ImGui::PushID(crypt_str("pxpx"));
		vars.movement.autopixelsurfkey.imgui(crypt_str("Autoduck on pixelsurf key"));
		if (vars.movement.autopixelsurfkey.valid())
		{
			ImGui::Checkbox(crypt_str("Freestanding on pixelsurf"), &vars.movement.autopixelsurf_freestand);

			ImGui::Checkbox(crypt_str("Align indicator"), &vars.movement.autopixelsurf_alignind);
			if (vars.movement.autopixelsurf_alignind)
				color_picker4(crypt_str("Indicator color"), vars.movement.autopixelsurf_indcol1);
		}
		ImGui::PopID();

		ImGui::PushID(crypt_str("alialing"));
		vars.movement.autopixelsurf_align.imgui(crypt_str("Auto align (experimental)"));
		ImGui::PopID();


		ImGui::Checkbox(crypt_str("Mouse speed limiter"), &vars.movement.msl);
		if (vars.movement.msl)
		{
			ImGui::PushID(crypt_str("13223sdddd")); 
			ImGui::Text(crypt_str("Max horizontal speed"));
			ImGui::SetNextItemWidth(270.f);
			ImGui::SliderFloat(crypt_str("Max horizontal speed"), &vars.movement.mslx, 0.f, 100.f, crypt_str("%.3f"));
			ImGui::PopID();
		}

		


		AltEndChild();
		break;
	case 5: // config
		//ImGui::SetCursorPosX(5);
		//ImGui::SetCursorPosY(70);
		//ImGui::BeginChild("Configors", { (window_size.x - 10) / 2, window_size.y - 80 }, true, ImGuiWindowFlags_NoScrollbar);
		AltBeginChild({ 5 ,  65 }, { 5 + ((window_size.x - 10) / 2), 80 + (window_size.x / 2) }, crypt_str("Configs"));


		if (!ConfigList.empty())
		{
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::ListBoxHeader(crypt_str("##configerinos")))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (int n = 0; n < ConfigList.size(); n++)
				{
					bool isselected = (vars.menu.active_config_index == std::clamp(n, 0, (int)ConfigList.size()));
					if (ImGui::Selectable(ConfigList.at(n).c_str(), isselected, 0, {305.f, 16.f}))
					{
						vars.menu.active_config_index = n;
					}
				}
				ImGui::ListBoxFooter();
			}
		}

		//ImGui::Combo(ConfigList.at(vars.menu.active_config_index).c_str(), &vars.menu.active_config_index, configs.c_str());
		ImGui::SetNextItemWidth(240.f);
		ImGui::PushID(crypt_str("Confignameinput"));
		ImGui::InputTextWithHint(crypt_str("Config name"), crypt_str(" Input config name to create"), buffer, 32);
		ImGui::PopID();



		vars.menu.active_config_name = std::string(buffer);
		if (!vars.menu.active_config_name.empty())
		{
			if (ImGui::Button(crypt_str("Create"), { 270, 23 }))
			{
				std::string add;
				if (vars.menu.active_config_name.find(crypt_str(".cfg")) == -1)
					add = crypt_str(".cfg");
				Config.Save(vars.menu.active_config_name + add);
				vars.menu.active_config_name = "";
				memset(buffer, 0, 32);
				RefreshConfigs();
			}
		}

		if (ImGui::Button(crypt_str("Open config folder"), { 270, 23 }))
		{
			std::string BAM = crypt_str("start ");
			BAM += csgo->config_directory;
			system(BAM.c_str());
		}

		if (ConfigList.size() > 0 && vars.menu.active_config_index > -1)
		{

			if (ImGui::Button(crypt_str("Load"), { 270, 23 }))
			{
				vars.menu.active_config_name = ConfigList.at(vars.menu.active_config_index);
				Config.Load(ConfigList.at(vars.menu.active_config_index));
				memcpy(buffer, ConfigList.at(vars.menu.active_config_index).c_str(), max(ConfigList.at(vars.menu.active_config_index).size(), 32));
				Skinchanger::Get().scheduleHudUpdate();
			}


			if (ImGui::Button(crypt_str("Save"), { 270, 23 }))
			{
				Config.Save(ConfigList.at(vars.menu.active_config_index));
			}
		}


		if (ImGui::Button(crypt_str("Load defaults"), { 270, 23 }))
		{
			Config.ResetToDefault();
			//Skinchanger::Get().scheduleHudUpdate();
		}

		if (ImGui::Button(crypt_str("Refresh configs"), { 270, 23 }))
		{
			vars.menu.active_config_index = 0;
			RefreshConfigs();
		}

		

		
		AltEndChild();

		AltBeginChild({ 5, 85 + (window_size.x / 2) }, { 5 + ((window_size.x - 10) / 2),  window_size.y - 10 }, crypt_str("Windows"));
		ImGui::Text(crypt_str("Background transparency"));
		ImGui::SetNextItemWidth(270.f);
		ImGui::SliderInt(crypt_str("##window_opacityslider"), &vars.menu.window_opacity, 0, 100);

		ImGui::Checkbox(crypt_str("Keybinds"), &vars.visuals.indicators);
		ImGui::Checkbox(crypt_str("Spectators"), &vars.visuals.speclist);
		ImGui::Checkbox(crypt_str("Team damage"), &vars.visuals.teamdmglist);
		
		if (vars.visuals.teamdmglist)
		{
			ImGui::Checkbox(crypt_str("Cooldown say"), &vars.visuals.cooldownsay);
			if (vars.visuals.cooldownsay)
			{
				ImGui::PushID(crypt_str("seXoANalecooldownsay"));
				ImGui::InputText(crypt_str("Cooldown say text"), vars.visuals.cooldownsaytext, 32);
				ImGui::PopID();

				ImGui::Checkbox(crypt_str("Add counter text to the end"), &vars.visuals.addcounter);
			}
		}
		ImGui::Checkbox(crypt_str("Recorder"), &vars.movement.recorder.enabled);

		ImGui::Checkbox(crypt_str("Checkpoint Menu"), &vars.movement.checkpoint.enabled);
		if (vars.movement.checkpoint.enabled)
		{

			vars.movement.checkpoint.teleportkey.imgui(crypt_str("Teleport Key"));

			vars.movement.checkpoint.checkpointkey.imgui(crypt_str("Checkpoint Key"));

			vars.movement.checkpoint.nextkey.imgui(crypt_str("Next Checkpoint Key"));

			vars.movement.checkpoint.prevkey.imgui(crypt_str("Prev Checkpoint Key"));

			vars.movement.checkpoint.undokey.imgui(crypt_str("Undo Checkpoint Key"));

			color_picker4(crypt_str("Checkpoint marker color"), vars.movement.checkpoint.color);
		}

		AltEndChild();




		AltBeginChild({ 10 + ((window_size.x - 10) / 2), 65 }, { window_size.x - 5, window_size.y - 10 }, crypt_str("Skins and Models"));
		
		
		

		ImGui::Text(crypt_str("Weapon"));
		ImGui::SetNextItemWidth(305.f);
		ImGui::Combo(crypt_str("##weaponselectorskinhang"), &selected_id, [](void* data, int idx, const char** out_text)
			{
				*out_text = game_data::weapon_names.at(idx).name;
				return true;
			}, nullptr, game_data::weapon_names.size(), 8);

		selected_entry = g_config.get_items().at(selected_id);
		selected_entry.definition_vector_index = selected_id;
		ImGui::PushID(std::hash<std::string>{}(game_data::weapon_names[selected_id].name));

		{
			



			// Enabled
			ImGui::Checkbox(crypt_str("Enabled"), &selected_entry.enabled);


			// Paint kit

			ImGui::Text(crypt_str("Skin"));


			ImGui::SetNextItemWidth(305.f);
			ImGui::PushID(crypt_str("SkinSearchS"));
			ImGui::InputText(crypt_str(" Search for skin"), SkinFilter, 32);
			ImGui::PopID();


			if (selected_entry.definition_index != GLOVE_T_SIDE)
			{

				ImGui::SetNextItemWidth(305.f);
				if(ImGui::ListBoxHeader(crypt_str("##normalskinheader")))
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
					std::string lastname = crypt_str("lalaltrololo");
					for (int i = 0; i < game_data::skin_kits.size(); i++)
					{
						bool is_selected = (selected_entry.paint_kit_vector_index == i);
						std::string Skrrt = SkinFilter;
						
						if (!Skrrt.empty())
						{
							std::string hay = to_lower_string(game_data::skin_kits[i].name);
							std::string needle = to_lower_string(Skrrt);
							if (hay.find(needle) == string::npos)
								continue;
						}


						//std::string label = game_data::skin_kits[i].name + crypt_str("###food") + to_string(i);

						if (lastname == game_data::skin_kits[i].name)
							ImGui::PushID(std::hash<std::string>{}(game_data::skin_kits[i].name)* i);


						if (ImGui::Selectable(game_data::skin_kits[i].name.c_str(), &is_selected, 0, {305.f, 16.f}))
						{
							selected_entry.paint_kit_vector_index = i;
							selected_entry.colors[0] = game_data::skin_kits[i].link->color1;
							selected_entry.colors[1] = game_data::skin_kits[i].link->color2;
							selected_entry.colors[2] = game_data::skin_kits[i].link->color3;
							selected_entry.colors[3] = game_data::skin_kits[i].link->color4;
							selected_entry.pearlescent = game_data::skin_kits[i].link->pearlescent;
							//memset(SkinFilter, 0, 32);
						}

						if (lastname == game_data::skin_kits[i].name)
							ImGui::PopID();

						lastname = game_data::skin_kits[i].name;
						


						if (is_selected)
							ImGui::SetItemDefaultFocus();

					}
					ImGui::ListBoxFooter();
				}
				
			}
			else
			{

				
					
				
				
					
				
				
				ImGui::SetNextItemWidth(305.f);
				if (ImGui::ListBoxHeader(crypt_str("##gloveskinheader")))
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
					for (int i = 0; i < game_data::glove_kits.size(); i++)
					{
						bool is_selected = (selected_entry.paint_kit_vector_index == i);
						std::string Skrrt = SkinFilter;
						std::string hay = to_lower_string(game_data::glove_kits[i].name);
						std::string needle = to_lower_string(Skrrt);

						if (!Skrrt.empty())
						{
							if (hay.find(needle) == string::npos)
								continue;
						}


						//std::string label = game_data::glove_kits[i].name + crypt_str("###foo") + to_string(i);

						ImGui::PushID(i * 23);

						if (ImGui::Selectable(game_data::glove_kits[i].name.c_str(), &is_selected, 0, {305.f, 16.f}))
						{
							selected_entry.paint_kit_vector_index = i;
							selected_entry.colors[0] = game_data::skin_kits[i].link->color1;
							selected_entry.colors[1] = game_data::skin_kits[i].link->color2;
							selected_entry.colors[2] = game_data::skin_kits[i].link->color3;
							selected_entry.colors[3] = game_data::skin_kits[i].link->color4;
							selected_entry.pearlescent = game_data::skin_kits[i].link->pearlescent;
							//memset(SkinFilter, 0, 32);
						}

						ImGui::PopID();

						if (is_selected)
							ImGui::SetItemDefaultFocus();

					}
					ImGui::ListBoxFooter();
				}
				
			}

			

			// Yes we do it twice to decide knifes
			selected_entry.update<sync_type::KEY_TO_VALUE>();

			
			// Item defindex override
			if (selected_entry.definition_index == WEAPON_KNIFE)
			{

				ImGui::Text(crypt_str("Knife"));
				ImGui::SetNextItemWidth(305.f);
				ImGui::Combo(crypt_str("##knifselect"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = game_data::knife_names.at(idx).name;
						return true;
					}, nullptr, game_data::knife_names.size(), 5);
			}
			else if (selected_entry.definition_index == GLOVE_T_SIDE)
			{

				ImGui::Text(crypt_str("Glove"));
				ImGui::SetNextItemWidth(305.f);
				ImGui::Combo(crypt_str("##glovselect"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = game_data::glove_names.at(idx).name;
						return true;
					}, nullptr, game_data::glove_names.size(), 5);
			}
			else
			{
				// We don't want to override weapons other than knives or gloves

				static auto unused_value = 0;
				selected_entry.definition_override_vector_index = 0;
				//ImGui::Combo(crypt_str("------------"), &unused_value, crypt_str("For knives or gloves\0"));
			}

			selected_entry.update<sync_type::KEY_TO_VALUE>();

			

			// Quality

			ImGui::Text(crypt_str("Custom colors"));
			std::array<float,3> color1 = colorconvert(selected_entry.colors[0]);
			if (ImGui::ColorEdit3(crypt_str("##Color1"), (float*)&color1, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
			{
				int color1int = 0xFF000000 + ((int)(color1[2] * 255) << 16) | ((int)(color1[1] * 255) << 8) | (int)(color1[0] * 255);
				selected_entry.colors[0] = color1int;
				CPaintKit* pk = (CPaintKit*)game_data::skin_kits[selected_entry.paint_kit_vector_index].link;
				pk->color1 = color1int;
				
			}
			ImGui::SameLine();
			std::array<float, 3> color2 = colorconvert(selected_entry.colors[1]);
			if (ImGui::ColorEdit3(crypt_str("##Color2"), (float*)&color2, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
			{
				int color2int = 0xFF000000 + ((int)(color2[2] * 255) << 16) | ((int)(color2[1] * 255) << 8) | (int)(color2[0] * 255);
				selected_entry.colors[1] = color2int;
				CPaintKit* pk = (CPaintKit*)game_data::skin_kits[selected_entry.paint_kit_vector_index].link;
				pk->color2 = color2int;
				
			}
			ImGui::SameLine();
			std::array<float, 3> color3 = colorconvert(selected_entry.colors[2]);
			if (ImGui::ColorEdit3(crypt_str("##Color3"), (float*)&color3, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
			{
				int color3int = 0xFF000000 + ((int)(color3[2] * 255) << 16) | ((int)(color3[1] * 255) << 8) | (int)(color3[0] * 255);
				selected_entry.colors[2] = color3int;
				CPaintKit* pk = (CPaintKit*)game_data::skin_kits[selected_entry.paint_kit_vector_index].link;
				pk->color3 = color3int;
				
			}
			ImGui::SameLine();
			std::array<float, 3> color4 = colorconvert(selected_entry.colors[3]);
			if (ImGui::ColorEdit3(crypt_str("##Color4"), (float*)&color4, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
			{
				int color4int = 0xFF000000 + ((int)(color4[2] * 255) << 16) | ((int)(color4[1] * 255) << 8) | (int)(color4[0] * 255);
				selected_entry.colors[3] = color4int;
				CPaintKit* pk = (CPaintKit*)game_data::skin_kits[selected_entry.paint_kit_vector_index].link;
				pk->color4 = color4int;
				
			}

			ImGui::Text(crypt_str("Pearlescent"));
			ImGui::SetNextItemWidth(235.f);
			if (ImGui::SliderFloat(crypt_str("##eeePearlescent"), &selected_entry.pearlescent, 0.f, 255.f, crypt_str("%.1f")))
			{
				CPaintKit* pk = (CPaintKit*)game_data::skin_kits[selected_entry.paint_kit_vector_index].link;
				pk->pearlescent = selected_entry.pearlescent;
			}
			

			ImGui::PushID(crypt_str("qualqual"));
			ImGui::Text(crypt_str("Prefix"));
			ImGui::SetNextItemWidth(305.f);
			ImGui::Combo(crypt_str("##quality_prefix_selector"), &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
				{
					*out_text = game_data::quality_names[idx].name;
					return true;
				}, nullptr, game_data::quality_names.size(), 5);
			ImGui::PopID();

			

			ImGui::Text(crypt_str("Wear"));
			ImGui::SetNextItemWidth(235.f);
			ImGui::SliderFloat(crypt_str("Wear"), &selected_entry.wear, FLT_MIN, 1.f, crypt_str("%.10f"));
			ImGui::InputInt(crypt_str("StatTrak"), &selected_entry.stat_trak);
			ImGui::InputInt(crypt_str("Seed"), &selected_entry.seed);
			ImGui::PushID(crypt_str("name taggeringChangering"));
			ImGui::InputText(crypt_str("Name Tag"), selected_entry.custom_name, 32);
			ImGui::PopID();
			g_config.get_items().at(selected_id) = selected_entry;
		}

		ImGui::PopID();


		static int DD{ vars.visuals.playermodelct };

		ImGui::Text(crypt_str("CT Model"));
		ImGui::SetNextItemWidth(305.f);
		ImGui::Combo(crypt_str("Label"), &vars.visuals.playermodelct, crypt_str("Default\0Agent Ava\0Operator\0Markus Delrow\0Michael Syfers\0Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Ricksaw\0Third Commando Company\0McCoy\0Dragomir\0Rezan The Ready\0Romanov\0Maximus\0Blackwolf\0Mr.Muhlik\0Ground Rebel\0Osiris\0Prof. Shahmat\0Enforcer\0Slingshot\0Soldier\0Pirate 1\0Pirate 2\0Pirate 3\0Pirate 4\0Pirate 5\0Anarchist 1\0Anarchist 2\0Anarchist 3\0Anarchist 4\0Anarchist 5\0Balkan 1\0Balkan 2\0Balkan 3\0Balkan 4\0Balkan 5\0Jumpsuit 1\0Jumpsuit 2\0Jumpsuit 3\0Street Soldier\0Buckshot\0McCoy\0Rezan\0Dragomir\0Jamison\0Farlow\0John Kask\0Bio-Haz Specialis\0Sergeant Bombson\0Chem-Haz Specialist\0Sir Bloody Miami Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Skullhead Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Loudmouth Darryl\0Safecracker Voltzmann\0Little Kev\0Number K\0Getaway Sally\0"));
		if (DD != vars.visuals.playermodelct)
		{
			Skinchanger::Get().scheduleHudUpdate();
			DD = vars.visuals.playermodelct;
		}



		static int CC{ vars.visuals.playermodelt };
		
		ImGui::Text(crypt_str("T Model"));
		ImGui::SetNextItemWidth(305.f);
		ImGui::Combo(crypt_str("LabelE"), &vars.visuals.playermodelt, crypt_str("Default\0Agent Ava\0Operator\0Markus Delrow\0Michael Syfers\0Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Ricksaw\0Third Commando Company\0McCoy\0Dragomir\0Rezan The Ready\0Romanov\0Maximus\0Blackwolf\0Mr.Muhlik\0Ground Rebel\0Osiris\0Prof. Shahmat\0Enforcer\0Slingshot\0Soldier\0Pirate 1\0Pirate 2\0Pirate 3\0Pirate 4\0Pirate 5\0Anarchist 1\0Anarchist 2\0Anarchist 3\0Anarchist 4\0Anarchist 5\0Balkan 1\0Balkan 2\0Balkan 3\0Balkan 4\0Balkan 5\0Jumpsuit 1\0Jumpsuit 2\0Jumpsuit 3\0Street Soldier\0Buckshot\0McCoy\0Rezan\0Dragomir\0Jamison\0Farlow\0John Kask\0Bio-Haz Specialis\0Sergeant Bombson\0Chem-Haz Specialist\0Sir Bloody Miami Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Skullhead Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Loudmouth Darryl\0Safecracker Voltzmann\0Little Kev\0Number K\0Getaway Sally\0"));
		if (CC != vars.visuals.playermodelt)
		{
			Skinchanger::Get().scheduleHudUpdate();
			CC = vars.visuals.playermodelt;
		}



		if (ImGui::Button(crypt_str("Update"), { 270, 23 }))
		{
			Skinchanger::Get().scheduleHudUpdate();

		}

		AltEndChild();

		break;
	case 6: // lua
		AltBeginChild({ 5 ,  65 }, { 5 + ((window_size.x - 10) / 2), window_size.y - 10 }, crypt_str("LUA Scripting"));


		if (!CLua::Get().scripts.empty())
		{
			ImGui::SetNextItemWidth(305.f);
			if (ImGui::ListBoxHeader(crypt_str("##LUACOMBO"), {0.f,window_size.y * 0.6f}))
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
				for (int n = 0; n < CLua::Get().scripts.size(); n++)
				{
					const bool is_selected = (lua_current_item == n);

					if (CLua::Get().loaded.at(n))
					{
						std::string txt = CLua::Get().scripts.at(n) + crypt_str("\t[ Running ]");
						if (ImGui::Selectable(txt.c_str(), is_selected))
							lua_current_item = n;
					}
					else
					{
						if (ImGui::Selectable(CLua::Get().scripts.at(n).c_str(), is_selected))
							lua_current_item = n;
					}
				}
				ImGui::ListBoxFooter();
			}


			if (CLua::Get().loaded.at(lua_current_item))
			{
				
				if (ImGui::Button(crypt_str("Unload script"), { 270, 23 }))
				{
					CLua::Get().unload_script(lua_current_item);
					CLua::Get().refresh_scripts();
				}
			}
			else
			{
				
				if (ImGui::Button(crypt_str("Load script"), { 270, 23 }))
				{
					CLua::Get().load_script(lua_current_item);
					CLua::Get().refresh_scripts();
				}
			}
		}

		
		if (ImGui::Button(crypt_str("Refresh scripts"), { 270, 23 }))
		{
			CLua::Get().refresh_scripts();
		}

		
		if (ImGui::Button(crypt_str("Open LUA folder"), { 270, 23 }))
		{
			std::string BAM = crypt_str("start ");
			BAM += csgo->config_directory;
			BAM += crypt_str("Scripts\\");
			system(BAM.c_str());
		}


		AltEndChild();
		

		AltBeginChild({ 10 + ((window_size.x - 10) / 2), 65 }, { window_size.x - 5, window_size.y - 10 }, crypt_str("LUA Items"));

		if (csgo->script_loaded)
			for (auto current : CLua::Get().hooks.get_hooks(crypt_str("menu_items")))
				current.func();

		AltEndChild();

		break;
	}
	ImGui::PopFont();

	ImGui::End();

	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(23.f / 255.f, 30.f / 255.f, 41.f / 255.f, vars.menu.window_opacity /100.f));

	/*
	if (playerlist_enabled)
	{
		ImGui::GetStyle().WindowPadding = { 5,3 };
		ImGui::Begin("Player List", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		if (ImGui::BeginTable("playar list", 3))
		{
			for (int row = 0; row < 4; row++)
			{
				ImGui::PushID(row * 32435);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text(players[row].name.c_str());
				ImGui::Dummy({ 0, 2 });
				ImGui::Separator();

				ImGui::TableNextColumn();
				ImGui::Checkbox("Im want Shoot", &players[row].shoot);
				ImGui::Separator();
				ImGui::TableNextColumn();
				ImGui::Checkbox("Dis is Freind", &players[row].is_friend);
				if (row != 3)
				{
					ImGui::Separator();
				}
				else
				{
					ImGui::Spacing();
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}


		ImGui::End();
	}
	*/


	ImGui::PopStyleColor(16);

}



c_menu* g_Menu = new c_menu();