#pragma once
#include "Hooks.h"

class CKnifebot : public Singleton<CKnifebot> {
public:
	void Run();
	bool work = false;
};