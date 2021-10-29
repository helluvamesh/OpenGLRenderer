#include "Engine/pch.h"
#include "HTextureCube.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HTextureCube::HTextureCube() :
	HTextureCubeBase(ETextureType::TEXTURE_CUBE)
{
}


void HTextureCube::Load()
{
	this->Unload();

	glCheck(glGenTextures(1, &this->TextureId));
	
	DArray<byte> bytes;
	if (SBinaryFile::ReadFile(this->Filepath, /*out*/bytes) == false) return;

	int32 byteProgress = 0;

	DAssetHeader header;
	byteProgress += SStrictSerializer::DeserializeStruct(
		header, DAssetHeader::GetDeclaredClass(), &bytes[byteProgress]
	);

	if (header.AssetType != EAssetType::TEXTURE_CUBE)
	{
		SLog::Error(TX("Wrong asset type in ") + this->Filepath);
		return;
	}

	DTextureCubeAssetSettings settings;
	byteProgress += SStrictSerializer::DeserializeStruct(
		settings, DTextureCubeAssetSettings::GetDeclaredClass(), &bytes[byteProgress]
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

	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));


	switch (settings.Format)
	{
	case ETextureCubeFormat::RGB8:
	{
		DTextureCubeAssetDataBitmap data;
		SStrictSerializer::DeserializeStruct(
			data, DTextureCubeAssetDataHDR::GetDeclaredClass(), &bytes[byteProgress]
		);

		if (data.Mips_PosX.Length() > 0)
		{
			this->Width = data.Mips_PosX[0].GetWidth();
			this->Height = data.Mips_PosX[0].GetHeight();
		}
		int32 mips = data.Mips_PosX.Length();

		glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0));
		glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mips - 1));
		glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mips > 1? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
			
		{
			DArray<DBitmapImage>& mips = data.Mips_PosX;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImage& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X, mip_i, settings.sRGB? GL_SRGB8 : GL_RGB8, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImage>& mips = data.Mips_NegX;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImage& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mip_i, settings.sRGB? GL_SRGB8 : GL_RGB8, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImage>& mips = data.Mips_PosY;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImage& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mip_i, settings.sRGB? GL_SRGB8 : GL_RGB8, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImage>& mips = data.Mips_NegY;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImage& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mip_i, settings.sRGB? GL_SRGB8 : GL_RGB8, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImage>& mips = data.Mips_PosZ;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImage& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mip_i, settings.sRGB? GL_SRGB8 : GL_RGB8, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImage>& mips = data.Mips_NegZ;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImage& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mip_i, settings.sRGB? GL_SRGB8 : GL_RGB8, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_UNSIGNED_BYTE, mip.GetBuffer().Data()
				));
			}
		}
		break;
	}
	case ETextureCubeFormat::RGB16F:
	{
		DTextureCubeAssetDataHDR data;
		SStrictSerializer::DeserializeStruct(
			data, DTextureCubeAssetDataHDR::GetDeclaredClass(), &bytes[byteProgress]
		);

		if (data.Mips_PosX.Length() > 0)
		{
			this->Width = data.Mips_PosX[0].GetWidth();
			this->Height = data.Mips_PosX[0].GetHeight();
		}
		int32 mips = data.Mips_PosX.Length();

		glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0));
		glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mips - 1));
		glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mips > 1? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));

		{
			DArray<DBitmapImageHDR>& mips = data.Mips_PosX;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImageHDR& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X, mip_i, GL_RGB16F, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_FLOAT, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImageHDR>& mips = data.Mips_NegX;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImageHDR& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mip_i, GL_RGB16F, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_FLOAT, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImageHDR>& mips = data.Mips_PosY;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImageHDR& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mip_i, GL_RGB16F, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_FLOAT, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImageHDR>& mips = data.Mips_NegY;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImageHDR& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mip_i, GL_RGB16F, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_FLOAT, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImageHDR>& mips = data.Mips_PosZ;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImageHDR& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mip_i, GL_RGB16F, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_FLOAT, mip.GetBuffer().Data()
				));
			}
		}
		{
			DArray<DBitmapImageHDR>& mips = data.Mips_NegZ;
			int32 mipCount = mips.Length();
			for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
			{
				DBitmapImageHDR& mip = mips[mip_i];
				glCheck(glTexImage2D(
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mip_i, GL_RGB16F, mip.GetWidth(), mip.GetHeight(), 
					0, GL_RGB, GL_FLOAT, mip.GetBuffer().Data()
				));
			}
		}
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format in ") + this->Filepath);
	}
	}
}


