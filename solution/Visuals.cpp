#include "Visuals.h"
#include "Autowall.hpp"
#include "Resolver.h"
#include "FakelagGraph.h"
#include "AntiAims.h"
#include "Ragebot.h"
#include "render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx9.h"
#include "GUI/gui.h"
#include "Menu.h"
#include "Autopeek.h"
#include "Dormant.h"
#include "EnginePrediction.h"
#include "images.h"
#include "interfaces.h"
#include <Mmsystem.h>  
#include <ShlObj.h>
#include <TlHelp32.h>

std::wstring ttl = L"";
DWORD procID = 0;

//check the process entry point snapshot for the exe file name
bool isSpotify(const PROCESSENTRY32W& entry) {
	return std::wstring(entry.szExeFile) == crypt_strw(L"Spotify.exe");
}

//get window enumeration
BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam) {
	const auto& pids = *reinterpret_cast<std::vector<DWORD>*>(lParam);

	DWORD winId;
	GetWindowThreadProcessId(hwnd, &winId);

	if (pids.empty())
		return FALSE;

	for (DWORD pid : pids) {
		if (winId == pid) {

			std::wstring title(GetWindowTextLength(hwnd) + 1, L'\0');
			GetWindowTextW(hwnd, &title[0], title.size()); //note: C++11 only
			if (wcsstr(title.c_str(), crypt_strw(L"GDI+ Window (Spotify.exe)")) || wcsstr(title.c_str(), crypt_strw(L"Default IME")) || title.empty() || wcsstr(title.c_str(), crypt_strw(L"MSCTFIME UI"))) {
				continue;
			}
			if (lstrlenW(title.c_str()) > 1) {
				procID = pid;
				ttl = title;
			}
		}
	}

	return TRUE;
}

bool spotifywindow()
{
	std::vector<DWORD> pids; //process ID's

	//get handle of our proc
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//get entry point
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(entry);

	//see if our snapshot is valid
	if (!Process32FirstW(snap, &entry)) {
		return false;
	}

	//check if the handle is to spotify or not
	do {
		if (isSpotify(entry)) {
			pids.emplace_back(entry.th32ProcessID);
		}
	} while (Process32NextW(snap, &entry));


	//get enumeration of processes
	return EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&pids));
}


struct weapkey
{
	ItemDefinitionIndex i;
	char c;
};


weapkey weapon_icons[] =
{
	{ WEAPON_DEAGLE, 'A' },
	{ WEAPON_ELITE, 'B' },
	{ WEAPON_FIVESEVEN, 'C' },
	{ WEAPON_GLOCK, 'D' },
	{ WEAPON_HKP2000, 'E' },
	{ WEAPON_P250, 'F' },
	{ WEAPON_USP_SILENCER, 'G' },
	{ WEAPON_TEC9, 'H' },
	{ WEAPON_CZ75A, 'I' },
	{ WEAPON_REVOLVER, 'J' },
	{ WEAPON_MAC10, 'K' },
	{ WEAPON_UMP45, 'L' },
	{ WEAPON_BIZON, 'M' },
	{ WEAPON_MP7, 'N' },
	{ WEAPON_MP5_SD, 'L' },
	{ WEAPON_MP9, 'O' },
	{ WEAPON_P90, 'P' },
	{ WEAPON_GALILAR, 'Q' },
	{ WEAPON_FAMAS, 'R' },
	{ WEAPON_M4A1, 'S' },
	{ WEAPON_M4A1_SILENCER, 'T' },
	{ WEAPON_AUG, 'U' },
	{ WEAPON_SG556, 'V' },
	{ WEAPON_AK47, 'W' },
	{ WEAPON_G3SG1, 'X' },
	{ WEAPON_SCAR20, 'Y' },
	{ WEAPON_AWP, 'Z' },
	{ WEAPON_SSG08, 'a' },
	{ WEAPON_XM1014, 'b' },
	{ WEAPON_SAWEDOFF, 'c' },
	{ WEAPON_MAG7, 'd' },
	{ WEAPON_NOVA, 'e' },
	{ WEAPON_NEGEV, 'f' },
	{ WEAPON_M249, 'g' },
	{ WEAPON_TASER, 'h' },
	{ WEAPON_KNIFE_T, 'i' },
	{ WEAPON_KNIFE, 'j' },
	{ WEAPON_KNIFE_FALCHION, '0' },
	{ WEAPON_KNIFE_BAYONET, '1' },
	{ WEAPON_KNIFE_FLIP, '2' },
	{ WEAPON_KNIFE_GUT, '3' },
	{ WEAPON_KNIFE_KARAMBIT, '4' },
	{ WEAPON_KNIFE_M9_BAYONET, '5' },
	{ WEAPON_KNIFE_TACTICAL, '6' },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE, '7' },
	{ WEAPON_KNIFE_BUTTERFLY, '8' },
	{ WEAPON_KNIFE_URSUS, 'j' },
	{ WEAPON_KNIFE_GYPSY_JACKKNIFE, 'j' },
	{ WEAPON_KNIFE_STILETTO, 'j' },
	{ WEAPON_KNIFE_WIDOWMAKER, 'j' },
	{ WEAPON_FLASHBANG, 'k' },
	{ WEAPON_HEGRENADE, 'l' },
	{ WEAPON_SMOKEGRENADE, 'm' },
	{ WEAPON_MOLOTOV, 'n' },
	{ WEAPON_DECOY, 'o' },
	{ WEAPON_INCGRENADE, 'p' },
	{ WEAPON_C4, 'q' },
};

void weap2icon(ItemDefinitionIndex o, char* twochars)
{
	for (int i = 0; i < 57; i++)
	{
		if (weapon_icons[i].i == o)
		{
			twochars[0] = weapon_icons[i].c;
			twochars[1] = '\0';
		}
			
	}

	twochars = "?\0";
}




bool GetBox(IBasePlayer* entity, int& x, int& y, int& w, int& h, Vector origin)
{
	auto client_class = entity->GetClientClass();
	if (client_class->m_ClassID == ClassId->CCSPlayer)
	{
		const auto& min = entity->GetCollideable()->OBBMins();
		const auto& max = entity->GetCollideable()->OBBMaxs();

		Vector dir, vF, vR, vU;

		interfaces.engine->GetViewAngles(dir);
		dir.x = 0;
		dir.z = 0;
		//dir.Normalize();
		//printf("%.1f\n", dir.y);
		Math::AngleVectors(dir, &vF, &vR, &vU);

		auto zh = vU * max.z + vF * max.y + vR * min.x; // = Front left front
		auto e = vU * max.z + vF * max.y + vR * max.x; //  = Front right front
		auto d = vU * max.z + vF * min.y + vR * min.x; //  = Front left back
		auto c = vU * max.z + vF * min.y + vR * max.x; //  = Front right back

		auto g = vU * min.z + vF * max.y + vR * min.x; //  = Bottom left front
		auto f = vU * min.z + vF * max.y + vR * max.x; //  = Bottom right front
		auto a = vU * min.z + vF * min.y + vR * min.x; //  = Bottom left back
		auto b = vU * min.z + vF * min.y + vR * max.x; //  = Bottom right back*-

		Vector pointList[] = {
			a,
			b,
			c,
			d,
			e,
			f,
			g,
			zh,
		};

		Vector transformed[ARRAYSIZE(pointList)];

		for (int i = 0; i < ARRAYSIZE(pointList); i++)
		{
			pointList[i] += origin;

			if (!Math::WorldToScreen2(pointList[i], transformed[i]))
				return false;
		}

		float left = FLT_MAX;
		float top = -FLT_MAX;
		float right = -FLT_MAX;
		float bottom = FLT_MAX;
		for (int i = 0; i < ARRAYSIZE(pointList); i++) {
			if (left > transformed[i].x)
				left = transformed[i].x;
			if (top < transformed[i].y)
				top = transformed[i].y;
			if (right < transformed[i].x)
				right = transformed[i].x;
			if (bottom > transformed[i].y)
				bottom = transformed[i].y;
		}

		x = left;
		y = bottom;
		w = right - left;
		h = top - bottom;

		return true;
	}
	else
	{
		Vector vOrigin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
		//float left, top, right, bottom;

		auto collideable = entity->GetCollideable();

		if (!collideable)
			return false;

		min = collideable->OBBMins();
		max = collideable->OBBMaxs();

		auto& trans = entity->GetrgflCoordinateFrame();

		Vector points[] =
		{
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			Math::VectorTransform(points[i], trans, pointsTransformed[i]);
		}

		Vector pos = entity->GetRenderOrigin();

		if (!Math::WorldToScreen2(pointsTransformed[3], flb) || !Math::WorldToScreen2(pointsTransformed[5], brt)
			|| !Math::WorldToScreen2(pointsTransformed[0], blb) || !Math::WorldToScreen2(pointsTransformed[4], frt)
			|| !Math::WorldToScreen2(pointsTransformed[2], frb) || !Math::WorldToScreen2(pointsTransformed[1], brb)
			|| !Math::WorldToScreen2(pointsTransformed[6], blt) || !Math::WorldToScreen2(pointsTransformed[7], flt))
			return false;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
		//+1 for each cuz of borders at the original box
		float left = flb.x;        // left
		float top = flb.y;        // top
		float right = flb.x;    // right
		float bottom = flb.y;    // bottom

		for (int i = 1; i < 8; i++)
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (bottom < arr[i].y)
				bottom = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (top > arr[i].y)
				top = arr[i].y;
		}

		x = (int)left;
		y = (int)top;
		w = (int)(right - left);
		h = (int)(bottom - top);

		return true;
	}
	return false;
}

void CVisuals::DrawAngleLine(Vector origin, float angle, color_t color)
{
	Vector src, dst, sc1, sc2, forward;

	src = origin;
	Math::AngleVectors(Vector(0, angle, 0), &forward);
	if (Math::WorldToScreen(src, sc1) && Math::WorldToScreen(src + (forward * 40), sc2))
	{
		//Drawing::DrawLine(sc1.x, sc1.y, sc2.x, sc2.y, color);
		g_Render->DrawLine(sc1.x, sc1.y, sc2.x, sc2.y, color, 1.f);
	}
}

static void drawProjectileTrajectory(const Trail& config, const std::vector<std::pair<float, Vector>>& trajectory) noexcept
{
	std::vector<ImVec2> points, shadowPoints;

	auto color = config.color;

	for (const auto& [time, point] : trajectory) {
		if (Vector pos; time + config.time >= interfaces.global_vars->realtime && Math::WorldToScreen(point, pos)) {
			if (config.type == Trail::Line) {
				points.push_back(ImVec2(pos.x, pos.y));
				shadowPoints.push_back(ImVec2(pos.x + 1, pos.y + 1));
			}
			else if (config.type == Trail::Circles) {
				g_Render->_drawList->AddCircle(ImVec2(pos.x, pos.y), 3.5f - point.DistTo(csgo->origin) / 700.0f, color.u32(), 12, config.thickness);
			}
			else if (config.type == Trail::FilledCircles) {
				g_Render->_drawList->AddCircleFilled(ImVec2(pos.x, pos.y), 3.5f - point.DistTo(csgo->origin) / 700.0f, color.u32());
			}
		}
	}

	if (config.type == Trail::Line) {
		g_Render->_drawList->AddPolyline(shadowPoints.data(), shadowPoints.size(), color.u32() & IM_COL32_A_MASK, false, config.thickness);
		g_Render->_drawList->AddPolyline(points.data(), points.size(), color.u32(), false, config.thickness);
	}
}

void RenderEntityBox(const ProjectileInfo_t& projectileData, const char* name) {
	Vector origin2d;

	if (strstr(name, crypt_str("Smoke"))) {

		float time = projectileData.time_to_die;

		static auto world_circle = [](Vector location, float radius) {
			static constexpr float Step = PI * 2.0f / 60;
			std::vector<ImVec2> points;
			for (float lat = 0.f; lat <= PI * 2.0f; lat += Step)
			{
				const auto& point3d = Vector(sin(lat), cos(lat), 0.f) * radius;
				Vector point2d;
				if (Math::WorldToScreen(location + point3d, point2d))
					points.push_back(ImVec2(point2d.x, point2d.y));
			}
			auto flags_backup = g_Render->_drawList->Flags;
			g_Render->_drawList->Flags |= ImDrawListFlags_AntiAliasedFill;
			ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(points.data(), points.size(), color_t(0xA0EB8C0C).u32());
			//g_Render->_drawList->AddPolyline(points.data(), points.size(), color_t(255, 150, 0, 0xA0).u32(), true, 2.f);
			g_Render->_drawList->Flags = flags_backup;
		};

		if (projectileData.m_bDidSmokeEffect && time < 18.5f && time > 0.f)
			world_circle(projectileData.origin, 120.f);

		constexpr int radius = 26;
		//g_Render->CircleFilled(origin2d.x, origin2d.y, radius, color_t(40, 40, 40, 255), 50);
		//g_Render->CircleFilled(origin2d.x, origin2d.y, radius - 6, color_t(25, 25, 25, 255), 50);

		//g_Render->_drawList->PathArcTo(ImVec2(origin2d.x, origin2d.y), radius - 3, 0.f, 2 * PI * (projectileData.time_to_die / 18.5f), 32);
		//g_Render->_drawList->PathStroke(color_t(0xFFEB8C0C).u32(), false, 4.f);

		
		if (Math::WorldToScreen(projectileData.origin, origin2d))
		{
			if (time > 0.f && time < 18.5f) {

				int step = 0;
				
				if (vars.visuals.world.projectiles.indtype[1])
				{
					char pp[2];
					weap2icon(WEAPON_SMOKEGRENADE, pp);

					g_Render->DrawString(origin2d.x - 1, origin2d.y + 1, color_t(5, 5, 5, 255),
						render::centered_x | render::centered_y, fonts::csgoicons_large, pp);

					g_Render->DrawString(origin2d.x, origin2d.y, color_t(255, 255, 255, 255),
						render::centered_x | render::centered_y, fonts::csgoicons_large, pp);

					ImGui::PushFont(fonts::csgoicons_large);
					step += ImGui::CalcTextSize(pp).y;
					ImGui::PopFont();

				}

				if (vars.visuals.world.projectiles.indtype[0])
				{
					g_Render->DrawString(origin2d.x - 1, origin2d.y + 1 + step, color_t(5, 5, 5, 255),
						render::centered_x | render::centered_y, fonts::esp_logs, crypt_str("%.1fs"), time);

					g_Render->DrawString(origin2d.x, origin2d.y + step, color_t(255, 255, 255, 255),
						render::centered_x | render::centered_y, fonts::esp_logs, crypt_str("%.1fs"), time);
				}
					
				
				
			}
			else
			{
				int step = 0;

				if (vars.visuals.world.projectiles.indtype[1])
				{
					char pp[2];
					weap2icon(WEAPON_SMOKEGRENADE, pp);

					g_Render->DrawString(origin2d.x - 1, origin2d.y + 1, color_t(5, 5, 5, 255),
						render::none | render::centered_x | render::centered_y, fonts::csgoicons, pp);

					g_Render->DrawString(origin2d.x, origin2d.y, color_t{ 210,210,210,255 },
						render::none | render::centered_x | render::centered_y, fonts::csgoicons, pp);

					ImGui::PushFont(fonts::csgoicons);
					step += ImGui::CalcTextSize(pp).y;
					ImGui::PopFont();
				}


				if (vars.visuals.world.projectiles.indtype[0])
				{

					g_Render->DrawString(origin2d.x - 1, origin2d.y + 1 + step, color_t(5, 5, 5, 255),
						render::none | render::centered_x | render::centered_y, fonts::esp_logs, name);

					g_Render->DrawString(origin2d.x, origin2d.y + step, color_t{ 210,210,210,255 },
						render::none | render::centered_x | render::centered_y, fonts::esp_logs, name);
				}
			}
		}
		

	}
	else if (Math::WorldToScreen(projectileData.origin, origin2d)) {

		color_t nxtcolor = { 255,255,255,255 };
		char pp[2];
		
		
		if (projectileData.classID == ClassId->CBaseCSGrenadeProjectile)
		{
			if (strstr(name, crypt_str("Flashbang")))
			{
				nxtcolor = { 236, 255, 94, 255 };
				weap2icon(WEAPON_FLASHBANG, pp);
			}
			else
			{
				nxtcolor = { 255, 0, 0, 255 };
				weap2icon(WEAPON_HEGRENADE, pp);
			}
			
		}
		else if (projectileData.classID == ClassId->CMolotovProjectile)
		{
			nxtcolor = { 255, 0, 0, 255 };
			weap2icon(WEAPON_MOLOTOV, pp);
		}
		else if (projectileData.classID == ClassId->CDecoyProjectile)
		{
			nxtcolor = { 220, 220, 220, 255 };
			weap2icon(WEAPON_DECOY, pp);
		}
		

		int step = 0;

		if (vars.visuals.world.weapons.indtype[1])
		{

			g_Render->DrawString(origin2d.x - 1, origin2d.y + 1, color_t(5, 5, 5, 255),
				render::none | render::centered_x | render::centered_y, fonts::csgoicons, pp);

			g_Render->DrawString(origin2d.x, origin2d.y, nxtcolor,
				render::none | render::centered_x | render::centered_y, fonts::csgoicons, pp);
			ImGui::PushFont(fonts::csgoicons);
			step += ImGui::CalcTextSize(pp).y;
			ImGui::PopFont();
			
		}
		
		if (vars.visuals.world.weapons.indtype[0])
		{
			g_Render->DrawString(origin2d.x - 1, origin2d.y + 1 + step, color_t(5, 5, 5, 255),
				render::none | render::centered_x | render::centered_y, fonts::esp_logs, name);

			g_Render->DrawString(origin2d.x, origin2d.y + step, nxtcolor,
				render::none | render::centered_x | render::centered_y, fonts::esp_logs, name);
		}

		
		
	}
}

