#include "Engine\pch.h"
#include "DStaticMeshData.h"
#include "Engine\Util\SPrimitiveConvert.h"


// ---------------- DStaticSubMeshData -------------------


void DStaticSubMeshData::Clear()
{
	this->Positions.Clear();
	this->Normals.Clear();
	this->Tangents.Clear();
	this->Colors.Clear();
	this->UVLayers.Clear();
}


void DStaticSubMeshData::PopulateTangents()
{
	if (this->UVLayers.Length() < 1) return;
	const DArray<FVec2>& uvs = this->UVLayers[0].UVs;

	DArray<DArray<FVec3>> TangentsPerVert;
	// populate with empty arrays
	TangentsPerVert.Resize(this->Positions.Length());

	// calculate tangent for every face
	// then assign tangent to every vertex the face is joined to
	for (DStaticMeshTriangle& tri : this->Tris)
	{
		const FVec3& p1 = this->Positions[tri.VertIndex0];
		const FVec3& p2 = this->Positions[tri.VertIndex1];
		const FVec3& p3 = this->Positions[tri.VertIndex2];

		const FVec2& u1 = uvs[tri.VertIndex0];
		const FVec2& u2 = uvs[tri.VertIndex1];
		const FVec2& u3 = uvs[tri.VertIndex2];

		FVec3 edge1 = p2 - p1;
		FVec3 edge2 = p3 - p1;
		FVec2 deltaUV1 = u2 - u1;
		FVec2 deltaUV2 = u3 - u1;

		float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

		FVec3 tangent(
			f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X),
			f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y),
			f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z)
		);

		TangentsPerVert[tri.VertIndex0].Add(tangent);
		TangentsPerVert[tri.VertIndex1].Add(tangent);
		TangentsPerVert[tri.VertIndex2].Add(tangent);
	}

	// average vertex tangents
	for (DArray<FVec3> tangents : TangentsPerVert)
	{
		if (tangents.Length() >= 1)
		{
			FVec3 average;
			for (FVec3& tan : tangents)
			{
				average += tan;
			}
			average /= (float)tangents.Length();
			average.Normalize();
			this->Tangents.Add(average);
		}
		else
		{
			this->Tangents.AddDefault();
			SLog::Error(TX("Found no tangents for vertex while calculating tangents for mesh: ") + this->Name);
		}
	}
}


void DStaticSubMeshData::Swizzle(EAxis FromRight, EAxis FromUp, EAxis FromBack, float ScaleToMeter)
{
	/*right*/   EAxis swizzleX = FromRight;
	/*up*/      EAxis swizzleY = FromUp;
	/*back*/ EAxis swizzleZ = FromBack;

	int32 vertCount = this->Positions.Length();

	if (this->Normals.Length() != vertCount)
	{
		SLog::Error("Mismatching mesh attribute counts while swizzling");
		return;
	}

	for (int32 i = 0; i < vertCount; i++)
	{
		FVec3& pos = this->Positions[i];
		FVec3& nor = this->Normals[i];

		float posX = pos.X;
		float posY = pos.Y;
		float posZ = pos.Z;

		float norX = nor.X;
		float norY = nor.Y;
		float norZ = nor.Z;

		float sposX, sposY, sposZ;
		float snorX, snorY, snorZ;

		switch (swizzleX)
		{
		case EAxis::X:	   sposX =  posX;  snorX =  norX;  break;
		case EAxis::Y:	   sposX =  posY;  snorX =  norY;  break;
		case EAxis::Z:	   sposX =  posZ;  snorX =  norZ;  break;
		case EAxis::NEG_X: sposX = -posX;  snorX = -norX;  break;
		case EAxis::NEG_Y: sposX = -posY;  snorX = -norY;  break;
		case EAxis::NEG_Z: sposX = -posZ;  snorX = -norZ;  break;
		default: sposX = .0f;  snorX = .0f;
		}

		switch (swizzleY)
		{
		case EAxis::X:	   sposY =  posX;  snorY =  norX;  break;
		case EAxis::Y:	   sposY =  posY;  snorY =  norY;  break;
		case EAxis::Z:	   sposY =  posZ;  snorY =  norZ;  break;
		case EAxis::NEG_X: sposY = -posX;  snorY = -norX;  break;
		case EAxis::NEG_Y: sposY = -posY;  snorY = -norY;  break;
		case EAxis::NEG_Z: sposY = -posZ;  snorY = -norZ;  break;
		default: sposY = .0f;  snorY = .0f;
		}

		switch (swizzleZ)
		{
		case EAxis::X:	   sposZ =  posX;  snorZ =  norX;  break;
		case EAxis::Y:	   sposZ =  posY;  snorZ =  norY;  break;
		case EAxis::Z:	   sposZ =  posZ;  snorZ =  norZ;  break;
		case EAxis::NEG_X: sposZ = -posX;  snorZ = -norX;  break;
		case EAxis::NEG_Y: sposZ = -posY;  snorZ = -norY;  break;
		case EAxis::NEG_Z: sposZ = -posZ;  snorZ = -norZ;  break;
		default: sposZ = .0f;  snorZ = .0f;
		}

		sposX *= ScaleToMeter;
		sposY *= ScaleToMeter;
		sposZ *= ScaleToMeter;

		// set data
		pos.X = sposX;
		pos.Y = sposY;
		pos.Z = sposZ;

		nor.X = snorX;
		nor.Y = snorY;
		nor.Z = snorZ;
	}	
}


