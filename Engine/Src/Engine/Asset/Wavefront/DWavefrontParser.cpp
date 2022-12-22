#include "Engine/pch.h"
#include "DWavefrontParser.h"
#include "DWavefrontEncoder.h"


bool DWavefrontParser::DImportOptions::Equals(const DImportOptions& Other) const
{
	return 
		this->Right == Other.Right &&
		this->Up == Other.Up &&
		this->Back == Other.Back &&
		this->ScaleToMeter == Other.ScaleToMeter;
}


int32 DWavefrontParser::FindEndOfNumberString(const DString8& Str, int32 Start)
{
	int32 len = Str.Length();
	bool foundFirstDigit = false;
	for (int32 i = Start; i < len; ++i)
	{
		char8 c = Str[i];
		// this will only return correct end index if the number string is correctly formed, this does not validate it
		if (SAscii::IsDigit(c) || c == '-' || c == '.' || c == '+' || c == 'E' || c == 'e')
		{
			foundFirstDigit = true;
		}
		else if (SAscii::IsWhitespace(c))
		{
			if (foundFirstDigit)
			{
				return i;
			}
		}
		else
		{
			return i;
		}
	}
	return len;
}



int32 DWavefrontParser::ParseNextFloat(int32 Start, float& OutValue)
{
	int32 end = FindEndOfNumberString(*this->Str, Start);
	if (end >= 0)
	{
		OutValue = DString8::ParseFloat(this->Str->ViewRange(Start, end));
		return end;
	}
	else
	{
		throw DWavefrontException("Expected float", *this->Str, Start);
	}
}


int32 DWavefrontParser::ParseNextIndex(int32 Start, int32& OutValue)
{
	int32 end = FindEndOfNumberString(*this->Str, Start);
	if (end >= 0)
	{
		OutValue = DString8::ParseInt32(this->Str->ViewRange(Start, end)) - 1;
		return end;
	}
	else
	{
		throw DWavefrontException("Expected index", *this->Str, Start);
	}
}


int32 DWavefrontParser::ParseNextName(int32 Start, DString8& OutValue)
{
	int32 end = Start;
	for (int32 i = Start; i < this->Str->Length(); ++i)
	{
		char c = this->Str->At(i);
		if (c == '\n')
		{
			end = i;
			break;
		}
		else if (SAscii::IsWhitespace(c))
		{
			if (end > Start) continue;
			else Start++;
		}
	}
	OutValue = this->Str->SubstrRange(Start, end);
	return end;
}


int32 DWavefrontParser::IndexOfVert(
	const DStaticSubMeshData& Submesh, const FVec3& Position, const FVec3& Normal, const FVec2& UV
)
{
	const DArray<FVec3>& positions = Submesh.Positions;
	const DArray<FVec3>& normals = Submesh.Normals;
	const DArray<FVec2>& uvs = Submesh.UVLayers[0].UVs;

	int32 vertCount = Submesh.Positions.Length();
	for (int32 i = 0; i < vertCount; ++i)
	{
		if (positions[i] == Position &&
			normals[i] == Normal &&
			uvs[i] == UV)
		{
			return i;
		}
	}

	return -1;
}


int32 DWavefrontParser::IndexOfVert(
	const DStaticSubMeshData& Submesh, const FVec3& Position, const FVec3& Normal
)
{
	const DArray<FVec3>& positions = Submesh.Positions;
	const DArray<FVec3>& normals = Submesh.Normals;

	int32 vertCount = Submesh.Positions.Length();
	for (int32 i = 0; i < vertCount; ++i)
	{
		if (positions[i] == Position &&
			normals[i] == Normal)
		{
			return i;
		}
	}

	return -1;
}


