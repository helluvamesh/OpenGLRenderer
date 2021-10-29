#pragma once

#include "HTextureBase.h"
#include "DBitmapImage.h"
#include "DBitmapImageHDR.h"


DECLARE_ENUM(
	ETextureCubeFormat,
	RGB8,      // sRGB / no sRGB
	RGB16F     // no sRGB
)


struct DTextureCubeAssetSettings : public DStruct
{
	REFLECT_H(DTextureCubeAssetSettings, DStruct)
public:
	ETextureCubeFormat Format = ETextureCubeFormat::RGB8;
	bool sRGB = true;
	ETextureWrap Wrap = ETextureWrap::CLAMP_TO_EDGE;
};


struct DTextureCubeAssetDataBitmap : public DStruct
{
	REFLECT_H(DTextureCubeAssetDataBitmap, DStruct)
public:
	DArray<DBitmapImage> Mips_PosX;
	DArray<DBitmapImage> Mips_NegX;
	DArray<DBitmapImage> Mips_PosY;
	DArray<DBitmapImage> Mips_NegY;
	DArray<DBitmapImage> Mips_PosZ;
	DArray<DBitmapImage> Mips_NegZ;
};


struct DTextureCubeAssetDataHDR : public DStruct
{
	REFLECT_H(DTextureCubeAssetDataHDR, DStruct)
public:
	DArray<DBitmapImageHDR> Mips_PosX;
	DArray<DBitmapImageHDR> Mips_NegX;
	DArray<DBitmapImageHDR> Mips_PosY;
	DArray<DBitmapImageHDR> Mips_NegY;
	DArray<DBitmapImageHDR> Mips_PosZ;
	DArray<DBitmapImageHDR> Mips_NegZ;
};


struct DTextureCubeSettings
{
	ETextureCubeFormat Format = ETextureCubeFormat::RGB8;
	bool sRGB = true;
	bool GenerateMipMaps = true;
	ETextureWrap Wrap = ETextureWrap::CLAMP_TO_EDGE;
};


/*
	6-image cubemap
	when loaded from source file:
	filepath: ...x.ext
	where "x" is "_PosX"(left), "_NegX"(right), "_PosY"(top), "_NegY"(bottom), "_PosZ"(back), "_NegZ"(front) when loading
*/
class HTextureCube : public HTextureCubeBase
{
private:
	int32 Width = 0;
	int32 Height = 0;

public:
	
	HTextureCube();

	void Load() override;

	void Load(
		const DBitmapImage& Image_PosX,
		const DBitmapImage& Image_NegX,
		const DBitmapImage& Image_PosY,
		const DBitmapImage& Image_NegY,
		const DBitmapImage& Image_PosZ,
		const DBitmapImage& Image_NegZ,
		const DTextureCubeSettings& Settings
	);

	void Load(
		const DBitmapImageHDR& Image_PosX,
		const DBitmapImageHDR& Image_NegX,
		const DBitmapImageHDR& Image_PosY,
		const DBitmapImageHDR& Image_NegY,
		const DBitmapImageHDR& Image_PosZ,
		const DBitmapImageHDR& Image_NegZ,
		const DTextureCubeSettings& Settings
	);

	void Bind() override;
	void BindNone() override;

	int32 GetWidth() const;
	int32 GetHeight() const;
};