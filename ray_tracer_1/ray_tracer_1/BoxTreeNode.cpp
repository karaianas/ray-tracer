#include "BoxTreeNode.h"
#include <iostream>

BoxTreeNode::BoxTreeNode()
{
}

BoxTreeNode::~BoxTreeNode()
{
}

bool BoxTreeNode::Intersect(const Ray & ray, Intersection & hit)
{
	return false;
}

void BoxTreeNode::Construct(int count, Triangle *tri)
{
	std::cout << count << std::endl;

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

	// Check if this is a leaf node
	if (count <= MaxTrianglesPerBox) 
	{
		// Copy triangles to BoxTreeNode’s Tri array
		for (int i = 0; i < count; i++)
			Tri[i] = &tri[i];

		return;
	}

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
		//tri2[count2 - 1] = NULL;
		count2--;
	}
	else if (count2 == 0)
	{
		tri2[count2] = tri1[count1 - 1];
		count2++;
		//tri1[count1 - 1] = NULL;
		count1--;
	}

	std::cout << count1 << " " << count2 << std::endl;
	// Recursively build sub-trees
	Child1 = new BoxTreeNode; 
	Child2 = new BoxTreeNode;
	Child1->Construct(count1, tri1); 
	Child2->Construct(count2, tri2);
	
	// Free up arrays
	delete[]tri1; delete[]tri2;
}
