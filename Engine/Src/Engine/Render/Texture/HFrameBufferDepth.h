#pragma once

#include "Engine/Render/Texture/HTextureBase.h"


class HFrameBufferDepth : public HObject
{
public:
	enum class EBlitFilter : uint8 { NEAREST, LINEAR };

private:
	uint32 FrameBufferId = 0;
	int32 ColorTextureCount = 0;
	int32 Width = 0;
	int32 Height = 0;
	DString Name;
	shared<HTextureBase> DepthAttachment;

public:
	HFrameBufferDepth();
	~HFrameBufferDepth();

private:
	void FreeGPU();

public:
	void Prepare(int32 InWidth, int32 InHeight);
	void AttachTextures(int32 MipLevel = 0);
	void Bind(bool Clear);
	void BindNone();

	uint32 GetFrameBufferId();
	
	int32 GetWidth();
	int32 GetHeight();
	
	const DString& GetName() const;
	void SetName(DStringView NewName);

	shared<HTextureBase> GetDepthAttachment() const;
	HTextureBase* GetDepthAttachmentPtr() const;
	void SetDepthAttachment(shared<HTextureBase> NewTexture);

	bool ErrorCheck();
};