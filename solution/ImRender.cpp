#include "render.h"
#include "Hooks.h"

ImGuiRendering* g_Render = new ImGuiRendering();

void __stdcall ImGuiRendering::CreateObjects(IDirect3DDevice9* pDevice)
{
	//if (csgo->Init.Window)
	ImGui_ImplDX9_CreateDeviceObjects();
	//_drawList = ImGui::GetBackgroundDrawList();
}

void __stdcall ImGuiRendering::InvalidateObjects()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	//_drawList = nullptr;
}

void __stdcall ImGuiRendering::SetupPresent(IDirect3DDevice9* device)
{
	//_drawList = ImGui::GetBackgroundDrawList();
}

void __stdcall ImGuiRendering::EndPresent(IDirect3DDevice9* device)
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void __stdcall ImGuiRendering::PreRender(IDirect3DDevice9* device)
{
	_drawList = ImGui::GetBackgroundDrawList();
	m_pDevice = device;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	_drawData.Valid = false;
	_drawList->_ResetForNewFrame();
	_drawList->PushClipRectFullScreen();
}

void __stdcall ImGuiRendering::PostRender(IDirect3DDevice9* device)
{
	_drawList = ImGui::GetBackgroundDrawList();
	if (!_drawList->VtxBuffer.empty()) {
		_drawData.Valid = true;
		_drawData.CmdLists = &_drawList;
		_drawData.CmdListsCount = 1;
		_drawData.TotalVtxCount = _drawList->VtxBuffer.Size;
		_drawData.TotalIdxCount = _drawList->IdxBuffer.Size;
	}

	ImGui_ImplDX9_RenderDrawData(&_drawData);
}

void ImGuiRendering::DrawString(float x, float y, color_t color, int flags, ImFont* font, const char* message, ...)
{
	_drawList = ImGui::GetBackgroundDrawList();


	char output[4096] = {};
	va_list args;
	va_start(args, message);
	vsprintf(output, message, args);
	va_end(args);


	_drawList->PushTextureID(font->ContainerAtlas->TexID);
	ImGui::PushFont(font);
	auto coord = ImVec2(x, y);
	auto size = ImGui::CalcTextSize(output);
	auto coord_out = ImVec2{ coord.x + 1.f, coord.y + 1.f };
	color_t outline_clr = color_t(0, 0, 0, color.get_alpha() * 0.6f);

	int width = 0, height = 0;

	if (!(flags & render::centered_x))
		size.x = 0;
	if (!(flags & render::centered_y))
		size.y = 0;

	ImVec2 pos = ImVec2(coord.x - (size.x * .5), coord.y - (size.y * .5));

	if (flags & render::outline)
	{
		_drawList->AddText(ImVec2(pos.x + 1, pos.y - 1), outline_clr.u32(), output);
		_drawList->AddText(ImVec2(pos.x - 1, pos.y + 1), outline_clr.u32(), output);
		_drawList->AddText(ImVec2(pos.x - 1, pos.y - 1), outline_clr.u32(), output);
		_drawList->AddText(ImVec2(pos.x + 1, pos.y + 1), outline_clr.u32(), output);

		_drawList->AddText(ImVec2(pos.x, pos.y + 1), outline_clr.u32(), output);
		_drawList->AddText(ImVec2(pos.x, pos.y - 1), outline_clr.u32(), output);
		_drawList->AddText(ImVec2(pos.x + 1, pos.y), outline_clr.u32(), output);
		_drawList->AddText(ImVec2(pos.x - 1, pos.y), outline_clr.u32(), output);
	}

	_drawList->AddText(pos, color.u32(), output);
	ImGui::PopFont();
}

void ImGuiRendering::DrawEspBox(Vector leftUpCorn, Vector rightDownCorn, color_t clr, float width)
{
	_drawList = ImGui::GetBackgroundDrawList();
	ImVec2 min = ImVec2(leftUpCorn.x, leftUpCorn.y);
	ImVec2 max = ImVec2(rightDownCorn.x, rightDownCorn.y);
	_drawList->AddRect(min, max, clr.u32(), 0.0F, -1, width);
}

void ImGuiRendering::DrawLine(float x1, float y1, float x2, float y2, color_t clr, float thickness)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), clr.u32(), thickness);
}

void ImGuiRendering::DrawLineGradient(float x1, float y1, float x2, float y2, color_t clr1, color_t cl2, float thickness)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddRectFilledMultiColor(ImVec2(x1, y2), ImVec2(x2 + thickness, y2 + thickness),
		clr1.u32(), cl2.u32(), cl2.u32(), clr1.u32());
}

