#include "imgui\imgui_impl_dx9.h"
#include "imgui\imgui_impl_win32.h"
#include "interfaces.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int wheelDelta = 0;

	const auto getButtonToggle = [uMsg, wParam](bool& bButton, int vKey)
	{
		if (wParam != vKey) return;

		if (uMsg == WM_KEYUP)
			bButton = !bButton;
	};

	if (uMsg == WM_MOUSEWHEEL) {
		wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
		for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
			csgo->scroll_amount += 1;
		for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
			csgo->scroll_amount -= 1;
	}
	switch (uMsg) {
	case WM_LBUTTONDOWN:
		csgo->key_pressed[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		csgo->key_pressed[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		csgo->key_pressed[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		csgo->key_pressed[VK_RBUTTON] = false;
		break;
	case WM_MBUTTONDOWN:
		csgo->key_pressed[VK_MBUTTON] = true;
		break;
	case WM_MBUTTONUP:
		csgo->key_pressed[VK_MBUTTON] = false;
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_XBUTTONDOWN:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			csgo->key_pressed[VK_XBUTTON1] = true;
		}
		else if (button == XBUTTON2)
		{
			csgo->key_pressed[VK_XBUTTON2] = true;
		}
		break;
	}
	case WM_XBUTTONUP:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			csgo->key_pressed[VK_XBUTTON1] = false;
		}
		else if (button == XBUTTON2)
		{
			csgo->key_pressed[VK_XBUTTON2] = false;
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		break;
	}
	case WM_SYSKEYDOWN:
		csgo->key_pressed[18] = true;
		break;
	case WM_SYSKEYUP:
		csgo->key_pressed[18] = false;
		break;
	case WM_KEYDOWN:
		csgo->key_pressed[wParam] = true;
		break;
	case WM_KEYUP:
		csgo->key_pressed[wParam] = false;
		break;
	default: break;
	}
	

	for (int i = 0; i < 256; i++) {
		if (csgo->key_pressed[i])
			csgo->key_down[i] = csgo->key_pressedticks[i]++ == 0;
		else
			csgo->key_pressedticks[i] = 0;
	}

	getButtonToggle(vars.menu.open, vars.menu.menu_key);

	if (vars.menu.open) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		if (ImGui::GetIO().WantTextInput)
			return true;
	}

	return CallWindowProc(csgo->Init.OldWindow, hWnd, uMsg, wParam, lParam);
}
