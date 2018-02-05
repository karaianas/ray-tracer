#include "FresnelMetalMaterial.h"

void FresnelMetalMaterial::ComputeReflectance(Color & col, const glm::vec3 & in, const glm::vec3 & out, const Intersection & hit)
{
	glm::vec3 reflected = glm::reflect(-in, hit.Normal);
	
	// *** How to set the threshold?
	if (glm::length(reflected - out) > 0.001f)
		col.Set(0.0f, 0.0f, 0.0f);
}

void FresnelMetalMaterial::GenerateSample(const Intersection & isect, const glm::vec3 & inDir, glm::vec3 & outDir, Color & outColor)
{
	// Assuming total reflection:
	outDir = glm::reflect(-inDir, isect.Normal);
	outColor = DiffuseColor;
}

void FresnelMetalMaterial::SetColor(Color c)
{
	DiffuseColor = c;
}
