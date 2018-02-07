#pragma once
#include "Light.h"

class DirectLight :public Light {
public:
	DirectLight();
	float Illuminate(const glm::vec3 &pos, Color &col, glm::vec3 &toLight, glm::vec3 &ltPos) 
	{
		toLight = -Direction;
		col = BaseColor;
		ltPos = pos -(1000000.0f * Direction); // Create virtual position
		return Intensity;
	}
	void SetDirection(glm::vec3 &dir) { Direction = glm::normalize(dir); }

private:
	glm::vec3 Direction;
};