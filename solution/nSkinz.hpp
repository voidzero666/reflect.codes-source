#pragma once
#include "Hooks.h"
#include "DataTable.hpp"

template <typename T>
auto get_entity_from_handle(int h) -> T*
{
	if (h == 0xFFFFFFFF)
		return nullptr;

	return static_cast<T*>(interfaces.ent_list->GetClientEntityFromHandle(h));
}

//auto ensure_dynamic_hooks() -> void;
//auto get_client_name() -> const char*;
class Skinchanger : public Singleton<Skinchanger>
{
public:
	void postDataUpdateSkins(void* thisptr);
	void scheduleHudUpdate() noexcept;
};

void __cdecl sequence_proxy_fn(const CRecvProxyData* proxy_data_const, void* entity, void* output);


extern MinHook* g_sequence_hook;