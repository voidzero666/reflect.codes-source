#pragma once
#include "element.h"



// when using remember to add pointer of the class instance to the keybinds vector in init_keybinds()
class pKeybind {
public:
	void imgui(std::string label);
	bool valid()
	{
		return (type != 0 && key != 0) || type == 4; // type not zero and key is valid or type is always on :D
	}
	bool properstate()
	{
		return valid() && state;
	}
	std::string name = "";
	int type = 0;
	int key = 0;
	bool state = false;
	bool opened = false;
	bool special = false; // for directional AA keys
};

inline std::vector<pKeybind*> keybinds;