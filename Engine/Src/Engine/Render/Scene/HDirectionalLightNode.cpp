#include "Engine\pch.h"
#include "HDirectionalLightNode.h"
#include "HCameraNode.h"
#include "HScene.h"


HDirectionalLightNode::HDirectionalLightNode(const DDirectionalLightSettings& Settings) :
	HLightNode(ELightType::DIRECTIONAL)
{
	this->LightIntensity = Settings.LightIntensity;
	this->LightColor = Settings.LightColor;

	this->EnableShadows = Settings.EnableShadows;
	this->ShadowResolution = Settings.ShadowResolution;
	this->ShadowDistance = Settings.ShadowDistance;
	this->ShadowExtension = Settings.ShadowExtension;
	this->ShadowCascades = Settings.ShadowCascades;
	this->CascadeDistribution = Settings.CascadeDistribution;

	int32 overlapMin = FMath::Min(MAX_SHADOW_CASCADES, Settings.ShadowBiasesMin.Length());
	for (int32 i = 0; i < overlapMin; ++i)
	{
		this->ShadowBiasesMin[i] = Settings.ShadowBiasesMin[i];
	}
	int32 overlapMax = FMath::Min(MAX_SHADOW_CASCADES, Settings.ShadowBiasesMax.Length());
	for (int32 i = 0; i < overlapMax; ++i)
	{
		this->ShadowBiasesMax[i] = Settings.ShadowBiasesMax[i];
	}

	if (this->EnableShadows)
		this->CreateShadowBuffers();
}


void HDirectionalLightNode::UpdateLightSpaceMatrices()
{
	shared<HScene> scene = this->GetScene();
	if (scene == nullptr) return;
	HCameraNode* cam = this->GetScene()->GetActiveCameraPtr();
	if (cam == nullptr) return;
	const DCameraRenderData& camData = cam->GetCameraData();

	float nearClip = cam->GetNearClip();
	float farClip = cam->GetFarClip();
	float fov = cam->GetFOV();
	float width = (float)cam->GetWidth();
	float height = (float)cam->GetHeight();

	// calc cascade ranges
	DFixedArray<DFixedArray<float, 2>, 4> ranges;

	DFixedArray<float, 4> multipliers { 
		1.0f, 
		2.0f * this->CascadeDistribution, 
		4.0f * this->CascadeDistribution, 
		16.0f * this->CascadeDistribution 
	};
	float segments = 0;
	for (int32 i = 0; i < this->ShadowCascades; ++i)
	{
		segments += multipliers[i];
	}
	float segment = this->ShadowDistance / segments;

	ranges[0][0] = nearClip;
	ranges[0][1] = segment;
	float prevSpan = segment;
	for (int32 i = 1; i < this->ShadowCascades; ++i)
	{
		float span = segment * multipliers[i];
		ranges[i][1] = ranges[i - 1][1] + span;
		ranges[i][0] = ranges[i - 1][1];
		prevSpan = span;
	}

	// calc cascade distances
	for (int32 i = 0; i < this->ShadowCascades - 1; ++i)
	{
		this->CascadeDistances[i] = ranges[i][1];
	}
	this->CascadeDistances[this->ShadowCascades - 1] = this->ShadowDistance;

	// calc light space matrices
	for (int32 i = 0; i < this->ShadowCascades; ++i)
	{
		float far = ranges[i][1];

		FMat4 projMat = GLMath::PerspectiveMatrix(fov, width / height, nearClip, far);

		FBox frustumCorners = GLMath::GetFrustumInWorldSpace(projMat, camData.ViewMatrix);

		FVec3 center = frustumCorners.Center();
		FMat4 lightViewMat = GLMath::LookAtMatrix(
			center,
			center + this->CalcForward(),
			HSceneNode::UP
		);

		for (FVec3& point : frustumCorners.Points)
		{
			point = lightViewMat.TransformPos(point);
		}
		FAxisBox frustumBounds = frustumCorners.ToAxisBox();

		FVec3& min = frustumBounds.Min;
		FVec3& max = frustumBounds.Max;

		min.Z -= this->ShadowExtension;
		max.Z += this->ShadowExtension;

		FMat4 lightProjMat = GLMath::OrthographicMatrix(min.X, max.X, min.Y, max.Y, min.Z, max.Z);

		this->LightSpaceMats[i] = lightProjMat * lightViewMat;
	}

	// calc frustum planes
	float far = ranges[this->ShadowCascades - 1][1];
	FMat4 projMat = GLMath::PerspectiveMatrix(fov, width / height, nearClip, far);

	FBox frustumCorners = GLMath::GetFrustumInWorldSpace(projMat, camData.ViewMatrix);

	FVec3 center = frustumCorners.Center();
	FMat4 lightViewMat = GLMath::LookAtMatrix(
		center,
		center + this->CalcForward(),
		HSceneNode::UP
	);

	for (FVec3& point : frustumCorners.Points)
	{
		point = lightViewMat.TransformPos(point);
	}
	FAxisBox frustumBounds = frustumCorners.ToAxisBox();

	FVec3& min = frustumBounds.Min;
	FVec3& max = frustumBounds.Max;

	min.Z -= this->ShadowExtension;
	max.Z += this->ShadowExtension;

	FMat4 lightProjMat = GLMath::OrthographicMatrix(min.X, max.X, min.Y, max.Y, min.Z, max.Z);

	this->FrustumPlanes.Populate(lightViewMat, lightProjMat);
}


