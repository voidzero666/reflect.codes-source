#pragma once
#include "DLL_MAIN.h"

typedef uint8_t*(*FindPatternIDA_T)(HMODULE szModule, const char* szSignature);
typedef void(*pattern_to_byte_t)(const char*, int**, size_t*);
typedef uint8_t*(*FindPatternIDACore_t)(pattern_to_byte_t, decltype(free), HMODULE, const char*);

class cUtils
{
public:
	FindPatternIDACore_t FindPatternIDACore;
	FindPatternIDA_T FindPatternIDA;
	//uint8_t*(*FindPatternIDA)(const char* szModule, const char* szSignature);
	//uint8_t* FindPatternIDA(/*const char**/HMODULE, const char* szSignature);
	bool LoadTextureFromFile(const char* filename, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height);
	bool LoadTextureFromMemory(void* data, size_t datasize, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height);
};


template <typename T, typename ... args_t>
constexpr T CallVFuncCDecl(void* thisptr, std::size_t nIndex, args_t... argList)
{
	using VirtualFn = T(__cdecl*)(void*, decltype(argList)...);
	return (*static_cast<VirtualFn**>(thisptr))[nIndex](thisptr, argList...);
}

class IHudChat
{
public:
	void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...) {
		CallVFuncCDecl<void>(this, 27, iPlayerIndex, iFilter, fmt);
	}
};


