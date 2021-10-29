#pragma once

#include "Engine\Render\Mesh\DStaticMeshData.h"
#include "Engine\Render\Shader\HSurfaceShader.h"
#include "Engine\Render\Mesh\HStaticMesh.h"
#include "Engine\Render\Scene\HSceneNode.h"
#include "Engine\Render\Texture\DBitmapImage.h"
#include "SGltf.h"


class SGltfImporter
{
public:

	struct DImportOptions
	{
		float ScaleToMeter = 1.0f;
		bool FlipNormalmapGreen = false;

		bool Equals(const DImportOptions& Other) const;
	};

	static bool ReadSceneFromGLTF(
		DStringView Filepath_gltf, 
		shared<HSurfaceShader> PBRMetallicShared, 
		shared<HSceneNode> ParentNode,
		const DImportOptions Options
	);
	static bool ReadSceneFromGLB(
		DStringView Filepath, 
		shared<HSurfaceShader> PBRMetallicShared, 
		shared<HSceneNode> ParentNode,
		const DImportOptions Options
	);

private:

	static bool ReadSceneFromData(
		const SGltf::DGltf_Gltf& Gltf, 
		const DArray<DArray<byte>>& Buffers,
		DArray<DBitmapImage>& Images,
		shared<HSurfaceShader> PBRMetallicShared, 
		shared<HSceneNode> ParentNode,
		DStringView Dirpath,
		DStringView MessagedFilepath,
		const DImportOptions Options
	);
	static bool ReadChunkHeader(DBinaryReader& Reader, DByteReconverter& Converter, uint32& OutChunkLength, uint32& OutChunkType);
	static void PopulateChildrenRecursive(
		shared<HSceneNode> Parent, const SGltf::DGltf_Node& GltfParent, 
		const DArray<shared<HSceneNode>>& StorageNodes, const DArray<SGltf::DGltf_Node>& GltfNodes,
		const DImportOptions& Options, bool NeedsScaling
	);
};
