#pragma once
#include "Visuals.h"
#include "GreandePrediction.h"
#include "Bunnyhop.h"
#include "Tracer.h"
#include "Eventlog.h"

class features_t
{
public:
	CVisuals* Visuals;
	CBunnyhop* Bunnyhop;
	CBulletTracer* BulletTracer;
	CEventlog* Eventlog;
};

extern features_t* features;