#include "LambertMaterial.h"

void LambertMaterial::GenerateSample(const Intersection & isect, const glm::vec3 & inDir, glm::vec3 & outDir, Color & outColor)
{
}

void LambertMaterial::SetColor(Color c)
{
	DiffuseColor = c;
}