void ImGuiRendering::Rect(float x, float y, float w, float h, color_t clr, float rounding)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), clr.u32(), rounding);
}

void ImGuiRendering::FilledRect(float x, float y, float w, float h, color_t clr, float rounding, ImDrawCornerFlags rounding_corners)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), clr.u32(), rounding, rounding_corners);
}
void ImGuiRendering::filled_rect_gradient(float x, float y, float w, float h, color_t col_upr_left,
	color_t col_upr_right, color_t col_bot_right, color_t col_bot_left)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h),
		col_upr_left.u32(), col_upr_right.u32(), col_bot_right.u32(), col_bot_left.u32());
}
void ImGuiRendering::Arc(float x, float y, float radius, float min_angle, float max_angle, color_t col, float thickness) {
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->PathArcTo(ImVec2(x, y), radius, DEG2RAD(min_angle), DEG2RAD(max_angle), 32);
	_drawList->PathStroke(col.u32(), true, thickness);
}

void ImGuiRendering::Triangle(float x1, float y1, float x2, float y2, float x3, float y3, color_t clr, float thickness)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr.u32(), thickness);
}

void ImGuiRendering::TriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, color_t clr)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), clr.u32());
}

void ImGuiRendering::Circle(float x1, float y1, float radius, color_t col, int segments, int thickness)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddCircle(ImVec2(x1, y1), radius, col.u32(), segments, thickness);
}

void ImGuiRendering::CircleFilled(float x1, float y1, float radius, color_t col, int segments)
{
	_drawList = ImGui::GetBackgroundDrawList();
	_drawList->AddCircleFilled(ImVec2(x1, y1), radius, col.u32(), segments);
}

void ImGuiRendering::circle_filled_rainbow(IDirect3DDevice9* dev, Vector2D center, float radius, bool rotate, float speed)
{
	_drawList = ImGui::GetBackgroundDrawList();
	static float base_hue = 0.f;
	if (rotate)
	{
		if (base_hue >= 1.f)
			base_hue = 0.f;

		base_hue += interfaces.global_vars->frametime * speed;
	}

	vertex vert[128 + 2] = {};

	float new_hue = base_hue;
	for (auto i = 1; i <= 128; i++)
	{
		if (new_hue >= 1.f)
			new_hue = 0.f;

		new_hue += 1.f / 128.f;
		new_hue = std::clamp(new_hue, 0.f, 1.f);

		auto temp = color_t::hsb(new_hue, 1.f, 1.f);
		auto col = D3DCOLOR_RGBA(temp.get_red(), temp.get_green(), temp.get_blue(), (int)vars.visuals.innacuracyoverlay_alpha);

		vert[i] =
		{
			center.x + radius * cos(D3DX_PI * (i / (128 / 2.0f))),
			center.y + radius * sin(D3DX_PI * (i / (128 / 2.0f))),
			0.0f,
			1.0f,
			col
		};
	}

	vert[0] = { center.x, center.y, 0.0f, 1.0f, D3DCOLOR_RGBA(100, 100, 100, 10) };
	vert[128 + 1] = vert[1];

	dev->SetTexture(0, NULL);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 128, &vert, sizeof(vertex));
}

void ImGuiRendering::build_lookup_table()
{
	if (!lookup_table.empty())
		return;

	for (auto i = 0; i <= points; i++)
		lookup_table.emplace_back(
			std::cos(2.f * D3DX_PI * (i / static_cast<float>(points))),
			std::sin(2.f * D3DX_PI * (i / static_cast<float>(points)))
		);
}

void ImGuiRendering::circle_filled_radial_gradient(IDirect3DDevice9* dev, Vector2D center, float radius, color_t color1, color_t color2)
{
	const auto col1 = color1.u32();
	const auto col2 = color2.u32();
	build_lookup_table();

	vertex vert[points + 2] = {};

	for (auto i = 1; i <= points; i++)
		vert[i] =
	{
		center.x + radius * lookup_table[i].x,
		center.y - radius * lookup_table[i].y,
		0.0f,
		1.0f,
		col1
	};

	vert[0] = { center.x, center.y, 0.0f, 1.0f, col2 };
	vert[points + 1] = vert[1];

	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, true);
	dev->SetTexture(0, nullptr);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points, &vert, sizeof vertex);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, false);
}