void renderProjectileEsp(const ProjectileInfo_t& projectileData, const char* name)
{
	color_t clr;

	if (projectileData.thrownByLocalPlayer)
		clr = vars.visuals.world.projectiles.colors[2];
	else if (projectileData.thrownByEnemy)
		clr = vars.visuals.world.projectiles.colors[1];
	else
		clr = vars.visuals.world.projectiles.colors[0];

	auto& config = Trail{};
	config.type = Trail::Type::Line;
	config.time = 1.5f;
	config.color = clr;

	// team enemy local
	if  ((projectileData.thrownByLocalPlayer && vars.visuals.world.projectiles.filter[2]) || (!projectileData.thrownByLocalPlayer && !projectileData.thrownByEnemy && vars.visuals.world.projectiles.filter[0]) || (projectileData.thrownByEnemy && vars.visuals.world.projectiles.filter[1])) {
		if(interfaces.ent_list->GetClientEntityFromHandle(projectileData.handle))
			RenderEntityBox(projectileData, name);
		//if (vars.visuals.world.projectiles.trajectories)
			//drawProjectileTrajectory(config, projectileData.trajectory);
	}
}


float lasttime = 0.f;
int realtickcount = 0;
float realrealtime = 0.f;

void CVisuals::DrawWatermark() {
	

	if (!csgo->local || !csgo->local->isAlive() || !interfaces.engine->IsConnected())
	{
		csgo->ebamount = 0;
	}
	std::string output = "";
	static std::string spotifytxt;
	
	if (vars.visuals.watermark)
	{

#ifdef _DEBUG
		output += crypt_str("Reflect.codes | [DEBUG]");
#else
		output += crypt_str("Reflect.codes | ");
		output += csgo->jewName;
#endif
		SYSTEMTIME tm;
		GetLocalTime(&tm);
		
		output += crypt_str(" | ") + (tm.wHour < 10 ? crypt_str("0") + std::to_string(tm.wHour) : std::to_string(tm.wHour)) + crypt_str(":") + (tm.wMinute < 10 ? crypt_str("0") + std::to_string(tm.wMinute) : std::to_string(tm.wMinute)) + crypt_str(":") + (tm.wSecond < 10 ? crypt_str("0") + std::to_string(tm.wSecond) : std::to_string(tm.wSecond));


		if (csgo->is_connected)
			output += crypt_str(" | ") + std::to_string((int)csgo->tick_rate) + crypt_str("t") + crypt_str(" | ") + std::to_string(int(csgo->ping)) + crypt_str("ms");

		if (vars.visuals.spotifysong)
			output += crypt_str("\n");


	}

	if (vars.visuals.spotifysong) {
		
		if (interfaces.global_vars->realtime > lasttime + 5.f) {


			if (spotifywindow()) {
				
				spotifytxt = wcsstr(ttl.c_str(), crypt_strw(L"Spotify")) ? crypt_str("Paused") : std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(ttl);
			}
			else
			{
				spotifytxt = crypt_str("Spotify not running!");
			}
			lasttime = interfaces.global_vars->realtime;
		}

		output += spotifytxt;
	}

	

	
	




	/*
	else
		output += crypt_str(" | ") + csgo->sub_time; //add time that subscription has left here
		*/

	/*
	const auto weapon = csgo->weapon;

	if (weapon) {
		float inac = weapon->GetInaccuracy();
		float spre = weapon->GetSpread();

		output = "ACC: " + std::to_string(inac) + " | SPRE: " + std::to_string(spre);
	}
	*/
		
	if (vars.visuals.spotifysong || vars.visuals.watermark)
	{
		ImGui::PushFont(fonts::logger);
		auto text_size = ImGui::CalcTextSize(output.c_str());
		ImGui::PopFont();
		static const ImVec2& padding = ImVec2(20, 20);
		static const ImVec2& margin = ImVec2(10, 6);
		static const int& box_offset = 10;

		const std::vector<ImVec2>& points = {
			ImVec2(csgo->w - text_size.x - padding.x - margin.x * 2.f, padding.y),
			ImVec2(csgo->w - padding.x + box_offset, padding.y),
			ImVec2(csgo->w - padding.x, text_size.y + padding.y + margin.y * 2.f),
			ImVec2(csgo->w - text_size.x - padding.x - margin.x * 2.f - box_offset, text_size.y + padding.y + margin.y * 2.f),
		};

		if (vars.visuals.spotifysong && vars.visuals.watermark)
		{
			ImGui::GetBackgroundDrawList()->AddRectFilled({ csgo->w - text_size.x - padding.x - margin.x * 2.f - 25.f ,  padding.y }, { csgo->w - padding.x, text_size.y + padding.y + margin.y * 2.f }, color_t(25, 25, 25, 200).u32(), 4.f);
			ImGui::GetBackgroundDrawList()->AddRect({ csgo->w - text_size.x - padding.x - margin.x * 2.f - 25.f ,  padding.y }, { csgo->w - padding.x, text_size.y + padding.y + margin.y * 2.f }, color_t(50, 50, 50, 255).u32(), 4.f);
			ImGui::GetBackgroundDrawList()->AddImage(LoadedImages::xtclogo.texture, { csgo->w - text_size.x - padding.x - margin.x - 38.f, padding.y + margin.y - 9.f }, { csgo->w - text_size.x - padding.x - margin.x + 4.f, padding.y + margin.y + 37.f });
		}
		else
		{
			ImGui::GetBackgroundDrawList()->AddRectFilled({ csgo->w - text_size.x - padding.x - margin.x * 2.f - 19.f ,  padding.y }, { csgo->w - padding.x, text_size.y + padding.y + margin.y * 2.f }, color_t(25, 25, 25, 200).u32(), 4.f);
			ImGui::GetBackgroundDrawList()->AddRect({ csgo->w - text_size.x - padding.x - margin.x * 2.f - 19.f ,  padding.y }, { csgo->w - padding.x, text_size.y + padding.y + margin.y * 2.f }, color_t(50, 50, 50, 255).u32(), 4.f);
			ImGui::GetBackgroundDrawList()->AddImage(LoadedImages::xtclogo.texture, { csgo->w - text_size.x - padding.x - margin.x - 29.f, padding.y + margin.y - 9.f }, { csgo->w - text_size.x - padding.x - margin.x + 4.f, padding.y + margin.y + 24.f });
		}

		g_Render->DrawString(csgo->w - text_size.x - padding.x - margin.x, padding.y + margin.y, color_t(200, 200, 200, 255),
			render::none, fonts::logger, output.c_str());
	}
	
}

static deque<std::pair<float, bool>> velocity_data;
static deque<std::pair<float, bool>> stamina_data;
static float last_log = 0.f;
int takeofftime{ 0 };
static int takeoffspeed = 0;
static float takeoffstamina = 0.f;
static int oldflags = 0;
static Vector oldvel;
static Vector curvel;
static float curstamina;

