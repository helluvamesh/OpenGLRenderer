#pragma once

#include "HShaderBase.h"


struct DPostProcessShaderSettings
{
	DArray<DString> SourceFilepaths;
};


class HPostProcessShader : public HShaderBase
{
public:
	// Not Implemented!
	void Load() override;
	void Load(const DPostProcessShaderSettings& Settings);

};
