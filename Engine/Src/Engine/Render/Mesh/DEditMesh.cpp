#include "Engine/pch.h"
#include "DEditMesh.h"


int32 DEditMeshTriangle::GetVertByIndex(int32 Index) const
{
	switch (Index)
	{
	case 0: return this->VertIndex0;
	case 1: return this->VertIndex1;
	case 2: return this->VertIndex2;
	default: 
		SLog::Error(
			DString::Format(
				TX("EditStaticTriangle::GetVertByIndex Index out of length: index: {0}, length: {1}"),
				{ Index, 3 }
		));
	}
}


void DEditMeshTriangle::SetVertByIndex(int32 Index, int32 Value)
{
	switch (Index)
	{
	case 0: this->VertIndex0 = Value; break;
	case 1: this->VertIndex1 = Value; break;
	case 2: this->VertIndex2 = Value; break;
	default: 
		SLog::Error(
			DString::Format(
				TX("EditStaticTriangle::SetVertByIndex Index out of length: index: {0}, length: {1}"),
				{ Index, 3 }
		));
	}
}


int32 DEditMeshTriangle::FindThirdVertIndex(int32 Index1, int32 Index2) const
{
	if (this->VertIndex0 == Index1)
	{
		return (this->VertIndex1 == Index2)? this->VertIndex2 : this->VertIndex1;
	}
	else if (this->VertIndex1 == Index1)
	{
		return (this->VertIndex0 == Index2)? this->VertIndex2 : this->VertIndex0;
	}
	else
	{
		return (this->VertIndex0 == Index2)? this->VertIndex1 : this->VertIndex0;
	}
}




float LineLineIntersectMargin = .001f;


void LineLineIntersect(
	const FVec3& A, const FVec3& B, const FVec3& C, const FVec3& D, 
	FVec3& OutP0, FVec3& OutP1, bool& OutOnSegment, bool& OutIntersects
)
{
	FVec3 r = B - A;
	FVec3 s = D - C;
	FVec3 q = A - C;

	float dot_qr = q.Dot(r);
	float dot_qs = q.Dot(s);
	float dot_rs = r.Dot(s);
	float dot_rr = r.Dot(r);
	float dot_ss = s.Dot(s);

	float denom = dot_rr * dot_ss - dot_rs * dot_rs;
	float numer = dot_qs * dot_rs - dot_qr * dot_ss;

	float t = numer / denom;
	float u = (dot_qs + t * dot_rs) / dot_ss;

	OutP0 = A + r * t;
	OutP1 = C + s * u;

	OutOnSegment = .0f <= t && t <= 1.0f && .0f <= u && u <= 1.0f;
	OutIntersects = (OutP0 - OutP1).Length() <= LineLineIntersectMargin;
}


FVec3 CalcTriNormal(const FVec3& VertPos0, const FVec3& VertPos1, const FVec3& VertPos2)
{
	FVec3 a = VertPos1 - VertPos0;
	FVec3 b = VertPos2 - VertPos0;
	FVec3 faceNormal (
		a.Y * b.Z - a.Z * b.Y,
		a.Z * b.X - a.X * b.Z,
		a.X * b.Y - a.Y * b.X
	);
	faceNormal.Normalize();
	return faceNormal;
}


FMat4 QuadricFromPlane(const FPlane& P)
{
	return FMat4(
		P.A * P.A, P.A * P.B, P.A * P.C, P.A * P.D,
		P.A * P.B, P.B * P.B, P.B * P.C, P.B * P.D,
		P.A * P.C, P.B * P.C, P.C * P.C, P.C * P.D,
		P.A * P.D, P.B * P.D, P.C * P.D, P.D * P.D
	);
}


const float BOUNDARY_QUADRIC_WEIGHT = 100.0f;


float TriangleArea(const FVec3& A, const FVec3& B, const FVec3& C)
{
	// Heron's formula
	float a = (B - A).Length();
	float b = (C - B).Length();
	float c = (A - C).Length();
	float s = (a + b + c) * .5f;
	return FMath::Sqrt(s * (s - a) * (s - b) * (s - c));
}


struct ConnectedFace
{
	int32 FaceIndex = 0;
	int8 FaceVertIndex = 0;