void CVisuals::DrawLocalVisuals()
{
	
	DrawWatermark();

	if (!interfaces.engine->IsInGame() || !interfaces.engine->IsConnected() || !interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer()))
	{
		velocity_data.clear();
		stamina_data.clear();
		last_log = 0.f;
		return;
	}

	if (!csgo->is_connected)
		return;

	CAutopeek::Get().Draw();

	int current_pos = csgo->h / 2;
	while (csgo->custom_hitlog.size() > 6)
		csgo->custom_hitlog.erase(csgo->custom_hitlog.begin());


	if (vars.visuals.hitlog) {
		Vector2D size = Vector2D(390, 220);
		g_Render->FilledRect(25, current_pos, size.x, size.y, color_t(0, 0, 0, 127));
		
		int iterations = 7;
		g_Render->DrawString(45, current_pos + (size.y / iterations) / 2.f, 
			color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("N."));

		g_Render->DrawString(152.5, current_pos + (size.y / iterations) / 2.f,
			color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("Player name"));

		for (int i = 1; i < iterations; i++) {
			int y_pos = current_pos + i * (size.y / iterations);
			g_Render->DrawLine(25, y_pos, 25 + size.x, y_pos, color_t(150, 150, 150, 255), 2.f);
			g_Render->DrawString(45, y_pos + (size.y / iterations) / 2.f, 
				color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("#%i"), i);
		}
		g_Render->DrawLine(65, current_pos, 65, current_pos + size.y, color_t(150, 150, 150, 255), 2.f);
		g_Render->DrawLine(240, current_pos, 240, current_pos + size.y, color_t(150, 150, 150, 255), 2.f);

		for (int i = 1; i <= 5; i++) {
			std::string name;
			switch (i)
			{
			case 1: name = crypt_str("H"); break;
			case 2: name = crypt_str("R"); break;
			case 3: name = crypt_str("BT"); break;
			case 4: name = crypt_str("SP"); break;
			case 5: name = crypt_str("Sp."); break;
			}
			g_Render->DrawString(240 + (i - 1) * 35 + 35 / 2.f, current_pos + (size.y / iterations) / 2.f, color_t(200, 200, 200, 255),
				render::centered_x | render::centered_y, fonts::menu_desc, name.c_str());
			if (i < 5)
				g_Render->DrawLine(240 + i * 35, current_pos, 240 + i * 35, current_pos + size.y, color_t(150, 150, 150, 255), 2.f);
		}

		if (!csgo->custom_hitlog.empty()) {
			for (int i = csgo->custom_hitlog.size() - 1; i >= 0; i--) {
				const auto& s = csgo->custom_hitlog[i];
				
				int y_pos = current_pos + (csgo->custom_hitlog.size() - i) * (size.y / iterations);
				g_Render->DrawString(152.5, y_pos + (size.y / iterations) / 2.f,
					color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("%s"), s.name.c_str());
				g_Render->DrawString(240 + 1 * 35 - 35 / 2.f, y_pos + (size.y / iterations) / 2.f,
					color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("%i"), s.hitbox);
				g_Render->DrawString(240 + 2 * 35 - 35 / 2.f, y_pos + (size.y / iterations) / 2.f,
					color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("%s"), s.resolver.c_str());
				g_Render->DrawString(240 + 3 * 35 - 35 / 2.f, y_pos + (size.y / iterations) / 2.f,
					color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("%i"), s.backtrack);
				g_Render->DrawString(240 + 4 * 35 - 35 / 2.f, y_pos + (size.y / iterations) / 2.f,
					color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("%d"), s.safepoint);
				g_Render->DrawString(240 + 5 * 35 - 35 / 2.f, y_pos + (size.y / iterations) / 2.f,
					color_t(200, 200, 200, 255), render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("%d"), s.spread);
			}
		}
		current_pos += 250;
	}

	

	if (csgo->is_local_alive && vars.antiaim.enable && vars.visuals.antiaim_arrows && vars.antiaim.manual_antiaim) {
		if (vars.antiaim.manual_back.state) {
			constexpr auto back_angle = PI / 2.f;
			g_Render->_drawList->PathArcTo(ImVec2(csgo->w / 2, csgo->h / 2), 40, back_angle - PI / 8.f, back_angle + PI / 8.f);
			g_Render->_drawList->PathStroke(vars.visuals.antiaim_arrows_color.u32(), false, 6.f);
		}
		if (vars.antiaim.manual_right.state) {
			constexpr auto back_angle = 0.f;
			g_Render->_drawList->PathArcTo(ImVec2(csgo->w / 2, csgo->h / 2), 40, back_angle - PI / 8.f, back_angle + PI / 8.f);
			g_Render->_drawList->PathStroke(vars.visuals.antiaim_arrows_color.u32(), false, 6.f);
		}
		if (vars.antiaim.manual_left.state) {
			constexpr auto back_angle = PI;
			g_Render->_drawList->PathArcTo(ImVec2(csgo->w / 2, csgo->h / 2), 40, back_angle - PI / 8.f, back_angle + PI / 8.f);
			g_Render->_drawList->PathStroke(vars.visuals.antiaim_arrows_color.u32(), false, 6.f);
		}
		if (vars.antiaim.manual_forward.state) {
			constexpr auto back_angle = PI * 1.5f;
			g_Render->_drawList->PathArcTo(ImVec2(csgo->w / 2, csgo->h / 2), 40, back_angle - PI / 8.f, back_angle + PI / 8.f);
			g_Render->_drawList->PathStroke(vars.visuals.antiaim_arrows_color.u32(), false, 6.f);
		}
	}


	if (vars.movement.showstamina)
	{
		if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && csgo->local)
		{
			color_t color{ vars.movement.staminacol };





			if (vars.movement.staminafade)
			{
				float p = (int)*color.get_ptr_a();
				p *= std::clamp(curstamina / vars.movement.staminafadevel, 0.f, 1.f);
				*color.get_ptr_a() = (int)p;
			}

			std::string print = to_string(curstamina);
			print.resize(print.find('.') + 2);
			ImGui::PushFont(fonts::move_indicator);
			auto txt_szie = ImGui::CalcTextSize(print.c_str());
			ImGui::PopFont();

			std::string tkprint = to_string(takeoffstamina);
			tkprint.resize(tkprint.find('.') + 2);

			if (vars.movement.staminatakeoff && (takeofftime > realtickcount))
			{
				//std::string spd = std::to_string(int(round(csgo->local->GetVelocity().Length2D()))) + " (" + std::to_string(takeoffspeed) + ")";Interesting esex:) 
				std::string firstline = crypt_str("(") + tkprint + crypt_str(")");
				std::string secondline = crypt_str(" (") + tkprint + crypt_str(")");
				std::string thirdline = crypt_str("(") + tkprint + crypt_str(") ");
				switch (vars.movement.showstaminatype)
				{
				case 0:



					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h * vars.movement.staminascrpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.staminascrpos) + 10.f, color, render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h * vars.movement.staminascrpos) + 11.f + txt_szie.y, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator_small, firstline.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.staminascrpos) + 10.f + txt_szie.y, color, render::centered_x, fonts::move_indicator_small, firstline.c_str());



					break;
				case 1:


					g_Render->DrawString(csgo->w / 2 + 1, (csgo->h * vars.movement.staminascrpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, std::string(print + secondline).c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.staminascrpos) + 10.f, color, render::centered_x, fonts::move_indicator, std::string(print + secondline).c_str());

					break;
				case 2:

					g_Render->DrawString(csgo->w / 2 + 1, (csgo->h * vars.movement.staminascrpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, std::string(thirdline + print).c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.staminascrpos) + 10.f, color, render::centered_x, fonts::move_indicator, std::string(thirdline + print).c_str());


					break;
				case 3:

					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h * vars.movement.staminascrpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.staminascrpos) + 10.f, color, render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h * vars.movement.staminascrpos) + 11.f - txt_szie.y, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator_small, firstline.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.staminascrpos) + 10.f - txt_szie.y, color, render::centered_x, fonts::move_indicator_small, firstline.c_str());

					break;
				};





			}
			else
			{
				//std::string print = to_string(int(round(curvel.Length2D()))); 
				g_Render->DrawString((csgo->w / 2) + 1, (csgo->h * vars.movement.staminascrpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, print.c_str());
				g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.staminascrpos) + 10.f, color, render::centered_x, fonts::move_indicator, print.c_str());
			}
		}
	}

	if (vars.movement.showvelocity)
	{
		if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && csgo->local)
		{
			
			
			
			
			static color_t color{ 255,255,255,255 };
			static float color_time{ 0.f };


			
			
			if (vars.movement.velocityfade)
			{
				*color.get_ptr_a() *= std::clamp(curvel.Length2D() / vars.movement.velfadevelocity, 0.f, 1.f);
			}

			std::string print = to_string(int(round(curvel.Length2D())));
			ImGui::PushFont(fonts::move_indicator);
			auto txt_szie = ImGui::CalcTextSize(print.c_str());
			ImGui::PopFont();

			if (vars.movement.showtakeoff && (takeofftime > realtickcount))
			{
				//std::string spd = std::to_string(int(round(csgo->local->GetVelocity().Length2D()))) + " (" + std::to_string(takeoffspeed) + ")";Interesting esex:) 
				std::string firstline = crypt_str("(") + to_string(takeoffspeed) + crypt_str(")");
				std::string secondline = crypt_str(" (") + to_string(takeoffspeed) + crypt_str(")");
				std::string thirdline = crypt_str("(") + to_string(takeoffspeed) + crypt_str(") ");
				switch(vars.movement.showveltype)
				{
				case 0:

					

					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h * vars.movement.showvelpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.showvelpos) + 10.f, color, render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h * vars.movement.showvelpos) + 11.f + txt_szie.y, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator_small, firstline.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h * vars.movement.showvelpos) + 10.f + txt_szie.y, color, render::centered_x, fonts::move_indicator_small, firstline.c_str());



					break;
				case 1:


					g_Render->DrawString(csgo->w / 2 + 1, (csgo->h* vars.movement.showvelpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, std::string(print + secondline).c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h* vars.movement.showvelpos) + 10.f, color, render::centered_x, fonts::move_indicator, std::string(print + secondline).c_str());

					break;
				case 2:

					g_Render->DrawString(csgo->w / 2 + 1, (csgo->h* vars.movement.showvelpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, std::string(thirdline + print).c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h* vars.movement.showvelpos) + 10.f, color, render::centered_x, fonts::move_indicator, std::string(thirdline + print).c_str());


					break;
				case 3:

					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h* vars.movement.showvelpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h* vars.movement.showvelpos) + 10.f, color, render::centered_x, fonts::move_indicator, print.c_str());
					g_Render->DrawString((csgo->w / 2) + 1, (csgo->h* vars.movement.showvelpos) + 11.f - txt_szie.y, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator_small, firstline.c_str());
					g_Render->DrawString(csgo->w / 2, (csgo->h* vars.movement.showvelpos) + 10.f - txt_szie.y, color, render::centered_x, fonts::move_indicator_small, firstline.c_str());

					break;
				};
				
				



			}
			else
			{
				//std::string print = to_string(int(round(curvel.Length2D()))); 
				g_Render->DrawString((csgo->w / 2) + 1, (csgo->h* vars.movement.showvelpos) + 11.f, color_t(0, 0, 0, color.get_alpha()), render::centered_x, fonts::move_indicator, print.c_str());
				g_Render->DrawString(csgo->w / 2, (csgo->h* vars.movement.showvelpos) + 10.f, color, render::centered_x, fonts::move_indicator, print.c_str());
			}

			if (vars.movement.staticvelcolor)
			{
				color = vars.movement.velcolor;
			}
			else
			{
				if (color_time <= realrealtime)
				{
					if (round(oldvel.Length2D()) > round(curvel.Length2D()))
					{
						color = { 255, 119, 119, 255 };
					}
					else if (round(oldvel.Length2D()) < round(curvel.Length2D()))
					{
						color = { 30, 255, 109, 255 };
					}
					else
					{
						color = { 255, 199, 89, 255 };
					}

					
					color_time = realrealtime + 0.1;
					oldvel = curvel;
				}
			}

			

			

			//some debug leftovers for yall
			/*
			static auto sv_gravity = interfaces.cvars->FindVar(crypt_str("sv_gravity"));
			if (sv_gravity)
			{
				int val = sv_gravity->GetInt();
				g_Render->DrawString(100, 100, color_t{ 255,255,255,255 }, 0, fonts::menu_desc, "%d", val);
			}
			*/
			//g_Render->DrawString(100, 200, color_t{ 255,255,255,255 }, 0, fonts::menu_desc, "%s", csgo->local->GetFlags() & 1 ? "onground" : "notonground");

		}
	}

	


		if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer()))
		{
			if (vars.movement.graph.enabled)
			{
				int x = csgo->w, y = csgo->h;


				if (vars.movement.graph.types[1])
				{
					if (stamina_data.size() == vars.movement.graph.size)
					{
						for (auto i = 0; i < stamina_data.size() - 2; i++)
						{

							auto& curdata = stamina_data[i];
							auto& nextdata = stamina_data[i + 1];

							auto cur = curdata.first;
							auto next = nextdata.first;
							auto landed = curdata.second && !nextdata.second;
							int alpha = 255;
							switch (vars.movement.graph.fadetype)
							{
							case 0:
								break;
							case 1:
								if (i < 10) // 0, 1, 2, 3 , 4, 5, 6, 7, 8, 9
								{
									float a = ((i + 1) / 10.f) * 255.f;
									alpha = (int)a;
								}
								else if (i > stamina_data.size() - 11) // 54 - 63
								{
									int todivide = stamina_data.size() - i;
									float a = (todivide / 10.f) * 255.f;
									alpha = (int)a;
								}
								break;
							case 2:
								alpha *= std::clamp(((cur + next) / 2) / vars.movement.graph.staminatofade, 0.f, 1.f);
								break;
							case 3:
								if (i < 10) // 0, 1, 2, 3 , 4, 5, 6, 7, 8, 9
								{
									float a = ((i + 1) / 10.f) * 255.f;
									alpha = (int)a;
								}
								else if (i > stamina_data.size() - 11) // 54 - 63
								{
									int todivide = stamina_data.size() - i;
									float a = (todivide / 10.f) * 255.f;
									alpha = (int)a;
								}

								alpha *= std::clamp(((cur + next) / 2) / vars.movement.graph.staminatofade, 0.f, 1.f);
								break;
							}

							alpha = std::clamp(alpha, 0, 255);

							color_t linecolor = color_t(vars.movement.graph.staminacol.get_red(), vars.movement.graph.staminacol.get_green(), vars.movement.graph.staminacol.get_blue(), (alpha / 255.f) * vars.movement.graph.staminacol.get_alpha());

							if (vars.movement.graph.staminagaincolors)
							{
								float cr = round(cur);
								float nx = round(next);
								if (nx > cr)
								{
									linecolor = color_t{ 255, 119, 119, int(round((alpha / 255.f) * (float)vars.movement.graph.staminacol.get_alpha())) };
								}
								else if (nx == cr)
								{
									linecolor = color_t{ 255, 199, 89, int(round((alpha / 255.f) * (float)vars.movement.graph.staminacol.get_alpha())) };
								}
								else if (nx < cr)
								{
									linecolor = color_t{ 30, 255, 109, int(round((alpha / 255.f) * (float)vars.movement.graph.staminacol.get_alpha())) };

								}
							}

							g_Render->DrawLine(((x / 2) + (((stamina_data.size() / 2) * vars.movement.graph.linewidth))) - ((i - 1) * vars.movement.graph.linewidth), csgo->h * vars.movement.graph.xscreenpos - 15 - ((clamp(cur, 0.f, 450.f) * 75 / 320) * vars.movement.graph.height), x / 2 + (((stamina_data.size() / 2) * vars.movement.graph.linewidth)) - i * vars.movement.graph.linewidth, csgo->h * vars.movement.graph.xscreenpos - 15 - ((clamp(next, 0.f, 450.f) * 75 / 320) * vars.movement.graph.height), linecolor, vars.movement.graph.thickness);


							if (landed)
								g_Render->DrawString(((x / 2) + (((stamina_data.size() / 2) * vars.movement.graph.linewidth))) - ((i + 1) * vars.movement.graph.linewidth), csgo->h * vars.movement.graph.xscreenpos - 30 - ((clamp(next, 0.f, 450.f) * 75 / 320) * vars.movement.graph.height), color_t(vars.movement.graph.staminatextcol.get_red(), vars.movement.graph.staminatextcol.get_green(), vars.movement.graph.staminatextcol.get_blue(), (alpha / 255.f) * vars.movement.graph.staminatextcol.get_alpha()), 0, fonts::graph_font, std::to_string((int)round(next)).c_str());
						}
					}
				}



				if (vars.movement.graph.types[0])
				{
					if (velocity_data.size() == vars.movement.graph.size)
					{
						for (auto i = 0; i < velocity_data.size() - 2; i++)
						{

							auto& curdata = velocity_data[i];
							auto& nextdata = velocity_data[i + 1];

							auto cur = curdata.first;
							auto next = nextdata.first;
							auto landed = curdata.second && !nextdata.second;
							int alpha = 255;
							switch (vars.movement.graph.fadetype)
							{
							case 0:
								break;
							case 1:
								if (i < 10) // 0, 1, 2, 3 , 4, 5, 6, 7, 8, 9
								{
									float a = ((i + 1) / 10.f) * 255.f;
									alpha = (int)a;
								}
								else if (i > velocity_data.size() - 11) // 54 - 63
								{
									int todivide = velocity_data.size() - i;
									float a = (todivide / 10.f) * 255.f;
									alpha = (int)a;
								}
								break;
							case 2:
								alpha *= std::clamp(((cur + next) / 2) / vars.movement.graph.speedtofade, 0.f, 1.f);
								break;
							case 3:
								if (i < 10) // 0, 1, 2, 3 , 4, 5, 6, 7, 8, 9
								{
									float a = ((i + 1) / 10.f) * 255.f;
									alpha = (int)a;
								}
								else if (i > velocity_data.size() - 11) // 54 - 63
								{
									int todivide = velocity_data.size() - i;
									float a = (todivide / 10.f) * 255.f;
									alpha = (int)a;
								}

								alpha *= std::clamp(((cur + next) / 2) / vars.movement.graph.speedtofade, 0.f, 1.f);
								break;
							}

							alpha = std::clamp(alpha, 0, 255);

							color_t linecolor = color_t(vars.movement.graph.color.get_red(), vars.movement.graph.color.get_green(), vars.movement.graph.color.get_blue(), (alpha / 255.f) * vars.movement.graph.color.get_alpha());

							if (vars.movement.graph.gaincolors)
							{
								float cr = round(cur);
								float nx = round(next);
								if (nx > cr)
								{
									linecolor = color_t{ 255, 119, 119, int(round((alpha / 255.f) * (float)vars.movement.graph.color.get_alpha())) };
								}
								else if (nx == cr)
								{
									linecolor = color_t{ 255, 199, 89, int(round((alpha / 255.f) * (float)vars.movement.graph.color.get_alpha())) };
								}
								else if (nx < cr)
								{
									linecolor = color_t{ 30, 255, 109, int(round((alpha / 255.f) * (float)vars.movement.graph.color.get_alpha())) };

								}
							}

							g_Render->DrawLine(((x / 2) + (((velocity_data.size() / 2) * vars.movement.graph.linewidth))) - ((i - 1) * vars.movement.graph.linewidth), csgo->h * vars.movement.graph.xscreenpos - 15 - ((clamp(cur, 0.f, 450.f) * 75 / 320) * vars.movement.graph.height), x / 2 + (((velocity_data.size() / 2) * vars.movement.graph.linewidth)) - i * vars.movement.graph.linewidth, csgo->h * vars.movement.graph.xscreenpos - 15 - ((clamp(next, 0.f, 450.f) * 75 / 320) * vars.movement.graph.height), linecolor, vars.movement.graph.thickness);


							if (landed)
								g_Render->DrawString(((x / 2) + (((velocity_data.size() / 2) * vars.movement.graph.linewidth))) - ((i + 1) * vars.movement.graph.linewidth), csgo->h * vars.movement.graph.xscreenpos - 30 - ((clamp(next, 0.f, 450.f) * 75 / 320) * vars.movement.graph.height), color_t(vars.movement.graph.textcolor.get_red(), vars.movement.graph.textcolor.get_green(), vars.movement.graph.textcolor.get_blue(), (alpha / 255.f) * vars.movement.graph.textcolor.get_alpha()), 0, fonts::graph_font, std::to_string((int)round(next)).c_str());
						}
					}
				}
				
			}
		}


	if (csgo->local && csgo->local->isAlive())
	{
		// here do debug drawing
		


		//if (csgo->client_state)
		//	g_Render->DrawString(100, 200, color_t(255, 255, 255, 255), 0, fonts::menu_desc, std::string("iChokedCommands: " + to_string(csgo->client_state->iChokedCommands) + " csgo->sendpacket: " + to_string(csgo->send_packet)).c_str());

		if (vars.movement.ebdetect.enabled[2])
			g_Render->DrawString(csgo->w / 2, csgo->h * 0.13f, color_t{ 255,255,255,255 }, render::outline | render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("EBs: %d"), csgo->ebamount);
	}
}

std::string str_toupper(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return toupper(c); }
	);
	return s;
}
void CVisuals::ResetInfo()
{
	int maxind = interfaces.ent_list->GetHighestEntityIndex();
	for (auto i = 0; i < maxind; i++)
	{
		IBasePlayer* entity = interfaces.ent_list->GetClientEntity(i);
		if (i < 64)
			player_info[i].Reset();
	
	}
}




