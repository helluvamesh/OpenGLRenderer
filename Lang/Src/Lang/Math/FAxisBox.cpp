#include "pch.h"
#include "FAxisBox.h"


const FAxisBox FAxisBox::DEFAULT;


FAxisBox::FAxisBox()
{
}


FAxisBox::FAxisBox(const FVec3& InMin, const FVec3& InMax) 
	: Min(InMin), Max(InMax)
{
}


FAxisBox::FAxisBox(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ)
	: Min(MinX, MinY, MinZ), Max(MaxX, MaxY, MaxZ)
{
}


void FAxisBox::Extend(const FAxisBox& Other)
{
	if (Other.Min.X < this->Min.X)
	{
		this->Min.X = Other.Min.X;
	}
	if (Other.Min.Y < this->Min.Y)
	{
		this->Min.Y = Other.Min.Y;
	}
	if (Other.Min.Z < this->Min.Z)
	{
		this->Min.Z = Other.Min.Z;
	}

	if (Other.Max.X > this->Max.X)
	{
		this->Max.X = Other.Max.X;
	}
	if (Other.Max.Y > this->Max.Y)
	{
		this->Max.Y = Other.Max.Y;
	}
	if (Other.Max.Z > this->Max.Z)
	{
		this->Max.Z = Other.Max.Z;
	}
}


FAxisBox FAxisBox::Extended(const FAxisBox& Other) const
{
	return FAxisBox (
		FMath::Min(this->Min.X, Other.Min.X),
		FMath::Min(this->Min.Y, Other.Min.Y),
		FMath::Min(this->Min.Z, Other.Min.Z),

		FMath::Max(this->Max.X, Other.Max.X),
		FMath::Max(this->Max.Y, Other.Max.Y),
		FMath::Max(this->Max.Z, Other.Max.Z)
	);
}


FVec3 FAxisBox::Center() const
{
	return FVec3(
		(this->Min.X + this->Max.X) * .5f,
		(this->Min.Y + this->Max.Y) * .5f,
		(this->Min.Z + this->Max.Z) * .5f
	);
}


FVec3 FAxisBox::Dimensions() const
{
	return FVec3(
		FMath::Abs(this->Max.X - this->Min.X),
		FMath::Abs(this->Max.Y - this->Min.Y),
		FMath::Abs(this->Max.Z - this->Min.Z)
	);
}


bool FAxisBox::ContainsPoint(const FVec3& Point) const
{
	return 
		this->Min.X <= Point.X && this->Min.Y <= Point.Y && this->Min.Z <= Point.Z &&
		this->Max.X >= Point.X && this->Max.Y >= Point.Y && this->Max.Z >= Point.Z;
}


int32 FAxisBox::GetIntersection(float fDst1, float fDst2, const FVec3& P1, const FVec3& P2, FVec3& Hit)
{
	if ((fDst1 * fDst2) >= 0.0f) return 0;
	if (fDst1 == fDst2) return 0; 
	Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
	return 1;
}


int32 FAxisBox::InBox(const FVec3& Hit, const FVec3& B1, const FVec3& B2, const int32 Axis)
{
	if ( Axis==1 && Hit.Z > B1.Z && Hit.Z < B2.Z && Hit.Y > B1.Y && Hit.Y < B2.Y) return 1;
	if ( Axis==2 && Hit.Z > B1.Z && Hit.Z < B2.Z && Hit.X > B1.X && Hit.X < B2.X) return 1;
	if ( Axis==3 && Hit.X > B1.X && Hit.X < B2.X && Hit.Y > B1.Y && Hit.Y < B2.Y) return 1;
	return 0;
}


bool FAxisBox::LineSegmentIntersect(const FVec3& L1, const FVec3& L2, FVec3& Hit) const
{
	const FVec3& B1 = this->Min;
	const FVec3& B2 = this->Max;

	if (L2.X < B1.X && L1.X < B1.X) return false;
	if (L2.X > B2.X && L1.X > B2.X) return false;
	if (L2.Y < B1.Y && L1.Y < B1.Z) return false;
	if (L2.Y > B2.Y && L1.Y > B2.Z) return false;
	if (L2.Z < B1.Z && L1.Z < B1.Y) return false;
	if (L2.Z > B2.Z && L1.Z > B2.Y) return false;
	if (L1.X > B1.X && L1.X < B2.X &&
		L1.Y > B1.Y && L1.Y < B2.Z &&
		L1.Z > B1.Z && L1.Z < B2.Y) 
	{
		Hit = L1; 
		return true;
	}

	if ( (GetIntersection( L1.X-B1.X, L2.X-B1.X, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
		|| (GetIntersection( L1.Y-B1.Y, L2.Y-B1.Y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 )) 
		|| (GetIntersection( L1.Z-B1.Z, L2.Z-B1.Z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 )) 
		|| (GetIntersection( L1.X-B2.X, L2.X-B2.X, L1, L2, Hit) && InBox( Hit, B1, B2, 1 )) 
		|| (GetIntersection( L1.Y-B2.Y, L2.Y-B2.Y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 )) 
		|| (GetIntersection( L1.Z-B2.Z, L2.Z-B2.Z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 )))
	{
		return true;
	}

	return false;
}