void DWavefrontParser::_Parse(const DString8& InStr, DArray<DStaticMeshData>& OutMeshes)
{
	this->Str = &InStr;

	OutMeshes.Clear();
	
	if (this->Str->Length() == 0) return;

	int32 activeGroup = -1;
	EAttributeLayout attrLayout = EAttributeLayout::UNDETECTED;

	DString8 objectName = u8"Main";
	DArray<FVec3> vertices;
	DArray<FVec2> uvs;
	DArray<FVec3> normals;
	DArray<_MaterialGroup> groups;

	auto resetObjectData = [&activeGroup, &attrLayout, &groups]() {

		activeGroup = -1;
		attrLayout = EAttributeLayout::UNDETECTED;
		groups.Clear();
	};

	vertices.Reserve(100);
	uvs.Reserve(100);
	normals.Reserve(100);
	groups.Reserve(1);

	int32 last_i = this->Str->Length() - 1;
	for (int32 i = 0; i < this->Str->Length(); i++)
	{
		char c = Str->At(i);
		
		if (c == 'v')
		{
			if (i + 1 < this->Str->Length())
			{
				char nextChar = Str->At(i + 1);
				// vertex
				if (nextChar == ' ')
				{
					FVec3& vert = vertices.AddDefault();
					i = this->ParseNextFloat(i + 1, /*out*/vert.X);
					i = this->ParseNextFloat(i, /*out*/vert.Y);
					i = this->ParseNextFloat(i, /*out*/vert.Z) - 1;
				}
				// uv
				else if (nextChar == 't' && i + 2 < this->Str->Length())
				{
					FVec2& uv = uvs.AddDefault();
					i = this->ParseNextFloat(i + 2, /*out*/uv.X);
					i = this->ParseNextFloat(i, /*out*/uv.Y) - 1;
				}
				// normal
				else if (nextChar == 'n' && i + 2 < this->Str->Length())
				{
					FVec3& nor = normals.AddDefault();
					i = this->ParseNextFloat(i + 2, /*out*/nor.X);
					i = this->ParseNextFloat(i, /*out*/nor.Y);
					i = this->ParseNextFloat(i, /*out*/nor.Z) - 1;
				}
			}
		}
		// face
		else if (c == 'f')
		{
			if (groups.IsValidIndex(activeGroup) == false)
			{
				// start new group
				_MaterialGroup& g = groups.AddDefault();
				g.MaterialName = objectName;
				activeGroup = groups.Length() - 1;
			}

			if (attrLayout == EAttributeLayout::UNDETECTED)
			{
				// determine attribute layout of indices
				int32 space_i = this->Str->IndexOf(" ", i + 2);
				if (space_i >= 0)
				{
					DString8 sample = this->Str->SubstrRange(i + 2, space_i);
					DArray<DString8> attributes = sample.Split("/");
					if (attributes.Length() == 3)
					{
						if (attributes[1].IsEmpty())
						{
							attrLayout = EAttributeLayout::VN;
						}
						else
						{
							attrLayout = EAttributeLayout::VTN;
						}
					}
					else if (attributes.Length() == 2)
					{
						attrLayout = EAttributeLayout::VT;
						throw DWavefrontException("No normals found in .obj file (found \"V/T\" layout)");
					}
					else  // 1
					{
						attrLayout = EAttributeLayout::V;
						throw DWavefrontException("No normals found in .obj file (found \"V\" layout)");
					}
				}
				else throw DWavefrontException("Malformed .obj file");
			}

			DFixedArray<int32, 9>* indices = &groups[activeGroup].FaceIndices.AddDefault();

			int32 face_i = 0;
			for (int32 si = i + 1; si < this->Str->Length(); ++si)
			{
				char c = this->Str->At(si);
				if (SAscii::IsDigit(c))
				{
					// triangulate immidiately

					if (attrLayout == EAttributeLayout::VTN)
					{
						if (face_i > 8)
						{
							DFixedArray<int32, 9>* prevIndices = indices;
							indices = &groups[activeGroup].FaceIndices.AddDefault();
							indices->At(0) = prevIndices->At(0);
							indices->At(1) = prevIndices->At(1);
							indices->At(2) = prevIndices->At(2);
							indices->At(3) = prevIndices->At(6);
							indices->At(4) = prevIndices->At(7);
							indices->At(5) = prevIndices->At(8);
							face_i = 6;
						}
					}
					else if (attrLayout == EAttributeLayout::VN)
					{
						if (face_i > 5)
						{
							DFixedArray<int32, 9>* prevIndices = indices;
							indices = &groups[activeGroup].FaceIndices.AddDefault();
							indices->At(0) = prevIndices->At(0);
							indices->At(1) = prevIndices->At(1);
							indices->At(2) = prevIndices->At(4);
							indices->At(3) = prevIndices->At(5);
							face_i = 4;
						}
					}
					
					si = this->ParseNextIndex(si, /*out*/indices->At(face_i++)) - 1;
				}
				else if (c == '/')
				{
					continue;
				}
				else if (c == '\n')
				{
					i = si;
					break;
				}
				else if (SAscii::IsWhitespace(c))
				{
					continue;
				}
				else
				{
					throw DWavefrontException("Unexpected char", *this->Str, si);
				}
			}
			
			int32 requiredFace_i = attrLayout == EAttributeLayout::VTN? 9 : 6;
			if (face_i != requiredFace_i)
			{
				// not enougth verts to make a face, discard
				groups[activeGroup].FaceIndices.RemoveLast();
			}
		}
		// object
		else if (c == 'o')
		{
			i = this->ParseNextName(i + 2, /*out*/objectName) - 1;
			
			if (this->FoundFirstObject)
			{
				DStaticMeshData& meshData = OutMeshes.AddDefault();
				meshData.Name = DStrings::UTF8ToUTF16(objectName);
				this->PopulateMeshData(meshData, vertices, uvs, normals, groups, attrLayout);
				resetObjectData();
			}

			this->FoundFirstObject = true;
		}
		// group
		else if (c == 'g')
		{
			DString8 line;
			i = this->ParseNextName(i, /*out*/line) - 1;
			groups.AddDefault();
			activeGroup = groups.Length() - 1;
		}
		// material
		else if (c == 'u')
		{
			if (groups.IsValidIndex(activeGroup) == false)
			{
				_MaterialGroup& g = groups.AddDefault();
				g.MaterialName = objectName;
				activeGroup = 0;
			}
			DString8 line;
			i = this->ParseNextName(i, /*out*/line) - 1;
			if (line.StartsWith("usemtl "))
			{
				groups[activeGroup].MaterialName = line.Substr(7);
			}
		}
		// comment(#) or unhandled prefix
		else
		{
			DString8 line;
			i = ParseNextName(i, /*out*/line);
		}
	}
	
	if (groups.IsEmpty() == false)
	{
		DStaticMeshData& meshData = OutMeshes.AddDefault();
		meshData.Name = DStrings::UTF8ToUTF16(objectName);
		this->PopulateMeshData(meshData, vertices, uvs, normals, groups, attrLayout);
	}
}


