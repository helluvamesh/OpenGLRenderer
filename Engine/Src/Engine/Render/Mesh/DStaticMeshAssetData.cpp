#include "Engine\pch.h"
#include "DStaticMeshAssetData.h"


void DStaticSubMeshAssetData::Clear()
{
	this->Positions.Clear();
	this->Normals.Clear();
	this->Tangents.Clear();
	this->Colors.Clear();
	this->UVs.Clear();
	this->UVs_2.Clear();
}


REFLECT(DStaticMeshAssetData)
FIELD(BoundingBox)
FIELD(SubMeshes)
REFLECT_END()


REFLECT(DStaticSubMeshAssetData)
FIELD(Name)
FIELD(FaceIndices)
FIELD(Positions)
FIELD(Normals)
FIELD(UVs)
FIELD(Tangents)
FIELD(Colors)
FIELD(UVs_2)
REFLECT_END()