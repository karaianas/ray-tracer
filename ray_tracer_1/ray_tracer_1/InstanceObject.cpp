#include "InstanceObject.h"

InstanceObject::InstanceObject(Object &obj)
{
	Child = &obj;
}

bool InstanceObject::Intersect(const Ray & ray, Intersection & hit)
{
	Ray ray2;
	ray2.Origin = glm::vec3(Inverse * glm::vec4(ray.Origin, 1));
	ray2.Direction = glm::vec3(Inverse * glm::vec4(ray.Direction, 0));

	if (Child->Intersect(ray2, hit) == false) 
		return false;
	
	hit.Position = glm::vec3(Matrix * glm::vec4(hit.Position, 1));
	hit.Normal = glm::vec3(Matrix * glm::vec4(hit.Normal, 0));
	hit.HitDistance = glm::distance(ray.Origin, hit.Position); // Correct for any scaling
	
	// -------------------
	hit.Mtl = Mtl;

	return true;
}

void InstanceObject::SetChild(Object & obj)
{
	Child = &obj;
}

void InstanceObject::SetMatrix(glm::mat4x4 & mtx)
{
	Matrix = mtx;
	Inverse = glm::inverse(mtx);
}

void InstanceObject::SetMaterial(Material* mtl)
{
	Mtl = mtl;
}

glm::vec3 InstanceObject::minBBox()
{
	return Child->minBBox();
}

glm::vec3 InstanceObject::maxBBox()
{
	return Child->maxBBox();
}