	bool operator==(const ConnectedFace& Other)
	{
		return this->FaceIndex == Other.FaceIndex && this->FaceVertIndex == Other.FaceVertIndex;
	}
	bool operator!=(const ConnectedFace& Other)
	{
		return this->FaceIndex != Other.FaceIndex || this->FaceVertIndex != Other.FaceVertIndex;
	}
};


struct VertInfo
{
	DArray<ConnectedFace> ConnectedFaces;
	DArray<int32> ConnectedPairs;
	FMat4 Quadric;
	bool Boundary = false;

	ConnectedFace* FindConnectedFaceByIndex(int32 Index)
	{
		for (ConnectedFace& faceVert : this->ConnectedFaces)
		{
			if (faceVert.FaceIndex == Index)
			{
				return &faceVert;
			}
		}
		return nullptr;
	}

	bool RemoveConnectedFaceByIndex(int32 Index)
	{
		int32 len = this->ConnectedFaces.Length();
		for (int32 i = 0; i < len; ++i)
		{
			if (this->ConnectedFaces[i].FaceIndex == Index)
			{
				this->ConnectedFaces.Remove(i);
				return true;
			}
		}
		return false;
	}

	void CalcQuadric(const DArray<DEditMeshTriangle>& Tris, const DArray<FVec3>& Positions)
	{
		this->Quadric = FMat4(.0f);  // default constructor is identity matrix
		for (ConnectedFace& connectedFace : this->ConnectedFaces)
		{
			const DEditMeshTriangle& face = Tris[connectedFace.FaceIndex];
			const FVec3& pos0 = Positions[face.VertIndex0];
			const FVec3& pos1 = Positions[face.VertIndex1];
			const FVec3& pos2 = Positions[face.VertIndex2];
			FVec3 faceNormal = CalcTriNormal(pos0, pos1, pos2);
			FPlane p = FPlane::FromPointAndNormal(pos0, faceNormal);
			this->Quadric += QuadricFromPlane(p);
		}
	}
};


struct VertPair
{
	int32 VertIndex0 = 0;
	int32 VertIndex1 = 0;
	float Cost = .0f;
	int32 Generation = 0;
	FVec3 Normal;

	FVec3 CalcContractedPos(const DArray<FVec3>& Positions, const DArray<VertInfo>& VertInfos)
	{
		const FVec3& pos0 = Positions[this->VertIndex0];
		const FVec3& pos1 = Positions[this->VertIndex1];

		const FMat4& q0 = VertInfos[this->VertIndex0].Quadric;
		const FMat4& q1 = VertInfos[this->VertIndex1].Quadric;
		FMat3 q = q0.ToMat3() + q1.ToMat3();
		FVec3 ql = q0.ToTranslation() + q1.ToTranslation();
		FMat3 invertQ;
		if (q.TryInvert(/*out*/invertQ))
		{
			return -ql * invertQ;
		}
		else
		{
			return (pos0 + pos1) * .5f;
		}
	}

	FVec3 CalcContractedPosFallback(const DArray<FVec3>& Positions, const DArray<VertInfo>& VertInfos)
	{
		const FVec3& pos0 = Positions[this->VertIndex0];
		const FVec3& pos1 = Positions[this->VertIndex1];

		return (pos0 + pos1) * .5f;
	}

private:

	static float CalcQuadricCost(const FMat4& Quadric, const FVec3& VertPos)
	{
		float vXX = VertPos.X * VertPos.X;
		float vXY = VertPos.X * VertPos.Y; 
		float vXZ = VertPos.X * VertPos.Z;
		float vYY = VertPos.Y * VertPos.Y;
		float vYZ = VertPos.Y * VertPos.Z;
		float vZZ = VertPos.Z * VertPos.Z;

		auto& m = Quadric.Data;
		return
			(m[0][0] * vXX) + (m[0][1] * 2 * vXY) + (m[0][2] * 2 * vXZ) + (m[0][3] * 2 * VertPos.X) + 
			(m[1][1] * vYY) + (m[1][2] * 2 * vYZ) + (m[1][3] * 2 * VertPos.Y) +
			(m[2][2] * vZZ) + (m[2][3] * 2 * VertPos.Z) + 
			m[3][3];
	}

public:

