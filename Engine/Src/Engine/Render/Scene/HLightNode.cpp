#include "Engine/pch.h"
#include "HLightNode.h"


HLightNode::HLightNode(ELightType InLightType) :
	HSceneNode(ESceneNodeType::LIGHT), 
	LightType(InLightType)
{
}

float HLightNode::GetLightIntensity() const
{
	return this->LightIntensity;
}

void HLightNode::SetLightIntensity(float Value)
{
	this->LightIntensity = Value;
}

const FVec4& HLightNode::GetLightColor() const
{
	return this->LightColor;
}

void HLightNode::SetLightColor(const FVec4& NewColor)
{
	this->LightColor = NewColor;
}
