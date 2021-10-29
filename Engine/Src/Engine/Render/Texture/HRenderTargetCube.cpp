#include "Engine/pch.h"
#include "HRenderTargetCube.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


const DFixedArray<FMat4, 6> HRenderTargetCube::CubeSideViews {

	// left
	{
		5.96047e-08f, 0, 1, 0,
		0, 1, 0, 0,
		-1, 0, 5.96047e-08f, 0,
		0, 0, 0, 1
	},
	// right
	{
		5.96047e-08f, 0, -1, 0,
		0, 1, 0, 0,
		1, 0, 5.96047e-08f, 0,
		0, 0, 0, 1
	},
	// top
	{
		1, 0, 0, 0,
		0, 5.96047e-08f, -1, 0,
		0, 1, 5.96047e-08f, 0,
		0, 0, 0, 1
	},
	// bottom
	{
		1, 0, 0, 0,
		0, 5.96047e-08f, 1, 0,
		0, -1, 5.96047e-08f, 0,
		0, 0, 0, 1
	},
	// back
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	},
	// front
	{
		-1, 0, 8.74228e-08f, 0,
		0, 1, 0, 0,
		-8.74228e-08f, 0, -1, 0,
		0, 0, 0, 1
	}
};


HRenderTargetCube::HRenderTargetCube() : 
	HTextureCubeBase(ETextureType::RENDER_TARGET_CUBE)
{
}


void HRenderTargetCube::Load()
{
	this->Unload();

	// TODO
	// Load settings from .asset file, then allocate GPU buffers
}


void HRenderTargetCube::Load(const DRenderTargetCubeSettings& Settings)
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
	if (Settings.Wrap == ETextureWrap::CLAMP_TO_BORDER)
		glCheck(glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &Settings.BorderColor.X)); 
	glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	this->Width = Settings.Width;
	this->Height = Settings.Height;
	this->Format = Settings.Format;

	switch (Settings.Format)
	{
	case ERenderTargetCubeFormat::RGB8:
	{
		for (int32 i = 0; i < 6; i++)
		{
			glCheck(glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, Settings.Width, Settings.Height, 0, 
				GL_RGB, GL_UNSIGNED_BYTE, nullptr
			));
		}

		if (Settings.FixedMipCount > 0)
		{
			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));

			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0));
			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, Settings.FixedMipCount));

			for (int32 side_i = 0; side_i < 6; side_i++)
			{
				for (int32 mip_i = 1; mip_i < Settings.FixedMipCount + 1; mip_i++)
				{
					int32 divider = (int32)DMath::Pow(2, mip_i);
					glCheck(glTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + side_i, mip_i, GL_RGB8, 
						Settings.Width / divider, Settings.Height / divider, 
						0, GL_RGB, GL_UNSIGNED_BYTE, nullptr
					));
				}
			}
		}
		else
		{
			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Settings.GenerateMipMaps? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
			if (Settings.GenerateMipMaps)
			{
				glCheck(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
			}
		}
		break;
	}
	case ERenderTargetCubeFormat::RGB16F:
	{
		for (int32 i = 0; i < 6; i++)
		{
			glCheck(glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, this->Width, this->Height, 0, 
				GL_RGB, GL_FLOAT, nullptr
			));
		}

		if (Settings.FixedMipCount > 0)
		{
			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));

			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0));
			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, Settings.FixedMipCount));
			
			for (int32 side_i = 0; side_i < 6; side_i++)
			{
				for (int32 mip_i = 1; mip_i < Settings.FixedMipCount + 1; mip_i++)
				{
					int32 divider = (int32)DMath::Pow(2, mip_i);
					glCheck(glTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + side_i, mip_i, GL_RGB16F, 
						Settings.Width / divider, Settings.Height / divider, 
						0, GL_RGB, GL_FLOAT, nullptr
					));
				}
			}
		}
		else
		{
			glCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Settings.GenerateMipMaps? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
			if (Settings.GenerateMipMaps)
			{
				glCheck(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
			}
		}
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported texture format in ") + this->Name);
	}
	}
}


void HRenderTargetCube::Bind()
{
	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));
}


void HRenderTargetCube::BindNone()
{
	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}


void HRenderTargetCube::ToImage(
	DBitmapImageHDR& OutImage_PosX,
	DBitmapImageHDR& OutImage_NegX,
	DBitmapImageHDR& OutImage_PosY,
	DBitmapImageHDR& OutImage_NegY,
	DBitmapImageHDR& OutImage_PosZ,
	DBitmapImageHDR& OutImage_NegZ
) const
{
	switch (this->Format)
	{
	case ERenderTargetCubeFormat::RGB16F:
	{
		glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));

		{
			int32 len = this->Width * this->Height * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutImage_PosX = DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutImage_NegX = DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutImage_PosY = DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutImage_NegY = DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutImage_PosZ = DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutImage_NegZ = DBitmapImageHDR(std::move(buffer), this->Width, this->Height, 3);
		}
		break;
	}
	default:
	{
		SLog::Error(TX("Wrong image format when writing pixels from RenderTargetCube to image: \"") + this->Name + TX("\""));
	}
	}
}


