#pragma once

#include "DBitmapImage.h"
#include "DBitmapImageHDR.h"
#include "DBitmapImage16.h"
#include "HTextureBase.h"


enum class ERenderTarget2DArrayFormat : uint8 { RGB8, RGBA8, RGB16F, RGBA16F, DEPTH };


struct DRenderTarget2DArraySettings
{
	ERenderTarget2DArrayFormat Format = ERenderTarget2DArrayFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;
	ETextureWrap Wrap = ETextureWrap::CLAMP_TO_EDGE;
	FVec4 BorderColor;
	int32 Length = 1;
	bool Anisotropy = false;
};


class HRenderTarget2DArray : public HTextureBase
{
private:
	ERenderTarget2DArrayFormat Format = ERenderTarget2DArrayFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;
	int32 Length = 1;

public:
	HRenderTarget2DArray();

	// Not Implemented!
	void Load() override;
	void Load(const DRenderTarget2DArraySettings& Settings);

	void Bind() override;
	void BindNone() override;

	int32 GetWidth() const;
	int32 GetHeight() const;
	ERenderTarget2DArrayFormat GetFormat() const;
	int32 GetLength() const;
};
