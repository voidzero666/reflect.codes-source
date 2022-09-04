#pragma once
inline int UtlMemory_CalcNewAllocationCount(int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem)
{
	if (nGrowSize)
		nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
	else
	{
		if (!nAllocationCount)
			nAllocationCount = (31 + nBytesItem) / nBytesItem;

		while (nAllocationCount < nNewSize)
			nAllocationCount *= 2;
	}

	return nAllocationCount;
}

template< class T, class I = int >
class CUtlMemory
{
public:
	T& operator[](I i)
	{
		return m_pMemory[i];
	}

	T* Base()
	{
		return m_pMemory;
	}

	int NumAllocated() const
	{
		return m_nAllocationCount;
	}

	void Grow(int num = 1)
	{
		if (IsExternallyAllocated())
			return;

		int nAllocationRequested = m_nAllocationCount + num;
		int nNewAllocationCount = UtlMemory_CalcNewAllocationCount(m_nAllocationCount, m_nGrowSize, nAllocationRequested, sizeof(T));

		if ((int)(I)nNewAllocationCount < nAllocationRequested)
		{
			if ((int)(I)nNewAllocationCount == 0 && (int)(I)(nNewAllocationCount - 1) >= nAllocationRequested)
				--nNewAllocationCount;
			else
			{
				if ((int)(I)nAllocationRequested != nAllocationRequested)
					return;

				while ((int)(I)nNewAllocationCount < nAllocationRequested)
					nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
			}
		}

		m_nAllocationCount = nNewAllocationCount;

		if (m_pMemory)
			m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
		else
			m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}

	bool IsExternallyAllocated() const
	{
		return m_nGrowSize < 0;
	}

protected:
	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template <class T>
inline T* Construct(T* pMemory)
{
	return ::new(pMemory) T;
}

template <class T>
inline void Destruct(T* pMemory)
{
	pMemory->~T();
}

template< class T, class A = CUtlMemory<T> >
class CUtlVector
{
	typedef A CAllocator;
public:
	CAllocator m_Memory;
	int m_Size;

	T& operator[](int i)
	{
		return m_Memory[i];
	}

	T& Element(int i)
	{
		return m_Memory[i];
	}

	T* Base()
	{
		return m_Memory.Base();
	}

	int Count() const
	{
		return m_Size;
	}

	void RemoveAll()
	{
		for (int i = m_Size; --i >= 0; )
			Destruct(&Element(i));

		m_Size = 0;
	}

	int AddToTail()
	{
		return InsertBefore(m_Size);
	}

	int InsertBefore(int elem)
	{
		GrowVector();
		ShiftElementsRight(elem);
		Construct(&Element(elem));

		return elem;
	}

protected:
	void GrowVector(int num = 1)
	{
		if (m_Size + num > m_Memory.NumAllocated())
			m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());

		m_Size += num;
		ResetDbgInfo();
	}

	void ShiftElementsRight(int elem, int num = 1)
	{
		int numToMove = m_Size - elem - num;
		if ((numToMove > 0) && (num > 0))
			memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
	}

	T* m_pElements;

	inline void ResetDbgInfo()
	{
		m_pElements = Base();
	}
};
enum EConVar
{
	ConVar_GetName = 5,
	ConVar_GetBaseName = 6,
	ConVar_GetFloat = 12,
	ConVar_GetInt = 13,
	ConVar_SetString = 14,
	ConVar_SetFloat,
	ConVar_SetInt
};

enum EICVar
{
	ICVar_FindVar = 15
};
typedef void(*FnChangeCallbackV1_t)(void);
typedef void(*FnChangeCallback_t)(void* var, const char *pOldValue, float flOldValue);

struct CVValue_t
{
	char* m_pszString;
	int m_StringLength;

	// Values
	float m_fValue;
	int m_nValue;
};





class ConVar
{
public:
	const char* GetString()
	{
		typedef const char*(__thiscall* GetString_t)(void*);
		return getvfunc<GetString_t>(this, 11)(this);
	}

	float GetFloat()
	{
		typedef float(__thiscall* GetFloat_t)(void*);
		return getvfunc <GetFloat_t>(this, 12)(this);
	}

	int GetInt()
	{
		typedef int(__thiscall* GetInt_t)(void*);
		return getvfunc <GetInt_t>(this, 13)(this);
	}

	void SetValue(const char *value)
	{
		typedef void(__thiscall* SetValue_t)(void*, const char*);
		return getvfunc<SetValue_t>(this, 14)(this, value);
	}

	void SetValue(float value)
	{
		typedef void(__thiscall* SetValue_t)(void*, float);
		return getvfunc<SetValue_t>(this, 15)(this, value);
	}

	void SetValue(int value)
	{
		typedef void(__thiscall* SetValue_t)(void*, int);
		return getvfunc<SetValue_t>(this, 16)(this, value);
	}

