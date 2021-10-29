#include "Engine/pch.h"
#include "HCameraNode.h"
#include "Engine/Render/RenderUtil/GLUtil.h"
#include "Engine/Render/RenderUtil/GLMath.h"


HCameraNode::HCameraNode(const DCameraSettings& Settings)
    : HSceneNode(ESceneNodeType::CAMERA)
{
    this->SetInheritScale(false);
    this->FOV = Settings.FOV;
    this->Width = Settings.Width;
    this->Height = Settings.Height;
    this->NearClip = Settings.NearClip;
    this->FarClip = Settings.FarClip;
    this->UpdateRenderData();
}


void HCameraNode::UpdateRenderData()
{
    this->CameraData.ProjectionMatrix = GLMath::PerspectiveMatrix(
        this->FOV,
        (float)this->Width / (float)this->Height,
        this->NearClip,
        this->FarClip
    );

    this->CameraData.ViewMatrix = GLMath::ViewMatrix(this->CalcForward(), this->CalcUp(), this->GetWorldLocation());
    this->CameraData.WorldLocation = this->GetWorldLocation();

    this->CameraData.FrustumPlanes.Populate(this->CameraData.ViewMatrix, this->CameraData.ProjectionMatrix);
}


void HCameraNode::OnTransform()
{
    HSceneNode::OnTransform();
    this->UpdateRenderData();
}


const DCameraRenderData& HCameraNode::GetCameraData()
{
    return this->CameraData;
}


const FMat4& HCameraNode::GetViewMatrix() const
{
    return this->CameraData.ViewMatrix;
}


const FMat4& HCameraNode::GetProjectionMatrix() const
{
    return this->CameraData.ProjectionMatrix;
}


float HCameraNode::GetFOV()
{
    return this->FOV;
}
void HCameraNode::SetFOV(float Value)
{
    this->FOV = Value;
    this->UpdateRenderData();
}

int32 HCameraNode::GetWidth()
{
    return this->Width;
}
void HCameraNode::SetWidth(int32 Value)
{
    this->Width = Value;
    this->UpdateRenderData();
}

int32 HCameraNode::GetHeight()
{
    return this->Height;
}
void HCameraNode::SetHeight(int32 Value)
{
    this->Height = Value;
    this->UpdateRenderData();
}

float HCameraNode::GetNearClip()
{
    return this->NearClip;
}
void HCameraNode::SetNearClip(float Value)
{
    this->NearClip = Value;
    this->UpdateRenderData();
}

float HCameraNode::GetFarClip()
{
    return this->FarClip;
}
void HCameraNode::SetFarClip(float Value)
{
    this->FarClip = Value;
    this->UpdateRenderData();
}
