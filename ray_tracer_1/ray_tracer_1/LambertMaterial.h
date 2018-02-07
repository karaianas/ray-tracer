////////////////////////////////////////
// LambertMaterial.h
////////////////////////////////////////

#pragma once

#include "Material.h"
#include "Random.h"

////////////////////////////////////////////////////////////////////////////////

class LambertMaterial:public Material {
public:

	LambertMaterial();

	void ComputeReflectance(Color &col, const glm::vec3 &in, const glm::vec3 &out, const Intersection &hit) 
	{
		float pi=3.1415926f;
		//col.Scale(DiffuseColor, 1.0f);
		col.Scale(DiffuseColor, 1.0f / pi);
	}

	void GenerateSample(const Intersection &isect, const glm::vec3 &inDir, glm::vec3 &outDir, Color &outColor);

	void GetColor(Color &c);

	void SetColor(Color c);

	glm::mat3 GetCoord(glm::vec3 Normal);

	void print(glm::mat3 M);

private:
	Color DiffuseColor;
	Random R;
};

////////////////////////////////////////////////////////////////////////////////
