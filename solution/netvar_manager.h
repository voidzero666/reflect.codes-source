#pragma once
#include "DLL_MAIN.h"
#include <vector>
#include <iosfwd>


class CNetVarManager
{
public:
	void Initialize();
	int GetOffset(const char* tableName, const char* propName);
	bool HookProp(const char* tableName, const char* propName, void* fun);
	int Get_Prop(const char* tableName, const char* propName, RecvProp** prop = 0);
private:
	int Get_Prop(RecvTable* recvTable, const char* propName, RecvProp** prop = 0);
	RecvTable* GetTable(const char* tableName);
	std::vector< RecvTable* > m_tables;
	void DumpTable(RecvTable* table, int depth);
};

extern CNetVarManager netvars;