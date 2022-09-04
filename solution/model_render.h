#include "DLL_MAIN.h"
typedef unsigned short ModelInstanceHandle_t;
typedef void* StudioDecalHandle_t;
class ITexture;

struct ModelRenderInfo_t
{
	Vector origin;
	Vector angles;
	uint32_t pad;
	IClientRenderable* pRenderable;
	const model_t* pModel;
	matrix* pModelToWorld;
	const matrix* pLightingOffset;
	const Vector* pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;
};

struct DrawModelState_t
{
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pStudioHWData;
	IClientRenderable* m_pRenderable;
	const matrix* m_pModelToWorld;
	StudioDecalHandle_t		m_decals;
	int						m_drawFlags;
	int						m_lod;
};

struct RenderableInfo_t {
	IClientRenderable* m_pRenderable; // 0
	void* m_pAlphaProperty; // 4
	int m_EnumCount; // 8
	int m_nRenderFrame; // 12
	unsigned short m_FirstShadow; // 16
	unsigned short m_LeafList; // 18
	short m_Area; // 20
	uint16_t m_Flags;   // 22
	uint16_t m_Flags2; // 24
	Vector m_vecBloatedAbsMins;
	Vector m_vecBloatedAbsMaxs;
	Vector m_vecAbsMins;
	Vector m_vecAbsMaxs;
	int pad;
};

struct ColorMeshInfo_t;

struct LightDesc_t
{
	int	type;
	Vector		vecColor;
	Vector		vecPosition;
	Vector		vecDirection;
	float		flRange;
	float		flFalloff;
	float		flAttenuation0;
	float		flAttenuation1;
	float		flAttenuation2;
	float		flTheta;
	float		flPhi;
	float		flThetaDot;
	float		flPhiDot;
	float		flOneOverThetaDotMinusPhiDot;
	std::uint32_t fFlags;
protected:
	float		flRangeSquared;
};

struct MaterialLightingState_t
{
	Vector			vecAmbientCube[6];
	Vector			vecLightingOrigin;
	int				nLocalLightCount;
	LightDesc_t		localLightDesc[4];
};

struct studiohwdata_t;

struct StaticPropRenderInfo_t
{
	const matrix* pModelToWorld;
	const model_t* pModel;
	IClientRenderable* pRenderable;
	Vector* pLightingOrigin;
	short                   skin;
	ModelInstanceHandle_t   instance;
};

typedef void* LightCacheHandle_t;

typedef int OverrideType_t;

struct DrawModelInfo_t
{
	studiohdr_t* pStudioHdr;
	studiohwdata_t* pHardwareData;
	StudioDecalHandle_t		hDecals;
	int						iSkin;
	int						iBody;
	int						iHitboxSet;
	IClientRenderable* pClientEntity;
	int						iLOD;
	ColorMeshInfo_t* pColorMeshes;
	bool					bStaticLighting;
	MaterialLightingState_t	LightingState;
};

struct LightingQuery_t
{
	Vector                  LightingOrigin;
	ModelInstanceHandle_t   InstanceHandle;
	bool                    bAmbientBoost;
};

struct StaticLightingQuery_t : public LightingQuery_t
{
	IClientRenderable* pRenderable;
};


class IMatRenderContext;
class DataCacheHandle_t;
struct color_tMeshInfo_t;

class IVModelRender
{
public:
	virtual int                     DrawModel(int flags, IClientRenderable* pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t* model, Vector const& origin, Vector const& angles, int skin, int body, int hitboxset, const matrix* modelToWorld = NULL, const matrix* pLightingOffset = NULL) = 0;
	virtual void                    ForcedMaterialOverride(IMaterial* newMaterial, int nOverrideType = 0, int nOverrides = 0) = 0;
	virtual bool                    IsForcedMaterialOverride(void) = 0; // hey hey!!!!! this is used to fix glow being retarded in DrawModel / DrawModelExecute. I bet you didn't know that! ~alpha
	virtual void                    SetViewTarget(const studiohdr_t* pStudioHdr, int nBodyIndex, const Vector& target) = 0;
	virtual ModelInstanceHandle_t   CreateInstance(IClientRenderable* pRenderable, LightCacheHandle_t* pCache = NULL) = 0;
	virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
	virtual void                    SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
	virtual LightCacheHandle_t      GetStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable* pRenderable) = 0;
	virtual void                    AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru, int maxLODToDecal) = 0;
	virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
	virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
	virtual void                    RemoveAllDecalsFromAllModels() = 0;
	virtual matrix* DrawModelShadowSetup(IClientRenderable* pRenderable, int body, int skin, ModelRenderInfo_t* pInfo, matrix* pCustomBoneToWorld = NULL) = 0;
	virtual void                    DrawModelShadow(IClientRenderable* pRenderable, const ModelRenderInfo_t& info, matrix* pCustomBoneToWorld = NULL) = 0;
	virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual void                    ReleaseAllStaticPropcolor_tData(void) = 0;
	virtual void                    RestoreAllStaticPropcolor_tData(void) = 0;
	virtual int                     DrawModelEx(ModelRenderInfo_t& pInfo) = 0;
	virtual int                     DrawModelExStaticProp(ModelRenderInfo_t& pInfo) = 0;
	virtual bool                    DrawModelSetup(ModelRenderInfo_t& pInfo, DrawModelInfo_t* pState, matrix** ppBoneToWorldOut) = 0;
	virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelInfo_t& state, const ModelRenderInfo_t& pInfo, matrix* pCustomBoneToWorld = NULL) = 0;
	virtual void                    SetupLighting(const Vector& vecCenter) = 0;
	virtual int                     DrawStaticPropArrayFast(StaticPropRenderInfo_t* pProps, int count, bool bShadowDepth) = 0;
	virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
	virtual void                    Setupcolor_tMeshes(int nTotalVerts) = 0;
	virtual void                    SetupLightingEx(const Vector& vecCenter, ModelInstanceHandle_t handle) = 0;
	virtual bool                    GetBrightestShadowingLightSource(const Vector& vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
	virtual void                    ComputeLightingState(int nCount, const LightingQuery_t* pQuery, MaterialLightingState_t* pState, ITexture** ppEnvCubemapTexture) = 0;
	virtual void                    GetModelDecalHandles(StudioDecalHandle_t* pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t* pHandles) = 0;
	virtual void                    ComputeStaticLightingState(int nCount, const StaticLightingQuery_t* pQuery, MaterialLightingState_t* pState, MaterialLightingState_t* pDecalState, color_tMeshInfo_t** ppStaticLighting, ITexture** ppEnvCubemapTexture, DataCacheHandle_t* pcolor_tMeshHandles) = 0;
	virtual void                    CleanupStaticLightingState(int nCount, DataCacheHandle_t* pcolor_tMeshHandles) = 0;
};