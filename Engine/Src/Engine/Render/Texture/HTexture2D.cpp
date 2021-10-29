#include "Engine/pch.h"
#include "HTexture2D.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"
#include "DBitmapImageHDR.h"


HTexture2D::HTexture2D() : 
	HTexture2DBase(ETextureType::TEXTURE_2D)
{
}


void HTexture2D::Load()
{
	this->Unload();

	glCheck(glGenTextures(1, &this->TextureId));
	
	// don't go higher than 8.0f, (it's an extension (not part of the specification but should be available everywhere))
	glCheck(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, TEXTURE_ANISOTROPY_LEVEL));

	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));  
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	DArray<byte> bytes;
	if (SBinaryFile::ReadFile(this->Filepath, /*out*/bytes) == false) return;

	int32 byteProgress = 0;

	DAssetHeader header;
	byteProgress += SStrictSerializer::DeserializeStruct(
		header, DAssetHeader::GetDeclaredClass(), &bytes[byteProgress]
	);
		
	if (header.AssetType != EAssetType::TEXTURE_2D)
	{
		SLog::Error(TX("Expected TEXTURE_2D asset type in ") + this->Filepath);
		return;
	}

	DTexture2DAssetSettings settings;
	byteProgress += SStrictSerializer::DeserializeStruct(
		settings, DTexture2DAssetSettings::GetDeclaredClass(), &bytes[byteProgress]
	);

	GLenum glWrap;
	switch (settings.Wrap)
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

	switch (settings.Format)
	{
	case ETexture2DFormat::BC1:
	{
		DTexture2DAssetDataBitmap data;
		SStrictSerializer::DeserializeStruct(
			data, DTexture2DAssetDataBitmap::GetDeclaredClass(), &bytes[byteProgress]
		);

		DArray<DBitmapImage>& mips = data.Mips;

		if (data.Mips.Length() > 0)
		{
			this->Width = mips[0].GetWidth();
			this->Height = mips[0].GetHeight();
		}

		int32 mipCount = mips.Length();
		for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
		{
			DBitmapImage& mip = mips[mip_i];
			glCheck(glCompressedTexImage2D(
				GL_TEXTURE_2D, mip_i, settings.sRGB? GL_COMPRESSED_SRGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 
				mip.GetWidth(), mip.GetHeight(), 0, mip.GetBuffer().Length(), mip.GetBuffer().Data()
			));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount));
		break;
	}
	case ETexture2DFormat::BC3:
	{
		DTexture2DAssetDataBitmap data;
		SStrictSerializer::DeserializeStruct(
			data, DTexture2DAssetDataBitmap::GetDeclaredClass(), &bytes[byteProgress]
		);

		DArray<DBitmapImage>& mips = data.Mips;

		if (data.Mips.Length() > 0)
		{
			this->Width = mips[0].GetWidth();
			this->Height = mips[0].GetHeight();
		}

		int32 mipCount = mips.Length();
		for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
		{
			DBitmapImage& mip = mips[mip_i];
			glCheck(glCompressedTexImage2D(
				GL_TEXTURE_2D, mip_i, settings.sRGB? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 
				mip.GetWidth(), mip.GetHeight(), 0, mip.GetBuffer().Length(), mip.GetBuffer().Data()
			));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount));
		break;
	}
	case ETexture2DFormat::BC4:
	{
		DTexture2DAssetDataBitmap data;
		SStrictSerializer::DeserializeStruct(
			data, DTexture2DAssetDataBitmap::GetDeclaredClass(), &bytes[byteProgress]
		);

		DArray<DBitmapImage>& mips = data.Mips;

		if (data.Mips.Length() > 0)
		{
			this->Width = mips[0].GetWidth();
			this->Height = mips[0].GetHeight();
		}

		int32 mipCount = mips.Length();
		for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
		{
			DBitmapImage& mip = mips[mip_i];
			glCheck(glCompressedTexImage2D(
				GL_TEXTURE_2D, mip_i, GL_COMPRESSED_RED_RGTC1, 
				mip.GetWidth(), mip.GetHeight(), 0, mip.GetBuffer().Length(), mip.GetBuffer().Data()
			));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount));
		break;
	}
	case ETexture2DFormat::BC5:
	{
		DTexture2DAssetDataBitmap data;
		SStrictSerializer::DeserializeStruct(
			data, DTexture2DAssetDataBitmap::GetDeclaredClass(), &bytes[byteProgress]
		);

		DArray<DBitmapImage>& mips = data.Mips;

		if (data.Mips.Length() > 0)
		{
			this->Width = mips[0].GetWidth();
			this->Height = mips[0].GetHeight();
		}

		int32 mipCount = mips.Length();
		for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
		{
			DBitmapImage& mip = mips[mip_i];
			glCheck(glCompressedTexImage2D(
				GL_TEXTURE_2D, mip_i, GL_COMPRESSED_RG_RGTC2, 
				mip.GetWidth(), mip.GetHeight(), 0, mip.GetBuffer().Length(), mip.GetBuffer().Data()
			));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount));
		break;
	}
	case ETexture2DFormat::RGB8:
	{
		DTexture2DAssetDataBitmap data;
		SStrictSerializer::DeserializeStruct(
			data, DTexture2DAssetDataBitmap::GetDeclaredClass(), &bytes[byteProgress]
		);

		DArray<DBitmapImage>& mips = data.Mips;

		if (data.Mips.Length() > 0)
		{
			this->Width = mips[0].GetWidth();
			this->Height = mips[0].GetHeight();
		}

		int32 mipCount = mips.Length();
		for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
		{
			DBitmapImage& mip = mips[mip_i];
			glCheck(glTexImage2D(
				GL_TEXTURE_2D, mip_i, settings.sRGB? GL_SRGB : GL_RGB, mip.GetWidth(), mip.GetHeight(), 0,
				GL_RGB, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
			));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount));
		break;
	}
	case ETexture2DFormat::RGBA8:
	{
		DTexture2DAssetDataBitmap data;
		SStrictSerializer::DeserializeStruct(
			data, DTexture2DAssetDataBitmap::GetDeclaredClass(), &bytes[byteProgress]
		);

		DArray<DBitmapImage>& mips = data.Mips;

		if (data.Mips.Length() > 0)
		{
			this->Width = mips[0].GetWidth();
			this->Height = mips[0].GetHeight();
		}

		int32 mipCount = mips.Length();
		for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
		{
			DBitmapImage& mip = mips[mip_i];
			glCheck(glTexImage2D(
				GL_TEXTURE_2D, mip_i, settings.sRGB? GL_SRGB_ALPHA : GL_RGBA, mip.GetWidth(), mip.GetHeight(), 0,
				GL_RGBA, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
			));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount));
		break;
	}
	case ETexture2DFormat::R16:
	{
		DTexture2DAssetDataBitmap16 data;
		SStrictSerializer::DeserializeStruct(
			data, DTexture2DAssetDataBitmap16::GetDeclaredClass(), &bytes[byteProgress]
		);
				
		DArray<DBitmapImage16>& mips = data.Mips;

		if (data.Mips.Length() > 0)
		{
			this->Width = mips[0].GetWidth();
			this->Height = mips[0].GetHeight();
		}

		int32 mipCount = mips.Length();
		for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
		{
			DBitmapImage16& mip = mips[mip_i];
			glCheck(glTexImage2D(
				GL_TEXTURE_2D, mip_i, GL_INTENSITY16, mip.GetWidth(), mip.GetHeight(), 0,
				GL_RED, GL_UNSIGNED_SHORT, mip.GetBuffer().Data()
			));
		}

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount));
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format in ") + this->Filepath);
	}
	}
}


