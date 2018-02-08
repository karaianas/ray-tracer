#include "BoxTreeNode.h"

using namespace std;

BoxTreeNode::BoxTreeNode()
{
}

BoxTreeNode::~BoxTreeNode()
{
}

bool BoxTreeNode::IntersectVolume(const Ray &ray, Intersection &hit) 
{
	// Do geometric test with the volume itself and find the closest hit (if less than existing hit.HitDistance)
	
	// Ray-AABB test
	glm::vec3 p = ray.Origin;
	glm::vec3 d = ray.Direction;
	float tx1 = (BoxMin[0] - p[0]) / d[0];
	float tx2 = (BoxMax[0] - p[0]) / d[0];
	float ty1 = (BoxMin[1] - p[1]) / d[1];
	float ty2 = (BoxMax[1] - p[1]) / d[1];
	float tz1 = (BoxMin[2] - p[2]) / d[2];
	float tz2 = (BoxMax[2] - p[2]) / d[2];

	float tminx = glm::min(tx1, tx2);
	float tminy = glm::min(ty1, ty2);
	float tminz = glm::min(tz1, tz2);

	float tmaxx = glm::max(tx1, tx2);
	float tmaxy = glm::max(ty1, ty2);
	float tmaxz = glm::max(tz1, tz2);

	float tmin = glm::max(glm::max(tminx, tminy), tminz);
	float tmax = glm::min(glm::min(tmaxx, tmaxy), tmaxz);

	if (tmax < 0)
		return false;

	if (tmin <= tmax)
	{
		if (tmin >= 0)
		{
			if (tmin < hit.HitDistance)
				hit.HitDistance = tmin;
		}
		else
		{
			if (tmax < hit.HitDistance)
				hit.HitDistance = tmax;
		}
		return true;
	}
	
	return false;
}

bool BoxTreeNode::Intersect(const Ray & ray, Intersection & hit)
{

	// If this is a leaf node, test against triangles
	if (isLeaf)
	{
		for (int i = 0; i < numTri; i++)
			if (Tri[i]->Intersect(ray, hit))
				return true;
		return false;
	}

	// Test all child volumes
	Intersection volhit[2];// Because this is a binary tree
	volhit[0].HitDistance = hit.HitDistance;
	volhit[1].HitDistance = hit.HitDistance;

	bool child1 = Child1->IntersectVolume(ray, volhit[0]);
	bool child2 = Child2->IntersectVolume(ray, volhit[1]);

	// Check if the ray starts from inside
	glm::vec3 p = ray.Origin;
	bool insideBox = (BoxMin[0] <= p[0] && BoxMax[0] >= p[0]) && (BoxMin[1] <= p[1] && BoxMax[1] >= p[1]) && (BoxMin[2] <= p[2] && BoxMax[2] >= p[2]);

	// Find the nearer child
	int order[2] = { 0, 1 };
	BoxTreeNode* nearChild = Child1;
	BoxTreeNode* farChild = Child2;

	if (volhit[0].HitDistance > volhit[1].HitDistance)
	{
		order[0] = 1;
		order[1] = 0;
		nearChild = Child2;
		farChild = Child1;
	}

	bool success = false;

	/*
	if (nearChild)
	{
		if (volhit[order[0]].HitDistance < hit.HitDistance)
			if (nearChild->Intersect(ray, hit))
			{
				if (!insideBox)
					return success = true;
				else
					success = true;
			}
	}
	if (farChild)
	{
		if (volhit[order[1]].HitDistance < hit.HitDistance || insideBox)
			if (farChild->Intersect(ray, hit))
				success = true;
	}
	*/
	
	if (nearChild)
	{
		if (volhit[order[0]].HitDistance < hit.HitDistance || insideBox)
			if (nearChild->Intersect(ray, hit))
				success = true;
	}
	if (farChild)
	{
		if (volhit[order[1]].HitDistance < hit.HitDistance || insideBox)
			if (farChild->Intersect(ray, hit))
				success = true;
	}
	
	
	return success;
}

void BoxTreeNode::Construct(int count, Triangle *tri)
{
	// Compute BoxMin & BoxMax to fit around all tri’s
	glm::vec3 tmin = tri[0].minBBox();
	glm::vec3 tmax = tri[0].maxBBox();
	for (int i = 0; i < count; i++)
	{
		tmin = glm::min(tmin, tri[i].minBBox());
		tmax = glm::max(tmax, tri[i].maxBBox());
	}

	BoxMin = tmin;
	BoxMax = tmax;

	numTri = count;

	// Check if this is a leaf node
	if (count <= MaxTrianglesPerBox)
	{
		// Copy triangles to BoxTreeNode’s Tri array
		for (int i = 0; i < count; i++)
		{
			Tri[i] = &tri[i];
			//std::cout << "Leaf:" << Tri[i]->num << std::endl;
		}
		//std::cout << "--------------" << std::endl;
		isLeaf = true;
		return;
	}
	else
		isLeaf = false;

	// Determine largest box dimension x, y, or z
	float x = tmax[0] - tmin[0];
	float y = tmax[1] - tmin[1];
	float z = tmax[2] - tmin[2];
	glm::vec3 bdim((tmax[0] + tmin[0]) / 2.0f, (tmax[1] + tmin[1]) / 2.0f, (tmax[2] + tmin[2]) / 2.0f);

	// Compute splitting plane halfway along largest dimension
	float dim;
	if (x > y)
		if (x > z)
			dim = 0;
		else
			dim = 2;
	else
		if (y > z)
			dim = 1;
		else
			dim = 2;

	// Allocate two new temporary arrays
	Triangle *tri1 = new Triangle[count]; 
	Triangle *tri2 = new Triangle[count];
	int count1 = 0, count2 = 0;

	// Place triangles into group 1 or group 2
	for (int i = 0; i < count; i++)
	{
		glm::vec3 ctr = tri[i].GetBarycenter();
		if (ctr[dim] > bdim[dim])
		{
			tri1[count1] = tri[i];
			count1++;
		}
		else
		{
			tri2[count2] = tri[i];
			count2++;
		}
	}
	
	// Check if either group is empty. If so, move (at least) 1 triangle into that group
	if (count1 == 0)
	{
		tri1[count1] = tri2[count2 - 1];
		count1++;
		//tri2[count2 - 1] = tri1[count1];
		count2--;
	}
	else if (count2 == 0)
	{
		tri2[count2] = tri1[count1 - 1];
		count2++;
		//tri1[count1 - 1] = tri2[count2];
		count1--;
	}

	// Recursively build sub-trees
	Child1 = new BoxTreeNode; 
	Child2 = new BoxTreeNode;

	Child1->Construct(count1, tri1);
	Child2->Construct(count2, tri2);
	
	// Free up arrays
	//delete[]tri1; delete[]tri2;

}
