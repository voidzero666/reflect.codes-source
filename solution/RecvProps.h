#pragma once
struct DVariant
{
	union
	{
		float		m_Float;
		long		m_Int;
		char* m_pString;
		void* m_pData;
		Vector		m_Vector;
		int64_t		m_Int64;
	};

	int m_Type;
};

struct RecvProp;

class CRecvProxyData
{
public:
	const RecvProp* m_pRecvProp;
	DVariant m_Value;
	int m_iElement;
	int m_ObjectID;
};

typedef void(*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);

struct RecvTable
{
	RecvProp* m_pProps;
	int m_nProps;
	void* m_pDecoder;
	char* m_pNetTableName;
	bool m_bInitialized;
	bool m_bInMainList;
};

struct RecvProp
{
	char* m_pVarName;
	int m_RecvType;
	int m_Flags;
	int m_StringBufferSize;
	bool m_bInsideArray;
	const void* m_pExtraData;
	RecvProp* m_pArrayProp;
	void* m_ArrayLengthProxy;
	void* m_ProxyFn;
	void* m_DataTableProxyFn;
	RecvTable* m_pDataTable;
	int m_Offset;
	int m_ElementStride;
	int m_nElements;
	const char* m_pParentArrayPropName;
};

//struct ClientClass
//{
//	void* m_pCreateFn;
//	void* m_pCreateEventFn;
//	char* m_pNetworkName;
//	RecvTable* m_pRecvTable;
//	ClientClass* m_pNext;
//	int m_ClassID;
//};

struct ClientClass {
	void* m_pCreateFn;
	void* m_pCreateEventFn;
	char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int m_ClassID;
};

struct ServerClass
{
	const char					*m_pNetworkName;
	void*						*m_pTable;
	ServerClass					*m_pNext;
	int							m_ClassID;
	int							m_InstanceBaselineIndex;
};
