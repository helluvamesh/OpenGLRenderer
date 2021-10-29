#pragma once

#include "HTextureBase.h"


enum class ERenderTarget2DMSFormat : uint8 { RGB8, RGBA8, RGB16F, RGBA16F };


struct DRenderTarget2DMSSettings
{
	ERenderTarget2DMSFormat Format = ERenderTarget2DMSFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;
	ETextureWrap Wrap = ETextureWrap::CLAMP_TO_EDGE;
	int32 Samples = 2;
};


class HRenderTarget2DMS : public HTexture2DBase
{
private:
	ERenderTarget2DMSFormat Format = ERenderTarget2DMSFormat::RGB8;
	int32 Width = 0;
	int32 Height = 0;
	int32 Samples = 2;

public:

	HRenderTarget2DMS();

	// Not Implemented!
	void Load() override;
	void Load(const DRenderTarget2DMSSettings& Settings);

	void Bind() override;
	void BindNone() override;

	int32 GetWidth() const;
	int32 GetHeight() const;
	ERenderTarget2DMSFormat GetFormat() const;
	int32 GetSamples() const;
};