void DWavefrontParser::PopulateMeshData(
	DStaticMeshData& OutMeshData, 
	const DArray<FVec3>& Vertices, const DArray<FVec2>& UVs, const DArray<FVec3>& Normals,
	const DArray<_MaterialGroup>& Groups, EAttributeLayout AttributeLayout
)
{
	for (const _MaterialGroup& group : Groups)
	{
		DStaticSubMeshData& outSubMesh = OutMeshData.SubMeshes.AddDefault();
		outSubMesh.Name = DStrings::UTF8ToUTF16(group.MaterialName);

		int32 faceCount = group.FaceIndices.Length();
		DArray<DStaticMeshTriangle>& outTris = outSubMesh.Tris;
		outTris.Reserve(faceCount);

		int32 vertCount = faceCount * 3;

		DArray<FVec3>& outPositions = outSubMesh.Positions;
		outPositions.Reserve(vertCount);

		DArray<FVec3>& outNormals = outSubMesh.Normals;
		outNormals.Reserve(vertCount);
		
		try
		{		
			if (AttributeLayout == EAttributeLayout::VTN)
			{
				DArray<FVec2>& outUVs = outSubMesh.UVLayers.AddDefault().UVs;
				outUVs.Reserve(vertCount);

				for (int32 i = 0; i < faceCount; i++)
				{
					DStaticMeshTriangle& outTri = outTris.AddDefault();

					const DFixedArray<int32, 9>& faceInfo = group.FaceIndices[i];
			
					{
						const FVec3& pos = Vertices[faceInfo[0]];
						const FVec2& uv = UVs[faceInfo[1]];
						const FVec3& nor = Normals[faceInfo[2]];

						int32 vert_i = IndexOfVert(outSubMesh, pos, nor, uv);
						if (vert_i >= 0)
						{
							outTri.VertIndex0 = vert_i;
						}
						else
						{
							outPositions.Add(pos);
							outUVs.Emplace(uv.X, 1.0f - uv.Y);
							outNormals.Add(nor);
							outTri.VertIndex0 = outPositions.Length() - 1;
						}
					}
					{
						const FVec3& pos = Vertices[faceInfo[3]];
						const FVec2& uv = UVs[faceInfo[4]];
						const FVec3& nor = Normals[faceInfo[5]];

						int32 vert_i = IndexOfVert(outSubMesh, pos, nor, uv);
						if (vert_i >= 0)
						{
							outTri.VertIndex1 = vert_i;
						}
						else
						{
							outPositions.Add(pos);
							outUVs.Emplace(uv.X, 1.0f - uv.Y);
							outNormals.Add(nor);
							outTri.VertIndex1 = outPositions.Length() - 1;
						}
					}
					{
						const FVec3& pos = Vertices[faceInfo[6]];
						const FVec2& uv = UVs[faceInfo[7]];
						const FVec3& nor = Normals[faceInfo[8]];

						int32 vert_i = IndexOfVert(outSubMesh, pos, nor, uv);
						if (vert_i >= 0)
						{
							outTri.VertIndex2 = vert_i;
						}
						else
						{
							outPositions.Add(pos);
							outUVs.Emplace(uv.X, 1.0f - uv.Y);
							outNormals.Add(nor);
							outTri.VertIndex2 = outPositions.Length() - 1;
						}
					}
				}
			}
			else if (AttributeLayout == EAttributeLayout::VN)
			{
				for (int32 i = 0; i < faceCount; i++)
				{
					DStaticMeshTriangle& outTri = outTris.AddDefault();

					const DFixedArray<int32, 9>& faceInfo = group.FaceIndices[i];

					{
						const FVec3& pos = Vertices[faceInfo[0]];
						const FVec3& nor = Normals[faceInfo[1]];

						int32 vert_i = IndexOfVert(outSubMesh, pos, nor);
						if (vert_i >= 0)
						{
							outTri.VertIndex0 = vert_i;
						}
						else
						{
							outPositions.Add(pos);
							outNormals.Add(nor);
							outTri.VertIndex0 = outPositions.Length() - 1;
						}
					}
					{
						const FVec3& pos = Vertices[faceInfo[2]];
						const FVec3& nor = Normals[faceInfo[3]];

						int32 vert_i = IndexOfVert(outSubMesh, pos, nor);
						if (vert_i >= 0)
						{
							outTri.VertIndex1 = vert_i;
						}
						else
						{
							outPositions.Add(pos);
							outNormals.Add(nor);
							outTri.VertIndex1 = outPositions.Length() - 1;
						}
					}
					{
						const FVec3& pos = Vertices[faceInfo[4]];
						const FVec3& nor = Normals[faceInfo[5]];

						int32 vert_i = IndexOfVert(outSubMesh, pos, nor);
						if (vert_i >= 0)
						{
							outTri.VertIndex2 = vert_i;
						}
						else
						{
							outPositions.Add(pos);
							outNormals.Add(nor);
							outTri.VertIndex2 = outPositions.Length() - 1;
						}
					}
				}
			}
		}
		catch (DIndexException error)
		{
			throw DWavefrontException(error.what());
		}

		outSubMesh.PopulateTangents();
	}

	OutMeshData.UpdateBounds();
}


