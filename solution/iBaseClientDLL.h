#pragma once
#include "common.h"
#include "protobufreader.h"


struct ServerClass;
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
class IServerGameDLL
{
public:
	// Initialize the game (one-time call when the DLL is first loaded )
	// Return false if there is an error during startup.
	/*virtual bool			DLLInit(CreateInterfaceFn engineFactory,
										CreateInterfaceFn physicsFactory,
										CreateInterfaceFn fileSystemFactory,
											CGlobalVarsBase *pGlobals) = 0;*/

											// Setup replay interfaces on the server
	virtual bool			ReplayInit(CreateInterfaceFn fnReplayFactory) = 0;

	// This is called when a new game is started. (restart, map)
	virtual bool			GameInit(void) = 0;

	// Called any time a new level is started (after GameInit() also on level transitions within a game)
	virtual bool			LevelInit(char const *pMapName,
		char const *pMapEntities, char const *pOldLevel,
		char const *pLandmarkName, bool loadGame, bool background) = 0;

	// The server is about to activate
	virtual void			ServerActivate(void *pEdictList, int edictCount, int clientMax) = 0;

	// The server should run physics/think on all edicts
	virtual void			GameFrame(bool simulating) = 0;

	// Called once per simulation frame on the final tick
	virtual void			PreClientUpdate(bool simulating) = 0;

	// Called when a level is shutdown (including changing levels)
	virtual void			LevelShutdown(void) = 0;
	// This is called when a game ends (server disconnect, death, restart, load)
	// NOT on level transitions within a game
	virtual void			GameShutdown(void) = 0;

	// Called once during DLL shutdown
	virtual void			DLLShutdown(void) = 0;

	// Get the simulation interval (must be compiled with identical values into both client and game .dll for MOD!!!)
	// Right now this is only requested at server startup time so it can't be changed on the fly, etc.
	virtual float			GetTickInterval(void) const = 0;

	// Give the list of datatable classes to the engine.  The engine matches class names from here with
	//  edict_t::classname to figure out how to encode a class's data for networking
	virtual ServerClass*	GetAllServerClasses(void) = 0;

	// Returns string describing current .dll.  e.g., TeamFortress 2, Half-Life 2.  
	//  Hey, it's more descriptive than just the name of the game directory
	virtual const char     *GetGameDescription(void) = 0;

	// Let the game .dll allocate it's own network/shared string tables
	virtual void			CreateNetworkStringTables(void) = 0;

	/// not a complete class
};


struct ClientClass;
class IBaseClientDll
{
public:

	ClientClass* GetAllClasses()
	{
		typedef ClientClass* (__thiscall* OriginalFn)(PVOID);
		return getvfunc< OriginalFn >(this, 8)(this);
	}

	bool WriteUsercmdDeltaToBuffer(int nSlot, void* buf,
		int from, int to, bool isNewCmd)
	{
		using Fn = bool(__thiscall*)(void*, int, void*, int, int, bool);
		return getvfunc<Fn>(this, 24)(this, nSlot, buf, from, to, isNewCmd);
	}

	bool dispatchUserMessage(UserMessageType messageType, int arg, int arg1, void* data)
	{
		using Fn = bool(__thiscall*)(void*, UserMessageType, int, int, void*);
		return getvfunc<Fn>(this, 38)(this, messageType, arg, arg1, data);
	}

};
