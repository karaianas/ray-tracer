#pragma once

#include "Core.h"
#include "Scene.h"

class RayTrace {
public:
	RayTrace(Scene &s);
	bool TraceRay(const Ray &ray, Intersection &hit, int depth = 1);

private:
	Scene *Scn;

	// Settings
	int MaxDepth;
	
	// Statistics
	int PrimaryRays;
	int SecondaryRays;
	int ShadowRays;
};