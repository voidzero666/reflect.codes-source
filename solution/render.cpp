#include "render.h"
#include <math.h>
#include <vector>
#include <fstream>
#include "Vector.h"
#include "Hooks.h"
#include "imgui/imgui.h"

bool file_exists(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

namespace fonts
{
	ImFont* esp_name = nullptr;
	ImFont* esp_info = nullptr;
	ImFont* esp_logs = nullptr;
	ImFont* graph_font = nullptr;
	ImFont* lby_indicator = nullptr;
	ImFont* damage = nullptr;
	ImFont* menu_main = nullptr;
	ImFont* menu_desc = nullptr;
	ImFont* very_small = nullptr;
	ImFont* logger = nullptr;
	ImFont* logo = nullptr;
	ImFont* tab_names = nullptr;
	ImFont* font = nullptr;
	ImFont* hitmarker = nullptr;
	ImFont* move_indicator = nullptr;
	ImFont* move_indicator_small = nullptr;
	ImFont* csgoicons = nullptr;
	ImFont* csgoicons_large = nullptr;
}

void Drawing::LimitDrawingArea(int x, int y, int w, int h) {
	typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
	getvfunc<OriginalFn>(interfaces.surfaces, 147)(interfaces.surfaces, x, y, x + w + 1, y + h + 1);
}

void Drawing::GetDrawingArea(int& x, int& y, int& w, int& h) {
	typedef void(__thiscall* OriginalFn)(void*, int&, int&, int&, int&);
	getvfunc<OriginalFn>(interfaces.surfaces, 146)(interfaces.surfaces, x, y, w, h);
}

void Drawing::DrawString(unsigned long font, int x, int y, color_t color, DWORD alignment, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int width, height;
	interfaces.surfaces->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	interfaces.surfaces->DrawSetTextFont(font);

	if (width > 65536)
		return;

	interfaces.surfaces->DrawSetTextColor(color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());
	interfaces.surfaces->DrawSetTextPos(x, y /*- height / 2*/);
	interfaces.surfaces->DrawPrintText(wbuf, wcslen(wbuf));
}

void Drawing::DrawStringFont(unsigned long font, int x, int y, color_t clrcolor_t, bool bCenter, const char* szText, ...)
{
	if (!szText)
		return;

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, szText);
	_vsnprintf_s(buf, sizeof(buf), szText, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	if (bCenter)
	{
		int Wide = 0, Tall = 0;

		interfaces.surfaces->GetTextSize(font, wbuf, Wide, Tall);

		x -= Wide / 2;
	}

	interfaces.surfaces->DrawSetTextFont(font);
	interfaces.surfaces->DrawSetTextPos(x, y);
	interfaces.surfaces->DrawSetTextColor(clrcolor_t.get_red(), clrcolor_t.get_green(), clrcolor_t.get_blue(), clrcolor_t.get_alpha());
	interfaces.surfaces->DrawPrintText(wbuf, wcslen(wbuf));
}

void Drawing::DrawStringUnicode(unsigned long font, int x, int y, color_t color, bool bCenter, const wchar_t* msg, ...)
{
	int r = 255, g = 255, b = 255, a = 255;
	color = color_t(r, g, b, a);

	int iWidth, iHeight;

	interfaces.surfaces->GetTextSize(font, msg, iWidth, iHeight);
	interfaces.surfaces->DrawSetTextFont(font);
	interfaces.surfaces->DrawSetTextColor(r, g, b, a);
	interfaces.surfaces->DrawSetTextPos(!bCenter ? x : x - iWidth / 2, y - iHeight / 2);
	interfaces.surfaces->DrawPrintText(msg, wcslen(msg));
}

void Drawing::DrawRect(int x, int y, int w, int h, color_t col)
{
	interfaces.surfaces->DrawSetcolor_t(col.get_red(), col.get_green(), col.get_blue(), col.get_alpha());
	interfaces.surfaces->DrawFilledRect(x, y, x + w, y + h);
}

void Drawing::Rectangle(float x, float y, float w, float h, float px, color_t col)
{
	DrawRect(x, (y + h - px), w, px, col);
	DrawRect(x, y, px, h, col);
	DrawRect(x, y, w, px, col);
	DrawRect((x + w - px), y, px, h, col);
}

void Drawing::Border(int x, int y, int w, int h, int line, color_t col)
{
	DrawRect(x, y, w, line, col);
	DrawRect(x, y, line, h, col);
	DrawRect((x + w), y, line, h, col);
	DrawRect(x, (y + h), (w + line), line, col);
}

void Drawing::DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float& flRainbow)
{
	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		color_t colRainbow = color_t::hsb(hue, 1.f, 1.f);
		Drawing::DrawRect(x + i, y, 1, height, colRainbow);
	}
}

