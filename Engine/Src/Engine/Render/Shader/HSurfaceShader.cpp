#include "Engine/pch.h"
#include "HSurfaceShader.h"


const DSet<DString8> HSurfaceShader::BaseUniformNames { 
    "ProjectMat", "ViewMat", "ModelMat", "ViewPos", "BlendMode", 
    "ShadowerdDirLightIdx", "DirLightShadowBiasMin", "DirLightShadowBiasMax",
    "DirLightCount", "PointLightCount", "SpotLightCount", 
    "AmbientColor", "DownOcclusion",
    "IrrMap", "RefMap", 
    "DirLightShadowMap", "DirLightSpaceMats", "CascadeCount", "CascadeDistances",
    "SpotLightSpaceMats", "SpotShadowBiasesMin", "SpotShadowBiasesMax", 
    "SpotShadowMap_0", "SpotShadowMap_1", "SpotShadowMap_2", "SpotShadowMap_3"
};


const DBaseSurfaceShaderLocs& HSurfaceShader::GetBaseUniformLocs()
{
    return this->BaseUniformLocs;
}


const DSet<DString8>& HSurfaceShader::GetBaseUniformNames()
{
    return BaseUniformNames;
}


void HSurfaceShader::OnLoad()
{
    this->BaseUniformLocs.ModelMat = this->GetUniformLocFromGPU("ModelMat");
    this->BaseUniformLocs.ViewMat = this->GetUniformLocFromGPU("ViewMat");
    this->BaseUniformLocs.ProjectMat = this->GetUniformLocFromGPU("ProjectMat");
    this->BaseUniformLocs.ViewPos = this->GetUniformLocFromGPU("ViewPos");
    this->BaseUniformLocs.BlendMode = this->GetUniformLocFromGPU("BlendMode");

    if (this->Unlit == false)
    {
        this->BaseUniformLocs.DirLightCount = this->GetUniformLocFromGPU("DirLightCount");
        this->BaseUniformLocs.PointLightCount = this->GetUniformLocFromGPU("PointLightCount");
        this->BaseUniformLocs.SpotLightCount = this->GetUniformLocFromGPU("SpotLightCount");

        this->BaseUniformLocs.DirLights[0].Direction = this->GetUniformLocFromGPU("DirLights[0].Direction");
        this->BaseUniformLocs.DirLights[1].Direction = this->GetUniformLocFromGPU("DirLights[1].Direction");
        this->BaseUniformLocs.DirLights[2].Direction = this->GetUniformLocFromGPU("DirLights[2].Direction");
        this->BaseUniformLocs.DirLights[3].Direction = this->GetUniformLocFromGPU("DirLights[3].Direction");
        this->BaseUniformLocs.DirLights[0].Color = this->GetUniformLocFromGPU("DirLights[0].Color");
        this->BaseUniformLocs.DirLights[1].Color = this->GetUniformLocFromGPU("DirLights[1].Color");
        this->BaseUniformLocs.DirLights[2].Color = this->GetUniformLocFromGPU("DirLights[2].Color");
        this->BaseUniformLocs.DirLights[3].Color = this->GetUniformLocFromGPU("DirLights[3].Color");
        this->BaseUniformLocs.ShadowedDirLightIdx = this->GetUniformLocFromGPU("ShadowedDirLightIdx");
        this->BaseUniformLocs.DirLightSpaceMats = this->GetUniformLocFromGPU("DirLightSpaceMats");
        this->BaseUniformLocs.DirLightCascadeDistances = this->GetUniformLocFromGPU("DirLightCascadeDistances");
        this->BaseUniformLocs.DirLightCascadeCount = this->GetUniformLocFromGPU("DirLightCascadeCount");
        this->BaseUniformLocs.DirLightShadowBiasesMin = this->GetUniformLocFromGPU("DirLightShadowBiasesMin");
        this->BaseUniformLocs.DirLightShadowBiasesMax = this->GetUniformLocFromGPU("DirLightShadowBiasesMax");

        this->BaseUniformLocs.PointLights[0].Position = this->GetUniformLocFromGPU("PointLights[0].Position");
        this->BaseUniformLocs.PointLights[1].Position = this->GetUniformLocFromGPU("PointLights[1].Position");
        this->BaseUniformLocs.PointLights[2].Position = this->GetUniformLocFromGPU("PointLights[2].Position");
        this->BaseUniformLocs.PointLights[3].Position = this->GetUniformLocFromGPU("PointLights[3].Position");
        this->BaseUniformLocs.PointLights[4].Position = this->GetUniformLocFromGPU("PointLights[4].Position");
        this->BaseUniformLocs.PointLights[5].Position = this->GetUniformLocFromGPU("PointLights[5].Position");
        this->BaseUniformLocs.PointLights[6].Position = this->GetUniformLocFromGPU("PointLights[6].Position");
        this->BaseUniformLocs.PointLights[7].Position = this->GetUniformLocFromGPU("PointLights[7].Position");
        this->BaseUniformLocs.PointLights[8].Position = this->GetUniformLocFromGPU("PointLights[8].Position");
        this->BaseUniformLocs.PointLights[9].Position = this->GetUniformLocFromGPU("PointLights[9].Position");
        this->BaseUniformLocs.PointLights[0].Color = this->GetUniformLocFromGPU("PointLights[0].Color");
        this->BaseUniformLocs.PointLights[1].Color = this->GetUniformLocFromGPU("PointLights[1].Color");
        this->BaseUniformLocs.PointLights[2].Color = this->GetUniformLocFromGPU("PointLights[2].Color");
        this->BaseUniformLocs.PointLights[3].Color = this->GetUniformLocFromGPU("PointLights[3].Color");
        this->BaseUniformLocs.PointLights[4].Color = this->GetUniformLocFromGPU("PointLights[4].Color");
        this->BaseUniformLocs.PointLights[5].Color = this->GetUniformLocFromGPU("PointLights[5].Color");
        this->BaseUniformLocs.PointLights[6].Color = this->GetUniformLocFromGPU("PointLights[6].Color");
        this->BaseUniformLocs.PointLights[7].Color = this->GetUniformLocFromGPU("PointLights[7].Color");
        this->BaseUniformLocs.PointLights[8].Color = this->GetUniformLocFromGPU("PointLights[8].Color");
        this->BaseUniformLocs.PointLights[9].Color = this->GetUniformLocFromGPU("PointLights[9].Color");

        this->BaseUniformLocs.SpotLights[0].Position = this->GetUniformLocFromGPU("SpotLights[0].Position");
        this->BaseUniformLocs.SpotLights[1].Position = this->GetUniformLocFromGPU("SpotLights[1].Position");
        this->BaseUniformLocs.SpotLights[2].Position = this->GetUniformLocFromGPU("SpotLights[2].Position");
        this->BaseUniformLocs.SpotLights[3].Position = this->GetUniformLocFromGPU("SpotLights[3].Position");
        this->BaseUniformLocs.SpotLights[4].Position = this->GetUniformLocFromGPU("SpotLights[4].Position");
        this->BaseUniformLocs.SpotLights[5].Position = this->GetUniformLocFromGPU("SpotLights[5].Position");
        this->BaseUniformLocs.SpotLights[6].Position = this->GetUniformLocFromGPU("SpotLights[6].Position");
        this->BaseUniformLocs.SpotLights[7].Position = this->GetUniformLocFromGPU("SpotLights[7].Position");
        this->BaseUniformLocs.SpotLights[8].Position = this->GetUniformLocFromGPU("SpotLights[8].Position");
        this->BaseUniformLocs.SpotLights[9].Position = this->GetUniformLocFromGPU("SpotLights[9].Position");
        this->BaseUniformLocs.SpotLights[0].Direction = this->GetUniformLocFromGPU("SpotLights[0].Direction");
        this->BaseUniformLocs.SpotLights[1].Direction = this->GetUniformLocFromGPU("SpotLights[1].Direction");
        this->BaseUniformLocs.SpotLights[2].Direction = this->GetUniformLocFromGPU("SpotLights[2].Direction");
        this->BaseUniformLocs.SpotLights[3].Direction = this->GetUniformLocFromGPU("SpotLights[3].Direction");
        this->BaseUniformLocs.SpotLights[4].Direction = this->GetUniformLocFromGPU("SpotLights[4].Direction");
        this->BaseUniformLocs.SpotLights[5].Direction = this->GetUniformLocFromGPU("SpotLights[5].Direction");
        this->BaseUniformLocs.SpotLights[6].Direction = this->GetUniformLocFromGPU("SpotLights[6].Direction");
        this->BaseUniformLocs.SpotLights[7].Direction = this->GetUniformLocFromGPU("SpotLights[7].Direction");
        this->BaseUniformLocs.SpotLights[8].Direction = this->GetUniformLocFromGPU("SpotLights[8].Direction");
        this->BaseUniformLocs.SpotLights[9].Direction = this->GetUniformLocFromGPU("SpotLights[9].Direction");
        this->BaseUniformLocs.SpotLights[0].Color = this->GetUniformLocFromGPU("SpotLights[0].Color");
        this->BaseUniformLocs.SpotLights[1].Color = this->GetUniformLocFromGPU("SpotLights[1].Color");
        this->BaseUniformLocs.SpotLights[2].Color = this->GetUniformLocFromGPU("SpotLights[2].Color");
        this->BaseUniformLocs.SpotLights[3].Color = this->GetUniformLocFromGPU("SpotLights[3].Color");
        this->BaseUniformLocs.SpotLights[4].Color = this->GetUniformLocFromGPU("SpotLights[4].Color");
        this->BaseUniformLocs.SpotLights[5].Color = this->GetUniformLocFromGPU("SpotLights[5].Color");
        this->BaseUniformLocs.SpotLights[6].Color = this->GetUniformLocFromGPU("SpotLights[6].Color");
        this->BaseUniformLocs.SpotLights[7].Color = this->GetUniformLocFromGPU("SpotLights[7].Color");
        this->BaseUniformLocs.SpotLights[8].Color = this->GetUniformLocFromGPU("SpotLights[8].Color");
        this->BaseUniformLocs.SpotLights[9].Color = this->GetUniformLocFromGPU("SpotLights[9].Color");
        this->BaseUniformLocs.SpotLights[0].Cosines = this->GetUniformLocFromGPU("SpotLights[0].Cosines");
        this->BaseUniformLocs.SpotLights[1].Cosines = this->GetUniformLocFromGPU("SpotLights[1].Cosines");
        this->BaseUniformLocs.SpotLights[2].Cosines = this->GetUniformLocFromGPU("SpotLights[2].Cosines");
        this->BaseUniformLocs.SpotLights[3].Cosines = this->GetUniformLocFromGPU("SpotLights[3].Cosines");
        this->BaseUniformLocs.SpotLights[4].Cosines = this->GetUniformLocFromGPU("SpotLights[4].Cosines");
        this->BaseUniformLocs.SpotLights[5].Cosines = this->GetUniformLocFromGPU("SpotLights[5].Cosines");
        this->BaseUniformLocs.SpotLights[6].Cosines = this->GetUniformLocFromGPU("SpotLights[6].Cosines");
        this->BaseUniformLocs.SpotLights[7].Cosines = this->GetUniformLocFromGPU("SpotLights[7].Cosines");
        this->BaseUniformLocs.SpotLights[8].Cosines = this->GetUniformLocFromGPU("SpotLights[8].Cosines");
        this->BaseUniformLocs.SpotLights[9].Cosines = this->GetUniformLocFromGPU("SpotLights[9].Cosines");
        this->BaseUniformLocs.SpotLights[0].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[0].ShadowIdx");
        this->BaseUniformLocs.SpotLights[1].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[1].ShadowIdx");
        this->BaseUniformLocs.SpotLights[2].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[2].ShadowIdx");
        this->BaseUniformLocs.SpotLights[3].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[3].ShadowIdx");
        this->BaseUniformLocs.SpotLights[4].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[4].ShadowIdx");
        this->BaseUniformLocs.SpotLights[5].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[5].ShadowIdx");
        this->BaseUniformLocs.SpotLights[6].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[6].ShadowIdx");
        this->BaseUniformLocs.SpotLights[7].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[7].ShadowIdx");
        this->BaseUniformLocs.SpotLights[8].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[8].ShadowIdx");
        this->BaseUniformLocs.SpotLights[9].ShadowIdx = this->GetUniformLocFromGPU("SpotLights[9].ShadowIdx");
        this->BaseUniformLocs.SpotLightSpaceMats_0 = this->GetUniformLocFromGPU("SpotLightSpaceMats[0]");
        this->BaseUniformLocs.SpotLightSpaceMats_1 = this->GetUniformLocFromGPU("SpotLightSpaceMats[1]");
        this->BaseUniformLocs.SpotLightSpaceMats_2 = this->GetUniformLocFromGPU("SpotLightSpaceMats[2]");
        this->BaseUniformLocs.SpotLightSpaceMats_3 = this->GetUniformLocFromGPU("SpotLightSpaceMats[3]");
        this->BaseUniformLocs.SpotShadowBiasesMin_0 = this->GetUniformLocFromGPU("SpotShadowBiasesMin[0]");
        this->BaseUniformLocs.SpotShadowBiasesMin_1 = this->GetUniformLocFromGPU("SpotShadowBiasesMin[1]");
        this->BaseUniformLocs.SpotShadowBiasesMin_2 = this->GetUniformLocFromGPU("SpotShadowBiasesMin[2]");
        this->BaseUniformLocs.SpotShadowBiasesMin_3 = this->GetUniformLocFromGPU("SpotShadowBiasesMin[3]");
        this->BaseUniformLocs.SpotShadowBiasesMax_0 = this->GetUniformLocFromGPU("SpotShadowBiasesMax[0]");
        this->BaseUniformLocs.SpotShadowBiasesMax_1 = this->GetUniformLocFromGPU("SpotShadowBiasesMax[1]");
        this->BaseUniformLocs.SpotShadowBiasesMax_2 = this->GetUniformLocFromGPU("SpotShadowBiasesMax[2]");
        this->BaseUniformLocs.SpotShadowBiasesMax_3 = this->GetUniformLocFromGPU("SpotShadowBiasesMax[3]");

        this->BaseUniformLocs.AmbientColor = this->GetUniformLocFromGPU("AmbientColor");
        this->BaseUniformLocs.DownOcclusion = this->GetUniformLocFromGPU("DownOcclusion");
        
        // texture samplers
        this->Use();
        
        this->AssignSamplerToTextureUnit(this->GetUniformLocFromGPU("IrrMap"), this->TextureUnitCount);
        this->BaseUniformLocs.IrrMapTexUnit = this->TextureUnitCount;
        this->TextureUnitCount++;

        this->AssignSamplerToTextureUnit(this->GetUniformLocFromGPU("RefMap"), this->TextureUnitCount);
        this->BaseUniformLocs.RefMapTexUnit = this->TextureUnitCount;
        this->TextureUnitCount++;

        this->AssignSamplerToTextureUnit(this->GetUniformLocFromGPU("DirLightShadowMap"), this->TextureUnitCount);
        this->BaseUniformLocs.DirLightShadowMapTexUnit = this->TextureUnitCount;
        this->TextureUnitCount++;

        this->AssignSamplerToTextureUnit(this->GetUniformLocFromGPU("SpotShadowMap_0"), this->TextureUnitCount);
        this->BaseUniformLocs.SpotShadowMap_0_TexUnit = this->TextureUnitCount;
        this->TextureUnitCount++;

        this->AssignSamplerToTextureUnit(this->GetUniformLocFromGPU("SpotShadowMap_1"), this->TextureUnitCount);
        this->BaseUniformLocs.SpotShadowMap_1_TexUnit = this->TextureUnitCount;
        this->TextureUnitCount++;

        this->AssignSamplerToTextureUnit(this->GetUniformLocFromGPU("SpotShadowMap_2"), this->TextureUnitCount);
        this->BaseUniformLocs.SpotShadowMap_2_TexUnit = this->TextureUnitCount;
        this->TextureUnitCount++;

        this->AssignSamplerToTextureUnit(this->GetUniformLocFromGPU("SpotShadowMap_3"), this->TextureUnitCount);
        this->BaseUniformLocs.SpotShadowMap_3_TexUnit = this->TextureUnitCount;
        this->TextureUnitCount++;
    }
}


void HSurfaceShader::Load()
{
    this->Unload();

    // load from .asset file here
}


void HSurfaceShader::Load(const DSurfaceShaderSettings& Settings)
{
    this->Unload();

    this->Unlit = Settings.Unlit;
    this->_Load(Settings.SourceFilepaths);
    this->ShadowDepthShader = Settings.ShadowDepthShader;
}


bool HSurfaceShader::IsUnlit() const
{
    return this->Unlit;
}
void HSurfaceShader::SetUnlit(bool Value)
{
    this->Unlit = Value;
}


shared<HShadowDepthShader> HSurfaceShader::GetShadowDepthShader() const
{
    return this->ShadowDepthShader;
}
HShadowDepthShader* HSurfaceShader::GetShadowDepthShaderPtr() const
{
    return this->ShadowDepthShader.Ptr();
}
void HSurfaceShader::SetShadowDepthShader(shared<HShadowDepthShader> NewShader)
{
    this->ShadowDepthShader = NewShader;
}
