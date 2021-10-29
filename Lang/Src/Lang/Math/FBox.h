#pragma once


struct FBox : public FFloatStruct
{
public:

	DFixedArray<FVec3, 8> Points;  // points in no particular order

	FBox();
	FBox(
		const FVec3& P0, const FVec3& P1, const FVec3& P2, const FVec3& P3, 
		const FVec3& P4, const FVec3& P5, const FVec3& P6, const FVec3& P7
	);
	
	static FBox FromAxisBox(const FAxisBox& AxisBox);

	FVec3 Center() const;
	FVec3 Dimensions() const;

	FAxisBox ToAxisBox() const;

	DString ToString() const;

};