	void CalcCost(const DArray<VertInfo>& VertInfos, const FVec3& ContractedPos)
	{
		this->Cost = 
			CalcQuadricCost(VertInfos[this->VertIndex0].Quadric, ContractedPos) +
			CalcQuadricCost(VertInfos[this->VertIndex1].Quadric, ContractedPos) +
			this->Generation * 1000.0f;
	}

	bool IsBoundary(const DArray<VertInfo>& VertInfos)
	{
		return VertInfos[this->VertIndex0].Boundary && VertInfos[this->VertIndex1].Boundary;
	}
};


enum class ESeamMergeMode { NONE, NON_SEAM_TO_SEAM, SEAM_TO_SEAM };


//DEditSubMesh DEditSubMesh::Simplified() const
//{
//	DEditSubMesh mesh = *this;
//
//	int32 vertCount = this->Positions.Length();
//
//	DArray<DArray<int32>> clusteredVerts;
//	clusteredVerts.Resize(vertCount);
//	for (int32 vert_i = 0; vert_i < vertCount; ++vert_i)
//	{
//		for (int32 otherVert_i = 0; otherVert_i < vertCount; ++otherVert_i)
//		{
//			if (vert_i != otherVert_i && 
//				this->Positions[vert_i] == this->Positions[otherVert_i])
//			{
//				clusteredVerts[vert_i].Add(otherVert_i);
//			}
//		}
//	}
//
//	DArray<VertInfo> vertInfos;
//	vertInfos.Resize(mesh.Positions.Length());
//	int32 faceCount = mesh.Tris.Length();
//	for (int32 face_i = 0; face_i < faceCount; ++face_i)
//	{
//		const DEditMeshTriangle& tri = mesh.Tris[face_i];
//		ConnectedFace& inf0 = vertInfos[tri.VertIndex0].ConnectedFaces.AddDefault();
//		inf0.FaceIndex = face_i;
//		inf0.FaceVertIndex = 0;
//		ConnectedFace& inf1 = vertInfos[tri.VertIndex1].ConnectedFaces.AddDefault();
//		inf1.FaceIndex = face_i;
//		inf1.FaceVertIndex = 1;
//		ConnectedFace& inf2 = vertInfos[tri.VertIndex2].ConnectedFaces.AddDefault();
//		inf2.FaceIndex = face_i;
//		inf2.FaceVertIndex = 2;
//	}
//
//	//for (VertInfo& vertInfo : vertInfos)
//	//{
//	//	vertInfo.CalcQuadric(mesh.Tris, mesh.Verts);
//	//}
//
//	DArray<VertPair> pairs;
//
//	for (const DEditMeshTriangle& tri : mesh.Tris)
//	{
//		{
//			bool alreadyRegistered = false;
//			for (VertPair& pair : pairs)
//			{
//				if ((pair.VertIndex0 == tri.VertIndex0 && pair.VertIndex1 == tri.VertIndex1)
//					||
//					(pair.VertIndex1 == tri.VertIndex0 && pair.VertIndex0 == tri.VertIndex1))
//				{
//					alreadyRegistered = true;
//					break;
//				}
//			}
//
//			if (alreadyRegistered == false)
//			{
//				VertPair& pair = pairs.AddDefault();
//				pair.VertIndex0 = tri.VertIndex0;
//				pair.VertIndex1 = tri.VertIndex1;
//
//				//pair.CalcCost(vertInfos, pair.CalcContractedPos(mesh.Verts, vertInfos));
//
//				vertInfos[tri.VertIndex0].ConnectedPairs.Add(pairs.Length() - 1);
//				vertInfos[tri.VertIndex1].ConnectedPairs.Add(pairs.Length() - 1);
//			}
//		}
//		{
//			bool alreadyRegistered = false;
//			for (VertPair& pair : pairs)
//			{
//				if ((pair.VertIndex0 == tri.VertIndex1 && pair.VertIndex1 == tri.VertIndex2)
//					||
//					(pair.VertIndex1 == tri.VertIndex1 && pair.VertIndex0 == tri.VertIndex2))
//				{
//					alreadyRegistered = true;
//					break;
//				}
//			}
//
//			if (alreadyRegistered == false)
//			{
//				VertPair& pair = pairs.AddDefault();
//				pair.VertIndex0 = tri.VertIndex1;
//				pair.VertIndex1 = tri.VertIndex2;
//
//				//pair.CalcCost(vertInfos, pair.CalcContractedPos(mesh.Verts, vertInfos));
//
//				vertInfos[tri.VertIndex1].ConnectedPairs.Add(pairs.Length() - 1);
//				vertInfos[tri.VertIndex2].ConnectedPairs.Add(pairs.Length() - 1);
//			}
//		}
//		{
//			bool alreadyRegistered = false;
//			for (VertPair& pair : pairs)
//			{
//				if ((pair.VertIndex0 == tri.VertIndex2 && pair.VertIndex1 == tri.VertIndex0)
//					||
//					(pair.VertIndex1 == tri.VertIndex2 && pair.VertIndex0 == tri.VertIndex0))
//				{
//					alreadyRegistered = true;
//					break;
//				}
//			}
//
//			if (alreadyRegistered == false)
//			{
//				VertPair& pair = pairs.AddDefault();
//				pair.VertIndex0 = tri.VertIndex2;
//				pair.VertIndex1 = tri.VertIndex0;
//
//				//pair.CalcCost(vertInfos, pair.CalcContractedPos(mesh.Verts, vertInfos));
//
//				vertInfos[tri.VertIndex2].ConnectedPairs.Add(pairs.Length() - 1);
//				vertInfos[tri.VertIndex0].ConnectedPairs.Add(pairs.Length() - 1);
//			}
//		}
//	}
//
//	// mark boundary verts
//	{
//		DArray<int32> commonFaces;
//
//		for (VertPair& pair : pairs)
//		{
//			int32 keptVertIndex = pair.VertIndex0;
//			int32 deletedVertIndex = pair.VertIndex1;
//
//			VertInfo& keptVertInfo = vertInfos[keptVertIndex];
//			VertInfo& deletedVertInfo = vertInfos[deletedVertIndex];
//
//			commonFaces.Clear();
//
//			for (ConnectedFace& connectedFace : deletedVertInfo.ConnectedFaces)
//			{
//				// if new common face
//				if (keptVertInfo.FindConnectedFaceByIndex(connectedFace.FaceIndex) &&
//					commonFaces.Contains(connectedFace.FaceIndex) == false)
//				{
//					commonFaces.Add(connectedFace.FaceIndex);
//				}
//			}
//
//			if (commonFaces.Length() == 1)
//			{
//				vertInfos[pair.VertIndex0].Boundary = true;
//				vertInfos[pair.VertIndex1].Boundary = true;
//				DEditMeshTriangle& face = mesh.Tris[commonFaces[0]];
//				pair.Normal = CalcTriNormal(
//					mesh.Positions[face.VertIndex0],
//					mesh.Positions[face.VertIndex1],
//					mesh.Positions[face.VertIndex2]
//				);
//			}
//		}
//	}
//
//	DArray<FVec3>& outPositions = mesh.Positions;
//	DArray<FVec3>& outNormals = mesh.Normals;
//	DArray<FVec3>& outTangents = mesh.Tangents;
//	DArray<FVec4>& outColors = mesh.Colors;
//	DArray<DEditMeshUVLayer>& outUVLayers = mesh.UVLayers;
//	DArray<DEditMeshTriangle>& outFaces = mesh.Tris;
//
//	DArray<bool> seams;
//	seams.Reserve(outPositions.Length());
//
//	for (int32 pos_i = 0; pos_i < outPositions.Length(); ++pos_i)
//	{
//		int32 found_i = -1;
//		for (int32 search_i = 0; search_i < outPositions.Length(); ++search_i)
//		{
//			if (search_i != pos_i && outPositions[search_i] == outPositions[pos_i])
//			{
//				found_i = search_i;
//				break;
//			}
//		}
//		seams.Add(found_i >= 0);
//	}
//
//	DArray<int32> sortedPairs;
//	sortedPairs.Reserve(pairs.Length());
//	for (int32 i = 0; i < pairs.Length(); ++i) sortedPairs.Add(i);
//
//	float simplifyRatio = .1f;
//	int32 targetTriCount = (int32)(mesh.Tris.Length() * simplifyRatio);
//
//	DSet<int32> removedFaces;
//	removedFaces.Reserve(outFaces.Length());
//
//	//for (VertInfo& vertInfo : vertInfos) vertInfo.CalcQuadric(outFaces, outVerts);
//	//for (VertPair& pair : pairs) pair.CalcCost(vertInfos, pair.CalcContractedPos(outVerts, vertInfos), outVerts);
//	//sortedPairs.Sort([&pairs](int32 a, int32 b) { return pairs[a].Cost > pairs[b].Cost; });
//
//	SLog::Print("---- Start ----");
//
//	while (outFaces.Length() - removedFaces.Length() > targetTriCount &&
//		sortedPairs.Length() > 0)
//	{
//		for (VertInfo& vertInfo : vertInfos) vertInfo.CalcQuadric(outFaces, outPositions);
//		for (VertPair& pair : pairs) pair.CalcCost(vertInfos, pair.CalcContractedPos(outPositions, vertInfos));
//		sortedPairs.Sort([&pairs](int32 a, int32 b) { return pairs[a].Cost > pairs[b].Cost; });
//
//		// preserve boundaries
//		for (VertPair& pair : pairs)
//		{
//			VertInfo& vertInfo0 = vertInfos[pair.VertIndex0];
//			VertInfo& vertInfo1 = vertInfos[pair.VertIndex1];
//
//			// is boundary edge
//			if (vertInfo0.Boundary && vertInfo1.Boundary)
//			{
//				FVec3& pos0 = outPositions[pair.VertIndex0];
//				FVec3& pos1 = outPositions[pair.VertIndex1];
//				FVec3 edgeDir = pos1 - pos0;
//				edgeDir.Normalize();
//				FVec3 cross = edgeDir.Cross(pair.Normal);
//				cross.Normalize();
//				FPlane perpendicularPlane = FPlane::FromPointAndNormal(
//					(pos0 + pos1) * .5f, 
//					cross
//				);
//				FMat4 q = QuadricFromPlane(perpendicularPlane);
//				q *= BOUNDARY_QUADRIC_WEIGHT;
//
//				vertInfos[pair.VertIndex0].Quadric += q;
//				vertInfos[pair.VertIndex1].Quadric += q;
//			}
//		}
//
//		int32 pair_i;
//		sortedPairs.RemoveGetLast(pair_i);
//		VertPair& pair = pairs[pair_i];
//
//		int32 keptVertIndex = pair.VertIndex0;
//		int32 deletedVertIndex = pair.VertIndex1;
//
//		if (vertInfos[deletedVertIndex].Boundary && vertInfos[keptVertIndex].Boundary == false ||
//			seams[deletedVertIndex] && seams[keptVertIndex] == false)
//		{
//			keptVertIndex = pair.VertIndex1;
//			deletedVertIndex = pair.VertIndex0;
//		}
//
//		VertInfo& keptVertInfo = vertInfos[keptVertIndex];
//		VertInfo& deletedVertInfo = vertInfos[deletedVertIndex];
//
//		// categorize connected faces
//		DArray<int32> commonFaces;
//		DArray<ConnectedFace> keptVertFaces;
//		DArray<ConnectedFace> deletedVertFaces;
//		DArray<int32> commonFaceThirdVerts;
//
//		for (ConnectedFace& connectedFace : keptVertInfo.ConnectedFaces)
//		{
//			// if not a common face
//			if (deletedVertInfo.FindConnectedFaceByIndex(connectedFace.FaceIndex) == nullptr)
//			{
//				keptVertFaces.Add(connectedFace);
//			}
//		}
//
//		for (ConnectedFace& connectedFace : deletedVertInfo.ConnectedFaces)
//		{
//			// if new common face
//			if (keptVertInfo.FindConnectedFaceByIndex(connectedFace.FaceIndex))
//			{
//				if (commonFaces.Contains(connectedFace.FaceIndex) == false)
//				{
//					commonFaces.Add(connectedFace.FaceIndex);
//				}
//				DEditMeshTriangle& face = outFaces[connectedFace.FaceIndex];
//				commonFaceThirdVerts.Add(face.FindThirdVertIndex(keptVertIndex, deletedVertIndex));
//			}
//			else
//			{
//				deletedVertFaces.Add(connectedFace);
//			}
//		}
//
//		FVec3 contractedPos;
//		bool incrementGeneration;
//		ESeamMergeMode seamMergeMode;
//
//		if (keptVertInfo.Boundary && deletedVertInfo.Boundary == false ||
//			seams[keptVertIndex] && seams[deletedVertIndex] == false)
//		{
//			contractedPos = outPositions[keptVertIndex];
//			incrementGeneration = true;
//			seamMergeMode = ESeamMergeMode::NON_SEAM_TO_SEAM;
//		}
//		else if (deletedVertInfo.Boundary && keptVertInfo.Boundary == false ||
//			seams[deletedVertIndex] && seams[keptVertIndex] == false)
//		{
//			contractedPos = outPositions[deletedVertIndex];
//			incrementGeneration = true;
//			seamMergeMode = ESeamMergeMode::NON_SEAM_TO_SEAM;
//		}
//		else if (keptVertInfo.Boundary && deletedVertInfo.Boundary)
//		{
//			contractedPos = pair.CalcContractedPos(outPositions, vertInfos);
//			incrementGeneration = true;
//			seamMergeMode = ESeamMergeMode::SEAM_TO_SEAM;
//		}
//		else
//		{
//			if (pair.Cost >= 1.0f)
//			{
//				contractedPos = pair.CalcContractedPos(outPositions, vertInfos);
//				incrementGeneration = false;
//			}
//			else
//			{
//				contractedPos = pair.CalcContractedPosFallback(outPositions, vertInfos);
//				incrementGeneration = true;
//			}
//			seamMergeMode = ESeamMergeMode::NONE;
//		}
//
//		for (ConnectedFace& face : deletedVertFaces)
//		{
//			FVec3 vertPos0 = outPositions[outFaces[face.FaceIndex].VertIndex0];
//			FVec3 vertPos1 = outPositions[outFaces[face.FaceIndex].VertIndex1];
//			FVec3 vertPos2 = outPositions[outFaces[face.FaceIndex].VertIndex2];
//			FVec3 normalBefore = CalcTriNormal(vertPos0, vertPos1, vertPos2);
//
//			if (face.FaceVertIndex == 0) vertPos0 = contractedPos;
//			else if (face.FaceVertIndex == 1) vertPos1 = contractedPos;
//			else vertPos2 = contractedPos;
//			FVec3 normalAfter = CalcTriNormal(vertPos0, vertPos1, vertPos2);
//
//			// could also re-add the pair with an incremented generation
//			if (normalBefore.Dot(normalAfter) < -.0f) goto endOfMainCycle;
//		}
//
//		for (ConnectedFace& face : keptVertFaces)
//		{
//			FVec3 vertPos0 = outPositions[outFaces[face.FaceIndex].VertIndex0];
//			FVec3 vertPos1 = outPositions[outFaces[face.FaceIndex].VertIndex1];
//			FVec3 vertPos2 = outPositions[outFaces[face.FaceIndex].VertIndex2];
//			FVec3 normalBefore = CalcTriNormal(vertPos0, vertPos1, vertPos2);
//
//			if (face.FaceVertIndex == 0) vertPos0 = contractedPos;
//			else if (face.FaceVertIndex == 1) vertPos1 = contractedPos;
//			else vertPos2 = contractedPos;
//			FVec3 normalAfter = CalcTriNormal(vertPos0, vertPos1, vertPos2);
//
//			// could also re-add the pair with an incremented generation
//			if (normalBefore.Dot(normalAfter) < -.0f) goto endOfMainCycle;
//		}
//
//		{
//			for (int32 i = 0; i < commonFaces.Length(); ++i)
//			{
//				int32 face_i = commonFaces[i];
//				bool ret;
//
//				ret = vertInfos[commonFaceThirdVerts[i]].RemoveConnectedFaceByIndex(face_i);
//				if (!ret) SLog::PrintList({ "1................ Could not remove face" });
//
//				ret = keptVertInfo.RemoveConnectedFaceByIndex(face_i);
//				if (!ret) SLog::PrintList({ "2................ Could not remove face" });
//			}
//
//			for (ConnectedFace& connectedFace : deletedVertFaces)
//			{
//				outFaces[connectedFace.FaceIndex].SetVertByIndex(connectedFace.FaceVertIndex, keptVertIndex);
//				keptVertInfo.ConnectedFaces.Add(connectedFace);
//			}
//
//			DArray<int32>& deletedPairIndices = deletedVertInfo.ConnectedPairs;
//			DArray<int32>& keptPairIndices = keptVertInfo.ConnectedPairs;
//
//			for (int32 dpi : deletedPairIndices)
//			{
//				VertPair& deletedVertPair = pairs[dpi];
//				if (deletedVertPair.VertIndex0 == deletedVertIndex)
//				{
//					if (commonFaceThirdVerts.Contains(deletedVertPair.VertIndex1)) 
//					{
//						int32 fi = sortedPairs.IndexOf(dpi);
//						if (fi >= 0) sortedPairs.Remove(fi);
//					}
//					else
//					{
//						deletedVertPair.VertIndex0 = keptVertIndex;
//						if (&deletedPairIndices != &keptPairIndices)
//						{
//							keptPairIndices.Add(dpi);
//						}
//					}
//				}
//				else if (deletedVertPair.VertIndex1 == deletedVertIndex)
//				{
//					if (commonFaceThirdVerts.Contains(deletedVertPair.VertIndex0)) 
//					{
//						int32 fi = sortedPairs.IndexOf(dpi);
//						if (fi >= 0) sortedPairs.Remove(fi);
//					}
//					else
//					{
//						deletedVertPair.VertIndex1 = keptVertIndex;
//						if (&deletedPairIndices != &keptPairIndices)
//						{
//							keptPairIndices.Add(dpi);
//						}
//					}
//				}
//			}
//
//			if (incrementGeneration)
//			{
//				for (int32 keptPair_i : keptVertInfo.ConnectedPairs)
//				{
//					pairs[keptPair_i].Generation++;
//				}
//			}
//
//			outPositions[keptVertIndex] = contractedPos;
//
//			for (int32 vert_i : clusteredVerts[deletedVertIndex])
//			{
//				if (clusteredVerts[keptVertIndex].Contains(vert_i) == false)
//				{
//					clusteredVerts[keptVertIndex].Add(vert_i);
//				}
//			}
//
//			for (int32 vert_i : clusteredVerts[keptVertIndex])
//			{
//				outPositions[vert_i] = contractedPos;			
//			}
//
//
//			FVec3& normal = outNormals[keptVertIndex];
//			normal += outNormals[deletedVertIndex];
//			normal *= .5f;
//			normal.Normalize();
//
//			if (outTangents.Length() > 0)
//			{
//				FVec3& tangent = outTangents[keptVertIndex];
//				tangent += outTangents[deletedVertIndex];
//				tangent *= .5f;
//				tangent.Normalize();
//			}
//
//			if (outColors.Length() > 0)
//			{
//				FVec4& color = outColors[keptVertIndex];
//				color += outColors[deletedVertIndex];
//				color *= .5f;
//			}
//
//			for (DEditMeshUVLayer& uvLayer : outUVLayers)
//			{
//				FVec2& uv = uvLayer.UVs[keptVertIndex];
//				uv += uvLayer.UVs[deletedVertIndex];
//				uv *= .5f;
//			}
//
//			for (int32 face_i : commonFaces) removedFaces.Add(face_i);
//		}
//
//	endOfMainCycle: { }
//	}
//
//	SLog::PrintList({ "remaining pairs:", sortedPairs.Length() });
//
//	outFaces.RemoveIf([&removedFaces](DEditMeshTriangle& v, int32 index){ return removedFaces.Contains(index); });
//
//	// remove degenerate faces
//	removedFaces.Clear();
//
//	int32 l = outFaces.Length();
//	for (int32 face_i = 0; face_i < l; ++face_i)
//	{
//		DEditMeshTriangle& face = outFaces[face_i];
//		float area = TriangleArea(
//			outPositions[face.VertIndex0], 
//			outPositions[face.VertIndex1], 
//			outPositions[face.VertIndex2]
//		);
//		if (area == .0f)
//		{
//			removedFaces.Add(face_i);
//		}
//	}
//
//	outFaces.RemoveIf([&removedFaces](DEditMeshTriangle& v, int32 index){ return removedFaces.Contains(index); });
//
//	return mesh;
//}


