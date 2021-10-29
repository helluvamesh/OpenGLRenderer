#pragma once

#include "Engine\Render\RenderUtil\GLMath.h"


struct DCameraRenderData
{
public:
	FMat4 ViewMatrix;
	FMat4 ProjectionMatrix;
	FVec3 WorldLocation;
	DFrustumPlanes FrustumPlanes;
};
