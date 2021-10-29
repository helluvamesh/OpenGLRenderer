#include "pch.h"
#include "FBox.h"


FBox::FBox()
{
}


FBox::FBox(
	const FVec3& P0, const FVec3& P1, const FVec3& P2, const FVec3& P3, 
	const FVec3& P4, const FVec3& P5, const FVec3& P6, const FVec3& P7
)
{
	this->Points[0] = P0;
	this->Points[1] = P1;
	this->Points[2] = P2;
	this->Points[3] = P3;
	this->Points[4] = P4;
	this->Points[5] = P5;
	this->Points[6] = P6;
	this->Points[7] = P7;
}


FBox FBox::FromAxisBox(const FAxisBox& AxisBox)
{
	return FBox(
		FVec3(AxisBox.Min.X, AxisBox.Min.Y, AxisBox.Min.Z),
		FVec3(AxisBox.Max.X, AxisBox.Min.Y, AxisBox.Min.Z),
		FVec3(AxisBox.Max.X, AxisBox.Max.Y, AxisBox.Min.Z),
		FVec3(AxisBox.Min.X, AxisBox.Max.Y, AxisBox.Min.Z),

		FVec3(AxisBox.Max.X, AxisBox.Max.Y, AxisBox.Max.Z),
		FVec3(AxisBox.Min.X, AxisBox.Max.Y, AxisBox.Max.Z),
		FVec3(AxisBox.Min.X, AxisBox.Min.Y, AxisBox.Max.Z),
		FVec3(AxisBox.Max.X, AxisBox.Min.Y, AxisBox.Max.Z)
	);
}


FVec3 FBox::Center() const
{
	FVec3 sum;
	for (cint i = 0; i < 8; ++i)
	{
		sum += this->Points[i];
	}
	return sum / 8;
}


FVec3 FBox::Dimensions() const
{
	return this->ToAxisBox().Dimensions();
}


FAxisBox FBox::ToAxisBox() const
{
	float minX =  FLT_MAX;
	float minY =  FLT_MAX;
	float minZ =  FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	float maxZ = -FLT_MAX;

	for (const FVec3& p : this->Points)
	{
		if (p.X < minX) minX = p.X;
		else if (p.X > maxX) maxX = p.X;

		if (p.Y < minY) minY = p.Y;
		else if (p.Y > maxY) maxY = p.Y;

		if (p.Z < minZ) minZ = p.Z;
		else if (p.Z > maxZ) maxZ = p.Z;
	}

	return FAxisBox( 
		minX, minY, minZ,
		maxX, maxY, maxZ
	);
}
