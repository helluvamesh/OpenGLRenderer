#include "Engine/pch.h"
#include "HSurfaceMaterial.h"
#include "HSurfaceShader.h"
#include "GL/glew.h"
#include "Engine\Render\RenderUtil\GLUtil.h"


void HSurfaceMaterial::_Load()
{
    if (this->Shader)
    {
        // populate params
        const DMap<DString, DShaderUniformInfo>& uniforms = this->Shader->GetUniforms();
        for (auto& entry : uniforms)
        {
            const DString& uniformName = entry.GetKey();
            const DShaderUniformInfo& uniform = entry.GetValue();
            if (uniform.Location >= 0)
            {
                switch (uniform.Type)
                {
                case EShaderParamType::FLOAT:
                {
                    DSurfaceMaterialFloatParam& param = this->FloatParams.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.Value = uniform.DefaultValue.X;
                    break;
                }
                case EShaderParamType::VEC4:
                {
                    DSurfaceMaterialVec4Param& param = this->Vec4Params.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.Value = uniform.DefaultValue;
                    break;
                }
                case EShaderParamType::TEXTURE:
                {
                    DSurfaceMaterialTextureParam& param = this->TextureParams.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.TextureUnit = uniform.TextureUnit;
                    break;
                }
                case EShaderParamType::INT:
                {
                    DSurfaceMaterialIntParam& param = this->IntParams.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.Value = (int32)uniform.DefaultValue.X;
                    break;
                }
                }
            }
        }

        HShadowDepthShader* shadowDepthShader = this->Shader->GetShadowDepthShaderPtr();
        if (shadowDepthShader)
        {
            // link uniforms of the shadow depth shader to those of the main shader by matching name
            const DMap<DString, DShaderUniformInfo>& uniforms = shadowDepthShader->GetUniforms();
            for (auto& entry : uniforms)
            {
                const DString& uniformName = entry.GetKey();
                const DShaderUniformInfo& uniform = entry.GetValue();
                if (uniform.Location >= 0)
                {
                    switch (uniform.Type)
                    {
                    case EShaderParamType::FLOAT:
                    {
                        DSurfaceMaterialFloatParam* param = this->FloatParams.Find(uniformName);
                        if (param)
                        {
                            param->DepthLocation = uniform.Location;
                        }
                        break;
                    }
                    case EShaderParamType::VEC4:
                    {
                        DSurfaceMaterialVec4Param* param = this->Vec4Params.Find(uniformName);
                        if (param)
                        {
                            param->DepthLocation = uniform.Location;
                        }
                        break;
                    }
                    case EShaderParamType::TEXTURE:
                    {
                        DSurfaceMaterialTextureParam* param = this->TextureParams.Find(uniformName);
                        if (param)
                        {
                            param->DepthLocation = uniform.Location;
                            param->DepthTextureUnit = uniform.TextureUnit;
                        }
                        break;
                    }
                    case EShaderParamType::INT:
                    {
                        DSurfaceMaterialIntParam* param = this->IntParams.Find(uniformName);
                        if (param)
                        {
                            param->DepthLocation = uniform.Location;
                        }
                        break;
                    }
                    }
                }
            }
        }
    }
    else
    {
        SLog::Error(TX("Shader is null in material \"") + this->Name + TX("\""));
    }
}


void HSurfaceMaterial::Load()
{
    this->Unload();

    // load from .asset file here
}


void HSurfaceMaterial::Load(const DSurfaceMaterialSettings& Settings)
{
    this->Unload();

    this->BlendMode = Settings.BlendMode;
    this->DoubleSided = Settings.DoubleSided;
    this->Skybox = Settings.Skybox;
    this->Shader = Settings.Shader;
    this->_Load();
}


bool HSurfaceMaterial::IsLoaded()
{
    return this->Shader != nullptr;
}


void HSurfaceMaterial::Unload()
{
    this->IntParams.Clear();
    this->FloatParams.Clear();
    this->Vec4Params.Clear();
    this->TextureParams.Clear();
}


