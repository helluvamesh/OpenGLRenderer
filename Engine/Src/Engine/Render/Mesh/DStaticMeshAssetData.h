#pragma once


struct DStaticSubMeshAssetData : public DStruct
{
	REFLECT_H(DStaticSubMeshAssetData, DStruct)
public:
	DString Name;
	DArray<uint32> FaceIndices;
	DArray<float> Positions;
	DArray<int16> Normals;
	DArray<int16> UVs;
	DArray<int16> Tangents;
	DArray<uint8> Colors;
	DArray<int16> UVs_2;

	void Clear();
};


struct DStaticMeshAssetData : public DStruct
{
	REFLECT_H(DStaticMeshAssetData, DStruct)
public:
	FAxisBox BoundingBox;
	DArray<DStaticSubMeshAssetData> SubMeshes;
};
