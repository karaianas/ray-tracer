////////////////////////////////////////
// Vertex.h
////////////////////////////////////////

#pragma once

#include "glm/glm.hpp"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

class Vertex {
public:
	void Set(const glm::vec3 &p,const glm::vec3 &n,const glm::vec3 &t)		{Position=p; Normal=n; TexCoord=t;}
	void print() {
		std::cout << Normal[0] << " " << Normal[1] << " "
			<< Normal[2] << std::endl;
	}
public:
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoord;

	int num;
};

////////////////////////////////////////////////////////////////////////////////
