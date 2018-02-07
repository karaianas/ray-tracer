#include "Random.h"

using namespace std;

vector<float> Random::RandomNumber64(uint64_t seed, int number)
{
	vector<float> series;
	int countmore = 0;

	seed ^= seed >> 21;
	seed ^= seed << 35;
	seed ^= seed >> 4;
	seed ^= seed >> 21;
	seed ^= seed << 35;
	seed ^= seed >> 4;

	for (int i = 0; i < number; i++)
	{
		seed ^= seed >> 21;
		seed ^= seed << 35;
		seed ^= seed >> 4;
		
		float next = float(seed) / float(UINT64_MAX);

		if (next >= 0.5f)
			countmore++;

		//cout << next << endl;
		series.push_back(next);
	}
	//cout << "bigger:" << countmore << " smaller:" << number - countmore << endl;
	return series;
}

vector<float> Random::RandomNumber32(uint32_t seed, int number)
{
	vector<float> series;
	int countmore = 0;

	seed ^= seed >> 13;
	seed ^= seed << 17;
	seed ^= seed >> 5;
	seed ^= seed >> 13;
	seed ^= seed << 17;
	seed ^= seed >> 5;

	for (int i = 0; i < number; i++)
	{
		seed ^= seed >> 13;
		seed ^= seed << 17;
		seed ^= seed >> 5;

		float next = float(seed) / float(UINT32_MAX);

		if (next >= 0.5f)
			countmore++;

		//cout << next << endl;
		series.push_back(next);
	}
	//cout << "bigger:" << countmore << " smaller:" << number - countmore << endl;
	return series;
}

vector<glm::vec2> Random::RandomNumbers(uint64_t seed, int pairNumber)
{
	vector<glm::vec2> series;
	int countless = 0;
	int countmore = 0;

	seed ^= seed >> 21;
	seed ^= seed << 35;
	seed ^= seed >> 4;
	seed ^= seed >> 21;
	seed ^= seed << 35;
	seed ^= seed >> 4;

	glm::vec2 counter(0, 0);

	for (int i = 0; i < pairNumber; i++)
	{
		seed ^= seed >> 21;
		seed ^= seed << 35;
		seed ^= seed >> 4;

		uint32_t low = (uint32_t)seed;
		uint32_t high = (uint32_t)(seed >> 32);

		float nextLow = float(low) / float(UINT32_MAX);
		float nextHigh = float(high) / float(UINT32_MAX);
		
		series.push_back(glm::vec2(nextLow, nextHigh));
		//if (nextLow >= 0.5f)
		//	counter[0]++;
		//if (nextHigh >= 0.5f)
		//	counter[1]++;
	}

	//float r1 = abs(counter[0] / (pairNumber - counter[0]) - 1.0f);
	//float r2 = abs(counter[1] / (pairNumber - counter[1]) - 1.0f);
	//cout << r1 << endl;
	//cout << r2 << endl;
	//cout << sqrt(pow(r1, 2) + pow(r2, 2)) << endl;
	//cout << "bigger:" << counter[0] << " smaller:" << pairNumber - counter[0] << endl;
	//cout << "bigger:" << counter[1] << " smaller:" << pairNumber - counter[1] << endl;
	return series;
}
