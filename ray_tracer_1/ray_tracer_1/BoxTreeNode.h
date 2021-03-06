#pragma once
#include "Core.h"
#include "Scene.h"
#include "Triangle.h"
#include "Object.h"
#define MaxTrianglesPerBox 3

class BoxTreeNode {
public:
	BoxTreeNode();
	~BoxTreeNode();
	bool IntersectVolume(const Ray &ray, Intersection &hit);
	bool Intersect(const Ray &ray, Intersection &hit);
	void Construct(int count, Triangle *tri);
	//void Construct(int count, Object & obj);

private:
	glm::vec3 BoxMin, BoxMax;
	BoxTreeNode *Child1, *Child2;
	Triangle *Tri[MaxTrianglesPerBox];
	bool isLeaf;
	int numTri;
};