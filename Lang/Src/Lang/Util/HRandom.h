#pragma once


class HRandom : public HObject
{
private:
    void* Generator;

public:
    HRandom();
    ~HRandom();

    float RandomFloatInRange(float Min, float Max);
    
    double RandomDoubleInRange(double Min, double Max);

    int32 RandomIntInRange(int32 Min, int32 Max);

    int64 RandomInt64InRange(int64 Min, int64 Max);

    bool RandomBool();

    bool RandomBoolWithWeight(double Weight);

};