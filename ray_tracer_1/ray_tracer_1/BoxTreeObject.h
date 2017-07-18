#pragma once
#include "MeshObject.h"
#include "BoxTreeNode.h"

class BoxTreeObject :public Object {
public:
	BoxTreeObject();
	~BoxTreeObject();
	void Construct(MeshObject &mesh);
	bool Intersect(const Ray &ray, Intersection &hit);

	glm::vec3 minBBox();
	glm::vec3 maxBBox();
	
private:
	BoxTreeNode *RootNode;
};