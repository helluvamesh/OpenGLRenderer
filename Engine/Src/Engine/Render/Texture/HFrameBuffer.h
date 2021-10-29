/*
	- multiple render targets
	- depth buffer
	- optional stencil buffer
	- can be HDR
	- can be multisampled
	- can render into cubemaps
	- can render into mip maps
*/

#pragma once

#include "Engine/Render/Texture/HTextureBase.h"


class HFrameBuffer : public HObject
{
public:
	enum class EBlitFilter : uint8 { NEAREST, LINEAR };

private:
	uint32 FrameBufferId = 0;
	uint32 RenderBufferId = 0;
	int32 ColorTextureCount = 0;
	int32 Width = 0;
	int32 Height = 0;
	bool HasStencil = false;
	DString Name;
	DFixedArray<shared<HTextureBase>, 10> ColorAttachments;
	shared<HTextureBase> DepthAttachment;

public:
	HFrameBuffer();
	~HFrameBuffer();

private:
	void FreeGPU();

public:
	/* 1 <= TextureCount <= 10 */
	void Prepare(int32 InWidth, int32 InHeight, int32 ColorTextureCount, int32 SampleCount, bool UseStencil = false);
	void AttachTextures(int32 MipLevel = 0);
	void AttachCubemapSide(int32 Side, int32 MipLevel = 0);
	void Bind(bool Clear);
	void BindNone();
	
	// multi sample frame buffer to single sample
	void BlitTo(shared<HFrameBuffer> Other, bool Color, bool Depth, bool Stencil, EBlitFilter Filter);

	uint32 GetFrameBufferId();
	uint32 GetRenderBufferId();

	int32 GetWidth();
	int32 GetHeight();
	bool GetHasStencil();

	const DString& GetName() const;
	void SetName(DStringView NewName);

	shared<HTextureBase> GetColorAttachment(int32 Index) const;
	HTextureBase* GetColorAttachmentPtr(int32 Index) const;
	void SetColorAttachment(int32 Index, shared<HTextureBase> NewTexture);

	shared<HTextureBase> GetDepthAttachment() const;
	HTextureBase* GetDepthAttachmentPtr() const;
	void SetDepthAttachment(shared<HTextureBase> NewTexture);

	bool ErrorCheck();
};