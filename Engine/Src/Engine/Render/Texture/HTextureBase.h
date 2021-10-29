#pragma once

#include "Engine/Asset/HAsset.h"


DECLARE_ENUM(
	ETextureType, 
		TEXTURE_2D, 
		TEXTURE_CUBE, 
		RENDER_TARGET_2D, 
		RENDER_TARGET_CUBE, 
		RENDER_TARGET_2D_MS,
		RENDER_TARGET_2D_ARRAY
)


DECLARE_ENUM( 
	ETextureWrap, 
		REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER, MIRRORED_REPEAT
)


#define TEXTURE_ANISOTROPY_LEVEL 8.0f


class HTextureBase : public HAsset
{
protected:
	uint32 TextureId = 0;

	void FreeGPU();

public:
	const ETextureType Type = ETextureType::TEXTURE_2D;
	
	HTextureBase(ETextureType InType);
	~HTextureBase();

public:
	bool IsLoaded() override;
	void Unload() override;

	uint32 GetTextureId() const;

	virtual void Bind() = 0;
	virtual void BindNone() = 0;

	static void BindNoneStatic();
};


class HTexture2DBase : public HTextureBase
{
public:
	HTexture2DBase(ETextureType InType);
};


class HTextureCubeBase : public HTextureBase
{
public:
	HTextureCubeBase(ETextureType InType);
};
