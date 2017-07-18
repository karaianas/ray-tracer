#include "BoxTreeObject.h"

BoxTreeObject::BoxTreeObject()
{
}

BoxTreeObject::~BoxTreeObject()
{
}

void BoxTreeObject::Construct(MeshObject & mesh)
{
	BoxTreeNode* btn = new BoxTreeNode();
	int count = mesh.GetTriangleNum();
	Triangle* tptr = mesh.GetTriangles();

	btn->Construct(count, tptr);

	RootNode = btn;
}

bool BoxTreeObject::Intersect(const Ray & ray, Intersection & hit)
{
	return RootNode->Intersect(ray, hit);
}

glm::vec3 BoxTreeObject::minBBox()
{
	return glm::vec3();
}

glm::vec3 BoxTreeObject::maxBBox()
{
	return glm::vec3();
}