	void SetInt(int value)
	{
		typedef void(__thiscall* SetValue_t)(void*, int);
		return getvfunc<SetValue_t>(this, 16)(this, value);
	}

	void SetString(const char* value)
	{
		typedef void(__thiscall* SetValue_t)(void*, const char*);
		return getvfunc<SetValue_t>(this, 14)(this, value);
	}

	void SetFloat(float value)
	{
		typedef void(__thiscall* SetValue_t)(void*, float);
		return getvfunc<SetValue_t>(this, 15)(this, value);
	}

	/*void SetValue(color_t value)
	{
	typedef void(__thiscall* SetValue_t)(void*, color_t);
	return getvfunc<SetValue_t>(this, 17)(this, value);
	}
	*/


	char* GetName()
	{
		typedef char*(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 5)(this);
	}


	void* virtualtable;
	ConVar* m_pNext;
	int m_bRegistered;
	char* m_pszName;
	char* m_pszHelpString;
	int m_nFlags;
	std::add_pointer_t<void __cdecl()> m_fnChangeCallbacksV1;
	ConVar* m_pParent;
	char* m_pszDefaultValue;
	CVValue_t m_Value;
	int m_bHasMin;
	float m_fMinVal;
	int m_bHasMax;
	float m_fMaxVal;
	CUtlVector<FnChangeCallback_t> m_fnChangeCallbacks; // note: this is also accessible as FnChangeCallback_t* instead of CUtlVector
	float GetValue()
	{
		DWORD xored = *(DWORD*)&m_pParent->m_Value.m_fValue ^ (DWORD)this;
		return *(float*)&xored;
	}
	int GetValueN()
	{
		return (int)(m_pParent->m_Value.m_nValue ^ (DWORD)this);
	}
	bool GetBool() { return !!GetInt(); }
};//Size=0x0048*

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

class IAppSystem {
public:
	virtual bool                            Connect(CreateInterfaceFn factory) = 0;                                     // 0
	virtual void                            Disconnect() = 0;                                                           // 1
	virtual void* QueryInterface(const char* pInterfaceName) = 0;                             // 2
	virtual int /*InitReturnVal_t*/         Init() = 0;                                                                 // 3
	virtual void                            Shutdown() = 0;                                                             // 4
	virtual const void* /*AppSystemInfo_t*/ GetDependencies() = 0;                                                      // 5
	virtual int /*AppSystemTier_t*/         GetTier() = 0;                                                              // 6
	virtual void                            Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) = 0;       // 7
	virtual void                            UnkFunc() = 0;                                                              // 8
};
enum MDLCacheDataType_t
{
	// Callbacks to get called when data is loaded or unloaded for these:
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,

	// Callbacks NOT called when data is loaded or unloaded for these:
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK,
};
//-----------------------------------------------------------------------------
enum MDLCacheFlush_t
{
	MDLCACHE_FLUSH_STUDIOHDR = 0x01,
	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
	MDLCACHE_FLUSH_VCOLLIDE = 0x04,
	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
	MDLCACHE_FLUSH_AUTOPLAY = 0x20,
	MDLCACHE_FLUSH_VERTEXES = 0x40,

	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};

typedef unsigned short MDLHandle_t;
struct studiohdr_t;
struct studiohwdata_t;
struct vcollide_t;
struct virtualmodel_t;
struct vertexFileHeader_t;
class IMDLCache : public IAppSystem
{
public:
	// Used to install callbacks for when data is loaded + unloaded
// Returns the prior notify
	virtual void SetCacheNotify(void* pNotify) = 0;

	// NOTE: This assumes the "GAME" path if you don't use
	// the UNC method of specifying files. This will also increment
	// the reference count of the MDL
	virtual MDLHandle_t FindMDL(const char* pMDLRelativePath) = 0;

	// Reference counting
	virtual int AddRef(MDLHandle_t handle) = 0;
	virtual int Release(MDLHandle_t handle) = 0;
	virtual int GetRef(MDLHandle_t handle) = 0;

	// Gets at the various data associated with a MDL
	virtual studiohdr_t* GetStudioHdr(MDLHandle_t handle) = 0;
	virtual studiohwdata_t* GetHardwareData(MDLHandle_t handle) = 0;
	virtual vcollide_t* GetVCollide(MDLHandle_t handle) = 0;
	virtual unsigned char* GetAnimBlock(MDLHandle_t handle, int nBlock) = 0;
	virtual virtualmodel_t* GetVirtualModel(MDLHandle_t handle) = 0;
	virtual int GetAutoplayList(MDLHandle_t handle, unsigned short** pOut) = 0;
	virtual vertexFileHeader_t* GetVertexData(MDLHandle_t handle) = 0;

