#include "Features.h"

static int scr_x{ -1 }, scr_y{ -1 };

void __stdcall Hooked_PaintTraverse(unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	//static auto PaintTraverse = g_pPanelHook->GetOriginal< PaintTraverseFn >(41);

	if (csgo->DoUnload) {
		interfaces.v_panel->SetMouseInputEnabled(vguiPanel, 0);
		return g_pPanelHook->callOriginal<void, 41>(vguiPanel, forceRepaint, allowForce);
	}

	std::string panel_name = interfaces.v_panel->GetName(vguiPanel);

	static std::string HudZoom = crypt_str("HudZoom");
	static std::string FocusOverlayPanel = crypt_str("FocusOverlayPanel");

	static bool init = false;

	int ScreenSize2W = -1;
	int ScreenSize2H = -1;

	

	interfaces.engine->GetScreenSize(ScreenSize2W, ScreenSize2H);
	if ((ScreenSize2W != csgo->w
			|| ScreenSize2H != csgo->h))
	{
		init = true;
		//interfaces.engine->GetScreenSize(ScreenSize2W, ScreenSize2H);
		if (scr_x != ScreenSize2W || scr_y != ScreenSize2H && ScreenSize2H != 0 && ScreenSize2W != 0)
		{
			vars.menu.inLoad = true;
			vars.menu.inLoad2 = true;
			vars.menu.inLoad3 = true;
			vars.menu.inLoadteamdmg = true;
			vars.menu.inLoadTeleport = true;
		}
		
		csgo->w = ScreenSize2W;
		csgo->h = ScreenSize2H;
	}

	if (panel_name == HudZoom && vars.visuals.remove[3])
		return;

	g_pPanelHook->callOriginal<void, 41>(vguiPanel, forceRepaint, allowForce);

	if (panel_name == FocusOverlayPanel) {

		if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && csgo->local)
		{
			csgo->is_local_alive = csgo->local->isAlive();

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					csgo->viewMatrix[i][j] = interfaces.engine->WorldToScreenMatrix()[i][j];

			if (csgo->local->isAlive()) {
				CGrenadePrediction::Get().Paint();
			}
			int dx = csgo->w, dy = csgo->h;

			if (vars.visuals.remove[3])
			{
				if (csgo->local->isAlive() && csgo->weapon->isSniper() && csgo->local->IsScoped())
				{
					Drawing::DrawLine(dx / 2, 0, dx / 2, dy, color_t(0, 0, 0, 150));
					Drawing::DrawLine(0, dy / 2, dx, dy / 2, color_t(0, 0, 0, 150));
				}
			}
		}
		else
			csgo->is_local_alive = false;

		interfaces.v_panel->SetMouseInputEnabled(vguiPanel, vars.menu.open);
	}

	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && csgo->local)
	{
		static auto weapon_debug_spread_show = interfaces.cvars->FindVar(hs::weapon_debug_spread_show.s().c_str());
		
		weapon_debug_spread_show->m_fnChangeCallbacks.m_Size = 0;
		
		if (csgo->local->isAlive())
			weapon_debug_spread_show->SetValue(vars.visuals.force_crosshair && !csgo->local->IsScoped() ? 3 : 0);

	}
}
float __fastcall Hooked_GetScreenAspectRatio(void* pEcx, void* pEdx, int32_t iWidth, int32_t iHeight)
{
	//auto GetScreenAspectRatio = g_pEngineHook->GetOriginal< AspectRatioFn >(101);

	//g_pEngineHook->callOriginal<float, 101>(pEcx, iWidth, iHeight);

	if (vars.visuals.aspect_ratio == 0.f)
		return g_pEngineHook->callOriginal<float, 101>(iWidth, iHeight);
	else
		return vars.visuals.aspect_ratio / 100.f;
}


void __stdcall on_screen_size_changed(int old_width, int old_height) {

	g_pSurfaceHook->callOriginal<void, 116>(old_width, old_height);
	vars.menu.inLoad = true;
	vars.menu.inLoad2 = true;
	vars.menu.inLoad3 = true;
	vars.menu.inLoadteamdmg = true;
	vars.menu.inLoadTeleport = true;
}