#pragma once
#include "iBaseClientDLL.h"
#include "entlist.h"
#include "cvar.h"
#include "input_system.h"
#include "engine.h"
#include "engine_trace.h"
#include "surface.h"
#include "vpanel.h"
#include "global_vars.h"
#include "model_render.h"
#include "model_info.h"
#include "material_system.h"
#include "render_view.h"
#include "glow_manager.h"
#include "iv_effects.h"
#include "game_event.h"
#include "debug_overlay.h"
#include "studio_render.h"
#include "view_render_beams.h"
#include "physics_surface_props.h"
#include "c_movehelper.h"
#include "prediction.h"
#include "memalloc.h"
#include "Panorama.h"
#include "ILocalize.hpp"
#include "CNetworkStringTableContainer.h"
#include "IGameTypes.h"


struct RenderableTranslucencyType_t;
struct RenderableModelType_t;
struct ClientRenderHandle_t;
class CClientLeafSubSystemData;
struct SetupRenderInfo_t;
struct ClientShadowHandle_t;
struct ClientLeafShadowHandle_t;
class IVFileSystem;
class CViewRender;

class IClientLeafSystem
{
public:
	//ClientLeafSystem002
	virtual void CreateRenderableHandle(IClientRenderable* pRenderable, bool bRenderWithViewModels, RenderableTranslucencyType_t nType, RenderableModelType_t nModelType, UINT32 nSplitscreenEnabled = 0xFFFFFFFF) = 0; // = RENDERABLE_MODEL_UNKNOWN_TYPE ) = 0;
	virtual void RemoveRenderable(ClientRenderHandle_t handle) = 0;
	virtual void AddRenderableToLeaves(ClientRenderHandle_t renderable, int nLeafCount, unsigned short *pLeaves) = 0;
	virtual void SetTranslucencyType(ClientRenderHandle_t handle, RenderableTranslucencyType_t nType) = 0;

	/*RenderInFastReflections(unsigned short, bool)
	DisableShadowDepthRendering(unsigned short, bool)
	DisableCSMRendering(unsigned short, bool)*/
	virtual void pad0() = 0;
	virtual void pad1() = 0;
	virtual void pad2() = 0;

	virtual void AddRenderable(IClientRenderable* pRenderable, bool IsStaticProp, RenderableTranslucencyType_t Type, RenderableModelType_t nModelType, UINT32 nSplitscreenEnabled = 0xFFFFFFFF) = 0; //7
	virtual bool IsRenderableInPVS(IClientRenderable *pRenderable) = 0; //8
	virtual void SetSubSystemDataInLeaf(int leaf, int nSubSystemIdx, CClientLeafSubSystemData *pData) = 0;
	virtual CClientLeafSubSystemData *GetSubSystemDataInLeaf(int leaf, int nSubSystemIdx) = 0;
	virtual void SetDetailObjectsInLeaf(int leaf, int firstDetailObject, int detailObjectCount) = 0;
	virtual void GetDetailObjectsInLeaf(int leaf, int& firstDetailObject, int& detailObjectCount) = 0;
	virtual void DrawDetailObjectsInLeaf(int leaf, int frameNumber, int& nFirstDetailObject, int& nDetailObjectCount) = 0;
	virtual bool ShouldDrawDetailObjectsInLeaf(int leaf, int frameNumber) = 0;
	virtual void RenderableChanged(ClientRenderHandle_t handle) = 0;
	virtual void BuildRenderablesList(const SetupRenderInfo_t &info) = 0;
	virtual void CollateViewModelRenderables(void*) = 0;
	virtual void DrawStaticProps(bool enable) = 0;
	virtual void DrawSmallEntities(bool enable) = 0;
	virtual ClientLeafShadowHandle_t AddShadow(ClientShadowHandle_t userId, unsigned short flags) = 0;
	virtual void RemoveShadow(ClientLeafShadowHandle_t h) = 0;
	virtual void ProjectShadow(ClientLeafShadowHandle_t handle, int nLeafCount, const int *pLeafList) = 0;
	virtual void ProjectFlashlight(ClientLeafShadowHandle_t handle, int nLeafCount, const int *pLeafList) = 0;
};
struct CEffectData
{
	Vector origin;
	Vector start;
	Vector normal;
	Vector angles;
	int flags;
	int entity;
	float scale;
	float magnitude;
	float radius;
	int attachmentIndex;
	short surfaceProp;

