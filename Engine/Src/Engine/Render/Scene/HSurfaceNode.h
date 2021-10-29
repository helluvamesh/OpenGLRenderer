#pragma once

#include "HSceneNode.h"


// abstract
class HSurfaceNode : public HSceneNode
{
	friend class HScene;

protected:
	uint64 LightGroups = 0;
	uint8 LastStencilValue = 0;

public:
	HSurfaceNode(ESceneNodeType NewType);

	uint64 GetLightGroups() const;
	bool IsInLightGroup(int32 Index) const;
	uint8 GetLastStencilValue() const;

	virtual const FBox& GetBoundingBox() const = 0;
	virtual const FSphere& GetBoundingSphere() const = 0;
	virtual const FAxisBox& GetLocalBoundingBox() const = 0;

};
