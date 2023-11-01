#include "Engine\pch.h"
#include "SGltfImporter.h"
#include "Engine\Render\Shader\HSurfaceMaterial.h"
#include "Engine\Render\Texture\HTexture2D.h"
#include "Engine\Render\Scene\HStaticMeshNode.h"
using namespace SGltf;

// default value of ids/references should be -1, because using structs every field is always present
// the layout of the 'metallicRoughness' texture is 'occlusion/roughness/metallic'


bool SGltfImporter::DImportOptions::Equals(const DImportOptions& Other) const
{
	return 
		this->ScaleToMeter == Other.ScaleToMeter;
}


bool SGltfImporter::ReadSceneFromGLTF(
	DStringView Filepath_gltf,
	shared<HSurfaceShader> PBRMetallicShared,
	shared<HSceneNode> ParentNode,
	const DImportOptions Options 
)
{
	// ------------------- json (.gltf) -----------------------

	DString8 jsonString;
	if (STextFile::ReadUTF8File(Filepath_gltf, jsonString) == false) return false;

	DJsonParser jsonParser;
	DGltf_Gltf gltf;
	if (jsonParser.ParseStruct(gltf, DGltf_Gltf::GetDeclaredClass(), jsonString) == false) return false;


	// ------------------- binary buffers (.bin or embedded) ---------------------

	DString dirpath = SPath::GetDirectory(Filepath_gltf);

	DByteReconverter cvt;

	DArray<DArray<byte>> buffers;

	for (const DGltf_Buffer& buffer : gltf.buffers)
	{
		if (buffer.uri.IsEmpty())
		{
			SLog::Error(TX("Empty buffer: ") + Filepath_gltf);  return false;
		}

		if (buffer.uri.StartsWith(EMBEDDED_BASE64_PREFIX))
		{
			// embedded
			if (SBase64::StringToByteArray(
				buffer.uri.ViewRange(EMBEDDED_BASE64_PREFIX.Length(), buffer.uri.Length()),
				buffers.AddDefault(),
				false
			) == false) return false;
		}
		else
		{
			// load from file (.bin)
			DString bufferFilepath = dirpath + DStrings::UTF8ToUTF16(buffer.uri);
			if (SBinaryFile::ReadFile(bufferFilepath, buffers.AddDefault()) == false) return false;
		}
	}

	// ------------------------------- images -----------------------------

	DArray<DBitmapImage> images;
	for (const DGltf_Image& gltfImage : gltf.images)
	{
		if (gltfImage.uri.IsEmpty())
		{
			// embedded in buffers
			const DGltf_BufferView& bufferView = gltf.bufferViews[gltfImage.bufferView];
			const DArray<byte>& buffer = buffers[bufferView.buffer];
			if (images.AddDefault().Load(
				buffer.ViewRange(bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength)
			) == false) return false;
		}
		else
		{
			// load from file
			if (images.AddDefault().Load(dirpath + gltfImage.uri) == false) return false;
		}
	}


	return ReadSceneFromData(
		gltf, buffers, images, PBRMetallicShared, ParentNode, dirpath, /*for messages*/Filepath_gltf, Options
	);
}


