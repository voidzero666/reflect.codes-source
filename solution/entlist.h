#include "DLL_MAIN.h"

class IBasePlayer;
class CEntityList
{
public:
	virtual void Function0() = 0;
	virtual void Function1() = 0;
	virtual void Function2() = 0;

	virtual IBasePlayer* GetClientEntity(int iIndex) = 0;
	virtual IBasePlayer* GetClientEntityFromHandle(uint32_t hHandle) = 0;
	virtual int NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int GetHighestEntityIndex() = 0;
};
