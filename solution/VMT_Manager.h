#pragma once
#include "DLL_MAIN.h"
#include <assert.h>
typedef DWORD** PPDWORD;

/*
class VFTableHook
{
	VFTableHook(const VFTableHook&) = delete;
public:
	VFTableHook(void* ppClass)
	{
		m_ppClassBase = (PPDWORD)ppClass;
		m_pOriginalVMTable = *(PPDWORD)ppClass;
		uint32_t dwLength = CalculateLength();

		m_pNewVMTable = new DWORD[dwLength];
		memcpy(m_pNewVMTable, m_pOriginalVMTable, dwLength * sizeof(DWORD));

		DWORD old;
		VirtualProtect(m_ppClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*m_ppClassBase = m_pNewVMTable;
		VirtualProtect(m_ppClassBase, sizeof(DWORD), old, &old);
	}
	~VFTableHook()
	{
		RestoreTable();
		delete[] m_pNewVMTable;
	}

	void RestoreTable()
	{
		DWORD old;
		VirtualProtect(m_ppClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*m_ppClassBase = m_pOriginalVMTable;
		VirtualProtect(m_ppClassBase, sizeof(DWORD), old, &old);
	}

	template<class Type>
	Type Hook(uint32_t index, Type fnNew)
	{
		DWORD dwOld = (DWORD)m_pOriginalVMTable[index];
		m_pNewVMTable[index] = (DWORD)fnNew;
		m_vecHookedIndexes.insert(std::make_pair(index, (DWORD)dwOld));
		return (Type)dwOld;
	}

	void Unhook(uint32_t index)
	{
		auto it = m_vecHookedIndexes.find(index);
		if (it != m_vecHookedIndexes.end()) {
			m_pNewVMTable[index] = (DWORD)it->second;
			m_vecHookedIndexes.erase(it);
		}
	}

	template<class Type>
	Type GetOriginal(uint32_t index)
	{
		return (Type)m_pOriginalVMTable[index];
	}

private:
	uint32_t CalculateLength()
	{
		uint32_t dwIndex = 0;
		if (!m_pOriginalVMTable) return 0;
		for (dwIndex = 0; m_pOriginalVMTable[dwIndex]; dwIndex++)
		{
			if (IsBadCodePtr((FARPROC)m_pOriginalVMTable[dwIndex]))
			{
				break;
			}
		}
		return dwIndex;
	}

private:
	std::map<uint32_t, DWORD> m_vecHookedIndexes;

	PPDWORD m_ppClassBase;
	PDWORD m_pOriginalVMTable;
	PDWORD m_pNewVMTable;
};

*/
#pragma endregion

typedef DWORD** PPDWORD;

#define VIRTUAL_METHOD(returnType, name, idx, args, argsRaw) \
constexpr auto name##args noexcept \
{ \
    return VirtualMethod::call<returnType, idx>##argsRaw; \
}

#define VIRTUAL_METHOD_V(returnType, name, idx, args, argsRaw) VIRTUAL_METHOD(returnType, name, idx, args, argsRaw)