void HSurfaceMaterial::ApplyStateOnShader(HSurfaceShader* SurfaceShader)
{
    // apply uniforms on the shader
    for (auto& entry : this->IntParams)
    {
        const DSurfaceMaterialIntParam& param = entry.GetValue();
        SurfaceShader->SetIntParam(param.Location, param.Value);
    }
    for (auto& entry : this->FloatParams)
    {
        const DSurfaceMaterialFloatParam& param = entry.GetValue();
        SurfaceShader->SetFloatParam(param.Location, param.Value);
    }
    for (auto& entry : this->Vec4Params)
    {
        const DSurfaceMaterialVec4Param& param = entry.GetValue();
        SurfaceShader->SetVec4Param(param.Location, param.Value);
    }
    for (auto& entry : this->TextureParams)
    {
        const DSurfaceMaterialTextureParam& param = entry.GetValue();
        SurfaceShader->SetTextureParam(param.TextureUnit, param.Value.Ptr());
    }
        
    if (this->DoubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);

    switch (this->BlendMode)
    {
    case ESurfaceMaterialBlendMode::OPAQUE:
    {
        glDisable(GL_BLEND);
        SurfaceShader->SetIntParam(SurfaceShader->GetBaseUniformLocs().BlendMode, 0);
        break;
    }
    case ESurfaceMaterialBlendMode::ALPHA_TEST:
    {
        glDisable(GL_BLEND);
        SurfaceShader->SetIntParam(SurfaceShader->GetBaseUniformLocs().BlendMode, 1);
        break;
    }
    case ESurfaceMaterialBlendMode::ALPHA_BLEND:
    {
        // call AlphaBlendStep1() from outside
        break;
    }
    case ESurfaceMaterialBlendMode::ADDITIVE:
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        SurfaceShader->SetIntParam(SurfaceShader->GetBaseUniformLocs().BlendMode, 4);
        break;
    }
    }
}


void HSurfaceMaterial::ApplyStateOnShader()
{
    if (this->Shader)
    {
        this->ApplyStateOnShader(this->Shader.Ptr());
    }
    else
    {
        SLog::Error(TX("Material has no shader: \"") + this->Name + TX("\""));
    }
}


void HSurfaceMaterial::ApplyStateOnShadowDepthShader(HShadowDepthShader* ShadowDepthShader)
{
    ShadowDepthShader->SetIntParam(ShadowDepthShader->GetBaseUniformLocs().BlendMode, (int32)this->BlendMode);

    // apply uniforms on the shader
    for (auto& entry : this->IntParams)
    {
        const DSurfaceMaterialIntParam& param = entry.GetValue();
        if (param.DepthLocation >= 0)
            ShadowDepthShader->SetIntParam(param.DepthLocation, param.Value);
    }
    for (auto& entry : this->FloatParams)
    {
        const DSurfaceMaterialFloatParam& param = entry.GetValue();
        if (param.DepthLocation >= 0)
            ShadowDepthShader->SetFloatParam(param.DepthLocation, param.Value);
    }
    for (auto& entry : this->Vec4Params)
    {
        const DSurfaceMaterialVec4Param& param = entry.GetValue();
        if (param.DepthLocation >= 0)
            ShadowDepthShader->SetVec4Param(param.DepthLocation, param.Value);
    }
    for (auto& entry : this->TextureParams)
    {
        const DSurfaceMaterialTextureParam& param = entry.GetValue();
        if (param.DepthTextureUnit >= 0)
            ShadowDepthShader->SetTextureParam(param.DepthTextureUnit, param.Value.Ptr());
    }

    if (this->DoubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
}


void HSurfaceMaterial::ApplyNodeStateOnShader(
    const DCameraRenderData& CameraData, const DSceneNodeRenderData& NodeData, HSurfaceShader* SurfaceShader
)
{
    const DBaseSurfaceShaderLocs& params = this->Shader->GetBaseUniformLocs();
    SurfaceShader->SetMat4Param(params.ProjectMat, CameraData.ProjectionMatrix);
    SurfaceShader->SetMat4Param(params.ModelMat, NodeData.WorldMatrix);
    SurfaceShader->SetVec3Param(params.ViewPos, CameraData.WorldLocation);
    if (this->Skybox)
    {
        auto& v = CameraData.ViewMatrix.Data;
        FMat4 skyboxViewMat (
            v[0][0], v[0][1], v[0][2], .0f,
            v[1][0], v[1][1], v[1][2], .0f,
            v[2][0], v[2][1], v[2][2], .0f,
            v[3][0], v[3][1], v[3][2], v[3][3]
        );
        this->Shader->SetMat4Param(params.ViewMat, skyboxViewMat);
    }
    else
    {
        this->Shader->SetMat4Param(params.ViewMat, CameraData.ViewMatrix);
    }
}


void HSurfaceMaterial::ApplyNodeStateOnShader(const DCameraRenderData& CameraData, const DSceneNodeRenderData& NodeData)
{
    if (this->Shader)
    {
        this->ApplyNodeStateOnShader(CameraData, NodeData, this->Shader.Ptr());
    }
    else
    {
        SLog::Error(TX("Shader is null in material ") + this->Name);
    }
}


void HSurfaceMaterial::ApplyNodeStateOnShadowDepthShader(
    const DCameraRenderData& CameraData, const DSceneNodeRenderData& NodeData, HShadowDepthShader* ShadowDepthShader
)
{
    const DBaseShadowDepthShaderLocs& params = ShadowDepthShader->GetBaseUniformLocs();
    ShadowDepthShader->SetMat4Param(params.ProjectMat, CameraData.ProjectionMatrix);
    ShadowDepthShader->SetMat4Param(params.ModelMat, NodeData.WorldMatrix);
    ShadowDepthShader->SetVec3Param(params.ViewPos, CameraData.WorldLocation);
    ShadowDepthShader->SetMat4Param(params.ViewMat, CameraData.ViewMatrix);
}


void HSurfaceMaterial::AlphaBlendStep1()
{
    if (this->Shader)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        this->Shader->SetIntParam(this->Shader->GetBaseUniformLocs().BlendMode, 2);
    }
    else
    {
        SLog::Error(TX("Material has no shader in ") + this->Name);
    }
}


