#pragma once

#include "DWavefrontException.h"
#include "Engine\Render\Mesh\DStaticMeshData.h"
#include "Engine\Render\Mesh\HStaticMesh.h"
#include "Engine\Render\Scene\HSceneNode.h"
#include "Engine\Render\Scene\HStaticMeshNode.h"
#include "Engine\Render\Shader\HSurfaceShader.h"


class DWavefrontParser
{
private:
	
	struct _MaterialGroup
	{
		DString8 MaterialName;
		DArray<DFixedArray<int32, 9>> FaceIndices;
	};

public:

	struct DImportOptions
	{
		EAxis Right = EAxis::X;
		EAxis Up = EAxis::Y;
		EAxis Back = EAxis::Z;
		float ScaleToMeter = 1.0f;

		bool Equals(const DImportOptions& Other) const;
	};

private:

	enum class EAttributeLayout : uint8
	{
		UNDETECTED,
		VTN, 
		VN, 
		VT,  // Don't care about this
		V	 // Don't care about this
	};

	const DString8* Str = nullptr;
	bool FoundFirstObject = false;

	static int32 FindEndOfNumberString(const DString8& Str, int32 Start);
	// returns progress, skips whitespace
	int32 ParseNextFloat(int32 Start, float& OutValue);
	// OutValue = parsed int - 1 (wavefront starts indexing at 1)
	// returns progress, skips whitespace
	int32 ParseNextIndex(int32 Start, int32& OutValue);
	// returns progress, skips whitespace
	int32 ParseNextName(int32 Start, DString8& OutValue);

	static int32 IndexOfVert(const DStaticSubMeshData& Submesh, const FVec3& Position, const FVec3& Normal, const FVec2& UV);
	static int32 IndexOfVert(const DStaticSubMeshData& Submesh, const FVec3& Position, const FVec3& Normal);

	/* throws WavefrontException */
	void _Parse(const DString8& InStr, DArray<DStaticMeshData>& OutMeshes);

	void PopulateMeshData(
		DStaticMeshData& OutMeshData, 
		const DArray<FVec3>& Vertices, const DArray<FVec2>& UVs, const DArray<FVec3>& Normals,
		const DArray<_MaterialGroup>& Groups, EAttributeLayout AttributeLayout
	);

public:
	
	bool ParseMeshData(
		const DString8& InStr, 
		const DImportOptions& Options, 
		DArray<DStaticMeshData>& OutMeshes
	);

	bool ParseScene(
		const DString8& InStr, 
		const DImportOptions& Options, 
		shared<HSceneNode> ParentNode, 
		shared<HSurfaceShader> DefaultShader
	);

};