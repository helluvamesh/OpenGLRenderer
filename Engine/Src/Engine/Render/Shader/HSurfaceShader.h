#pragma once

#include "HShaderBase.h"
#include "HShadowDepthShader.h"


struct DDirLightLocations
{
	int32 Direction = -1;
	int32 Color = -1;
};


struct DPointLightLocations
{
	int32 Position = -1;
	int32 Color = -1;
};


struct DSpotLightLocations
{
	int32 Position = -1;
	int32 Direction = -1;
	int32 Color = -1;
	int32 Cosines = -1;
	int32 ShadowIdx = -1;
};


struct DBaseSurfaceShaderLocs
{
public:
	int32 ModelMat = -1;
	int32 ViewMat = -1;
	int32 ProjectMat = -1;
	int32 ViewPos = -1;
	int32 BlendMode = -1;

	int32 DirLightCount = -1;
	int32 PointLightCount = -1;
	int32 SpotLightCount = -1;
	
	DFixedArray<DDirLightLocations, 4> DirLights;

	int32 ShadowedDirLightIdx = -1;
	int32 DirLightSpaceMats = -1;
	int32 DirLightCascadeCount = -1;
	int32 DirLightCascadeDistances = -1;
	
	int32 DirLightShadowMapTexUnit = -1;
	int32 DirLightShadowBiasesMin = -1;
	int32 DirLightShadowBiasesMax = -1;

	DFixedArray<DPointLightLocations, 10> PointLights;

	int32 SpotShadowMap_0_TexUnit = -1;
	int32 SpotShadowMap_1_TexUnit = -1;
	int32 SpotShadowMap_2_TexUnit = -1;
	int32 SpotShadowMap_3_TexUnit = -1;

	int32 SpotLightSpaceMats_0 = -1;
	int32 SpotLightSpaceMats_1 = -1;
	int32 SpotLightSpaceMats_2 = -1;
	int32 SpotLightSpaceMats_3 = -1;
	int32 SpotShadowBiasesMin_0 = -1;
	int32 SpotShadowBiasesMin_1 = -1;
	int32 SpotShadowBiasesMin_2 = -1;
	int32 SpotShadowBiasesMin_3 = -1;
	int32 SpotShadowBiasesMax_0 = -1;
	int32 SpotShadowBiasesMax_1 = -1;
	int32 SpotShadowBiasesMax_2 = -1;
	int32 SpotShadowBiasesMax_3 = -1;

	DFixedArray<DSpotLightLocations, 10> SpotLights;

	int32 IrrMapTexUnit = -1;
	int32 RefMapTexUnit = -1;
	int32 AmbientColor = -1;
	int32 DownOcclusion = -1;
};


struct DSurfaceShaderSettings
{
	DArray<DString> SourceFilepaths;
	bool Unlit = false;
	shared<HShadowDepthShader> ShadowDepthShader;
};


class HSurfaceShader : public HShaderBase
{
	friend class HScene;

private:
	static const DSet<DString8> BaseUniformNames;
	DBaseSurfaceShaderLocs BaseUniformLocs;
	bool Unlit = false;
	shared<HShadowDepthShader> ShadowDepthShader;
	
public:
	const DBaseSurfaceShaderLocs& GetBaseUniformLocs();
	const DSet<DString8>& GetBaseUniformNames() override;
	
protected:
	void OnLoad() override;

public:
	// Not Implemented!
	void Load() override;
	void Load(const DSurfaceShaderSettings& Settings);

	bool IsUnlit() const;
	void SetUnlit(bool Value);

	shared<HShadowDepthShader> GetShadowDepthShader() const;
	HShadowDepthShader* GetShadowDepthShaderPtr() const;
	void SetShadowDepthShader(shared<HShadowDepthShader> NewShader);

};
