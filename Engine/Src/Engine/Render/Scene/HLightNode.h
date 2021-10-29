#pragma once

#include "HSceneNode.h"


enum class ELightType : uint8 { DIRECTIONAL, POINT, SPOT, AMBIENT_CUBEMAP };


struct DLightNodeSettings
{
	float LightIntensity = 1.0f;
	FVec4 LightColor = FVec4(1.0f);
};


class HLightNode : public HSceneNode
{
	friend class HScene;

public:
	static const int32 MAX_SHADOW_MAP_RESOLUTION = 4096;

	const ELightType LightType;
	
protected:
	float LightIntensity = 1.0f;
	FVec4 LightColor;

private:
	uint64 LightGroups = 0;

public:
	HLightNode(ELightType InLightType);
	
	float GetLightIntensity() const;
	void SetLightIntensity(float Value);
	const FVec4& GetLightColor() const;
	void SetLightColor(const FVec4& NewColor);
};
