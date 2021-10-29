#include "Engine/pch.h"
#include "HFrameBuffer2D.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HFrameBuffer2D::HFrameBuffer2D()
{
	glCheck(glGenFramebuffers(1, &this->FrameBufferId));
}


HFrameBuffer2D::~HFrameBuffer2D()
{
	this->FreeGPU();
}


void HFrameBuffer2D::FreeGPU()
{
	if (this->FrameBufferId)
	{
		glCheck(glDeleteFramebuffers(1, &this->FrameBufferId));
		this->FrameBufferId = 0;
	}
}


void HFrameBuffer2D::Prepare(int32 InWidth, int32 InHeight, int32 TextureCount)
{
	this->ColorTextureCount = FMath::Clamp(TextureCount, 0, 10);
	this->Width = InWidth;
	this->Height = InHeight;

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));
	
	// enable multiple render targets
	if (this->ColorTextureCount > 0)
	{
		uint32 colorAttachments[10];
		for (int32 i = 0; i < TextureCount; i++)
		{
			colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(this->ColorTextureCount, colorAttachments);
	}
	else
	{
		glCheck(glDrawBuffer(GL_NONE));
		glCheck(glReadBuffer(GL_NONE));
	}

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


void HFrameBuffer2D::AttachTextures(int32 MipLevel)
{
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));

	for (int32 i = 0; i < this->ColorTextureCount; i++)
	{
		HTextureBase* texture = this->ColorAttachments[i].Ptr();
		if (texture)
		{
			switch (texture->Type)
			{
			case ETextureType::RENDER_TARGET_2D:
			{
				glCheck(glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
					GL_TEXTURE_2D, texture->GetTextureId(), MipLevel
				));
				break;
			}
			case ETextureType::RENDER_TARGET_2D_MS:
			{
				glCheck(glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
					GL_TEXTURE_2D_MULTISAMPLE, texture->GetTextureId(), 0
				));
				break;
			}
			default:
			{
				SLog::Error(
					DString::Format(TX(" Unexpected texture type FrameBuffer2D::ColorArrachment[{0}] in {1}"), { i, this->Name })
				);
			}
			}
		}
		else
		{
			SLog::Error(
				DString::Format(TX("FrameBuffer2D::ColorAttachment[{0}] is null in {1}"), { i, this->Name })
			);
		}
	}

	this->ErrorCheck();

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


void HFrameBuffer2D::AttachCubemapSide(int32 Side, int32 MipLevel)
{
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));
	
	for (int32 i = 0; i < this->ColorTextureCount; i++)
	{
		HTextureBase* texture = this->ColorAttachments[i].Ptr();
		if (texture)
		{
			glCheck(glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + Side, this->ColorAttachments[i]->GetTextureId(), MipLevel
			));
			this->ErrorCheck();
		}
		else
		{
			SLog::Error(
				DString::Format(TX("FrameBuffer2D::ColorAttachment[{0}] is null in {1}"), { i, this->Name })
			);
		}
	}

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


void HFrameBuffer2D::Bind()
{
	glCheck(glViewport(0, 0, this->Width, this->Height));
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));
	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	this->ErrorCheck();
	glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}


void HFrameBuffer2D::BindNone()
{
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}


uint32 HFrameBuffer2D::GetFrameBufferId()
{
	return this->FrameBufferId;
}
int32 HFrameBuffer2D::GetWidth()
{
	return this->Width;
}
int32 HFrameBuffer2D::GetHeight()
{
	return this->Height;
}


const DString& HFrameBuffer2D::GetName() const
{
	return this->Name;
}
void  HFrameBuffer2D::SetName(DStringView NewName)
{
	this->Name = DString::From(NewName);
}


shared<HTextureBase> HFrameBuffer2D::GetColorAttachment(int32 Index) const
{
	return this->ColorAttachments[Index];
}
HTextureBase* HFrameBuffer2D::GetColorAttachmentPtr(int32 Index) const
{
	return this->ColorAttachments[Index].Ptr();
}
void HFrameBuffer2D::SetColorAttachment(int32 Index, shared<HTextureBase> NewTexture)
{
	this->ColorAttachments[Index] = NewTexture;
}


bool HFrameBuffer2D::ErrorCheck()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			SLog::Error(TX("FrameBuffer Error: Incomplete attachment in FrameBuffer2D \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			SLog::Error(TX("FrameBuffer Error: wrong dimensions in FrameBuffer2D \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			SLog::Error(TX("FrameBuffer Error: missing attachment in FrameBuffer2D \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			SLog::Error(TX("FrameBuffer Error: unsupported combination of settings in FrameBuffer2D \"") + this->Name + TX("\""));
			break;
		default:
			SLog::Error(TX("FrameBuffer Error in FrameBuffer2D \"") + this->Name + TX("\""));
		}
		return false;
	}
	return true;
}