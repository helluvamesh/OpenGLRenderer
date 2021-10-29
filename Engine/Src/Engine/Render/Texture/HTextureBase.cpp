#include "Engine/pch.h"
#include "HTextureBase.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HTextureBase::HTextureBase(ETextureType InType) : 
	Type(InType)
{
}


HTextureBase::~HTextureBase()
{
	this->FreeGPU();
}


void HTextureBase::FreeGPU()
{
	if (this->TextureId)
	{
		glCheck(glDeleteTextures(1, &this->TextureId));
	}
	this->TextureId = 0;
}


bool HTextureBase::IsLoaded()
{
	return this->TextureId != 0;
}


void HTextureBase::Unload()
{
	this->FreeGPU();
}


uint32 HTextureBase::GetTextureId() const
{
	return this->TextureId;
}


void HTextureBase::BindNoneStatic()
{
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
	glCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	glCheck(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
	glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
}


HTexture2DBase::HTexture2DBase(ETextureType InType) :
	HTextureBase(InType)
{
}


HTextureCubeBase::HTextureCubeBase(ETextureType InType) :
	HTextureBase(InType)
{
}
