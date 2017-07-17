#include "Triangle.h"
#include <iostream>

Triangle::Triangle()
{
	// I don't know yet

}

bool Triangle::Intersect(const Ray & ray, Intersection & hit) const
{
	glm::vec3 a = Vtx[0]->Position;
	glm::vec3 b = Vtx[1]->Position;
	glm::vec3 c = Vtx[2]->Position;
	glm::vec3 d = ray.Direction;
	glm::vec3 p = ray.Origin;

	float detM = glm::dot(-d, glm::cross(b - a, c - a));
	float t = glm::dot(p - a, glm::cross(b - a, c - a) / detM);
	float alpha = glm::dot(-d, glm::cross(p - a, c - a) / detM);
	float beta = glm::dot(-d, glm::cross(b - a, p - a) / detM);

	if (detM == 0)
	{
		//std::cout << "The ray is parallel to the triangle" << std::endl;
		return false;
	}

	if (alpha <= 0 || beta <= 0 || alpha + beta >= 1 || t <= 0)
	{
		//std::cout << "Coefficients out of criteria" << std::endl;
		return false;
	}

	// Thresholding/round-off w/o this, the shadows will looks scattered and dirty
	if (t < 0.0001)
		return false;

	if (hit.HitDistance > t)
	{
		hit.HitDistance = t;
		hit.Position = p + t * d;
		glm::vec3 n = (1 - alpha - beta) * Vtx[0]->Normal + alpha * Vtx[1]->Normal + beta * Vtx[2]->Normal;

		if (glm::dot(hit.Position - p, n) >= 0)
			n = -n;

		hit.Normal = n;

		return true;
	}
	else
		return false;// w/o this, the other part of the box is visible
	//std::cout << "The triangle hit point is: " << hit.Position[0] << " " << hit.Position[1] << " " << hit.Position[2] << std::endl;
}
