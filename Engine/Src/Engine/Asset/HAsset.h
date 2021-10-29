/*
* Assets can be loaded from raw source media files or from .asset files.
* .asset files must contain data ready to be used without any heavy conversion for fast loading times. 
* For an .asset file to be created or recreated must exist an .import file
* that contains an absolute path to the source file and settings relating to asset creation.
* An .asset file consist of three structs:
* - AssetHeader
* - {AssetClassName}AssetSettings[Suffix]
* - {AssetClassName}AssetData[Suffix]
* Loading from an .asset file must not require any knowledge about the state of an asset, 
* the state of an asset must be 100% loaded from the .asset file (except for Filepath).
* void {SomeClass extends HAsset} ::Load() method should always load an asset from a .asset file
* any public Load function must call Unload() as the first statement
*/



#pragma once


DECLARE_ENUM(
	EAssetType,
	TEXTURE_2D,
	TEXTURE_CUBE,
	STATIC_MESH
)


struct DAssetHeader : public DStruct
{
	REFLECT_H(DAssetHeader, DStruct)
public:
	int32 MajorVersion;
	int32 MinorVersion;
	int32 RevisionVersion;
	EAssetType AssetType;

	DAssetHeader();
	DAssetHeader(EAssetType InAssetType);
};


class HAsset : public HObject
{
protected:
	DString Name;
	DString Filepath;

public:
	virtual void Load() = 0;
	virtual bool IsLoaded() = 0;
	virtual void Unload() = 0;
	
	const DString& GetName() const;
	void SetName(DStringView NewName);

	const DString& GetFilepath() const;
	void SetFilepath(DStringView NewFilepath);

};