void HTexture2D::Load(const DBitmapImage& ImageData, const DTexture2DSettings& Settings)
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

	// don't go higher than 8.0f, (it's an extension (not part of the core but should be available everywhere))
	glCheck(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, TEXTURE_ANISOTROPY_LEVEL));

	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));  
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	this->Width = ImageData.GetWidth();
	this->Height = ImageData.GetHeight();
	
	switch (Settings.Format)
	{
	case ETexture2DFormat::RGB8:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, Settings.sRGB? GL_SRGB : GL_RGB, this->Width, this->Height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, ImageData.GetBuffer().Data()
		));
		if (Settings.GenerateMipMaps)
		{
			glCheck(glGenerateMipmap(GL_TEXTURE_2D));
		}
		break;
	}
	case ETexture2DFormat::RGBA8:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, Settings.sRGB? GL_SRGB_ALPHA : GL_RGBA, this->Width, this->Height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, ImageData.GetBuffer().Data()
		));
		if (Settings.GenerateMipMaps)
		{
			glCheck(glGenerateMipmap(GL_TEXTURE_2D));
		}
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format"));
	}
	}
}


void HTexture2D::Load(const DBitmapImage16& ImageData, const DTexture2DSettings& Settings)
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

	// don't go higher than 8.0f, (it's an extension (not part of the core but should be available everywhere))
	glCheck(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, TEXTURE_ANISOTROPY_LEVEL));

	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));  
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	this->Width = ImageData.GetWidth();
	this->Height = ImageData.GetHeight();

	switch (Settings.Format)
	{
	case ETexture2DFormat::R16:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_INTENSITY16, this->Width, this->Height, 0,
			GL_RED, GL_UNSIGNED_SHORT, ImageData.GetBuffer().Data()
		));
		if (Settings.GenerateMipMaps)
		{
			glCheck(glGenerateMipmap(GL_TEXTURE_2D));
		}
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format"));
	}
	}
}


void HTexture2D::Load(const DBitmapImageHDR& ImageData, const DTexture2DSettings& Settings)
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
	
	// don't go higher than 8.0f, (it's an extension (not part of the core but should be available everywhere))
	glCheck(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, TEXTURE_ANISOTROPY_LEVEL));

	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));  
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	
	this->Width = ImageData.GetWidth();
	this->Height = ImageData.GetHeight();

	switch (Settings.Format)
	{
	case ETexture2DFormat::RGB16F:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, this->Width, this->Height, 0, 
			GL_RGB, GL_FLOAT, ImageData.GetBuffer().Data()
		));
		if (Settings.GenerateMipMaps)
		{
			glCheck(glGenerateMipmap(GL_TEXTURE_2D));
		}
		break;
	}
	case ETexture2DFormat::RGBA16F:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, this->Width, this->Height, 0, 
			GL_RGBA, GL_FLOAT, ImageData.GetBuffer().Data()
		));
		if (Settings.GenerateMipMaps)
		{
			glCheck(glGenerateMipmap(GL_TEXTURE_2D));
		}
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format"));
	}
	}
}


void HTexture2D::Bind()
{
	glCheck(glBindTexture(GL_TEXTURE_2D, this->TextureId));
}


void HTexture2D::BindNone()
{
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
}


int32 HTexture2D::GetWidth() const
{
	return this->Width;
}
int32 HTexture2D::GetHeight() const
{
	return this->Height;
}


REFLECT(DTexture2DAssetSettings)
FIELD(Format)
FIELD(sRGB)
FIELD(Wrap)
REFLECT_END()


REFLECT(DTexture2DAssetDataBitmap)
FIELD(Mips)
REFLECT_END()


REFLECT(DTexture2DAssetDataBitmap16)
FIELD(Mips)
REFLECT_END()