void DStaticSubMeshData::Scale(float ScaleToMeter)
{
	for (FVec3& pos : this->Positions)
	{
		pos *= ScaleToMeter;
	}
}


FAxisBox DStaticSubMeshData::CalcBoundingBox() const
{
	float minX =  FLT_MAX;
	float minY =  FLT_MAX;
	float minZ =  FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	float maxZ = -FLT_MAX;

	for (const FVec3& p : this->Positions)
	{
			 if (p.X < minX) minX = p.X;
		else if (p.X > maxX) maxX = p.X;

			 if (p.Y < minY) minY = p.Y;
		else if (p.Y > maxY) maxY = p.Y;

			 if (p.Z < minZ) minZ = p.Z;
		else if (p.Z > maxZ) maxZ = p.Z;
	}

	return FAxisBox( 
		minX, minY, minZ,
		maxX, maxY, maxZ
	);
}


bool DStaticSubMeshData::DetectMirroredUVs(int32 Index) const
{
	if (this->UVLayers.IsValidIndex(Index) == false) return false;
	const DArray<FVec2>& uvs = this->UVLayers[Index].UVs;

	int32 mirroredFaceCount = 0;
	int32 triCount = this->Tris.Length();

	for (const DStaticMeshTriangle& tri : this->Tris)
	{
		// check, if winding order is counter-clockwise:
		// in modelling apps clockwise is mirrored but OpenGL needs flipped y texture coords
		// sum over the edges is negative (again, normally positive would be bad)
		// sum of (x2 - x1) * (y2 + y1) for every vertex of a face
		const FVec2& a = uvs[tri.VertIndex0];
		const FVec2& b = uvs[tri.VertIndex1];
		const FVec2& c = uvs[tri.VertIndex2];

		float sum = .0f;
		sum += (b.X - a.X) * (b.Y + a.Y);
		sum += (c.X - b.X) * (c.Y + b.Y);
		sum += (a.X - c.X) * (a.Y + c.Y);

		if (sum < .0f)
		{
			return true;  // mirrored UVs found
		}
	}
	return false;
}


