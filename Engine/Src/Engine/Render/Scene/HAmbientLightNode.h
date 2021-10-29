#pragma once

#include "HLightNode.h"
#include "Engine\Render\Texture\HTextureBase.h"
#include "Engine\Render\Shader\HSurfaceShader.h"


struct DAmbientLightSettings : public DLightNodeSettings
{
	shared<HTextureCubeBase> IrradianceMap;
	shared<HTextureCubeBase> ReflectionMap;
	float DownOcclusion = 1.0f;
};


class HAmbientLightNode : public HLightNode
{
	friend class HScene;

private:
	shared<HTextureCubeBase> IrradianceMap;
	shared<HTextureCubeBase> ReflectionMap;
	// Occlude ambient light on geometry facing downward
	float DownOcclusion;

public:
	HAmbientLightNode(const DAmbientLightSettings& Settings);

	void ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex);

	shared<HTextureCubeBase> GetIrradianceMap() const;
	HTextureCubeBase* GetIrradianceMapPtr() const;
	void SetIrradianceMap(shared<HTextureCubeBase> Cubemap);

	shared<HTextureCubeBase> GetReflectionMap() const;
	HTextureCubeBase* GetReflectionMapPtr() const;
	void SetReflectionMap(shared<HTextureCubeBase> Cubemap);

	float GetDownOcclusion() const;
	void SetDownOcclusion(float Value);
};
