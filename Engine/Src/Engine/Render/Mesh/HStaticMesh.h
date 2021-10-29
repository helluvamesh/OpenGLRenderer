#pragma once

#include "Engine/Asset/HAsset.h"
#include "Engine/Render/Shader/HSurfaceMaterial.h"
#include "DStaticMeshAssetData.h"
#include "DStaticMeshData.h"
#include "ISurface.h"


class HStaticSubMesh : public HObject, public ISurface
{
private:
	uint32 VertArrayId;
	uint32 VertBufferId;
	uint32 IndexBufferId;
	int32 IndexBufferLength;
	
public:
	HStaticSubMesh(const DStaticSubMeshAssetData& Data);
	HStaticSubMesh(const DStaticSubMeshData& Data);
	~HStaticSubMesh();

	// ISurface
	// a shader must be 'used'
	void Draw(HSurfaceMaterial* Material) override;
	// a shader must be 'used'
	void DrawStandalone() override;
	// .
	
	bool IsLoaded();
};


struct DStaticMeshMaterialSlot
{
public:
	DString Name;
	shared<HSurfaceMaterial> Material;
	shared<HStaticSubMesh> Submesh;
};


class HStaticMesh : public HAsset
{
private:
	FAxisBox BoundingBox;
	DArray<DStaticMeshMaterialSlot> MaterialSlots;

public:
	// a shader must be 'used'
	void DrawStandalone();

	void Load() override;
	bool IsLoaded() override;
	void Unload() override;

	void Load(const DStaticMeshData& MeshData);
	const FAxisBox& GetBoundingBox() const;
	const DArray<DStaticMeshMaterialSlot>& GetMaterialSlots() const;

	void SetMaterialInSlot(int32 Index, shared<HSurfaceMaterial> NewMaterial);
	void SetMaterialSlotName(int32 Index, DStringView NewName);

};