//DEditMesh DEditMesh::Simplified() const
//{
//	DEditMesh outMesh;
//	outMesh.SubMeshes.Reserve(this->SubMeshes.Length());
//	for (const DEditSubMesh& submesh : this->SubMeshes)
//	{
//		outMesh.SubMeshes.Add(submesh.Simplified());
//	}
//	return outMesh;
//}


DStaticMeshData DEditMesh::ToStaticMeshData()
{
	DStaticMeshData mesh;
	mesh.Name = this->Name;
	for (DEditSubMesh& editSubmesh : this->SubMeshes)
	{
		DStaticSubMeshData& submesh = mesh.SubMeshes.AddDefault();
		submesh.Positions = editSubmesh.Positions;
		submesh.Normals = editSubmesh.Normals;
		submesh.Tangents = editSubmesh.Tangents;
		submesh.Colors = editSubmesh.Colors;
		for (DEditMeshUVLayer& editUVLayer : editSubmesh.UVLayers)
		{
			DStaticMeshUVLayer& uvLayer = submesh.UVLayers.AddDefault();
			uvLayer.UVs = editUVLayer.UVs;
		}
		DArray<DStaticMeshTriangle>& tris = submesh.Tris;
		tris.Reserve(editSubmesh.Tris.Length());
		for (DEditMeshTriangle& editTree : editSubmesh.Tris)
		{
			DStaticMeshTriangle& tri = tris.AddDefault();
			tri.VertIndex0 = editTree.VertIndex0;
			tri.VertIndex1 = editTree.VertIndex1;
			tri.VertIndex2 = editTree.VertIndex2;
		}
	}
	return mesh;
}


