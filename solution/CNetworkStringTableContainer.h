#pragma once
class NetworkStringTable {
public:
	int add_string(bool isServer, const char* value, int length = -1, const void* userdata = nullptr) {
		using original_fn = int(__thiscall*)(NetworkStringTable*, bool, const char*, int, const void*);
		return (*(original_fn**)this)[8](this, isServer, value, length, userdata);
	}
};

class NetworkStringTableContainer {
public:
	NetworkStringTable* find_table(const char* name) {
		using original_fn = NetworkStringTable * (__thiscall*)(NetworkStringTableContainer*, const char*);
		return (*(original_fn**)this)[3](this, name);
	}
};