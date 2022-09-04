#pragma once
#include "Hooks.h"

class FakelagGraph
{
public:
	void Render(DVariant& data, void* arg2, void* arg3);
};

extern FakelagGraph* Graph;