void renderWeaponBox(const WeaponData_t& weaponData) noexcept
{
	//if (config.name.enabled && !weaponData.displayName.empty()) {
	//	renderText(weaponData.distanceToLocal, config.textCullDistance, config.name, weaponData.displayName.c_str(),
	//		{ (bbox.min.x + bbox.max.x) / 2, bbox.min.y - 5 });
	//}
	Vector screen;
	if (!Math::WorldToScreen(weaponData.origin, screen))
		return;

	int step = 0;

	if (vars.visuals.world.weapons.indtype[1])
	{
		char popo[2];
		weap2icon(weaponData.index, popo);
		g_Render->DrawString(screen.x, screen.y, vars.visuals.world.weapons.color,
			render::outline | render::centered_x | render::centered_y, fonts::csgoicons, popo);
		ImGui::PushFont(fonts::csgoicons);
		step += ImGui::CalcTextSize(popo).y;
		ImGui::PopFont();

	}

	if (vars.visuals.world.weapons.indtype[0])
	{
		g_Render->DrawString(screen.x, screen.y + step, vars.visuals.world.weapons.color,
			render::outline | render::centered_x | render::centered_y, fonts::esp_name, weaponData.name.c_str());
	}
	

	/*if (config.ammo.enabled && weaponData.clip != -1) {
		const auto text{ std::to_string(weaponData.clip) + " / " + std::to_string(weaponData.reserveAmmo) };
		renderText(weaponData.distanceToLocal, config.textCullDistance, config.ammo, text.c_str(), { (bbox.min.x + bbox.max.x) / 2, bbox.max.y + 5 }, true, false);
	}*/
}
void renderInferno(const InfernoInfo_t& info) {
	Vector origin2d;
	if (!Math::WorldToScreen(info.entity_origin, origin2d))
		return;

	static auto world_circle = [](Vector location, float radius){
		static constexpr float Step = PI * 2.0f / 60;
		std::vector<ImVec2> points;
		for (float lat = 0.f; lat <= PI * 2.0f; lat += Step)
		{
			const auto& point3d = Vector(sin(lat), cos(lat), 0.f) * radius;
			Vector point2d;
			if (Math::WorldToScreen(location + point3d, point2d))
				points.push_back(ImVec2(point2d.x, point2d.y));
		}
		auto flags_backup = g_Render->_drawList->Flags;
		g_Render->_drawList->Flags |= ImDrawListFlags_AntiAliasedFill;
		ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(points.data(), points.size(), color_t(255, 35, 35, 140).u32());
		//g_Render->_drawList->AddPolyline(points.data(), points.size(), color_t(255, 150, 0, 0xA0).u32(), true, 2.f);
		g_Render->_drawList->Flags = flags_backup;
	};


	world_circle(info.origin, info.range);

	//std::vector<ImVec2> points;
	//for (const auto& p : info.points)
	//{
	//	Vector point2d;
	//	if (Math::WorldToScreen(p, point2d)) {
	//		points.push_back(ImVec2(point2d.x, point2d.y));
	//	}
	//}
	//
	//auto flags_backup = g_Render->_drawList->Flags;
	//g_Render->_drawList->Flags |= ImDrawListFlags_AntiAliasedFill;
	//g_Render->_drawList->AddConvexPolyFilled(points.data(), points.size(), color_t(255, 35, 35, 140).u32());
	//g_Render->_drawList->AddPolyline(points.data(), points.size(), color_t(0, 0, 255, 255).u32(), true, 3.f);
	//g_Render->_drawList->Flags = flags_backup;
	//
	//int i = 0;
	//for (const auto& p : info.points)
	//{
	//	Vector point2d;
	//	if (Math::WorldToScreen(p, point2d)) {
	//		g_Render->DrawLine(point2d.x, point2d.y, origin2d.x, origin2d.y, color_t(0, 0, 255, 255).u32(), 3.f);
	//		g_Render->DrawString(point2d.x, point2d.y, color_t(255, 255, 255, 255).u32(), render::outline, fonts::menu_desc, "%i", ++i);
	//		//points.push_back(ImVec2(point2d.x, point2d.y));
	//	}
	//}

	constexpr int radius = 26;
	//g_Render->CircleFilled(origin2d.x, origin2d.y, radius, color_t(40, 40, 40, 255), 50);
	//g_Render->CircleFilled(origin2d.x, origin2d.y, radius - 6, color_t(25, 25, 25, 255), 50);

	//g_Render->_drawList->PathArcTo(ImVec2(origin2d.x, origin2d.y), radius - 3, 0.f, 2.f * PI * (info.time_to_die / 7.03125f), 32);
	//g_Render->_drawList->PathStroke(color_t(255, 150, 0, 255).u32(), false, 4.f);
	int step = 0;

	if (vars.visuals.world.projectiles.indtype[1])
	{
		char pp[2];
		weap2icon(WEAPON_MOLOTOV, pp);
		g_Render->DrawString(origin2d.x + 1, origin2d.y + 1, color_t(5, 5, 5, 255),
			render::none | render::centered_x | render::centered_y, fonts::csgoicons_large, pp);

		g_Render->DrawString(origin2d.x, origin2d.y, color_t(255,255,255,255),
			render::none | render::centered_x | render::centered_y, fonts::csgoicons_large, pp);
		ImGui::PushFont(fonts::csgoicons_large);
		step += ImGui::CalcTextSize(pp).y;
		ImGui::PopFont();

	}



	if (vars.visuals.world.projectiles.indtype[0])
	{
		g_Render->DrawString(origin2d.x + 1, origin2d.y + 1 + step, color_t(5, 5, 5, 255),
			render::centered_x | render::centered_y, fonts::esp_logs, crypt_str("%.1fs"), info.time_to_die + 0.5f);

		g_Render->DrawString(origin2d.x, origin2d.y + step, color_t(255, 255, 255, 255),
			render::centered_x | render::centered_y, fonts::esp_logs, crypt_str("%.1fs"), info.time_to_die + 0.5f);
		
	}
}
	

	

void PushFlag(int x, int y, bool flag_enabled, std::string flag_name, color_t flag_clr, int& offset_y) {
	if (!flag_enabled)
		return;
	color_t clrb = color_t(5, 5, 5, 255);
	g_Render->DrawString(x-1, y+1,
		clrb, render::none, fonts::esp_info, flag_name.c_str());

	g_Render->DrawString(x, y,
		flag_clr, render::none, fonts::esp_info, flag_name.c_str());

	offset_y += 10;
}

void renderBomb(const BombInfo_t& info) { //NEEDS CHANGING BAD! doesnt need anymore  ~emilia
	if (!csgo->local || !interfaces.engine->IsInGame() || !interfaces.engine->IsConnected())
		return;

	if (!vars.visuals.world.weapons.planted_bomb)
		return;

	constexpr int radius = 60;
	const float offset = 170.f;

	static float pulse = 0.f;
	static bool b_switch = false;

	if (b_switch) {
		if (pulse >= 0.f)
			pulse += animation_speed / 4.f;
		if (pulse >= 1.f)
			b_switch = false;
	}
	else {
		if (pulse <= 1.f)
			pulse -= animation_speed / 4.f;
		if (pulse <= 0.f)
			b_switch = true;
	}
	pulse = clamp(pulse, 0.f, 1.f);

	if (info.bomb_defused) {

		g_Render->FilledRect((csgo->w / 2) - (offset / 2), (csgo->h * vars.visuals.world.weapons.planted_bombheight) - (offset / 4), offset, offset / 2, color_t(0, 255, 0, 255), 5);
		g_Render->FilledRect(((csgo->w / 2) - (offset / 2)) + 3, (csgo->h * vars.visuals.world.weapons.planted_bombheight) - (offset / 4) + 3, offset - 6, (offset / 2) - 6, color_t(25, 25, 25, 255), 5);
		/*
		g_Render->CircleFilled(csgo->w / 2, csgo->h - offset, radius, color_t(40, 40, 40, 255), 50);
		g_Render->CircleFilled(csgo->w / 2, csgo->h - offset, radius - 6, color_t(25, 25, 25, 255), 50);

		g_Render->_drawList->PathArcTo(ImVec2(csgo->w / 2, csgo->h - offset), radius - 3, DEG2RAD(0.f), DEG2RAD(360.f), 32);
		g_Render->_drawList->PathStroke(color_t(0, 255, 0, 255).u32(), false, 4.f);
		*/

		g_Render->DrawString(csgo->w / 2, csgo->h * vars.visuals.world.weapons.planted_bombheight, color_t(255, 255, 255, 255),
			render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("Defused"));
	}
	else {
		if (info.is_defusing) {

			g_Render->FilledRect((csgo->w / 2) - (offset / 2), (csgo->h * vars.visuals.world.weapons.planted_bombheight) - (offset / 4), offset, offset / 2, color_t(84, 156, 255, 255), 5);
			g_Render->FilledRect(((csgo->w / 2) - (offset / 2)) + 3, (csgo->h * vars.visuals.world.weapons.planted_bombheight) - (offset / 4) + 3, offset - 6, (offset / 2) - 6, color_t(25, 25, 25, 255), 5);
			
			g_Render->DrawString(csgo->w / 2, csgo->h * vars.visuals.world.weapons.planted_bombheight, color_t(255, 255, 255, 255),
				render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("Defusing"));
		}
		else {
			if (info.bomb_ticking && info.blow_time > 0.f) {

				g_Render->FilledRect((csgo->w / 2) - (offset / 2), (csgo->h * vars.visuals.world.weapons.planted_bombheight) - (offset / 4) , offset, offset / 2, color_t(220, 0, 0, 255), 5);
				g_Render->FilledRect(((csgo->w / 2) - (offset / 2)) + 3, (csgo->h * vars.visuals.world.weapons.planted_bombheight) - (offset / 4) + 3, offset - 6, (offset / 2) - 6, color_t(25, 25, 25, 255), 5);


				/*
				g_Render->CircleFilled(csgo->w / 2, csgo->h - offset, radius, color_t(40, 40, 40, 255), 50);
				g_Render->CircleFilled(csgo->w / 2, csgo->h - offset, radius - 6, color_t(25, 25, 25, 255), 50);
				
				g_Render->_drawList->PathArcTo(ImVec2(csgo->w / 2, csgo->h - offset), radius - 3, 0.f, 2.f * PI * (info.blow_time / info.time), 32);
				g_Render->_drawList->PathStroke(color_t(255, 0, 0, 255).u32(), false, 4.f);
				*/

				g_Render->DrawString(csgo->w / 2, csgo->h * vars.visuals.world.weapons.planted_bombheight - 10, color_t(255, 255, 255, 255),
					render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("Explosion in:"));

				g_Render->DrawString(csgo->w / 2, csgo->h * vars.visuals.world.weapons.planted_bombheight + 10, color_t(255, 255, 255, 255),
					render::centered_x | render::centered_y, fonts::menu_desc, crypt_str("%.3f s"), info.blow_time);
			}
		}
	}

	Vector origin;
	if (!Math::WorldToScreen(info.origin, origin) || info.blow_time < 0.f)
		return;

	
	int step = 0;

	if (vars.visuals.world.weapons.indtype[1])
	{
		char pp[2];
		weap2icon(WEAPON_C4, pp);
		g_Render->DrawString(origin.x, origin.y, color_t(255,int(pulse * 255.f), int(pulse * 255.f), 255),
			render::outline | render::centered_x | render::centered_y, fonts::csgoicons, pp);
		ImGui::PushFont(fonts::csgoicons);
		step += ImGui::CalcTextSize(pp).y;
		ImGui::PopFont();
	}

	if (vars.visuals.world.weapons.indtype[0])
	{
		g_Render->DrawString(origin.x, origin.y + step, color_t(255, 0, 0, 255),
			render::outline | render::centered_x | render::centered_y, fonts::esp_logs, crypt_str("Planted Bomb %.3fs"), info.blow_time);

	}

	
}

void renderChicken()
{
	std::vector<IBasePlayer*> ownedchickens;

	int maxind = interfaces.ent_list->GetHighestEntityIndex();
	for (size_t i = 0; i <= maxind; i++)
	{
		auto ent = interfaces.ent_list->GetClientEntity(i);
		if (!ent)
			continue;

		if (ent->IsDormant())
			continue;

		if (ent->GetClientClass()->m_ClassID == ClassId->CChicken && vars.misc.chicken.enable) // chicken
		{

			auto leader = interfaces.ent_list->GetClientEntityFromHandle(ent->GetLeaderHandle());
			color_t col{};
			if (leader) // if owned by some1
			{

				if (leader == csgo->local)
				{
					ownedchickens.push_back(ent);
					col = { 20,20,255,255 };
				}
				else
				{
					if (!leader->isEnemy(csgo->local))
					{
						col = { 255,20,20,255 };
					}
					else
					{
						col = { 20,255,20,255 };
					}
				}
			}

			Vector out;
			if (!Math::WorldToScreen(ent->GetOrigin(), out)) // w2s
				continue;

			if (leader && vars.misc.chicken.owner)
			{
				g_Render->DrawString(out.x, out.y - 13, col, render::centered_x | render::centered_y, fonts::esp_name, leader->GetName().c_str()); // display the name of owner
			}
			g_Render->DrawString(out.x, out.y, color_t(255, 255, 255, 255), render::centered_x | render::centered_y, fonts::esp_name, crypt_str("Chicken")); // name
		}
		else if (ent->GetClientClass()->m_ClassID == ClassId->CFish && vars.misc.chicken.fish)
		{
			Vector out;
			if (!Math::WorldToScreen(ent->GetOrigin(), out)) // w2s
				continue;
			
				g_Render->DrawString(out.x, out.y, vars.misc.chicken.fishcolor, render::centered_x | render::centered_y, fonts::esp_name, crypt_str("Fish")); // name
		}
	}

	if (vars.misc.chicken.sidelist && vars.misc.chicken.enable) {
		for (size_t i = 0; i < ownedchickens.size(); i++)
		{
			IBasePlayer* chik = ownedchickens.at(i);
			std::string line = crypt_str("Chicken ") + to_string(chik->GetIndex()) + crypt_str("id | ") + to_string(int(csgo->local->GetOrigin().DistTo(chik->GetOrigin()))) + crypt_str(" units");
			ImVec2 txtsize = fonts::esp_name->CalcTextSizeA(12, FLT_MAX, 0.0f, line.c_str());
			g_Render->DrawString(csgo->w - txtsize.x, (csgo->h / 2) + (i * 13), color_t(255, 255, 255, 255), 0, fonts::esp_name, line.c_str());
		}
	}
}

void rendereb()
{
	
		static auto world_circle = [](Vector location, float radius) {
		static constexpr float Step = PI * 2.0f / 60;
		std::vector<ImVec2> points;
		for (float lat = 0.f; lat <= PI * 2.0f; lat += Step)
		{
			const auto& point3d = Vector(sin(lat), cos(lat), 0.f) * radius;
			Vector point2d;
			if (Math::WorldToScreen(location + point3d, point2d))
				points.push_back(ImVec2(point2d.x, point2d.y));
		}
		auto flags_backup = ImGui::GetBackgroundDrawList()->Flags;
		ImGui::GetBackgroundDrawList()->Flags |= ImDrawListFlags_AntiAliasedFill;
		ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(points.data(), points.size(), color_t(127, 30, 255, 140).u32());
		//g_Render->_drawList->AddPolyline(points.data(), points.size(), color_t(0, 0, 255, 255).u32(), true, 2.f);
		ImGui::GetBackgroundDrawList()->Flags = flags_backup;
		};

	world_circle(csgo->ebpos, 5.f);

}

