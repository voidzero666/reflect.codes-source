#include "toolbar_customs.h"

namespace ToolBarE
{
	bool Item(float width, int style1, std::string nm, std::string nmsecrt,
		int pos1_, int pos2_, int pos3_, int pos4_,
		ImColor color20, ImColor color21, ImColor color22, ImColor color23,
		int think, int segment, int rounding, int rad)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(nmsecrt.c_str());
		const ImVec2 label_size = ImGui::CalcTextSize(nm.c_str(), NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize({ width,  30 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
		const ImRect bb(pos, { pos.x + width, pos.y + 40 });

		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		ImGui::RenderFrame({ bb.Min.x + 5, bb.Min.y }, { bb.Max.x - 5, bb.Max.y }, ImColor(45, 45, 45), true, 2);

		window->DrawList->AddRect({ bb.Min.x + 5, bb.Min.y }, { bb.Max.x - 5, bb.Max.y }, ImColor(20, 20, 20));


		if (style1 == 3)
		{
			window->DrawList->AddRectFilledMultiColor({ bb.Min.x + 10, bb.Min.y + 5 }, { bb.Min.x + 40, bb.Max.y - 5 }, color20, color21, color22, color23);
		}
		else
		{
			window->DrawList->AddRectFilled({ bb.Min.x + 10, bb.Min.y + 5 }, { bb.Min.x + 40, bb.Max.y - 5 }, color20);
		}

		window->DrawList->AddRect({ bb.Min.x + 10, bb.Min.y + 5 }, { bb.Min.x + 40, bb.Max.y - 5 }, ImColor(20, 20, 20));

		nm.append(" ("); nm.append(std::to_string(pos1_));  nm.append(":");  nm.append(std::to_string(pos2_));

		if (style1 != 4 && style1 != 5)
		{
			nm.append(" -> "); nm.append(std::to_string(pos3_));  nm.append(":");  nm.append(std::to_string(pos4_));
		}

		nm.append(")");

		if (label_size.x > 0.0f)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 15 - ImGui::CalcTextSize(nm.c_str()).y / 2 - 1), ImColor(240 / 255.f, 240 / 255.f, 240 / 255.f, 200 / 255.f), nm.c_str());

		if (style1 == 0)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 15 - ImGui::CalcTextSize(nm.c_str()).y / 2 + 12), ImColor(240 / 255.f, 240 / 255.f, 240 / 255.f, 200 / 255.f), "Line");

		if (style1 == 1)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 15 - ImGui::CalcTextSize(nm.c_str()).y / 2 + 12), ImColor(240 / 255.f, 240 / 255.f, 240 / 255.f, 200 / 255.f), "Rect");

		if (style1 == 2)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 15 - ImGui::CalcTextSize(nm.c_str()).y / 2 + 12), ImColor(240 / 255.f, 240 / 255.f, 240 / 255.f, 200 / 255.f), "Rect Filled");

		if (style1 == 3)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 15 - ImGui::CalcTextSize(nm.c_str()).y / 2 + 12), ImColor(240 / 255.f, 240 / 255.f, 240 / 255.f, 200 / 255.f), "Rect Filled Multicolor");

		if (style1 == 4)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 15 - ImGui::CalcTextSize(nm.c_str()).y / 2 + 12), ImColor(240 / 255.f, 240 / 255.f, 240 / 255.f, 200 / 255.f), "Circle");

		if (style1 == 5)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 15 - ImGui::CalcTextSize(nm.c_str()).y / 2 + 12), ImColor(240 / 255.f, 240 / 255.f, 240 / 255.f, 200 / 255.f), "Circle Filled");

		return pressed;
	}


	bool BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* parent_window = g.CurrentWindow;

		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
		flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

		// Size
		const ImVec2 content_avail = ImGui::GetContentRegionAvail();
		ImVec2 size = ImFloor(size_arg);
		const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
		if (size.x <= 0.0f)
			size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
		if (size.y <= 0.0f)
			size.y = ImMax(content_avail.y + size.y, 4.0f);
		ImGui::SetNextWindowSize(size);

		// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
		char title[256];
		if (name)
			ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
		else
			ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

		const float backup_border_size = g.Style.ChildBorderSize;
		if (!border)
			g.Style.ChildBorderSize = 0.0f;

		bool ret = ImGui::Begin(title, NULL, flags);
		g.Style.ChildBorderSize = backup_border_size;

		ImGuiWindow* child_window = g.CurrentWindow;
		child_window->ChildId = id;
		child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

		// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
		// While this is not really documented/defined, it seems that the expected thing to do.
		if (child_window->BeginCount == 1)
			parent_window->DC.CursorPos = child_window->Pos;

		// Process navigation-in immediately so NavInit can run on first frame
		if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavHasScroll))
		{
			ImGui::FocusWindow(child_window);
			ImGui::NavInitWindow(child_window, false);
			ImGui::SetActiveID(id + 1, child_window); // Steal ActiveId with a dummy id so that key-press won't activate child item
			g.ActiveIdSource = ImGuiInputSource_Nav;
		}

		parent_window->DrawList->AddRectFilled({ ImGui::GetWindowPos().x,ImGui::GetWindowPos().y - 4 }, { ImGui::GetWindowPos().x + size_arg.x,ImGui::GetWindowPos().y + size_arg.y }, ImColor(50, 50, 50));

		return ret;
	}

	void EndChild()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		IM_ASSERT(g.WithinEndChild == false);
		IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() calls

		g.WithinEndChild = true;
		if (window->BeginCount > 1)
		{
			ImGui::End();
		}
		else
		{
			ImVec2 sz = window->Size;
			if (window->AutoFitChildAxises & (1 << ImGuiAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
				sz.x = ImMax(4.0f, sz.x);
			if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
				sz.y = ImMax(4.0f, sz.y);
			ImGui::End();

			ImGuiWindow* parent_window = g.CurrentWindow;
			ImRect bb(parent_window->DC.CursorPos, { parent_window->DC.CursorPos.x + sz.x ,parent_window->DC.CursorPos.y + sz.y });
			ImGui::ItemSize(sz);
			if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
			{
				ImGui::ItemAdd(bb, window->ChildId);
				ImGui::RenderNavHighlight(bb, window->ChildId);

				// When browsing a window that has no activable items (scroll only) we keep a highlight on the child
				if (window->DC.NavLayersActiveMask == 0 && window == g.NavWindow)
					ImGui::RenderNavHighlight(ImRect(ImVec2(bb.Min.x - 2, bb.Min.y - 2), ImVec2(bb.Max.x + 2, bb.Max.y + 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
			}
			else
			{
				// Not navigable into
				ImGui::ItemAdd(bb, 0);
			}
		}
		g.WithinEndChild = false;
	}


	bool BeginChild(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		return BeginChildEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
	}

	bool BeginChild(ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
	{
		IM_ASSERT(id != 0);
		return BeginChildEx(NULL, id, size_arg, border, extra_flags);
	}


}

