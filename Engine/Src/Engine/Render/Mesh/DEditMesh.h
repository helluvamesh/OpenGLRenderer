#pragma once

#include "DStaticMeshData.h"


struct DEditMeshTriangle
{
	int32 VertIndex0 = 0;
	int32 VertIndex1 = 0;
	int32 VertIndex2 = 0;

	int32 GetVertByIndex(int32 Index) const;
	void SetVertByIndex(int32 Index, int32 Value);
	int32 FindThirdVertIndex(int32 Index1, int32 Index2) const;
};


struct DEditMeshUVLayer
{
	DArray<FVec2> UVs;
};


struct DEditSubMesh
{
	DString Name;
	DArray<FVec3> Positions;
	DArray<FVec3> Normals;
	DArray<FVec3> Tangents;
	DArray<FVec4> Colors;
	DArray<DEditMeshUVLayer> UVLayers;
	DArray<DEditMeshTriangle> Tris;

	DEditSubMesh Simplified() const;
};


struct DEditMesh
{
	DString Name;
	DArray<DEditSubMesh> SubMeshes;

	//DEditMesh Simplified() const;

	DStaticMeshData ToStaticMeshData();
	static DEditMesh FromStaticMeshData(DStaticMeshData& StaticMeshData);
	DString MakeStatisticsString();
};