void Drawing::DrawRectGradientVertical(int x, int y, int width, int height, color_t color1, color_t color2)
{
	float flDifferenceR = (float)(color2.get_red() - color1.get_red()) / (float)height;
	float flDifferenceG = (float)(color2.get_green() - color1.get_green()) / (float)height;
	float flDifferenceB = (float)(color2.get_blue() - color1.get_blue()) / (float)height;

	for (float i = 0.f; i < height; i++)
	{
		color_t colGradient = color_t(color1.get_red() + (flDifferenceR * i), color1.get_green() + (flDifferenceG * i),
			color1.get_blue() + (flDifferenceB * i), color1.get_alpha());
		Drawing::DrawRect(x, y + i, width, 1, colGradient);
	}
}

void Drawing::DrawRectGradientHorizontal(int x, int y, int width, int height, color_t color1, color_t color2)
{
	float flDifferenceR = (float)(color2.get_red() - color1.get_red()) / (float)width;
	float flDifferenceG = (float)(color2.get_green() - color1.get_green()) / (float)width;
	float flDifferenceB = (float)(color2.get_blue() - color1.get_blue()) / (float)width;

	for (float i = 0.f; i < width; i++)
	{
		color_t colGradient = color_t(color1.get_red() + (flDifferenceR * i), color1.get_green() + (flDifferenceG * i), color1.get_blue() + (flDifferenceB * i), color1.get_alpha());
		Drawing::DrawRect(x + i, y, 1, height, colGradient);
	}
}

void Drawing::DrawPixel(int x, int y, color_t col)
{
	interfaces.surfaces->DrawSetcolor_t(col.get_red(), col.get_green(), col.get_blue(), col.get_alpha());
	interfaces.surfaces->DrawFilledRect(x, y, x + 1, y + 1);
}

void Drawing::DrawOutlinedRect(int x, int y, int w, int h, color_t col)
{
	interfaces.surfaces->DrawSetcolor_t(col.get_red(), col.get_green(), col.get_blue(), col.get_alpha());
	interfaces.surfaces->DrawOutlinedRect(x, y, x + w, y + h);
}

void Drawing::DrawOutlinedCircle(int x, int y, int r, color_t col)
{
	interfaces.surfaces->DrawSetcolor_t(col.get_red(), col.get_green(), col.get_blue(), col.get_alpha());
	interfaces.surfaces->DrawOutlinedCircle(x, y, r, 1);
}

void Drawing::DrawLine(int x0, int y0, int x1, int y1, color_t col)
{
	if (x0 == INT_MAX
		|| y0 == INT_MAX
		|| x1 == INT_MAX
		|| y1 == INT_MAX)
		return;

	interfaces.surfaces->DrawSetcolor_t(col.get_red(), col.get_green(), col.get_blue(), col.get_alpha());
	interfaces.surfaces->DrawLine(x0, y0, x1, y1);
}

void Drawing::DrawCorner(int iX, int iY, int iWidth, int iHeight, bool bRight, bool bDown, color_t colDraw)
{
	int iRealX = bRight ? iX - iWidth : iX;
	int iRealY = bDown ? iY - iHeight : iY;

	if (bDown && bRight)
		iWidth = iWidth + 1;

	Drawing::DrawRect(iRealX, iY, iWidth, 1, colDraw);
	Drawing::DrawRect(iX, iRealY, 1, iHeight, colDraw);

	Drawing::DrawRect(iRealX, bDown ? iY + 1 : iY - 1, !bDown && bRight ? iWidth + 1 : iWidth, 1, color_t(0, 0, 0, 255));
	Drawing::DrawRect(bRight ? iX + 1 : iX - 1, bDown ? iRealY : iRealY - 1, 1, bDown ? iHeight + 2 : iHeight + 1, color_t(0, 0, 0, 255));
}
//
//#define COL2DWORD(x) (D3DCOLOR_ARGB(x.alpha, x.red, x.green, x.blue))
//void Drawing::Triangle(Vector2D pos1, Vector2D pos2, Vector2D pos3, color_t color) const
//{
//	const auto dwcolor_t = COL2DWORD(color);
//	Vertex vert[4] =
//	{
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwcolor_t },
//		{ pos2.x, pos2.y, 0.0f, 1.0f, dwcolor_t },
//		{ pos3.x, pos3.y, 0.0f, 1.0f, dwcolor_t },
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwcolor_t }
//	};
//
//	this->pDevice->SetTexture(0, nullptr);
//	this->pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(Vertex));
//}
//
//
//void Drawing::TriangleFilled(Vector2D pos1, Vector2D pos2, Vector2D pos3, color_t color) const
//{
//	const auto dwcolor_t = COL2DWORD(color);
//	Vertex vert[3] =
//	{
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwcolor_t },
//		{ pos2.x, pos2.y, 0.0f, 1.0f, dwcolor_t },
//		{ pos3.x, pos3.y, 0.0f, 1.0f, dwcolor_t }
//	};
//
//	this->pDevice->SetTexture(0, nullptr);
//	this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(Vertex));
//}

