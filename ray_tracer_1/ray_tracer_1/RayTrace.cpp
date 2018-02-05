#include "RayTrace.h"

RayTrace::RayTrace(Scene & s)
{
	Scn = &s;
	MaxDepth = 0;
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
		float actual_intensity = intensity * glm::clamp(dot(hit.Normal, toLight), 0.0f, 1.0f);

		//std::cout << hit.Normal[0] << " " << hit.Normal[0] << " " << hit.Normal[0] << std::endl;
		Ray s;
		s.Direction = toLight;
		s.Origin = hit.Position;

		Intersection shit;
		bool in = Scn->Intersect(s, shit);

		float lightdistance = glm::length(ltPos - hit.Position);
		float obstdistance = glm::length(shit.Position - hit.Position);

		// No obstacle
		if (!in)
		{
			c.Scale(actual_intensity);
			hit.Shade.Add(c);
		}
		// There was an obstacle
		else
		{
			// Obstacle is beyond light
			if (obstdistance >= lightdistance)
			{
				c.Scale(actual_intensity);
				hit.Shade.Add(c);
			}
			// Obstacle is in between
			else
			{
				// For direct illumination, there is no need for proceeding
			}
		}

	}

	if (depth == MaxDepth)
		return true;

	return true;
}