void HDirectionalLightNode::CreateShadowBuffers()
{
	this->ShadowMapSettings.Width = this->ShadowResolution;
	this->ShadowMapSettings.Height = this->ShadowResolution;
	this->ShadowMapSettings.Format = ERenderTarget2DArrayFormat::DEPTH;
	this->ShadowMapSettings.Length = MAX_SHADOW_CASCADES;
	this->ShadowMapSettings.Wrap = ETextureWrap::CLAMP_TO_BORDER;
	this->ShadowMapSettings.BorderColor = FVec4(1.0f);

	this->ShadowMap = MakeShared<HRenderTarget2DArray>();
	this->ShadowMap->SetName(this->Name + TX("_CascadedShadowMap"));
	this->ShadowMap->Load(this->ShadowMapSettings);

	this->ShadowFBO = MakeShared<HFrameBufferDepth>();
	this->ShadowFBO->SetName(this->Name + TX("_ShadowFBO"));
	this->ShadowFBO->SetDepthAttachment(this->ShadowMap);
	this->ShadowFBO->Prepare(this->ShadowResolution, this->ShadowResolution);
	this->ShadowFBO->AttachTextures();
}


void HDirectionalLightNode::DeleteShadowBuffers()
{
	this->ShadowFBO = nullptr;
	this->ShadowMap = nullptr;
}


void HDirectionalLightNode::ScheduleMatrixUpdate()
{
	if (this->EnableShadows)
	{
		shared<HScene> scene = this->GetScene();
		if (scene)
		{
			DTimerDelegate del (WeakThisAs<HDirectionalLightNode>(), &HDirectionalLightNode::UpdateLightSpaceMatrices);
			this->TimerId = scene->GetTimer()->ScheduleIntervalMS(0, 1, del);
		}
	}
}


void HDirectionalLightNode::UncheduleMatrixUpdate()
{
	shared<HScene> scene = this->GetScene();
	if (scene)
	{
		scene->GetTimer()->Cancel(this->TimerId);
	}
}


void HDirectionalLightNode::OnEnteringScene()
{
	if (this->EnableShadows)
		this->ScheduleMatrixUpdate();
}


void HDirectionalLightNode::OnLeavingScene()
{
	this->UncheduleMatrixUpdate();
}


void HDirectionalLightNode::ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex, bool ApplyShadows)
{
	if (Shader)
	{
		// light
		FVec3 color (
			this->LightColor.X * this->LightIntensity, 
			this->LightColor.Y * this->LightIntensity, 
			this->LightColor.Z * this->LightIntensity
		);
		FVec3 direction = this->CalcForward();
		direction.Invert();

		const DBaseSurfaceShaderLocs& locs = Shader->GetBaseUniformLocs();
		Shader->SetVec3Param(locs.DirLights[LightIndex].Color, color);
		Shader->SetVec3Param(locs.DirLights[LightIndex].Direction, direction);

		// shadow
		if (ApplyShadows)
		{
			Shader->SetIntParam(locs.ShadowedDirLightIdx, LightIndex);
			Shader->SetTextureParam(locs.DirLightShadowMapTexUnit, this->ShadowMap.Ptr());
			Shader->SetFloatArrayParam(locs.DirLightShadowBiasesMin, this->ShadowBiasesMin.Data(), this->ShadowCascades);
			Shader->SetFloatArrayParam(locs.DirLightShadowBiasesMax, this->ShadowBiasesMax.Data(), this->ShadowCascades);
			Shader->SetMat4ArrayParam(locs.DirLightSpaceMats, this->LightSpaceMats.Data(), this->ShadowCascades);
			Shader->SetIntParam(locs.DirLightCascadeCount, this->ShadowCascades);
			Shader->SetFloatArrayParam(locs.DirLightCascadeDistances, this->CascadeDistances.Data(), this->ShadowCascades);
		}
	}
}


