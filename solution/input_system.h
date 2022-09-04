#include "common.h"
#include "c_usercmd.h"

enum ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,
};




class IInputSystem
{
public:
	void EnableInput(bool bEnable)
	{
		typedef void(__thiscall* OriginalFn)(void*, bool);
		return getvfunc< OriginalFn >(this, 11)(this, bEnable);
	}

	void ResetInputState()
	{
		typedef void(__thiscall* OriginalFn)(void*);
		return getvfunc< OriginalFn >(this, 39)(this);
	}

	bool IsButtonDown(ButtonCode_t code)
	{
		typedef bool(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return getvfunc< OriginalFn >(this, 15)(this, code);
	}

	void GetCursorPosition(int* m_pX, int* m_pY)
	{
		typedef void(__thiscall* OriginalFn)(void*, int*, int*);
		return getvfunc< OriginalFn >(this, 56)(this, m_pX, m_pY);
	}

	ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey)
	{
		typedef ButtonCode_t(__thiscall* OriginalFn)(void*, int);
		return getvfunc< OriginalFn >(this, 44)(this, nVirtualKey);
	}

	int ButtonCodeToVirtualKey(ButtonCode_t code)
	{
		typedef int(__thiscall* OriginalFn)(void*, ButtonCode_t);
		return getvfunc< OriginalFn >(this, 45)(this, code);
	}

	int vkey2buttoncode(int virtualKey)
	{
		if (virtualKey <= VK_XBUTTON2) {
			if (virtualKey > VK_CANCEL) virtualKey--;
			return virtualKey + 106;
		}
		typedef int (__thiscall* OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 45)(this, virtualKey);
	}

	const char* buttonCodeToString(int buttonCode)
	{
		typedef const char* (__thiscall* OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 40)(this, buttonCode);
	}

	auto vkey2string(int virtualKey)
	{
		
		return buttonCodeToString(vkey2buttoncode(virtualKey));
		
	}

	


};
class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	unsigned long m_crc;
};

class CInput
{
public:

	char pad0[0xC]; // 0x0
	bool m_fTrackIRAvailable; // 0xC
	bool m_fMouseInitialized; // 0xD
	bool m_fMouseActive; // 0xE
	char pad1[0x9A]; // 0xF
	bool m_fCameraInThirdPerson; // 0xAD
	char pad2[0x2]; // 0xAE
	Vector  m_vecCameraOffset; // 0xB0
	char pad3[0x38]; // 0xBC
	CUserCmd* m_pCommands; // 0xF4
	CVerifiedUserCmd* m_pVerifiedCommands; // 0xF8


	CUserCmd* GetUserCmd(int sequence_number)
	{
		return &m_pCommands[sequence_number % 150];
	}

	CUserCmd* GetUserCmd(int nSlot, int sequence_number)
	{
		typedef CUserCmd* (__thiscall* GetUserCmd_t)(void*, int, int);
		return getvfunc<GetUserCmd_t>(this, 8)(this, nSlot, sequence_number);
	}

	CVerifiedUserCmd* GetVerifiedUserCmd(int sequence_number)
	{
		return &m_pVerifiedCommands[sequence_number % 150];
	}
};
