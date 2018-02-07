#include "LambertMaterial.h"

using namespace std;

LambertMaterial::LambertMaterial()
{
	R = Random();
}

void LambertMaterial::GenerateSample(const Intersection & isect, const glm::vec3 & inDir, glm::vec3 & outDir, Color & outColor)
{
	int seed = rand() % 1000 + 10;
	glm::vec2 pair = R.RandomNumbers(seed, 1)[0];
	float u = 2 * PI * pair[0];
	float v = sqrt(1 - pow(pair[1], 2));

	glm::vec3 P(v * glm::cos(u), sqrt(pair[1]), v * glm::sin(u));
	outDir = glm::normalize(GetCoord(isect.Normal) * P);
	outColor = DiffuseColor;
}

void LambertMaterial::SetColor(Color c)
{
	DiffuseColor = c;
}

glm::mat3 LambertMaterial::GetCoord(glm::vec3 Normal)
{
	glm::vec3 temp;
	if (Normal[0] == Normal[1] && Normal[1] == Normal[2])
	{
		temp[0] = Normal[0] * -1.0f;
		temp[1] = Normal[1];
		temp[2] = Normal[2];
	}
	else
	{
		temp[0] = Normal[2];
		temp[1] = Normal[0];
		temp[2] = Normal[1];
	}

	glm::vec3 i = glm::normalize(glm::cross(Normal, temp));
	glm::vec3 k = glm::cross(Normal, i);
	
	glm::mat3 M;
	M[0] = i;
	M[1] = Normal;
	M[2] = k;

	for (int i = 0; i < 100; i++)
	{
		int seed = rand() % 1000 + 10;
		glm::vec2 pair = R.RandomNumbers(seed, 1)[0];
		float u = 2 * PI * pair[0];
		float v = sqrt(1 - pair[1]);

		glm::vec3 P(v * glm::cos(u), sqrt(pair[1]), v * glm::sin(u));
		glm::vec3 result = M * P;
		result = glm::normalize(result);
		cout << result[0] << " " << result[1] << " " << result[2] << endl;
	}
	
	return M;
}

void LambertMaterial::print(glm::mat3 M)
{
	for(int i = 0; i < 3; i++)
		cout << "column " << i << " " << M[i][0] << " " << M[i][1] << " " << M[i][2] << endl;
}