bool HDirectionalLightNode::GetEnableShadows() const
{
	return this->EnableShadows;
}
void HDirectionalLightNode::SetEnableShadows(bool Value)
{
	if (Value)
	{
		if (this->EnableShadows == false)
		{
			this->CreateShadowBuffers();
			this->ScheduleMatrixUpdate();
		}
	}
	else
	{
		this->DeleteShadowBuffers();
		this->UncheduleMatrixUpdate();
	}
	this->EnableShadows = Value;
}


int32 HDirectionalLightNode::GetShadowResolution() const
{
	return this->ShadowResolution;
}
void HDirectionalLightNode::SetShadowResolution(int32 Value)
{
	this->ShadowResolution = Value;
	if (this->EnableShadows)
	{
		if (this->ShadowMap && this->ShadowFBO)
		{
			this->ShadowMapSettings.Width = this->ShadowMapSettings.Height = FMath::Clamp(Value, 16, MAX_SHADOW_MAP_RESOLUTION);
			this->ShadowMap->Load(this->ShadowMapSettings);
			this->ShadowFBO->Prepare(Value, Value);
			this->ShadowFBO->AttachTextures();
		}
	}
}


float HDirectionalLightNode::GetShadowDistance() const
{
	return this->ShadowDistance;
}
void HDirectionalLightNode::SetShadowDistance(float Value)
{
	this->ShadowDistance = Value;
}


float HDirectionalLightNode::GetShadowExtension() const
{
	return this->ShadowExtension;
}
void HDirectionalLightNode::SetShadowExtension(float Value)
{
	this->ShadowExtension = Value;
}


float HDirectionalLightNode::GetShadowBiasMin(int32 CascadeIndex) const
{
	return this->ShadowBiasesMin[CascadeIndex];
}
void HDirectionalLightNode::SetShadowBiasMin(int32 CascadeIndex, float Value)
{
	this->ShadowBiasesMin[CascadeIndex] = Value;
}


float HDirectionalLightNode::GetShadowBiasMax(int32 CascadeIndex) const
{
	return this->ShadowBiasesMax[CascadeIndex];
}
void HDirectionalLightNode::SetShadowBiasMax(int32 CascadeIndex, float Value)
{
	this->ShadowBiasesMax[CascadeIndex] = Value;
}



int32 HDirectionalLightNode::GetShadowCascades() const
{
	return this->ShadowCascades;
}
void HDirectionalLightNode::SetShadowCascades(int32 Value)
{
	this->ShadowCascades = FMath::Clamp(Value, 1, MAX_SHADOW_CASCADES);
}


float HDirectionalLightNode::GetCascadeDistribution() const
{
	return this->CascadeDistribution;
}
void HDirectionalLightNode::SetCascadeDistribution(float Value)
{
	this->CascadeDistribution = Value;
}


const DFixedArray<FMat4, 4>& HDirectionalLightNode::GetLightSpaceMats() const
{
	return this->LightSpaceMats;
}


shared<HRenderTarget2DArray> HDirectionalLightNode::GetShadowMap() const
{
	return this->ShadowMap;
}
HRenderTarget2DArray* HDirectionalLightNode::GetShadowMapPtr() const
{
	return this->ShadowMap.Ptr();
}


shared<HFrameBufferDepth> HDirectionalLightNode::GetShadowFBO() const
{
	return this->ShadowFBO;
}
HFrameBufferDepth* HDirectionalLightNode::GetShadowFBOPtr() const
{
	return this->ShadowFBO.Ptr();
}


const DFrustumPlanes& HDirectionalLightNode::GetFrustum() const
{
	return this->FrustumPlanes;
}
