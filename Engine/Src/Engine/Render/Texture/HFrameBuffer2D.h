/*
- multiple render targets
- no depth buffer
- no stencil buffer
- can be HDR
- can render into cubemaps
- can render into mip maps
*/

#pragma once

#include "Engine/Render/Texture/HTextureBase.h"


class HFrameBuffer2D : public HObject
{
private:
	uint32 FrameBufferId = 0;
	int32 ColorTextureCount = 1;
	int32 Width = 0;
	int32 Height = 0;
	
	DString Name;
	DFixedArray<shared<HTextureBase>, 10> ColorAttachments;

public:
	HFrameBuffer2D();
	~HFrameBuffer2D();

private:
	void FreeGPU();

public:
	/* 1 <= TextureCount <= 10 */
	void Prepare(int32 InWidth, int32 InHeight, int32 TextureCount);
	void AttachTextures(int32 MipLevel = 0);
	void AttachCubemapSide(int32 Side, int32 MipLevel = 0);
	void Bind();
	void BindNone();

	uint32 GetFrameBufferId();
	int32 GetWidth();
	int32 GetHeight();

	const DString& GetName() const;
	void SetName(DStringView NewName);

	shared<HTextureBase> GetColorAttachment(int32 Index) const;
	HTextureBase* GetColorAttachmentPtr(int32 Index) const;
	void SetColorAttachment(int32 Index, shared<HTextureBase> NewTexture);

	bool ErrorCheck();
};