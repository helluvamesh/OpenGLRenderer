#pragma once

#include "HTextureBase.h"
#include "DBitmapImage.h"
#include "DBitmapImage16.h"
#include "DBitmapImageHDR.h"


DECLARE_ENUM(
	ETexture2DFormat,
	BC1,      // sRGB/no sRGB
	BC3,      // sRGB/no sRGB
	BC4,
	BC5,      // no sRGB
	RGB8,     // sRGB/ no sRGB
	RGBA8,    // sRGB/ no sRGB
	RGB16F,   // no sRGB
	RGBA16F,  // no sRGB
	R16
)


struct DTexture2DAssetSettings : public DStruct
{
	REFLECT_H(DTexture2DAssetSettings, DStruct)
public:
	ETexture2DFormat Format = ETexture2DFormat::BC1;
	bool sRGB = true;
	ETextureWrap Wrap = ETextureWrap::REPEAT;
};


struct DTexture2DAssetDataBitmap : public DStruct
{
	REFLECT_H(DTexture2DAssetDataBitmap, DStruct)
public:
	DArray<DBitmapImage> Mips;
};


struct DTexture2DAssetDataBitmap16 : public DStruct
{
	REFLECT_H(DTexture2DAssetDataBitmap, DStruct)
public:
	DArray<DBitmapImage16> Mips;
};


struct DTexture2DSettings
{
	ETexture2DFormat Format = ETexture2DFormat::RGB8;
	bool sRGB = true;
	bool GenerateMipMaps = true;
	ETextureWrap Wrap = ETextureWrap::REPEAT;
};


class HTexture2D : public HTexture2DBase
{
private:
	int32 Width = 0;
	int32 Height = 0;
	
public:
	HTexture2D();

	void Load() override;

	void Load(const DBitmapImage& ImageData, const DTexture2DSettings& Settings);
	void Load(const DBitmapImage16& ImageData, const DTexture2DSettings& Settings);
	void Load(const DBitmapImageHDR& ImageData, const DTexture2DSettings& Settings);

	void Bind() override;
	void BindNone() override;

	int32 GetWidth() const;
	int32 GetHeight() const;
};