void HSurfaceMaterial::AlphaBlendStep2()
{
    if (this->Shader)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        this->Shader->SetIntParam(this->Shader->GetBaseUniformLocs().BlendMode, 3);
    }
    else
    {
        SLog::Error(TX("Material has no shader in ") + this->Name);
    }
}


void HSurfaceMaterial::SetIntParam(const DString& InName, int32 Value)
{
    DSurfaceMaterialIntParam* param = this->IntParams.Find(InName);
    if (param)
    {
        param->Value = Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HSurfaceMaterial::SetFloatParam(const DString& InName, float Value)
{
    DSurfaceMaterialFloatParam* param = this->FloatParams.Find(InName);
    if (param)
    {
        param->Value = Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HSurfaceMaterial::SetVec4Param(const DString& InName, const FVec4& Vec)
{
    DSurfaceMaterialVec4Param* param = this->Vec4Params.Find(InName);
    if (param)
    {
        param->Value = Vec;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HSurfaceMaterial::SetVec4Param(const DString& InName, const FVec3& Vec, float W)
{
    DSurfaceMaterialVec4Param* param = this->Vec4Params.Find(InName);
    if (param)
    {
        param->Value = FVec4(Vec.X, Vec.Y, Vec.Z, W);
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HSurfaceMaterial::SetTextureParam(const DString& InName, shared<HTextureBase> Tex)
{
    DSurfaceMaterialTextureParam* param = this->TextureParams.Find(InName);
    if (param)
    {
        param->Value = Tex;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


int32 HSurfaceMaterial::GetIntParam(const DString& InName)
{
    DSurfaceMaterialIntParam* param = this->IntParams.Find(InName);
    if (param)
    {
        return param->Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
        return false;
    }
}


float HSurfaceMaterial::GetFloatParam(const DString& InName)
{
    DSurfaceMaterialFloatParam* param = this->FloatParams.Find(InName);
    if (param)
    {
        return param->Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
        return .0f;
    }
}


FVec4 HSurfaceMaterial::GetVec4Param(const DString& InName)
{
    DSurfaceMaterialVec4Param* param = this->Vec4Params.Find(InName);
    if (param)
    {
        return param->Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
        return FVec4();
    }
}


shared<HTextureBase> HSurfaceMaterial::GetTextureParam(const DString& InName)
{
    DSurfaceMaterialTextureParam* param = this->TextureParams.Find(InName);
    if (param)
    {
        return param->Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
        return nullptr;
    }
}


shared<HSurfaceShader> HSurfaceMaterial::GetShader() const
{
    return this->Shader;
}
HSurfaceShader* HSurfaceMaterial::GetShaderPtr() const
{
    return this->Shader.Ptr();
}


ESurfaceMaterialBlendMode HSurfaceMaterial::GetBlendMode() const
{
    return this->BlendMode;
}
void HSurfaceMaterial::SetBlendMode(ESurfaceMaterialBlendMode Value)
{
    this->BlendMode = Value;
}


bool HSurfaceMaterial::IsDoubleSided() const
{
    return this->DoubleSided;
}
void HSurfaceMaterial::SetDoubleSided(bool Value)
{
    this->DoubleSided = Value;
}
    

bool HSurfaceMaterial::IsSkybox() const
{
    return this->Skybox;
}
void HSurfaceMaterial::SetSkybox(bool Value)
{
    this->Skybox = Value;
}
