#pragma once

#include "Engine/Render/Shader/HSurfaceMaterial.h"


class ISurface : public IInterface
{
	
public:
	// a shader must be 'used'
	virtual void Draw(HSurfaceMaterial* Material) = 0;
	// a shader must be 'used'
	virtual void DrawStandalone() = 0;

};
