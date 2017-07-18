#include "RayTrace.h"
#include <iostream>

RayTrace::RayTrace(Scene & s)
{
	Scn = &s;
	MaxDepth = 10;
}

bool RayTrace::TraceRay(const Ray & ray, Intersection & hit, int depth)
{
	// Find ray intersection
	if (Scn->Intersect(ray, hit) == false) 
	{
		hit.Shade = Scn->GetSkyColor();
		return false;
	}
	
	// Compute shade due to lighting
	hit.Shade = Color::BLACK;
	
	int num = Scn->GetNumLights();
	for (int i = 0; i < num; i++)
	{
		glm::vec3 toLight, ltPos;
		Color c;

		float intensity = Scn->GetLight(i).Illuminate(hit.Position, c, toLight, ltPos);
		float actual_intensity = intensity * glm::clamp(dot(hit.Normal, toLight), 0.f, 1.0f);

		Ray s;
		s.Direction = toLight;
		s.Origin = hit.Position;

		Intersection shit;
		bool in = Scn->Intersect(s, shit);

		if (!in)
		{
			c.Scale(actual_intensity);
			hit.Shade.Add(c);
		}
	}

	if (depth == MaxDepth)
		return true;

	// Compute shade due to reflections/refractions
	glm::vec3 d = -ray.Direction;
	
	// Reflection
	Ray r; 
	r.Direction = 2 * glm::dot(d, hit.Normal) * hit.Normal - d;
	r.Origin = hit.Position;

	Intersection rhit;
	bool reflect = TraceRay(r, rhit, depth + 1);

	//float n = 2.485;
	//float k = 3.433;
	//float rpar_2 = ((pow(n, 2) + pow(k, 2)) * pow(dot(hit.Normal, d), 2) - 2 * n * dot(hit.Normal, d) + 1) / ((pow(n, 2) + pow(k, 2)) * pow(dot(hit.Normal, d), 2) + 2 * n * dot(hit.Normal, d) + 1);
	//float rper_2 = ((pow(n, 2) + pow(k, 2)) + pow(dot(hit.Normal, d), 2) - 2 * n * dot(hit.Normal, d)) / ((pow(n, 2) + pow(k, 2)) + pow(dot(hit.Normal, d), 2) + 2 * n * dot(hit.Normal, d));
	//float f = 1.0f / 2.0f * (rpar_2 + rper_2);
	rhit.Shade.Scale(0.3f);
	hit.Shade.Add(rhit.Shade);

	// Refraction
	glm::vec3 z = 0.5f * glm::dot(d, hit.Normal) - d;
	Ray t;
	t.Direction = z - glm::sqrt(1 - pow(glm::length(z), 2)) * hit.Normal;
	t.Origin = hit.Position;

	Intersection thit;
	bool refract = TraceRay(t, thit, depth + 1);
	thit.Shade.Scale(0.2f);
	hit.Shade.Add(thit.Shade);

	return true;
}