void HRenderTargetCube::ToImage(
	DBitmapImage& OutImage_PosX,
	DBitmapImage& OutImage_NegX,
	DBitmapImage& OutImage_PosY,
	DBitmapImage& OutImage_NegY,
	DBitmapImage& OutImage_PosZ,
	DBitmapImage& OutImage_NegZ
) const
{
	switch (this->Format)
	{
	case ERenderTargetCubeFormat::RGB8:
	{
		glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));

		{
			int32 len = this->Width * this->Height * 3;
			DArray<uint8, DBitmapImageAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
			));
			OutImage_PosX = DBitmapImage(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<uint8, DBitmapImageAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
			));
			OutImage_NegX = DBitmapImage(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<uint8, DBitmapImageAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
			));
			OutImage_PosY = DBitmapImage(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<uint8, DBitmapImageAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
			));
			OutImage_NegY = DBitmapImage(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<uint8, DBitmapImageAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
			));
			OutImage_PosZ = DBitmapImage(std::move(buffer), this->Width, this->Height, 3);
		}
		{
			int32 len = this->Width * this->Height * 3;
			DArray<uint8, DBitmapImageAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.GetDataUnsafe()
			));
			OutImage_NegZ = DBitmapImage(std::move(buffer), this->Width, this->Height, 3);
		}
		break;
	}
	default:
	{
		SLog::Error(TX("Wrong image format when writing pixels from RenderTargetCube to image: \"") + this->Name + TX("\""));
	}
	}
}


void HRenderTargetCube::ToImageMips(
	int32 MipCount,
	DArray<DBitmapImageHDR>& OutMips_PosX,
	DArray<DBitmapImageHDR>& OutMips_NegX,
	DArray<DBitmapImageHDR>& OutMips_PosY,
	DArray<DBitmapImageHDR>& OutMips_NegY,
	DArray<DBitmapImageHDR>& OutMips_PosZ,
	DArray<DBitmapImageHDR>& OutMips_NegZ
) const
{
	switch (this->Format)
	{
	case ERenderTargetCubeFormat::RGB16F:
	{
		glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureId));

		OutMips_PosX.Reserve(MipCount);
		OutMips_NegX.Reserve(MipCount);
		OutMips_PosY.Reserve(MipCount);
		OutMips_NegY.Reserve(MipCount);
		OutMips_PosZ.Reserve(MipCount);
		OutMips_NegZ.Reserve(MipCount);
		
		for (int32 mip_i = 0; mip_i < MipCount; mip_i++)
		{
			int32 w = this->Width / (int32)DMath::Pow(2, mip_i);
			int32 h = this->Height / (int32)DMath::Pow(2, mip_i);
			int32 len = w * h * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X, mip_i, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutMips_PosX.Add(DBitmapImageHDR(std::move(buffer), w, h, 3));
		}

		for (int32 mip_i = 0; mip_i < MipCount; mip_i++)
		{
			int32 w = this->Width / (int32)DMath::Pow(2, mip_i);
			int32 h = this->Height / (int32)DMath::Pow(2, mip_i);
			int32 len = w * h * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mip_i, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutMips_NegX.Add(DBitmapImageHDR(std::move(buffer), w, h, 3));
		}

		for (int32 mip_i = 0; mip_i < MipCount; mip_i++)
		{
			int32 w = this->Width / (int32)DMath::Pow(2, mip_i);
			int32 h = this->Height / (int32)DMath::Pow(2, mip_i);
			int32 len = w * h * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mip_i, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutMips_PosY.Add(DBitmapImageHDR(std::move(buffer), w, h, 3));
		}

		for (int32 mip_i = 0; mip_i < MipCount; mip_i++)
		{
			int32 w = this->Width / (int32)DMath::Pow(2, mip_i);
			int32 h = this->Height / (int32)DMath::Pow(2, mip_i);
			int32 len = w * h * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mip_i, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutMips_NegY.Add(DBitmapImageHDR(std::move(buffer), w, h, 3));
		}

		for (int32 mip_i = 0; mip_i < MipCount; mip_i++)
		{
			int32 w = this->Width / (int32)DMath::Pow(2, mip_i);
			int32 h = this->Height / (int32)DMath::Pow(2, mip_i);
			int32 len = w * h * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mip_i, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutMips_PosZ.Add(DBitmapImageHDR(std::move(buffer), w, h, 3));
		}

		for (int32 mip_i = 0; mip_i < MipCount; mip_i++)
		{
			int32 w = this->Width / (int32)DMath::Pow(2, mip_i);
			int32 h = this->Height / (int32)DMath::Pow(2, mip_i);
			int32 len = w * h * 3;
			DArray<float, DBitmapImageHDRAllocator> buffer (len, true);
			glCheck(glGetTexImage(
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mip_i, GL_RGB, GL_FLOAT, buffer.GetDataUnsafe()
			));
			OutMips_NegZ.Add(DBitmapImageHDR(std::move(buffer), w, h, 3));
		}

		break;
	}
	default:
	{
		SLog::Error(TX("Wrong image format when writing pixels from RenderTargetCube to image: \"") + this->Name + TX("\""));
	}
	}
}


int32 HRenderTargetCube::GetWidth() const
{
	return this->Width;
}
int32 HRenderTargetCube::GetHeight() const
{
	return this->Height;
}
ERenderTargetCubeFormat HRenderTargetCube::GetFormat() const
{
	return this->Format;
}
