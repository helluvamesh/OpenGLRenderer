#include "Engine/pch.h"
#include "HFrameBufferDepth.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HFrameBufferDepth::HFrameBufferDepth()
{
	glCheck(glGenFramebuffers(1, &this->FrameBufferId));
}


HFrameBufferDepth::~HFrameBufferDepth()
{
	this->FreeGPU();
}


void HFrameBufferDepth::FreeGPU()
{
	if (this->FrameBufferId)
	{
		glCheck(glDeleteFramebuffers(1, &this->FrameBufferId));
		this->FrameBufferId = 0;
	}
}


void HFrameBufferDepth::Prepare(int32 InWidth, int32 InHeight)
{
	this->Width = InWidth;
	this->Height = InHeight;

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));

	glCheck(glDrawBuffer(GL_NONE));
	glCheck(glReadBuffer(GL_NONE));

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


void HFrameBufferDepth::AttachTextures(int32 MipLevel)
{
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));

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
			SLog::Error(TX(" Unexpected texture type FrameBufferDepth::DepthAttachment in ") + this->Name);
		}
		}
	}
	else
	{
		SLog::Error(TX("FrameBufferDepth::DepthAttachment is null in ") + this->Name);
	}

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	this->ErrorCheck();
}


void HFrameBufferDepth::Bind(bool Clear)
{
	glCheck(glViewport(0, 0, this->Width, this->Height));
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, this->FrameBufferId));
	this->ErrorCheck();
	if (Clear)
	{
		glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
}


void HFrameBufferDepth::BindNone()
{
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


uint32 HFrameBufferDepth::GetFrameBufferId()
{
	return this->FrameBufferId;
}
int32 HFrameBufferDepth::GetWidth()
{
	return this->Width;
}
int32 HFrameBufferDepth::GetHeight()
{
	return this->Height;
}


const DString& HFrameBufferDepth::GetName() const
{
	return this->Name;
}
void  HFrameBufferDepth::SetName(DStringView NewName)
{
	this->Name = DString::From(NewName);
}


shared<HTextureBase> HFrameBufferDepth::GetDepthAttachment() const
{
	return this->DepthAttachment;
}
HTextureBase* HFrameBufferDepth::GetDepthAttachmentPtr() const
{
	return this->DepthAttachment.Ptr();
}
void HFrameBufferDepth::SetDepthAttachment(shared<HTextureBase> NewTexture)
{
	this->DepthAttachment = NewTexture;
}


bool HFrameBufferDepth::ErrorCheck()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			SLog::Error(TX("FrameBuffer Error: Incomplete attachment in FrameBufferDepth \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			SLog::Error(TX("FrameBuffer Error: wrong dimensions in FrameBufferDepth \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			SLog::Error(TX("FrameBuffer Error: missing attachment in FrameBufferDepth \"") + this->Name + TX("\""));
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			SLog::Error(TX("FrameBuffer Error: unsupported combination of settings in FrameBufferDepth \"") + this->Name + TX("\""));
			break;
		default:
			SLog::Error(TX("FrameBuffer Error in FrameBufferDepth \"") + this->Name + TX("\""));
		}
		return false;
	}
	return true;
}