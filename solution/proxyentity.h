#ifndef PROXY_ENTITY_H
#define PROXY_ENTITY_H
#pragma once


class IMaterialVar;
class IBasePlayer;
class IMaterial;
class KeyValues;

class IMaterialProxy
{
public:
	virtual bool Init(IMaterial * pMaterial, KeyValues * pKeyValues) = 0;
	virtual void OnBind(void*) = 0;
	virtual void Release() = 0;
	virtual IMaterial* GetMaterial() = 0;

protected:
	// no one should call this directly
	virtual ~IMaterialProxy() {}
};

//-----------------------------------------------------------------------------
// Base class all material proxies should inherit from
//-----------------------------------------------------------------------------
class CEntityMaterialProxy : public IMaterialProxy
{
public:
	//virtual void Release(void);
	//virtual void OnBind(void* pC_BaseEntity);

protected:
	// base classes should implement these
	virtual void OnBind(IBasePlayer* pBaseEntity) = 0;
};


#endif // PROXY_ENTITY_H
