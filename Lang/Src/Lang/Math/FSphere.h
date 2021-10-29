#pragma once


struct FSphere : public FFloatStruct
{
	static const FSphere DEFAULT;

	FVec3 Center;
	float Radius;

	FSphere();
	FSphere(const FVec3& InCenter, float InRadius);

	DString ToString() const;

};
