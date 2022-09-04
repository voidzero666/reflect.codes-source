#pragma once

#include "VMT_Manager.h"
#include "Pad.h"

class UIPanel2 {
public:

        //VIRTUAL_METHOD(int, getChildCount, 48, (), (this))
        int getChildCount()
        {
            typedef int(__thiscall* OriginalFn)(void*);
            return getvfunc< OriginalFn >(this, 48)(this);
        }

        void* getChild(int n)
        {
            typedef void*(__thiscall* OriginalFn)(void*, int);
            return getvfunc< OriginalFn >(this, 49)(this, n);
        }

        bool hasClass(const char* name)
        {
            typedef bool(__thiscall* OriginalFn)(void*, const char*);
            return getvfunc< OriginalFn >(this, 139)(this, name);
        }

        void setHasClass(const char* name, bool hasClass)
        {
            typedef void(__thiscall* OriginalFn)(void*, const char*, bool);
            return getvfunc< OriginalFn >(this, 145)(this, name, hasClass);
        }

        float getAttributeFloat(const char* name, float defaultValue)
        {
            typedef float(__thiscall* OriginalFn)(void*, const char*, float);
            return getvfunc< OriginalFn >(this, 278)(this, name, defaultValue);
        }

        void setAttributeFloat(const char* name, float value)
        {
            typedef void(__thiscall* OriginalFn)(void*, const char*, float);
            return getvfunc< OriginalFn >(this, 288)(this, name, value);
        }
};

struct PanoramaEventRegistration {
    int numberOfArgs;
    char pad1[4];
    void* (__cdecl* makeEvent)(void*);
    void* (__cdecl* createEventFromString)(void*, const char* args, const char** result);
    char pad2[24];
};

class UIEngine {
public:
    void dispatchEvent(void* eventPtr)
    {
        typedef void(__thiscall* OriginalFn)(void*, void*);
        return getvfunc< OriginalFn >(this, 52)(this, eventPtr);
    }
    
};

class PanoramaUIEngine {
public:
    UIEngine* accessUIEngine()
    {
        typedef UIEngine*(__thiscall* OriginalFn)(void*);
        return getvfunc< OriginalFn >(this, 11)(this);
    }
};