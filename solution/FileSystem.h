#pragma once
#include "Hooks.h"

static int __stdcall Hooked_getUnverifiedFileHashes(void* someClass, int maxFiles)
{
    //auto original = g_pFileSystemHook->GetOriginal<getUnverifiedFileHashesFn>(g_HookIndices[fnva1(hs::Hooked_GetUnverifiedFileHashes.s().c_str())]);
    if (vars.misc.sv_purebypass)
        return 0;
    //return hooks->fileSystem.callOriginal<int, 101>(thisPointer, maxFiles);
    //return original(g_pFileSystemHook->getclassbase(), someClass, maxFiles);
    //size_t index = g_HookIndices[fnva1(hs::Hooked_GetUnverifiedFileHashes.s().c_str())];
    return g_pFileSystemHook->callOriginal<int, 101>(someClass, maxFiles);
}

static int __fastcall Hooked_canLoadThirdPartyFiles(void* thisPointer, void* edx) noexcept
{
   // auto original = g_pFileSystemHook->GetOriginal<canLoadThirdPartyFilesFn>(g_HookIndices[fnva1(hs::Hooked_CanLoadThirdPartyFiles.s().c_str())]);
    if (vars.misc.sv_purebypass && (_ReturnAddress() == CSignatures::Get().tpfilesengine || _ReturnAddress() == CSignatures::Get().tpfilespanorama))
        return 0;

    return g_pFileSystemHook->callOriginal<int, 128>();
}

typedef void(__fastcall* Panorama_CZip_UnkLoadFiles_t)(void* This, void* edx, void* pData, unsigned int countBytes);

Panorama_CZip_UnkLoadFiles_t TruePanorama_CZip_UnkLoadFiles;

void __fastcall MyPanorama_CZip_UnkLoadFiles(void* This, void* edx, void* pData, unsigned int countBytes)
{
	TruePanorama_CZip_UnkLoadFiles = (Panorama_CZip_UnkLoadFiles_t)csgo->panorama_CZip_UnkLoadFiles_original;

	std::string filePath;

	filePath = csgo->config_directory + crypt_str("PanoramaFiles\\");
	filePath.append(crypt_str("panorama.org.zip"));

	FILE* file = fopen(filePath.c_str(), crypt_str("wb"));
	if (file)
	{
		fwrite(pData, sizeof(BYTE), countBytes, file);
		fclose(file);
	}

	filePath = csgo->config_directory + crypt_str("PanoramaFiles\\");
	filePath.append(crypt_str("panorama.my.zip"));

	file = fopen(filePath.c_str(), crypt_str("rb"));
	if (file)
	{
		long file_size_signed;
		void* pMemory = 0;
		if (fseek(file, 0, SEEK_END) || (file_size_signed = ftell(file)) == -1 || fseek(file, 0, SEEK_SET) || 0 == (pMemory = malloc(file_size_signed)) || file_size_signed != fread(pMemory, sizeof(BYTE), file_size_signed, file))
		{
			fclose(file);
			free(pMemory);
		}
		else
		{
			
			TruePanorama_CZip_UnkLoadFiles(This, edx, pMemory, (unsigned int)file_size_signed);
			fclose(file);
			free(pMemory);
			return;
		}
	}

	return TruePanorama_CZip_UnkLoadFiles(This, edx, pData, countBytes);
}