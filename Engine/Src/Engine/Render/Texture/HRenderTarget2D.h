#pragma once

#include "HTextureBase.h"
#include "DBitmapImage.h"
#include "DBitmapImageHDR.h"
#include "DBitmapImage16.h"


enum class ERenderTarget2DFormat : uint8 { RGB8, RGBA8, RGB16F, RGBA16F, DEPTH };


struct DRenderTarget2DSettings
{
	ERenderTarget2DFormat Format = ERenderTarget2DFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;
	ETextureWrap Wrap = ETextureWrap::CLAMP_TO_EDGE;
	FVec4 BorderColor;
	bool Anisotropy = false;
	bool GenerateMipMaps = false;
};


class HRenderTarget2D : public HTexture2DBase
{
private:
	ERenderTarget2DFormat Format = ERenderTarget2DFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;

public:
	HRenderTarget2D();

	// Not Implemented!
	void Load() override;
	void Load(const DRenderTarget2DSettings& Settings);

	void Bind() override;
	void BindNone() override;

	DBitmapImage ToImage() const;
	DBitmapImageHDR ToImageHDR() const;

	int32 GetWidth() const;
	int32 GetHeight() const;
	ERenderTarget2DFormat GetFormat() const;
};