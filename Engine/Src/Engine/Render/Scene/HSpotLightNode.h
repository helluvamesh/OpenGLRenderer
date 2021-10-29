#pragma once

#include "HLightNode.h"
#include "Engine\Render\Texture\HFrameBufferDepth.h"
#include "Engine\Render\Texture\HRenderTarget2DArray.h"
#include "Engine\Render\Shader\HSurfaceShader.h"
#include "Engine\Render\RenderUtil\GLMath.h"


struct DSpotLightSettings : public DLightNodeSettings
{
	float LightDistance = 10.0f;
	float InnerCone = FMath::PI / 18;
	float OuterCone = FMath::PI / 4;

	bool EnableShadows = false;
	int32 ShadowResolution = 1024;
	float ShadowBiasMin = .005f;
	float ShadowBiasMax = .02f;
	float ShadowNearClip = .5f;
	float ShadowFarClip = 100.0f;
};


class HSpotLightNode : public HLightNode
{
	friend class HScene;

private:
	float LightDistance;
	float InnerCone;
	float OuterCone;

	bool EnableShadows;
	int32 ShadowResolution;
	float ShadowBiasMin;
	float ShadowBiasMax;
	float ShadowNearClip;
	float ShadowFarClip;

	FMat4 LightSpaceMat;
	DRenderTarget2DArraySettings ShadowMapSettings;
	shared<HRenderTarget2DArray> ShadowMap;
	shared<HFrameBufferDepth> ShadowFBO;
	DFrustumPlanes FrustumPlanes;

public:
	HSpotLightNode(const DSpotLightSettings& Settings);

private:
	void UpdateLightSpaceMatrix();
	void CreateShadowBuffers();
	void DeleteShadowBuffers();
	void OnTransform() override;

public:
	void ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex, int32 ShadowIndex);

	float GetInnerCone() const;
	void SetInnerCone(float Value);

	float GetOuterCone() const;
	void SetOuterCone(float Value);

	bool GetEnableShadows() const;
	void SetEnableShadows(bool Value);

	int32 GetShadowResolution() const;
	void SetShadowResolution(int32 Value);

	float GetShadowBiasMin() const;
	void SetShadowBiasMin(float Value);

	float GetShadowBiasMax() const;
	void SetShadowBiasMax(float Value);

	float GetShadowNearClip() const;
	void SetShadowNearClip(float Value);

	float GetShadowFarClip() const;
	void SetShadowFarClip(float Value);

	const FMat4& GetLightSpaceMat() const;

	shared<HRenderTarget2DArray> GetShadowMap() const;
	HRenderTarget2DArray* GetShadowMapPtr() const;

	shared<HFrameBufferDepth> GetShadowFBO() const;
	HFrameBufferDepth* GetShadowFBOPtr() const;

	const DFrustumPlanes& GetFrustum() const;
};
