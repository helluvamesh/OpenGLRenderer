#pragma once

#include "HSurfaceShader.h"
#include "Engine/Render/Scene/DCameraRenderData.h"
#include "Engine/Render/Scene/DSceneNodeRenderData.h"
#include "HShadowDepthShader.h"


struct DSurfaceMaterialIntParam
{
	int32 Value = 0;
	int32 Location = -1;
	int32 DepthLocation = -1;
};

struct DSurfaceMaterialFloatParam
{
	float Value = .0f;
	int32 Location = 0;
	int32 DepthLocation = -1;
};

struct DSurfaceMaterialVec4Param
{
	FVec4 Value;
	int32 Location = 0;
	int32 DepthLocation = -1;
};

struct DSurfaceMaterialTextureParam
{
	shared<HTextureBase> Value;
	int32 Location = 0;
	int32 TextureUnit = 0;
	int32 DepthLocation = -1;
	int32 DepthTextureUnit = -1;
};


enum class ESurfaceMaterialBlendMode : uint8 { OPAQUE, ALPHA_TEST, ALPHA_BLEND, ADDITIVE };


struct DSurfaceMaterialSettings
{
	shared<HSurfaceShader> Shader;
	ESurfaceMaterialBlendMode BlendMode = ESurfaceMaterialBlendMode::OPAQUE;
	bool DoubleSided = false;
	bool Skybox = false;
};


class HSurfaceMaterial : public HAsset
{
	friend class HScene;

protected:
	DMap<DString, DSurfaceMaterialIntParam> IntParams;
	DMap<DString, DSurfaceMaterialFloatParam> FloatParams;
	DMap<DString, DSurfaceMaterialVec4Param> Vec4Params;
	DMap<DString, DSurfaceMaterialTextureParam> TextureParams;

private:
	shared<HSurfaceShader> Shader;
	ESurfaceMaterialBlendMode BlendMode = ESurfaceMaterialBlendMode::OPAQUE;
	bool DoubleSided = false;
	bool Skybox = false;

private:
	void _Load();

public:
	// Not Implemented!
	void Load() override;
	void Load(const DSurfaceMaterialSettings& Settings);

	bool IsLoaded() override;
	void Unload() override;

	// shader must be 'used'
	void ApplyStateOnShader(HSurfaceShader* SurfaceShader);
	// shader must be 'used'
	void ApplyStateOnShader();
	// shader must be 'used'
	void ApplyStateOnShadowDepthShader(HShadowDepthShader* ShadowDepthShader);
	// shader must be 'used'
	void ApplyNodeStateOnShader(const DCameraRenderData& CameraData, const DSceneNodeRenderData& NodeData, HSurfaceShader* Shader);
	// shader must be 'used'
	void ApplyNodeStateOnShader(const DCameraRenderData& CameraData, const DSceneNodeRenderData& NodeData);
	// Shader->ShadowDepthShader must be 'used'
	void ApplyNodeStateOnShadowDepthShader(const DCameraRenderData& CameraData, const DSceneNodeRenderData& NodeData, HShadowDepthShader* ShadowDepthShader);
	// shader must be 'used'
	void AlphaBlendStep1();
	// shader must be 'used'
	void AlphaBlendStep2();

	void SetIntParam(const DString& InName, int32 Value);
	void SetFloatParam(const DString& InName, float Value);
	void SetVec4Param(const DString& InName, const FVec4& Vec);
	void SetVec4Param(const DString& InName, const FVec3& Vec, float W);
	void SetTextureParam(const DString& InName, shared<HTextureBase> Tex);

	int32 GetIntParam(const DString& InName);
	float GetFloatParam(const DString& InName);
	FVec4 GetVec4Param(const DString& InName);
	shared<HTextureBase> GetTextureParam(const DString& InName);

	shared<HSurfaceShader> GetShader() const;
	HSurfaceShader* GetShaderPtr() const;
	
	ESurfaceMaterialBlendMode GetBlendMode() const;
	void SetBlendMode(ESurfaceMaterialBlendMode Value);

	bool IsDoubleSided() const;
	void SetDoubleSided(bool Value);

	bool IsSkybox() const;
	void SetSkybox(bool Value);

};
