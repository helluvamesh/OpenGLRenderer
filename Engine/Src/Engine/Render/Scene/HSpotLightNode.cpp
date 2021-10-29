#include "Engine\pch.h"
#include "HSpotLightNode.h"
#include "HScene.h"


HSpotLightNode::HSpotLightNode(const DSpotLightSettings& Settings) :
	HLightNode(ELightType::SPOT)
{
	this->LightIntensity = Settings.LightIntensity;
	this->LightColor = Settings.LightColor;

	this->LightDistance = Settings.LightDistance;
	this->InnerCone = Settings.InnerCone;
	this->OuterCone = Settings.OuterCone;

	this->EnableShadows = Settings.EnableShadows;
	this->ShadowResolution = Settings.ShadowResolution;
	this->ShadowBiasMin = Settings.ShadowBiasMin;
	this->ShadowBiasMax = Settings.ShadowBiasMax;
	this->ShadowNearClip = Settings.ShadowNearClip;
	this->ShadowFarClip = Settings.ShadowFarClip;

	this->UpdateLightSpaceMatrix();
	if (this->EnableShadows)
		this->CreateShadowBuffers();
}


void HSpotLightNode::UpdateLightSpaceMatrix()
{
	shared<HScene> scene = this->GetScene();
	if (scene == nullptr) return;

	FMat4 lightProjMat = GLMath::PerspectiveMatrix(
		this->OuterCone * 2, 1.0f, this->ShadowNearClip, this->ShadowFarClip
	);
	FMat4 lightViewMat = GLMath::LookAtMatrix(
		this->GetWorldLocation(),
		this->GetWorldLocation() - this->CalcForward(),
		HSceneNode::UP
	);

	this->LightSpaceMat = lightProjMat * lightViewMat;

	this->FrustumPlanes.Populate(lightViewMat, lightProjMat);
}


void HSpotLightNode::CreateShadowBuffers()
{
	this->ShadowMapSettings.Width = this->ShadowResolution;
	this->ShadowMapSettings.Height = this->ShadowResolution;
	this->ShadowMapSettings.Format = ERenderTarget2DArrayFormat::DEPTH;
	this->ShadowMapSettings.Length = 1;
	this->ShadowMapSettings.Wrap = ETextureWrap::CLAMP_TO_BORDER;
	this->ShadowMapSettings.BorderColor = FVec4(1.0f);

	this->ShadowMap = MakeShared<HRenderTarget2DArray>();
	this->ShadowMap->SetName(this->Name + TX("_ShadowMap"));
	this->ShadowMap->Load(this->ShadowMapSettings);

	this->ShadowFBO = MakeShared<HFrameBufferDepth>();
	this->ShadowFBO->SetName(this->Name + TX("_ShadowFBO"));
	this->ShadowFBO->SetDepthAttachment(this->ShadowMap);
	this->ShadowFBO->Prepare(this->ShadowResolution, this->ShadowResolution);
	this->ShadowFBO->AttachTextures();
}


void HSpotLightNode::DeleteShadowBuffers()
{
	this->ShadowMap = nullptr;
	this->ShadowFBO = nullptr;
}


void HSpotLightNode::OnTransform()
{
	this->UpdateLightSpaceMatrix();
}