struct ooftarget
{
	Vector origin;
	bool visible;
};

static std::vector<ooftarget> ooftargets;
static std::vector<std::pair<ImageData, int>> player_images; // ImageData and entindex
void CVisuals::Draw()
{
	if (!csgo->is_connected)
	{
		player_images.clear();
		return;
	}
		

	auto flags_backup = g_Render->_drawList->Flags;
	g_Render->_drawList->Flags |= ImDrawListFlags_AntiAliasedFill | ImDrawListFlags_AntiAliasedLines;
	if (vars.visuals.world.projectiles.enable) {
		for (const auto& projectile : ProjectileInfo) {
			if (!projectile.name.empty())
				renderProjectileEsp(projectile, projectile.name.c_str());
		}
	}

	if (vars.visuals.world.weapons.enabled) {
		for (const auto& weapon : WeaponData)
			renderWeaponBox(weapon);
	}

	for (const auto& c4_info : BombInfo)
		renderBomb(c4_info);

	for (const auto& info : InfernoInfo)
		renderInferno(info);
		
	g_Render->_drawList->Flags = flags_backup;

	static float oof_pulsating = 0.f;
	static bool b_switch = false;

	if (b_switch) {
		if (oof_pulsating >= 0.f)
			oof_pulsating += animation_speed / 4.f;
		if (oof_pulsating >= 1.f)
			b_switch = false;
	}
	else {
		if (oof_pulsating <= 1.f)
			oof_pulsating -= animation_speed / 4.f;
		if (oof_pulsating <= 0.f)
			b_switch = true;
	}	
	oof_pulsating = clamp(oof_pulsating, 0.f, 1.f);


	if (vars.visuals.enable)
	{

		float width = 10.f;
		Vector viewangles;
		interfaces.engine->GetViewAngles(viewangles);

		auto center = ImVec2(csgo->w / 2.f, csgo->h / 2.f);

		for (int i = 0; i < 64; ++i)
		{
			auto& info = player_info[i];
			if (!info.player || !info.is_valid)
				continue;



			if (vars.visuals.out_of_fov)
			{
				if (info.player && info.ClassID == ClassId->CCSPlayer && info.offscreen)
				{
					

						const auto& rot = DEG2RAD(viewangles.y - Math::CalculateAngle(csgo->origin, info.origin).y - 90.f);

						auto radius = 50 + vars.visuals.out_of_fov_distance;
						auto size = vars.visuals.out_of_fov_size;

						auto pos = ImVec2(center.x + radius * cosf(rot) * (2 * (0.5f + 10 * 0.5f * 0.01f)), center.y + radius * sinf(rot));
						auto line = pos - center;

						auto arrowBase = pos - (line * (size / (2 * (tanf(PI / 4) / 2) * Vector2D { line.x, line.y }.Length())));
						auto normal = ImVec2(-line.y, line.x);
						auto left = arrowBase + normal * (size / (2 * Vector2D{line.x, line.y}.Length()));
						auto right = arrowBase + normal * (-size / (2 * Vector2D{ line.x, line.y }.Length()));

						auto clr = info.visible ? vars.visuals.out_of_fov_visiblecolor : vars.visuals.out_of_fov_color;

						ImGui::GetBackgroundDrawList()->AddTriangleFilled(left, right, pos, clr.u32());
					
				}
			}
			



			int alpha = vars.visuals.dormant ? info.alpha : 255;
			alpha = std::clamp(alpha, 0, 255);

			if (alpha <= 0
				|| info.offscreen)
				continue;

			if (vars.visuals.zeus_warning) {
				auto flags_backup = g_Render->_drawList->Flags;
				g_Render->_drawList->Flags |= ImDrawListFlags_AntiAliasedFill | ImDrawListFlags_AntiAliasedLines;
				if (info.zeuser_stages != none) {
					color_t warning_clr = [&]() {
						switch (info.zeuser_stages) {
						case good:
							return color_t(0, 255, 0, 155 + (100 * oof_pulsating));
							break;
						case warning:
							return color_t(255, 208, 55, 155 + (100 * oof_pulsating));
							break;
						case fatal:
							return color_t(255, 0, 0, 155 + (100 * oof_pulsating));
							break;
						}
					}();

					int add = vars.visuals.name ? 45 : 35;

					constexpr int radius = 26;
					g_Render->CircleFilled(info.box.x + info.box.w / 2, info.box.y - add, radius, color_t(40, 40, 40, 155 + (100 * oof_pulsating)), 50);
					g_Render->CircleFilled(info.box.x + info.box.w / 2, info.box.y - add, radius - 6, color_t(25, 25, 25, 255), 50);

					g_Render->_drawList->PathArcTo(ImVec2(info.box.x + info.box.w / 2, info.box.y - add), radius - 3, DEG2RAD(0.f), DEG2RAD(360.f), 32);
					g_Render->_drawList->PathStroke(warning_clr.u32(), false, 4.f);

					g_Render->DrawString(info.box.x + info.box.w / 2, info.box.y - add, color_t(
						255, 
						info.zeuser_stages == fatal ? 0 : 255, 
						info.zeuser_stages == fatal ? 0 : 255,
						255 * oof_pulsating),
						render::outline | render::centered_x | render::centered_y,
						fonts::lby_indicator, crypt_str("!"));
				}
				g_Render->_drawList->Flags = flags_backup;
			}

			if (vars.visuals.box)
			{
				color_t& pclr = info.visible ? vars.visuals.box_color : vars.visuals.box_color_occluded;


				int box_alpha = info.dormant && vars.visuals.dormant ? alpha : pclr[3];
				auto clr = info.dormant ? color_t(255, 255, 255, box_alpha * 0.7f) : color_t(
					pclr[0],
					pclr[1],
					pclr[2],
					box_alpha * 0.8f);

				g_Render->Rect(info.box.x, info.box.y, info.box.w, info.box.h, clr);
				//g_Render->Rect(info.box.x + 1, info.box.y + 1, info.box.w - 1, info.box.h - 1, clr);

				g_Render->Rect(info.box.x, info.box.y, info.box.w, info.box.h, clr);
				g_Render->Rect(info.box.x - 1, info.box.y - 1, info.box.w + 2, info.box.h + 2, color_t(0, 0, 0, (box_alpha * 0.5f)));
				g_Render->Rect(info.box.x + 1, info.box.y + 1, info.box.w - 2, info.box.h - 2, color_t(0, 0, 0, (box_alpha * 0.5f)));
			}

			int count = 0;
			if (vars.visuals.name)
			{
				ImGui::PushFont(fonts::esp_name);
				auto text_size = ImGui::CalcTextSize(info.name.c_str());

				color_t& pclr = info.visible ? vars.visuals.name_color : vars.visuals.name_color_occluded;

				int r = pclr[0],
					g = pclr[1],
					b = pclr[2],
					a = pclr[3] * (alpha / 255.f);

				auto clr = info.dormant ? color_t(255, 255, 255, alpha) : color_t(r, g, b, a);
				color_t clrb = color_t(5, 5, 5, 255);

				//shawdow
				g_Render->DrawString((info.box.x + info.box.w / 2 - text_size.x / 2 ) - 1, info.box.y - 15 + 1, clrb,
					render::none, fonts::esp_name, info.name.c_str());
				//real
				g_Render->DrawString(info.box.x + info.box.w / 2 - text_size.x / 2, info.box.y - 15, clr,
					render::none, fonts::esp_name, info.name.c_str());
				ImGui::PopFont();
			}

			if (!info.dormant) {
				auto clr = info.visible ? vars.visuals.skeleton_color : vars.visuals.skeleton_color_occluded;

				if (vars.visuals.shot_multipoint && csgo->is_local_alive) {

					//if (info.AnimInfo.points.size() > 0) {
					//	Vector p1 = info.AnimInfo.points[0];
					//	Vector p2 = info.AnimInfo.points[1];

					//	int add = 5;

					//	Vector w1, w2;
					//	if (Math::WorldToScreen(p1, w1)
					//		&& Math::WorldToScreen(p2, w2)) {
					//		g_Render->FilledRect(w1.x - 4 + add, w1.y - 4 + add, 6 + add, 6 + add, color_t(0, 0, 0, 255));
					//		g_Render->FilledRect(w1.x - 3 + add, w1.y - 3 + add, 4 + add, 4 + add, color_t(255, 0, 0, 255));

					//		g_Render->FilledRect(w2.x - 4 + add, w2.y - 4, 6 + add, 6 +add, color_t(0, 0, 0, 255));
					//		g_Render->FilledRect(w2.x - 3 + add, w2.y - 3, 4 + add, 4 +add, color_t(0, 255, 0, 255));
					//	}
					//}

					for (const auto& p : info.AnimInfo.points) {
						Vector world;
						if (Math::WorldToScreen(p, world)) {
							g_Render->CircleFilled(world.x, world.y, 3.f, color_t(0, 0, 0, alpha), 100);
							g_Render->CircleFilled(world.x, world.y, 2.f, color_t(255, 255, 255, alpha), 100);
						}
					}
				}

				if (info.hdr && vars.visuals.skeleton) {
					for (int i = 0; i < 128; i++) {
						Vector sParent{}, sChild{};

						if (Math::WorldToScreen(info.bone_pos_parent[i], sParent) && Math::WorldToScreen(info.bone_pos_child[i], sChild))
							g_Render->DrawLine(sParent[0], sParent[1], sChild[0], sChild[1], clr, 1.f);
					}
				}
			}

			if (vars.visuals.healthbar)
			{
				int hp = info.hp;

				if (hp > 100)
					hp = 100;

				int hp_percent = info.box.h - (int)((info.box.h * hp) / 100);

				int width = (info.box.w * (hp / 100.f));

				int red = 255 - (hp * 2.55);
				int green = hp * 2.55;

				auto clr = info.dormant ? color_t(200, 200, 200, alpha) : color_t(255, 255, 255, alpha);
				color_t clrb = color_t(5, 5, 5, 255);

				color_t& pclr = info.visible ? vars.visuals.hp_color : vars.visuals.hp_color_occluded;

				auto hp_color = info.dormant ? color_t(200, 200, 200, alpha) : vars.visuals.override_hp ? color_t(
					pclr[0],
					pclr[1],
					pclr[2],
					pclr[3] * (alpha / 255.f)) : color_t(red, green, 0, alpha);

				g_Render->FilledRect(info.box.x - 6, info.box.y - 1, 4, info.box.h + 2, color_t(80, 80, 80, alpha * 0.49f));
				//g_Render->Rect(info.box.x - 6, info.box.y - 1, 4, info.box.h + 2, color_t(10, 10, 10, (alpha * 0.8f)));
				g_Render->FilledRect(info.box.x - 5, info.box.y + hp_percent, 2, info.box.h - hp_percent, hp_color);
				if (hp < 100) {
					std::string health_str = std::to_string(hp);

					ImGui::PushFont(fonts::esp_info);
					const auto& health_width = ImGui::CalcTextSize(health_str.c_str());
					ImGui::PopFont();

					//shading
					g_Render->DrawString((info.box.x - 6 - health_width.x) -1, (info.box.y + hp_percent)+1,
						color_t(5, 5, 5, 255), render::none | render::centered_y, fonts::esp_info,
						health_str.c_str());
					//fin

					g_Render->DrawString(info.box.x - 6 - health_width.x, info.box.y + hp_percent,
						color_t(255, 255, 255, alpha), render::none | render::centered_y, fonts::esp_info,
						health_str.c_str());
				}
			}

			//DrawAngleLine(info.origin, resolver->GetForwardYaw(info.player), color_t(255, 0, 0, alpha));
			//DrawAngleLine(info.origin, resolver->GetBackwardYaw(info.player), color_t(0, 255, 0, alpha));
			//DrawAngleLine(info.origin, resolver->GetAngle(info.player) - resolver->GetForwardYaw(info.player), color_t(255, 255, 0, alpha));

			if (!info.dormant) {
				auto ammo = info.ammo;
				auto max_ammo = info.max_ammo;
				if (vars.visuals.ammo && ammo <= max_ammo && max_ammo > 0)
				{
					color_t& pclr = info.visible ? vars.visuals.ammo_color : vars.visuals.ammo_color_occluded;

					auto clr = color_t(
						pclr[0],
						pclr[1],
						pclr[2],
						pclr[3] * (alpha / 255.f));

					int hp_percent = info.box.w - (int)((info.box.w * ammo) / 100);

					int width = (info.box.w * (ammo / float(max_ammo)));

					char ammostr[10];
					sprintf_s(ammostr, crypt_str("%d"), ammo);

					ImGui::PushFont(fonts::esp_info);

					const auto text_size = ImGui::CalcTextSize(ammostr);

					g_Render->FilledRect(info.box.x - 1, info.box.y + 2 + info.box.h, info.box.w + 2, 4, color_t(80, 80, 80, alpha * 0.49f));
					//g_Render->Rect(info.box.x - 1, info.box.y + 2 + info.box.h, info.box.w + 2, 4, color_t(10, 10, 10, (alpha * 0.8f)));
					g_Render->FilledRect(info.box.x, info.box.y + 3 + info.box.h, width, 2, clr);

					if (ammo < max_ammo) {
						g_Render->DrawString((info.box.x + width - 1 - text_size.x )-1, (info.box.y + info.box.h + 3) +1,
							color_t(5, 5, 5, 255), render::none | render::centered_y, fonts::esp_info,
							ammostr);

						g_Render->DrawString(info.box.x + width - 1 - text_size.x, info.box.y + info.box.h + 3,
							color_t(255, 255, 255, alpha), render::none | render::centered_y, fonts::esp_info,
							ammostr);
					}

					ImGui::PopFont();

					count++;
				}
				if (vars.visuals.weapon)
				{
					auto& clr = info.visible ? vars.visuals.weapon_color : vars.visuals.weapon_color_occluded;
					color_t clrb = color_t(5, 5, 5, 255);
					

					//std::transform(info.weapon_name.begin(), info.weapon_name.end(), info.weapon_name.begin(), tolower);

					int step = 0;
					if (vars.visuals.weaponicontype[1])
					{
						char pp[2];
						weap2icon((ItemDefinitionIndex)info.weapon->GetItemDefinitionIndex(), pp);
						

						g_Render->DrawString((info.box.x + info.box.w / 2) + 1, (info.box.y + 1 + info.box.h + (count * 6.f)) + 1,
							clrb, render::centered_x, fonts::csgoicons, pp);

						g_Render->DrawString(info.box.x + info.box.w / 2, info.box.y + 1 + info.box.h + (count * 6.f),
							clr, render::centered_x, fonts::csgoicons, pp);

						ImGui::PushFont(fonts::csgoicons_large);
						step += ImGui::CalcTextSize(pp).y;
						ImGui::PopFont();
					}
					
					if (vars.visuals.weaponicontype[0])
					{
						g_Render->DrawString((info.box.x + info.box.w / 2) - 1, (info.box.y + 1 + info.box.h + (count * 6.f)) + 1 + step,
							clrb, render::centered_x, fonts::esp_info, info.weapon_name.c_str());

						g_Render->DrawString(info.box.x + info.box.w / 2, info.box.y + 1 + info.box.h + (count * 6.f) + step,
							clr, render::centered_x, fonts::esp_info, info.weapon_name.c_str());
					}
					
				}
			}

			int offset = 0;
			if (info.weapon)
			{
				color_t& pclr = info.visible ? vars.visuals.flags_color : vars.visuals.flags_color_occluded;

				auto clr = info.dormant ? color_t(200, 200, 200, alpha)
					: pclr;

				std::string dist_to_target = std::to_string(info.player_distance); /*+ "ft";*/

				int y_pos = (info.box.y - 2);

				PushFlag(info.box.x + info.box.w + 3, y_pos + offset,
					vars.visuals.flags & 1 && info.have_armor,
					info.have_helmet ? crypt_str("KH") : crypt_str("K"), clr, offset);

				PushFlag(info.box.x + info.box.w + 3, y_pos + offset,
					vars.visuals.flags & 2 && info.scope,
					crypt_str("Scoped"), clr, offset);

				PushFlag(info.box.x + info.box.w + 3, y_pos + offset,
					vars.visuals.flags & 4 && info.flash,
					crypt_str("Blind"), clr, offset);

				PushFlag(info.box.x + info.box.w + 3, y_pos + offset,
					vars.visuals.flags & 8 && info.have_kit,
					crypt_str("Defuser"), clr, offset);

				PushFlag(info.box.x + info.box.w + 3, y_pos + offset,
					vars.visuals.flags & 16 && info.fake_duck,
					crypt_str("FD"), clr, offset);

				PushFlag(info.box.x + info.box.w + 3, y_pos + offset,
					vars.visuals.flags & 32 && info.player_distance > 0.f,
					dist_to_target.c_str(), clr, offset);

				PushFlag(info.box.x + info.box.w + 3, y_pos + offset,
					vars.visuals.flags & 64,
					info.last_place, clr, offset);
			}

			
			

				
			



		}
	}

	renderChicken();

}



