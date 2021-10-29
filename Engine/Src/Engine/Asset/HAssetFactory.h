#pragma once

#include "Engine\Render\Texture\HTexture2D.h"
#include "Engine\Render\Shader\HEqMapConvert.h"
#include "Engine\Render\Shader\HIBLConvert.h"
#include "Wavefront\DWavefrontParser.h"
#include "Wavefront\DWavefrontEncoder.h"


struct DTextureImport : public DStruct
{
	REFLECT_H(DTextureImport, DStruct)
public:
	DString Source;
	ETexture2DFormat Format = ETexture2DFormat::BC1;
	bool sRGB = true;
	bool InvertGreen = false;
};


struct DStaticMeshImport : public DStruct
{
	REFLECT_H(DStaticMeshImport, DStruct)
public:
	DString Source;
};


class HAssetFactory
{
private:
	DJsonParser JSONParser;
	shared<HEqMapConvert> EqMapConverter;
	shared<HIBLConvert> IBLConverter;
	DWavefrontParser ObjParser;
	DWavefrontEncoder ObjEncoder;

public:
	HAssetFactory();

	bool CreateTexture2D(DStringView AssetDir, DStringView AssetName);

	bool CreateIBLMaps(DStringView AssetDir, DStringView AssetName);

private:
	
	bool SaveStaticMeshAsset(const DStaticMeshData& MeshData, DStringView AssetPathNoExt);

public:

	bool CreateStaticMesh(DStringView AssetDir, DStringView AssetName);

};
