#include "Engine\pch.h"
#include "Engine\Asset\HAssetFactory.h"
#include <filesystem>
#include "Engine\Render\Texture\DBitmapImage.h"
#include "Engine\Render\Texture\DBitmapImage16.h"
#include "Engine\Render\Texture\DBitmapImageHDR.h"
#include "Engine\Render\Texture\SImageCompressor.h"
#include "Engine\Render\Texture\HRenderTargetCube.h"
#include "Engine\Render\Texture\HTextureCube.h"
#include "Engine\Dependencies\STB\stb_image_write.h"
namespace fs = std::filesystem;


HAssetFactory::HAssetFactory()
{
	this->EqMapConverter = MakeShared<HEqMapConvert>();
	this->IBLConverter = MakeShared<HIBLConvert>();
	this->EqMapConverter->Load();
	this->IBLConverter->Load();
}


bool HAssetFactory::CreateTexture2D(DStringView AssetDir, DStringView AssetName)
{
	DString assetPathNoExt = AssetDir + AssetName;

	DString8 jstr;
	if (STextFile::ReadUTF8File(assetPathNoExt + TX(".import"), jstr) == false)
	{
		SLog::Error(TX("Can't load .import file of asset: ") + assetPathNoExt);
		return false;
	}
	
	DTextureImport importSettings;
	if (this->JSONParser.ParseStruct(importSettings, DTextureImport::GetDeclaredClass(), jstr) == false)
	{
		SLog::Error(TX("Can't parse .import file: ") + assetPathNoExt);
		return false;
	}

	DAssetHeader assetHeader (EAssetType::TEXTURE_2D);

	DTexture2DAssetSettings assetSettings;
	assetSettings.Format = importSettings.Format;
	assetSettings.sRGB = importSettings.sRGB;

	int32 headerPlusSettingsByteCount = 
		  SStrictSerializer::GetSerializedByteCountOfStruct(assetHeader, DAssetHeader::GetDeclaredClass())
		+ SStrictSerializer::GetSerializedByteCountOfStruct(assetSettings, DTextureCubeAssetSettings::GetDeclaredClass());

	switch (importSettings.Format)
	{
	case ETexture2DFormat::BC1:
	case ETexture2DFormat::BC3:
	case ETexture2DFormat::BC4:
	case ETexture2DFormat::BC5:
	{
		DTexture2DAssetDataBitmap asset;

		DBitmapImage image;
		image.Load(importSettings.Source, 4);

		if (importSettings.InvertGreen)
		{
			image.InvertChannel(1);
		}

		int32 mipCount = DMath::RoundToInt(DMath::LogN(2, DMath::Max(image.GetWidth(), image.GetHeight())));

		asset.Mips.Reserve(mipCount);

		for (int32 mip_i = 0; mip_i < mipCount + 1; mip_i++)
		{
			int32 w = DMath::Max(1, image.GetWidth() / (int32)(DMath::Pow(2, mip_i)));
			int32 h = DMath::Max(1, image.GetHeight() / (int32)(DMath::Pow(2, mip_i)));

			int32 cw = DMath::Max(4, w);
			int32 ch = DMath::Max(4, h);

			DBitmapImage resizedImage;
			image.Resize(cw, ch, importSettings.sRGB, /*out*/resizedImage);

			DBitmapImage& mip = asset.Mips.AddDefault();
			
			switch (importSettings.Format)
			{
			case ETexture2DFormat::BC1:
			{
				SImageCompressor::CompressToBC1(resizedImage, mip);
				break;
			}
			case ETexture2DFormat::BC3:
			{
				SImageCompressor::CompressToBC3(resizedImage, mip);
				break;
			}
			case ETexture2DFormat::BC4:
			{
				SImageCompressor::CompressToBC4(resizedImage, mip);
				break;
			}
			case ETexture2DFormat::BC5:
			{
				SImageCompressor::CompressToBC5(resizedImage, mip);
				break;
			}
			}

			mip.OverwriteSize(w, h);
		}

		int32 assetByteCount = 
			headerPlusSettingsByteCount
			+ SStrictSerializer::GetSerializedByteCountOfStruct(asset, DTexture2DAssetDataBitmap::GetDeclaredClass());

		DArray<byte> assetBytes (assetByteCount, true);  // uninitialized array
		
		int32 byteProgress = 0;
		byteProgress += SStrictSerializer::SerializeStruct(
			assetHeader,
			DAssetHeader::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		byteProgress += SStrictSerializer::SerializeStruct(
			assetSettings,
			DTexture2DAssetSettings::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SStrictSerializer::SerializeStruct(
			asset,
			DTexture2DAssetDataBitmap::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SBinaryFile::WriteToFile(assetPathNoExt + TX(".asset"), assetBytes);
		break;
	}
	case ETexture2DFormat::RGB8:
	case ETexture2DFormat::RGBA8:
	{
		DTexture2DAssetDataBitmap asset;

		DBitmapImage& image = asset.Mips.AddDefault();
		switch (importSettings.Format)
		{
		case ETexture2DFormat::RGB8:
		{
			image.Load(importSettings.Source, 3);
			break;
		}
		case ETexture2DFormat::RGBA8:
		{
			image.Load(importSettings.Source, 4);
			break;
		}
		}

		int32 mipCount = DMath::RoundToInt(DMath::LogN(2, DMath::Max(image.GetWidth(), image.GetHeight())));

		asset.Mips.Reserve(mipCount);

		for (int32 mip_i = 1; mip_i < mipCount + 1; mip_i++)
		{
			int32 w = DMath::Max(1, image.GetWidth() / (int32)(DMath::Pow(2, mip_i)));
			int32 h = DMath::Max(1, image.GetHeight() / (int32)(DMath::Pow(2, mip_i)));
			
			DBitmapImage& mip = asset.Mips.AddDefault();
			image.Resize(w, h, importSettings.sRGB, /*out*/mip);
		}

		int32 assetByteCount =   
			  headerPlusSettingsByteCount
			+ SStrictSerializer::GetSerializedByteCountOfStruct(asset, DTexture2DAssetDataBitmap::GetDeclaredClass());

		DArray<byte> assetBytes (assetByteCount, true);  // uninitialized array

		int32 byteProgress = 0;
		SStrictSerializer::SerializeStruct(
			assetHeader,
			DAssetHeader::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SStrictSerializer::SerializeStruct(
			assetSettings,
			DTexture2DAssetSettings::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SStrictSerializer::SerializeStruct(
			asset,
			DTexture2DAssetDataBitmap::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SBinaryFile::WriteToFile(assetPathNoExt + TX(".asset"), assetBytes);
		break;
	}
	case ETexture2DFormat::R16:
	{
		DTexture2DAssetDataBitmap16 asset;

		DBitmapImage16& image = asset.Mips.AddDefault();
		image.Load(importSettings.Source, 3);

		int32 mipCount = DMath::RoundToInt(DMath::LogN(2, DMath::Max(image.GetWidth(), image.GetHeight())));

		asset.Mips.Reserve(mipCount);

		for (int32 mip_i = 1; mip_i < mipCount + 1; mip_i++)
		{
			int32 w = DMath::Max(1, image.GetWidth() / (int32)(DMath::Pow(2, mip_i)));
			int32 h = DMath::Max(1, image.GetHeight() / (int32)(DMath::Pow(2, mip_i)));

			DBitmapImage16& mip = asset.Mips.AddDefault();
			image.Resize(w, h, false, /*out*/mip);
		}

		int32 assetByteCount = 
			  headerPlusSettingsByteCount
			+ SStrictSerializer::GetSerializedByteCountOfStruct(asset, DTexture2DAssetDataBitmap16::GetDeclaredClass());

		DArray<byte> assetBytes (assetByteCount, true);  // uninitialized array

		int32 byteProgress = 0;
		SStrictSerializer::SerializeStruct(
			assetHeader,
			DAssetHeader::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SStrictSerializer::SerializeStruct(
			assetSettings,
			DTexture2DAssetSettings::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SStrictSerializer::SerializeStruct(
			asset,
			DTexture2DAssetDataBitmap16::GetDeclaredClass(),
			&assetBytes[byteProgress]
		);
		SBinaryFile::WriteToFile(assetPathNoExt + TX(".asset"), assetBytes);
		break;
	}
	default:
	{
		SLog::Error(TX("Unsupported format for Texture2D asset file: ") + assetPathNoExt);
		return false;
	}
	}

	return true;
}


bool HAssetFactory::CreateIBLMaps(DStringView AssetDir, DStringView AssetName)
{
	DAssetHeader assetHeader (EAssetType::TEXTURE_CUBE);

	DTextureCubeAssetSettings assetSettings;
	assetSettings.Format = ETextureCubeFormat::RGB16F;
	assetSettings.sRGB = false;

	DString assetPathNoExt = AssetDir + AssetName;

	DString8 jstr;
	if (STextFile::ReadUTF8File(assetPathNoExt + TX(".import"), jstr) == false)
	{
		SLog::Error(TX("Can't load .import file of asset: ") + assetPathNoExt);
		return false;
	}

	DTextureImport importSettings;
	if (this->JSONParser.ParseStruct(importSettings, DTextureImport::GetDeclaredClass(), jstr) == false)
	{
		SLog::Error(TX("Can't parse .import file of asset: ") + assetPathNoExt);
		return false;
	}

	DBitmapImageHDR eqImage;
	if (eqImage.Load(importSettings.Source, 3, 1.0f) == false) return false;
	DTexture2DSettings settings;
	settings.Format = ETexture2DFormat::RGB16F;

	shared<HTexture2D> eqMap = MakeShared<HTexture2D>();
	eqMap->Load(eqImage, settings);

	DArray<shared<HTexture2D>> eqMaps (1);
	eqMaps.Add(eqMap);

	DArray<shared<HRenderTargetCube>> cubemaps;
	this->EqMapConverter->EquirectangularToCubemap(eqMaps, 128, ERenderTargetCubeFormat::RGB16F, /*out*/cubemaps);

	DArray<shared<HRenderTargetCube>> irradianceMaps;
	DArray<shared<HRenderTargetCube>> reflectionMaps;
	this->IBLConverter->GenerateLightmaps(cubemaps, 32, 128, /*out*/irradianceMaps, /*out*/reflectionMaps);

	DTextureCubeAssetDataHDR irrAsset;
	irrAsset.Mips_PosX.Resize(1);
	irrAsset.Mips_NegX.Resize(1);
	irrAsset.Mips_PosY.Resize(1);
	irrAsset.Mips_NegY.Resize(1);
	irrAsset.Mips_PosZ.Resize(1);
	irrAsset.Mips_NegZ.Resize(1);

	irradianceMaps[0]->ToImage(
		/*out*/irrAsset.Mips_PosX[0],
		/*out*/irrAsset.Mips_NegX[0],
		/*out*/irrAsset.Mips_PosY[0],
		/*out*/irrAsset.Mips_NegY[0],
		/*out*/irrAsset.Mips_PosZ[0],
		/*out*/irrAsset.Mips_NegZ[0]
	);

	DTextureCubeAssetDataHDR refAsset;
	
	reflectionMaps[0]->ToImageMips(
		5,
		/*out*/refAsset.Mips_PosX,
		/*out*/refAsset.Mips_NegX,
		/*out*/refAsset.Mips_PosY,
		/*out*/refAsset.Mips_NegY,
		/*out*/refAsset.Mips_PosZ,
		/*out*/refAsset.Mips_NegZ
	);

	int32 headerPlusSettingsByteCount = 
		  SStrictSerializer::GetSerializedByteCountOfStruct(assetHeader, DAssetHeader::GetDeclaredClass())
		+ SStrictSerializer::GetSerializedByteCountOfStruct(assetSettings, DTextureCubeAssetSettings::GetDeclaredClass());

	int32 byteProgress;

	// write irr asset
	int32 irrAssetByteCount = 
		  headerPlusSettingsByteCount
		+ SStrictSerializer::GetSerializedByteCountOfStruct(irrAsset, DTextureCubeAssetDataHDR::GetDeclaredClass());

	DArray<byte> irrAssetBytes (irrAssetByteCount, true);  // uninitialized array

	byteProgress = 0;
	byteProgress += SStrictSerializer::SerializeStruct(
		assetHeader,
		DAssetHeader::GetDeclaredClass(),
		&irrAssetBytes[0]
	);
	byteProgress += SStrictSerializer::SerializeStruct(
		assetSettings,
		DTexture2DAssetSettings::GetDeclaredClass(),
		&irrAssetBytes[byteProgress]
	);
	SStrictSerializer::SerializeStruct(
		irrAsset,
		DTextureCubeAssetDataHDR::GetDeclaredClass(),
		&irrAssetBytes[byteProgress]
	);
	SBinaryFile::WriteToFile(assetPathNoExt + TX("_Irradiance.asset"), irrAssetBytes);

	// write ref asset
	int32 refAssetByteCount = 
		  headerPlusSettingsByteCount
		+ SStrictSerializer::GetSerializedByteCountOfStruct(refAsset, DTextureCubeAssetDataHDR::GetDeclaredClass());

	DArray<byte> refAssetBytes (refAssetByteCount, true);  // uninitialized array

	byteProgress = 0;
	byteProgress += SStrictSerializer::SerializeStruct(
		assetHeader,
		DAssetHeader::GetDeclaredClass(),
		&refAssetBytes[byteProgress]
	);
	byteProgress += SStrictSerializer::SerializeStruct(
		assetSettings,
		DTexture2DAssetSettings::GetDeclaredClass(),
		&refAssetBytes[byteProgress]
	);
	SStrictSerializer::SerializeStruct(
		refAsset,
		DTextureCubeAssetDataHDR::GetDeclaredClass(),
		&refAssetBytes[byteProgress]
	);
	SBinaryFile::WriteToFile(assetPathNoExt + TX("_Reflection.asset"), refAssetBytes);

	return true;
}


bool HAssetFactory::SaveStaticMeshAsset(const DStaticMeshData& MeshData, DStringView Filepath)
{
	DAssetHeader assetHeader (EAssetType::STATIC_MESH);

	DStaticMeshAssetData asset;
	MeshData.ToStaticMeshAssetData(asset);

	int32 byteCount =   
		  SStrictSerializer::GetSerializedByteCountOfStruct(assetHeader, DAssetHeader::GetDeclaredClass())
		+ SStrictSerializer::GetSerializedByteCountOfStruct(asset, DStaticMeshAssetData::GetDeclaredClass());

	DArray<byte> assetBytes (byteCount, true);  // uninitialized array
	
	int32 byteProgress = 0;
	byteProgress += SStrictSerializer::SerializeStruct(
		assetHeader,
		DAssetHeader::GetDeclaredClass(),
		&assetBytes[byteProgress]
	);

	SStrictSerializer::SerializeStruct(
		asset,
		DStaticMeshAssetData::GetDeclaredClass(),
		&assetBytes[byteProgress]
	);

	return SBinaryFile::WriteToFile(Filepath, assetBytes);
}


bool HAssetFactory::CreateStaticMesh(DStringView AssetDir, DStringView AssetName)
{
	DString assetPathNoExt = AssetDir + AssetName;

	DString8 jstr;
	if (STextFile::ReadUTF8File(assetPathNoExt + TX(".import"), jstr) == false)
	{
		SLog::Error(TX("Can't load .import file of asset: ") + assetPathNoExt);
		return false;
	}

	DStaticMeshImport importSettings;
	if (this->JSONParser.ParseStruct(importSettings, DStaticMeshImport::GetDeclaredClass(), jstr) == false)
	{
		SLog::Error(TX("Can't parse .import file of asset: ") + assetPathNoExt);
		return false;
	}
	
	if (DString::EndsWith(importSettings.Source, TX(".obj")))
	{
		DString8 objStr;
		if (STextFile::ReadAsciiFile(importSettings.Source, objStr) == false)
		{
			return false;
		}

		DArray<DStaticMeshData> meshDataItems;
		DWavefrontParser wfParser;
		if (wfParser.ParseMeshData(objStr, {}, meshDataItems) == false) return false;

		if (meshDataItems.IsEmpty()) return false;

		DStaticMeshData& meshData = meshDataItems[0];

		if (meshData.DetectMirroredUVs())
		{
			SLog::Message(TX("Mirorred UVs found in ") + assetPathNoExt);
		}

		SaveStaticMeshAsset(meshData, assetPathNoExt + TX(".asset"));
	}
	else
	{
		SLog::Error(u"Unsupported file format: " + importSettings.Source);
	}
	
	return true;
}


REFLECT(DTextureImport)
FIELD(Source)
FIELD(Format)
FIELD(sRGB)
FIELD(InvertGreen)
REFLECT_END()


REFLECT(DStaticMeshImport)
FIELD(Source)
REFLECT_END()
