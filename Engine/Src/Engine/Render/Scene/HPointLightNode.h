#pragma once

#include "HLightNode.h"
#include "Engine\Render\Shader\HSurfaceShader.h"


struct DPointLightSettings : public DLightNodeSettings
{
	float LightDistance = 10.0f;
};


class HPointLightNode : public HLightNode
{
	friend class HScene;

private:
	float LightDistance;

public:
	HPointLightNode(const DPointLightSettings& Settings);

	void ApplyOnShader(HSurfaceShader* Shader, int32 LightIndex);

	float GetLightDistance() const;
	void SetLightDistance(float Value);
};
