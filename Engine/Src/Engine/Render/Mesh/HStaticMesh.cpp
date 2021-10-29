#include "Engine/pch.h"
#include "HStaticMesh.h"
#include "Engine/Asset/Wavefront/DWavefrontParser.h"
#include "GL\glew.h"
#include "Engine\Render\RenderUtil\GLUtil.h"
#include "Engine\Asset\GLTF\SGltfImporter.h"


// ------------------ HStaticSubMesh --------------------


HStaticSubMesh::HStaticSubMesh(const DStaticSubMeshAssetData& Data)
{
    this->IndexBufferLength = Data.FaceIndices.Length();

    int32 positionByteCount = Data.Positions.Length() * sizeof(float);
    int32 normalByteCount = Data.Normals.Length() * sizeof(int16);
    int32 uvByteCount = Data.UVs.Length() * sizeof(int16);
    int32 tangentByteCount = Data.Tangents.Length() * sizeof(int16);
    int32 colorByteCount = Data.Colors.Length() * sizeof(uint8);
    int32 uv2ByteCount = Data.UVs_2.Length() * sizeof(int16);

    int32 totalBufferByteCount = 
        positionByteCount + normalByteCount + uvByteCount + tangentByteCount + colorByteCount + uv2ByteCount;

    int32 positionOffset = 0;
    int32 normalOffset = positionOffset + positionByteCount;
    int32 uvOffset = normalOffset + normalByteCount;
    int32 tangentOffset = uvOffset + uvByteCount;
    int32 colorOffset = tangentOffset + tangentByteCount;
    int32 uv2Offset = colorOffset + colorByteCount;

    // VERTEX ARRAY
    glCheck(glGenVertexArrays(1, &this->VertArrayId));
    glCheck(glBindVertexArray(this->VertArrayId));

    // VERTEX BUFFER
    glCheck(glGenBuffers(1, &this->VertBufferId));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, this->VertBufferId));
    glCheck(glBufferData(GL_ARRAY_BUFFER, totalBufferByteCount, nullptr, GL_STATIC_DRAW));

    glCheck(glBufferSubData(GL_ARRAY_BUFFER, positionOffset, positionByteCount, Data.Positions.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, normalOffset, normalByteCount, Data.Normals.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, uvOffset, uvByteCount, Data.UVs.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, tangentOffset, tangentByteCount, Data.Tangents.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, colorOffset, colorByteCount, Data.Colors.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, uv2Offset, uv2ByteCount, Data.UVs_2.Data()));

    // POSITION ATTRIBUTE
    glCheck(glEnableVertexAttribArray(0));
    glCheck(glVertexAttribPointer(
        0,                  // attribute index: position
        3,                  // size: 3 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(float) * 3,  // stride: bytes to jump to get to the next vertex (all attributes included)
        (void*)(intptr_t)positionOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // NORMAL ATTRIBUTE
    glCheck(glEnableVertexAttribArray(1));
    glCheck(glVertexAttribPointer(
        1,                  // attribute index: normal
        3,                  // size: 3 shorts
        GL_SHORT,           // data type
        GL_TRUE,            // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(int16) * 3,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)normalOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // UV ATTRIBUTE
    glCheck(glEnableVertexAttribArray(2));
    glCheck(glVertexAttribPointer(
        2,                  // attribute index: uv-0
        2,                  // size: 2 unsigned shorts
        GL_HALF_FLOAT,      // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(int16) * 2,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)uvOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // TANGENT ATTRIBUTE
    glCheck(glEnableVertexAttribArray(3));
    glCheck(glVertexAttribPointer(
        3,                  // attribute index: tangent
        3,                  // size: 3 shorts
        GL_SHORT,           // data type
        GL_TRUE,            // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(int16) * 3,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)tangentOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // COLOR ATTRIBUTE
    glCheck(glEnableVertexAttribArray(4));
    glCheck(glVertexAttribPointer(
        4,                  // attribute index: color
        4,                  // size: 4 bytes
        GL_UNSIGNED_BYTE,   // data type
        GL_TRUE,            // whether the coords need to be normalized to [0.0...1.0]
        sizeof(uint8) * 4,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)colorOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // UV2 ATTRIBUTE
    glCheck(glEnableVertexAttribArray(5));
    glCheck(glVertexAttribPointer(
        5,                  // attribute index: uv-1
        2,                  // size: 2 unsigned shorts
        GL_HALF_FLOAT,      // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(int16) * 2,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)uv2Offset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // INDEX BUFFER
    glCheck(glGenBuffers(1, &this->IndexBufferId));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferId));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * this->IndexBufferLength, Data.FaceIndices.Data(), GL_STATIC_DRAW));

    glCheck(glBindVertexArray(0));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


HStaticSubMesh::HStaticSubMesh(const DStaticSubMeshData& Data)
{
    int32 uvLayer1_byteCount;
    int32 uvLayer2_byteCount;
    const FVec2* uvLayer1_data;
    const FVec2* uvLayer2_data;

    if (Data.UVLayers.Length() >= 1)
    {
        uvLayer1_byteCount = Data.UVLayers[0].UVs.Length() * 2 * sizeof(float);
        uvLayer1_data = Data.UVLayers[0].UVs.Data();
    }
    else
    {
        uvLayer1_byteCount = 0;
        uvLayer1_data = nullptr;
    }

    if (Data.UVLayers.Length() >= 2)
    {
        uvLayer2_byteCount = Data.UVLayers[1].UVs.Length() * 2 * sizeof(float);
        uvLayer2_data = Data.UVLayers[1].UVs.Data();
    }
    else
    {
        uvLayer2_byteCount = 0;
        uvLayer2_data = nullptr;
    }
    
    this->IndexBufferLength = Data.Tris.Length() * 3;

    int32 positionByteCount = Data.Positions.Length() * 3 * sizeof(float);
    int32 normalByteCount = Data.Normals.Length() * 3 * sizeof(float);
    int32 uvByteCount = uvLayer1_byteCount;
    int32 tangentByteCount = Data.Tangents.Length() * 3 * sizeof(float);
    int32 colorByteCount = Data.Colors.Length() * 4 * sizeof(float);
    int32 uv2ByteCount = uvLayer2_byteCount;

    int32 totalBufferByteCount = 
        positionByteCount + normalByteCount + uvByteCount + tangentByteCount + colorByteCount + uv2ByteCount;

    int32 positionOffset = 0;
    int32 normalOffset = positionOffset + positionByteCount;
    int32 uvOffset = normalOffset + normalByteCount;
    int32 tangentOffset = uvOffset + uvByteCount;
    int32 colorOffset = tangentOffset + tangentByteCount;
    int32 uv2Offset = colorOffset + colorByteCount;
    
    // VERTEX ARRAY
    glCheck(glGenVertexArrays(1, &this->VertArrayId));
    glCheck(glBindVertexArray(this->VertArrayId));

    // VERTEX BUFFER
    glCheck(glGenBuffers(1, &this->VertBufferId));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, this->VertBufferId));
    glCheck(glBufferData(GL_ARRAY_BUFFER, totalBufferByteCount, nullptr, GL_STATIC_DRAW));

    glCheck(glBufferSubData(GL_ARRAY_BUFFER, positionOffset, positionByteCount, Data.Positions.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, normalOffset, normalByteCount, Data.Normals.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, uvOffset, uvByteCount, uvLayer1_data));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, tangentOffset, tangentByteCount, Data.Tangents.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, colorOffset, colorByteCount, Data.Colors.Data()));
    glCheck(glBufferSubData(GL_ARRAY_BUFFER, uv2Offset, uv2ByteCount, uvLayer2_data));

    // POSITION ATTRIBUTE
    glCheck(glEnableVertexAttribArray(0));
    glCheck(glVertexAttribPointer(
        0,                  // attribute index: position
        3,                  // size: 3 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(float) * 3,  // stride: bytes to jump to get to the next vertex (all attributes included)
        (void*)(intptr_t)positionOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // NORMAL ATTRIBUTE
    glCheck(glEnableVertexAttribArray(1));
    glCheck(glVertexAttribPointer(
        1,                  // attribute index: normal
        3,                  // size: 3 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(float) * 3,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)normalOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // UV ATTRIBUTE
    glCheck(glEnableVertexAttribArray(2));
    glCheck(glVertexAttribPointer(
        2,                  // attribute index: uv-0
        2,                  // size: 2 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(float) * 2,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)uvOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // TANGENT ATTRIBUTE
    glCheck(glEnableVertexAttribArray(3));
    glCheck(glVertexAttribPointer(
        3,                  // attribute index: tangent
        3,                  // size: 3 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(float) * 3,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)tangentOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // COLOR ATTRIBUTE
    glCheck(glEnableVertexAttribArray(4));
    glCheck(glVertexAttribPointer(
        4,                  // attribute index: color
        4,                  // size: 4 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [0.0...1.0]
        sizeof(float) * 4,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)colorOffset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // UV2 ATTRIBUTE
    glCheck(glEnableVertexAttribArray(5));
    glCheck(glVertexAttribPointer(
        5,                  // attribute index: uv-1
        2,                  // size: 2 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(float) * 2,  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)uv2Offset  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // INDEX BUFFER
    glCheck(glGenBuffers(1, &this->IndexBufferId));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferId));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * this->IndexBufferLength, Data.Tris.Data(), GL_STATIC_DRAW));

    glCheck(glBindVertexArray(0));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


HStaticSubMesh::~HStaticSubMesh()
{
    glCheck(glDeleteVertexArrays(1, &this->VertArrayId));
}


void HStaticSubMesh::Draw(HSurfaceMaterial* Material)
{
    if (Material)
    {
        glCheck(glBindVertexArray(this->VertArrayId));

        bool alphaBlend = Material->GetBlendMode() == ESurfaceMaterialBlendMode::ALPHA_BLEND;
        
        if (alphaBlend)
        {
            Material->AlphaBlendStep1();
        }

        glCheck(glDrawElements(GL_TRIANGLES, this->IndexBufferLength, GL_UNSIGNED_INT, nullptr));

        if (alphaBlend)
        {
            Material->AlphaBlendStep2();
            glCheck(glDrawElements(GL_TRIANGLES, this->IndexBufferLength, GL_UNSIGNED_INT, nullptr));
        }
    }
}


void HStaticSubMesh::DrawStandalone()
{
    glCheck(glBindVertexArray(this->VertArrayId));
    glCheck(glDrawElements(GL_TRIANGLES, this->IndexBufferLength, GL_UNSIGNED_INT, nullptr));
}


bool HStaticSubMesh::IsLoaded()
{
    return this->VertBufferId != 0;
}


// ------------------ HStaticMesh --------------------


void HStaticMesh::DrawStandalone()
{
	for (int32 i = 0; i < this->MaterialSlots.Length(); i++)
	{
		const DStaticMeshMaterialSlot& slot = this->MaterialSlots[i];
		if (slot.Submesh)
		{
			slot.Submesh->DrawStandalone();
		}
	}
}


void HStaticMesh::Load()
{
    this->Unload();

	DArray<byte> bytes;
	if (SBinaryFile::ReadFile(this->Filepath, /*out*/bytes) == false) return;

	DAssetHeader header;
	int32 byteProgress = 
		SStrictSerializer::DeserializeStruct(header, DAssetHeader::GetDeclaredClass(), bytes.Data());

	if (header.AssetType != EAssetType::STATIC_MESH)
	{
		SLog::Error(TX("Expected STATIC_MESH asset type in ") + this->Filepath);
		return;
	}

	DStaticMeshAssetData data;
	SStrictSerializer::DeserializeStruct(data, DStaticMeshAssetData::GetDeclaredClass(), &bytes[byteProgress]);
	this->BoundingBox = data.BoundingBox;
	this->MaterialSlots.Clear();
	for (DStaticSubMeshAssetData& loadedSlot : data.SubMeshes)
	{
		DStaticMeshMaterialSlot& slot = this->MaterialSlots.AddDefault();
		slot.Name = loadedSlot.Name;
		slot.Submesh = MakeShared<HStaticSubMesh>(loadedSlot);
	}
}


bool HStaticMesh::IsLoaded()
{
	return this->MaterialSlots.Length() > 0;
}


void HStaticMesh::Unload()
{
	this->MaterialSlots.Clear();
}


void HStaticMesh::Load(const DStaticMeshData& MeshData)
{
    this->Unload();

    this->MaterialSlots.Clear();
    this->BoundingBox = MeshData.BoundingBox;
    for (const DStaticSubMeshData& loadedSlot : MeshData.SubMeshes)
    {
        DStaticMeshMaterialSlot& slot = this->MaterialSlots.AddDefault();
        slot.Name = loadedSlot.Name;
        slot.Submesh = MakeShared<HStaticSubMesh>(loadedSlot);
    }
}


const FAxisBox& HStaticMesh::GetBoundingBox() const
{
	return this->BoundingBox;
}


const DArray<DStaticMeshMaterialSlot>& HStaticMesh::GetMaterialSlots() const
{
    return this->MaterialSlots;
}


void HStaticMesh::SetMaterialInSlot(int32 Index, shared<HSurfaceMaterial> NewMaterial)
{
    if (this->MaterialSlots.IsValidIndex(Index))
    {
        this->MaterialSlots[Index].Material = NewMaterial;
    }
}


void HStaticMesh::SetMaterialSlotName(int32 Index, DStringView NewName)
{
    if (this->MaterialSlots.IsValidIndex(Index))
    {
        this->MaterialSlots[Index].Name = DString::From(NewName);
    }
}
