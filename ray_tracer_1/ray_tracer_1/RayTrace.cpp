#include "RayTrace.h"

using namespace std;

RayTrace::RayTrace(Scene & s)
{
	Scn = &s;
	MaxDepth = 20;
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

bool RayTrace::TracePath(const Ray & ray, Intersection & hit, int depth)
{
	// Nothing was hit
	if (Scn->Intersect(ray, hit) == false)
	{
		hit.Shade = Scn->GetSkyColor();
		return false;
	}

	// Maximum depth reached * This is before implementing Russian Roulette
	if (depth == MaxDepth)
	{
		hit.Shade = Color::BLACK;
		return false;
	}

	// Initialize
	hit.Shade = Color::BLACK;
	Color totalColor = Color::BLACK;

	// (1) Direct light
	int num = Scn->GetNumLights();
	for (int i = 0; i < num; i++)
	{
		glm::vec3 toLight, ltPos;
		Color c;

		float intensityXcolor = Scn->GetLight(i).Illuminate(hit.Position, c, toLight, ltPos);
		float directLight = intensityXcolor * glm::clamp(dot(hit.Normal, toLight), 0.0f, 1.0f);

		// (1-1) Compute shadow ray
		Ray s;
		s.Direction = toLight;
		s.Origin = hit.Position;

		Intersection shit;
		bool in = Scn->Intersect(s, shit);

		float lightdistance = glm::length(ltPos - hit.Position);
		float obstdistance = glm::length(shit.Position - hit.Position);

		// (1-2) Add the direct light * material color
		// No obstacle
		if (!in)
		{
			c.Scale(directLight);
			Color temp = Color::BLACK;
			hit.Mtl->GetColor(temp);
			//cout << temp.Red << " " << temp.Green << " " << temp.Blue << endl;
			c.Multiply(temp);
			totalColor.Add(c);
			//hit.Shade.Add(c);
		}
		// There was an obstacle
		else
		{
			// Obstacle is beyond light
			if (obstdistance >= lightdistance)
			{
				c.Scale(directLight);
				Color temp = Color::BLACK;
				hit.Mtl->GetColor(temp);
				c.Multiply(temp);
				totalColor.Add(c);
				//hit.Shade.Add(c);
			}
			// Obstacle is in between
			else
			{
				// There was no direct lighting
			}
		}

	}

	hit.Shade = totalColor;
	// (2) Indirect light
	
	// (2-1) Generate sample ray
	glm::vec3 outDir;
	Color outColor;
	hit.Mtl->GenerateSample(hit, ray.Direction, outDir, outColor);

	Ray sampleRay;
	sampleRay.Direction = outDir;
	sampleRay.Origin = hit.Position;

	// (2-1) Find next intersection
	Intersection nextHit;
	TracePath(sampleRay, nextHit, depth + 1);

	// (2-2) Add indirect light * nextHit.Shade * reflectance
	//hit.Mtl->ComputeReflectance(sth, ray.Direction, outDir, hit);
	outColor.Multiply(nextHit.Shade);

	totalColor.Add(outColor);
	hit.Shade = totalColor;

	return true;
}
