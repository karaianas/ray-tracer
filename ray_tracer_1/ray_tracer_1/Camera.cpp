#include "Camera.h"
#include "RayTrace.h"
#include <iostream>
#define PI 3.14159

Camera::Camera()
{
	
}

void Camera::SetFOV(float f)
{
	VerticalFOV = f * PI / 180.f;
}

void Camera::SetAspect(float a)
{
	Aspect = a;
}

void Camera::SetResolution(int x, int y)
{
	XRes = x;
	YRes = y;
	BMP = new Bitmap(x, y);
	//std::cout << BMP->GetXRes() << " " << BMP->GetYRes() << std::endl;
}

void Camera::LookAt(glm::vec3 & pos, glm::vec3 & target, glm::vec3 & up)
{
	glm::vec3 d = pos;
	glm::vec3 c = glm::normalize(d - target);
	glm::vec3 a = glm::normalize(glm::cross(up, c));
	glm::vec3 b = glm::cross(c, a);

	glm::mat4x4 cam(a.x, a.y, a.z, 0, b.x, b.y, b.z, 0, c.x, c.y, c.z, 0, d.x, d.y, d.z, 1);
	WorldMatrix = cam;
}

void Camera::Render(Scene & s)
{
	float HorizonalFOV = 2 * glm::atan(Aspect * glm::tan(VerticalFOV / 2.0f));
	float scaleX = 2 * glm::tan(HorizonalFOV / 2.0f);
	float scaleY = 2 * glm::tan(VerticalFOV / 2.0f);

	int x, y;
	for (int y = 0; y < YRes; y++)
	{
		for (int x = 0; x < XRes; x++)
		{
			float fx = ((float(x) + 0.5f)) / float(XRes) - 0.5f;
			float fy = ((float(y) + 0.5f)) / float(YRes) - 0.5f;

			Ray ray;
			ray.Direction = glm::normalize(fx * scaleX * WorldMatrix[0] + fy * scaleY * WorldMatrix[1] - WorldMatrix[2]);
			ray.Origin = WorldMatrix[3];

			Intersection hit;

			RayTrace RT(s);
			bool in = RT.TraceRay(ray, hit, 0);
			if (in)
				BMP->SetPixel(x, y, hit.Shade.ToInt());
			else
				BMP->SetPixel(x, y, s.GetSkyColor().ToInt());
		}
		
	}

	//std::cout << BMP.GetXRes()  << " " << BMP.GetYRes() << " " << BMP.getpix() << std::endl;
}

void Camera::SaveBitmap(char * filename)
{
	BMP->SaveBMP(filename);
}

void Camera::RenderPixel(int x, int y)
{

}
