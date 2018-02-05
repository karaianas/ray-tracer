////////////////////////////////////////
// LambertMaterial.h
////////////////////////////////////////

#pragma once

#include "Material.h"

////////////////////////////////////////////////////////////////////////////////

class LambertMaterial:public Material {
public:
	void ComputeReflectance(Color &col, const glm::vec3 &in, const glm::vec3 &out, const Intersection &hit) 
	{
		float pi=3.1415926f;
		//col.Scale(DiffuseColor, 0.0f);
		col.Scale(DiffuseColor, 1.0f / pi);
	}

	void GenerateSample(const Intersection &isect, const glm::vec3 &inDir, glm::vec3 &outDir, Color &outColor);

	void SetColor(Color c);

private:
	Color DiffuseColor;
};

////////////////////////////////////////////////////////////////////////////////
