#pragma once

#include "HTextureBase.h"
#include "DBitmapImage.h"
#include "DBitmapImageHDR.h"


enum class ERenderTargetCubeFormat : uint8 { RGB8, RGB16F };


struct DRenderTargetCubeSettings
{
	ERenderTargetCubeFormat Format = ERenderTargetCubeFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;
	ETextureWrap Wrap = ETextureWrap::CLAMP_TO_EDGE;
	FVec4 BorderColor;
	bool Anisotropy = false;
	bool GenerateMipMaps = false;
	int32 FixedMipCount = 0;
};


class HRenderTargetCube : public HTextureCubeBase
{
public:
	static const DFixedArray<FMat4, 6> CubeSideViews;

private:
	ERenderTargetCubeFormat Format = ERenderTargetCubeFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;

public:
	HRenderTargetCube();

	// Not Implemented!
	void Load() override;
	void Load(const DRenderTargetCubeSettings& Settings);

	void Bind() override;
	void BindNone() override;

	void ToImage(
		DBitmapImageHDR& OutImage_PosX,
		DBitmapImageHDR& OutImage_NegX,
		DBitmapImageHDR& OutImage_PosY,
		DBitmapImageHDR& OutImage_NegY,
		DBitmapImageHDR& OutImage_PosZ,
		DBitmapImageHDR& OutImage_NegZ
	) const;

	void ToImage(
		DBitmapImage& OutImage_PosX,
		DBitmapImage& OutImage_NegX,
		DBitmapImage& OutImage_PosY,
		DBitmapImage& OutImage_NegY,
		DBitmapImage& OutImage_PosZ,
		DBitmapImage& OutImage_NegZ
	) const;

	void ToImageMips(
		int32 MipCount,
		DArray<DBitmapImageHDR>& OutMips_PosX,
		DArray<DBitmapImageHDR>& OutMips_NegX,
		DArray<DBitmapImageHDR>& OutMips_PosY,
		DArray<DBitmapImageHDR>& OutMips_NegY,
		DArray<DBitmapImageHDR>& OutMips_PosZ,
		DArray<DBitmapImageHDR>& OutMips_NegZ
	) const;

	int32 GetWidth() const;
	int32 GetHeight() const;
	ERenderTargetCubeFormat GetFormat() const;
};