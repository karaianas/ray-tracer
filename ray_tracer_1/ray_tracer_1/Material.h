////////////////////////////////////////
// Material.h
////////////////////////////////////////

#pragma once

#include "Intersection.h"

////////////////////////////////////////////////////////////////////////////////

class Material {
public:
	Material();
	virtual void ComputeReflectance(Color &col, const glm::vec3 &in, const glm::vec3 &out, const Intersection &hit)=0;
	virtual void GenerateSample(const Intersection &isect, const glm::vec3 &inDir, glm::vec3 &outDir, Color &outColor)=0;
	virtual void GetColor(Color &c)=0;
};

////////////////////////////////////////////////////////////////////////////////
