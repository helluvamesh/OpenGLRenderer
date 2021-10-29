#include "Engine\pch.h"
#include "DLineWorkData.h"


// ------------------ DLineStrokeData --------------------


FAxisBox DLineStrokeData::CalcBoundingBox() const
{
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float minZ = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	float maxZ = -FLT_MAX;

	for (const FVec3& p : this->Points)
	{
		if (p.X < minX)
		{
			minX = p.X;
		}
		else if (p.X > maxX)
		{
			maxX = p.X;
		}

		if (p.Y < minY)
		{
			minY = p.Y;
		}
		else if (p.Y > maxY)
		{
			maxY = p.Y;
		}

		if (p.Z < minZ)
		{
			minZ = p.Z;
		}
		else if (p.Z > maxZ)
		{
			maxZ = p.Z;
		}
	}

	return FAxisBox( 
		minX, minY, minZ,
		maxX, maxY, maxZ
	);
}


void DLineStrokeData::BracketsFromBounds(const FAxisBox& LocalBounds, DLineStrokeData& OutData)
{
	DArray<FVec3>& outPoints = OutData.Points;
	DArray<uint32>& outIndices = OutData.Indices;

	outPoints.Clear();
	outIndices.Clear();

	// extend box - 10 percent or 5 cm
	float extensionLimit = .05f;

	FVec3 center = LocalBounds.Center();
	FVec3 newMin = FVec3::Lerp(center, LocalBounds.Min, 1.1f);
	FVec3 newMax = FVec3::Lerp(center, LocalBounds.Max, 1.1f);
	if (newMin.Distance(LocalBounds.Min) > extensionLimit)
	{
		newMin = newMin + (newMin - LocalBounds.Min).Normalized() * extensionLimit;
		newMax = newMax + (newMax - LocalBounds.Max).Normalized() * extensionLimit;
	}

	FAxisBox bounds (newMin, newMax);
	// .

	FVec3 dims = bounds.Dimensions();
	DFixedArray<float, 3> dimArr { dims.X, dims.Y, dims.Z };
	dimArr.Sort();
	float len = dimArr[0] / 3;

	FBox box = FBox::FromAxisBox(bounds);
	DFixedArray<FVec3, 8>& pts = box.Points;  // 8 points
	
	DFixedArray<DFixedArray<int32, 3>, 8> map {
		{ 0, 1, 3 },
		{ 1, 2, 0 },
		{ 2, 3, 1 },
		{ 3, 0, 2 },

		{ 4, 5, 7 },
		{ 5, 6, 4 },
		{ 6, 7, 5 },
		{ 7, 4, 6 }
	};

	DFixedArray<DFixedArray<int32, 2>, 4> map2 {
		{ 0, 6 },
		{ 1, 7 },
		{ 2, 4 },
		{ 3, 5 }
	};

	for (DFixedArray<int32, 3>& entry : map)
	{
		outPoints.Add(pts[entry[0]]);
		FVec3 dir = pts[entry[1]] - pts[entry[0]];
		dir.Normalize();
		outPoints.Emplace(pts[entry[0]] + dir * len);
		
		outPoints.Add(pts[entry[0]]);
		FVec3 dir2 = pts[entry[2]] - pts[entry[0]];
		dir2.Normalize();
		outPoints.Emplace(pts[entry[0]] + dir2 * len);
	}

	for (int32 i = 0; i < 4; ++i)
	{
		FVec3 dir = pts[map2[i][1]] - pts[map2[i][0]];
		dir.Normalize();
		outPoints.Add(pts[map2[i][0]]);
		outPoints.Add(pts[map2[i][0]] + dir * len);

		FVec3 dir2 = pts[map2[i][0]] - pts[map2[i][1]];
		dir2.Normalize();
		outPoints.Add(pts[map2[i][1]]);
		outPoints.Add(pts[map2[i][1]] + dir2 * len);
	}

	outIndices.Resize(outPoints.Length());
	for (int32 i = 0; i < outPoints.Length(); ++i)
	{
		outIndices[i] = i;
	}
}


// ------------------ DLineWorkData --------------------


void DLineWorkData::UpdateBounds()
{
	this->BoundingBox = this->Stroke.CalcBoundingBox();
}
