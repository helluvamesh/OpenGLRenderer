#pragma once

#include "HShaderBase.h"


struct DBaseShadowDepthShaderLocs
{
public:
	int32 ModelMat = -1;
	int32 ViewMat = -1;
	int32 ProjectMat = -1;
	int32 ViewPos = -1;
	int32 BlendMode = -1;
	int32 LightSpaceMats = -1;
	int32 LightSpaceMats_0 = -1;
	int32 Layers = -1;
};


struct DShadowDepthShaderSettings
{
	DArray<DString> SourceFilepaths;
};


class HShadowDepthShader : public HShaderBase
{
	friend class HScene;

public:
	enum class EShadowMapType : uint8 { SINGLE, CSM, CUBE };

private:
	static const DSet<DString8> BaseUniformNames;
	DBaseShadowDepthShaderLocs BaseUniformLocs;

public:
	const DBaseShadowDepthShaderLocs& GetBaseUniformLocs();
	const DSet<DString8>& GetBaseUniformNames() override;

protected:
	void OnLoad() override;

public:
	// Not Implemented!
	void Load() override;
	void Load(const DShadowDepthShaderSettings& Settings);

};