bool DWavefrontParser::ParseMeshData(
	const DString8& InStr, 
	const DImportOptions& Options, 
	DArray<DStaticMeshData>& OutMeshes
)
{
	try
	{
		this->_Parse(InStr, OutMeshes);
		if (Options.Equals(DImportOptions()) == false)
		{
			for (DStaticMeshData& meshData : OutMeshes)
			{
				meshData.Swizzle(Options.Right, Options.Up, Options.Back, Options.ScaleToMeter);
			}
		}
		return true;
	}
	catch (DWavefrontException error)
	{
		SLog::Error(error.what());
		SLog::Error(error.ErrorPartString);
		return false;
	}
}


bool DWavefrontParser::ParseScene(
	const DString8& InStr, 
	const DImportOptions& Options, 
	shared<HSceneNode> ParentNode, 
	shared<HSurfaceShader> DefaultShader
)
{
	try
	{
		DArray<DStaticMeshData> outMeshesDataItems;
		this->_Parse(InStr, outMeshesDataItems);

		for (DStaticMeshData& meshData : outMeshesDataItems)
		{
			if (Options.Equals(DImportOptions()) == false)
			{
				meshData.Swizzle(Options.Right, Options.Up, Options.Back, Options.ScaleToMeter);
			}

			shared<HStaticMesh> mesh = MakeShared<HStaticMesh>();
			mesh->Load(meshData);
			int32 materialCount = mesh->GetMaterialSlots().Length();
			for (int32 i = 0; i < materialCount; ++i)
			{
				DSurfaceMaterialSettings materialSettings;
				materialSettings.Shader = DefaultShader;
				shared<HSurfaceMaterial> material = MakeShared<HSurfaceMaterial>();
				material->Load(materialSettings);
				mesh->SetMaterialInSlot(i, material);
				mesh->SetMaterialSlotName(i, material->GetName());
			}
			shared<HStaticMeshNode> meshNode = MakeShared<HStaticMeshNode>();
			meshNode->SetMesh(mesh);
			ParentNode->AddChild(meshNode);
		}

		return outMeshesDataItems.IsEmpty() == false;
	}
	catch (DWavefrontException error)
	{
		SLog::Error(error.what());
		SLog::Error(error.ErrorPartString);
		return false;
	}
}
