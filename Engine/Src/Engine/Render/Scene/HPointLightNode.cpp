#include "Engine\pch.h"
#include "HPointLightNode.h"


HPointLightNode::HPointLightNode(const DPointLightSettings& Settings) :
	HLightNode(ELightType::POINT)
{
	this->LightIntensity = Settings.LightIntensity;
	this->LightColor = Settings.LightColor;

	this->LightDistance = Settings.LightDistance;
}


void HPointLightNode::ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex)
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

		const DBaseSurfaceShaderLocs& locs = Shader->GetBaseUniformLocs();
		Shader->SetVec3Param(locs.PointLights[LightIndex].Color, color);
		Shader->SetVec3Param(locs.PointLights[LightIndex].Position, position);
	}
}


float HPointLightNode::GetLightDistance() const
{
	return this->LightDistance;
}
void HPointLightNode::SetLightDistance(float Value)
{
	this->LightDistance = Value;
}