void DStaticSubMeshData::ToStaticSubMeshAssetData(DStaticSubMeshAssetData& OutData) const
{
	OutData.Clear();
	
	int32 vertCount = this->Positions.Length();

	DArray<float>& outPositions = OutData.Positions;
	DArray<int16>& outNormals = OutData.Normals;
	DArray<int16>& outUVs = OutData.UVs;
	DArray<int16>& outTangents = OutData.Tangents;
	DArray<uint8>& outColors = OutData.Colors;
	DArray<int16>& outUVs_2 = OutData.UVs_2;
	
	outPositions.Reserve(vertCount * 3);
	outNormals.Reserve(vertCount * 3);
	outUVs.Reserve(vertCount * 2);
	outTangents.Reserve(vertCount * 3);
	outColors.Reserve(vertCount * 4);
	outUVs_2.Reserve(vertCount * 2);

	for (int32 i = 0; i < vertCount; ++i)
	{
		const FVec3& p = this->Positions[i];
		outPositions.Add(p.X);
		outPositions.Add(p.Y);
		outPositions.Add(p.Z);

		const FVec3& n = this->Normals[i];
		outNormals.Add(SPrimitiveConvert::UnitVectorToUInt16(n.X));
		outNormals.Add(SPrimitiveConvert::UnitVectorToUInt16(n.Y));
		outNormals.Add(SPrimitiveConvert::UnitVectorToUInt16(n.Z));
	}

	if (this->Tangents.Length() > 0)
	{
		for (int32 i = 0; i < vertCount; ++i)
		{
			const FVec3& t = this->Tangents[i];
			outTangents.Add(SPrimitiveConvert::UnitVectorToUInt16(t.X));
			outTangents.Add(SPrimitiveConvert::UnitVectorToUInt16(t.Y));
			outTangents.Add(SPrimitiveConvert::UnitVectorToUInt16(t.Z));
		}
	}

	if (this->Colors.Length() > 0)
	{
		for (int32 i = 0; i < vertCount; ++i)
		{
			const FVec4& c = this->Colors[i];
			outColors.Add((uint8)(c.X * UINT8_MAX));
			outColors.Add((uint8)(c.Y * UINT8_MAX));
			outColors.Add((uint8)(c.Z * UINT8_MAX));
			outColors.Add((uint8)(c.W * UINT8_MAX));
		}
	}

	if (this->UVLayers.IsValidIndex(0))
	{
		const DArray<FVec2>& uvs = this->UVLayers[0].UVs;
		for (int32 i = 0; i < vertCount; ++i)
		{
			const FVec2& u = uvs[i];
			outUVs.Add(SPrimitiveConvert::FloatToHalfFloat(u.X));
			outUVs.Add(SPrimitiveConvert::FloatToHalfFloat(u.Y));
		}
	}

	if (this->UVLayers.IsValidIndex(1))
	{
		const DArray<FVec2>& uvs = this->UVLayers[1].UVs;
		for (int32 i = 0; i < vertCount; ++i)
		{
			const FVec2& u = uvs[i];
			outUVs.Add(SPrimitiveConvert::FloatToHalfFloat(u.X));
			outUVs.Add(SPrimitiveConvert::FloatToHalfFloat(u.Y));
		}
	}

	DArray<uint32>& outFaceIndices = OutData.FaceIndices;
	outFaceIndices.Clear();
	outFaceIndices.Reserve(this->Tris.Length() * 3);
	for (const DStaticMeshTriangle& tri : this->Tris)
	{
		outFaceIndices.Add(tri.VertIndex0);
		outFaceIndices.Add(tri.VertIndex1);
		outFaceIndices.Add(tri.VertIndex2);
	}

	OutData.Name = this->Name;
}


// ----------------- DStaticMeshData ------------------


void DStaticMeshData::ToStaticMeshAssetData(DStaticMeshAssetData& OutData) const
{
	OutData.SubMeshes.Clear();
	OutData.BoundingBox = this->BoundingBox;
	for (const DStaticSubMeshData& submesh : this->SubMeshes)
	{
		submesh.ToStaticSubMeshAssetData( OutData.SubMeshes.AddDefault() );
	}
}


void DStaticMeshData::UpdateBounds()
{
	this->BoundingBox = FAxisBox();
	for (DStaticSubMeshData& submesh : this->SubMeshes)
	{
		this->BoundingBox.Extend( submesh.CalcBoundingBox() );
	}
}


void DStaticMeshData::Swizzle(EAxis FromRight, EAxis FromUp, EAxis FromForward, float ScaleToMeter)
{
	for (DStaticSubMeshData& submesh : this->SubMeshes)
	{
		submesh.Swizzle(FromRight, FromUp, FromForward, ScaleToMeter);
	}
}


void DStaticMeshData::Scale(float ScaleToMeter)
{
	for (DStaticSubMeshData& submesh : this->SubMeshes)
	{
		submesh.Scale(ScaleToMeter);
	}
}


void DStaticMeshData::Clear()
{
	for (DStaticSubMeshData& submesh : this->SubMeshes)
	{
		submesh.Clear();
	}
}


bool DStaticMeshData::DetectMirroredUVs()
{
	for (const DStaticSubMeshData& submesh : this->SubMeshes)
	{
		for (int32 i = 0; i < submesh.UVLayers.Length(); ++i)
		{
			if (submesh.DetectMirroredUVs(i)) return true;
		}
	}
	return false;
}
