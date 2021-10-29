#include "pch.h"
#include "HRandom.h"
#include <random>


HRandom::HRandom()
{
    auto* gen = new std::default_random_engine();
    this->Generator = gen;
    gen->seed((uint32) SClock::GetEpochMS());
}


HRandom::~HRandom()
{
    delete this->Generator;
}


float HRandom::RandomFloatInRange(float Min, float Max)
{
    std::uniform_real_distribution<float> distribution(Min, Max);
    return distribution(*static_cast<std::default_random_engine*>(this->Generator));
}


double HRandom::RandomDoubleInRange(double Min, double Max)
{
    std::uniform_real_distribution<double> distribution(Min, Max);
    return distribution(*static_cast<std::default_random_engine*>(this->Generator));
}


int32 HRandom::RandomIntInRange(int32 Min, int32 Max)
{
    std::uniform_int_distribution<int32> distribution(Min, Max);
    return distribution(*static_cast<std::default_random_engine*>(this->Generator));
}


int64 HRandom::RandomInt64InRange(int64 Min, int64 Max)
{
    std::uniform_int_distribution<int64> distribution(Min, Max);
    return distribution(*static_cast<std::default_random_engine*>(this->Generator));
}


bool HRandom::RandomBool()
{
    std::uniform_int_distribution<int32> distribution(0, 1);
    return (bool) distribution(*static_cast<std::default_random_engine*>(this->Generator));
}


bool HRandom::RandomBoolWithWeight(double Weight)
{
    if (Weight <= 0)
    {   
        return false;
    }
    else
    {
        return HRandom::RandomFloatInRange(.0f, 1.0f) <= Weight;
    }
}