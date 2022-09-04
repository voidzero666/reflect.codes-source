#include "DLL_MAIN.h"
#include "Utils.h"
#include "Hooks.h"

//
//uint8_t* cUtils::FindPatternIDA(/*const char**/HMODULE hModule, const char* szSignature)
//{
//	static auto pattern_to_byte = [](const char* pattern) {
//		auto bytes = std::vector<int>{};
//		auto start = const_cast<char*>(pattern);
//		auto end = const_cast<char*>(pattern) + strlen(pattern);
//
//		for (auto current = start; current < end; ++current) {
//			if (*current == '?') {
//				++current;
//				if (*current == '?')
//					++current;
//				bytes.push_back(-1);
//			}
//			else {
//				bytes.push_back(strtoul(current, &current, 16));
//			}
//		}
//		return bytes;
//	};
//
//	//auto Module = GetModuleHandleA(szModule);
//
//	auto dosHeader = (PIMAGE_DOS_HEADER)hModule;
//	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + dosHeader->e_lfanew);
//
//	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
//	auto patternBytes = pattern_to_byte(szSignature);
//	auto scanBytes = reinterpret_cast<std::uint8_t*>(hModule);
//
//	auto s = patternBytes.size();
//	auto d = patternBytes.data();
//
//	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
//		bool found = true;
//		for (auto j = 0ul; j < s; ++j) {
//			if (scanBytes[i + j] != d[j] && d[j] != -1) {
//				found = false;
//				break;
//			}
//		}
//		if (found) {
//			return &scanBytes[i];
//		}
//	}
//	return nullptr;
//
//}


bool cUtils::LoadTextureFromFile(const char* filename, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height)
{
    // Load texture from disk
    PDIRECT3DTEXTURE9 texture;
    HRESULT hr = D3DXCreateTextureFromFileA(csgo->render_device, filename, &texture);
    if (hr != S_OK)
        return false;

    // Retrieve description of the texture surface so we can access its size
    D3DSURFACE_DESC my_image_desc;
    texture->GetLevelDesc(0, &my_image_desc);
    *out_texture = texture;
    *out_width = (int)my_image_desc.Width;
    *out_height = (int)my_image_desc.Height;
    return true;
}


bool cUtils::LoadTextureFromMemory(void* data, size_t datasize, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height)
{

    // Load texture from disk
    PDIRECT3DTEXTURE9 texture;
    HRESULT hr = D3DXCreateTextureFromFileInMemory(csgo->render_device, data, datasize, &texture);
    if (hr != S_OK)
        return false;

    // Retrieve description of the texture surface so we can access its size
    D3DSURFACE_DESC my_image_desc;
    texture->GetLevelDesc(0, &my_image_desc);
    *out_texture = texture;
    *out_width = (int)my_image_desc.Width;
    *out_height = (int)my_image_desc.Height;
    return true;

}

