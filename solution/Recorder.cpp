#include <mutex>
#include <numeric>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <deque>
#include <windows.h>
#include <math.h>
#include <xstring>
#include <vector>
#include <iostream>
#include "imgui/imgui.h"
#include "Recorder.h"
#include "Hooks.h"
#include "Misc.h"
#include "EnginePrediction.h"


void color_picker4(const char* name, color_t& clr)
{

	ImVec4 clrs = ImVec4(std::clamp((float)clr[0] / 255.f, 0.f, 1.f), std::clamp((float)clr[1] / 255.f, 0.f, 1.f), std::clamp((float)clr[2] / 255.f, 0.f, 1.f), std::clamp((float)clr[3] / 255.f, 0.f, 1.f));


	float color[4] = { clrs.x, clrs.y,clrs.z,clrs.w };
	std::string nm = crypt_str("\t") + std::string(name);
	ImGui::TextColored(ImVec4{ 0.55,0.55,0.55,1.f }, nm.c_str());
	ImGui::SameLine();
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

static auto world_circle = [](Vector location, float radius, color_t col = color_t(127, 30, 255, 140), bool filled = false) {
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
	if (filled)
		ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(points.data(), points.size(), col.u32());
	else
		ImGui::GetBackgroundDrawList()->AddPolyline(points.data(), points.size(), col.u32(), true, 2.f);

	ImGui::GetBackgroundDrawList()->Flags = flags_backup;
};

struct Frame
{
	float viewangles[2];
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	//unsigned char impulse;
	short mousedx;
	short mousedy;
	Vector position;

	Frame(CUserCmd* cmd, Vector pos)
	{
		this->viewangles[0] = cmd->viewangles.x;
		this->viewangles[1] = cmd->viewangles.y;
		this->forwardmove = cmd->forwardmove;
		this->sidemove = cmd->sidemove;
		this->upmove = cmd->upmove;
		this->buttons = cmd->buttons;
		//this->impulse = cmd->impulse;
		this->mousedx = cmd->mousedx;
		this->mousedy = cmd->mousedy;
		this->position = pos;
	}

	void Replay(CUserCmd* cmd)
	{
		cmd->viewangles.x = this->viewangles[0];
		cmd->viewangles.y = this->viewangles[1];
		cmd->forwardmove = this->forwardmove;
		cmd->sidemove = this->sidemove;
		cmd->upmove = this->upmove;
		cmd->buttons = this->buttons;
		//cmd->impulse = this->impulse;
		cmd->mousedx = this->mousedx;
		cmd->mousedy = this->mousedy;
	}
};

typedef std::vector<Frame> FrameContainer;

struct FullRoute
{
	std::string routename;
	std::string makername;
	FrameContainer frames;
};

std::vector<FullRoute> mapWithRoutes;

class Recorder
{
private:
	bool is_recording_active = false;
	FrameContainer recording_frames;

public:
	void StartRecording()
	{
		this->is_recording_active = true;
	}

	void StopRecording()
	{
		this->is_recording_active = false;
	}

	bool IsRecordingActive() const
	{
		return this->is_recording_active;
	}

	FrameContainer& GetActiveRecording()
	{
		return this->recording_frames;
	}

	void SetRecording(FrameContainer frames)
	{
		this->recording_frames = frames;
	}
};

class Playback
{
private:
	bool is_playback_active = false;
	size_t current_frame = 0;
	const FrameContainer& active_demo = FrameContainer(); // FrameContainer& active_demo = FrameContainer();

public:
	void StartPlayback(FrameContainer& frames)
	{
		this->is_playback_active = true;

		uintptr_t active_demoptr = (uintptr_t)&this->active_demo;
		*(FrameContainer*)active_demoptr = frames;
	};

	void StopPlayback()
	{
		this->is_playback_active = false;
		this->current_frame = 0;
	};

	bool IsPlaybackActive() const
	{
		return this->is_playback_active;
	}

	size_t GetCurrentFrame() const
	{
		return this->current_frame;
	};

	void SetCurrentFrame(size_t frame)
	{
		this->current_frame = frame;
	};

	const FrameContainer& GetActiveDemo() //FrameContainer& GetActiveDemo() const
	{
		return this->active_demo;
	}
};

Playback playback;
Recorder recorder;

char filename[32] = "";

int curroute = -1;

static char buffer[32];
static char buffer1[32];

static int delselection;

std::string routestodelete = "";
std::string routelist = "";

void movementRecorder::forcestop()
{
	playback.StopPlayback();
}


void movementRecorder::Draw() {

	ImGui::PushFont(fonts::font);
	
	//ImGui::Checkbox(crypt_str("Enabled"), &vars.movement.recorder.enabled);
	

	if (ImGui::Button(crypt_str("Reload routes")))
		route->listRoutes();

	ImGui::PushID(crypt_str("routelist"));
	int flags;
	flags = ImGuiInputTextFlags_EnterReturnsTrue;
	
	auto& routeItems = route->getRoutes();

	static int currentRoute = -1;

	if (static_cast<std::size_t>(currentRoute) >= routeItems.size())
		currentRoute = -1;
	

	ImGui::TextWrapped(crypt_str("Files"));
	ImGui::Combo(crypt_str(""), &currentRoute, [](void* data, int idx, const char** out_text) {
		auto& vector = *static_cast<std::vector<std::string>*>(data);
		*out_text = vector[idx].c_str();
		return true;
		}, &routeItems, routeItems.size(), 10);

	ImGui::PopID();
	/*
	if (ImGui::ListBox("", &currentRoute, [](void* data, int idx, const char** out_text) {
		auto& vector = *static_cast<std::vector<std::string>*>(data);
		*out_text = vector[idx].c_str();
		return true;
		}, &routeItems, routeItems.size(), 10) && currentRoute != -1) {
		//do nothing
	}
	*/
	//buffer = routeItems[currentRoute];

	
	ImGui::PushID(1);
	ImGui::InputTextWithHint(crypt_str(""), crypt_str("Route name"), buffer, 32, flags);
	ImGui::PopID();
	
	ImGui::PushID(2);
	ImGui::InputTextWithHint(crypt_str(""), crypt_str("Creator name"), buffer1, 32, flags);
	ImGui::PopID();

	if (currentRoute != -1) {
		
		if (ImGui::Button(crypt_str("Save route to selected"))) {
			route->save(currentRoute, buffer, buffer1);
			ZeroMemory(buffer, 32);
			ZeroMemory(buffer1, 32);
		}
	}


	if (interfaces.engine->IsInGame()) {
		
		if (ImGui::Button(crypt_str("Create File"))) {
			std::string routname = interfaces.engine->GetLevelNameShort() + std::to_string(int(1.0f / interfaces.global_vars->interval_per_tick));
			route->add(routname.c_str());
			ZeroMemory(buffer, 32);
			ZeroMemory(buffer1, 32);
		}
	}

	if (currentRoute != -1) {
		
		if (ImGui::Button(crypt_str("Load selected map file"))) {
			route->load(currentRoute);
			ZeroMemory(buffer, 32);
			ZeroMemory(buffer1, 32);
		}

		
		if (ImGui::Button(crypt_str("Delete selected map file"))) {
			route->remove(currentRoute);
			currentRoute = -1;
			ZeroMemory(buffer, 32);
			ZeroMemory(buffer1, 32);
		}

		
		if (ImGui::Button(crypt_str("Clear Routes from map file"))) {
			if (!mapWithRoutes.empty())
				mapWithRoutes.clear();
		}

		if (!mapWithRoutes.empty()) {

			std::string text = crypt_str("Routes (") + std::to_string(mapWithRoutes.size()) + crypt_str("):");

			routelist.clear();

			for (int i = 0; i < mapWithRoutes.size(); i++) {
				std::string item = std::to_string(i) + crypt_str(" ") + mapWithRoutes[i].routename + crypt_str(" - ") + mapWithRoutes[i].makername + crypt_str("\n");
				routelist += item;
			}

			ImGui::TextWrapped(text.c_str());
			ImGui::TextWrapped(routelist.c_str());

			std::clamp(delselection, 0, (int)mapWithRoutes.size() - 1);
			ImGui::InputInt(crypt_str("Route to delete"), &delselection);

			
			if (ImGui::Button(crypt_str("Delete Route"))) {
				if (delselection >= 0 && delselection < mapWithRoutes.size()) {
					mapWithRoutes.erase(mapWithRoutes.begin() + delselection);
				}
			}

		}
	}

	ImGui::PushID(crypt_str("slider1"));
	ImGui::SliderFloat(crypt_str(""), &vars.movement.recorder.smoothing, 1.f, 100.f, crypt_str("Smoothing: %.3f"));
	ImGui::PopID();
	ImGui::PushID(crypt_str("slider2"));
	ImGui::SliderInt(crypt_str(""), &vars.movement.recorder.maxrender, 0, 2000, crypt_str("Circle Render Distance: %d"));
	ImGui::PopID();
	ImGui::PushID(crypt_str("slider3"));
	ImGui::SliderFloat(crypt_str(""), &vars.movement.recorder.ringsize, 5.f, 50.f, crypt_str("Circle size: %.1f"));
	ImGui::PopID();
#ifdef _DEBUG
	ImGui::Checkbox(crypt_str("Lock viewangles"), &vars.movement.recorder.lockva);
#endif
	FrameContainer& recording = recorder.GetActiveRecording();
	int currentFrame = playback.GetCurrentFrame();
	int maxFrames = recording.size();
	ImGui::Text(std::string(crypt_str("\tCurrent frame: ") + std::to_string(currentFrame) + crypt_str(" / ") + std::to_string(maxFrames)).c_str());
	if (recorder.IsRecordingActive())
		ImGui::Text(crypt_str("\tRecording"));
	if (playback.IsPlaybackActive())
		ImGui::Text(crypt_str("\tPlaying"));;
	ImGui::Spacing();
	ImGui::Text(crypt_str("\tKeybinds"));
	ImGui::Separator();

	vars.movement.recorder.clearrecord.imgui("");
	
	vars.movement.recorder.startplayback.imgui("");
	
	vars.movement.recorder.stopplayback.imgui("");
	
	vars.movement.recorder.startrecord.imgui("");
	
	vars.movement.recorder.stoprecord.imgui("");
	ImGui::Spacing();
	ImGui::Text(crypt_str("\tColors"));
	ImGui::Separator();
	color_picker4(crypt_str("Start Ring Color"), vars.movement.recorder.startring);
	color_picker4(crypt_str("End Ring Color"), vars.movement.recorder.endring);
	color_picker4(crypt_str("Line Color"), vars.movement.recorder.linecolor);
	color_picker4(crypt_str("Text Color"), vars.movement.recorder.textcolor);
	
	ImGui::PopFont();

#ifdef _LambdaDEV

	if (!recorder.GetActiveRecording().empty()) {

		if (ImGui::Button("Dump recording to console")) {
			auto recording = recorder.GetActiveRecording();
			memory->debugMsg("[\n");
			for (size_t i = 0; i < recording.size(); i++) {
				memory->debugMsg("\t[\n");
				std::string fm = "\t" + std::to_string(recording.at(i).forwardmove) + "\n";
				memory->debugMsg(fm.c_str());
				std::string sm = "\t" + std::to_string(recording.at(i).sidemove) + "\n";
				memory->debugMsg(sm.c_str());
				std::string um = "\t" + std::to_string(recording.at(i).upmove) + "\n";
				memory->debugMsg(um.c_str());
				std::string bt = "\t" + std::to_string(recording.at(i).buttons) + "\n";
				memory->debugMsg(bt.c_str());
				std::string mx = "\t" + std::to_string(recording.at(i).mousedx) + "\n";
				memory->debugMsg(mx.c_str());
				std::string my = "\t" + std::to_string(recording.at(i).mousedy) + "\n";
				memory->debugMsg(my.c_str());
				std::string pos = "\t" + std::to_string(recording.at(i).position.x) + " , " + std::to_string(recording.at(i).position.y) + " , " + std::to_string(recording.at(i).position.z) + "\n";
				memory->debugMsg(pos.c_str());
				std::string va = "\t" + std::to_string(recording.at(i).viewangles[0]) + " , " + std::to_string(recording.at(i).viewangles[1]) + "\n";
				memory->debugMsg(va.c_str());
			}
		}
	}
#endif
}

void movementRecorder::DrawRoutes()
{
	if (!vars.movement.recorder.enabled)
		return;

	auto AngleVectors = [](const Vector& angles, Vector* forward)
	{
		float	sp, sy, cp, cy;

		sy = sin(DEG2RAD(angles.y));
		cy = cos(DEG2RAD(angles.y));

		sp = sin(DEG2RAD(angles.x));
		cp = cos(DEG2RAD(angles.x));

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	};

	if (playback.IsPlaybackActive() || recorder.IsRecordingActive()) {
		if (recorder.GetActiveRecording().size() > 2) {
			if (interfaces.engine->IsConnected()) {
				if (csgo->local) {
					if (csgo->local->isAlive()) {

						auto& recording = recorder.GetActiveRecording();
						for (int i = 1; i < recording.size(); i++) {
							Vector from; Vector to;
							if (Math::WorldToScreen(recording.at(i).position, from) && Math::WorldToScreen(recording.at(i-1).position, to)) {
								//draw the recording line
								ImGui::GetBackgroundDrawList()->AddLine(ImVec2(from.x, from.y), ImVec2(to.x, to.y), vars.movement.recorder.linecolor.u32());
								//circle @ start
								if (i == 1) {
									
									//interfaces->surface->drawCircle(from.x, from.y, 0, 15)
									if (csgo->origin.DistTo(recording.at(i - 1).position) < 0.5f) {
										world_circle(recording.at(i-1).position, vars.movement.recorder.ringsize, color_t(0, 255, 0, 200), true);
									}
									else {
										world_circle(recording.at(i-1).position, vars.movement.recorder.ringsize, vars.movement.recorder.startring, true);
									}

								}
								//circle @ end
								else if (i == recording.size() - 1 && !recorder.IsRecordingActive()) {
									world_circle(recording.at(i).position, vars.movement.recorder.ringsize, vars.movement.recorder.endring, true);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if (!mapWithRoutes.empty()) {
			if (csgo->is_local_alive) {
				for (size_t i = 0; i < mapWithRoutes.size(); i++) {

					if (vars.movement.recorder.maxrender != 0) {
						if (csgo->origin.DistTo(mapWithRoutes.at(i).frames.at(0).position) > vars.movement.recorder.maxrender) {
							continue;
						}
					}

					if (csgo->origin.DistTo(mapWithRoutes.at(i).frames.at(0).position) < 0.5f) {
						world_circle(mapWithRoutes.at(i).frames.at(0).position, vars.movement.recorder.ringsize, color_t(0, 255, 0, 200), true);
					}
					else {
						world_circle(mapWithRoutes.at(i).frames.at(0).position, vars.movement.recorder.ringsize, vars.movement.recorder.startring, true);
					}

					Vector scrpos;
					if (Math::WorldToScreen(Vector{ mapWithRoutes.at(i).frames.at(0).position.x, mapWithRoutes.at(i).frames.at(0).position.y, mapWithRoutes.at(i).frames.at(0).position.z + 45.f }, scrpos)) {
						g_Render->DrawString(scrpos.x, scrpos.y, vars.movement.recorder.textcolor, render::centered_x | render::centered_y, fonts::esp_name, mapWithRoutes.at(i).routename.c_str());
						g_Render->DrawString(scrpos.x, scrpos.y + 17, vars.movement.recorder.textcolor, render::centered_x | render::centered_y, fonts::esp_name, mapWithRoutes.at(i).makername.c_str());
					}

					if (csgo->origin.DistTo(mapWithRoutes.at(i).frames.at(0).position) < vars.movement.recorder.ringsize)
					{
						Vector o = csgo->local->GetEyePosition();

						Vector angles = { mapWithRoutes.at(i).frames.at(0).viewangles[0], mapWithRoutes.at(i).frames.at(0).viewangles[1], 0.f };

						Vector forward;
						AngleVectors(angles, &forward);

						Vector out;
						if (Math::WorldToScreen(o + (forward * 100.f), out))
						{
							ImGui::PushFont(fonts::esp_name);
							const auto& text_size = ImGui::CalcTextSize(crypt_str("Aim here!"));
							ImGui::PopFont();
							g_Render->FilledRect(out.x - 7, out.y - (text_size.y / 2) - 2, 16 + text_size.x, 4 + text_size.y, color_t(25, 25, 25, 255), 3.f);
							g_Render->DrawString(out.x + 7, out.y - (text_size.y / 2), color_t(230, 230, 230, 255), 0, fonts::esp_name, crypt_str("Aim here!"));
							ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(out.x, out.y), 5, color_t(0, 255, 128, 255).u32());
						}

					}
				}
			}
		}
	}
}

bool goingtostart = false;
bool smoothingtostart = false;
bool wishtostart = false;

void movementRecorder::Hook(CUserCmd* cmd, Vector unpredictpos)
{
	if (!vars.movement.recorder.enabled)
		return;

	

	auto VectorAngles = [](const Vector& forward, Vector& angles)
	{
		if (forward.y == 0.0f && forward.x == 0.0f)
		{
			angles.x = (forward.z > 0.0f) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			angles.x = atan2(-forward.z, forward.Length2D()) * -180.f / PI;
			angles.y = atan2(forward.y, forward.x) * 180.f / PI;

			if (angles.y > 90)
				angles.y -= 180;
			else if (angles.y < 90)
				angles.y += 180;
			else if (angles.y == 90)
				angles.y = 0;
		}

		angles.z = 0.0f;
	};
	auto AngleVectors = [](const Vector& angles, Vector* forward)
	{
		float	sp, sy, cp, cy;

		sy = sin(DEG2RAD(angles.y));
		cy = cos(DEG2RAD(angles.y));

		sp = sin(DEG2RAD(angles.x));
		cp = cos(DEG2RAD(angles.x));

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	};
	/*
	if (!once) {
		if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
			path = pathToDocuments;
			path /= "LambdaHook\\Routes\\";
			CoTaskMemFree(pathToDocuments);
		}

		std::error_code ec;
		std::filesystem::create_directory(path, ec);
		once = true;
	}
	*/
	bool isPlaybackActive = playback.IsPlaybackActive();
	bool isRecordingActive = recorder.IsRecordingActive();

	if (csgo->is_local_alive) {

		if (vars.movement.recorder.startrecord.valid() && vars.movement.recorder.startrecord.state && !recorder.IsRecordingActive() && !playback.IsPlaybackActive())
			recorder.StartRecording();

		if (vars.movement.recorder.stoprecord.valid() && vars.movement.recorder.stoprecord.state && !playback.IsPlaybackActive())
		{
			recorder.StopRecording();
			cmd->viewangles.Normalize();
		}
			

		if (vars.movement.recorder.startplayback.valid() && vars.movement.recorder.startplayback.state && !recorder.IsRecordingActive() && !playback.IsPlaybackActive()) {
			if (mapWithRoutes.size() > 0) {

				size_t closestroute = -1;
				float bestdist = 999999;

				for (size_t i = 0; i < mapWithRoutes.size(); i++)
				{
					FrameContainer& currec = mapWithRoutes.at(i).frames;
					if (currec.at(0).position.DistTo(unpredictpos) < bestdist) {
						closestroute = i;
						bestdist = currec.at(0).position.DistTo(unpredictpos);
					}
				}

				if (closestroute == -1)
					return;

				if (mapWithRoutes.at(closestroute).frames.size() > 0) {
					if (mapWithRoutes.at(closestroute).frames.at(0).position.DistTo(unpredictpos) < vars.movement.recorder.ringsize) {
						recorder.SetRecording(mapWithRoutes.at(closestroute).frames);
						goingtostart = true;
						if (vars.movement.recorder.lockva)
							smoothingtostart = true;
						wishtostart = true;
					}
					//playback.StartPlayback(recorder.GetActiveRecording())	
				}
			}
		}

		if (goingtostart) {


			float dist = unpredictpos.DistTo(recorder.GetActiveRecording().at(0).position);

			if (dist <= 0.1f) {
				goingtostart = false;
			}

			auto curPos = unpredictpos;
			auto difference = curPos - recorder.GetActiveRecording().at(0).position;

			float cos_flt, sin_flt;
			cos_flt = cos(cmd->viewangles.y / 180.f * PI);
			sin_flt = sin(cmd->viewangles.y / 180.f * PI);
			auto velocity = Vector
			{
				difference.x * cos_flt + difference.y * sin_flt,
				difference.y * cos_flt - difference.x * sin_flt,
				difference.z
			};

			cmd->forwardmove = -velocity.x * 20.f;
			cmd->sidemove = velocity.y * 20.f;
		}


		if (smoothingtostart) {
			cmd->mousedx = 0;
			cmd->mousedy = 0;

			Vector2D targetviewangles = { recorder.GetActiveRecording().at(0).viewangles[0], recorder.GetActiveRecording().at(0).viewangles[1] };

			Vector deltaang = Vector{ targetviewangles.x - cmd->viewangles.x, targetviewangles.y - cmd->viewangles.y, 0.f };

			if (deltaang.Length() < 0.1f || !vars.movement.recorder.lockva) {
				smoothingtostart = false;
			}

			deltaang.Normalize();

			deltaang.x /= vars.movement.recorder.smoothing;
			deltaang.y /= vars.movement.recorder.smoothing;
			cmd->viewangles.x += deltaang.x;
			cmd->viewangles.y += deltaang.y;
			interfaces.engine->SetViewAngles(cmd->viewangles);
		}

		if (wishtostart) {
			if (!smoothingtostart && !goingtostart) {
				if (!recorder.GetActiveRecording().empty()) {
					if (unpredictpos.DistTo(recorder.GetActiveRecording().at(0).position) < 0.1f) {
						if ((Vector{ Vector{ recorder.GetActiveRecording().at(0).viewangles[0], recorder.GetActiveRecording().at(0).viewangles[1], 0.f } - cmd->viewangles }.Length2D() < 0.1f) || !vars.movement.recorder.lockva) {
							playback.StartPlayback(recorder.GetActiveRecording());
							wishtostart = false;
							return;
						}
					}
				}
			}
		}

		if (vars.movement.recorder.stopplayback.valid() && vars.movement.recorder.stopplayback.state && playback.IsPlaybackActive() && !recorder.IsRecordingActive())
		{
			goingtostart = false;
			smoothingtostart = false;
			cmd->viewangles = { playback.GetActiveDemo().at(playback.GetCurrentFrame()).viewangles[0], playback.GetActiveDemo().at(playback.GetCurrentFrame()).viewangles[1], 0.f };
			playback.StopPlayback();
		}

		if (isRecordingActive) {
			engine_prediction->restoreprediction(0, interfaces.prediction->Split->nCommandsPredicted);
			recorder.GetActiveRecording().push_back(Frame(cmd, csgo->local->origin()));
		}
			

		if (isPlaybackActive)
		{
			const size_t current_playback_frame = playback.GetCurrentFrame();
			try
			{
				Vector orang = cmd->viewangles;
				recorder.GetActiveRecording().at(current_playback_frame).Replay(cmd);
				Vector tomvfix = cmd->viewangles;
				if (vars.movement.recorder.lockva)
					interfaces.engine->SetViewAngles(cmd->viewangles);
				else
				{
					cmd->viewangles = orang;
					Misc::Get().FixMovement(cmd, tomvfix);
				}
					

				if (current_playback_frame + 1 == recorder.GetActiveRecording().size())
				{
					playback.StopPlayback();
				}
				else
				{
					playback.SetCurrentFrame(current_playback_frame + 1);
				}
			}
			catch (std::out_of_range)
			{
				playback.StopPlayback();
			}
		}
	}

	if (vars.movement.recorder.clearrecord.valid() && vars.movement.recorder.clearrecord.state) {
		if (!recorder.IsRecordingActive() && !playback.IsPlaybackActive()) {
			recorder.GetActiveRecording().clear();
		}
	}


	cmd->viewangles.Normalize();
}

savingroute::savingroute(const char* name) noexcept
{
	if (wchar_t pathToDocuments[MAX_PATH]; SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, pathToDocuments))) {
		path = pathToDocuments;	
		path /= crypt_str("Reflect");
		path /= name;
		std::error_code ec;
		std::filesystem::create_directory(path, ec);
		
	}
	listRoutes();
}

bool is_empty_file(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

void savingroute::load(size_t id) noexcept {

	playback.StopPlayback();

	mapWithRoutes.clear();

	Json::Value json;

	std::ifstream in(path.string() + "\\" + routes[id].c_str());

	if (in && in.good())
	{
		if (!is_empty_file(in)) {
			in >> json;
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}

	//std::vector<FullRoute> topush;
	auto& mainjson = json[crypt_str("Main")];

	size_t routeamount = 0;
	if (mainjson.isMember(crypt_str("Routes"))) routeamount = mainjson[crypt_str("Routes")].asInt();

	for (size_t i = 0; i < routeamount; i++)
	{
		FullRoute thisroute = {};
		//memset(&thisroute, 0, sizeof(thisroute));

		auto& globaljson = json[crypt_str("Globals")][i];
		if (globaljson.isMember(crypt_str("routename"))) thisroute.routename = globaljson[crypt_str("routename")].asCString();
		if (globaljson.isMember(crypt_str("creatorname"))) thisroute.makername = globaljson[crypt_str("creatorname")].asCString();
		size_t framecount = 0;
		if (globaljson.isMember(crypt_str("framecount"))) framecount = globaljson[crypt_str("framecount")].asInt();

		for (size_t j = 0; j < framecount; j++) {
			auto& framejson = json[crypt_str("frame")][i][j];

			CUserCmd curframe = CUserCmd();
			Vector pos = {};

			if (framejson.isMember(crypt_str("Viewanglex"))) curframe.viewangles.x = framejson[crypt_str("Viewanglex")].asFloat();
			if (framejson.isMember(crypt_str("Viewangley"))) curframe.viewangles.y = framejson[crypt_str("Viewangley")].asFloat();
			if (framejson.isMember(crypt_str("forwardmove"))) curframe.forwardmove = framejson[crypt_str("forwardmove")].asFloat();
			if (framejson.isMember(crypt_str("sidemove"))) curframe.sidemove = framejson[crypt_str("sidemove")].asFloat();
			if (framejson.isMember(crypt_str("upmove"))) curframe.upmove = framejson[crypt_str("upmove")].asFloat();
			if (framejson.isMember(crypt_str("buttons"))) curframe.buttons = framejson[crypt_str("buttons")].asInt();
			if (framejson.isMember(crypt_str("mousedx"))) curframe.mousedx = framejson[crypt_str("mousedx")].asInt();
			if (framejson.isMember(crypt_str("mousedy"))) curframe.mousedy = framejson[crypt_str("mousedy")].asInt();
			if (framejson.isMember(crypt_str("posx"))) pos.x = framejson[crypt_str("posx")].asFloat();
			if (framejson.isMember(crypt_str("posy"))) pos.y = framejson[crypt_str("posy")].asFloat();
			if (framejson.isMember(crypt_str("posz"))) pos.z = framejson[crypt_str("posz")].asFloat();

			thisroute.frames.push_back(Frame(&curframe, pos));
		}
		mapWithRoutes.push_back(thisroute);
	}
	curroute = id;
}

void savingroute::save(size_t id, std::string name, std::string creator) const noexcept {

	if (recorder.GetActiveRecording().size() < 1)
		return;

	FullRoute routetosave;
	routetosave.frames = recorder.GetActiveRecording();
	routetosave.makername = creator.c_str();
	routetosave.routename = name.c_str();

	mapWithRoutes.push_back(routetosave);

	Json::Value json;
	auto& mainjson = json[crypt_str("Main")];
	mainjson[crypt_str("Routes")] = mapWithRoutes.size();

	for (size_t i = 0; i < mapWithRoutes.size(); i++)
	{
		FullRoute thisroute = mapWithRoutes[i];

		auto& globaljson = json[crypt_str("Globals")][i];
		globaljson[crypt_str("routename")] = thisroute.routename;
		globaljson[crypt_str("creatorname")] = thisroute.makername;
		globaljson[crypt_str("framecount")] = thisroute.frames.size();
		for (size_t j = 0; j < thisroute.frames.size(); j++) {
			auto& framejson = json[crypt_str("frame")][i][j];

			framejson[crypt_str("Viewanglex")] = thisroute.frames.at(j).viewangles[0];
			framejson[crypt_str("Viewangley")] = thisroute.frames.at(j).viewangles[1];
			framejson[crypt_str("forwardmove")] = thisroute.frames.at(j).forwardmove;
			framejson[crypt_str("sidemove")] = thisroute.frames.at(j).sidemove;
			framejson[crypt_str("upmove")] = thisroute.frames.at(j).upmove;
			framejson[crypt_str("buttons")] = thisroute.frames.at(j).buttons;
			framejson[crypt_str("mousedx")] = thisroute.frames.at(j).mousedx;
			framejson[crypt_str("mousedy")] = thisroute.frames.at(j).mousedy;
			framejson[crypt_str("posx")] = thisroute.frames.at(j).position.x;
			framejson[crypt_str("posy")] = thisroute.frames.at(j).position.y;
			framejson[crypt_str("posz")] = thisroute.frames.at(j).position.z;
		}
	}

	std::ofstream out(path.string() + "\\" + routes[id].c_str());

	if (out && out.good())
	{
		out << json;
		curroute = id;
	}
}

void savingroute::add(const char* name) noexcept
{
	if (*name && std::find(routes.cbegin(), routes.cend(), name) == routes.cend()) {
		routes.emplace_back(name);
		if (routes.size() == 0) {
			if (std::ofstream out{ path.string() + "\\" + routes[0].c_str() }; out.good())
				out << "";
		}
		else
		{
			if (std::ofstream out{ path.string() + "\\" + routes[routes.size() - 1].c_str() }; out.good())
				out << "";
		}
	}
}

void savingroute::remove(size_t id) noexcept
{
	std::error_code ec;
	std::filesystem::remove(path.string() + "\\" + routes[id].c_str(), ec);
	routes.erase(routes.cbegin() + id);
}

void savingroute::rename(size_t item, const char* newName) noexcept
{
	std::error_code ec;
	std::filesystem::rename(path.string() + "\\" + routes[item].c_str(), path.string() + "\\" + newName, ec);
	routes[item] = newName;
}


void savingroute::listRoutes() noexcept
{
	routes.clear();

	std::error_code ec;
	std::transform(std::filesystem::directory_iterator{ path, ec },
		std::filesystem::directory_iterator{ },
		std::back_inserter(routes),
		[](const auto& entry) { return std::string{ (const char*)entry.path().filename().u8string().c_str() }; });
}



void movementRecorder::infowindow()
{
	if (!vars.movement.recorder.enabled || !ImGui::GetCurrentContext() || !vars.menu.open)
		return;

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
	ImGui::Begin(crypt_str("Recorder"), (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

	if (vars.menu.inLoad) {
		//ImGui::SetWindowPos(ImVec2(vars.visuals.keybindx * csgo->w, vars.visuals.keybindy * csgo->h), ImGuiCond_Always);
		ImGui::SetWindowPos(ImVec2(std::clamp(vars.movement.recorder.infowindowx * csgo->w, 0.f, (float)csgo->w), std::clamp(vars.movement.recorder.infowindowy * csgo->h, 0.f, (float)csgo->h)), ImGuiCond_Always);
		vars.menu.inLoad = false;
	}

	vars.menu.crecorderx = ImGui::GetWindowPos().x / csgo->w;
	vars.menu.crecordery = ImGui::GetWindowPos().y / csgo->h;

	Draw();


	ImGui::PopFont();
	ImGui::PopStyleColor(16);
	ImGui::End();
}

