#include "Engine/pch.h"
#include "HPostProcessMaterial.h"


void HPostProcessMaterial::_Load()
{
    if (this->Shader)
    {
        // populate params
        const DMap<DString, DShaderUniformInfo>& uniforms = this->Shader->GetUniforms();
        for (auto& entry : uniforms)
        {
            const DString uniformName = entry.GetKey();
            const DShaderUniformInfo& uniform = entry.GetValue();
            if (uniform.Location >= 0)
            {
                switch (uniform.Type)
                {
                case EShaderParamType::FLOAT:
                {
                    DPostProcessMaterialFloatParam& param = this->FloatParams.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.Value = uniform.DefaultValue.X;
                    break;
                }
                case EShaderParamType::VEC4:
                {
                    DPostProcessMaterialVec4Param& param = this->Vec4Params.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.Value = uniform.DefaultValue;
                    break;
                }
                case EShaderParamType::TEXTURE:
                {
                    DPostProcessMaterialTextureParam& param = this->TextureParams.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.TextureUnit = uniform.TextureUnit;
                    break;
                }
                case EShaderParamType::INT:
                {
                    DPostProcessMaterialIntParam& param = this->IntParams.AddDefault(uniformName);
                    param.Location = uniform.Location;
                    param.Value = (int32)uniform.DefaultValue.X;
                    break;
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


void HPostProcessMaterial::Load()
{
    this->Unload();

    // load from .asset file here
}


void HPostProcessMaterial::Load(const DPostProcessMaterialSettings& Settings)
{
    this->Unload();

    this->Shader = Settings.Shader;
    this->_Load();
}


bool HPostProcessMaterial::IsLoaded()
{
    return this->Shader != nullptr;
}


void HPostProcessMaterial::Unload()
{
    this->IntParams.Clear();
    this->FloatParams.Clear();
    this->Vec4Params.Clear();
    this->TextureParams.Clear();
}


void HPostProcessMaterial::ApplyStateOnShader()
{
    if (this->Shader)
    {
        HPostProcessShader* shader = this->Shader.Ptr();
        // apply uniforms on the shader
        for (auto& entry : this->IntParams)
        {
            const DPostProcessMaterialIntParam& param = entry.GetValue();
            shader->SetIntParam(param.Location, param.Value);
        }
        for (auto& entry : this->FloatParams)
        {
            const DPostProcessMaterialFloatParam& param = entry.GetValue();
            shader->SetFloatParam(param.Location, param.Value);
        }
        for (auto& entry : this->Vec4Params)
        {
            const DPostProcessMaterialVec4Param& param = entry.GetValue();
            shader->SetVec4Param(param.Location, param.Value);
        }
        for (auto& entry : this->TextureParams)
        {
            const DPostProcessMaterialTextureParam& param = entry.GetValue();
            shader->SetTextureParam(param.TextureUnit, param.Value.Ptr());
        }
    }
    else
    {
        SLog::Error(TX("Material has no shader in ") + this->Name);
    }
}


void HPostProcessMaterial::SetIntParam(const DString& InName, int32 Value)
{
    DPostProcessMaterialIntParam* param = this->IntParams.Find(InName);
    if (param)
    {
        param->Value = Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HPostProcessMaterial::SetFloatParam(const DString& InName, float Value)
{
    DPostProcessMaterialFloatParam* param = this->FloatParams.Find(InName);
    if (param)
    {
        param->Value = Value;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HPostProcessMaterial::SetVec4Param(const DString& InName, const FVec4& Vec)
{
    DPostProcessMaterialVec4Param* param = this->Vec4Params.Find(InName);
    if (param)
    {
        param->Value = Vec;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HPostProcessMaterial::SetVec4Param(const DString& InName, const FVec3& Vec, float W)
{
    DPostProcessMaterialVec4Param* param = this->Vec4Params.Find(InName);
    if (param)
    {
        param->Value = FVec4(Vec.X, Vec.Y, Vec.Z, W);
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


void HPostProcessMaterial::SetTextureParam(const DString& InName, shared<HTextureBase> Tex)
{
    DPostProcessMaterialTextureParam* param = this->TextureParams.Find(InName);
    if (param)
    {
        param->Value = Tex;
    }
    else
    {
        SLog::Error(TX("Material param \"") + InName + TX("\" not found in ") + this->Name);
    }
}


int32 HPostProcessMaterial::GetIntParam(const DString& InName)
{
    DPostProcessMaterialIntParam* param = this->IntParams.Find(InName);
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


float HPostProcessMaterial::GetFloatParam(const DString& InName)
{
    DPostProcessMaterialFloatParam* param = this->FloatParams.Find(InName);
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


FVec4 HPostProcessMaterial::GetVec4Param(const DString& InName)
{
    DPostProcessMaterialVec4Param* param = this->Vec4Params.Find(InName);
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


shared<HTextureBase> HPostProcessMaterial::GetTextureParam(const DString& InName)
{
    DPostProcessMaterialTextureParam* param = this->TextureParams.Find(InName);
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


shared<HPostProcessShader> HPostProcessMaterial::GetShader() const
{
    return this->Shader;
}
HPostProcessShader* HPostProcessMaterial::GetShaderPtr() const
{
    return this->Shader.Ptr();
}
