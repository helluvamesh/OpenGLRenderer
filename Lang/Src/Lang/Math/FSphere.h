#pragma once


struct FSphere : public FFloatStruct
{
public:
	FVec3 Center;
	float Radius;

	FSphere();
	FSphere(const FVec3& InCenter, float InRadius);

	DString ToString() const;

};
