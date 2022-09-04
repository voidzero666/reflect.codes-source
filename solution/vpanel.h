#include "DLL_MAIN.h"
class IVPanel
{
public:
	const char* GetName(int iIndex)
	{
		typedef const char*(__thiscall* OriginalFn)(void*, int);
		return getvfunc< OriginalFn >(this, 36)(this, iIndex);
	}
	void SetMouseInputEnabled(unsigned int panel, bool state)
	{
		return getvfunc<void(__thiscall *)(PVOID, int, bool)>(this, 32)(this, panel, state);
	}
};

class UIPanel {
public:

	int GetChildCount() {
		return getvfunc<int(__thiscall*)(PVOID)>(this, 48)(this);
	}

	UIPanel* GetChild(int n) {
		return getvfunc<UIPanel*(__thiscall*)(PVOID, int)>(this, 49)(this, n);
	}

	bool HasClass(const char* name) {
		return getvfunc<bool(__thiscall*)(PVOID, const char*)>(this, 139)(this, name);
	}

	void SetAttributeFloat(const char* name, float value) {
		return getvfunc<void(__thiscall*)(PVOID, const char*, float)>(this, 288)(this, name, value);
	}
};