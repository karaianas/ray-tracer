#pragma once

#include "Core.h"
#include "Scene.h"
#include "Material.h"

class RayTrace {
public:
	RayTrace(Scene &s);
	bool TraceRay(const Ray &ray, Intersection &hit, int depth = 1);
	bool TracePath(const Ray &ray, Intersection &hit, int depth = 0, float weight = 1.0f);

private:
	Scene *Scn;

	// Settings
	int MaxDepth;
	
	// Statistics
	int PrimaryRays;
	int SecondaryRays;
	int ShadowRays;
};