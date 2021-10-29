#pragma once


// axis-aligned box
struct FAxisBox : public FFloatStruct
{
public:
	FVec3 Min;
	FVec3 Max;

	FAxisBox();
	FAxisBox(const FVec3& InMin, const FVec3& InMax);
	FAxisBox(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ);

	void Extend(const FAxisBox& Other);
	FAxisBox Extended(const FAxisBox& Other) const;
	FVec3 Center() const;
	FVec3 Dimensions() const;
	// on the border is inside
	bool ContainsPoint(const FVec3& Point) const;

	// -http://www.3dkingdoms.com/weekly/weekly.php?a=3

private:
	FORCEINLINE static int32 GetIntersection(float fDst1, float fDst2, const FVec3& P1, const FVec3& P2, FVec3& Hit);
	FORCEINLINE static int32 InBox(const FVec3& Hit, const FVec3& B1, const FVec3& B2, const int32 Axis);
	
public:
	bool LineSegmentIntersect(const FVec3& L1, const FVec3& L2, FVec3& Hit) const;

	DString ToString() const;
};
