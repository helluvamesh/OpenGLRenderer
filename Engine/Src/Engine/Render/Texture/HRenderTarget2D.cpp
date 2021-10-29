#include "Engine/pch.h"
#include "HRenderTarget2D.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HRenderTarget2D::HRenderTarget2D() :
	HTexture2DBase(ETextureType::RENDER_TARGET_2D)
{
}


void HRenderTarget2D::Load()
{
	this->Unload();

	// TODO
	// Load settings from .asset file, then allocate GPU buffers
}


void HRenderTarget2D::Load(const DRenderTarget2DSettings& Settings)
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

	glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap));
	if (Settings.Wrap == ETextureWrap::CLAMP_TO_BORDER)
		glCheck(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &Settings.BorderColor.X)); 
	// don't go higher than 8.0f, (it's an extension (not part of the specification but should be available everywhere))
	if (Settings.Anisotropy)
	{
		glCheck(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f));
	}

	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Settings.GenerateMipMaps? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	this->Width = Settings.Width;
	this->Height = Settings.Height;
	this->Format = Settings.Format;

	switch (Settings.Format)
	{
	case ERenderTarget2DFormat::RGB8:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB8, Settings.Width, Settings.Height, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, nullptr
		));
		break;
	}
	case ERenderTarget2DFormat::RGBA8:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA8, Settings.Width, Settings.Height, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, nullptr
		));
		break;
	}
	case ERenderTarget2DFormat::RGB16F:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, Settings.Width, Settings.Height, 0, 
			GL_RGB, GL_FLOAT, nullptr
		));
		break;
	}
	case ERenderTarget2DFormat::RGBA16F:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, Settings.Width, Settings.Height, 0, 
			GL_RGBA, GL_FLOAT, nullptr
		));
		break;
	}
	case ERenderTarget2DFormat::DEPTH:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Settings.Width, Settings.Height, 0, 
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL
		));
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format in ") + this->Name);
	}
	}

	if (Settings.GenerateMipMaps)
	{
		glCheck(glGenerateMipmap(GL_TEXTURE_2D));
	}
}


void HRenderTarget2D::Bind()
{
	glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));
}


void HRenderTarget2D::BindNone()
{
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
}


DBitmapImage HRenderTarget2D::ToImage() const
{
	switch (this->Format)
	{
	case ERenderTarget2DFormat::RGB8:
	{
		glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));

		int32 len = this->Width * this->Height * 3;
		DArray<uint8, DBitmapImageAllocator> buffer (len, true);
		glCheck(glGetTexImage(
			GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
		));
		return DBitmapImage(std::move(buffer), this->Width, this->Height, 3);
		break;
	}
	case ERenderTarget2DFormat::RGBA8:
	{
		glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));

		int32 len = this->Width * this->Height * 4;
		DArray<uint8, DBitmapImageAllocator> buffer (len, true);
		glCheck(glGetTexImage(
			GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
		));
		return DBitmapImage(std::move(buffer), this->Width, this->Height, 4);
		break;
	}
	default:
	{
		SLog::Error(TX("Wrong image format when writing pixels from RenderTarget2D to image: \"") + this->Name + TX("\""));
	}
	}

	return {};
}


DBitmapImageHDR HRenderTarget2D::ToImageHDR() const
{
	switch (this->Format)
	{
	case ERenderTarget2DFormat::RGB16F:
	{
		glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));

		int32 len = this->Width * this->Height * 3;
		DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
		glCheck(glGetTexImage(
			GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
		));
		return DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 3);
		break;
	}
	case ERenderTarget2DFormat::RGBA16F:
	{
		glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));

		int32 len = this->Width * this->Height * 4;
		DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
		glCheck(glGetTexImage(
			GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer.GetDataUnsafe()
		));
		return DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 4);
		break;
	}
	case ERenderTarget2DFormat::DEPTH:
	{
		glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));

		int32 len = this->Width * this->Height;
		DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
		glCheck(glGetTexImage(
			GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, buffer.GetDataUnsafe()
		));
		return DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 1);
		break;
	}
	default:
	{
		SLog::Error(TX("Wrong image format when writing pixels from RenderTarget2D to image: \"") + this->Name + TX("\""));
	}
	}

	return {};
}


int32 HRenderTarget2D::GetWidth() const
{
	return this->Width;
}
int32 HRenderTarget2D::GetHeight() const
{
	return this->Height;
}
ERenderTarget2DFormat HRenderTarget2D::GetFormat() const
{
	return this->Format;
}
