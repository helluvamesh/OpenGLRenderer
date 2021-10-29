#pragma once


struct DLineStrokeData
{
	DArray<FVec3> Points;
	DArray<uint32> Indices;
	float LineWidth = 1.0f;

	FAxisBox CalcBoundingBox() const;
	static void BracketsFromBounds(const FAxisBox& LocalBounds, DLineStrokeData& OutData);
};


struct DLineWorkData
{
	DLineStrokeData Stroke;
	FAxisBox BoundingBox;

	void UpdateBounds();
};
