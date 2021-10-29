#include "Engine\pch.h"
#include "HLineWorkNode.h"


HLineWorkNode::HLineWorkNode() :
	HSurfaceNode(ESceneNodeType::LINE_WORK)
{
}


void HLineWorkNode::UpdateBounds()
{
    if (this->LineWork)
    {
        FAxisBox localBox = this->LineWork->GetBoundingBox();
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


void HLineWorkNode::OnTransform()
{
	this->UpdateBounds();
}


shared<HLineWork> HLineWorkNode::GetLineWork() const
{
	return this->LineWork;
}
HLineWork* HLineWorkNode::GetLineWorkPtr() const
{
    return this->LineWork.Ptr();
}
void HLineWorkNode::SetLineWork(shared<HLineWork> NewLineWork)
{
	this->LineWork = NewLineWork;
	this->UpdateBounds();
}


const FBox& HLineWorkNode::GetBoundingBox() const
{
	return this->WorldBoundingBox;	
}


const FSphere& HLineWorkNode::GetBoundingSphere() const
{
    return this->WorldBoundingSphere;
}


const FAxisBox& HLineWorkNode::GetLocalBoundingBox() const
{
    if (this->LineWork)
    {
        return this->LineWork->GetBoundingBox();
    }
    return FAxisBox::DEFAULT;
}
