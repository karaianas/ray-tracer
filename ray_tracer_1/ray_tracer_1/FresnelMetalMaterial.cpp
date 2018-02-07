#include "FresnelMetalMaterial.h"

using namespace std;

void FresnelMetalMaterial::ComputeReflectance(Color & col, const glm::vec3 & in, const glm::vec3 & out, const Intersection & hit)
{
	glm::vec3 reflected = glm::reflect(-in, hit.Normal);
	
	// *** How to set the threshold?
	if (glm::length(reflected - out) > 0.001f)
	{
		cout << "lol" << endl;
		col.Set(0.0f, 0.0f, 0.0f);
	}
	else
		col = DiffuseColor;
}

void FresnelMetalMaterial::GenerateSample(const Intersection & isect, const glm::vec3 & inDir, glm::vec3 & outDir, Color & outColor)
{
	// Assuming total reflection:
	outDir = glm::reflect(-inDir, isect.Normal);
	outColor = DiffuseColor; 
	outColor.Scale(glm::clamp(glm::dot(-inDir, isect.Normal), 0.0f, 1.0f));
}

void FresnelMetalMaterial::GetColor(Color &c)
{
	c = DiffuseColor;
}

void FresnelMetalMaterial::SetColor(Color c)
{
	DiffuseColor = c;
}
