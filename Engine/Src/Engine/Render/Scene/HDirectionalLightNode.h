#pragma once

#include "HLightNode.h"
#include "Engine\Render\Texture\HRenderTarget2DArray.h"
#include "Engine\Render\Texture\HFrameBufferDepth.h"
#include "Engine\Render\RenderUtil\GLMath.h"
#include "Engine\Render\Shader\HSurfaceShader.h"


struct DDirectionalLightSettings : public DLightNodeSettings
{
	bool EnableShadows = true;
	int32 ShadowResolution = 2048;
	float ShadowDistance = 500.0f;
	float ShadowExtension = 50.0f;
	DArray<float> ShadowBiasesMin;
	DArray<float> ShadowBiasesMax;
	int32 ShadowCascades = 3;
	float CascadeDistribution = 1.0f;
};


class HDirectionalLightNode : public HLightNode
{
	friend class HScene;

private:
	static const int32 MAX_SHADOW_CASCADES = 4;

	bool EnableShadows;
	int32 ShadowResolution;
	float ShadowDistance;
	float ShadowExtension;

	DFixedArray<FMat4, 4> LightSpaceMats;
	DFixedArray<float, 4> CascadeDistances;
	int32 ShadowCascades;  // [1...4]
	float CascadeDistribution = 1.0f;
	DRenderTarget2DArraySettings ShadowMapSettings;
	shared<HRenderTarget2DArray> ShadowMap;
	shared<HFrameBufferDepth> ShadowFBO;
	DFixedArray<float, 4> ShadowBiasesMin { .005f, .005f, .002f, .001f };
	DFixedArray<float, 4> ShadowBiasesMax { .02f, .01f, .005f, .002f };
	DFrustumPlanes FrustumPlanes;
	uint64 TimerId = 0LL;

public:
	HDirectionalLightNode(const DDirectionalLightSettings& Settings);

private:
	void UpdateLightSpaceMatrices();
	void CreateShadowBuffers();
	void DeleteShadowBuffers();
	void ScheduleMatrixUpdate();
	void UncheduleMatrixUpdate();

protected:
	void OnEnteringScene() override;
	void OnLeavingScene() override;

public:
	void ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex, bool ApplyShadows);

	bool GetEnableShadows() const;
	void SetEnableShadows(bool Value);

	int32 GetShadowResolution() const;
	void SetShadowResolution(int32 Value);

	float GetShadowDistance() const;
	void SetShadowDistance(float Value);

	float GetShadowExtension() const;
	void SetShadowExtension(float Value);

	float GetShadowBiasMin(int32 CascadeIndex) const;
	void SetShadowBiasMin(int32 CascadeIndex, float Value);

	float GetShadowBiasMax(int32 CascadeIndex) const;
	void SetShadowBiasMax(int32 CascadeIndex, float Value);

	int32 GetShadowCascades() const;
	void SetShadowCascades(int32 Value);

	float GetCascadeDistribution() const;
	void SetCascadeDistribution(float Value);

	const DFixedArray<FMat4, 4>& GetLightSpaceMats() const;

	shared<HRenderTarget2DArray> GetShadowMap() const;
	HRenderTarget2DArray* GetShadowMapPtr() const;

	shared<HFrameBufferDepth> GetShadowFBO() const;
	HFrameBufferDepth* GetShadowFBOPtr() const;

	const DFrustumPlanes& GetFrustum() const;
};