	// Brings all data associated with an MDL into memory
	virtual void TouchAllData(MDLHandle_t handle) = 0;

	// Gets/sets user data associated with the MDL
	virtual void SetUserData(MDLHandle_t handle, void* pData) = 0;
	virtual void* GetUserData(MDLHandle_t handle) = 0;

	// Is this MDL using the error model?
	virtual bool IsErrorModel(MDLHandle_t handle) = 0;

	// Flushes the cache, force a full discard
	virtual void Flush(MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;

	// Flushes a particular model out of memory
	virtual void Flush(MDLHandle_t handle, int nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;

	// Returns the name of the model (its relative path)
	virtual const char* GetModelName(MDLHandle_t handle) = 0;

	// faster access when you already have the studiohdr
	virtual virtualmodel_t* GetVirtualModelFast(const studiohdr_t* pStudioHdr, MDLHandle_t handle) = 0;

	// all cache entries that subsequently allocated or successfully checked 
	// are considered "locked" and will not be freed when additional memory is needed
	virtual void BeginLock() = 0;

	// reset all protected blocks to normal
	virtual void EndLock() = 0;

	// returns a pointer to a counter that is incremented every time the cache has been out of the locked state (EVIL)
	virtual int* GetFrameUnlockCounterPtrOLD() = 0;

	// Finish all pending async operations
	virtual void FinishPendingLoads() = 0;

	virtual vcollide_t* GetVCollideEx(MDLHandle_t handle, bool synchronousLoad = true) = 0;
	virtual bool GetVCollideSize(MDLHandle_t handle, int* pVCollideSize) = 0;

	virtual bool GetAsyncLoad(MDLCacheDataType_t type) = 0;
	virtual bool SetAsyncLoad(MDLCacheDataType_t type, bool bAsync) = 0;

	virtual void BeginMapLoad() = 0;
	virtual void EndMapLoad() = 0;
	virtual void MarkAsLoaded(MDLHandle_t handle) = 0;

	virtual void InitPreloadData(bool rebuild) = 0;
	virtual void ShutdownPreloadData() = 0;

	virtual bool IsDataLoaded(MDLHandle_t handle, MDLCacheDataType_t type) = 0;

	virtual int* GetFrameUnlockCounterPtr(MDLCacheDataType_t type) = 0;

	virtual studiohdr_t* LockStudioHdr(MDLHandle_t handle) = 0;
	virtual void UnlockStudioHdr(MDLHandle_t handle) = 0;

	virtual bool PreloadModel(MDLHandle_t handle) = 0;

	// Hammer uses this. If a model has an error loading in GetStudioHdr, then it is flagged
	// as an error model and any further attempts to load it will just get the error model.
	// That is, until you call this function. Then it will load the correct model.
	virtual void ResetErrorModelStatus(MDLHandle_t handle) = 0;

	virtual void MarkFrame() = 0;
};


class color_t;

struct CVarDLLIdentifier_t;
class ConCommandBase
{
public:
	PVOID pVMTBase; // 0x00

	ConCommandBase *m_pNext; // 0x04
	bool m_bRegistered; // 0x08
	const char *m_pszName; // 0x0C
	const char *m_pszHelpString; // 0x10
	int m_nFlags; // 0x14

	ConCommandBase *s_pConCommandBases; // 0x18
	void* s_pAccessor; // 0x1C
}; // Size: 0x0020
class ConCommand;

class IConsoleDisplayFunc
{
public:
	virtual void color_tPrint(const uint8_t* clr, const char *pMessage) = 0;
	virtual void Print(const char *pMessage) = 0;
	virtual void DPrint(const char *pMessage) = 0;
};

class ICVar
{
public:
	template <typename... Values>
	void ConsoleColorPrintf(const color_t& Msgcolor_t, const char* szMsgFormat, Values... Parameters)
	{
		typedef void(*oConsoleColorPrintf)(void*, const color_t&, const char*, ...);
		return getvfunc<oConsoleColorPrintf>(this, 25)(this, Msgcolor_t, szMsgFormat, Parameters...);
	}

	void UnregisterConCommand(ConVar* pCommandBase)
	{
		typedef ConVar*(__thiscall* FindVarFn)(void*, ConVar*);
		getvfunc< FindVarFn >(this, 13)(this, pCommandBase);
	}
	ConVar * FindVar(const char* var_name)
	{
		typedef ConVar*(__thiscall* FindVarFn)(void*, const char*);
		return getvfunc< FindVarFn >(this, 16)(this, var_name);
	}

	void RegisterConCommand(ConVar* pCommandBase)
	{
		typedef ConVar*(__thiscall* FindVarFn)(void*, ConVar*);
		getvfunc< FindVarFn >(this, 12)(this, pCommandBase);
	}
};
