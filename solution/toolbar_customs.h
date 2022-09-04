#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#include <string>

namespace ToolBarE
{
	bool Item(float width, int style1, std::string nm, std::string nmsecrt, int pos1_, int pos2_, int pos3_, int pos4_, ImColor color20, ImColor color21, ImColor color22, ImColor color23, int think, int segment, int rounding, int rad);
	bool BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
	void EndChild();
	bool BeginChild(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags);
	bool BeginChild(ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags);
}