DEditMesh DEditMesh::FromStaticMeshData(DStaticMeshData& StaticMeshData)
{
	DEditMesh editMesh;
	editMesh.Name = StaticMeshData.Name;
	for (DStaticSubMeshData& submesh : StaticMeshData.SubMeshes)
	{
		DEditSubMesh& editSubmesh = editMesh.SubMeshes.AddDefault();
		editSubmesh.Positions = submesh.Positions;
		editSubmesh.Normals = submesh.Normals;
		editSubmesh.Tangents = submesh.Tangents;
		editSubmesh.Colors = submesh.Colors;
		for (DStaticMeshUVLayer& uvLayer : submesh.UVLayers)
		{
			DEditMeshUVLayer& editUVLayer = editSubmesh.UVLayers.AddDefault();
			editUVLayer.UVs = uvLayer.UVs;
		}
		DArray<DEditMeshTriangle>& editTris = editSubmesh.Tris;
		editTris.Reserve(submesh.Tris.Length());
		for (DStaticMeshTriangle& tri : submesh.Tris)
		{
			DEditMeshTriangle& editTri = editTris.AddDefault();
			editTri.VertIndex0 = tri.VertIndex0;
			editTri.VertIndex1 = tri.VertIndex1;
			editTri.VertIndex2 = tri.VertIndex2;
		}
	}
	return editMesh;
}


DString DEditMesh::MakeStatisticsString()
{
	DString str;
	str.Append(TX("\nSubmeshes:"));
	for (DEditSubMesh submesh : this->SubMeshes)
	{
		str.Append(TX("\nSubmesh:"));
		str.Append(TX("\nPositions:"));
		str.Append(DNumberFormat::DEFAULT.ToString(submesh.Positions.Length()));
		str.Append(TX("\nTris:"));
		str.Append(DNumberFormat::DEFAULT.ToString(submesh.Tris.Length()));
		str.Append(TX("\nNormals:"));
		str.Append(DNumberFormat::DEFAULT.ToString(submesh.Normals.Length()));
		str.Append(TX("\nTangents:"));
		str.Append(DNumberFormat::DEFAULT.ToString(submesh.Tangents.Length()));
		str.Append(TX("\nColors:"));
		str.Append(DNumberFormat::DEFAULT.ToString(submesh.Colors.Length()));
		str.Append(TX("\nUVLayers:"));
		for (DEditMeshUVLayer& uvLayer : submesh.UVLayers)
		{
			str.Append(TX("\nUVLayer:"));
			str.Append(DNumberFormat::DEFAULT.ToString(uvLayer.UVs.Length()));
		}
	}
	return str;
}