void CVisuals::StoreOtherInfo()
{
	static int lastFrame = 0;
	if (lastFrame == interfaces.global_vars->framecount)
		return;

	lastFrame = interfaces.global_vars->framecount;

	WeaponData.clear();
	InfernoInfo.clear();
	BombInfo.clear();

	if (!csgo->local) {
		ProjectileInfo.clear();
		return;
	}


	int maxind = interfaces.ent_list->GetHighestEntityIndex();
	for (auto i = 0; i < maxind; i++)
	{
		IBasePlayer* entity = interfaces.ent_list->GetClientEntity(i);


		if (!entity || entity->IsPlayer() || entity->IsDormant()) {
			continue;
		}


		auto clientcl = entity->GetClientClass();
		if (!clientcl)
			continue;

		auto class_id = entity->GetClientClass()->m_ClassID;

		if (class_id == ClassId->CAK47 || class_id == ClassId->CDEagle || class_id == ClassId->CWeaponAug || class_id == ClassId->CWeaponAWP || class_id == ClassId->CWeaponBaseItem || class_id == ClassId->CWeaponBizon
			|| class_id == ClassId->CWeaponCSBase || class_id == ClassId->CWeaponCSBaseGun || class_id == ClassId->CWeaponCycler || class_id == ClassId->CWeaponElite || class_id == ClassId->CWeaponFamas || class_id == ClassId->CWeaponFiveSeven
			|| class_id == ClassId->CWeaponG3SG1 || class_id == ClassId->CWeaponGalil || class_id == ClassId->CWeaponGalilAR || class_id == ClassId->CWeaponGlock || class_id == ClassId->CWeaponHKP2000 || class_id == ClassId->CWeaponM249
			|| class_id == ClassId->CWeaponM3 || class_id == ClassId->CWeaponM4A1 || class_id == ClassId->CWeaponMAC10 || class_id == ClassId->CWeaponMag7 || class_id == ClassId->CWeaponMP5Navy || class_id == ClassId->CWeaponMP7
			|| class_id == ClassId->CWeaponMP9 || class_id == ClassId->CWeaponNegev || class_id == ClassId->CWeaponNOVA || class_id == ClassId->CWeaponP228 || class_id == ClassId->CWeaponP250 || class_id == ClassId->CWeaponP90
			|| class_id == ClassId->CWeaponSawedoff || class_id == ClassId->CWeaponSCAR20 || class_id == ClassId->CWeaponScout || class_id == ClassId->CWeaponSG550 || class_id == ClassId->CWeaponSG552 || class_id == ClassId->CWeaponSG556
			|| class_id == ClassId->CWeaponSSG08 || class_id == ClassId->CWeaponTaser || class_id == ClassId->CWeaponTec9 || class_id == ClassId->CWeaponTMP || class_id == ClassId->CWeaponUMP45 || class_id == ClassId->CWeaponUSP || class_id == ClassId->CWeaponXM1014 || class_id == ClassId->CC4)
		{
			if (reinterpret_cast<IBaseCombatWeapon*>(entity)->Owner() == -1) // droped grenades and weapones
			{
				WeaponData.emplace_back(entity);
			}

		}
		else if (class_id == ClassId->CPlantedC4)
		{
			BombInfo_t info;
			info.origin = entity->GetAbsOrigin();
			info.time = entity->GetTimerLength();
			info.blow = entity->GetC4Blow();
			info.blow_time = entity->GetC4Blow() - interfaces.global_vars->curtime;
			info.defuse_cooldown = entity->GetDefuseCooldown();
			info.is_defusing = entity->GetDefuser() != nullptr;
			info.bomb_defused = entity->IsBombDefused();
			info.defuse_time = entity->GetDefuseCooldown() - interfaces.global_vars->curtime;
			info.bomb_ticking = entity->GetBombTicking();
			BombInfo.emplace_back(info);
		}
		else if (class_id == ClassId->CInferno)
		{
			auto& inferno = InfernoInfo.emplace_back();
			const auto& inferno_origin = entity->GetOrigin();
			inferno.time_to_die = (((*(float*)(uintptr_t(entity) + 0x20)) + 7.03125f) - interfaces.global_vars->curtime);

			bool* m_bFireIsBurning = entity->m_bFireIsBurning(); //0xE94
			int* m_fireXDelta = entity->m_fireXDelta(); //0x9E4
			int* m_fireYDelta = entity->m_fireYDelta(); //0xB74
			int* m_fireZDelta = entity->m_fireZDelta(); //0xD04
			int m_fireCount = entity->m_fireCount();  //0x13A8
			inferno.entity_origin = inferno_origin;
			inferno.range = 0.f;
			Vector average_vector = Vector(0, 0, 0);
			//	std::vector<Vector> points;
			for (int i = 0; i <= m_fireCount; i++) {
				if (!m_bFireIsBurning[i])
					continue;

				Vector fire_origin = Vector(m_fireXDelta[i], m_fireYDelta[i], m_fireZDelta[i]);
				float delta = fire_origin.Length2D() + 14.4f;
				if (delta > inferno.range)
					inferno.range = delta;

				average_vector += fire_origin;
				if (fire_origin == Vector(0, 0, 0))
					continue;
				inferno.points.push_back(inferno_origin + fire_origin);
			}

			if (m_fireCount <= 1)
				inferno.origin = inferno_origin;
			else
				inferno.origin = (average_vector / m_fireCount) + inferno_origin;

			static auto get_angle = [](Vector point) {
				auto a = (point);
				Vector b = Vector(a.Length2D(), 0.f, 0.f);

				if (a.y < 0.f)
					return float(PI * 2.f) - (acos((a.x * b.x + a.y * b.y + a.z * b.z) / (a.Length() * b.Length())));

				return acos((a.x * b.x + a.y * b.y + a.z * b.z) / (a.Length() * b.Length()));
			};

			std::sort(inferno.points.begin(), inferno.points.end(), [&](Vector a, Vector b) {
				return get_angle(a - inferno.origin) > get_angle(b - inferno.origin);
				});

		}
		else if (class_id == ClassId->CBaseCSGrenade || class_id == ClassId->CBaseGrenade || class_id == ClassId->CFlashbang || class_id == ClassId->CHEGrenade || class_id == ClassId->CIncendiaryGrenade || class_id == ClassId->CItem_Healthshot || class_id == ClassId->CMolotovGrenade || class_id == ClassId->CBreachCharge || class_id == ClassId->CDecoyGrenade || class_id == ClassId->CMolotovGrenade || class_id == ClassId->CSensorGrenade || class_id == ClassId->CSnowball || class_id == ClassId->CSmokeGrenade)
		{
			if (reinterpret_cast<IBaseCombatWeapon*>(entity)->Owner() == -1) // droped grenades and weapones
			{
				WeaponData.emplace_back(entity);
			}
		}
		else if (class_id == ClassId->CBaseCSGrenadeProjectile || class_id == ClassId->CBumpMineProjectile || class_id == ClassId->CBreachChargeProjectile || class_id == ClassId->CDecoyProjectile || class_id == ClassId->CMolotovProjectile || class_id == ClassId->CSensorGrenadeProjectile || class_id == ClassId->CSnowballProjectile || class_id == ClassId->CSmokeGrenadeProjectile)
		{ // grenades that are thrown
			if (const auto it = std::find(ProjectileInfo.begin(), ProjectileInfo.end(), entity->GetRefEHandle()); it != ProjectileInfo.end())
				it->update(entity);
			else
				ProjectileInfo.emplace_back(entity);
		}
		else if (class_id == ClassId->CFogController)
		{
			auto pEnt = (CFogController*)entity;

			int enable = vars.visuals.fog.enabled;
			if (vars.visuals.remove[6])
				enable = false;

			pEnt->enable() = enable ? 1 : 0;
			pEnt->start() = vars.visuals.fog.start;
			pEnt->end() = vars.visuals.fog.end;
			pEnt->density() = vars.visuals.fog.density;
			pEnt->colorPrimary() = vars.visuals.fog.color.u32();
		}
	}



	std::sort(WeaponData.begin(), WeaponData.end());
	
	for (auto it = ProjectileInfo.begin(); it != ProjectileInfo.end();) {
		IBasePlayer* ent = interfaces.ent_list->GetClientEntityFromHandle(it->handle);

		if (!ent) {
			it->exploded = true;

			it = ProjectileInfo.erase(it);
			continue;
			
		}
		else if (!ent->GrenadeExploded() && it->classID == ClassId->CBaseCSGrenadeProjectile)
		{
			it = ProjectileInfo.erase(it);
			continue;
		}
		++it;
	}

}

BaseData::BaseData(IBasePlayer* entity) noexcept
{
	distanceToLocal = entity->GetAbsOrigin().DistTo(csgo->local->GetOrigin());
	//if (entity->IsPlayer()) {
	//	const auto collideable = entity->GetCollideable();
	//	obbMins = collideable->OBBMins();
	//	obbMaxs = collideable->OBBMaxs();
	//}
	//else if (const auto model = entity->GetModel()) {
	//	obbMins = model->mins;
	//	obbMaxs = model->maxs;
	//}

	box.x = 0;
	box.y = 0;
	box.w = 0;
	box.h = 0;
	//coordinateFrame = entity->GetrgflCoordinateFrame();
}

ProjectileInfo_t::ProjectileInfo_t(IBasePlayer* projectile) noexcept : BaseData{ projectile }
{
	name = [](IBasePlayer* projectile) -> std::string {
		switch (projectile->GetClientClass()->m_ClassID) {
		case (int)Z::ClassId::BaseCSGrenadeProjectile:
			if (const auto model = projectile->GetModel(); model && strstr(model->name, crypt_str("flashbang")))
				return crypt_str("Flashbang");
			else
				return crypt_str("HE Grenade");
		case  (int)Z::ClassId::BreachChargeProjectile: return crypt_str("Breach Charge");
		case  (int)Z::ClassId::BumpMineProjectile: return crypt_str("Bump Mine");
		case  (int)Z::ClassId::DecoyProjectile: return crypt_str("Decoy Grenade");
		case  (int)Z::ClassId::MolotovProjectile: return crypt_str("Molotov");
		case  (int)Z::ClassId::SensorGrenadeProjectile: return crypt_str("TA Grenade");
		case  (int)Z::ClassId::SmokeGrenadeProjectile: return crypt_str("Smoke");
		case  (int)Z::ClassId::SnowballProjectile: return crypt_str("Snowball");
		default: assert(false); return crypt_str("unknown");
		}
	}(projectile);

	auto thrower = interfaces.ent_list->GetClientEntityFromHandle(projectile->Thrower());
	if (thrower && csgo->local) {
		if (thrower == csgo->local)
			thrownByLocalPlayer = true;
		else
			thrownByEnemy = (thrower->isEnemy(csgo->local));
	}
	classID = projectile->GetClientClass()->m_ClassID;
	handle = projectile->GetRefEHandle();
}

void ProjectileInfo_t::update(IBasePlayer* projectile) noexcept
{
	static_cast<BaseData&>(*this) = { projectile };
	this->time_to_die = -1.f;
	if (const auto& pos = projectile->GetAbsOrigin(); trajectory.size() < 1 || trajectory[trajectory.size() - 1].second != pos)
		trajectory.emplace_back(interfaces.global_vars->realtime, pos);
	else {
		if (name == crypt_str("Smoke")) {
			if (projectile->m_nSmokeEffectTickBegin())
			{
				float starttime = TICKS_TO_TIME(projectile->m_nSmokeEffectTickBegin());
				this->time_to_die = (starttime + 18.5f) - interfaces.global_vars->curtime;
				this->m_bDidSmokeEffect = projectile->m_bDidSmokeEffect();
			}
		}
	}
	this->origin = projectile->GetAbsOrigin();
	this->entity = projectile;
}

WeaponData_t::WeaponData_t(IBasePlayer* entity) noexcept : BaseData{ entity }
{
	auto weapon = (IBaseCombatWeapon*)entity;
	if (weapon->IsGun())
	{
		clip = weapon->GetAmmo(false);
		reserveAmmo = weapon->GetAmmo(true);
	}
	else
	{
		clip = 0;
		reserveAmmo = 0;
	}
	index = (ItemDefinitionIndex)weapon->GetItemDefinitionIndex();

	if (const auto weaponInfo = weapon->GetCSWpnData()) {
		group = [](int type, int weaponId) -> std::string {
			switch (type) {
			case (int)Z::WeaponType::SubMachinegun: return crypt_str("SMGs");
			case (int)Z::WeaponType::Pistol: return crypt_str("Pistols");
			case (int)Z::WeaponType::Rifle: return crypt_str("Rifles");
			case (int)Z::WeaponType::SniperRifle: return crypt_str("Sniper Rifles");
			case (int)Z::WeaponType::Shotgun: return crypt_str("Shotguns");
			case (int)Z::WeaponType::Machinegun: return crypt_str("Machineguns");
			case (int)Z::WeaponType::Grenade: return crypt_str("Grenades");
			case (int)Z::WeaponType::Melee: return crypt_str("Melee");
			default:
				switch (weaponId) {
				case (int)Z::WeaponId::C4:
				case (int)Z::WeaponId::Healthshot:
				case (int)Z::WeaponId::BumpMine:
				case (int)Z::WeaponId::ZoneRepulsor:
				case (int)Z::WeaponId::Shield:
					return crypt_str("Other");
				default: return crypt_str("All");
				}
			}
		}(weaponInfo->m_iWeaponType, weapon->GetItemDefinitionIndex());
		displayName = name = weapon->GetGunName();
		origin = entity->GetRenderOrigin();
		//displayName = interfaces->localize->findAsUTF8(weaponInfo->name);
	}
}

