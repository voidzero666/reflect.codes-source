#pragma once
#include <string>
#include <time.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <vector>
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx9.h"
#include "ImGui\imgui_impl_win32.h"
#include "imgui\imgui_freetype.h"
#include <vector>
#include "Vector.h"
#include "colors.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

struct Vertex_t;
class Vector;
class Vector2D;

struct vertex
{
	float x, y, z, rhw;
	uint32_t color;
};



namespace fonts
{
	extern ImFont* esp_name;
	extern ImFont* esp_info;
	extern ImFont* esp_logs;
	extern ImFont* graph_font;
	extern ImFont* lby_indicator;
	extern ImFont* damage;
	extern ImFont* menu_main;
	extern ImFont* menu_desc;
	extern ImFont* logger;
	extern ImFont* logo;
	extern ImFont* tab_names;
	extern ImFont* font;
	extern ImFont* hitmarker;
	extern ImFont* move_indicator;
	extern ImFont* move_indicator_small;
	extern ImFont* csgoicons;
	extern ImFont* csgoicons_large;
}



namespace render {
	enum e_textflags {
		none = 0,
		outline = 1 << 0,
		centered_x = 1 << 1,
		centered_y = 1 << 2,
	};
}
class ImGuiRendering {
public:


	void __stdcall CreateObjects(IDirect3DDevice9* pDevice);
	void __stdcall InvalidateObjects();
	void __stdcall PreRender(IDirect3DDevice9* device);
	void __stdcall PostRender(IDirect3DDevice9* deivce);
	void __stdcall EndPresent(IDirect3DDevice9* device);
	void __stdcall SetupPresent(IDirect3DDevice9* device);

	void DrawEspBox(Vector leftUpCorn, Vector rightDownCorn, color_t clr, float width);
	void DrawLine(float x1, float y1, float x2, float y2, color_t clr, float thickness = 1.f);
	void DrawLineGradient(float x1, float y1, float x2, float y2, color_t clr1, color_t cl2, float thickness = 1.f);
	void Rect(float x, float y, float w, float h, color_t clr, float rounding = 0.f);
	void FilledRect(float x, float y, float w, float h, color_t clr, float rounding = 0.f, ImDrawCornerFlags rounding_corners = 15);
	void Triangle(float x1, float y1, float x2, float y2, float x3, float y3, color_t clr, float thickness = 1.f);
	void TriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3, color_t clr);
	void Circle(float x1, float y1, float radius, color_t col, int segments, int thickness);
	void CircleFilled(float x1, float y1, float radius, color_t col, int segments);
	void DrawString(float x, float y, color_t color, int flags, ImFont* font, const char* message, ...);
	void circle_filled_radial_gradient(IDirect3DDevice9* dev, Vector2D center, float radius, color_t color1, color_t color2);
	void circle_filled_rainbow(IDirect3DDevice9* dev, Vector2D center, float radius, bool rotate, float speed);
	void filled_rect_gradient(float x, float y, float w, float h, color_t col_upr_left,
		color_t col_upr_right, color_t col_bot_right, color_t col_bot_left);
	void Arc(float x, float y, float radius, float min_angle, float max_angle, color_t col, float thickness = 1.f);
	void build_lookup_table();
	IDirect3DDevice9* GetDevice() {
		return m_pDevice;
	}
	ImDrawList* _drawList;
private:
	IDirect3DDevice9* m_pDevice;
	static constexpr auto points = 64;
	std::vector<Vector2D> lookup_table;
	ImDrawData _drawData;
	
	IDirect3DTexture9* _texture;
	ImFontAtlas _fonts;
	DWORD dwOld_D3DRS_COLORWRITEENABLE;
}; extern ImGuiRendering* g_Render;

namespace Drawing
{
	extern void LimitDrawingArea(int x, int y, int w, int h);
	extern void GetDrawingArea(int& x, int& y, int& w, int& h);
	extern void DrawString(unsigned long font, int x, int y, color_t color, DWORD alignment, const char* msg, ...);
	extern void DrawStringFont(unsigned long font, int x, int y, color_t clrcolor_t, bool bCenter, const char* szText, ...);
	extern void DrawStringUnicode(unsigned long font, int x, int y, color_t color, bool bCenter, const wchar_t* msg, ...);
	extern void DrawRect(int x, int y, int w, int h, color_t col);
	extern void Rectangle(float x, float y, float w, float h, float px, color_t col);
	extern void Border(int x, int y, int w, int h, int line, color_t col);
	extern void DrawRectRainbow(int x, int y, int w, int h, float flSpeed, float& flRainbow);
	extern void DrawRectGradientVertical(int x, int y, int w, int h, color_t color1, color_t color2);
	extern void DrawRectGradientHorizontal(int x, int y, int w, int h, color_t color1, color_t color2);
	extern void DrawPixel(int x, int y, color_t col);
	extern void DrawOutlinedRect(int x, int y, int w, int h, color_t col);
	extern void DrawOutlinedCircle(int x, int y, int r, color_t col);
	extern void DrawLine(int x0, int y0, int x1, int y1, color_t col);
	extern void DrawCorner(int iX, int iY, int iWidth, int iHeight, bool bRight, bool bDown, color_t colDraw);
	extern void DrawRoundedBox(int x, int y, int w, int h, int r, int v, color_t col);
	extern void Triangle(Vector ldcorner, Vector rucorner, color_t col);
	extern void DrawPolygon(int count, Vertex_t* Vertexs, color_t color);
	extern void DrawBox(int x, int y, int w, int h, color_t color);
	extern RECT GetViewport();
	extern int	GetStringWidth(unsigned long font, const char* msg, ...);
	extern RECT GetTextSize(unsigned long font, const char* text);
	extern void Draw3DBox(Vector* boxVectors, color_t color);
	extern void rotate_point(Vector2D& point, Vector2D origin, bool clockwise, float angle);
	extern void DrawFilledCircle(int x, int y, int radius, int segments, color_t color);
	extern void TexturedPolygon(int n, std::vector<Vertex_t> vertice, color_t color);
	extern void filled_tilted_triangle(Vector2D position, Vector2D size, Vector2D origin, bool clockwise, float angle, color_t color, bool rotate = true);
	extern void DrawCircle(float x, float y, float r, float s, color_t color);
}
