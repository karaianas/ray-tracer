#pragma once
#include "Light.h"

class PointLight :public Light {
public:
	PointLight();
	float Illuminate(const glm::vec3 &pos, Color &col, glm::vec3 &toLight, glm::vec3
		&ltPos) {
		toLight = Position - pos;
		float bright = Intensity / pow(glm::length(toLight), 2); // Inverse square falloff
		toLight = glm::normalize(toLight);
		col = BaseColor;
		ltPos = Position;
		return bright;
	}

	void SetPosition(glm::vec3 pos) { Position = pos; }

private:
	glm::vec3 Position;
};