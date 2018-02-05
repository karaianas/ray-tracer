#pragma once
#include "Core.h"
#include <cstdint>

using namespace std;

class Random {
public:
	vector<float> RandomNumber64(uint64_t seed, int number);
	vector<float> RandomNumber32(uint32_t seed, int number);
	vector<glm::vec2> RandomNumbers(uint64_t seed, int pairNumber);
};