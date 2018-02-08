////////////////////////////////////////
// Camera.h
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "Bitmap.h"
#include "Ray.h"
#include "Intersection.h"
#include "Scene.h"
#include "Random.h"
////////////////////////////////////////////////////////////////////////////////


class Camera {
public:
	Camera();
	void SetFOV(float f);
	void SetAspect(float a);
	void SetResolution(int x, int y);
	void LookAt(glm::vec3 &pos, glm::vec3 &target, glm::vec3 &up);
	void Render(Scene &s);
	void SaveBitmap(char *filename);

	// Project 3
	void SetSuperSample(int xsamples, int ysamples);

	// Render each pixel
	void RenderPixel(int x, int y);

private:
	int XRes, YRes;
	glm::mat4x4 WorldMatrix;
	float VerticalFOV;
	float Aspect;
	Bitmap* BMP;

	// Project 3
	int nx, ny;
};