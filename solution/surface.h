#include "colors.h"
#include "common.h"
typedef unsigned long HFont;

enum FontRenderFlag_t
{
	FONT_LEFT = 0,
	FONT_RIGHT = 1,
	FONT_CENTER = 2
};
#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900

enum FontFlags_t
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

enum FontDrawType_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

struct Vertex_t
{
	Vertex_t() {}
	Vertex_t(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vector2D	m_Position;
	Vector2D	m_TexCoord;
};

class IEngineVGui
{
public:
	virtual					~IEngineVGui(void) { }

	virtual unsigned int	GetPanel(int type) = 0;

	virtual bool			IsGameUIVisible() = 0;
};


class ISurface
{
public:
	// ISURFACE
	unsigned long Create_Font()
	{
		typedef unsigned int(__thiscall* OriginalFn)(PVOID);
		return getvfunc< OriginalFn >(this, 71)(this);
	}

	void _PlaySound(const char* fileName)
	{
		typedef void(__thiscall* OriginalFn)(void*, const char*);
		return getvfunc<OriginalFn>(this, 82)(this, fileName);
	}
	void DrawSetcolor_t(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int); // 
		getvfunc< OriginalFn >(this, 15)(this, r, g, b, a); // r, g, b, a
	}

	void DrawSetcolor_t1(color_t color)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, color_t); // int, int , int, int
		getvfunc< OriginalFn >(this, 15)(this, color); // r, g, b, a
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc< OriginalFn >(this, 16)(this, x0, y0, x1, y1);
	}

	void DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int, unsigned int, unsigned int, bool);
		getvfunc< OriginalFn >(this, 123)(this, x0, y0, x1, y1, alpha0, alpha1, bHorizontal);
	}

	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long);
		getvfunc< OriginalFn >(this, 23)(this, font);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc< OriginalFn >(this, 19)(this, x0, y0, x1, y1);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc< OriginalFn >(this, 25)(this, r, g, b, a);
	}

	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int);
		getvfunc< OriginalFn >(this, 26)(this, x, y);
	}

	void DrawPrintText(const wchar_t* text, int textLen)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const wchar_t*, int, int);
		return getvfunc< OriginalFn >(this, 28)(this, text, textLen, 0);
	}

	void SetFontGlyphSet(unsigned long& font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		getvfunc< OriginalFn >(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}

	void GetTextSize(unsigned long font, const wchar_t* text, int& wide, int& tall)
	{
		typedef void(__thiscall* OriginalFn)(void*, unsigned long font, const wchar_t* text, int& wide, int& tall);
		getvfunc< OriginalFn >(this, 79)(this, font, text, wide, tall);
	}

	void Drawcolor_tedCircle(int centerx, int centery, float radius, int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, float, int, int, int, int);
		getvfunc< OriginalFn >(this, 162)(this, centerx, centery, radius, r, g, b, a);
	}

	void DrawOutlinedCircle(int x, int y, int r, int seg)
	{
		typedef void(__thiscall* oDrawOutlinedCircle)(PVOID, int, int, int, int);
		return getvfunc< oDrawOutlinedCircle >(this, 103)(this, x, y, r, seg);
	}
	void DrawTexturedPoly(int n, Vertex_t* vertice, int r, int g, int b, int a) {
		static int texture_id = CreateNewTextureID(true);
		static unsigned char buf[4] = { 255, 255, 255, 255 };
		DrawSetTextureRGBA(texture_id, buf, 1, 1);
		DrawSetcolor_t(r, g, b, a);
		DrawSetTexture(texture_id);
		DrawTexturedPolygon(n, vertice);
	}

	void DrawFilledCircle(Vector2D center, int r, int g, int b, int a, float radius, float points)
	{
		std::vector<Vertex_t> vertices;
		float step = (float)3.14159265358979323846f * 2.0f / points;

		for (float a = 0; a < (3.14159265358979323846f * 2.0f); a += step)
			vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + center.x, radius * sinf(a) + center.y)));

		DrawTexturedPoly((int)points, vertices.data(), r, g, b, a);
	}
	void LockCursor()
	{
		typedef void(__thiscall* OriginalFn)(void*);
		getvfunc< OriginalFn >(this, 67)(this);
	}

	void DrawOutlinedRect(int X, int Y, int W, int H)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
		return getvfunc< OriginalFn >(this, 18)(this, X, Y, W, H);
	}

	void DrawT(int X, int Y, color_t Color, int Font, bool Center, char* _Input, ...)
	{
		char Buffer[1024] = { '\0' };

		/* set up varargs*/
		va_list Args;

		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);

		size_t Size = strlen(Buffer) + 1;

		/* set up widebuffer*/
		wchar_t* WideBuffer = new wchar_t[Size];

		/* char -> wchar */
		mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

		/* check center */
		int Width = 0, Height = 0;

		if (Center)
		{
			GetTextSize(Font, WideBuffer, Width, Height);
		}

		/* call and draw*/
		DrawSetTextColor(Color.get_red(), Color.get_green(), Color.get_blue(), Color.get_alpha());
		DrawSetTextFont(Font);
		DrawSetTextPos(X - (Width / 2), Y);
		DrawPrintText(WideBuffer, wcslen(WideBuffer));

		return;
	}

	void DrawString(int font, int x, int y, int align, color_t Color, const wchar_t* text, ...)
	{
		DrawSetTextPos(x, y);
		DrawSetTextColor(Color.get_red(), Color.get_green(), Color.get_blue(), Color.get_alpha());
		DrawSetTextFont(font);
		DrawPrintText(text, wcslen(text));

		return;
	}

	int CreateNewTextureID(bool procedural)
	{
		typedef int(__thiscall* oDrawSetTextcolor_t)(PVOID, bool);
		return getvfunc< oDrawSetTextcolor_t >(this, 43)(this, procedural);
	}

	int DrawUnicodeString(wchar_t const* cx, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT)
	{
		typedef int(__thiscall* oDrawSetTextcolor_t)(PVOID, wchar_t const*, int, FontDrawType_t);
		return getvfunc< oDrawSetTextcolor_t >(this, 132)(this, cx, textLen, drawType);
	}

	void DrawSetTexture(int textureID)
	{
		typedef void(__thiscall* oDrawSetTextcolor_t)(PVOID, int);
		return getvfunc< oDrawSetTextcolor_t >(this, 38)(this, textureID);
	}

	void DrawSetTextureRGBA(int textureID, unsigned char const* colors, int w, int h)
	{
		typedef void(__thiscall* oDrawSetTextcolor_t)(PVOID, int, unsigned char const*, int, int);
		return getvfunc< oDrawSetTextcolor_t >(this, 37)(this, textureID, colors, w, h);
	}

	void DrawTexturedPolygon(int vtxCount, Vertex_t* vtx, bool bClipVertices = true)
	{
		typedef void(__thiscall* oDrawSetTextcolor_t)(PVOID, int, Vertex_t*, bool);
		return getvfunc< oDrawSetTextcolor_t >(this, 106)(this, vtxCount, vtx, bClipVertices);
	}

	void DrawFilledCircle(int centerx, int centery, float radius, int r, int g, int b, int a)
	{
		for (int i = 0; i < radius; i++)
		{
			Drawcolor_tedCircle(centerx, centery, i, r, g, b, a);
		}
	}

	void DrawT2(int x, int y, color_t Color, DWORD font, bool Center, char* _Input, ...)
	{
		int apple = 0;
		char Buffer[1024] = { '\0' };
		va_list Args;
		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);
		size_t Size = strlen(Buffer) + 1;
		wchar_t* WideBuffer = new wchar_t[Size];
		mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);
		/* check center */
		int Width = 0, Height = 0;

		if (Center)
		{
			GetTextSize(font, WideBuffer, Width, Height);
		}

		DrawSetTextFont(font);
		DrawSetTextColor(Color.get_red(), Color.get_green(), Color.get_blue(), Color.get_alpha());
		DrawSetTextPos(x, y);
		DrawPrintText(WideBuffer, wcslen(WideBuffer));
	}
};