void Drawing::Triangle(Vector ldcorner, Vector rucorner, color_t col)
{
	DrawLine(ldcorner.x, ldcorner.y, (rucorner.x / 2) - 1, rucorner.y, col); // left shit

	DrawLine(rucorner.x, ldcorner.y, (rucorner.x / 2) - 1, rucorner.y, col); // right shit

	DrawLine(ldcorner.x, ldcorner.y, rucorner.x, ldcorner.y/*(ldcorner.x - rucorner.x), (ldcorner.y - rucorner.y)*/, col); // down shit
}

void Drawing::DrawPolygon(int count, Vertex_t* Vertexs, color_t color)
{
	static int Texture = interfaces.surfaces->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	interfaces.surfaces->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	interfaces.surfaces->DrawSetcolor_t(color.get_red(), color.get_green(), color.get_blue(), color.get_alpha());
	interfaces.surfaces->DrawSetTexture(Texture);

	interfaces.surfaces->DrawTexturedPolygon(count, Vertexs);
}

void Drawing::DrawBox(int x, int y, int w, int h, color_t color)
{
	// top
	DrawRect(x, y, w, 1, color);
	// right
	DrawRect(x, y + 1, 1, h - 1, color);
	// left
	DrawRect(x + w - 1, y + 1, 1, h - 1, color);
	// bottom
	DrawRect(x, y + h - 1, w - 1, 1, color);
}

void Drawing::DrawRoundedBox(int x, int y, int w, int h, int r, int v, color_t col)
{
	std::vector<Vertex_t> p;
	for (int _i = 0; _i < 3; _i++)
	{
		int _x = x + (_i < 2 && r || w - r);
		int _y = y + (_i % 3 > 0 && r || h - r);
		for (int i = 0; i < v; i++)
		{
			int a = RAD2DEG((i / v) * -90 - _i * 90);
			p.push_back(Vertex_t(Vector2D(_x + sin(a) * r, _y + cos(a) * r)));
		}
	}

	Drawing::DrawPolygon(4 * (v + 1), &p[0], col);
	/*
	function DrawRoundedBox(x, y, w, h, r, v, col)
	local p = {};
	for _i = 0, 3 do
	local _x = x + (_i < 2 && r || w - r)
	local _y = y + (_i%3 > 0 && r || h - r)
	for i = 0, v do
	local a = math.rad((i / v) * - 90 - _i * 90)
	table.insert(p, {x = _x + math.sin(a) * r, y = _y + math.cos(a) * r})
	end
	end

	surface.SetDrawcolor_t(col.r, col.g, col.b, 255)
	draw.NoTexture()
	surface.DrawPoly(p)
	end
	*/

	// Notes: amount of vertexes is 4(v + 1) where v is the number of vertices on each corner bit.
	// I did it in lua cause I have no idea how the vertex_t struct works and i'm still aids at C++
}

RECT Drawing::GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	Viewport.right = csgo->w; Viewport.bottom = csgo->h;
	return Viewport;
}

int Drawing::GetStringWidth(unsigned long font, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int iWidth, iHeight;

	interfaces.surfaces->GetTextSize(font, wbuf, iWidth, iHeight);

	return iWidth;
}

RECT Drawing::GetTextSize(unsigned long font, const char* text)
{
	/*size_t origsize = strlen(text) + 1;
	const size_t newsize = 500;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);*/

	RECT rect;

	rect.left = rect.right = rect.bottom = rect.top = 0;

	wchar_t wbuf[1024];
	if (MultiByteToWideChar(CP_UTF8, 0, text, -1, wbuf, 256) > 0) {
		int x, y;
		interfaces.surfaces->GetTextSize(font, wbuf, x, y);
		rect.left = x; rect.bottom = y;
		rect.right = x; rect.top = y;
	}

	return rect;
}

