////////////////////////////////////////
// MeshObject.h
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "Object.h"
#include "Triangle.h"
#include "Material.h"

////////////////////////////////////////////////////////////////////////////////

class MeshObject:public Object {
public:
	MeshObject();
	~MeshObject();

	bool Intersect(const Ray &ray,Intersection &hit);

	void MakeBox(float x,float y,float z,Material *mtl = 0);

	bool LoadPLY(const char *filename, Material *mtl = 0);
	void Smooth();

	glm::vec3 minBBox();
	glm::vec3 maxBBox();

	int GetTriangleNum() { return NumTriangles; }
	Triangle* GetTriangles() { return Triangles; }

private:
	int NumVertexes,NumTriangles;
	Vertex *Vertexes;
	Triangle *Triangles;
	// -------------------
	Material *Mtl;
};

////////////////////////////////////////////////////////////////////////////////
