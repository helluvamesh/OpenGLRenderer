#pragma once

#include "HSceneNode.h"
#include "Engine/Render/Mesh/HStaticMesh.h"
#include "Engine/Render/Shader/HSurfaceMaterial.h"
#include "HSurfaceNode.h"


struct DStaticMeshNodeMaterialSlot
{
public:
	shared<HSurfaceMaterial> Material;
	shared<HStaticSubMesh> Submesh;
};


class HStaticMeshNode : public HSurfaceNode
{
	friend class HScene;

private:
	FBox WorldBoundingBox;
	FSphere WorldBoundingSphere;

	shared<HStaticMesh> Mesh;
	DArray<DStaticMeshNodeMaterialSlot> MaterialSlots;
	bool CastShadow = true;
	
public:
	HStaticMeshNode();

protected:
	virtual void OnTransform() override;

public:
	void UpdateBounds();

public:
	void UpdateMaterialSlots();
	
	const FBox& GetBoundingBox() const override;
	const FSphere& GetBoundingSphere() const override;
	const FAxisBox& GetLocalBoundingBox() const override;
	
	shared<HStaticMesh> GetMesh() const;
	HStaticMesh* GetMeshPtr() const;
	void SetMesh(shared<HStaticMesh> NewMesh);

	bool GetCastShadow() const;
	void SetCastShadow(bool Value);
	
	const DArray<DStaticMeshNodeMaterialSlot>& GetMaterialSlots() const;
	void SetMaterialOverride(int32 Index, shared<HSurfaceMaterial> Material);

};