	int material;
	int damageType;
	int hitBox;

	int otherEntIndex;

	unsigned char color;

	//bool positionsAreRelativeToEntity;

	int effectName;

	CEffectData()
	{
		origin = Vector();
		start = Vector();
		normal = Vector();
		angles = Vector();

		flags = 0;
		entity = -1;
		scale = 1.f;
		attachmentIndex = 0;
		surfaceProp = 0;

		magnitude = 0.0f;
		radius = 0.0f;

		material = 0;
		damageType = 0;
		hitBox = 0;

		color = 0;

		otherEntIndex = 0;

		//positionsAreRelativeToEntity = false;
	}

	int GetEffectNameIndex() { return effectName; }

	IBasePlayer* GetEntity() const;
	int entindex() const;
};

typedef void(*ClientEffectCallback)(const CEffectData& data);

struct CClientEffectRegistration
{
	CClientEffectRegistration(const char* pEffectName, ClientEffectCallback fn);
	const char* effectName;
	ClientEffectCallback function;
	CClientEffectRegistration* next;
};

class CViewSetup {
public:
	int            x, x_old;
	int            y, y_old;
	int            width, width_old;
	int            height, height_old;
	bool        m_bOrtho;
	float        m_OrthoLeft;
	float        m_OrthoTop;
	float        m_OrthoRight;
	float        m_OrthoBottom;
	bool        m_bCustomViewMatrix;
	matrix		m_matCustomViewMatrix;
	char        pad_0x68[0x48];
	float        fov;
	float        fovViewmodel;
	Vector        origin;
	Vector        angles;
	float        zNear;
	float        zFar;
	float        zNearViewmodel;
	float        zFarViewmodel;
	float        m_flAspectRatio;
	float        m_flNearBlurDepth;
	float        m_flNearFocusDepth;
	float        m_flFarFocusDepth;
	float        m_flFarBlurDepth;
	float        m_flNearBlurRadius;
	float        m_flFarBlurRadius;
	int            m_nDoFQuality;
	int            m_nMotionBlurMode;
	float        m_flShutterTime;
	Vector        m_vShutterOpenPosition;
	Vector        m_shutterOpenAngles;
	Vector        m_vShutterClosePosition;
	Vector        m_shutterCloseAngles;
	float        m_flOffCenterTop;
	float        m_flOffCenterBottom;
	float        m_flOffCenterLeft;
	float        m_flOffCenterRight;
	int            m_iEdgeBlur;
};

class IClientMode
{
public:

};
class c_model_interfaces
{
public:
	IVModelRender* model_render;
	IVModelInfo* model_info;
};
class CUtlVectorSimple
{
public:
	unsigned memory;
	char pad[8];
	unsigned int count;
	inline void* Retrieve(int index, unsigned sizeofdata)
	{
		return (void*)((*(unsigned*)this) + (sizeofdata * index));
	}
	inline void* Base()
	{
		return (void*)memory;
	}
	inline int Count()
	{
		return count;
	}
};
class c_interfaces
{
public:
	IServerGameDLL* server;
	IBaseClientDll* client;
	IClientMode* client_mode;
	CEntityList* ent_list;
	IClientLeafSystem* leaf_system;
	ICVar* cvars;
	IInputSystem* inputsystem;
	CInput* input;
	IEngineClient* engine;
	IEngineTrace* trace;
	global_vars_t* global_vars;
	ISurface* surfaces;
	IEngineVGui* engine_vgui;
	IVPanel* v_panel;
	c_model_interfaces models;
	IMaterialSystem* material_system;
	IVRenderView* render_view;
	CGlowObjectManager* glow_manager;
	IGameEventManager2* event_manager;
	IVDebugOverlay* debug_overlay;
	IStudioRender* studio_render;
	IViewRenderBeams* beams;
	IPhysicsSurfaceProps* phys_props;
	IMoveHelper* move_helper;
	CGameMovement* game_movement;
	CPrediction* prediction;
	IMemAlloc* memalloc;
	IEngineSound* engine_sound;
	IMDLCache* model_cache;
	IVFileSystem* v_file_system;
	PanoramaUIEngine* panorama_ui_engine;
	sdk::ILocalize* localize;
	IVEffects* iv_effects;
	NetworkStringTableContainer* network_string_table_container;
	CViewRender* viewrender;
	sdk::IGameTypes* gameType;
};


extern c_interfaces interfaces;
