#pragma once

#include "HShaderBase.h"
#include "HPostProcessShader.h"


struct DPostProcessMaterialIntParam
{
	int32 Value = 0;
	int32 Location = -1;
};
struct DPostProcessMaterialFloatParam
{
	float Value = .0f;
	int32 Location = -1;
};
struct DPostProcessMaterialVec4Param
{
	FVec4 Value;
	int32 Location = -1;
};
struct DPostProcessMaterialTextureParam
{
	shared<HTextureBase> Value;
	int32 Location = -1;
	int32 TextureUnit = -1;
};


struct DPostProcessMaterialSettings
{
	shared<HPostProcessShader> Shader;
};


class HPostProcessMaterial : public HAsset
{
protected:
	DMap<DString, DPostProcessMaterialIntParam> IntParams;
	DMap<DString, DPostProcessMaterialFloatParam> FloatParams;
	DMap<DString, DPostProcessMaterialVec4Param> Vec4Params;
	DMap<DString, DPostProcessMaterialTextureParam> TextureParams;

private:
	shared<HPostProcessShader> Shader;

	void _Load();

public:
	// Not Implemented!
	void Load() override;
	void Load(const DPostProcessMaterialSettings& Settings);

	bool IsLoaded() override;
	void Unload() override;

	// shader must be 'used'
	void ApplyStateOnShader();

	void SetIntParam(const DString& InName, int32 Value);
	void SetFloatParam(const DString& InName, float Value);
	void SetVec4Param(const DString& InName, const FVec4& Vec);
	void SetVec4Param(const DString& InName, const FVec3& Vec, float W);
	void SetTextureParam(const DString& InName, shared<HTextureBase> Tex);

	int32 GetIntParam(const DString& InName);
	float GetFloatParam(const DString& InName);
	FVec4 GetVec4Param(const DString& InName);
	shared<HTextureBase> GetTextureParam(const DString& InName);

	shared<HPostProcessShader> GetShader() const;
	HPostProcessShader* GetShaderPtr() const;

};