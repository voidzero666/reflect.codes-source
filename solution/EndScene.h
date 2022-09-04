#include "Hooks.h"
#include "Menu.h"
#include "imgui\imgui_impl_dx9.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_freetype.h"
#include <d3dx9tex.h>
#include "AntiAims.h"
#include "Eventlog.h"
#include "protect/encoding.hpp"
#include <chrono>
#include "manrope_meduim_ttf.h"
#include "Recorder.h"
#include "scripting.h"
#include "images.h"

void GUI_Init(IDirect3DDevice9* pDevice)
{
	if (!vars.menu.guiinited)
	{
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(csgo->Init.Window);
		ImGui_ImplDX9_Init(pDevice);

		ImGuiIO& io = ImGui::GetIO();

		auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		{
			return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
		};

		ImGuiStyle& style = ImGui::GetStyle();
		style.AntiAliasedFill = false;
		style.AntiAliasedLines = false;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_FrameBg] = ColorFromBytes(60, 60, 60, 127);
		colors[ImGuiCol_Button] = ColorFromBytes(60, 60, 60, 127);
		colors[ImGuiCol_ButtonHovered] = ColorFromBytes(60, 60, 60, 127);
		colors[ImGuiCol_ButtonActive] = ColorFromBytes(80, 80, 80, 200);

		ImFontConfig cfg;
		cfg.OversampleH = 3;
		cfg.OversampleV = 1;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x044F, // Cyrillic
			0,
		};

		static const ImWchar sexanal[] =
		{
			0x0020, 0x00FF,
			0xE000, 0xE100,
			0,
		};

		

		fonts::menu_main = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/arialbd.ttf"), 12, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::menu_desc = io.Fonts->AddFontFromMemoryTTF(rawDatattf, sizeof(rawDatattf), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		//fonts::menu_desc = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/comicbd.ttf"), 20, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		fonts::lby_indicator = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/tahomabd.ttf"), 25, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::damage = io.Fonts->AddFontFromMemoryTTF(rawDatattf, sizeof(rawDatattf), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		fonts::hitmarker = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/micross.ttf"), 17, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::esp_name = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/micross.ttf"), 12, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::esp_info = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/micross.ttf"), 10, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		//fonts::esp_logs = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/micross.ttf"), 12, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		fonts::esp_logs = io.Fonts->AddFontFromMemoryTTF(rawDatattf, sizeof(rawDatattf), 14, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		fonts::logger = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/Verdana.ttf"), 14, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		fonts::logo = io.Fonts->AddFontFromMemoryTTF(orbitron, sizeof(orbitron), 50.0f, &cfg, ranges);
		fonts::tab_names = io.Fonts->AddFontFromMemoryTTF(orbitron, sizeof(orbitron), 15.0f, &cfg, ranges);
		fonts::font = io.Fonts->AddFontFromMemoryTTF(nigger, sizeof(nigger), 13.0f, &cfg, ranges);
		fonts::graph_font = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/micross.ttf"), 12, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::move_indicator = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/tahomabd.ttf"), 28, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::move_indicator_small = io.Fonts->AddFontFromFileTTF(crypt_str("C:/windows/fonts/tahomabd.ttf"), 22, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::csgoicons = io.Fonts->AddFontFromMemoryTTF(csgoiconfont, sizeof(csgoiconfont), 15, &cfg, ranges);
		fonts::csgoicons_large = io.Fonts->AddFontFromMemoryTTF(csgoiconfont, sizeof(csgoiconfont), 19, &cfg, ranges);

		
		csgo->Utils.LoadTextureFromMemory((void*)xtclogoPNG, sizeof(xtclogoPNG), &LoadedImages::xtclogo.texture, &LoadedImages::xtclogo.w, &LoadedImages::xtclogo.h);
		csgo->Utils.LoadTextureFromMemory((void*)penosPNG, sizeof(penosPNG), &LoadedImages::penisTab.texture, &LoadedImages::penisTab.w, &LoadedImages::penisTab.h);
		csgo->Utils.LoadTextureFromMemory((void*)penosPNG, sizeof(penosPNG), &LoadedImages::penisTab.texture, &LoadedImages::penisTab.w, &LoadedImages::penisTab.h);
		csgo->Utils.LoadTextureFromMemory((void*)targetPNG, sizeof(targetPNG), &LoadedImages::tabIcons[0].texture, &LoadedImages::tabIcons[0].w, &LoadedImages::tabIcons[0].h);
		csgo->Utils.LoadTextureFromMemory((void*)customerPNG, sizeof(customerPNG), &LoadedImages::tabIcons[1].texture, &LoadedImages::tabIcons[1].w, &LoadedImages::tabIcons[1].h);
		csgo->Utils.LoadTextureFromMemory((void*)visionPNG, sizeof(visionPNG), &LoadedImages::tabIcons[2].texture, &LoadedImages::tabIcons[2].w, &LoadedImages::tabIcons[2].h);
		csgo->Utils.LoadTextureFromMemory((void*)scanningPNG, sizeof(scanningPNG), &LoadedImages::tabIcons[3].texture, &LoadedImages::tabIcons[3].w, &LoadedImages::tabIcons[3].h);
		csgo->Utils.LoadTextureFromMemory((void*)misctabPNG, sizeof(misctabPNG), &LoadedImages::tabIcons[4].texture, &LoadedImages::tabIcons[4].w, &LoadedImages::tabIcons[4].h);
		csgo->Utils.LoadTextureFromMemory((void*)settingsPNG, sizeof(settingsPNG), &LoadedImages::tabIcons[5].texture, &LoadedImages::tabIcons[5].w, &LoadedImages::tabIcons[5].h);
		csgo->Utils.LoadTextureFromMemory((void*)web_programmingPNG, sizeof(web_programmingPNG), &LoadedImages::tabIcons[6].texture, &LoadedImages::tabIcons[6].w, &LoadedImages::tabIcons[6].h);
	
		
		
		//csgo->Utils.LoadTextureFromMemory((void*)xtclogoPNG, sizeof(xtclogoPNG), &textures::xtclogo.texture, &textures::xtclogo.w, &textures::xtclogo.h);

		ImGuiFreeType::BuildFontAtlas(io.Fonts, 0x0);

		ImGui_ImplDX9_CreateDeviceObjects();

		vars.menu.guiinited = true;
	}
}

