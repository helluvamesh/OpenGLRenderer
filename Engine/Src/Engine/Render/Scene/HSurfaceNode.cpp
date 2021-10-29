#include "Engine\pch.h"
#include "HSurfaceNode.h"


HSurfaceNode::HSurfaceNode(ESceneNodeType NewType) :
	HSceneNode(NewType)
{
}


uint64 HSurfaceNode::GetLightGroups() const
{
	return this->LightGroups;
}


bool HSurfaceNode::IsInLightGroup(int32 Index) const
{
	return SBitSet::Get(this->LightGroups, Index);
}


uint8 HSurfaceNode::GetLastStencilValue() const
{
	return this->LastStencilValue;
}
