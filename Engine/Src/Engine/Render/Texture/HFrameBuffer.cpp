#include "Engine/pch.h"
#include "HFrameBuffer.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HFrameBuffer::HFrameBuffer()
{
	glCheck(glGenFramebuffers(1, &this->FrameBufferId));
	glCheck(glGenRenderbuffers(1, &this->RenderBufferId));
}


HFrameBuffer::~HFrameBuffer()
{
	this->FreeGPU();
}


void HFrameBuffer::FreeGPU()
{
	if (this->FrameBufferId)
	{
		glCheck(glDeleteFramebuffers(1, &this->FrameBufferId));
		this->FrameBufferId = 0;
	}
	if (this->RenderBufferId)
	{
		glCheck(glDeleteRenderbuffers(1, &this->RenderBufferId));
		this->RenderBufferId = 0;
	}
}


void HFrameBuffer::Prepare(int32 InWidth, int32 InHeight, int32 ColorTextureCount, int32 SampleCount, bool UseStencil)
{
	this->Width = InWidth;
	this->Height = InHeight;
	this->ColorTextureCount = FMath::Clamp(ColorTextureCount, 0, 10);
	this->HasStencil = UseStencil;

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));
	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, this->RenderBufferId));

	if (SampleCount >= 2) 
	{
		if (UseStencil)
		{
			glCheck(glRenderbufferStorageMultisample(
				GL_RENDERBUFFER, SampleCount, GL_DEPTH24_STENCIL8, InWidth, InHeight
			));
		}
		else
		{
			glCheck(glRenderbufferStorageMultisample(
				GL_RENDERBUFFER, SampleCount, GL_DEPTH_COMPONENT24, InWidth, InHeight
			));
		}
	} 
	else 
	{
		if (UseStencil)
		{
			glCheck(glRenderbufferStorage(
				GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, InWidth, InHeight
			));
		}
		else
		{
			glCheck(glRenderbufferStorage(
				GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, InWidth, InHeight
			));
		}
	}

	if (UseStencil)
	{
		glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->RenderBufferId)); 
	}
	else
	{
		glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->RenderBufferId)); 
	}

	// enable multiple render targets
	if (this->ColorTextureCount > 0)
	{
		uint32 colorAttachments[10];
		for (int32 i = 0; i < ColorTextureCount; i++)
		{
			colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glCheck(glDrawBuffers(this->ColorTextureCount, colorAttachments));
	}
	else
	{
		glCheck(glDrawBuffer(GL_NONE));
		glCheck(glReadBuffer(GL_NONE));
	}

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}


void HFrameBuffer::AttachTextures(int32 MipLevel)
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
			case ETextureType::RENDER_TARGET_2D_ARRAY:
			{
				glCheck(glFramebufferTexture(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
					texture->GetTextureId(), 0
				));
				break;
			}
			default:
			{
				SLog::Error(
					DString::Format(TX(" Unexpected texture type FrameBuffer::ColorArrachment[{0}] in {1}"), { i, this->Name })
				);
			}
			}
		}
		else
		{
			SLog::Error(
				DString::Format(TX("FrameBuffer::ColorAttachment[{0}] is null in {1}"), { i, this->Name })
			);
		}
	}

	if (this->DepthAttachment)
	{
		switch (this->DepthAttachment->Type)
		{
		case ETextureType::RENDER_TARGET_2D:
		{
			glCheck(glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
				GL_TEXTURE_2D, this->DepthAttachment->GetTextureId(), 0
			));
			break;
		}
		case ETextureType::RENDER_TARGET_2D_MS:
		{
			glCheck(glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
				GL_TEXTURE_2D_MULTISAMPLE, this->DepthAttachment->GetTextureId(), 0
			));
			break;
		}
		case ETextureType::RENDER_TARGET_2D_ARRAY:
		{
			glCheck(glFramebufferTexture(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
				this->DepthAttachment->GetTextureId(), 0
			));
			break;
		}
		default:
		{
			SLog::Error(TX(" Unexpected texture type FrameBuffer::DepthAttachment in ") + this->Name);
		}
		}
	}

	this->ErrorCheck();
	
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