LPDIRECT3DTEXTURE9 img = nullptr;
LPD3DXSPRITE sprite = nullptr;

/*
void DrawTaserRange() {
	if (!csgo->should_draw_taser_range || !vars.visuals.taser_range)
		return;
	static float rainbow = 0.f;
	rainbow += 0.001f;
	if (rainbow > 1.f)
		rainbow = 0.f;
	std::pair<Vector, Vector> last = std::pair(Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f));
	color_t last_color = color_t(0, 0, 0, 0);
	color_t first_color = color_t(255, 255, 255, 0);

	constexpr size_t size = 106;

	for (size_t i = 0; i < size; i++)
	{
		auto& trace = csgo->taser_ranges[i];
		float r = rainbow + float(i + 1) / size;
		while (r >= 1.f) r -= 1.f;
		while (r < 0.f) r += 1.f;
		color_t clr = color_t::hsb(r, 1.f, 1.f);
		if (i == 0) first_color = clr;
		std::pair<Vector, Vector> buf;
		if (last.first != Vector(0.f, 0.f, 0.f) && last.second != Vector(0.f, 0.f, 0.f)) {
			Vector v1, v2;
			if (Math::WorldToScreen(trace.first, v1) && Math::WorldToScreen(trace.second, v2)) {
				if (i == 105)
					last_color = first_color;
				if (vars.visuals.taser_range == 1)
					g_Render->_drawList->AddQuadFilledMultiColor(
						ImVec2(last.second.x, last.second.y), ImVec2(v2.x, v2.y), ImVec2(v1.x, v1.y),
						ImVec2(last.first.x, last.first.y),
						last_color.manage_alpha(0).u32(), clr.manage_alpha(0).u32(),
						clr.manage_alpha(255).u32(), last_color.manage_alpha(255).u32());
				else
					g_Render->_drawList->AddQuadFilledMultiColor(
						ImVec2(last.second.x, last.second.y), ImVec2(v2.x, v2.y), ImVec2(v1.x, v1.y),
						ImVec2(last.first.x, last.first.y),
						vars.visuals.taser_range_color.manage_alpha(0).u32(), vars.visuals.taser_range_color.manage_alpha(0).u32(),
						vars.visuals.taser_range_color.u32(), vars.visuals.taser_range_color.u32());
			}
		}
		last_color = clr;
		Math::WorldToScreen(trace.first, last.first);
		Math::WorldToScreen(trace.second, last.second);
	}
}

*/
bool onnn = false;
static HRESULT __stdcall Hooked_Present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
	if (csgo->DoUnload)
		return H::SteamPresent(device, src, dest, windowOverride, dirtyRegion);

	DWORD colorwrite, srgbwrite;
	IDirect3DVertexDeclaration9* vert_dec = nullptr;
	IDirect3DVertexShader9* vert_shader = nullptr;
	DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
	device->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
	device->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

	device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	//removes the source engine color correction
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	device->GetVertexDeclaration(&vert_dec);
	device->GetVertexShader(&vert_shader);
	device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

	
	csgo->render_device = device;
	GUI_Init(device);
	if (!onnn)
	{
		csgo->lua_fonts.insert({ crypt_str("mainfont"), fonts::menu_desc });
		csgo->lua_fonts.insert({ crypt_str("cheatfont1"), fonts::esp_name });
		csgo->lua_fonts.insert({ crypt_str("cheatfont2"), fonts::logger });
		onnn = true;
	}
	g_Render->SetupPresent(device);
	static void* dwReturnAddress = _ReturnAddress();

	if (dwReturnAddress == _ReturnAddress() && GetForegroundWindow() == csgo->Init.Window)
	{
		g_Render->PreRender(device);

		MoveRecorder->DrawRoutes();
		features->Visuals->Draw();
		Movement::Get().CheckpointDraw();
		g_Hitmarker->Paint();

		auto flags_backup = g_Render->_drawList->Flags;
		g_Render->_drawList->Flags |= ImDrawListFlags_AntiAliasedFill | ImDrawListFlags_AntiAliasedLines;
		features->Visuals->DrawLocalVisuals();
		
		features->Eventlog->Draw();
		//DrawTaserRange();
		if (g_Menu->window)
			g_Menu->window->update_keystates();
		//g_Menu->update_binds();
		//g_Menu->draw_indicators();

		if (csgo->script_loaded)
			for (auto current : CLua::Get().hooks.get_hooks(crypt_str("on_paint")))
				current.func();

		g_Menu->renderNewMenu();

		
		features->Visuals->DrawKeybindsWindow();
		features->Visuals->DrawSpectatorWindow();
		features->Visuals->TeamDamage_DisplayInfo();
		MoveRecorder->infowindow();
		Movement::Get().CheckpointENDSCENE();
		
		
		
		g_Render->_drawList->Flags = flags_backup;

		g_Render->PostRender(device);
		g_Render->EndPresent(device);
	}
	device->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	device->SetVertexDeclaration(vert_dec);
	device->SetVertexShader(vert_shader);

	return H::SteamPresent(device, src, dest, windowOverride, dirtyRegion);//return present here

}

static HRESULT D3DAPI Hooked_Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{

	if (csgo->DoUnload)
		return H::SteamReset(device, params);

	g_Render->InvalidateObjects();
	//ImGui_ImplDX9_InvalidateDeviceObjects();

	auto hr = H::SteamReset(device, params);
	if (hr >= 0) {

		g_Render->CreateObjects(device);
		//	ImGui_ImplDX9_CreateDeviceObjects();

	}
	return hr;

}