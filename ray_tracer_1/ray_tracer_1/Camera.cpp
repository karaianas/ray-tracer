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

	A = new Bitmap(x, y);
	B = new Bitmap(x, y);

	Ia = new Img();
	Ia->empVar(YRes, XRes);

	Ib = new Img();
	Ib->empVar(YRes, XRes);

	It = new Img();
	It->empVar(YRes, XRes);

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

	Random R;
	vector<glm::vec2> samples;
	int counter = 0;

	if (EnableJitter)
	{
		int num = rand() % 100 + 100;
		samples = R.RandomNumbers(num, nx * ny * XRes * YRes);

		//vector<float> test;
		//mt19937::result_type seed = time(0);
		//for (int i = 0; i < nx * ny * XRes * YRes; i++)
		//{
		//	auto real_rand = std::bind(std::uniform_real_distribution<double>(0, 1), mt19937(seed));
		//	test.push_back(float(real_rand()));
		//}
		
		#pragma omp parallel
		for (int y = 0; y < YRes; y++)
		{
			#pragma omp for schedule(dynamic,1)
			for (int x = 0; x < XRes; x++)
			{

				if (EnableJitter)
				{
					Color c;
					c.Set(0.0f, 0.0f, 0.0f);

					vector<Color> tcolors;
					Color a, b;
					a.Set(0.0f, 0.0f, 0.0f);
					b.Set(0.0f, 0.0f, 0.0f);
					//cout << "--------------" << endl;
					for (int index = 0; index < nx * ny; index++)
					{
						int j = index % nx;
						int i = (index - j) / nx;

						float s_ = float(j) / float(nx) + samples[index + counter][0] / float(nx);
						float t_ = float(i) / float(ny) + samples[index + counter][1] / float(ny);

						//cout << s_ << " " << t_ << endl;
						float fx = ((float(x) + s_)) / float(XRes) - 0.5f;
						float fy = ((float(y) + t_)) / float(YRes) - 0.5f;
						//cout << fx << " " << fy << endl;

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
						counter++;

					}

					c.Scale(1.0f / float(nx * ny));
					BMP->SetPixel(x, y, c.ToInt());

					a.Scale(2.0f / float(nx * ny));
					A->SetPixel(x, y, a.ToInt());

					b.Scale(2.0f / float(nx * ny));
					B->SetPixel(x, y, b.ToInt());

					// Need to calculate empirical variances here
					It->setEmpV(YRes - y, x, getVariance(c, tcolors, 0));
					Ia->setEmpV(YRes - y, x, getVariance(a, tcolors, 1));
					Ib->setEmpV(YRes - y, x, getVariance(b, tcolors, 2));
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

	
	//It->showImg('e');
	//std::cout << BMP.GetXRes()  << " " << BMP.GetYRes() << " " << BMP.getpix() << std::endl;
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

void Camera::BlockProcess(Scene &s, float scaleX, float scaleY, int counter, vector<glm::vec2> samples)
{
	for (int y = 0; y < YRes; y++)
	{
		for (int x = 0; x < XRes; x++)
		{

			Color c;
			c.Set(0.0f, 0.0f, 0.0f);

			for (int index = 0; index < nx * ny; index++)
			{
				int j = index % nx;
				int i = (index - j) / nx;
				
				float s_ = -0.5f + float(j) / float(nx) + samples[index + counter][0] / float(nx);
				float t_ = -0.5f + float(i) / float(ny) + samples[index + counter][1] / float(ny);

				float fx = ((float(x) + 0.5f + s_)) / float(XRes) - 0.5f;
				float fy = ((float(y) + 0.5f + t_)) / float(YRes) - 0.5f;

				Ray ray;
				ray.Direction = glm::normalize(fx * scaleX * WorldMatrix[0] + fy * scaleY * WorldMatrix[1] - WorldMatrix[2]);
				ray.Origin = WorldMatrix[3];

				Intersection hit;

				RayTrace RT(s);

				// Path tracer
				bool in = RT.TracePath(ray, hit, 0, 1.0f);

				//if (in)
				//	c.Add(hit.Shade);
				//else
				//	c.Add(hit.Shade);

				counter++;

			}
			//c.Scale(1.0f / float(nx * ny));
			//BMP->SetPixel(x, y, c.ToInt());
		}
	}
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
		for (int i = 0; i < colors.size() - 1; i+=2)
		{
			Color c = colors[i];
			rsum += pow(c.Red - r, 2.0f);
			gsum += pow(c.Green - g, 2.0f);
			bsum += pow(c.Blue - b, 2.0f);
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
