#pragma once

#include "HSurfaceShader.h"
#include "HSurfaceMaterial.h"
#include "..\Scene\HStaticMeshNode.h"
#include "..\Texture\HFrameBuffer.h"
#include "..\Texture\HTexture2D.h"
#include "..\Texture\HRenderTargetCube.h"


class HEqMapConvert : public HObject
{
private:
	bool Loaded = false;
	shared<HSurfaceShader> EqShader;
	shared<HSurfaceMaterial> EqMaterial;
	shared<HStaticMesh> UnitCubeMesh;
	shared<HStaticMeshNode> UnitCube;
	shared<HFrameBuffer> CubeTarget;
	
public:
	void Load();
	void EquirectangularToCubemap(
		const DArray<shared<HTexture2D>>& EqMaps,
		int32 Dimension,
		ERenderTargetCubeFormat Format,
		DArray<shared<HRenderTargetCube>>& OutCubemaps,
		float UOffset = .0f,
		bool VFlip = false
	);

};