bool isFakeDucking(IBasePlayer* player) {
	static float storedTick;
	static float crouchedTicks[64];

	if (!player->GetPlayerAnimState())
		return false;

	float m_flDuckAmount = player->GetPlayerAnimState()->m_flAnimDuckAmount;
	float m_flDuckSpeed = player->GetDuckSpeed();
	auto m_fFlags = player->GetFlags();
	auto curtime = TICKS_TO_TIME(player->GetTickBase());

	if (m_flDuckSpeed != 0.f && m_flDuckAmount != 0.f)
	{
		if (m_flDuckSpeed == 8.f && m_flDuckAmount <= 0.9f && m_flDuckAmount > 0.01f)
		{
			if (storedTick != TIME_TO_TICKS(curtime))
			{
				crouchedTicks[player->EntIndex()] = crouchedTicks[player->EntIndex()] + 1;
				storedTick = TIME_TO_TICKS(curtime);
			}
			return (crouchedTicks[player->EntIndex()] >= 5) && m_fFlags & FL_ONGROUND;
		}
		else
			crouchedTicks[player->EntIndex()] = 0;
	}

	return false;
}





void CVisuals::RecordInfo()
{
	if (!vars.visuals.enable) {
		for (int i = 0; i < 64; i++) {
			player_info[i].Reset();
			CDormantEsp::Get().m_cSoundPlayers[i].reset();
		}
		CDormantEsp::Get().m_utlvecSoundBuffer.RemoveAll();
		CDormantEsp::Get().m_utlCurSoundList.RemoveAll();
		return;
	}
	ooftargets.clear();
	CDormantEsp::Get().Start();

	/*
	static auto hud_ptr = *(DWORD**)(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
		crypt_str("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);

	static auto find_hud_element =
		reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnva1(hs::client_dll.s().c_str())]().c_str()),
			crypt_str("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
			*/

	if (!H::hud || !H::findHudElement)
		return;

	auto radar_base = H::findHudElement(H::hud, crypt_str("CCSGO_HudRadar"));
	auto hud_radar = (CCSGO_HudRadar*)(radar_base - 0x14);

	int maxind = interfaces.ent_list->GetHighestEntityIndex();
	
	for (auto i = 0; i < maxind; ++i)
	{
		IBasePlayer* entity = interfaces.ent_list->GetClientEntity(i);
		
		if (i < 64) {
			
			INFO_t& info = player_info[i];

			info.is_valid = false;
			info.offscreen = false;

			if (!csgo->local || !entity)
				continue;

			if (!entity->IsPlayer())
				continue;

			if (csgo->local == nullptr
				|| !entity
				|| !entity->IsPlayer()
				|| entity == csgo->local
				//|| entity->GetTeam() == csgo->local->GetTeam()
				|| !entity->isEnemy(csgo->local)
				|| !entity->isAlive()) {
				continue;
			}

			info.is_valid = true;
			info.player = entity;
			info.ClassID = entity->GetClientClass()->m_ClassID;

			if (vars.visuals.dormant) {
				info.dormant = false;
				auto backup_flags = entity->GetFlags();
				auto backup_origin = entity->GetRenderOrigin();

				if (entity->IsDormant()) {
					info.dormant = CDormantEsp::Get().AdjustSound(entity);
					if (!info.dormant) {
						info.is_valid = false;
						continue;
					}
				}
				else
				{
					health[i] = entity->GetHealth();
					CDormantEsp::Get().m_cSoundPlayers[i].reset(true, entity->GetAbsOrigin(), entity->GetFlags());
				}

				if (radar_base && hud_radar && entity->IsDormant() && entity->isEnemy(csgo->local) && entity->TargetSpotted())
					health[i] = hud_radar->radar_info[i].health;

				if (!health[i])
				{
					if (entity->IsDormant())
					{
						entity->GetFlagsPtr() = backup_flags;
						entity->SetAbsOrigin(backup_origin);
						info.is_valid = false;
					}

					continue;
				}

				if (info.dormant)
				{
					if ((csgo->LastSeenTime[entity->GetIndex()] + 3.f) <= csgo->get_absolute_time()
						&& info.alpha > 0)
						info.alpha--;

					if (info.alpha <= 1) {
						info.is_valid = false;
						continue;
					}
				}
				else
				{
					info.alpha = 255;
					csgo->LastSeenTime[entity->GetIndex()] = csgo->get_absolute_time();
				}
				info.origin = entity->GetRenderOrigin();
				info.alpha = std::clamp(info.alpha, 0.f, 255.f);
			}
			else {
				bool dormant = entity->IsDormant();
				info.dormant = dormant;

				if (dormant || !entity->isAlive()) {
					info.is_valid = false;
					csgo->LastSeenTime[entity->GetIndex()] = csgo->get_absolute_time();
					continue;
				}

				info.alpha = 255;
				info.origin = entity->GetRenderOrigin();
			}

			auto weapon = entity->GetWeapon();
			if (!weapon) {
				info.is_valid = false;
				continue;
			}

			CBaseCSGrenade* pCSGrenade = (CBaseCSGrenade*)weapon;
			if (!pCSGrenade) {
				info.is_valid = false;
				continue;
			}

			auto isOnScreen = [](Vector origin, Vector& screen) -> bool
			{
				if (!Math::WorldToScreen2(origin, screen))
					return false;

				auto xOk = csgo->w > screen.x;
				auto yOk = csgo->h > screen.y;

				return xOk && yOk;
			};

			Vector screenPos;
			info.on_screen = isOnScreen(entity->GetAbsOrigin(), screenPos);
			info.visible = csgo->local->IsVisible(entity);

			info.offscreen = !GetBox(entity, info.box.x, info.box.y, info.box.w, info.box.h, info.origin);
			if (info.offscreen) {
				//info.is_valid = false;
				continue;
			}

			if (csgo->local && csgo->local->isAlive()) {
				info.player_distance = std::floor(csgo->local->GetAbsOrigin().DistTo(entity->GetAbsOrigin())); // calculate distance between local and target

				if (entity->GetWeapon()->IsZeus()) { // have taser, will turn indicator
					info.zeuser_stages = good;
					if (info.player_distance <= 600.f) {// target can move and hit you
						info.zeuser_stages = warning;
						if (info.player_distance <= 250.f) // target is near you so he can kill you with taser
							info.zeuser_stages = fatal;
					}
				}
				else
					info.zeuser_stages = none; // doesn't have taser, don't need to indicate smth
			}
			else {
				info.zeuser_stages = none; // local is dead so we have no reasons to indicate that enemy have zeus
				info.player_distance = -1; // clear last distance to entity
			}

			info.name = entity->GetName();
			info.weapon = weapon;
			info.hp = entity->GetHealth();
			info.is_gun = weapon->IsGun();
			if (info.is_gun)
			{
				info.ammo = weapon->GetAmmo(false);
				info.max_ammo = weapon->GetAmmo(true);
			}
			else
			{
				info.ammo = 0;
				info.max_ammo = 0;
			}
			info.scope = entity->IsScoped();
			info.flash = entity->IsFlashed();
			info.have_kit = entity->HaveDefuser();
			info.choke = entity->GetChokedPackets();
			info.have_armor = entity->GetArmor() > 0;
			info.have_helmet = entity->HasHelmet();
			info.weapon_name = weapon->GetGunName();
			info.duck = entity->GetDuckAmount();
			info.last_place = entity->GetLastPlace();
			info.fake_duck = isFakeDucking(entity);
			

			//std::transform(info.weapon_name.begin(), info.weapon_name.end(), info.weapon_name.begin(), ::toupper);

			auto record = g_Animfix->get_latest_animation(entity);


			/*
			if (record && record->player) {

				auto hitboxes = Ragebot::Get().GetHitboxesToScan(record->player);
				info.AnimInfo.hitboxes.clear();
				info.AnimInfo.hitboxes = hitboxes;

				memcpy(info.AnimInfo.bones, record->bones, sizeof(matrix) * 128);

				info.AnimInfo.points.clear();

	//			auto points = Ragebot::Get().GetAdvancedHeadPoints(record->player, info.AnimInfo.bones);
	//			for (const auto& p : points)
	//				info.AnimInfo.points.push_back(p);

				for (auto i : hitboxes) {
					auto points = Ragebot::Get().GetMultipoints(record->player, i, info.AnimInfo.bones);
					for (const auto& p : points)
						info.AnimInfo.points.push_back(p);
				}
			}
			else {
				info.AnimInfo.points.clear();
				info.AnimInfo.hitboxes.clear();
			}
			*/

			info.hdr = interfaces.models.model_info->GetStudioModel(entity->GetModel());
			if (!info.hdr)
				continue;

#ifndef _DEBUG
			for (int j = 0; j < info.hdr->num_bones; j++)
			{
				mstudiobone_t* pBone = info.hdr->GetBone(j);

				if (pBone && (pBone->flags & 0x100) && (pBone->parent != -1))
				{
					info.bone_pos_child[j] = info.player->GetBonePos(info.player->GetBoneCache().Base(), j);
					info.bone_pos_parent[j] = info.player->GetBonePos(info.player->GetBoneCache().Base(), pBone->parent);
				}
			}
#else
			if (record && record->player) {
				for (int j = 0; j < info.hdr->num_bones; j++)
				{
					mstudiobone_t* pBone = info.hdr->GetBone(j);

					if (pBone && (pBone->flags & 0x100) && (pBone->parent != -1))
					{
						info.bone_pos_child[j] = info.player->GetBonePos(record->bones, j);
						info.bone_pos_parent[j] = info.player->GetBonePos(record->bones, pBone->parent);
					}
				}
			}
			else {
				for (int i = 0; i < 128; i++) {
					info.bone_pos_child[i] = Vector(0, 0, 0);
					info.bone_pos_parent[i] = Vector(0, 0, 0);
				}
			}
#endif
		}
	
	}
}

void CVisuals::RecoilCrosshair()
{
	static auto cl_crosshair_recoil = interfaces.cvars->FindVar(crypt_str("cl_crosshair_recoil"));
	cl_crosshair_recoil->SetValue(vars.visuals.recoil_crosshair ? 1 : 0);
}

typedef struct {
	uint64_t steamid;
	int uid;
	int dmg;
	int kills;
	int suicides;
	std::string name;
}PlayerStruct;

std::vector<PlayerStruct> PlayerDamageInfo;

bool first = false;

void CVisuals::TeamDamage_CollectInfo(IGameEvent* event)
{
	if (!first)
		return;

	if (!interfaces.engine->IsConnected() || !csgo->local)
		return;

	if (strstr(event->GetName(), crypt_str("player_hurt")))
	{
		int attackerID = event->GetInt(crypt_str("attacker"));
		int hurtID = event->GetInt(crypt_str("userid"));

		if (attackerID == hurtID)
			return;

		int dmg = event->GetInt(crypt_str("dmg_health"));
		auto attacker = interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(attackerID));
		auto hurt = interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(hurtID));

		if (!attacker || !hurt)
			return;

		dmg = min(dmg, hurt->GetHealth());

		//int ourteam = csgo->local->GetTeam();

		if(attacker->isEnemy(csgo->local) || hurt->isEnemy(csgo->local))
			return;

		bool found = false;
		for (int i = 0; i < PlayerDamageInfo.size(); i++) {
			if (PlayerDamageInfo[i].steamid == attacker->getSteamID64()) {
				PlayerDamageInfo[i].dmg += dmg;
				found = true;
			}
		}
		if (!found) { // if no info about attacker, create it
			PlayerStruct info;
			info.steamid = attacker->getSteamID64();
			info.uid = attackerID;
			info.dmg = dmg;
			info.kills = 0;
			info.suicides = 0;
			info.name = attacker->GetName();
			PlayerDamageInfo.push_back(info);
		}
		

	}
	else if (strstr(event->GetName(), crypt_str("player_death")))
	{

		int attackerID = event->GetInt(crypt_str("attacker"));
		int hurtID = event->GetInt(crypt_str("userid"));

		

		auto attacker = interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(attackerID));
		auto hurt = interfaces.ent_list->GetClientEntity(interfaces.engine->GetPlayerForUserID(hurtID));

		if (!attacker || !hurt)
			return;

		//int ourteam = csgo->local->GetTeam();
		if (attacker->isEnemy(csgo->local) || hurt->isEnemy(csgo->local))
			return;

		if (attacker == hurt)
		{
			
			bool found = false;
			for (int i = 0; i < PlayerDamageInfo.size(); i++) {
				if (PlayerDamageInfo[i].steamid == attacker->getSteamID64()) {
					PlayerDamageInfo[i].suicides++;
					found = true;
				}
			}
			if (!found) { // if no info about attacker, create it
				PlayerStruct info;
				info.steamid = attacker->getSteamID64();
				info.uid = attackerID;
				info.dmg = 0;
				info.kills = 0;
				info.suicides = 1;
				info.name = attacker->GetName();
				PlayerDamageInfo.push_back(info);
			}
			
		}
		else
		{
			
			bool found = false;
			for (int i = 0; i < PlayerDamageInfo.size(); i++) {
				if (PlayerDamageInfo[i].steamid == attacker->getSteamID64()) {
					PlayerDamageInfo[i].kills++;
					found = true;
				}
			}
			if (!found) { // if no info about attacker, create it
				PlayerStruct info;
				info.steamid = attacker->getSteamID64();
				info.uid = attackerID;
				info.dmg = 0;
				info.kills = 1;
				info.suicides = 0;
				info.name = attacker->GetName();
				PlayerDamageInfo.push_back(info);
			}
			
		}
	}
	else if (strstr(event->GetName(), crypt_str("cs_win_panel_match")))
	{
		PlayerDamageInfo.clear();
	}
	
}