void HSpotLightNode::ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex, int32 ShadowIndex)
{
	if (Shader)
	{
		// light
		FVec3 color (
			this->LightColor.X * this->LightIntensity, 
			this->LightColor.Y * this->LightIntensity, 
			this->LightColor.Z * this->LightIntensity
		);
		FVec3 position = this->GetWorldLocation();
		FVec3 direction = this->CalcForward();
		direction.Invert();
		float cos1 = FMath::Cos(this->InnerCone);
		float cos2 = FMath::Cos(this->OuterCone);

		const DBaseSurfaceShaderLocs& locs = Shader->GetBaseUniformLocs();
		Shader->SetVec3Param(locs.SpotLights[LightIndex].Color, color);
		Shader->SetVec3Param(locs.SpotLights[LightIndex].Position, position);
		Shader->SetVec3Param(locs.SpotLights[LightIndex].Direction, direction);
		Shader->SetVec2Param(locs.SpotLights[LightIndex].Cosines, cos1, cos2);

		Shader->SetIntParam(locs.SpotLights[LightIndex].ShadowIdx, ShadowIndex);

		// shadow
		if (ShadowIndex >= 0)
		{
			switch (ShadowIndex)
			{
			case 0:
				Shader->SetTextureParam(locs.SpotShadowMap_0_TexUnit, this->ShadowMap.Ptr());
				Shader->SetMat4Param(locs.SpotLightSpaceMats_0, this->LightSpaceMat);
				Shader->SetFloatParam(locs.SpotShadowBiasesMin_0, this->ShadowBiasMin);
				Shader->SetFloatParam(locs.SpotShadowBiasesMax_0, this->ShadowBiasMax);
				break;
			case 1:
				Shader->SetTextureParam(locs.SpotShadowMap_1_TexUnit, this->ShadowMap.Ptr());
				Shader->SetMat4Param(locs.SpotLightSpaceMats_1, this->LightSpaceMat);
				Shader->SetFloatParam(locs.SpotShadowBiasesMin_1, this->ShadowBiasMin);
				Shader->SetFloatParam(locs.SpotShadowBiasesMax_1, this->ShadowBiasMax);
				break;
			case 2:
				Shader->SetTextureParam(locs.SpotShadowMap_2_TexUnit, this->ShadowMap.Ptr()); 
				Shader->SetMat4Param(locs.SpotLightSpaceMats_2, this->LightSpaceMat);
				Shader->SetFloatParam(locs.SpotShadowBiasesMin_2, this->ShadowBiasMin);
				Shader->SetFloatParam(locs.SpotShadowBiasesMax_2, this->ShadowBiasMax);
				break;
			case 3:
				Shader->SetTextureParam(locs.SpotShadowMap_3_TexUnit, this->ShadowMap.Ptr()); 
				Shader->SetMat4Param(locs.SpotLightSpaceMats_3, this->LightSpaceMat);
				Shader->SetFloatParam(locs.SpotShadowBiasesMin_3, this->ShadowBiasMin);
				Shader->SetFloatParam(locs.SpotShadowBiasesMax_3, this->ShadowBiasMax);
				break;
			}
		}
	}
}


float HSpotLightNode::GetInnerCone() const
{
	return this->InnerCone;
}
void HSpotLightNode::SetInnerCone(float Value)
{
	this->InnerCone = Value;
}


float HSpotLightNode::GetOuterCone() const
{
	return this->OuterCone;
}
void HSpotLightNode::SetOuterCone(float Value)
{
	this->OuterCone = Value;
}


bool HSpotLightNode::GetEnableShadows() const
{
	return this->EnableShadows;
}
void HSpotLightNode::SetEnableShadows(bool Value)
{
	this->EnableShadows = Value;
	if (Value)
	{
		if (this->ShadowFBO == nullptr)
			this->CreateShadowBuffers();
	}
	else
	{
		this->DeleteShadowBuffers();
	}
}


int32 HSpotLightNode::GetShadowResolution() const
{
	return this->ShadowResolution;
}
void HSpotLightNode::SetShadowResolution(int32 Value)
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


float HSpotLightNode::GetShadowBiasMin() const
{
	return this->ShadowBiasMin;
}
void HSpotLightNode::SetShadowBiasMin(float Value)
{
	this->ShadowBiasMin = Value;
}


float HSpotLightNode::GetShadowBiasMax() const
{
	return this->ShadowBiasMax;
}
void HSpotLightNode::SetShadowBiasMax(float Value)
{
	this->ShadowBiasMax = Value;
}


float HSpotLightNode::GetShadowNearClip() const
{
	return this->ShadowNearClip;
}
void HSpotLightNode::SetShadowNearClip(float Value)
{
	this->ShadowNearClip = Value;
	this->UpdateLightSpaceMatrix();
}


float HSpotLightNode::GetShadowFarClip() const
{
	return this->ShadowFarClip;
}
void HSpotLightNode::SetShadowFarClip(float Value)
{
	this->ShadowFarClip = Value;
	this->UpdateLightSpaceMatrix();
}


const FMat4& HSpotLightNode::GetLightSpaceMat() const
{
	return this->LightSpaceMat;
}


shared<HRenderTarget2DArray> HSpotLightNode::GetShadowMap() const
{
	return this->ShadowMap;
}
HRenderTarget2DArray* HSpotLightNode::GetShadowMapPtr() const
{
	return this->ShadowMap.Ptr();
}


shared<HFrameBufferDepth> HSpotLightNode::GetShadowFBO() const
{
	return this->ShadowFBO;
}
HFrameBufferDepth* HSpotLightNode::GetShadowFBOPtr() const
{
	return this->ShadowFBO.Ptr();
}


const DFrustumPlanes& HSpotLightNode::GetFrustum() const
{
	return this->FrustumPlanes;
}