void HFrameBuffer::AttachCubemapSide(int32 Side, int32 MipLevel)
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
		}
		else
		{
			SLog::Error(
				DString::Format(TX("FrameBuffer::ColorAttachment[{0}] is null in {1}"), { i, this->Name })
			);
		}
	}

	this->ErrorCheck();
	
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


void HFrameBuffer::Bind(bool Clear)
{
	glCheck(glViewport(0, 0, this->Width, this->Height));
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));
	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, this->RenderBufferId));
	this->ErrorCheck();
	if (Clear)
	{
		if (this->HasStencil)
		{
			glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		}
		else
		{
			glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		}
	}
}


void HFrameBuffer::BindNone()
{
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glCheck(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}


void HFrameBuffer::BlitTo(shared<HFrameBuffer> Other, bool Color, bool Depth, bool Stencil, EBlitFilter Filter)
{
	GLenum filter;
	switch (Filter)
	{
	case EBlitFilter::NEAREST: filter = GL_NEAREST;  break;
	case EBlitFilter::LINEAR: filter = GL_LINEAR;  break; 
	default: filter = GL_NEAREST;
	}
	
	glCheck(glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FrameBufferId));
	glCheck(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Other->GetFrameBufferId()));
	
	GLint bits = 0;
	if (Color) bits |= GL_COLOR_BUFFER_BIT;
	if (Depth) bits |= GL_DEPTH_BUFFER_BIT;
	if (Stencil) bits |= GL_STENCIL_BUFFER_BIT;

	glCheck(glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, Other->GetWidth(), Other->GetHeight(), bits, filter));
	
	glCheck(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	glCheck(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}


uint32 HFrameBuffer::GetFrameBufferId()
{
	return this->FrameBufferId;
}
uint32 HFrameBuffer::GetRenderBufferId()
{
	return this->RenderBufferId;
}
int32 HFrameBuffer::GetWidth()
{
	return this->Width;
}
int32 HFrameBuffer::GetHeight()
{
	return this->Height;
}
bool HFrameBuffer::GetHasStencil()
{
	return this->HasStencil;
}


const DString& HFrameBuffer::GetName() const
{
	return this->Name;
}
void  HFrameBuffer::SetName(DStringView NewName)
{
	this->Name = DString::From(NewName);
}


shared<HTextureBase> HFrameBuffer::GetColorAttachment(int32 Index) const
{
	return this->ColorAttachments[Index];
}
HTextureBase* HFrameBuffer::GetColorAttachmentPtr(int32 Index) const
{
	return this->ColorAttachments[Index].Ptr();
}
void HFrameBuffer::SetColorAttachment(int32 Index, shared<HTextureBase> NewTexture)
{
	this->ColorAttachments[Index] = NewTexture;
}


shared<HTextureBase> HFrameBuffer::GetDepthAttachment() const
{
	return this->DepthAttachment;
}
HTextureBase* HFrameBuffer::GetDepthAttachmentPtr() const
{
	return this->DepthAttachment.Ptr();
}
void HFrameBuffer::SetDepthAttachment(shared<HTextureBase> NewTexture)
{
	this->DepthAttachment = NewTexture;
}


bool HFrameBuffer::ErrorCheck()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			SLog::Error(TX("FrameBuffer Error: Incomplete attachment in FrameBuffer \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			SLog::Error(TX("FrameBuffer Error: wrong dimensions in FrameBuffer \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			SLog::Error(TX("FrameBuffer Error: missing attachment in FrameBuffer \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			SLog::Error(TX("FrameBuffer Error: unsupported combination of settings in FrameBuffer \"") + this->Name + TX("\""));
			break;
		default:
			SLog::Error(TX("FrameBuffer Error in FrameBuffer \"") + this->Name + TX("\""));
		}
		return false;
	}
	return true;
}