bool SGltfImporter::ReadSceneFromGLB(
	DStringView Filepath, 
	shared<HSurfaceShader> PBRMetallicShared, 
	shared<HSceneNode> ParentNode,
	const DImportOptions Options 
)
{
	DBinaryReader reader(Filepath);
	if (reader.IsOpen() == false) return false;

	DByteReconverter cvt(/*LittleEndian*/true);

	DFixedArray<byte, sizeof(uint32) * 3> headerBytes;
	if (reader.ReadBytes(sizeof(uint32) * 3, headerBytes.GetDataUnsafe()) == false)
	{
		SLog::Error(TX("Not a GLTF file (1): ") + Filepath);  return false;
	}

	cvt.SetChunk(headerBytes);

	uint32 magic;
	cvt.ReadUInt32(magic);
	uint32 version;
	cvt.ReadUInt32(version);
	uint32 length;
	cvt.ReadUInt32(length);

	if (magic != GLTF_ID)
	{
		SLog::Error(TX("Not a GLTF file (2)"));  return false;
	}
	if (version != 2)
	{
		SLog::Error(
			DString::Format(
				TX("Expected GLTF version 2, got {0}: "),
				{ DNumberFormat::DEFAULT.ToString(version), Filepath }
			)
		);  
		return false;
	}

	// ----------------------- JSON -------------------

	uint32 jsonChunkLength, jsonChunkType;
	ReadChunkHeader(reader, cvt, jsonChunkLength, jsonChunkType);

	if (jsonChunkType != JSON_ID)
	{
		SLog::Error(TX("No Json in GLTF file: ") + Filepath);  return false;
	}

	DArray<byte> jsonBytes;
	reader.ReadBytes(jsonChunkLength, jsonBytes);
	cvt.SetChunk(jsonBytes);
	DString8 jsonString;
	cvt.ReadString8(jsonChunkLength, jsonString);
	
	DJsonParser jsonParser;
	DGltf_Gltf gltf;
	if (jsonParser.ParseStruct(gltf, DGltf_Gltf::GetDeclaredClass(), jsonString) == false) return false;

	if (gltf.scenes.IsEmpty() || gltf.scene == -1) return false;

	// ------------------- buffers ---------------------

	DArray<DArray<byte>> buffers;

	uint32 bufferByteCount, bufferType;
	while (ReadChunkHeader(reader, cvt, bufferByteCount, bufferType))
	{
		if (bufferType != BIN_ID)
		{
			SLog::Error(TX("Not a binary buffer in: ") + Filepath);  return false;
		}

		if (reader.ReadBytes(bufferByteCount, buffers.AddDefault()) == false)
		{
			SLog::Error(TX("Failed to read buffer bytes in: ") + Filepath);  return false;
		}
	}

	// --------------------- images ------------------------

	DArray<DBitmapImage> images;
	for (const DGltf_Image& gltfImage : gltf.images)
	{
		const DGltf_BufferView& bufferView = gltf.bufferViews[gltfImage.bufferView];
		const DArray<byte>& buffer = buffers[bufferView.buffer];
		if (images.AddDefault().Load(
			buffer.ViewRange(bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength)
		) == false) return false;
	}


	return ReadSceneFromData(
		gltf, buffers, images, PBRMetallicShared, ParentNode, SPath::GetDirectory(Filepath), /*for messages*/Filepath, Options
	);
}


