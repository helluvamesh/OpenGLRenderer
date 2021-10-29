#include "Engine\pch.h"
#include "HRenderTarget2DArray.h"
#include "GL\glew.h"
#include "Engine\Render\RenderUtil\GLUtil.h"


HRenderTarget2DArray::HRenderTarget2DArray() :
	HTextureBase(ETextureType::RENDER_TARGET_2D_ARRAY)
{
}


void HRenderTarget2DArray::Load()
{
	this->Unload();

	// TODO
	// Load settings from .asset file, then allocate GPU buffers
}


void HRenderTarget2DArray::Load(const DRenderTarget2DArraySettings& Settings)
{
	this->Unload();

	glCheck(glGenTextures(1, &this->TextureId));

	GLenum glWrap;
	switch (Settings.Wrap)
	{
	case ETextureWrap::REPEAT: glWrap = GL_REPEAT; break;
	case ETextureWrap::CLAMP_TO_EDGE: glWrap = GL_CLAMP_TO_EDGE; break;
	case ETextureWrap::CLAMP_TO_BORDER: glWrap = GL_CLAMP_TO_BORDER; break;
	case ETextureWrap::MIRRORED_REPEAT: glWrap = GL_MIRRORED_REPEAT; break;
	default: glWrap = GL_CLAMP_TO_EDGE;
	}

	glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, this->TextureId));
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, glWrap));
	if (Settings.Wrap == ETextureWrap::CLAMP_TO_BORDER)
		glCheck(glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, &Settings.BorderColor.X)); 
	// don't go higher than 8.0f, (it's an extension (not part of the specification but should be available everywhere))
	if (Settings.Anisotropy)
	{
		glCheck(glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f));
	}

	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glCheck(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	this->Width = Settings.Width;
	this->Height = Settings.Height;
	this->Format = Settings.Format;

	switch (Settings.Format)
	{
	case ERenderTarget2DArrayFormat::RGB8:
	{
		glCheck(glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, Settings.Width, Settings.Height, Settings.Length, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, nullptr
		));
		break;
	}
	case ERenderTarget2DArrayFormat::RGBA8:
	{
		glCheck(glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Settings.Width, Settings.Height, Settings.Length, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, nullptr
		));
		break;
	}
	case ERenderTarget2DArrayFormat::RGB16F:
	{
		glCheck(glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_RGB16F, Settings.Width, Settings.Height, Settings.Length, 0, 
			GL_RGB, GL_FLOAT, nullptr
		));
		break;
	}
	case ERenderTarget2DArrayFormat::RGBA16F:
	{
		glCheck(glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, Settings.Width, Settings.Height, Settings.Length, 0, 
			GL_RGBA, GL_FLOAT, nullptr
		));
		break;
	}
	case ERenderTarget2DArrayFormat::DEPTH:
	{
		glCheck(glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, Settings.Width, Settings.Height, Settings.Length, 0, 
			GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
		));
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format in ") + this->Name);
	}
	}
}


void HRenderTarget2DArray::Bind()
{
	glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, this->TextureId));
}


void HRenderTarget2DArray::BindNone()
{
	glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
}


int32 HRenderTarget2DArray::GetWidth() const
{
	return this->Width;
}
int32 HRenderTarget2DArray::GetHeight() const
{
	return this->Height;
}
ERenderTarget2DArrayFormat HRenderTarget2DArray::GetFormat() const
{
	return this->Format;
}
int32 HRenderTarget2DArray::GetLength() const
{
	return this->Length;
}
