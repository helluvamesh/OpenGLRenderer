#include "Engine\pch.h"
#include "SGltf.h"


REFLECT(SGltf::DGltf_Asset)
FIELD(generator)
FIELD(version)
REFLECT_END()


REFLECT(SGltf::DGltf_Primitive)
FIELD(mode)
FIELD(indices)
FIELD(attributes)
FIELD(material)
REFLECT_END()


REFLECT(SGltf::DGltf_Mesh)
FIELD(name)
FIELD(primitives)
REFLECT_END()


REFLECT(SGltf::DGltf_Node)
FIELD(name)
FIELD(mesh)
FIELD(children)
FIELD(matrix)
FIELD(translation)
FIELD(rotation)
FIELD(scale)
REFLECT_END()


REFLECT(SGltf::DGltf_Scene)
FIELD(name)
FIELD(nodes)
REFLECT_END()


REFLECT(SGltf::DGltf_Buffer)
FIELD(byteLength)
FIELD(uri)
REFLECT_END()


REFLECT(SGltf::DGltf_BufferView)
FIELD(buffer)
FIELD(byteOffset)
FIELD(byteLength)
FIELD(byteStride)
FIELD(target)
REFLECT_END()


REFLECT(SGltf::DGltf_SparseValues)
FIELD(bufferView)
REFLECT_END()


REFLECT(SGltf::DGltf_SparseIndices)
FIELD(bufferView)
FIELD(componentType)
REFLECT_END()


REFLECT(SGltf::DGltf_Sparse)
FIELD(values)
FIELD(indices)
REFLECT_END()


REFLECT(SGltf::DGltf_Accessor)
FIELD(bufferView)
FIELD(byteOffset)
FIELD(type)
FIELD(componentType)
FIELD(count)
FIELD(min)
FIELD(max)
FIELD(sparse)
REFLECT_END()


REFLECT(SGltf::DGltf_GeneralTexture)
FIELD(index)
FIELD(texCoord)
REFLECT_END()


REFLECT(SGltf::DGltf_NormalTexture)
FIELD(scale)
FIELD(index)
FIELD(texCoord)
REFLECT_END()


REFLECT(SGltf::DGltf_OcclusionTexture)
FIELD(strength)
FIELD(index)
FIELD(texCoord)
REFLECT_END()


REFLECT(SGltf::DGltf_PBRMetallicRoughness)
FIELD(baseColorTexture)
FIELD(baseColorFactor)
FIELD(metallicRoughnessTexture)
FIELD(metallicFactor)
FIELD(roughnessFactor)
REFLECT_END()


REFLECT(SGltf::DGltf_Material)
FIELD(name)
FIELD(pbrMetallicRoughness)
FIELD(normalTexture)
FIELD(occlusionTexture)
FIELD(emissiveTexture)
FIELD(emissiveFactor)
FIELD(alphaMode)
FIELD(alphaCutoff)
FIELD(doubleSided)
REFLECT_END()


REFLECT(SGltf::DGltf_Texture)
FIELD(source)
REFLECT_END()


REFLECT(SGltf::DGltf_Image)
FIELD(bufferView)
FIELD(name)
FIELD(uri)
REFLECT_END()


REFLECT(SGltf::DGltf_Gltf)
FIELD(asset)
FIELD(scene)
FIELD(nodes)
FIELD(meshes)
FIELD(accessors)
FIELD(bufferViews)
FIELD(buffers)
FIELD(materials)
FIELD(textures)
FIELD(images)
FIELD(scenes)
REFLECT_END()