void Drawing::Draw3DBox(Vector* boxVectors, color_t color)
{
	Vector boxVectors0, boxVectors1, boxVectors2, boxVectors3,
		boxVectors4, boxVectors5, boxVectors6, boxVectors7;

	if (Math::WorldToScreen(boxVectors[0], boxVectors0) &&
		Math::WorldToScreen(boxVectors[1], boxVectors1) &&
		Math::WorldToScreen(boxVectors[2], boxVectors2) &&
		Math::WorldToScreen(boxVectors[3], boxVectors3) &&
		Math::WorldToScreen(boxVectors[4], boxVectors4) &&
		Math::WorldToScreen(boxVectors[5], boxVectors5) &&
		Math::WorldToScreen(boxVectors[6], boxVectors6) &&
		Math::WorldToScreen(boxVectors[7], boxVectors7))
	{

		/*
		.+--5---+
		.8 4    6'|
		+--7---+'  11
		9   |  10  |
		|  ,+--|3--+
		|.0    | 2'
		+--1---+'
		*/

		Vector2D lines[12][2];
		//bottom of box
		lines[0][0] = { boxVectors0.x, boxVectors0.y };
		lines[0][1] = { boxVectors1.x, boxVectors1.y };
		lines[1][0] = { boxVectors1.x, boxVectors1.y };
		lines[1][1] = { boxVectors2.x, boxVectors2.y };
		lines[2][0] = { boxVectors2.x, boxVectors2.y };
		lines[2][1] = { boxVectors3.x, boxVectors3.y };
		lines[3][0] = { boxVectors3.x, boxVectors3.y };
		lines[3][1] = { boxVectors0.x, boxVectors0.y };

		lines[4][0] = { boxVectors0.x, boxVectors0.y };
		lines[4][1] = { boxVectors6.x, boxVectors6.y };

		// top of box
		lines[5][0] = { boxVectors6.x, boxVectors6.y };
		lines[5][1] = { boxVectors5.x, boxVectors5.y };
		lines[6][0] = { boxVectors5.x, boxVectors5.y };
		lines[6][1] = { boxVectors4.x, boxVectors4.y };
		lines[7][0] = { boxVectors4.x, boxVectors4.y };
		lines[7][1] = { boxVectors7.x, boxVectors7.y };
		lines[8][0] = { boxVectors7.x, boxVectors7.y };
		lines[8][1] = { boxVectors6.x, boxVectors6.y };


		lines[9][0] = { boxVectors5.x, boxVectors5.y };
		lines[9][1] = { boxVectors1.x, boxVectors1.y };

		lines[10][0] = { boxVectors4.x, boxVectors4.y };
		lines[10][1] = { boxVectors2.x, boxVectors2.y };

		lines[11][0] = { boxVectors7.x, boxVectors7.y };
		lines[11][1] = { boxVectors3.x, boxVectors3.y };

		for (int i = 0; i < 12; i++)
		{
			Drawing::DrawLine(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, color);
		}
	}
}

void Drawing::rotate_point(Vector2D& point, Vector2D origin, bool clockwise, float angle) {
	Vector2D delta = point - origin;
	Vector2D rotated;

	if (clockwise) {
		rotated = Vector2D(delta.x * cos(angle) - delta.y * sin(angle), delta.x * sin(angle) + delta.y * cos(angle));
	}
	else {
		rotated = Vector2D(delta.x * sin(angle) - delta.y * cos(angle), delta.x * cos(angle) + delta.y * sin(angle));
	}

	point = rotated + origin;
}

void Drawing::DrawFilledCircle(int x, int y, int radius, int segments, color_t color) {
	std::vector< Vertex_t > vertices;
	float step = PI * 2.0f / segments;
	for (float a = 0; a < (PI * 2.0f); a += step)
		vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + x, radius * sinf(a) + y)));

	TexturedPolygon(vertices.size(), vertices, color);
}

void Drawing::TexturedPolygon(int n, std::vector< Vertex_t > vertice, color_t color) {
	static int texture_id = interfaces.surfaces->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	interfaces.surfaces->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
	interfaces.surfaces->DrawSetcolor_t(color.get_red(), color.get_green(), color.get_blue(), color.get_alpha()); //
	interfaces.surfaces->DrawSetTexture(texture_id); //
	interfaces.surfaces->DrawTexturedPolygon(n, vertice.data()); //
}

void Drawing::filled_tilted_triangle(Vector2D position, Vector2D size, Vector2D origin, bool clockwise, float angle, color_t color, bool rotate) {
	std::vector< Vertex_t > vertices =
	{
		Vertex_t{ Vector2D(position.x - size.x, position.y + size.y), Vector2D() },
		Vertex_t{ Vector2D(position.x, position.y - size.y), Vector2D() },
		Vertex_t{ position + size, Vector2D() }
	};

	if (rotate) {
		for (unsigned int p = 0; p < vertices.size(); p++) {
			rotate_point(vertices[p].m_Position, origin, clockwise, angle);
		}
	}

	TexturedPolygon(vertices.size(), vertices, color);

	vertices.clear();
}

void Drawing::DrawCircle(float x, float y, float r, float s, color_t color)
{
	float Step = PI * 2.0 / s;
	for (float a = 0; a < (PI * 2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;

		DrawLine(x1, y1, x2, y2, color);
	}
}