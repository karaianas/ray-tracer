////////////////////////////////////////
// FresnelMetalMaterial.h
////////////////////////////////////////

#pragma once
#include "Material.h"
#include "Core.h"

class FresnelMetalMaterial:public Material{
public:
	void ComputeReflectance(Color &col, const glm::vec3		&in, const glm::vec3 &out, const Intersection &hit);
	
	void GenerateSample(const Intersection &isect, const glm::vec3 &inDir, glm::vec3 &outDir, Color &outColor);

	void GetColor(Color &c);

	void SetColor(Color c);

private:
	Color DiffuseColor;
};