bool SGltfImporter::ReadSceneFromData(
	const DGltf_Gltf& Gltf, 
	const DArray<DArray<byte>>& Buffers,
	DArray<DBitmapImage>& Images,
	shared<HSurfaceShader> PBRMetallicShared, 
	shared<HSceneNode> ParentNode,
	DStringView Dirpath,
	DStringView MessagedFilepath,
	const DImportOptions Options 
)
{
	bool needsScaling = Options.ScaleToMeter != DImportOptions().ScaleToMeter;
	DSet<DBitmapImage*> flippedNormalmaps;

	DByteReconverter cvt;

	/*
	buffer -> bufferView -> accessor
	buffer: byte array
	bufferView: a section of a buffer with stride info
	accessor: item type info
	*/

	// ----------------------------- storage ---------------------------

	DArray<shared<HStaticMesh>> storageMeshes (Gltf.meshes.Length());
	DArray<shared<HSceneNode>> storageNodes;  // CAN CONTAIN NULLS!
	DArray<shared<HSurfaceMaterial>> storageMaterials (Gltf.materials.Length());
	DArray<shared<HTexture2D>> storageTextures (Gltf.textures.Length());

	// ---------------------------- textures ----------------------------

	for (const DGltf_Texture& gltfTexture : Gltf.textures)
	{
		shared<HTexture2D> texture = MakeShared<HTexture2D>();
		storageTextures.Add(texture);
		// not loading it becaus we don't know the settings just yet
	}

	// --------------------------- materials ---------------------------

	for (const DGltf_Material& gltfMaterial : Gltf.materials)
	{
		DSurfaceMaterialSettings materialSettings;
		materialSettings.Shader = PBRMetallicShared;
		materialSettings.DoubleSided = gltfMaterial.doubleSided;
		
		if (gltfMaterial.alphaMode == DGltf_Material::ALPHA_MODE_MASK)
		{
			materialSettings.BlendMode = ESurfaceMaterialBlendMode::ALPHA_TEST;
		}
		//else if (gltfMaterial.alphaMode == DGltf_Material::ALPHA_MODE_BLEND)
		//{
		//	material->BlendMode = ESurfaceMaterialBlendMode::ALPHA_BLEND;
		//}

		shared<HSurfaceMaterial> material = MakeShared<HSurfaceMaterial>();
		storageMaterials.Add(material);
		material->SetName(gltfMaterial.name);
		material->Load(materialSettings);
		
		// base color texture
		material->SetVec4Param(TX("BaseColor"), gltfMaterial.pbrMetallicRoughness.baseColorFactor);
		if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			const DGltf_GeneralTexture& gltfTexture = gltfMaterial.pbrMetallicRoughness.baseColorTexture;
			shared<HTexture2D> texture = storageTextures[gltfTexture.index];
			if (texture->IsLoaded() == false)
			{
				const DBitmapImage& image = Images[gltfTexture.index];
				DTexture2DSettings settings;
				bool success = true;
				switch (image.GetChannelCount())
				{
				case 3: settings.Format = ETexture2DFormat::RGB8;  break;
				case 4: settings.Format = ETexture2DFormat::RGBA8;  break;
				default : success = false;
				}
				settings.sRGB = true;
				if (success) texture->Load(image, settings);
			}

			material->SetTextureParam(TX("BaseColorTex"), texture);
			material->SetIntParam(TX("UseBaseColorTex"), 1);
		}

		// reflectance texture
		if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
		{
			const DGltf_GeneralTexture& gltfTexture = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture;
			shared<HTexture2D> texture = storageTextures[gltfTexture.index];
			if (texture->IsLoaded() == false)
			{
				const DBitmapImage& image = Images[gltfTexture.index];
				DTexture2DSettings settings;
				bool success = true;
				switch (image.GetChannelCount())
				{
				case 3: settings.Format = ETexture2DFormat::RGB8;  break;
				case 4: settings.Format = ETexture2DFormat::RGBA8;  break;
				default : success = false;
				}
				settings.sRGB = false;
				if (success) texture->Load(image, settings);
			}

			material->SetTextureParam(TX("ORMTex"), texture);
			material->SetIntParam(TX("UseORMTex"), 1);
			material->SetFloatParam(TX("Metallic"), 0);
			material->SetFloatParam(TX("Roughness"), 0);
		}
		else
		{
			material->SetFloatParam(TX("Metallic"), gltfMaterial.pbrMetallicRoughness.metallicFactor);
			material->SetFloatParam(TX("Roughness"), gltfMaterial.pbrMetallicRoughness.roughnessFactor);
		}

		// normalmap
		if (gltfMaterial.normalTexture.index >= 0)
		{
			const DGltf_NormalTexture& gltfTexture = gltfMaterial.normalTexture;
			shared<HTexture2D> texture = storageTextures[gltfTexture.index];
			if (texture->IsLoaded() == false)
			{
				DBitmapImage& image = Images[gltfTexture.index];
				DTexture2DSettings settings;
				bool success = true;
				switch (image.GetChannelCount())
				{
				case 3: settings.Format = ETexture2DFormat::RGB8;  break;
				case 4: settings.Format = ETexture2DFormat::RGBA8;  break;
				default : success = false;
				}
				settings.sRGB = false;
				if (success) 
				{
					if (Options.FlipNormalmapGreen)
					{
						if (flippedNormalmaps.Contains(&image) == false)
						{
							flippedNormalmaps.Add(&image);
							image.InvertChannel(1);
						}
					}
					texture->Load(image, settings);
				}
			}

			material->SetTextureParam(TX("NormalMap"), texture);
			material->SetIntParam(TX("UseNormalMap"), 1);
			material->SetFloatParam(TX("NormalIntensity"), gltfMaterial.normalTexture.scale);
		}

		// emission
		material->SetVec4Param(TX("Emission"), FVec4(gltfMaterial.emissiveFactor, 1.0f));
		if (gltfMaterial.emissiveTexture.index >= 0)
		{
			const DGltf_GeneralTexture& gltfTexture = gltfMaterial.emissiveTexture;
			shared<HTexture2D> texture = storageTextures[gltfTexture.index];
			if (texture->IsLoaded() == false)
			{
				const DBitmapImage& image = Images[gltfTexture.index];
				DTexture2DSettings settings;
				bool success = true;
				switch (image.GetChannelCount())
				{
				case 3: settings.Format = ETexture2DFormat::RGB8;  break;
				case 4: settings.Format = ETexture2DFormat::RGBA8;  break;
				default : success = false;
				}
				settings.sRGB = false;
				if (success) texture->Load(image, settings);
			}

			material->SetTextureParam(TX("EmissionTex"), texture);
			material->SetIntParam(TX("UseEmissionTex"), 1);
		}
	}

	// ----------------------- meshes ----------------------

	for (const DGltf_Mesh& gltfMesh : Gltf.meshes)
	{
		// ----------------- load geometry -----------------

		DStaticMeshData outMeshData;

		for (const DGltf_Primitive& gltfPrimitive : gltfMesh.primitives)
		{
			if (gltfPrimitive.mode != DGltf_Primitive::TYPE_TRIANGLES) continue;

			DStaticSubMeshData& outSubmeshData = outMeshData.SubMeshes.AddDefault();

			// POSITION
			const int32* positionAccessorIndexPtr = gltfPrimitive.attributes.Find(DGltf_Primitive::ATTRIBUTE_POSITION);
			if (positionAccessorIndexPtr)
			{
				const DGltf_Accessor& accessor = Gltf.accessors[*positionAccessorIndexPtr];
				const DGltf_BufferView& bufferView = Gltf.bufferViews[accessor.bufferView];
				const DArray<byte>& buffer = Buffers[bufferView.buffer];
				cvt.SetChunk(buffer.ViewRange(bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength));
				if (accessor.type == DGltf_Accessor::TYPE_VEC3 && accessor.componentType == DGltf_Accessor::COMPONENT_TYPE_FLOAT)
				{
					cvt.ReadHomogenousStructArray<FVec3, float, 3>(accessor.count, outSubmeshData.Positions);
				}
				else SLog::Error(TX("Unexpected position buffer format: ") + MessagedFilepath);
			}
			else SLog::Error(TX("No position buffer: ") + MessagedFilepath);

			// NORMAL
			const int32* normalAccessorIndexPtr = gltfPrimitive.attributes.Find(DGltf_Primitive::ATTRIBUTE_NORMAL);
			if (normalAccessorIndexPtr)
			{
				const DGltf_Accessor& accessor = Gltf.accessors[*normalAccessorIndexPtr];
				const DGltf_BufferView& bufferView = Gltf.bufferViews[accessor.bufferView];
				const DArray<byte>& buffer = Buffers[bufferView.buffer];
				cvt.SetChunk(buffer.ViewRange(bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength));
				if (accessor.type == DGltf_Accessor::TYPE_VEC3 && accessor.componentType == DGltf_Accessor::COMPONENT_TYPE_FLOAT)
				{
					cvt.ReadHomogenousStructArray<FVec3, float, 3>(accessor.count, outSubmeshData.Normals);
				}
				else SLog::Error(TX("Unexpected normal buffer format: ") + MessagedFilepath);
			}
			else SLog::Error(TX("No normal buffer: ") + MessagedFilepath);

			// UV 0 (optional)
			outSubmeshData.UVLayers.Resize(1);
			const int32* uvAccessorIndexPtr = gltfPrimitive.attributes.Find(DGltf_Primitive::ATTRIBUTE_TEXCOORD_0);
			if (uvAccessorIndexPtr)
			{
				const DGltf_Accessor& accessor = Gltf.accessors[*uvAccessorIndexPtr];
				const DGltf_BufferView& bufferView = Gltf.bufferViews[accessor.bufferView];
				const DArray<byte>& buffer = Buffers[bufferView.buffer];
				cvt.SetChunk(buffer.ViewRange(bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength));
				if (accessor.type == DGltf_Accessor::TYPE_VEC2 && accessor.componentType == DGltf_Accessor::COMPONENT_TYPE_FLOAT)
				{
					cvt.ReadHomogenousStructArray<FVec2, float, 2>(accessor.count, outSubmeshData.UVLayers[0].UVs);
				}
				else SLog::Error(TX("Unexpected uv0 buffer format: ") + MessagedFilepath);
			}

			// TANGENT (optional)
			bool tangentsImported = false;
			const int32* tangentAccessorIndexPtr = gltfPrimitive.attributes.Find(DGltf_Primitive::ATTRIBUTE_TANGENT);
			if (tangentAccessorIndexPtr)
			{
				tangentsImported = true;
				const DGltf_Accessor& accessor = Gltf.accessors[*tangentAccessorIndexPtr];
				const DGltf_BufferView& bufferView = Gltf.bufferViews[accessor.bufferView];
				const DArray<byte>& buffer = Buffers[bufferView.buffer];
				cvt.SetChunk(buffer.ViewRange(bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength));
				if (accessor.type == DGltf_Accessor::TYPE_VEC4 && accessor.componentType == DGltf_Accessor::COMPONENT_TYPE_FLOAT)
				{
					DArray<FVec4> tangents;
					cvt.ReadHomogenousStructArray<FVec4, float, 4>(accessor.count, tangents);
					DArray<FVec3>& finalTangents = outSubmeshData.Tangents;
					finalTangents.Reserve(tangents.Length());
					for (FVec4& vec4 : tangents)
					{
						outSubmeshData.Tangents.Emplace(vec4.X, vec4.Y, vec4.Z);
					}
				}
				else SLog::Error(TX("Unexpected tangent buffer format: ") + MessagedFilepath);
			}

			// INDEX
			{
				const DGltf_Accessor& accessor = Gltf.accessors[gltfPrimitive.indices];
				const DGltf_BufferView& bufferView = Gltf.bufferViews[accessor.bufferView];
				const DArray<byte>& buffer = Buffers[bufferView.buffer];
				cvt.SetChunk(buffer.ViewRange(bufferView.byteOffset, bufferView.byteOffset + bufferView.byteLength));

				if (accessor.type == DGltf_Accessor::TYPE_SCALAR)
				{
					if (accessor.componentType == DGltf_Accessor::COMPONENT_TYPE_UNSIGNED_INT)
					{
						cvt.ReadHomogenousStructArray<DStaticMeshTriangle, uint32, 3>(accessor.count, outSubmeshData.Tris);
					}
					else if (accessor.componentType == DGltf_Accessor::COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						DArray<uint16> indices;
						cvt.ReadUInt16Array(accessor.count, indices);
						DArray<DStaticMeshTriangle>& outIndices = outSubmeshData.Tris;
						int32 triCount = accessor.count / 3;
						outIndices.Reserve(triCount);
						int32 p = 0;
						for (int32 i = 0; i < triCount; ++i)
						{
							DStaticMeshTriangle& tri = outIndices.AddDefault();
							tri.VertIndex0 = indices[p];
							tri.VertIndex1 = indices[p + 1];
							tri.VertIndex2 = indices[p + 2];
							p += 3;
						}
					}
					else if (accessor.componentType == DGltf_Accessor::COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						DArray<uint8> indices;
						cvt.ReadUInt8Array(accessor.count, indices);
						DArray<DStaticMeshTriangle>& outIndices = outSubmeshData.Tris;
						int32 triCount = accessor.count / 3;
						outIndices.Reserve(triCount);
						int32 p = 0;
						for (int32 i = 0; i < triCount; ++i)
						{
							DStaticMeshTriangle& tri = outIndices.AddDefault();
							tri.VertIndex0 = indices[p];
							tri.VertIndex1 = indices[p + 1];
							tri.VertIndex2 = indices[p + 2];
							p += 3;
						}
					}
					else SLog::Error(TX("Unexpected index buffer format: ") + MessagedFilepath);
				}
				else SLog::Error(TX("Unexpected index buffer format: ") + MessagedFilepath);
			}

			if (needsScaling)
			{
				outSubmeshData.Scale(Options.ScaleToMeter);
			}

			if (tangentsImported == false)
			{
				outSubmeshData.PopulateTangents();
			}
		}

		outMeshData.UpdateBounds();

		shared<HStaticMesh> outMesh = MakeShared<HStaticMesh>();
		outMesh->Load(outMeshData);
		storageMeshes.Add(outMesh);
		outMesh->SetName(gltfMesh.name);

		// ------------------- assign materials to submeshes -----------------

		int32 prim_i = -1;
		for (const DGltf_Primitive& gltfPrimitive : gltfMesh.primitives)
		{
			if (gltfPrimitive.mode != DGltf_Primitive::TYPE_TRIANGLES) continue;

			prim_i++;

			if (gltfPrimitive.material >= 0)
			{
				shared<HSurfaceMaterial> material = storageMaterials[gltfPrimitive.material];
				outMesh->SetMaterialInSlot(prim_i, material);
				outMesh->SetMaterialSlotName(prim_i, material->GetName());
			}
			else if (PBRMetallicShared)
			{
				DSurfaceMaterialSettings materialSettings;
				materialSettings.Shader = PBRMetallicShared;
				shared<HSurfaceMaterial> material = MakeShared<HSurfaceMaterial>();
				material->Load(materialSettings);
				outMesh->SetMaterialInSlot(prim_i, material);
				outMesh->SetMaterialSlotName(prim_i, material->GetName());
			}
		}

	}

	// -------------------------------- nodes ----------------------------------

	for (const DGltf_Node& gltfNode : Gltf.nodes)
	{
		if (gltfNode.mesh >= 0)
		{
			shared<HStaticMeshNode> outMeshNode = MakeShared<HStaticMeshNode>();
			outMeshNode->SetMesh(storageMeshes[gltfNode.mesh]);
			outMeshNode->SetName(gltfNode.name);
			storageNodes.Add(outMeshNode);
		}
		else  // don't care about skeletal meshes and cameras
		{
			storageNodes.Add(nullptr);  // must not break indices
		}
	}

	// ----------------------------- default scene ------------------------------

	const DGltf_Scene& gltfScene = Gltf.scenes[Gltf.scene];

	for (int32 rootNode_i : gltfScene.nodes)
	{
		const DGltf_Node& gltfRootNode = Gltf.nodes[rootNode_i];
		shared<HSceneNode> rootNode = storageNodes[rootNode_i];
		if (rootNode)
		{
			ParentNode->AddChild(rootNode);
			FVec3 translation = gltfRootNode.translation;
			if (needsScaling) translation *= Options.ScaleToMeter;
			rootNode->SetTransform(translation, gltfRootNode.rotation, gltfRootNode.scale);
			PopulateChildrenRecursive(rootNode, gltfRootNode, storageNodes, Gltf.nodes, Options, needsScaling);
		}
	}

	return true;
}


