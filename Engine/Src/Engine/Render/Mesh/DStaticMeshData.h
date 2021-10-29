#pragma once

#include "DStaticMeshAssetData.h"
#include "Engine\Util\UtilTypes.h"


struct DStaticMeshTriangle
{
	uint32 VertIndex0 = 0;
	uint32 VertIndex1 = 0;
	uint32 VertIndex2 = 0;
};


struct DStaticMeshUVLayer
{
	DArray<FVec2> UVs;
};


// triangulated editable mesh
struct DStaticSubMeshData
{
	DString Name;
	DArray<FVec3> Positions;
	DArray<FVec3> Normals;
	DArray<FVec3> Tangents;  // Required, if there's a UV Layer
	DArray<FVec4> Colors;  // Optional
	DArray<DStaticMeshUVLayer> UVLayers;  // Optional
	DArray<DStaticMeshTriangle> Tris;

	void Clear();
	void PopulateTangents();
	void Swizzle(EAxis FromRight, EAxis FromUp, EAxis FromForward, float ScaleToMeter);
	void Scale(float ScaleToMeter);

	FAxisBox CalcBoundingBox() const;
	bool DetectMirroredUVs(int32 Index) const;
	void ToStaticSubMeshAssetData(DStaticSubMeshAssetData& OutData) const;
};


struct DStaticMeshData
{
	DString Name;
	DArray<DStaticSubMeshData> SubMeshes;
	FAxisBox BoundingBox;
	
	void ToStaticMeshAssetData(DStaticMeshAssetData& OutData) const;
	void UpdateBounds();
	void Swizzle(EAxis FromRight, EAxis FromUp, EAxis FromForward, float ScaleToMeter);
	void Scale(float ScaleToMeter);
	void Clear();
	bool DetectMirroredUVs();
};
