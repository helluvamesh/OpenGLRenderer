#include "Engine/pch.h"
#include "HPostProcessShader.h"


void HPostProcessShader::Load()
{
    this->Unload();

    // load from .asset file here
}


void HPostProcessShader::Load(const DPostProcessShaderSettings& Settings)
{
    this->Unload();

    this->_Load(Settings.SourceFilepaths);
}
