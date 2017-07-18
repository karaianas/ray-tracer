#pragma once
#include "Object.h"

class InstanceObject : public Object{
public:
	InstanceObject(Object &obj);
	bool Intersect(const Ray &ray, Intersection &hit);
	void SetChild(Object &obj);
	void SetMatrix(glm::mat4x4 &mtx);

	glm::vec3 minBBox();
	glm::vec3 maxBBox();

private:
	glm::mat4x4 Matrix;
	glm::mat4x4 Inverse; // Pre-computed inverse of Matrix
	Object *Child;
};