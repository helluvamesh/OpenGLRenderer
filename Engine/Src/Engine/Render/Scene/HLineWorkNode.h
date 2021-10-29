#pragma once

#include "HSceneNode.h"
#include "Engine\Render\Mesh\HLineWork.h"
#include "HLineWorkNode.h"
#include "HSurfaceNode.h"


class HLineWorkNode : public HSurfaceNode
{
	friend class HScene;

private:
	shared<HLineWork> LineWork;
	FBox WorldBoundingBox;
	FSphere WorldBoundingSphere;

public:
	HLineWorkNode();

public:
	void UpdateBounds();
	
protected:
	void OnTransform() override;

public:
	shared<HLineWork> GetLineWork() const;
	HLineWork* GetLineWorkPtr() const;
	void SetLineWork(shared<HLineWork> NewLineWork);	

	const FBox& GetBoundingBox() const override;
	const FSphere& GetBoundingSphere() const override;
	const FAxisBox& GetLocalBoundingBox() const override;

};
