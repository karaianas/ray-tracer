#include "Camera.h"
#include "RayTrace.h"

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
	// ------
	cout << "Random..." << endl;
	Random R;
	int num = rand() % 100 + 10;
	vector<glm::vec2> samples = R.RandomNumbers(num, nx * ny * XRes * YRes);
	cout << "Random fin." << endl;
	// ------

	float HorizonalFOV = 2 * glm::atan(Aspect * glm::tan(VerticalFOV / 2.0f));
	float scaleX = 2 * glm::tan(HorizonalFOV / 2.0f);
	float scaleY = 2 * glm::tan(VerticalFOV / 2.0f);

	int counter = 0;
	for (int y = 0; y < YRes; y++)
	{
		for (int x = 0; x < XRes; x++)
		{
			//int num = rand() % 100 + 10;
			//vector<glm::vec2> samples = R.RandomNumbers(num, nx * ny);
			int color = 0;

			/*
			// Within a pixel
			for (int index = 0; index < nx * ny; index++)
			{
				int j = index % nx;
				int i = (index - j) / nx;

				float s_ = -0.5f + float(j) / float(nx) + samples[index][0] / float(nx);
				float t_ = -0.5f + float(i) / float(ny) + samples[index][1] / float(ny);
				//cout << "(" << j << ", " << i << ")" << endl;
				//cout << "-------------------------" << endl;
				//cout << "(" << samples[index][0] << ", " << samples[index][1] << ")" << endl;
				//cout << "(" << s << ", " << t << ")" << endl;

				float fx = ((float(x) + 0.5f + s_)) / float(XRes) - 0.5f;
				float fy = ((float(y) + 0.5f + t_)) / float(YRes) - 0.5f;

				Ray ray;
				ray.Direction = glm::normalize(fx * scaleX * WorldMatrix[0] + fy * scaleY * WorldMatrix[1] - WorldMatrix[2]);
				ray.Origin = WorldMatrix[3];

				Intersection hit;

				RayTrace RT(s);

				// Basic ray tracer
				//bool in = RT.TraceRay(ray, hit, 0);

				// Path tracer
				bool in = RT.TracePath(ray, hit, 0);

				if (in)
				{
					//color += hit.Shade.ToInt();
					BMP->SetPixel(x, y, hit.Shade.ToInt());
				}
				else
				{
					//color += s.GetSkyColor().ToInt();
					BMP->SetPixel(x, y, s.GetSkyColor().ToInt());
				}

			}
			*/
			//BMP->SetPixel(x, y, int(float(color)/float(nx * ny)));

		}

	}

	//std::cout << BMP.GetXRes()  << " " << BMP.GetYRes() << " " << BMP.getpix() << std::endl;
}

//void Camera::Render(Scene & s)
//{
//	float HorizonalFOV = 2 * glm::atan(Aspect * glm::tan(VerticalFOV / 2.0f));
//	float scaleX = 2 * glm::tan(HorizonalFOV / 2.0f);
//	float scaleY = 2 * glm::tan(VerticalFOV / 2.0f);
//
//	for (int y = 0; y < YRes; y++)
//	{
//		for (int x = 0; x < XRes; x++)
//		{
//			float fx = ((float(x) + 0.5f)) / float(XRes) - 0.5f;
//			float fy = ((float(y) + 0.5f)) / float(YRes) - 0.5f;
//
//			Ray ray;
//			ray.Direction = glm::normalize(fx * scaleX * WorldMatrix[0] + fy * scaleY * WorldMatrix[1] - WorldMatrix[2]);
//			ray.Origin = WorldMatrix[3];
//
//			Intersection hit;
//
//			RayTrace RT(s);
//
//			// Basic ray tracer
//			//bool in = RT.TraceRay(ray, hit, 0);
//
//			// Path tracer
//			bool in = RT.TracePath(ray, hit, 0);
//
//			if (in)
//				BMP->SetPixel(x, y, hit.Shade.ToInt());
//			else
//				BMP->SetPixel(x, y, s.GetSkyColor().ToInt());
//		}
//		
//	}
//
//	//std::cout << BMP.GetXRes()  << " " << BMP.GetYRes() << " " << BMP.getpix() << std::endl;
//}

void Camera::SaveBitmap(char * filename)
{
	BMP->SaveBMP(filename);
}

void Camera::SetSuperSample(int xsamples, int ysamples)
{
	nx = xsamples;
	ny = ysamples;
}

void Camera::RenderPixel(int x, int y)
{

}
