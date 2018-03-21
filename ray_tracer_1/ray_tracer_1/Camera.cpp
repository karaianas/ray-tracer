#include "Camera.h"

using namespace std;

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

	//Random R;
	//vector<glm::vec2> samples;
	//int counter = 0;

	clock_t t;
	t = clock();

	if (EnableJitter)
	{
		//int num = rand() % 100 + 100;
		//samples = R.RandomNumbers(num, nx * ny * XRes * YRes);

		#pragma omp parallel
		for (int y = 0; y < YRes; y++)
		{
			#pragma omp for schedule(dynamic,1)
			for (int x = 0; x < XRes; x++)
			{

				int spp = I->getSampleNum(YRes - y - 1, x);

				if (EnableJitter && spp > 0)
				{
					Color c;
					c.Set(0.0f, 0.0f, 0.0f);

					vector<Color> tcolors;
					Color a, b;
					a.Set(0.0f, 0.0f, 0.0f);
					b.Set(0.0f, 0.0f, 0.0f);

					for (int index = 0; index < spp; index++)
					{
						float s_ = float(rand() % 10000) / 10000.0f;
						float t_ = float(rand() % 10000) / 10000.0f;

						float fx = ((float(x) + s_)) / float(XRes) - 0.5f;
						float fy = ((float(y) + t_)) / float(YRes) - 0.5f;

						Ray ray;
						ray.Direction = glm::normalize(fx * scaleX * WorldMatrix[0] + fy * scaleY * WorldMatrix[1] - WorldMatrix[2]);
						ray.Origin = WorldMatrix[3];

						Intersection hit;

						RayTrace RT(s);

						// Path tracer
						bool in = RT.TracePath(ray, hit, 0);

						c.Add(hit.Shade);

						if (index % 2 == 0)
							a.Add(hit.Shade);
						else
							b.Add(hit.Shade);
						tcolors.push_back(hit.Shade);
						//counter++;

					}
					a.Scale(1.0f / float(spp / 2.0f));
					b.Scale(1.0f / float(spp / 2.0f));
					c.Scale(1.0f / float(spp));

					// *********************************
					BMP->SetPixel(x, y, c.ToInt());
					// *********************************

					// Need to calculate empirical variances here
					//I->sendToV(YRes - y - 1, x, getVariance(c, tcolors, 0));
					//I->sendToA(YRes - y - 1, x, getVariance(a, tcolors, 1), glm::vec3(a.Red, a.Green, a.Blue));
					//I->sendToB(YRes - y - 1, x, getVariance(b, tcolors, 2), glm::vec3(b.Red, b.Green, b.Blue));
					
				}
			}
		}
	}
	else
	{
		#pragma omp parallel
		for (int y = 0; y < YRes; y++)
		{
			#pragma omp for schedule(dynamic,1)
			for (int x = 0; x < XRes; x++)
			{
				float fx = ((float(x) + 0.5f)) / float(XRes) - 0.5f;
				float fy = ((float(y) + 0.5f)) / float(YRes) - 0.5f;

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
					BMP->SetPixel(x, y, hit.Shade.ToInt());
				else
					BMP->SetPixel(x, y, s.GetSkyColor().ToInt());
			}
		}
	}

	t = clock() - t;
	float seconds = ((double)t) / CLOCKS_PER_SEC;
	printf("Rendering Time: %2.2f seconds\n", seconds);
}

void Camera::SaveBitmap(char * filename)
{
	BMP->SaveBMP(filename);
}

void Camera::SetSuperSample(int xsamples, int ysamples)
{
	nx = xsamples;
	ny = ysamples;
}

void Camera::SetJitter(bool enable)
{
	EnableJitter = enable;
}

void Camera::RenderPixel(int x, int y)
{

}

glm::vec3 Camera::getVariance(Color & avg, vector<Color>& colors, int mode)
{
	glm::vec3 var(0.0f);
	float r = avg.Red;
	float g = avg.Green;
	float b = avg.Blue;
	float rsum = 0.0f; float gsum = 0.0f; float bsum = 0.0f;

	// Total variance
	if (mode == 0)
	{
		for (int i = 0; i < colors.size(); i++)
		{
			Color c = colors[i];
			rsum += pow(c.Red - r, 2.0f);
			gsum += pow(c.Green - g, 2.0f);
			bsum += pow(c.Blue - b, 2.0f);
		}
	}
	// Variance of A
	else if (mode == 1)
	{
		for (int i = 0; i < colors.size(); i++)
		{
			if (i % 2 == 0)
			{
				Color c = colors[i];
				rsum += pow(c.Red - r, 2.0f);
				gsum += pow(c.Green - g, 2.0f);
				bsum += pow(c.Blue - b, 2.0f);
			}
		}
	}
	// Variance of B
	else if (mode == 2)
	{
		for (int i = 1; i < colors.size(); i+=2)
		{
			Color c = colors[i];
			rsum += pow(c.Red - r, 2.0f);
			gsum += pow(c.Green - g, 2.0f);
			bsum += pow(c.Blue - b, 2.0f);
		}
	}

	float div = (mode == 0) ? colors.size() : colors.size() / 2.0f;
	return glm::vec3(rsum, gsum, bsum) / div;
}
