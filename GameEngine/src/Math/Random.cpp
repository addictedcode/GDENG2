#include "stdafx.h"
#include "Random.h"

#include <random>

namespace Random
{
	static std::default_random_engine generator(std::chrono::duration<unsigned int, std::ratio<1, 10000000>>(std::chrono::system_clock::now().time_since_epoch()).count());


	int Int(const int min, const int max)
	{
		const std::uniform_int_distribution<int> distribution(min, max);
		return distribution(generator);
	}

	float Float(const float min, const float max)
	{
		const std::uniform_real_distribution<float> distribution(min, max);
		return distribution(generator);
	}
}