void SGltfImporter::PopulateChildrenRecursive(
	shared<HSceneNode> Parent, const DGltf_Node& GltfParent, 
	const DArray<shared<HSceneNode>>& StorageNodes, const DArray<DGltf_Node>& GltfNodes,
	const DImportOptions& Options, bool NeedsScaling
)
{
	for (int32 childNode_i : GltfParent.children)
	{
		const DGltf_Node& gltfChildNode = GltfNodes[childNode_i];
		shared<HSceneNode> childNode = StorageNodes[childNode_i];
		if (childNode)
		{
			Parent->AddChild(childNode);
			FVec3 translation = gltfChildNode.translation;
			if (NeedsScaling) translation *= Options.ScaleToMeter;
			childNode->SetTransform(gltfChildNode.translation, gltfChildNode.rotation, gltfChildNode.scale);
			PopulateChildrenRecursive(childNode, gltfChildNode, StorageNodes, GltfNodes, Options, NeedsScaling);
		}
	}
}


bool SGltfImporter::ReadChunkHeader(DBinaryReader& Reader, DByteReconverter& Converter, uint32& OutChunkLength, uint32& OutChunkType)
{
	DFixedArray<byte, sizeof(uint32) * 2> headerBytes;
	if (Reader.ReadBytes(sizeof(uint32) * 2, headerBytes.GetDataUnsafe()))
	{
		Converter.SetChunk(headerBytes);
		Converter.ReadUInt32(OutChunkLength);
		Converter.ReadUInt32(OutChunkType);
		return true;
	}
	return false;
}