void CVisuals::TeamDamage_DisplayInfo()
{
	if (!first)
	{
		PlayerDamageInfo.clear();
		first = true;
	}

	if (!vars.visuals.teamdmglist)
	{
		return;
	}

	if (!interfaces.engine->IsConnected()) {
		if (!PlayerDamageInfo.empty())
			PlayerDamageInfo.clear();
		csgo->cooldowncounter = 0;
	}



	if (ImGui::GetCurrentContext() && ((!interfaces.engine->IsInGame() && vars.menu.open) || ((interfaces.engine->IsInGame() && csgo->cmd) && ((csgo->cmd->buttons & IN_SCORE) || vars.menu.open))))
	{
		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, vars.menu.window_opacity / 100.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.f, 1.f, 0.5f, 1.f));
		//selectable colors
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));

		auto& style = ImGui::GetStyle();

		style.WindowRounding = 3.f;
		style.WindowTitleAlign = { 0.5, 0.5 };
		style.WindowBorderSize = 0.f;
		style.FrameBorderSize = 0.f;
		style.ChildRounding = 3.f;
		style.WindowPadding.x = 5.f;
		style.FramePadding = { 0.f, 2.f };
		style.FrameRounding = 3.f;

		ImGui::GetStyle().WindowPadding = { 5,3 };
		ImGui::PushFont(fonts::logger);
		ImGui::SetNextWindowSizeConstraints({ 90,30 }, { 400,800 });
		ImGui::Begin(crypt_str("Team Damage"), (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		if (vars.menu.inLoad) {
			//ImGui::SetWindowPos(ImVec2(vars.visuals.keybindx * csgo->w, vars.visuals.keybindy * csgo->h), ImGuiCond_Always);
			ImGui::SetWindowPos(ImVec2(std::clamp(vars.visuals.teamdmgx * csgo->w, 0.f, (float)csgo->w), std::clamp(vars.visuals.teamdmgy * csgo->h, 0.f, (float)csgo->h)), ImGuiCond_Always);
			vars.menu.inLoad = false;
		}

		vars.menu.cteamdmgx = ImGui::GetWindowPos().x / csgo->w;
		vars.menu.cteamdmgy = ImGui::GetWindowPos().y / csgo->h;


		if (ImGui::BeginTable(crypt_str("##teamdamag_windaw"), 4))
		{

			for (int row = 0; row < PlayerDamageInfo.size() + 1; row++)
			{
				ImGui::PushID(row * 23232);
				if (row == 0)
				{

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text(crypt_str("Name"));
					if (row != PlayerDamageInfo.size())
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
					ImGui::TableNextColumn();
					ImGui::Text(crypt_str("Kills"));
					if (row != PlayerDamageInfo.size())
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
					ImGui::TableNextColumn();
					ImGui::Text(crypt_str("Damage"));
					if (row != PlayerDamageInfo.size())
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
					ImGui::TableNextColumn();
					ImGui::Text(crypt_str("Suicides"));
				}
				else
				{
					int curvectorind = row - 1;
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text(std::string(PlayerDamageInfo.at(curvectorind).name + crypt_str("\t")).c_str());
					if (row != PlayerDamageInfo.size())
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
					ImGui::TableNextColumn();
					ImGui::Text(to_string(PlayerDamageInfo.at(curvectorind).kills).c_str());
					if (row != PlayerDamageInfo.size())
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
					ImGui::TableNextColumn();
					ImGui::Text(to_string(PlayerDamageInfo.at(curvectorind).dmg).c_str());
					if (row != PlayerDamageInfo.size())
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
					ImGui::TableNextColumn();
					ImGui::Text(to_string(PlayerDamageInfo.at(curvectorind).suicides).c_str());
				}

				if (row != PlayerDamageInfo.size())
				{
					ImGui::Dummy({ 0, 2 });
					//ImGui::Separator();
				}
				else
				{
					ImGui::Spacing();
				}

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(16);
		ImGui::End();

		
	}

}



void CVisuals::DrawKeybindsWindow()
{

	static float last_empty_time;
	static float last_notempty_time;
	static vector<pKeybind*> active_keys;
	const float fade_time = 0.15f;
	if (vars.visuals.indicators && interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && csgo->local)
	{
		active_keys.clear();
		for (auto key : keybinds)
		{
			if (key->properstate())
				active_keys.push_back(key);
		}
	}

	if (active_keys.empty())
		last_empty_time = realrealtime;
	else
		last_notempty_time = realrealtime;


	float fadepercent = 1.f;
	

	if (!vars.menu.open)
	{
		if (last_notempty_time + fade_time < realrealtime && (interfaces.engine->IsInGame()))
			return;

		if (last_empty_time < realrealtime && last_empty_time + fade_time > realrealtime)
		{
			fadepercent = std::clamp((realrealtime - last_empty_time) / fade_time, 0.f, 1.f);
		}

		if (last_notempty_time < realrealtime && last_notempty_time + fade_time > realrealtime)
		{
			fadepercent = std::clamp(1.f - ((realrealtime - last_notempty_time) / fade_time), 0.f, 1.f);
		}
	}


	if (vars.visuals.indicators && ImGui::GetCurrentContext() && ((!interfaces.engine->IsInGame() && vars.menu.open) || interfaces.engine->IsInGame()))
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, (vars.menu.window_opacity / 100.f) * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.f, 1.f, 0.5f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f * fadepercent));
		//selectable colors
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));

		auto& style = ImGui::GetStyle();

		style.WindowRounding = 3.f;
		style.WindowTitleAlign = { 0.5, 0.5 };
		style.WindowBorderSize = 0.f;
		style.FrameBorderSize = 0.f;
		style.ChildRounding = 3.f;
		style.FramePadding = { 0.f, 2.f };
		style.FrameRounding = 3.f;
		style.WindowPadding = { 5,3 };
		ImGui::PushFont(fonts::logger);
		ImGui::SetNextWindowSizeConstraints({ 90,30 }, { 400,800 });
		ImGui::Begin(crypt_str("Keybinds"), (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		if (vars.menu.inLoad) {
			//ImGui::SetWindowPos(ImVec2(vars.visuals.keybindx * csgo->w, vars.visuals.keybindy * csgo->h), ImGuiCond_Always);
			ImGui::SetWindowPos(ImVec2(std::clamp(vars.visuals.keybindx * csgo->w, 0.f, (float)csgo->w), std::clamp(vars.visuals.keybindy * csgo->h, 0.f, (float)csgo->h)), ImGuiCond_Always);
			vars.menu.inLoad = false;
		}

		vars.menu.ckeybindx = ImGui::GetWindowPos().x / csgo->w;
		vars.menu.ckeybindy = ImGui::GetWindowPos().y / csgo->h;

		

		if (!active_keys.empty())
		{
			if (ImGui::BeginTable(crypt_str("##keybindas_windaw"), 2))
			{
				for (int i = 0; i < active_keys.size(); i++)
				{
					auto& curkey = active_keys.at(i);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text(std::string(curkey->name + crypt_str("\t")).c_str());
					if (i != active_keys.size() - 1)
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
					ImGui::TableNextColumn();
					ImGui::Text(interfaces.inputsystem->vkey2string(curkey->key));

					if (i != active_keys.size() - 1)
					{
						ImGui::Dummy({ 0, 2 });
						//ImGui::Separator();
					}
					else
					{
						ImGui::Spacing();
					}
				}

				ImGui::EndTable();
			}
		}
		
		ImGui::PopFont();
		ImGui::PopStyleColor(17);
		ImGui::End();
	}
}

void CVisuals::DrawSpectatorWindow()
{
	static float last_empty_time;
	static float last_notempty_time;
	static vector<std::string> ents_to_list;
	if (vars.visuals.speclist && interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && csgo->local)
	{
		ents_to_list.clear();
		if (csgo->local->isAlive())
		{
			for (int i = 0; i < interfaces.engine->GetMaxClients(); i++)
			{
				auto ent = interfaces.ent_list->GetClientEntity(i);
				if (!ent)
					continue;
				if (ent->isAlive() || ent == csgo->local || ent->IsDormant())
					continue;
				auto obsent = interfaces.ent_list->GetClientEntityFromHandle(ent->GetObserverTargetHandle());
				if (!obsent)
					continue;
				if (obsent != csgo->local)
					continue;
				ents_to_list.push_back(ent->GetName());
				//text += ent->GetName() + crypt_str("\n");
			}
		}
		else
		{
			auto localobsent = interfaces.ent_list->GetClientEntityFromHandle(csgo->local->GetObserverTargetHandle());
			if (localobsent)
			{
				for (int i = 0; i < interfaces.engine->GetMaxClients(); i++)
				{
					auto ent = interfaces.ent_list->GetClientEntity(i);
					if (!ent)
						continue;

					if (ent->isAlive() || ent == csgo->local || ent->IsDormant())
						continue;
					auto obsent = interfaces.ent_list->GetClientEntityFromHandle(ent->GetObserverTargetHandle());
					if (!obsent)
						continue;
					if (obsent != localobsent)
						continue;
					ents_to_list.push_back(ent->GetName());
				}
			}

		}

	}

	if (ents_to_list.empty())
		last_empty_time = realrealtime;
	else
		last_notempty_time = realrealtime;
		
	

	float fadepercent = 1.f;

	if (!vars.menu.open)
	{
		if (last_notempty_time + 0.5 < realrealtime && (interfaces.engine->IsInGame()))
			return;

		if (last_empty_time < realrealtime && last_empty_time + 0.5f > realrealtime)
		{
			fadepercent = std::clamp((realrealtime - last_empty_time) / 0.5f, 0.f, 1.f);
		}

		if (last_notempty_time < realrealtime && last_notempty_time + 0.5f > realrealtime)
		{
			fadepercent = std::clamp(1.f - ((realrealtime - last_notempty_time) / 0.5f), 0.f, 1.f);
		}
	}
	


	if (vars.visuals.speclist && ImGui::GetCurrentContext() && ((!interfaces.engine->IsInGame() && vars.menu.open) || interfaces.engine->IsInGame()))
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, (vars.menu.window_opacity / 100.f) * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f * fadepercent));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.f, 1.f, 0.5f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f * fadepercent));
		//selectable colors
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(41.f / 255.f, 41.f / 255.f, 41.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(55.f / 255.f, 55.f / 255.f, 55.f / 255.f, 1.f));

		auto& style = ImGui::GetStyle();

		style.WindowRounding = 3.f;
		style.WindowTitleAlign = { 0.5, 0.5 };
		style.WindowBorderSize = 0.f;
		style.FrameBorderSize = 0.f;
		style.ChildRounding = 3.f;
		style.FramePadding = { 0.f, 2.f };
		style.FrameRounding = 3.f;
		style.WindowPadding = { 5,3 };
		ImGui::PushFont(fonts::logger);
		ImGui::SetNextWindowSizeConstraints({ 90,30 }, { 400,800 });
		ImGui::Begin(crypt_str("Spectators"), (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		
		
		

		if (vars.menu.inLoad2) {
			ImGui::SetWindowPos(ImVec2(std::clamp(vars.visuals.speclistx * csgo->w, 0.f, (float)csgo->w), std::clamp(vars.visuals.speclisty * csgo->h ,0.f, (float)csgo->h)), ImGuiCond_Always);
			vars.menu.inLoad2 = false;
		}

		vars.menu.cspeclistx = ImGui::GetWindowPos().x / csgo->w;
		vars.menu.cspeclisty = ImGui::GetWindowPos().y / csgo->h;

		ImVec2 cur_pos = ImGui::GetCursorPos();
		ImVec2 win_size = ImGui::GetWindowSize();
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(cur_pos, { win_size.x / 2 + cur_pos.x, cur_pos.y + 1 }, color_t(0, 255, 128, 0).u32(), color_t(0, 255, 128, 255).u32(), color_t(0, 255, 128, 255).u32(), color_t(0, 255, 128, 0).u32());
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor({ win_size.x / 2 + cur_pos.x, cur_pos.y }, {cur_pos.x + win_size.x, cur_pos.y + 1}, color_t(0, 255, 128, 255).u32(), color_t(0, 255, 128, 0).u32(), color_t(0, 255, 128, 0).u32(), color_t(0, 255, 128, 255).u32());
		
		if (interfaces.engine->IsInGame())
		{
			for (int i = 0; i < ents_to_list.size(); i++)
			{
				ImGui::Text(ents_to_list.at(i).c_str());
					
				if (i != ents_to_list.size() - 1)
				{
					ImGui::Dummy({ 0, 2 });
					//ImGui::Separator();
				}
				else
				{
					ImGui::Spacing();
				}
			}
		

		}
		ImGui::PopFont();
		ImGui::PopStyleColor(17);

		ImGui::End();
	}
}




void CVisuals::Graph_CollectInfo()
{
	if (vars.movement.graph.enabled || vars.movement.showvelocity || vars.movement.showstamina) {

		realtickcount = interfaces.global_vars->tickcount;
		realrealtime = interfaces.global_vars->realtime;

		if (!csgo->is_connected)
			return;

		auto ent = csgo->local;
		if (!ent)
			return;
		
		
		if (!csgo->local->isAlive())
		{

			auto obshand = ent->GetObserverTargetHandle();
			if (!obshand)
				return;
			ent = interfaces.ent_list->GetClientEntityFromHandle(obshand);
			if (!ent)
				return;
			if (!ent->isAlive())
				return;
			

		}



		if (oldflags & FL_ONGROUND && !(ent->GetFlags() & FL_ONGROUND))
		{
			takeoffspeed = int(round(ent->GetVelocity().Length2D()));

			if (ent != csgo->local)
				takeoffstamina = 0.f;
			else
				takeoffstamina = ent->GetStamina();

			takeofftime = interfaces.global_vars->tickcount + TIME_TO_TICKS(5.f);
		}


		oldflags = ent->GetFlags();
		curvel = ent->GetVelocity();
		if (ent != csgo->local)
			curstamina = 0.f;
		else
			curstamina = ent->GetStamina();


		if (interfaces.global_vars->curtime - last_log > interfaces.global_vars->interval_per_tick)
		{
			last_log = interfaces.global_vars->curtime;
			//engine_prediction->RestoreEntityToPredictedFrame(0, interfaces.prediction->Split->nCommandsPredicted - 1);

			velocity_data.push_front({ ent->GetVelocity().Length2D(), !(ent->GetFlags() & FL_ONGROUND) });
			
			
			stamina_data.push_front({ ent != csgo->local ? 0.f : ent->GetStamina(), !(ent->GetFlags() & FL_ONGROUND) });
		}

		if (velocity_data.size() > vars.movement.graph.size)
			velocity_data.pop_back();

		if (stamina_data.size() > vars.movement.graph.size)
			stamina_data.pop_back();
	}
}

void CVisuals::DrawDlights()
{
	if (!interfaces.engine->IsInGame() || !interfaces.engine->IsConnected()) 
		return;

	int clnum = interfaces.engine->GetMaxClients();
	for (size_t i = 0; i < clnum; i++)
	{
		auto ent = interfaces.ent_list->GetClientEntity(i);
		if (!ent)
			continue;

		if (ent != csgo->local && ent->isEnemy(csgo->local) && !ent->IsDormant() && ent->IsPlayer() && ent->isAlive())
		{
			color_t& pclr = player_info[ent->index()].visible ? vars.visuals.dlights_color : vars.visuals.dlights_color_occluded;

			if (vars.visuals.dlights)
			{
				dlight_t* thislight = interfaces.iv_effects->CL_AllocDlight(i);
				thislight->origin = Ragebot::Get().GetVisualHitbox(ent, (int)CSGOHitboxID::Head);
				thislight->radius = 100.f;
				ColorRGBExp32 col;
				col.r = pclr.get_red();
				col.g = pclr.get_green();
				col.b = pclr.get_blue();
				col.exponent = 8.f;
				thislight->color = col;
				thislight->die = interfaces.global_vars->curtime + 0.05;
				thislight->decay = thislight->radius / 5.f;
				thislight->key = i;
			}

			
		}
	}
}



void CVisuals::sunsetmode()
{
	if (!csgo->is_connected || !csgo->local || !interfaces.engine->IsInGame())
		return;

	static ConVar* cl_csm_shadows = interfaces.cvars->FindVar(crypt_str("cl_csm_shadows"));

	if (!vars.visuals.sunset_enabled)
	{
		cl_csm_shadows->SetInt(0);
		return;
	}

	cl_csm_shadows->SetInt(1);
	

	static ConVar* cl_csm_max_shadow_dist = interfaces.cvars->FindVar(crypt_str("cl_csm_max_shadow_dist"));
	cl_csm_max_shadow_dist->SetInt(800);

	static ConVar* cl_csm_rot_override = interfaces.cvars->FindVar(crypt_str("cl_csm_rot_override"));
	cl_csm_rot_override->SetInt(800);

	static ConVar* cl_csm_rot_x = interfaces.cvars->FindVar(crypt_str("cl_csm_rot_x"));
	cl_csm_rot_x->SetInt(vars.visuals.rot_x);

	static ConVar* cl_csm_rot_y = interfaces.cvars->FindVar(crypt_str("cl_csm_rot_y"));
	cl_csm_rot_y->SetInt(vars.visuals.rot_y);
}