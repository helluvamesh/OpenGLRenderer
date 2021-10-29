#include "Engine\pch.h"
#include "HAmbientLightNode.h"


HAmbientLightNode::HAmbientLightNode(const DAmbientLightSettings& Settings) : 
	HLightNode(ELightType::AMBIENT_CUBEMAP)
{
	this->LightIntensity = Settings.LightIntensity;
	this->LightColor = Settings.LightColor;

	this->IrradianceMap = Settings.IrradianceMap;
	this->ReflectionMap = Settings.ReflectionMap;
	this->DownOcclusion = Settings.DownOcclusion;
}


void HAmbientLightNode::ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex)
{
	if (Shader)
	{
		FVec3 color (
			this->LightColor.X * this->LightIntensity, 
			this->LightColor.Y * LightIntensity, 
			this->LightColor.Z * LightIntensity
		);

		const DBaseSurfaceShaderLocs& locs = Shader->GetBaseUniformLocs();
		Shader->SetVec3Param(locs.AmbientColor, color);
		Shader->SetTextureParam(locs.IrrMapTexUnit, this->IrradianceMap? this->IrradianceMap.Ptr() : nullptr);
		Shader->SetTextureParam(locs.RefMapTexUnit, this->ReflectionMap? this->ReflectionMap.Ptr() : nullptr);
		Shader->SetFloatParam(locs.DownOcclusion, this->DownOcclusion);
	}
}


shared<HTextureCubeBase> HAmbientLightNode::GetIrradianceMap() const
{
	return this->IrradianceMap;
}
HTextureCubeBase* HAmbientLightNode::GetIrradianceMapPtr() const
{
	return this->IrradianceMap.Ptr();
}
void HAmbientLightNode::SetIrradianceMap(shared<HTextureCubeBase> Cubemap)
{
	this->IrradianceMap = Cubemap;
}


shared<HTextureCubeBase> HAmbientLightNode::GetReflectionMap() const
{
	return this->ReflectionMap;
}
HTextureCubeBase* HAmbientLightNode::GetReflectionMapPtr() const
{
	return this->ReflectionMap.Ptr();
}
void HAmbientLightNode::SetReflectionMap(shared<HTextureCubeBase> Cubemap)
{
	this->ReflectionMap = Cubemap;
}


float HAmbientLightNode::GetDownOcclusion() const
{
	return this->DownOcclusion;
}
void HAmbientLightNode::SetDownOcclusion(float Value)
{
	this->DownOcclusion = Value;
}
