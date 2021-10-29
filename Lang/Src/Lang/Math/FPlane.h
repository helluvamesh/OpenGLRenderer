#pragma once


struct FPlane : public FFloatStruct
{
	static const FPlane DEFAULT;

	float A;
	float B;
	float C;
	float D;

	FORCEINLINE FPlane();
	FORCEINLINE FPlane(float InA, float InB, float InC, float InD);

	FORCEINLINE static FPlane FromPointAndNormal(const FVec3& Point, const FVec3& Normal);

	/* -https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf */
	FORCEINLINE float SignedDistance(const FVec3& Point) const;

	/* -https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf */
	FORCEINLINE void Normalize();
	FORCEINLINE FPlane Normalized();

	DString ToString() const;
};


#include "FPlane.inl"