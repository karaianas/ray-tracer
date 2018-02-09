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
		// ***** Why dividing by PI produces a match?
		col = DiffuseColor;
		col.Scale(1.0f / PI);

		//col.Scale(DiffuseColor, 1.0f / PI);
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
