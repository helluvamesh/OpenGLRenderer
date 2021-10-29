#include "Engine/pch.h"
#include "HStaticMeshNode.h"


HStaticMeshNode::HStaticMeshNode() 
    : HSurfaceNode(ESceneNodeType::STATIC_MESH)
{
}


void HStaticMeshNode::OnTransform()
{
    this->UpdateBounds();
}


void HStaticMeshNode::UpdateBounds()
{
    if (this->Mesh)
    {
        FAxisBox localBox = this->Mesh->GetBoundingBox();
        const FMat4& worldMat = this->GetWorldMatrix();

        // box
        this->WorldBoundingBox = FBox::FromAxisBox(localBox);
        for (FVec3& p : this->WorldBoundingBox.Points)
        {
            p = worldMat.TransformPos(p);
        }

        // sphere
        this->WorldBoundingSphere.Center = worldMat.TransformPos(localBox.Center());
        FVec3 dims = this->WorldBoundingBox.Dimensions();
        float maxDim = dims.X;
        if (dims.Y > maxDim) maxDim = dims.Y;
        if (dims.Z > maxDim) maxDim = dims.Z;
        this->WorldBoundingSphere.Radius = maxDim;
    }
}


void HStaticMeshNode::UpdateMaterialSlots()
{
    if (this->Mesh)
    {
        const DArray<DStaticMeshMaterialSlot>& meshSlots = this->Mesh->GetMaterialSlots();
        this->MaterialSlots.Resize(meshSlots.Length());
        for (int32 i = 0; i < meshSlots.Length(); ++i)
        {
            const DStaticMeshMaterialSlot& meshSlot = meshSlots[i];
            DStaticMeshNodeMaterialSlot& nodeSlot = this->MaterialSlots[i];
            nodeSlot.Submesh = meshSlot.Submesh;
            if (nodeSlot.Material == nullptr)
            {
                nodeSlot.Material = meshSlot.Material;
            }
        }
    }
}


const FBox& HStaticMeshNode::GetBoundingBox() const
{
    return this->WorldBoundingBox;
}


const FSphere& HStaticMeshNode::GetBoundingSphere() const
{
    return this->WorldBoundingSphere;
}


const FAxisBox& HStaticMeshNode::GetLocalBoundingBox() const
{
    if (this->Mesh)
    {
        return this->Mesh->GetBoundingBox();
    }
    return FAxisBox();
}


shared<HStaticMesh> HStaticMeshNode::GetMesh() const
{
    return this->Mesh;
}
HStaticMesh* HStaticMeshNode::GetMeshPtr() const
{
    return this->Mesh.Ptr();
}
void HStaticMeshNode::SetMesh(shared<HStaticMesh> NewMesh)
{
    if (this->Mesh != NewMesh)
    {
        this->MaterialSlots.Clear();
    }
    this->Mesh = NewMesh;
    this->UpdateMaterialSlots();
    this->UpdateBounds();
}


bool HStaticMeshNode::GetCastShadow() const
{
    return this->CastShadow;
}
void HStaticMeshNode::SetCastShadow(bool Value)
{
    this->CastShadow = Value;
}


const DArray<DStaticMeshNodeMaterialSlot>& HStaticMeshNode::GetMaterialSlots() const
{
    return this->MaterialSlots;
}


void HStaticMeshNode::SetMaterialOverride(int32 Index, shared<HSurfaceMaterial> Material)
{
    if (this->MaterialSlots.IsValidIndex(Index))
    {
        this->MaterialSlots[Index].Material = Material;
    }
    else
    {
        SLog::Message("StaticMeshNode has no material override: " + DNumberFormat8::DEFAULT.ToString(Index));
    }
}
