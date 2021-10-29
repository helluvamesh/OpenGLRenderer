#pragma once


// GLTF-2 Specification:
// https://github.com/KhronosGroup/glTF/tree/main/specification/2.0/schema
// https://www.khronos.org/files/gltf20-reference-guide.pdf


namespace SGltf
{

	const uint32 GLTF_ID = 0x46546C67;
	const uint32 JSON_ID = 0x4E4F534A;
	const uint32 BIN_ID = 0x004E4942;
	const DString8 EMBEDDED_BASE64_PREFIX = "data:application/octet-stream;base64,";


	struct DGltf_Asset : public DStruct
	{
		REFLECT_H(DGltf_Asset, DStruct)
	public:
		DString8 generator;
		DString8 version;
	};


	struct DGltf_Primitive : public DStruct
	{
		REFLECT_H(DGltf_Primitive, DStruct)
	public:
		static const int32 TYPE_TRIANGLES = 4;

		static inline const DString8 ATTRIBUTE_POSITION = "POSITION";
		static inline const DString8 ATTRIBUTE_TEXCOORD_0 = "TEXCOORD_0";
		static inline const DString8 ATTRIBUTE_NORMAL = "NORMAL";
		static inline const DString8 ATTRIBUTE_TANGENT = "TANGENT";

		int32 mode = TYPE_TRIANGLES;
		int32 indices = -1;
		DMap<DString8, int32> attributes;
		int32 material = -1;
	};


	struct DGltf_Mesh : public DStruct
	{
		REFLECT_H(DGltf_Mesh, DStruct)
	public:
		DString name;
		DArray<DGltf_Primitive> primitives;
	};


	struct DGltf_Node : public DStruct
	{
		REFLECT_H(DGltf_Node, DStruct)
	public:
		DString name;
		int32 mesh = -1;
		DArray<int32> children;
		FMat4 matrix;  // column-major! - needs to be transposed
		FVec3 translation;
		FQuat rotation;
		FVec3 scale = FVec3(1.0f);
	};


	struct DGltf_Scene : public DStruct
	{
		REFLECT_H(DGltf_Scene, DStruct)
	public:
		DString name;
		DArray<int32> nodes;
	};


	struct DGltf_Buffer : public DStruct
	{
		REFLECT_H(DGltf_Buffer, DStruct)
	public:
		int32 byteLength = 0;
		DString8 uri;
	};


	struct DGltf_BufferView : public DStruct
	{
		REFLECT_H(DGltf_BufferView, DStruct)
	public:
		int32 buffer = -1;
		int32 byteOffset = 0;
		int32 byteLength = 0;
		int32 byteStride = 0;
		int32 target = -1;
	};


	struct DGltf_SparseValues : public DStruct
	{
		REFLECT_H(DGltf_SparseValues, DStruct)
	public:
		int32 bufferView = -1;
	};


	struct DGltf_SparseIndices : public DStruct
	{
		REFLECT_H(DGltf_SparseIndices, DStruct)
	public:
		int32 bufferView = -1;
		int32 componentType = 0;
	};


	struct DGltf_Sparse : public DStruct
	{
		REFLECT_H(DGltf_Sparse, DStruct)
	public:
		int32 count = 0;
		DArray<DGltf_SparseValues> values;
		DArray<DGltf_SparseIndices> indices;
	};


	struct DGltf_Accessor : public DStruct
	{
		REFLECT_H(DGltf_Accessor, DStruct)
	public:
		static inline const DString8 TYPE_SCALAR = "SCALAR";
		static inline const DString8 TYPE_VEC2 = "VEC2";
		static inline const DString8 TYPE_VEC3 = "VEC3";
		static inline const DString8 TYPE_VEC4 = "VEC4";
		static inline const DString8 TYPE_MAT2 = "MAT2";
		static inline const DString8 TYPE_MAT3 = "MAT3";
		static inline const DString8 TYPE_MAT4 = "MAT4";

		static const int32 COMPONENT_TYPE_BYTE = 5120;
		static const int32 COMPONENT_TYPE_UNSIGNED_BYTE = 5121;
		static const int32 COMPONENT_TYPE_SHORT = 5122;
		static const int32 COMPONENT_TYPE_UNSIGNED_SHORT = 5123;
		static const int32 COMPONENT_TYPE_UNSIGNED_INT = 5125;
		static const int32 COMPONENT_TYPE_FLOAT = 5126;

		int32 bufferView = -1;
		int32 byteOffset = 0;
		DString8 type;
		int32 componentType = 0;
		int32 count = 0;
		DArray<float> min;  // itemCount: 1 - 16
		DArray<float> max;  // itemCount: 1 - 16
		DGltf_Sparse sparse;
	};


	struct DGltf_GeneralTexture : public DStruct
	{
		REFLECT_H(DGltf_GeneralTexture, DStruct)
	public:
		int32 index = -1;
		int32 texCoord = 0;
	};


	struct DGltf_NormalTexture : public DStruct
	{
		REFLECT_H(DGltf_NormalTexture, DStruct)
	public:
		float scale = 1.0f;
		int32 index = -1;
		int32 texCoord = 0;
	};


	struct DGltf_OcclusionTexture : public DStruct
	{
		REFLECT_H(DGltf_OcclusionTexture, DStruct)
	public:
		float strength = 1.0f;
		int32 index = -1;
		int32 texCoord = 0;
	};


	struct DGltf_PBRMetallicRoughness : public DStruct
	{
		REFLECT_H(DGltf_PBRMetallicRoughness, DStruct)
	public:
		DGltf_GeneralTexture baseColorTexture;
		FVec4 baseColorFactor = FVec4(1.0f);
		DGltf_GeneralTexture metallicRoughnessTexture;
		float metallicFactor = 1.0f;  // if no value is found, it should be 1
		float roughnessFactor = 1.0f;  // if no value is found, it should be 1
	};


	struct DGltf_Material : public DStruct
	{
		REFLECT_H(DGltf_Material, DStruct)
	public:
		static inline const DString8 ALPHA_MODE_OPAQUE = "OPAQUE";
		static inline const DString8 ALPHA_MODE_MASK = "MASK";
		static inline const DString8 ALPHA_MODE_BLEND = "BLEND";

		DString name;
		DGltf_PBRMetallicRoughness pbrMetallicRoughness;
		DGltf_NormalTexture normalTexture;
		DGltf_OcclusionTexture occlusionTexture;
		DGltf_GeneralTexture emissiveTexture;
		FVec3 emissiveFactor = FVec3(.0f);
		DString8 alphaMode = ALPHA_MODE_OPAQUE;
		float alphaCutoff = .5f;
		bool doubleSided = false;
	};


	struct DGltf_Texture : public DStruct
	{
		REFLECT_H(DGltf_Texture, DStruct)
	public:
		int32 source = -1;
	};


	struct DGltf_Image : public DStruct
	{
		REFLECT_H(DGltf_Image, DStruct)
	public:
		int32 bufferView = -1;
		DString name;
		DString uri;
	};


	struct DGltf_Gltf : public DStruct
	{
		REFLECT_H(DGltf_Gltf, DStruct)
	public:
		DGltf_Asset asset;
		int32 scene = -1;
		DArray<DGltf_Node> nodes;
		DArray<DGltf_Mesh> meshes;
		DArray<DGltf_Accessor> accessors;
		DArray<DGltf_BufferView> bufferViews;
		DArray<DGltf_Buffer> buffers;
		DArray<DGltf_Material> materials;
		DArray<DGltf_Texture> textures;
		DArray<DGltf_Image> images;
		DArray<DGltf_Scene> scenes;
	};
};