void HTextureCube::Load(
	const DBitmapImage& Image_PosX,
	const DBitmapImage& Image_NegX,
	const DBitmapImage& Image_PosY,
	const DBitmapImage& Image_NegY,
	const DBitmapImage& Image_PosZ,
	const DBitmapImage& Image_NegZ,
	const DTextureCubeSettings& Settings
)
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

	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	
	this->Width = Image_PosX.GetWidth();
	this->Height = Image_PosX.GetHeight();

	switch (Settings.Format)
	{
	case ETextureCubeFormat::RGB8:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, Settings.sRGB? GL_SRGB8 : GL_RGB8, this->Width, this->Height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, Image_PosX.GetBuffer().Data()
		));

		this->Width = Image_PosX.GetWidth();
		this->Height = Image_PosX.GetHeight();

		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, Settings.sRGB? GL_SRGB8 : GL_RGB8, this->Width, this->Height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, Image_NegX.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, Settings.sRGB? GL_SRGB8 : GL_RGB8, this->Width, this->Height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, Image_PosY.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, Settings.sRGB? GL_SRGB8 : GL_RGB8, this->Width, this->Height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, Image_NegY.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, Settings.sRGB? GL_SRGB8 : GL_RGB8, this->Width, this->Height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, Image_PosZ.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, Settings.sRGB? GL_SRGB8 : GL_RGB8, this->Width, this->Height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, Image_NegZ.GetBuffer().Data()
		));

		if (Settings.GenerateMipMaps)
		{
			glCheck(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
		}
		break;
	}
	}
}

void HTextureCube::Load(
	const DBitmapImageHDR& Image_PosX,
	const DBitmapImageHDR& Image_NegX,
	const DBitmapImageHDR& Image_PosY,
	const DBitmapImageHDR& Image_NegY,
	const DBitmapImageHDR& Image_PosZ,
	const DBitmapImageHDR& Image_NegZ,
	const DTextureCubeSettings& Settings
)
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

	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, glWrap));
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	
	switch (Settings.Format)
	{
	case ETextureCubeFormat::RGB16F:
	{
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB16F, this->Width, this->Height, 
			0, GL_RGB, GL_FLOAT, Image_PosX.GetBuffer().Data()
		));

		this->Width = Image_PosX.GetWidth();
		this->Height = Image_PosX.GetHeight();

		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB16F, this->Width, this->Height, 
			0, GL_RGB, GL_FLOAT, Image_NegX.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB16F, this->Width, this->Height, 
			0, GL_RGB, GL_FLOAT, Image_PosY.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB16F, this->Width, this->Height, 
			0, GL_RGB, GL_FLOAT, Image_NegY.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB16F, this->Width, this->Height, 
			0, GL_RGB, GL_FLOAT, Image_PosZ.GetBuffer().Data()
		));
		glCheck(glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB16F, this->Width, this->Height, 
			0, GL_RGB, GL_FLOAT, Image_NegZ.GetBuffer().Data()
		));

		if (Settings.GenerateMipMaps)
		{
			glCheck(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
		}
		break;
	}
	}
}


void HTextureCube::Bind()
{
	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));
}


void HTextureCube::BindNone()
{
	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}


int32 HTextureCube::GetWidth() const
{
	return this->Width;
}
int32 HTextureCube::GetHeight() const
{
	return this->Height;
}


REFLECT(DTextureCubeAssetSettings)
FIELD(Format)
FIELD(sRGB)
FIELD(Wrap)
REFLECT_END()


REFLECT(DTextureCubeAssetDataBitmap)
FIELD(Mips_PosX)
FIELD(Mips_NegX)
FIELD(Mips_PosY)
FIELD(Mips_NegY)
FIELD(Mips_PosZ)
FIELD(Mips_NegZ)
REFLECT_END()


REFLECT(DTextureCubeAssetDataHDR)
FIELD(Mips_PosX)
FIELD(Mips_NegX)
FIELD(Mips_PosY)
FIELD(Mips_NegY)
FIELD(Mips_PosZ)
FIELD(Mips_NegZ)
REFLECT_END()