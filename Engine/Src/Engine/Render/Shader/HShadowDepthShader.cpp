#include "Engine\pch.h"
#include "HShadowDepthShader.h"


const DSet<DString8> HShadowDepthShader::BaseUniformNames { 
    "ProjectMat", "ViewMat", "ModelMat", "ViewPos", "BlendMode", "LightSpaceMats", "Layers"
};


const DBaseShadowDepthShaderLocs& HShadowDepthShader::GetBaseUniformLocs()
{
    return this->BaseUniformLocs;
}


const DSet<DString8>& HShadowDepthShader::GetBaseUniformNames()
{
    return BaseUniformNames;
}


void HShadowDepthShader::OnLoad()
{
    this->BaseUniformLocs.ModelMat = this->GetUniformLocFromGPU("ModelMat");
    this->BaseUniformLocs.ViewMat = this->GetUniformLocFromGPU("ViewMat");
    this->BaseUniformLocs.ProjectMat = this->GetUniformLocFromGPU("ProjectMat");
    this->BaseUniformLocs.ViewPos = this->GetUniformLocFromGPU("ViewPos");
    this->BaseUniformLocs.BlendMode = this->GetUniformLocFromGPU("BlendMode");
    this->BaseUniformLocs.LightSpaceMats = this->GetUniformLocFromGPU("LightSpaceMats");
    this->BaseUniformLocs.LightSpaceMats_0 = this->GetUniformLocFromGPU("LightSpaceMats[0]");
    this->BaseUniformLocs.Layers = this->GetUniformLocFromGPU("Layers");
}


void HShadowDepthShader::Load()
{
    this->Unload();

    // load from .asset file here
}


void HShadowDepthShader::Load(const DShadowDepthShaderSettings& Settings)
{
    this->Unload();

    this->_Load(Settings.SourceFilepaths);
}
