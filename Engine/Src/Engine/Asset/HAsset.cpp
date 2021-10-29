#include "Engine/pch.h"
#include "HAsset.h"
#include "Engine\Util\SEngine.h"


DAssetHeader::DAssetHeader() :
	MajorVersion(0),
	MinorVersion(0),
	RevisionVersion(0),
	AssetType(EAssetType::TEXTURE_2D)
{
}


DAssetHeader::DAssetHeader(EAssetType InAssetType) :
	MajorVersion(SEngine::MajorVersion),
	MinorVersion(SEngine::MinorVersion),
	RevisionVersion(SEngine::RevisionVersion),
	AssetType(InAssetType)
{
}


const DString& HAsset::GetName() const
{
	return this->Name;
}
void HAsset::SetName(DStringView NewName)
{
	this->Name = DString::From(NewName);
}


const DString& HAsset::GetFilepath() const
{
	return this->Filepath;
}
void HAsset::SetFilepath(DStringView NewFilepath)
{
	this->Filepath = DString::From(NewFilepath);
}


REFLECT(DAssetHeader)
FIELD(MajorVersion)
FIELD(MinorVersion)
FIELD(RevisionVersion)
FIELD(AssetType)
REFLECT_END()
