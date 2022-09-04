#pragma once
/* Other */
#define _CRT_SECURE_NO_WARNINGS
//#include "protect/VMProtectSDK.h"
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <psapi.h>
#include <time.h>
#include <process.h>
#include <vector>
#include <map>
#include <ostream>
#include <Shlobj.h>
#include <stdint.h>
#include <string>
#include <string.h>
#include <cmath>
#include <float.h>
#include <codecvt>
#include <sapi.h>
#include <algorithm>
#include <iterator>
#include <d3d9.h>
#include <rpcndr.h>
#include "protect/protect.hpp"

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(size) BYTE MACRO_CONCAT(_pad, __COUNTER__)[size];

/* Cheat */
#include "cMainStruct.h"
#include "Vector.h"
#include "RecvProps.h"
#include "bspflags.h"
#include "UtlMap.h"

template<typename T>
class Singleton
{
protected:
	Singleton() {}
	~Singleton() {}

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

public:
	__forceinline static T& Get()
	{
		static T inst{};
		return inst;
	}
};

class CSignatures : public Singleton<CSignatures>
{
public:
	void Initialize();

	std::uintptr_t Reset;
	std::uintptr_t Present;
	uintptr_t* tpfilesengine;
	uintptr_t* tpfilespanorama;
};

struct c_bind
{
	c_bind()
	{
		active = false;
		type = 0;
		key = 0;
	}

	unsigned short type = 0;
	unsigned int key = 0;
	bool active = false;
};