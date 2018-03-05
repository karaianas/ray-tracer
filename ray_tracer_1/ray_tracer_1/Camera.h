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
#include "RayTrace.h"

#include "Img.h"

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
	void SetJitter(bool enable);

	// Render each pixel
	void RenderPixel(int x, int y);

	// Multi-threading
	void BlockProcess(Scene &s, float scaleX, float scaleY, int counter, vector<glm::vec2> samples);

	// Calculated empirical variance
	glm::vec3 getVariance(Color & avg, vector<Color>& colors);

private:
	int XRes, YRes;
	glm::mat4x4 WorldMatrix;
	float VerticalFOV;
	float Aspect;
	Bitmap* BMP;

	// Project 3
	int nx, ny;
	bool EnableJitter;

public:
	// Buffers
	Bitmap* A;
	Bitmap* B;

	Img* I;
};