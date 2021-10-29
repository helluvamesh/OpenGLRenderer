#include "Engine/pch.h"
#include "HRenderTarget2DMS.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HRenderTarget2DMS::HRenderTarget2DMS() : 
	HTexture2DBase(ETextureType::RENDER_TARGET_2D_MS)
{
}


void HRenderTarget2DMS::Load()
{
	this->Unload();

	// TODO
	// Load settings from .asset file, then allocate GPU buffers
}


void HRenderTarget2DMS::Load(const DRenderTarget2DMSSettings& Settings)
{
	this->Unload();

	glCheck(glGenTextures(1, &this->TextureId));

	glCheck(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->TextureId));

	this->Format = Settings.Format;
	this->Width = Settings.Width;
	this->Height = Settings.Height;
	this->Samples = Settings.Samples;

	switch (this->Format)
	{
	case ERenderTarget2DMSFormat::RGB8:
	{
		glCheck(glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE, Settings.Samples, GL_RGB8, Settings.Width, Settings.Height, GL_TRUE
		));
		break;
	}
	case ERenderTarget2DMSFormat::RGBA8:
	{
		glCheck(glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE, Settings.Samples, GL_RGBA8, Settings.Width, Settings.Height, GL_TRUE
		));
		break;
	}
	case ERenderTarget2DMSFormat::RGB16F:
	{
		glCheck(glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE, Settings.Samples, GL_RGB16F, Settings.Width, Settings.Height, GL_TRUE
		));
		break;
	}
	case ERenderTarget2DMSFormat::RGBA16F:
	{
		glCheck(glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE, Settings.Samples, GL_RGBA16F, Settings.Width, Settings.Height, GL_TRUE
		));
		break;
	}
	default: 
	{
		SLog::Error(TX("Unsupported texture format in ") + this->Name);
	}
	}
}


void HRenderTarget2DMS::Bind()
{
	glCheck(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->TextureId));
}


void HRenderTarget2DMS::BindNone()
{
	glCheck(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
}


int32 HRenderTarget2DMS::GetWidth() const
{
	return this->Width;
}
int32 HRenderTarget2DMS::GetHeight() const
{
	return this->Height;
}
ERenderTarget2DMSFormat HRenderTarget2DMS::GetFormat() const
{
	return this->Format;
}
int32 HRenderTarget2DMS::GetSamples() const
{
	return this->Samples;
}
