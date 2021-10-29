#pragma once

#include "HSceneNode.h"
#include "DCameraRenderData.h"


struct DCameraSettings
{
	float FOV = FMath::PI * .25f;
	int32 Width = 800;
	int32 Height = 600;
	float NearClip = 0.5f;
	float FarClip = 1000.0f;
};


class HCameraNode : public HSceneNode
{
private:
	float FOV;
	int32 Width;
	int32 Height;
	float NearClip;
	float FarClip;
	DCameraRenderData CameraData;

	void UpdateRenderData();

protected:
	void OnTransform() override;

public:
	HCameraNode(const DCameraSettings& Settings);

	const DCameraRenderData& GetCameraData();
	const FMat4& GetViewMatrix() const;
	const FMat4& GetProjectionMatrix() const;

	float GetFOV();
	void SetFOV(float Value);

	int32 GetWidth();
	void SetWidth(int32 Value);

	int32 GetHeight();
	void SetHeight(int32 Value);

	float GetNearClip();
	void SetNearClip(float Value);

	float GetFarClip();
	void SetFarClip(float Value);
};